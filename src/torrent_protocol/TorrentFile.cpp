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

#include <fstream>

#include "LogHelper.h"

#include "Decoder.h"
#include "TorrentFile.h"

using namespace bencoding;

TorrentFile::TorrentFile(std::string path) :
    m_bytesDownloaded(0),
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

    return http::URL(static_cast<BenString*>(it->second.get())->getValue());
}

BenDictionary *TorrentFile::getInfoDictionary()
{
    BenDictionary::iterator infoItr = m_metaInfo->find("info");
    if (infoItr == m_metaInfo->end())
    {
        LOG_ERROR("torrent_protocol.TorrentFile", "Unable to get info dictionary");
        return nullptr;
    }

    return static_cast<BenDictionary*>(infoItr->second.get());
}

uint8_t *TorrentFile::getInfoHash()
{
    return m_infoHash.getDigest();
}

const uint64_t &TorrentFile::getFileSize() const
{
    return m_size;
}

void TorrentFile::parseFile(const std::string &path)
{
    std::ifstream file(path, std::ifstream::in);
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
        infoDictStr = encodedData.substr(infoBeginPos, infoEndPos - infoBeginPos + 1);

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
        m_size = (uint64_t) static_cast<BenInt*>(it->second.get())->getValue();
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
        BenList *fileList = static_cast<BenList*>(it->second.get());

        // Next, iterate through list of files
        for (auto fileIt = fileList->begin(); fileIt != fileList->end(); ++fileIt)
        {
            BenDictionary *fileDict = static_cast<BenDictionary*>(fileIt->get());
            auto lenIt = fileDict->find("length");
            if (lenIt != fileDict->end())
                m_size += static_cast<BenInt*>(lenIt->second.get())->getValue();
        }
    }
}
