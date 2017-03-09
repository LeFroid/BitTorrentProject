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
     * @class ClientBase
     * @brief Base class for async. client implementations
     */
    class ClientBase : public Socket
    {
    public:
        /// Constructs a Client object, given a reference to an io_service
        explicit ClientBase(boost::asio::io_service &ioService);

        /// Attempts to connect to the given tcp endpoint
        void connect(boost::asio::ip::tcp::endpoint &endpoint);

        /// Attempts to connect to the given udp endpoint
        //void connect(boost::asio::ip::udp::endpoint &endpoint);

        /// Returns true if the client has formed an active connection, false if else
        bool isConnected() const;

    public:
        /// Called after a successful read operation
        virtual void onRead() { }

    protected:
        /// Callback for client connect event
        void handleConnect(const boost::system::error_code& ec);

    protected:
        /// True if client has formed a connection, false if else
        std::atomic_bool m_isConnected;
    };
}
