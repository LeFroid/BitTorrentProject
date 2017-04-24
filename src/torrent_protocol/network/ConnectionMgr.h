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

#include <algorithm>
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <vector>

#include "LogHelper.h"
#include "Socket.h"

#include "TorrentState.h"

namespace network
{
    /**
     * @class ConnectionMgr
     * @brief Responsible for active connections of any Socket-based class
     */
    template <class SocketType>
    class ConnectionMgr
    {
    public:
        /// Constructor - requires IO service reference
        ConnectionMgr(boost::asio::io_service &ioService) :
            m_ioService(ioService),
            m_connectionTimer(ioService),
            m_connections()
        {
        }

        /// Begins the timer-based routine of checking for stale connections
        void run()
        {
            m_connectionTimer.expires_from_now(boost::posix_time::seconds(5));
            m_connectionTimer.async_wait(std::bind(&ConnectionMgr<SocketType>::checkForStaleConns, this, std::placeholders::_1));
        }

    public:
        /// Adds a connection to the list of active connections
        void addConnection(std::shared_ptr<SocketType> connection)
        {
            std::lock_guard<std::mutex> lock(m_connectionLock);
            m_connections.push_back(connection);
        }

        /// Attempts to add a new SocketType (Peer) which will connect to a remote endpoint given the IP address and port
        /// Also passes the torrent state to the socket for peer handshake
        void attemptConnection(uint32_t ipAddress, uint16_t port, std::shared_ptr<TorrentState> &torrentState)
        {
            boost::asio::ip::address_v4 ip(ipAddress);
            boost::asio::ip::tcp::endpoint connEndpoint(ip, port);

            // When this method is specifically called, we should check if there is already
            // a connection with the same endpoint
            std::lock_guard<std::mutex> lock(m_connectionLock);
            for (auto conn : m_connections)
                if (conn->getTCPEndpoint() == connEndpoint)
                    return;

            // Made it this far, add new connection
            LOG_INFO("torrent_protocol.network", "Attempting connection with endpoint ", ip.to_string(), ":", port);
            std::shared_ptr<SocketType> conn = std::make_shared<SocketType>(m_ioService, Socket::Mode::TCP);
            conn->setTorrentState(torrentState);
            conn->connect(connEndpoint);
        }

        /// Attempts to create a new SocketType which will connect to the givne TCP endpoint
        void attemptConnection(boost::asio::ip::tcp::endpoint &endpoint)
        {
            std::shared_ptr<SocketType> conn = std::make_shared<SocketType>(m_ioService, Socket::Mode::TCP);
            conn->connect(endpoint);

            std::lock_guard<std::mutex> lock(m_connectionLock);
            m_connections.push_back(conn);
        }

        /// Returns the number of peers which the client is connected to
        size_t getNumConnected() const
        {
            std::lock_guard<std::mutex> lock(m_connectionLock);
            return m_connections.size();
        }

    private:
        /// Iterates through the container of pointers to Sockets, checking
        /// if any connections have gone stale, and if so, removing them.
        void checkForStaleConns(const boost::system::error_code &ec)
        {
            if (ec)
            {
                LOG_ERROR("torrent_protocol.network", "Error in ConnectionMgr::checkForStaleConns. Message: ", ec.message());
                return;
            }

            std::lock_guard<std::mutex> lock(m_connectionLock);
            m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [this](std::shared_ptr<SocketType> connection)
            {
                if (connection.get() && connection->isClosing())
                    return true;
                connection->sendPieceHave();
                return false;
            }), m_connections.end());

            // Reset timer
            m_connectionTimer.expires_from_now(boost::posix_time::seconds(5));
            m_connectionTimer.async_wait(std::bind(&ConnectionMgr<SocketType>::checkForStaleConns, this, std::placeholders::_1));
        }

    private:
        /// IO Service reference
        boost::asio::io_service &m_ioService;

        /// Timer used to check for stale connections
        boost::asio::deadline_timer m_connectionTimer;

        /// Container for active connections
        std::vector< std::shared_ptr<SocketType> > m_connections;

        /// Lock for access to connection container
        std::mutex m_connectionLock;
    };
}
