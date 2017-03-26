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

#include <cstdint>
#include "Peer.h"

namespace network
{
    Peer::Peer(boost::asio::io_service &ioService) :
        Socket(ioService, Socket::Mode::TCP),
        m_doneHandshake(false)
    {
    }

    void Peer::onConnect()
    {
        // Send handshake, following the format "<pstrlen><pstr><reserved><info_hash><peer_id>"
        m_doneHandshake.store(true);
    }

    void Peer::onRead()
    {
        if (!m_doneHandshake.load())
        {
            // Read handshake, check info hash.
            // Drop connection if info hash not in TorrentMgr
            //m_doneHandshake.store(true);
            return;
        }

        // Data format: "<length prefix (32-bit)><message ID (8-bit)><payload>."
        uint32_t length;
        m_bufferRead >> length;

        // Before reading message ID, check if keep-alive was sent (len == 0)
        if (length == 0)
        {
            // Update time of last message received
            return;
        }

        uint8_t messageID;
        m_bufferRead >> messageID;

        // Handle message
        switch (messageID)
        {
            default:
                break;
        }
    }
}
