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

#include <cmath>
#include <fstream>

#include "LogHelper.h"

#include "Decoder.h"
#include "TorrentFile.h"

using namespace bencoding;

TorrentFile::TorrentFile(std::string path) :
    m_numPieces(0),
    m_size(0),
    m_infoHash(),
    m_metaInfo()
{
    parseFile(path);
}

http::URL TorrentFile::getAnnounceURL()
{
    auto it = m_metaInfo->find("announce");
    if (it == m_metaInfo->end())
    {
        LOG_ERROR("torrent_protocol.TorrentFile", "Unable to get announce URL from metainfo!");
        return http::URL("");
    }

    return http::URL(bencast<BenString*>(it->second)->getValue());
}

std::shared_ptr<BenString> TorrentFile::getDigestString()
{
    std::shared_ptr<BenString> retVal(nullptr);

    BenDictionary *infoDict = getInfoDictionary();
    if (infoDict)
    {
        auto itr = infoDict->find("pieces");
        if (itr != infoDict->end())
            retVal = std::static_pointer_cast<BenString>(itr->second);
    }

    return retVal;
}

BenDictionary *TorrentFile::getInfoDictionary()
{
    BenDictionary::iterator infoItr = m_metaInfo->find("info");
    if (infoItr == m_metaInfo->end())
    {
        LOG_ERROR("torrent_protocol.TorrentFile", "Unable to get info dictionary");
        return nullptr;
    }

    return bencast<BenDictionary*>(infoItr->second);
}

uint8_t *TorrentFile::getInfoHash()
{
    return m_infoHash.getDigest();
}

const uint64_t &TorrentFile::getFileSize() const
{
    return m_size;
}

const uint64_t &TorrentFile::getNumPieces() const
{
    return m_numPieces;
}

uint64_t TorrentFile::getPieceLength()
{
    if (BenDictionary *infoDict = getInfoDictionary())
    {
        auto it = infoDict->find("piece length");
        if (it != infoDict->end())
            return bencast<BenInt*>(it->second)->getValue();
    }
    
    LOG_ERROR("torrent_protocol.TorrentFile", "Unable to fetch piece length");
    return 0;
}

void TorrentFile::parseFile(const std::string &path)
{
    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if (!file.is_open())
        return;

    std::string encodedData;

    // Get length of file
    file.seekg(0, file.end);

    encodedData.reserve(file.tellg());

    // Restore position and read contents into encodedData
    file.seekg(0, file.beg);

    encodedData.assign((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

    // Decode dictionary and set it as metainfo
    Decoder decoder;
    m_metaInfo = std::static_pointer_cast<BenDictionary>(decoder.decode(encodedData));

    // Get digest of info dictionary
    auto infoBeginPos = encodedData.find("4:info");
    if (infoBeginPos != std::string::npos)
    {
        // Determine length of dictionary, get substring and then hash the value
        infoBeginPos += 6;
        std::string infoDictStr = encodedData.substr(infoBeginPos);
        decoder.decode(infoDictStr);

        auto infoEndPos = decoder.getIndex();
        infoDictStr = encodedData.substr(infoBeginPos, infoEndPos);

        m_infoHash.update((const uint8_t*)infoDictStr.c_str(), infoDictStr.size());
        m_infoHash.finalize();
    }

    // Calculate total file size
    calculateFileSize();
}

void TorrentFile::calculateFileSize()
{
    BenDictionary *infoDict = getInfoDictionary();
    if (!infoDict)
        return;

    // If single file mode, file size will be assicated with key "length"
    auto it = infoDict->find("length");
    if (it != infoDict->end())
    {
        m_size = (uint64_t) bencast<BenInt*>(it->second)->getValue();
    }
    else
    {
        // For multi-file mode, must calculate the sum of each file's respective length

        // First get files list
        it = infoDict->find("files");
        if (it == infoDict->end())
        {
            LOG_ERROR("torrent_protocol.TorrentFile", "Torrent is missing file information");
            return;
        }
        BenList *fileList = bencast<BenList*>(it->second);

        // Next, iterate through list of files
        for (auto fileIt = fileList->begin(); fileIt != fileList->end(); ++fileIt)
        {
            BenDictionary *fileDict = bencast<BenDictionary*>(*fileIt);
            auto lenIt = fileDict->find("length");
            if (lenIt != fileDict->end())
                m_size += bencast<BenInt*>(lenIt->second)->getValue();
        }
    }

    // Also determine piece count based on total size
    it = infoDict->find("piece length");
    if (it != infoDict->end())
    {
        m_numPieces = (uint64_t) ceill( ((long double)m_size) / bencast<BenInt*>(it->second)->getValue() );
    }
}
