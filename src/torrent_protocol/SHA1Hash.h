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
#include <openssl/sha.h>

/**
 * @class SHA1Hash
 * @brief Acts as a wrapper for openssl or libressl's SHA-1 hash functions
 */
class SHA1Hash
{
public:
    /// SHA1Hash constructor - initializes the internal message digest
    SHA1Hash();

    /// SHA1Hash destructor - frees the sha context
    ~SHA1Hash();

    /// Initializes / Re-initializes the SHA context
    void initialize();

    /// Updates the message with length bytes of data
    void update(const uint8_t *data, size_t length);

    /// Finalizes the message digest, must be called before accessing the digest
    void finalize();

    /// Returns a pointer to the digest upon success, or a null pointer
    /// if the message could not be hashed
    uint8_t *getDigest();

private:
    /// Structure that holds the context of the hash
    SHA_CTX m_ctx;

    /// Message digest
    uint8_t m_digest[SHA_DIGEST_LENGTH];

    /// True if digest has been finalized, false if else
    bool m_finalized;
};
