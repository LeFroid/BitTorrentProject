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

#include "Socket.h"

namespace network
{
    /**
     * @class Peer
     * @brief Represents a single remote entity in the swarm, connected
     *        to the local client through a TCP socket
     */
    class Peer : public Socket
    {
    public:
        /// Constructs a new peer object, given a reference to an io_service
        explicit Peer(boost::asio::io_service &ioService, Socket::Mode mode);

        /// Constructs a peer by moving the given tcp socket
        Peer(boost::asio::ip::tcp::socket &&socket);

        //void requestPiece(uint64_t pieceNum);

    protected:
        /// Called after the local client has successfully initiated a connection with a remote peer
        virtual void onConnect() override;

        /// Called after a successful read operation
        virtual void onRead() override;

    private:
        /// Used to determine if handshake has been performed
        std::atomic_bool m_doneHandshake;
    };
}
