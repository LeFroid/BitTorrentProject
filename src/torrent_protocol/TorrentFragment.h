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
#include <cstring>
#include <memory>

/**
 * @brief Stores information about a fragment (also referred to as block)
 *        which is a part of a piece of a torrent file.
 */
struct TorrentFragment
{
    /// Index of the piece that the fragment belongs to
    uint32_t PieceIdx;

    /// Offset of the fragment within the piece
    uint32_t Offset;

    /// Length of the fragment
    uint32_t Length;

    /// Raw data associated with the fragment
    uint8_t *Data;

    std::mutex m_mutex;

    /// Method to copy the given buffer into the data member
    /**
     * @brief WriteData Copies data from the buffer into the Data buffer
     * @param buffer Source buffer
     * @param dataOffset Offset to begin writing into the TorrentFragment Data buffer
     * @param bufferLen Length of the source buffer to be copied
     */
    void WriteData(char *buffer, size_t dataOffset, size_t bufferLen)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        memcpy(&Data[dataOffset], buffer, bufferLen);
    }

    /// Default constructor
    TorrentFragment() : PieceIdx(0), Offset(0), Length(0), Data(nullptr) {}

    /// Constructs TorrentFragment given piece index, offset and length
    TorrentFragment(uint32_t pieceIdx, uint32_t offset, uint32_t length) :
        PieceIdx(pieceIdx), Offset(offset), Length(length)
    {
        if (Length > 0)
            Data = new uint8_t[Length];
    }

    /// Copy constructor
    TorrentFragment(const TorrentFragment &other) :
        PieceIdx(other.PieceIdx), Offset(other.PieceIdx), Length(other.Length), Data(nullptr)
    {
        if (other.Data != nullptr)
        {
            Data = new uint8_t[Length];
            memcpy(Data, other.Data, Length);
        }
    }

    /// Copy assignment operator
    TorrentFragment &operator=(const TorrentFragment &other)
    {
        if (this != &other)
        {
            PieceIdx = other.PieceIdx;
            Offset   = other.Offset;
            Length   = other.Length;

            if (other.Data != nullptr)
            {
                Data = new uint8_t[Length];
                memcpy(Data, other.Data, Length);
            }
        }

        return *this;
    }

    /// TorrentFragment destructor
    ~TorrentFragment()
    {
        if (Data)
            delete[] Data;
    }
};
