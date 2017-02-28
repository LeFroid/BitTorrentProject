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
    Socket::Socket(boost::asio::io_service &ioService) :
        m_socket(ioService),
        m_bufferRead(),
        m_queueSend(),
        m_lockSend(),
        m_isClosing(false)
    {
    }

    bool Socket::isClosing() const
    {
        return m_isClosing.load();
    }

    void Socket::close()
    {
        m_isClosing.store(true);
        m_socket.close();
    }

    void Socket::read()
    {
        if (isClosing())
            return;

        m_bufferRead.discardInactiveData();
        if (!m_bufferRead.getAvailableSpace())
            m_bufferRead.resize(m_bufferRead.getSize() * 2);

        m_socket.async_read_some(boost::asio::buffer(m_bufferRead.getWritePointer(), m_bufferRead.getSizeNotWritten()),
                                 std::bind(&Socket::handleRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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

    void Socket::sendNextItem()
    {
        if (isClosing() || m_queueSend.empty())
            return;

        auto &buffer = m_queueSend.front();
        m_socket.async_write_some(boost::asio::buffer(buffer.getReadPointer(), buffer.getSizeUnread()),
                                  std::bind(&Socket::handleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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
