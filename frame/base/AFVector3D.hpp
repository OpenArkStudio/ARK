/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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

namespace ark
{

    class AFVector3D
    {
    public:
        float x{ 0.0f };
        float y{ 0.0f };
        float z{ 0.0f };

        AFVector3D()
        {
        }

        explicit AFVector3D(float f3[])
        {
            x = f3[0];
            y = f3[1];
            z = f3[2];
        }

        AFVector3D(float a, float b, float c)
            : x(a),
              y(b),
              z(c)
        {
        }

        AFVector3D(const AFVector3D& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }

        const AFVector3D& operator=(const AFVector3D& rhs)
        {
            if (this != &rhs)
            {
                x = rhs.x;
                y = rhs.y;
                z = rhs.z;
            }
            return *this;
        }

        inline bool operator==(const AFVector3D& rhs) const
        {
            return AFMisc::IsZeroFloat(Distance(*this, rhs));
        }

        inline bool operator!=(const AFVector3D& rhs) const
        {
            return !(*this == rhs);
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
            //(x,y,z)
            return ARK_FORMAT("({:.2f},{:.2f},{:.2f})", x, y, z);
        }

        bool FromString(const std::string& value)
        {
            //TODO:split by new format
            //x,y,z
            size_t len = value.length();
            size_t pos = value.find(',');

            if (pos == std::string::npos)
            {
                return false;
            }

            std::string str_x = value.substr(0, pos);

            const std::string new_value = value.substr(pos + 1, len - pos);
            pos = new_value.find(',');

            if (pos == std::string::npos)
            {
                return false;
            }

            std::string str_y = new_value.substr(0, pos);

            std::string str_z;

            if ((pos + 1) < new_value.length())
            {
                str_z = new_value.substr(pos + 1, new_value.length() - pos);
            }

            try
            {
                x = ARK_LEXICAL_CAST<float>(str_x);
                y = ARK_LEXICAL_CAST<float>(str_y);
                z = ARK_LEXICAL_CAST<float>(str_z);
                return true;
            }
            catch (std::system_error& ex)
            {
                CONSOLE_INFO_LOG << "Vector3 FromString failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
                ARK_ASSERT_NO_EFFECT(0);
                return false;
            }

            return true;
        }

        static float Distance(const AFVector3D& a, const AFVector3D& b)
        {
            const float dx = a.x - b.x;
            const float dy = a.y - b.y;
            const float dz = a.z - b.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }
    };
}