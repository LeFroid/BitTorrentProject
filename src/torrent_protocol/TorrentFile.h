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

#include <string>

#include "BenDictionary.h"
#include "BenInt.h"
#include "BenList.h"
#include "BenString.h"

#include "SHA1Hash.h"

#include "URL.h"

/**
 * @class TorrentFile
 * @brief Represents a .torrent file stored on the local machine, used to
 *        download one or more files through the torrent protocol.
 */
class TorrentFile
{
public:
    /// Constructs the torrent file object with the path to a torrent file on the local machine
    explicit TorrentFile(std::string path);

    /// Returns the announce URL
    http::URL getAnnounceURL();

    /// Returns a pointer to the info dictionary associated with the torrent file
    bencoding::BenDictionary *getInfoDictionary();

    /// Returns the digest of the value of the info key from the torrent file
    uint8_t *getInfoHash();

    /// Returns the total length in bytes of the file(s) associated with the torrent
    const uint64_t &getFileSize() const;

private:
    /// Parses the torrent file with the given path, storing the decoded data into the meta info dictionary
    void parseFile(const std::string &path);

    /// Calculates and sets the total byte size of the file(s) associated with the torrent
    void calculateFileSize();

private:
    /// Total number of bytes that have been downloaded
    uint64_t m_bytesDownloaded;

    /// Total size of the file(s) associated with the torrent, in bytes
    uint64_t m_size;

    /// Stores the digest of the value of the info key from the torrent file
    SHA1Hash m_infoHash;

    /// Metainfo contained in the torrent file
    std::shared_ptr<bencoding::BenDictionary> m_metaInfo;
};
