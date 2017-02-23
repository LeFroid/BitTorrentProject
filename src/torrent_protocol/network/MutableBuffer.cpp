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

#include <cstring>
#include <utility>

#include "MutableBuffer.h"

namespace network
{
    const MutableBuffer::size_type MutableBuffer::DefaultBufferLen;

    MutableBuffer::MutableBuffer(size_type capacity) :
        m_data(),
        m_readPos(0),
        m_writePos(0)
    {
        m_data.reserve(capacity);
    }

    MutableBuffer::MutableBuffer(const MutableBuffer &other) :
        m_data(other.m_data),
        m_readPos(other.m_readPos),
        m_writePos(other.m_writePos)
    {
    }

    MutableBuffer::MutableBuffer(MutableBuffer &&other) :
        m_data(),
        m_readPos(0),
        m_writePos(0)
    {
        m_data = std::move(other.m_data);
        m_readPos = other.m_readPos;
        m_writePos = other.m_writePos;

        other.m_readPos = 0;
        other.m_writePos = 0;
    }

    MutableBuffer& MutableBuffer::operator=(const MutableBuffer &other)
    {
        if (this != &other)
        {
            m_readPos = other.m_readPos;
            m_writePos = other.m_writePos;

            m_data.assign(other.m_data.begin(), other.m_data.end());
        }
        return *this;
    }

    MutableBuffer& MutableBuffer::operator=(MutableBuffer &&other)
    {
        if (this != &other)
        {
            m_readPos = other.m_readPos;
            m_writePos = other.m_writePos;

            m_data = std::move(other.m_data);

            other.m_readPos = 0;
            other.m_writePos = 0;
        }
        return *this;
    }

    bool MutableBuffer::empty() const
    {
        return m_data.empty();
    }

    const MutableBuffer::size_type MutableBuffer::getAvailableSpace() const
    {
        return m_data.capacity() - m_writePos;
    }

    const MutableBuffer::size_type MutableBuffer::getSizeUnread() const
    {
        return m_writePos - m_readPos;
    }

    const MutableBuffer::size_type &MutableBuffer::getReadPosition() const
    {
        return m_readPos;
    }

    const MutableBuffer::size_type &MutableBuffer::getWritePosition() const
    {
        return m_writePos;
    }

    char *MutableBuffer::getPointer()
    {
        return m_data.data();
    }

    const char *MutableBuffer::getConstPointer() const
    {
        return m_data.data();
    }

    char *MutableBuffer::getReadPointer()
    {
        return &m_data[m_readPos];
    }

    char *MutableBuffer::getWritePointer()
    {
        return &m_data[m_writePos];
    }

    void MutableBuffer::advanceReadPosition(size_type numBytes)
    {
        m_readPos += numBytes;
    }

    void MutableBuffer::advanceWritePosition(size_type numBytes)
    {
        m_writePos += numBytes;
    }

    void MutableBuffer::clear()
    {
        m_data.clear();
        m_readPos = 0;
        m_writePos = 0;
    }

    void MutableBuffer::discardInactiveData()
    {
        if (!m_readPos)
            return;

        // Move position of data that is in use
        if (m_readPos != m_writePos)
            std::memmove(&m_data[0], &m_data[m_readPos], getSizeUnread());

        // Adjust read & write positions
        m_writePos -= m_readPos;
        m_readPos = 0;
    }

    void MutableBuffer::write(const char *data, size_t length)
    {
        // Check if there's enough space to write data into buffer
        if (getAvailableSpace() < length)
            m_data.reserve(m_data.capacity() + length);

        // Copy data into buffer, advance position
        std::memcpy(&m_data[m_writePos], data, length);

        advanceWritePosition(length);
    }

    void MutableBuffer::write(const MutableBuffer &buffer)
    {
        if (const auto &len = buffer.getWritePosition())
            write(buffer.getConstPointer(), len);
    }

    MutableBuffer& MutableBuffer::operator<<(int8_t data)
    {
        write<int8_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(int16_t data)
    {
        write<int16_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(int32_t data)
    {
        write<int32_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(int64_t data)
    {
        write<int64_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(uint8_t data)
    {
        write<uint8_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(uint16_t data)
    {
        write<uint16_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(uint32_t data)
    {
        write<uint32_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(uint64_t data)
    {
        write<uint64_t>(data);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator<<(const std::string &data)
    {
        // append string, followed by null terminator
        write(data.c_str(), data.size());
        write((char)0);
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(int8_t &data)
    {
        data = read<int8_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(int16_t &data)
    {
        data = read<int16_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(int32_t &data)
    {
        data = read<int32_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(int64_t &data)
    {
        data = read<int64_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(uint8_t &data)
    {
        data = read<uint8_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(uint16_t &data)
    {
        data = read<uint16_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(uint32_t &data)
    {
        data = read<uint32_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(uint64_t &data)
    {
        data = read<uint64_t>();
        return *this;
    }

    MutableBuffer& MutableBuffer::operator>>(std::string &data)
    {
        // clear string if it held any contents
        data.clear();

        // read from buffer into string until a null terminator is found
        size_type i = m_readPos;
        while (m_data[i] != '\0')
            data.push_back(m_data[i++]);

        advanceReadPosition(data.size() + 1);
        return *this;
    }
}
