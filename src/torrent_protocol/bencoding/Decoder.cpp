/*
Copyright (c) 2017, Timothy Vaccarelli
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdlib>
#include "Decoder.h"

#include "BenDictionary.h"
#include "BenInt.h"
#include "BenList.h"
#include "BenString.h"

#include "LogHelper.h"

namespace bencoding
{
    std::shared_ptr<BenObjectBase> Decoder::decode(const std::string &encoded, bool recursive)
    {
        std::shared_ptr<BenObjectBase> retVal;

        if (encoded.size() < 2)
            return retVal;

        if (!recursive)
            m_index = 0;

        // Determine data type
        switch (encoded.at(m_index))
        {
            // BenInt
            case 'i':
            {
                ++m_index;
                retVal = std::make_shared<BenInt>(getInt(encoded));
                break;
            }
            // BenList
            case 'l':
            {
                ++m_index;
                retVal = std::make_shared<BenList>();
                auto listPtr = std::static_pointer_cast<BenList>(retVal);
                while (m_index < encoded.size() && encoded.at(m_index) != 'e')
                {
                    listPtr->push_back(decode(encoded, true));
                }
                ++m_index;
                break;
            }
            // BenDictionary
            case 'd':
            {
                ++m_index;
                retVal = std::make_shared<BenDictionary>();
                auto dictPtr = std::static_pointer_cast<BenDictionary>(retVal);
                while (m_index < encoded.size() && encoded.at(m_index) != 'e')
                {
                    dictPtr->insert(std::make_pair(getString(encoded), decode(encoded, true)));
                }
                ++m_index;
                break;
            }
            // Default case is string
            default:
            {
                retVal = std::make_shared<BenString>(getString(encoded));
                break;
            }
        }

        return retVal;
    }

    const std::size_t &Decoder::getIndex() const
    {
        return m_index;
    }
    
    int64_t Decoder::getInt(const std::string &encoded)
    {
        std::string tmp;
        while (m_index < encoded.size() && encoded.at(m_index) != 'e')
        {
            tmp.push_back(encoded.at(m_index++));
        }
        ++m_index;

        // Convert to long long
        return std::strtoll(tmp.c_str(), nullptr, 10);
    }
    
    std::string Decoder::getString(const std::string &encoded)
    {
        std::string tmp;
        
        // Get length of string
        while (m_index < encoded.size() && encoded.at(m_index) != ':')
        {
            tmp.push_back(encoded.at(m_index++));
        }
        ++m_index;

        uint64_t stringLen = std::strtoull(tmp.c_str(), nullptr, 10);

        // Ensure the string length is valid
        if ((uint64_t)m_index + stringLen > encoded.size())
            return std::string();

        tmp = encoded.substr(m_index, stringLen);
        m_index += stringLen;
        return tmp;
    }
}

