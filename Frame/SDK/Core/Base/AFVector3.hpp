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

#include "AFMisc.hpp"

class Point3D
{
public:
    float x;
    float y;
    float z;

    Point3D()
        : x(.0f)
        , y(.0f)
        , z(.0f)
    {
    }

    explicit Point3D(float f3[])
    {
        x = f3[0];
        y = f3[1];
        z = f3[2];
    }

    Point3D(float a, float b, float c)
        : x(a),
          y(b),
          z(c)
    {
    }

    Point3D(const Point3D& rht)
    {
        x = rht.x;
        y = rht.y;
        z = rht.z;
    }

    const Point3D& operator=(const Point3D& rht)
    {
        x = rht.x;
        y = rht.y;
        z = rht.z;
        return *this;
    }

    inline bool operator==(const Point3D& rht) const
    {
        return AFMisc::IsZeroFloat(Distance(*this, rht));
    }

    inline bool operator!=(const Point3D& rht) const
    {
        return !(*this == rht);
    }

    float GetLength() const
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    bool IsZero() const
    {
        return (AFMisc::IsZeroFloat(x) && AFMisc::IsZeroFloat(y) && AFMisc::IsZeroFloat(z));
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << x << "," << y << "," << z;
        return ss.str();
    }

    bool FromString(const std::string& strValue)
    {
        //x,y,z
        size_t nStrLength = strValue.length();
        size_t nPos = strValue.find(',');
        if (nPos == std::string::npos)
        {
            return false;
        }

        std::string strX = strValue.substr(0, nPos);

        const std::string& strNewData = strValue.substr(nPos + 1, nStrLength - nPos);
        nPos = strNewData.find(',');
        if (nPos == std::string::npos)
        {
            return false;
        }

        std::string strY = strNewData.substr(0, nPos);

        std::string strZ = "";
        if ((nPos + 1) < strNewData.length())
        {
            strZ = strNewData.substr(nPos + 1, strNewData.length() - nPos);
        }

        try
        {
            x = ARK_LEXICAL_CAST<float>(strX);
            y = ARK_LEXICAL_CAST<float>(strY);
            z = ARK_LEXICAL_CAST<float>(strZ);
            return true;
        }
        catch (std::system_error& ex)
        {
            CONSOLE_LOG_NO_FILE << "Vector3 FromString failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
            ARK_ASSERT_NO_EFFECT(0);
            return false;
        }

        return true;
    }

    static float Distance(const Point3D& a, const Point3D& b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
};