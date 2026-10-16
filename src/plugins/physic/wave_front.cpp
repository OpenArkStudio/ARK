//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2019 NVIDIA Corporation. All rights reserved.
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ark/physic/physic_define.hpp"
#include "ark/physic/wave_front.hpp"

namespace ark {

namespace wavefront {

/*******************************************************************/
/******************** InParser.h  ********************************/
/*******************************************************************/
class in_place_parser_interface
{
public:
    virtual int ParseLine(int lineno, int argc,
        const char** argv) = 0; // return TRUE to continue parsing, return FALSE to abort parsing process

    virtual ~in_place_parser_interface() {}
};

enum separator_type
{
    ST_DATA, // is data
    ST_HARD, // is a hard separator
    ST_SOFT, // is a soft separator
    ST_EOS   // is a comment symbol, and everything past this character should be ignored
};

class in_place_parser
{
public:
    in_place_parser(void) {
        init();
    }

    in_place_parser(char* data, int len) {
        init();
        SetSourceData(data, len);
    }

    in_place_parser(const char* fname) {
        init();
        SetFile(fname);
    }

    ~in_place_parser(void);

    void init(void) {
        mQuoteChar = 34;
        mData = 0;
        mLen = 0;
        mMyAlloc = false;
        for (int i = 0; i < 256; i++)
        {
            mHard[i] = ST_DATA;
            mHardString[i * 2] = i;
            mHardString[i * 2 + 1] = 0;
        }
        mHard[0] = ST_EOS;
        mHard[32] = ST_SOFT;
        mHard[9] = ST_SOFT;
        mHard[13] = ST_SOFT;
        mHard[10] = ST_SOFT;
    }

    void SetFile(const char* fname); // use this file as source data to parse.

    physx::PxI32 OpenFileSafe(FILE** file, const char* name, const char* mode);

    void SetSourceData(char* data, int len) {
        mData = data;
        mLen = len;
        mMyAlloc = false;
    }

    int Parse(in_place_parser_interface*
            callback); // returns true if entire file was parsed, false if it aborted for some reason

    int ProcessLine(int lineno, char* line, in_place_parser_interface* callback);

    const char** GetArglist(
        char* source, int& count); // convert source string into an arg list, this is a destructive parse.

    void SetHardSeparator(char c) // add a hard separator
    {
        mHard[(unsigned int)c] = ST_HARD;
    }

    void SetHard(char c) // add a hard separator
    {
        mHard[(unsigned int)c] = ST_HARD;
    }

    void SetCommentSymbol(char c) // comment character, treated as 'end of string'
    {
        mHard[(unsigned int)c] = ST_EOS;
    }

    void ClearHardSeparator(char c) {
        mHard[(unsigned int)c] = ST_DATA;
    }

    void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

    bool EOS(char c) {
        if (mHard[(unsigned int)c] == ST_EOS)
        {
            return true;
        }
        return false;
    }

    void SetQuoteChar(char c) {
        mQuoteChar = c;
    }

private:
    inline char* AddHard(int& argc, const char** argv, char* foo);
    inline bool IsHard(char c);
    inline char* SkipSpaces(char* foo);
    inline bool IsWhiteSpace(char c);
    inline bool IsNonSeparator(char c); // non seperator,neither hard nor soft

    bool mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
    char* mData;   // ascii data to parse.
    int mLen;      // length of data
    separator_type mHard[256];
    char mHardString[256 * 2];
    char mQuoteChar;
};

/*******************************************************************/
/******************** InParser.cpp  ********************************/
/*******************************************************************/
physx::PxI32 in_place_parser::OpenFileSafe(FILE** file, const char* name, const char* mode) {
    return physic::OpenFile(file, name, mode);
};

void in_place_parser::SetFile(const char* fname) {
    if (mMyAlloc)
    {
        free(mData);
    }

    mData = 0;
    mLen = 0;
    mMyAlloc = false;

    ::FILE* fph = NULL;
    OpenFileSafe(&fph, fname, "rb");
    if (fph)
    {
        fseek(fph, 0L, SEEK_END);
        mLen = (int)ftell(fph);
        fseek(fph, 0L, SEEK_SET);
        if (mLen)
        {
            mData = (char*)malloc(sizeof(char) * (mLen + 1));
            int ok = int(fread(mData, 1, mLen, fph));
            if (!ok)
            {
                free(mData);
                mData = 0;
            }
            else
            {
                mData[mLen] = 0; // zero byte terminate end of file marker.
                mMyAlloc = true;
            }
        }
        fclose(fph);
    }
}

in_place_parser::~in_place_parser(void) {
    if (mMyAlloc)
    {
        free(mData);
    }
}

#define MAXARGS 512

bool in_place_parser::IsHard(char c) {
    return mHard[(unsigned int)c] == ST_HARD;
}

char* in_place_parser::AddHard(int& argc, const char** argv, char* foo) {
    while (IsHard(*foo))
    {
        const char* hard = &mHardString[*foo * 2];
        if (argc < MAXARGS)
        {
            argv[argc++] = hard;
        }
        foo++;
    }
    return foo;
}

bool in_place_parser::IsWhiteSpace(char c) {
    return mHard[(unsigned int)c] == ST_SOFT;
}

char* in_place_parser::SkipSpaces(char* foo) {
    while (!EOS(*foo) && IsWhiteSpace(*foo))
    {
        foo++;
    }
    return foo;
}

bool in_place_parser::IsNonSeparator(char c) {
    if (!IsHard(c) && !IsWhiteSpace(c) && c != 0)
    {
        return true;
    }
    return false;
}

int in_place_parser::ProcessLine(int lineno, char* line, in_place_parser_interface* callback) {
    int ret = 0;

    const char* argv[MAXARGS];
    int argc = 0;

    char* foo = line;

    while (!EOS(*foo) && argc < MAXARGS)
    {

        foo = SkipSpaces(foo); // skip any leading spaces

        if (EOS(*foo))
        {
            break;
        }

        if (*foo == mQuoteChar) // if it is an open quote
        {
            foo++;
            if (argc < MAXARGS)
            {
                argv[argc++] = foo;
            }
            while (!EOS(*foo) && *foo != mQuoteChar)
            {
                foo++;
            }
            if (!EOS(*foo))
            {
                *foo = 0; // replace close quote with zero byte EOS
                foo++;
            }
        }
        else
        {

            foo = AddHard(argc, argv, foo); // add any hard separators, skip any spaces

            if (IsNonSeparator(*foo)) // add non-hard argument.
            {
                bool quote = false;
                if (*foo == mQuoteChar)
                {
                    foo++;
                    quote = true;
                }

                if (argc < MAXARGS)
                {
                    argv[argc++] = foo;
                }

                if (quote)
                {
                    while (*foo && *foo != mQuoteChar)
                    {
                        foo++;
                    }
                    if (*foo)
                    {
                        *foo = 32;
                    }
                }

                // continue..until we hit an eos ..
                while (!EOS(*foo)) // until we hit EOS
                {
                    if (IsWhiteSpace(*foo)) // if we hit a space, stomp a zero byte, and exit
                    {
                        *foo = 0;
                        foo++;
                        break;
                    }
                    else if (
                        IsHard(
                            *foo)) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
                    {
                        const char* hard = &mHardString[*foo * 2];
                        *foo = 0;
                        if (argc < MAXARGS)
                        {
                            argv[argc++] = hard;
                        }
                        foo++;
                        break;
                    }
                    foo++;
                } // end of while loop...
            }
        }
    }

    if (argc)
    {
        ret = callback->ParseLine(lineno, argc, argv);
    }

    return ret;
}

int in_place_parser::Parse(
    in_place_parser_interface* callback) // returns true if entire file was parsed, false if it aborted for some reason
{
    assert(callback);
    if (!mData)
    {
        return 0;
    }

    int ret = 0;

    int lineno = 0;

    char* foo = mData;
    char* begin = foo;

    while (*foo)
    {
        if (*foo == 10 || *foo == 13)
        {
            lineno++;
            *foo = 0;

            if (*begin) // if there is any data to parse at all...
            {
                int v = ProcessLine(lineno, begin, callback);
                if (v)
                {
                    ret = v;
                }
            }

            foo++;
            if (*foo == 10)
            {
                foo++; // skip line feed, if it is in the carraige-return line-feed format...
            }
            begin = foo;
        }
        else
        {
            foo++;
        }
    }

    lineno++; // last line.

    int v = ProcessLine(lineno, begin, callback);
    if (v)
    {
        ret = v;
    }
    return ret;
}

void in_place_parser::DefaultSymbols(void) {
    SetHardSeparator(',');
    SetHardSeparator('(');
    SetHardSeparator(')');
    SetHardSeparator('=');
    SetHardSeparator('[');
    SetHardSeparator(']');
    SetHardSeparator('{');
    SetHardSeparator('}');
    SetCommentSymbol('#');
}

const char** in_place_parser::GetArglist(
    char* line, int& count) // convert source string into an arg list, this is a destructive parse.
{
    const char** ret = 0;

    static const char* argv[MAXARGS];
    int argc = 0;

    char* foo = line;

    while (!EOS(*foo) && argc < MAXARGS)
    {

        foo = SkipSpaces(foo); // skip any leading spaces

        if (EOS(*foo))
        {
            break;
        }

        if (*foo == mQuoteChar) // if it is an open quote
        {
            foo++;
            if (argc < MAXARGS)
            {
                argv[argc++] = foo;
            }
            while (!EOS(*foo) && *foo != mQuoteChar)
            {
                foo++;
            }
            if (!EOS(*foo))
            {
                *foo = 0; // replace close quote with zero byte EOS
                foo++;
            }
        }
        else
        {

            foo = AddHard(argc, argv, foo); // add any hard separators, skip any spaces

            if (IsNonSeparator(*foo)) // add non-hard argument.
            {
                bool quote = false;
                if (*foo == mQuoteChar)
                {
                    foo++;
                    quote = true;
                }

                if (argc < MAXARGS)
                {
                    argv[argc++] = foo;
                }

                if (quote)
                {
                    while (*foo && *foo != mQuoteChar)
                    {
                        foo++;
                    }
                    if (*foo)
                    {
                        *foo = 32;
                    }
                }

                // continue..until we hit an eos ..
                while (!EOS(*foo)) // until we hit EOS
                {
                    if (IsWhiteSpace(*foo)) // if we hit a space, stomp a zero byte, and exit
                    {
                        *foo = 0;
                        foo++;
                        break;
                    }
                    else if (
                        IsHard(
                            *foo)) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
                    {
                        const char* hard = &mHardString[*foo * 2];
                        *foo = 0;
                        if (argc < MAXARGS)
                        {
                            argv[argc++] = hard;
                        }
                        foo++;
                        break;
                    }
                    foo++;
                } // end of while loop...
            }
        }
    }

    count = argc;
    if (argc)
    {
        ret = argv;
    }

    return ret;
}

/*******************************************************************/
/******************** Geometry.h  ********************************/
/*******************************************************************/

class geometry_vertex
{
public:
    float mPos[3];
    float mNormal[3];
    float mTexel[2];
};

class geometry_interface
{
public:
    virtual void NodeTriangle(
        const geometry_vertex* v1, const geometry_vertex* v2, const geometry_vertex* v3, bool textured) {}

    virtual ~geometry_interface() {}
};

/*******************************************************************/
/******************** Obj.h  ********************************/
/*******************************************************************/

class obj : public in_place_parser_interface
{
public:
    int LoadMesh(const char* fname, geometry_interface* callback, bool textured);
    int ParseLine(int lineno, int argc,
        const char** argv); // return TRUE to continue parsing, return FALSE to abort parsing process

    obj()
        : mVerts(NULL)
        , mTexels(NULL)
        , mNormals(NULL)
        , mTextured(false)
        , mCallback(NULL) {
        Clear();
    }

    ~obj() {
        Clear();
    }

private:
    void Clear();
    void GetVertex(geometry_vertex& v, const char* face) const;

    float* mVerts;
    unsigned mNumVerts; // this is the tripled number of verts
    unsigned mMaxVerts;

    float* mTexels; // doubled number of texcoords
    unsigned mNumTexels;
    unsigned mMaxTexels;

    float* mNormals; // tripled number of normals
    unsigned mNumNormals;
    unsigned mMaxNormals;

    bool mTextured;
    geometry_interface* mCallback;
};

/*******************************************************************/
/******************** Obj.cpp  ********************************/
/*******************************************************************/

int obj::LoadMesh(const char* fname, geometry_interface* iface, bool textured) {
    Clear();

    mTextured = textured;
    int ret = 0;
    mCallback = iface;

    in_place_parser ipp(fname);

    ipp.Parse(this);

    return ret;
}

void obj::Clear() {
    if (mVerts)
    {
        delete[] mVerts;
    }
    mMaxVerts = 0;
    mNumVerts = 0;

    if (mTexels)
    {
        delete[] mTexels;
    }
    mMaxTexels = 0;
    mNumTexels = 0;

    if (mNormals)
    {
        delete[] mNormals;
    }
    mMaxNormals = 0;
    mNumNormals = 0;
}

void obj::GetVertex(geometry_vertex& v, const char* face) const {
    v.mPos[0] = 0;
    v.mPos[1] = 0;
    v.mPos[2] = 0;

    v.mTexel[0] = 0;
    v.mTexel[1] = 0;

    v.mNormal[0] = 0;
    v.mNormal[1] = 1;
    v.mNormal[2] = 0;

    int index = atoi(face) - 1;

    const char* texel = strstr(face, "/");

    if (texel)
    {
        int tindex = atoi(texel + 1) - 1;

        if (tindex >= 0 && tindex < (int)(mNumTexels / 2))
        {
            const float* t = &mTexels[tindex * 2];

            v.mTexel[0] = t[0];
            v.mTexel[1] = t[1];
        }

        const char* normal = strstr(texel + 1, "/");
        if (normal)
        {
            int nindex = atoi(normal + 1) - 1;

            if (nindex >= 0 && nindex < (int)(mNumNormals / 3))
            {
                const float* n = &mNormals[nindex * 3];

                v.mNormal[0] = n[0];
                v.mNormal[1] = n[1];
                v.mNormal[2] = n[2];
            }
        }
    }

    if (index >= 0 && index < (int)(mNumVerts / 3))
    {

        const float* p = &mVerts[index * 3];

        v.mPos[0] = p[0];
        v.mPos[1] = p[1];
        v.mPos[2] = p[2];
    }
    else
    {
        assert(0); // "Negative face vertex indices are not supported in wavefront loader.");
    }
}

template<typename T>
void Resize(T*& data, unsigned& capacity, unsigned count, unsigned new_count) {
    if (new_count >= capacity)
    {
        capacity = new_count * 2;
        T* tmp = new T[capacity];
        memcpy(tmp, data, count * sizeof(T));
        delete[] data;
        data = tmp;
    }
}

int obj::ParseLine(
    int lineno, int argc, const char** argv) // return TRUE to continue parsing, return FALSE to abort parsing process
{
    int ret = 0;

    if (argc >= 1)
    {
        const char* foo = argv[0];
        if (*foo != '#')
        {
            if (strcmp(argv[0], "v") == 0 && argc == 4)
            {
                float vx = (float)atof(argv[1]);
                float vy = (float)atof(argv[2]);
                float vz = (float)atof(argv[3]);
                Resize(mVerts, mMaxVerts, mNumVerts, mNumVerts + 3);
                mVerts[mNumVerts++] = vx;
                mVerts[mNumVerts++] = vy;
                mVerts[mNumVerts++] = vz;
            }
            else if (strcmp(argv[0], "vt") == 0 && (argc == 3 || argc == 4))
            {
                // ignore 4rd component if present
                float tx = (float)atof(argv[1]);
                float ty = (float)atof(argv[2]);
                Resize(mTexels, mMaxTexels, mNumTexels, mNumTexels + 2);
                mTexels[mNumTexels++] = tx;
                mTexels[mNumTexels++] = ty;
            }
            else if (strcmp(argv[0], "vn") == 0 && argc == 4)
            {
                float normalx = (float)atof(argv[1]);
                float normaly = (float)atof(argv[2]);
                float normalz = (float)atof(argv[3]);
                Resize(mNormals, mMaxNormals, mNumNormals, mNumNormals + 3);
                mNormals[mNumNormals++] = normalx;
                mNormals[mNumNormals++] = normaly;
                mNormals[mNumNormals++] = normalz;
            }
            else if (strcmp(argv[0], "f") == 0 && argc >= 4)
            {
                geometry_vertex v[32];

                int vcount = argc - 1;

                for (int i = 1; i < argc; i++)
                {
                    GetVertex(v[i - 1], argv[i]);
                }

                // need to generate a normal!
#if 0 // not currently implemented
					if ( mNormals.empty() )
					{
						Vector3d<float> p1( v[0].mPos );
						Vector3d<float> p2( v[1].mPos );
						Vector3d<float> p3( v[2].mPos );

						Vector3d<float> n;
						n.ComputeNormal(p3,p2,p1);

						for (int i=0; i<vcount; i++)
						{
							v[i].mNormal[0] = n.x;
							v[i].mNormal[1] = n.y;
							v[i].mNormal[2] = n.z;
						}

					}
#endif

                mCallback->NodeTriangle(&v[0], &v[1], &v[2], mTextured);

                if (vcount >= 3) // do the fan
                {
                    for (int i = 2; i < (vcount - 1); i++)
                    {
                        mCallback->NodeTriangle(&v[0], &v[i], &v[i + 1], mTextured);
                    }
                }
            }
        }
    }

    return ret;
}

class build_mesh : public geometry_interface
{
public:
    build_mesh()
        : mVertices(NULL)
        , mTexCoords(NULL)
        , mIndices(NULL) {
        Clear();
    }

    ~build_mesh() {
        Clear();
    }

    void Clear() {
        if (mVertices)
        {
            delete[] mVertices;
        }
        mMaxVertices = 0;
        mNumVertices = 0;

        if (mTexCoords)
        {
            delete[] mTexCoords;
        }
        mMaxTexCoords = 0;
        mNumTexCoords = 0;

        if (mIndices)
        {
            delete[] mIndices;
        }
        mMaxIndices = 0;
        mNumIndices = 0;
    }

    int GetIndex(const float* p, const float* texCoord) {

        int vcount = mNumVertices / 3;

        if (vcount > 0)
        {
            //New MS STL library checks indices in debug build, so zero causes an assert if it is empty.
            const float* v = &mVertices[0];
            const float* t = texCoord != NULL ? &mTexCoords[0] : NULL;

            for (int i = 0; i < vcount; i++)
            {
                if (v[0] == p[0] && v[1] == p[1] && v[2] == p[2])
                {
                    if (texCoord == NULL || (t[0] == texCoord[0] && t[1] == texCoord[1]))
                    {
                        return i;
                    }
                }
                v += 3;
                if (t != NULL)
                {
                    t += 2;
                }
            }
        }

        Resize(mVertices, mMaxVertices, mNumVertices, mNumVertices + 3);
        mVertices[mNumVertices++] = p[0];
        mVertices[mNumVertices++] = p[1];
        mVertices[mNumVertices++] = p[2];

        if (texCoord != NULL)
        {
            Resize(mTexCoords, mMaxTexCoords, mNumTexCoords, mNumTexCoords + 2);
            mTexCoords[mNumTexCoords++] = texCoord[0];
            mTexCoords[mNumTexCoords++] = texCoord[1];
        }

        return vcount;
    }

    virtual void NodeTriangle(
        const geometry_vertex* v1, const geometry_vertex* v2, const geometry_vertex* v3, bool textured) {
        Resize(mIndices, mMaxIndices, mNumIndices, mNumIndices + 3);

        mIndices[mNumIndices++] = GetIndex(v1->mPos, textured ? v1->mTexel : NULL);
        mIndices[mNumIndices++] = GetIndex(v2->mPos, textured ? v2->mTexel : NULL);
        mIndices[mNumIndices++] = GetIndex(v3->mPos, textured ? v3->mTexel : NULL);
    }

    const float* GetVertices() const {
        return mVertices;
    }

    unsigned GetNumVertices() const {
        return mNumVertices;
    }

    const float* GetTexCoords(void) const {
        return mTexCoords;
    }

    unsigned GetNumTexCoords(void) const {
        return mNumTexCoords;
    }

    const int* GetIndices() const {
        return mIndices;
    }

    unsigned GetNumIndices() const {
        return mNumIndices;
    }

private:
    float* mVertices;
    unsigned mMaxVertices;
    unsigned mNumVertices;

    float* mTexCoords;
    unsigned mMaxTexCoords;
    unsigned mNumTexCoords;

    int* mIndices;
    unsigned mMaxIndices;
    unsigned mNumIndices;
};

}; // namespace wavefront

using namespace wavefront;

wave_front_obj::wave_front_obj(void) {
    mVertexCount = 0;
    mTriCount = 0;
    mIndices = 0;
    mVertices = NULL;
    mTexCoords = NULL;
}

wave_front_obj::~wave_front_obj(void) {
    delete[] mIndices;
    delete[] mVertices;
    if (mTexCoords)
    {
        delete[] mTexCoords;
    }
}

unsigned int wave_front_obj::LoadObj(const char* fname,
    bool
        textured) // load a wavefront obj returns number of triangles that were loaded.  Data is persists until the class is destructed.
{
    unsigned int ret = 0;
    delete[] mVertices;
    mVertices = 0;
    delete[] mIndices;
    mIndices = 0;
    mVertexCount = 0;
    mTriCount = 0;

    build_mesh bm;
    obj obj;
    obj.LoadMesh(fname, &bm, textured);

    if (bm.GetNumVertices())
    {
        mVertexCount = bm.GetNumVertices() / 3;
        mVertices = new float[mVertexCount * 3];
        memcpy(mVertices, bm.GetVertices(), sizeof(float) * mVertexCount * 3);

        if (textured)
        {
            mTexCoords = new float[mVertexCount * 2];
            memcpy(mTexCoords, bm.GetTexCoords(), sizeof(float) * mVertexCount * 2);
        }

        mTriCount = bm.GetNumIndices() / 3;
        mIndices = new int[mTriCount * 3];
        memcpy(mIndices, bm.GetIndices(), sizeof(int) * mTriCount * 3);
        ret = mTriCount;
    }

    return ret;
}
} // namespace ark
