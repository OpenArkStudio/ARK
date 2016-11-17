#ifndef VECTOR_3_HPP
#define VECTOR_3_HPP

/**
 * @struct  Point3D
 *
 * @brief   3D point structure.
 *
 * @author  Nick Yang
 * @date    2016/11/16
 */

struct Point3D
{
    /** @brief   x coordinate. */
    float x;
    /** @brief   y coordinate. */
    float y;
    /** @brief   z coordinate. */
    float z;

    /**
     * @fn  Point3D()
     *
     * @brief   Default constructor.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     */

    Point3D()
        : x(.0f)
        , y(.0f)
        , z(.0f)
    {
    }

    /**
     * @fn  Point3D(float f3[])
     *
     * @brief   Constructor.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   f3  The float array.
     */

    Point3D(float f3[])
    {
        x = f3[0];
        y = f3[1];
        z = f3[2];
    }

    /**
     * @fn  Point3D(float a, float b, float c)
     *
     * @brief   Constructor.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   a   The float to process.
     * @param   b   The float to process.
     * @param   c   The float to process.
     */

    Point3D(float a, float b, float c)
        : x(a),
          y(b),
          z(c)
    {
    }

    /**
     * @fn  Point3D(const Point3D& rht)
     *
     * @brief   Constructor.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   rht The rht.
     */

    Point3D(const Point3D& rht)
    {
        x = rht.x;
        y = rht.y;
        z = rht.z;
    }

    /**
     * @fn  const Point3D& operator=(const Point3D& rht)
     *
     * @brief   Assignment operator.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   rht The rht.
     *
     * @return  A shallow copy of this object.
     */

    const Point3D& operator=(const Point3D& rht)
    {
        x = rht.x;
        y = rht.y;
        z = rht.z;
        return *this;
    }

    /**
     * @fn  inline bool operator==(const Point3D& rht) const
     *
     * @brief   Equality operator.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   rht The rht.
     *
     * @return  True if the parameters are considered equivalent.
     */

    inline bool operator==(const Point3D& rht) const
    {
        return IsZeroFloat(Distance(*this, rht));
    }

    /**
     * @fn  inline bool operator!=(const Point3D& rht) const
     *
     * @brief   Inequality operator.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   rht The rht.
     *
     * @return  True if the parameters are not considered equivalent.
     */

    inline bool operator!=(const Point3D& rht) const
    {
        return !(*this == rht);
    }

    /**
     * @fn  float GetLength() const
     *
     * @brief   Gets the length.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @return  The length.
     */

    float GetLength() const
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    /**
     * @fn  bool IsZero() const
     *
     * @brief   Query if this object is zero.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @return  True if zero, false if not.
     */

    bool IsZero() const
    {
        return (IsZeroFloat(x) && IsZeroFloat(y) && IsZeroFloat(z));
    }

    /**
     * @fn  std::string ToString() const
     *
     * @brief   Convert this object into a string representation.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @return  A std::string that represents this object.
     */

    std::string ToString() const
    {
        std::stringstream ss;
        ss << x << "," << y << "," << z;
        return ss.str();
    }

    /**
     * @fn  bool FromString(const std::string& strValue)
     *
     * @brief   Initializes this object from the given from string.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   strValue    The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

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

    /**
     * @fn  static float Distance(const Point3D& a, const Point3D& b)
     *
     * @brief   Distances.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   a   The Point3D to process.
     * @param   b   The Point3D to process.
     *
     * @return  A float.
     */

    static float Distance(const Point3D& a, const Point3D& b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
};

#endif //!VECTOR_3_HPP