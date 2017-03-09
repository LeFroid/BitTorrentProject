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

#include "ClientBase.h"
#include "LogHelper.h"

namespace network
{
    ClientBase::ClientBase(boost::asio::io_service &ioService) :
        Socket(ioService),
        m_isConnected(false)
    {
    }

    void ClientBase::connect(boost::asio::ip::tcp::endpoint &endpoint)
    {
        m_socket.async_connect(endpoint, std::bind(&ClientBase::handleConnect, this, std::placeholders::_1));
    }

    /*void ClientBase::connect(boost::asio::ip::udp::endpoint &endpoint)
    {
        setMode(UDP);
        m_udpSocket.async_connect(endpoint, std::bind(&ClientBase::handleConnect, this, std::placeholders::_1));
    }*/

    bool ClientBase::isConnected() const
    {
        return m_isConnected.load();
    }

    void ClientBase::handleConnect(const boost::system::error_code& ec)
    {
        if (ec)
        {
            LOG_ERROR("torrent_protocol.network", "Error with ClientBase::connect(...), error message: ", ec.message());
            close();
            return;
        }

        m_isConnected.store(true);
        //read();
    }
}
