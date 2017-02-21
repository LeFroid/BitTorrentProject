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

#include "LogHelper.h"
#include "SHA1Hash.h"

SHA1Hash::SHA1Hash() :
    m_ctx(),
    m_digest(),
    m_finalized(false)
{
    initialize();
}

SHA1Hash::~SHA1Hash()
{
    finalize();
}

void SHA1Hash::initialize()
{
    m_finalized = false;

    if (SHA1_Init(&m_ctx) != 1)
        LOG_ERROR("torrent_protocol", "Unable to initialize SHA1 context!");
}

void SHA1Hash::update(const uint8_t *data, size_t length)
{
    if (SHA1_Update(&m_ctx, data, length) != 1)
        LOG_ERROR("torrent_protocol", "SHA1Hash::update - Unable to update message context!");
}

void SHA1Hash::finalize()
{
    if (SHA1_Final(&m_digest[0], &m_ctx) != 1)
        LOG_ERROR("torrent_protocol", "SHA1Hash::finalize - Unable to finalize message digest!");
    else
        m_finalized = true;
}

uint8_t *SHA1Hash::getDigest()
{
    if (!m_finalized)
        return nullptr;

    return &m_digest[0];
}
