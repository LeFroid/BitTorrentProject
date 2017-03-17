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

#include <boost/endian/conversion.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace network
{
    /**
     * @class MutableBuffer
     * @brief Used to store data that is sent or received over the network
     */
    class MutableBuffer
    {
    public:
        typedef std::vector<char>::size_type size_type;

        /// Default buffer size
        static const size_type DefaultBufferLen = 4096;

    // Constructors, assignment operators
    public:
        /// Constructs a buffer with optionally a given capacity.
        MutableBuffer(size_type capacity = DefaultBufferLen);

        /// Copy constructor
        MutableBuffer(const MutableBuffer &other);

        /// Move constructor
        MutableBuffer(MutableBuffer &&other);

        /// Copy assignment operator
        MutableBuffer& operator=(const MutableBuffer &other);

        /// Move assignment operator
        MutableBuffer& operator=(MutableBuffer &&other);

    // Getters
    public:
        /// Returns true if the buffer is empty, false if else
        bool empty() const;

        /// Returns the number of bytes available to be written into the buffer
        const size_type getAvailableSpace() const;

        /// Returns the number of bytes that have not been marked as read from
        const size_type getSizeUnread();

        /// Returns the number of bytes that have not been writen from the buffer
        const size_type getSizeNotWritten() const;

        /// Returns the size of the container
        const size_type getSize() const;

        /// Returns the current position for reading from the buffer
        const size_type &getReadPosition() const;

        /// Returns the current position for writing into the buffer
        const size_type &getWritePosition() const;

        /// Returns a raw pointer to the beginning of the buffer
        char *getPointer();

        /// Returns a const pointer to the beginning of the buffer
        const char *getConstPointer() const;

        /// Returns a raw pointer to the buffer's contents at the present read position
        char *getReadPointer();

        /// Returns a raw pointer to the buffer's contents at the present write position
        char *getWritePointer();

    // Setters
    public:
        /// Advances the buffer's read position by the given number of bytes
        void advanceReadPosition(size_type numBytes);

        /// Advances the buffer's write position by the given number of bytes
        void advanceWritePosition(size_type numBytes);

        /// Removes the contents in the buffer, resetting the read & write positions as well
        void clear();

        /// Discards data that is no longer needed to free space in the buffer (should be called
        /// whenever data is being written into the buffer from the network)
        void discardInactiveData();

        /// Resizes the buffer to the given length
        void resize(size_type length);

    // Methods to write data into the buffer
    public:
        /// Writes length bytes of data into the buffer
        void write(const char *data, size_t length);

        /// Writes the contents of one buffer into the calling object
        void write(const MutableBuffer &buffer);

        /// Writes the data of type T into the buffer
        template <typename T>
        void write(T data)
        {
            // Convert to big endian if applicable
            boost::endian::native_to_big_inplace(data);
            // Append to buffer
            write((const char*)&data, sizeof(data));
        }

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(int8_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(int16_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(int32_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(int64_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(uint8_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(uint16_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(uint32_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(uint64_t data);

        /// Appends the given value to the end of the buffer
        MutableBuffer& operator<<(const std::string &data);

    // Methods to read data from the buffer
    public:
        /// Reads the data out of the buffer at the given position, returning it as the given type
        template <typename T>
        T read(size_type position)
        {
            // Make sure read is valid
            if (position + sizeof(T) > m_writePos)
                throw std::out_of_range("Attempt to read further than possible from MutableBuffer!");

            // Cast data into type T, convert from big endian to native if applicable
            T value = *((const T*)&m_data[position]);
            boost::endian::big_to_native_inplace(value);

            // Advance read position
            advanceReadPosition(sizeof(T));

            return value;
        }

        /// Reads the data out of the buffer at the current reading position, returning it as the given type
        template <typename T>
        T read()
        {
            return read<T>(m_readPos);
        }

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(int8_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(int16_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(int32_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(int64_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(uint8_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(uint16_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(uint32_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(uint64_t &data);

        /// Reads data out of the buffer into the given value
        MutableBuffer& operator>>(std::string &data);

    private:
        /// Container for buffer data
        std::vector<char> m_data;

        /// Current read position in the buffer
        size_type m_readPos;

        /// Current write position in the buffer
        size_type m_writePos;
    };
}
