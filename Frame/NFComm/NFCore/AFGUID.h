#pragma once
#include "NFComm/NFPluginModule/NFPlatform.h"

struct AFGUID 
{
    union 
    {
        struct
        {
            uint64_t n64Value;
        };

        struct 
        {
            uint32_t nIdent;
            uint32_t nSerial;
        };
    };

    AFGUID()
    {
        n64Value = 0;
    }

    AFGUID(uint32_t ident, uint32_t serial)
    {
        nIdent = ident;
        nSerial = serial;
    }

    bool IsNull()
    {
        return (0 == n64Value);
    }

    bool operator==(const AFGUID& rhs) const
    {
        return (this->n64Value == rhs.n64Value);
    }

    bool operator!=(const AFGUID& rhs) const
    {
        return (this->n64Value != rhs.n64Value);
    }

    bool operator<(const AFGUID& rhs) const
    {
        if (this->nIdent == rhs.nIdent)
        {
            return this->nSerial < rhs.nSerial;
        }

        return this->nIdent < rhs.nIdent;
    }

    std::string ToString() const
    {
        return AF_LEXICAL_CAST<std::string>(nIdent) + "-" + AF_LEXICAL_CAST<std::string>(nSerial);
    }

    bool FromString(const std::string& strID)
    {
        size_t nStrLength = strID.length();
        size_t nPos = strID.find('-');
        if (nPos == std::string::npos)
        {
            return false;
        }

        std::string strIdent = strID.substr(0, nPos);
        std::string strSerial = "";
        if (nPos + 1 < nStrLength)
        {
            strSerial = strID.substr(nPos + 1, nStrLength - nPos);
        }

        try
        {
            nIdent = AF_LEXICAL_CAST<NFINT32>(strIdent);
            nSerial = AF_LEXICAL_CAST<NFINT32>(strSerial);

            return true;
        }
        catch (...)
        {
            return false;
        }

        return true;
    }
};

//inline bool operator==(const AFGUID& source, const AFGUID& other)
//{
//    return source.n64Value == other.n64Value;
//}
//
//inline bool operator!=(const AFGUID& source, const AFGUID& other)
//{
//    return source.n64Value != other.n64Value;
//}