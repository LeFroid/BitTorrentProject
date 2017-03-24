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

#include <cctype>
#include <iomanip>
#include <sstream>
#include "URL.h"

namespace http
{
    URL::URL(const std::string &url)
    {
        // Parse host string, found within substring after "http://" and either before the trailing "/" or at the end of the string
        auto addrPosBegin = url.find("://");
        if (addrPosBegin != std::string::npos
                && url.size() > addrPosBegin + 3)
        {
            auto addrPosEnd = url.find_first_of('/', addrPosBegin + 3);

            // If '/' was found in the original string, modify addrPosEnd to represent the length of the host substring
            if (addrPosEnd != std::string::npos)
            {
                // Also check for page name, potential request parameters, add requests to the hash map if any exist in url string
                if (url.size() > addrPosEnd + 2)
                {
                    auto paramPos = url.find_first_of('?');
                    if (paramPos != std::string::npos && paramPos > addrPosEnd)
                    {
                        m_pageName = url.substr(addrPosEnd + 1, paramPos - addrPosEnd - 1);
                        extractParameters(url.substr(paramPos + 1));
                    }
                    else
                        m_pageName = url.substr(addrPosEnd + 1);
                }

                addrPosEnd -= (addrPosBegin + 3);
            }

            // Set host member to the substring
            m_host = url.substr(addrPosBegin + 3, addrPosEnd);
        }
    }

    const std::string &URL::getHost() const
    {
        return m_host;
    }

    const std::string &URL::getPageName() const
    {
        return m_pageName;
    }

    std::string URL::getRequest() const
    {
        // Build request string
        std::ostringstream oss;

        oss << '/';
        if (!m_pageName.empty())
            oss << m_pageName;
        if (!m_parameters.empty())
            oss << '?';
        for (const auto &it : m_parameters)
        {
           oss << getEncodedFor(it.first)
               << '='
               << getEncodedFor(it.second)
               << '&';
        }

        std::string request = oss.str();
        if (request.at(request.size() - 1) == '&')
            return request.substr(0, request.size() - 1);
        return request;
    }

    void URL::setPageName(std::string name)
    {
        m_pageName = name;
    }

    std::string URL::getEncodedFor(const std::string &str) const
    {
        std::ostringstream oss;
        oss << std::hex;
        oss.fill('0');

        for (char c : str)
        {
            int cInt = int((unsigned char)c);
            // Check if character is unreserved, if so, add to string stream as-is
            if (isalnum(cInt)
                    || c == '-'
                    || c == '_'
                    || c == '.'
                    || c == '~')
            {
                oss << c;
                continue;
            }

            // Percent encode anything reserved
            oss << std::uppercase;
            oss << '%' << std::setw(2) << cInt;
            oss << std::nouppercase;
        }

        return oss.str();
    }

    void URL::extractParameters(std::string paramString)
    {
        // Split paramString by '&' for each parameter
        std::string::size_type last = 0;
        auto paramDelim = paramString.find('&');

        while (last != std::string::npos)
        {
            // Get substring, ex: "paramX=valueX", get key value pair and insert into hash map
            std::string requestPair = paramString.substr(last, paramDelim - last);
            auto keyValDelim = requestPair.find_first_of('=');
            if (keyValDelim != std::string::npos)
                m_parameters.insert(std::make_pair(requestPair.substr(0, keyValDelim), requestPair.substr(keyValDelim + 1)));

            // Check if more parameters to extract or if loop should stop
            if (paramDelim != std::string::npos)
            {
                last = paramDelim + 1;
                paramDelim = paramString.find('&', last);
            }
            else
                last = std::string::npos;
        }
    }
}
