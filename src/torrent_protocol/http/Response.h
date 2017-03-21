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

#include "LogHelper.h"

namespace http
{
    /**
     * @brief A simple HTTP response structure
     */
    struct Response
    {
        /// The HTTP version
        std::string Version;

        /// The status code
        int StatusCode;

        /// The reason phrase
        std::string Reason;

        /// The payload
        std::string Payload;

        /// Constructs a respose given the raw string sent by the server
        Response(const std::string &data)
        {
            std::size_t versionPos = data.find_first_of('/');
            std::size_t statusPos = data.find_first_of(' ', versionPos);
            // Extract version of protocol
            if (versionPos != std::string::npos && statusPos != std::string::npos)
            {
                Version = data.substr(versionPos + 1, statusPos - versionPos - 1);

                // Extract status code, followed by reason
                std::size_t reasonPos = data.find(' ', versionPos + 1);
                if (reasonPos != std::string::npos)
                {
                    try {
                        StatusCode = std::stoi(data.substr(statusPos + 1, reasonPos - statusPos - 1));
                    } catch (const std::invalid_argument &err) {
                         LOG_ERROR("torrent_protocol.http", "Error parsing status code from HTTP resonse, message: ", err.what());
                    }

                    reasonPos = data.find(' ', statusPos + 1);
                    Reason = data.substr(reasonPos + 1, data.find("\r\n", reasonPos + 1) - reasonPos - 1);
                }
            }

            // Get position of end of header, extract payload
            std::size_t headerEndPos = data.find("\r\n\r\n");
            if (headerEndPos != std::string::npos && headerEndPos + 4 < data.size())
            {
                Payload = data.substr(headerEndPos + 4);
            }
        }
    };
}
