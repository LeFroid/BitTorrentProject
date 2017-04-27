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

#include "BenDictionary.h"

#include <string>

/**
 * @class TorrentGenerator
 * @brief Used to generate a .torrent file, given a valid file or directory
 *        in the filesystem as well as the other necessary parameters used
 *        by the torrent protocol.
 */
class TorrentGenerator
{
public:
    /// Default constructor
    TorrentGenerator() = default;

    /// Creates the metainfo file, outputting the contents into the file with the given path.
    /// Returns true on successful generation, otherwise false.
    bool createFile(const std::string &outputFile);

    /// Sets the string of the announcer which will store information about the torrent
    void setAnnounceURL(const std::string &url);

    /// Sets the length parameter of each piece of the torrent
    void setPieceLength(int64_t length);

    /// Sets the target file or directory for torrent generation.
    /// Must only be called after the piece length is set
    void setTarget(const std::string &path);

private:
    /// Metainfo dictionary
    bencoding::BenDictionary m_metaInfo;
};
