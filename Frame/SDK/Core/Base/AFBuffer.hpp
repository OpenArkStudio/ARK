/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

class AFBuffer
{
public:
    ~AFBuffer()
    {
        if (mData != nullptr)
        {
            free(mData);
            mData = nullptr;
        }
    }

    explicit AFBuffer(size_t nBufferSize = 1024 * 512) : mData(nullptr), mnDataSize(0), mWritePos(0), mnReadPos(0)
    {
        if ((mData = (char*)malloc(sizeof(char) * nBufferSize)) != NULL)
        {
            mnDataSize = nBufferSize;
            mnReadPos = 0;
            mWritePos = 0;
        }
    }

    bool write(const char* data, size_t len)
    {
        bool write_ret = true;

        if (getwritevalidcount() >= len)
        {
            memcpy(getwriteptr(), data, len);
            addwritepos(len);
        }
        else
        {
            size_t left_len = mnDataSize - getlength();
            if (left_len >= len)
            {
                AdjusttoHead();
                write(data, len);
            }
            else
            {
                size_t needLen = len - left_len;
                if (needLen > 0)
                {
                    grow(needLen);
                    write(data, len);
                }
                else
                {
                    write_ret = false;
                }
            }
        }

        return write_ret;
    }

    size_t getlength()
    {
        return mWritePos - mnReadPos;
    }

    char* getdata()
    {
        if (mnReadPos < mnDataSize)
        {
            return mData + mnReadPos;
        }
        else
        {
            return nullptr;
        }
    }

    void removedata(size_t value)
    {
        size_t temp = mnReadPos + value;
        if (temp <= mnDataSize)
        {
            mnReadPos = temp;
        }
    }

private:
    void AdjusttoHead()
    {
        size_t len = 0;

        if (mnReadPos <= 0)
        {
            return;
        }

        len = getlength();
        if (len > 0)
        {
            memmove(mData, mData + mnReadPos, len);
        }

        mnReadPos = 0;
        mWritePos = len;
    }

    //Remove unused function
    //void init()
    //{
    //    mnReadPos = 0;
    //    mWritePos = 0;
    //}

    size_t getwritepos()
    {
        return mWritePos;
    }

    //Remove unused function
    //size_t getreadpos()
    //{
    //    return mnReadPos;
    //}

    void addwritepos(size_t value)
    {
        size_t temp = mWritePos + value;
        if (temp <= mnDataSize)
        {
            mWritePos = temp;
        }
    }

    size_t getwritevalidcount()
    {
        return mnDataSize - mWritePos;
    }

    //Remove unused function
    //size_t getsize()
    //{
    //    return mnDataSize;
    //}

    char* getwriteptr()
    {
        if (mWritePos < mnDataSize)
        {
            return mData + mWritePos;
        }
        else
        {
            return nullptr;
        }
    }

    void grow(size_t len)
    {
        size_t n = mnDataSize + len;
        char* d = new char[n];

        memcpy(d, mData, getwritepos());
        mnDataSize = n;
        delete[] mData;
        mData = d;
    }

    char*   mData;
    size_t mnDataSize;

    size_t mWritePos;
    size_t mnReadPos;
};