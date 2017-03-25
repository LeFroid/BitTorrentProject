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

#include <atomic>
#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include "MutableBuffer.h"

namespace network
{
    class Listener;

    /**
     * @class Socket
     * @brief Responsible for communications
     */
    class Socket : public std::enable_shared_from_this<Socket>
    {
        friend class Listener;

    public:
        /// The mode of the socket
        enum class Mode
        {
            TCP,
            UDP
        };

    public:
        /// Constructs a Socket, given a reference to an io_service and the mode of operation
        explicit Socket(boost::asio::io_service &ioService, Mode mode);

        /// Closes the socket if currently open
        ~Socket();

    public:
        /// Attempts to connect to the given tcp endpoint
        void connect(boost::asio::ip::tcp::endpoint &endpoint);

        /// Attempts to connect to the given udp endpoint
        void connect(boost::asio::ip::udp::endpoint &endpoint);

        /// Returns true if the client has formed an active connection, false if else
        bool isConnected() const;

        /// Returns true if the socket is closing or is closed, false if else
        bool isClosing() const;

        /// Closes the socket
        void close();

        /// Performs an asynchronous read operation
        void read();

        /// Moves the buffer into the queue to be sent out
        void send(MutableBuffer &&buffer);

        /// Returns the mode of operation
        const Mode &getMode() const;

    protected:
        /// Called during handleConnect(..) if connection has been made successfully
        virtual void onConnect() { }

        /// Called after a successful read operation
        virtual void onRead() = 0;

    private:
        /// Sends the item at the front of the send queue
        void sendNextItem();

        /// Internal callback for client connect event
        void handleConnect(const boost::system::error_code& ec);

        /// Internal read handler
        void handleRead(const boost::system::error_code& ec, std::size_t bytesTransferred);

        /// Internal write handler
        void handleWrite(const boost::system::error_code& ec, std::size_t bytesTransferred);

    protected:
        /// The TCP socket
        boost::asio::ip::tcp::socket m_socket;

        /// The UDP socket
        boost::asio::ip::udp::socket m_udpSocket;

        /// The mode of the socket
        Mode m_mode;

        /// Buffer used to store incoming data
        MutableBuffer m_bufferRead;

        /// Collection of buffers to be sent
        std::deque<MutableBuffer> m_queueSend;

        /// Mutex for operations on m_queueSend
        std::mutex m_lockSend;

        /// True if socket is being closed, false if else
        std::atomic_bool m_isClosing;

        /// True if client has formed a connection, false if else
        std::atomic_bool m_isConnected;
    };
}
