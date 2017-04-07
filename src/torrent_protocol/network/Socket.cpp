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
#include "Socket.h"

namespace network
{
    Socket::Socket(boost::asio::io_service &ioService, Socket::Mode mode) :
        m_socket(ioService),
        m_udpSocket(ioService),
        m_mode(mode),
        m_bufferRead(),
        m_queueSend(),
        m_lockSend(),
        m_isClosing(false),
        m_isConnected(false)
    {
    }

    Socket::Socket(boost::asio::ip::tcp::socket &&socket) :
        m_socket(std::move(socket)),
        m_udpSocket(m_socket.get_io_service()),
        m_mode(Socket::Mode::TCP),
        m_bufferRead(),
        m_queueSend(),
        m_lockSend(),
        m_isClosing(false)
    {
        m_isConnected.store(m_socket.is_open());
    }

    Socket::Socket(boost::asio::ip::udp::socket &&socket) :
        m_socket(socket.get_io_service()),
        m_udpSocket(std::move(socket)),
        m_mode(Socket::Mode::UDP),
        m_bufferRead(),
        m_queueSend(),
        m_lockSend(),
        m_isClosing(false)
    {
        m_isConnected.store(m_udpSocket.is_open());
    }

    Socket::~Socket()
    {
        if (!isClosing())
            close();
    }

    void Socket::connect(boost::asio::ip::tcp::endpoint &endpoint)
    {
        m_mode = Mode::TCP;
        m_socket.async_connect(endpoint, std::bind(&Socket::handleConnect, shared_from_this(), std::placeholders::_1));
    }

    void Socket::connect(boost::asio::ip::udp::endpoint &endpoint)
    {
        m_mode = Mode::UDP;
        m_udpSocket.async_connect(endpoint, std::bind(&Socket::handleConnect, shared_from_this(), std::placeholders::_1));
    }

    bool Socket::isConnected() const
    {
        return m_isConnected;
    }

    bool Socket::isClosing() const
    {
        return m_isClosing;
    }

    void Socket::close()
    {
        m_isClosing.store(true);

        if (m_mode == Mode::TCP)
        {
            //m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            m_socket.close();
        }
        else
        {
            //m_udpSocket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
            m_udpSocket.close();
        }
    }

    void Socket::read()
    {
        if (isClosing())
            return;

        m_bufferRead.discardInactiveData();
        if (!m_bufferRead.getAvailableSpace())
            m_bufferRead.resize(m_bufferRead.getSize() * 2);

        if (m_mode == Mode::TCP)
        {
            m_socket.async_read_some(boost::asio::buffer(m_bufferRead.getWritePointer(), m_bufferRead.getSizeNotWritten()),
                                     std::bind(&Socket::handleRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            m_udpSocket.async_receive(boost::asio::buffer(m_bufferRead.getWritePointer(), m_bufferRead.getSizeNotWritten()),
                                      std::bind(&Socket::handleRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }

    void Socket::send(MutableBuffer &&buffer)
    {
        m_lockSend.lock();
        m_queueSend.push_back(std::move(buffer));
        m_lockSend.unlock();

        if (m_queueSend.size() > 1)
            return;

        sendNextItem();
    }

    boost::asio::ip::tcp::endpoint Socket::getTCPEndpoint() const
    {
        return m_socket.remote_endpoint();
    }

    boost::asio::ip::udp::endpoint Socket::getUDPEndpoint() const
    {
        return m_udpSocket.remote_endpoint();
    }

    const Socket::Mode &Socket::getMode() const
    {
        return m_mode;
    }

    void Socket::sendNextItem()
    {
        if (isClosing() || m_queueSend.empty())
            return;

        auto &buffer = m_queueSend.front();

        if (m_mode == Mode::TCP)
        {
            m_socket.async_write_some(boost::asio::buffer(buffer.getReadPointer(), buffer.getSizeUnread()),
                                      std::bind(&Socket::handleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            m_udpSocket.async_send(boost::asio::buffer(buffer.getReadPointer(), buffer.getSizeUnread()),
                                      std::bind(&Socket::handleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }

    void Socket::handleConnect(const boost::system::error_code& ec)
    {
        if (ec)
        {
            LOG_ERROR("torrent_protocol.network", "Error with Socket::connect(...), error message: ", ec.message());
            close();
            return;
        }

        m_isConnected.store(true);
        onConnect();
    }

    void Socket::handleRead(const boost::system::error_code& ec, std::size_t bytesTransferred)
    {
        if (isClosing())
            return;

        if (ec)
        {
            LOG_ERROR("torrent_protocol.network", "Error in Socket::handleRead, message: ", ec.message());
            close();
        }

        m_bufferRead.advanceWritePosition(bytesTransferred);
        onRead();
    }

    void Socket::handleWrite(const boost::system::error_code &ec, std::size_t /*bytesTransferred*/)
    {
        if (isClosing())
            return;

        if (ec)
        {
            LOG_ERROR("torrent_protocol.network", "Error in Socket::handleWrite, message: ", ec.message());
            close();
        }
        else
        {
            m_lockSend.lock();

            m_queueSend.pop_front();
            if (!m_queueSend.empty())
                sendNextItem();

            m_lockSend.unlock();
        }
    }
}
