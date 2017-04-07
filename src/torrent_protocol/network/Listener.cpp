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

#include "Listener.h"
#include "LogHelper.h"

namespace network
{
    Listener::Listener(boost::asio::io_service &ioService) :
        m_ioService(ioService),
        m_acceptor(ioService),
        m_socket(ioService),
        m_connectionMgr(nullptr)
    {
    }

    void Listener::start(uint16_t port, int maxConnections, std::shared_ptr< ConnectionMgr<Peer> > connectionMgr)
    {
        m_connectionMgr = connectionMgr;

        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        m_acceptor.open(endpoint.protocol());
        m_acceptor.bind(endpoint);
        m_acceptor.listen(maxConnections);

        accept();
    }

    void Listener::accept()
    {
        m_acceptor.async_accept(m_socket, [this](const boost::system::error_code &ec)
        {
            if (!ec)
            {
                m_socket.non_blocking(true);
                auto conn = std::make_shared<Peer>(std::move(this->m_socket));

                // Call read as peer is expected to immediately send their handshake
                conn->read();

                // Add to connection mgr
                this->m_connectionMgr->addConnection(conn);

                LOG_INFO("torrent_protocol.network", "Accepted new peer");

                // Accept next peer
                accept();
            }
            else
                LOG_ERROR("torrent_protocol.network", "Error in Listener::accept handler - message: ", ec.message());
        });
    }
}
