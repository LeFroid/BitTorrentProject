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

#pragma once

#include <string>
#include <unordered_map>

#include "StringHelper.h"

namespace http
{
    /**
     * @class URL
     * @brief Used in order to format requests for a tracker
     */
    class URL
    {
    public:
        /// Constructs a URL given a string of proper format
        explicit URL(const std::string &url);

        /// Returns the host associated with the URL
        const std::string &getHost() const;

        /// Generates and returns a string of requests which can be send to the host through the HTTP protocol
        std::string getRequest();

        /// Associates the key parameter to the given value
        template <typename T>
        void setParameter(std::string key, T value)
        {
            m_parameters[key] = StringHelper::toString(value);
        }

    private:
        /// Converts the string, wherever applicable, into percent encoded URI format
        std::string getEncodedFor(const std::string &str);

        /// Extracts any request parameters found in the url given during object instantiation
        void extractParameters(std::string paramString);

    private:
        /// Host location. Ex: www.sitename.com
        std::string m_host;

        /// Map of request parameters, which can be formatted as "/?key1=value1&keyN=valueN" when sending a request
        std::unordered_map<std::string, std::string> m_parameters;
    };
}
