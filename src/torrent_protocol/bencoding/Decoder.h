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

#include <cstdint>
#include <memory>
#include <string>
#include "BenObject.h"

namespace bencoding
{
    class BenDictionary;
    class BenInt;
    class BenList;
    class BenString;
    /**
     * @class Decoder
     * @brief Parses a string in bencoding format into a BenObjectBase-derived type.
     */
    class Decoder
    {
    public:
        /// Default constructor
        Decoder() = default;

        /// Decodes the given string, returning a shared_ptr to the appropriate data type
        std::shared_ptr<BenObjectBase> decode(const std::string &encoded, bool recursive = false);

        /// Returns the final index value from the last decode operation (used for info hash) 
        const std::size_t &getIndex() const;

    private:
        /// Returns the integer associated with the bencoded string
        int64_t getInt(const std::string &encoded);

        /// Returns the bencoded string in string format
        std::string getString(const std::string &encoded);

    private:
        /// Current index in the string being decoded
        std::size_t m_index;
    };
}

