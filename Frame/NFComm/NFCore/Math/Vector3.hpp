#ifndef VECTOR_3_HPP
#define VECTOR_3_HPP

//3D point structure
struct Point3D
{
    float x;  //x坐标
    float y;  //y坐标
    float z;  //z坐标

    Point3D()
        : x(.0f)
        , y(.0f)
        , z(.0f)
    {
    }

    Point3D(float f3[])
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
        return IsZeroFloat(Distance(*this, rht));
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
        return (IsZeroFloat(x) && IsZeroFloat(y) && IsZeroFloat(z));
    }

    //x,y,z
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
        if(nPos == std::string::npos)
        {
            return false;
        }

        std::string strX = strValue.substr(0, nPos);

        //切过之后的
        const std::string& strNewData = strValue.substr(nPos + 1, nStrLength - nPos);
        nPos = strNewData.find(',');
        if(nPos == std::string::npos)
        {
            return false;
        }

        std::string strY = strNewData.substr(0, nPos);

        std::string strZ = "";
        if((nPos + 1) < strNewData.length())
        {
            strZ = strNewData.substr(nPos + 1, strNewData.length() - nPos);
        }

        try
        {
            x = lexical_cast<float>(strX);
            y = lexical_cast<float>(strY);
            z = lexical_cast<float>(strZ);
            return true;
        }
        catch(...)
        {
            return false;
        }
    }

    static float Distance(const Point3D& a, const Point3D& b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
};

#endif //!VECTOR_3_HPP