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

#include <boost/filesystem.hpp>
#include <fstream>
#include "BenInt.h"
#include "BenString.h"
#include "Encoder.h"
#include "SHA1Hash.h"
#include "TorrentGenerator.h"

using namespace bencoding;

bool TorrentGenerator::createFile(const std::string &outputFile)
{
    // Check if required key-value pairs are in metainfo dictionary
    if (m_metaInfo.find("announce") == m_metaInfo.end())
        return false;

    auto infoItr = m_metaInfo.find("info");
    if (infoItr == m_metaInfo.end())
        return false;

    BenDictionary *infoDict = bencast<BenDictionary*>(infoItr->second);
    if (infoDict->find("piece length") == infoDict->end()
            || infoDict->find("pieces") == infoDict->end()
            || infoDict->find("name") == infoDict->end())
        return false;

    // info dictionary must have either length key (single file mode) or files key
    if (infoDict->find("length") == infoDict->end() && infoDict->find("files") == infoDict->end())
        return false;

    // Good to go
    std::ofstream f(outputFile, std::ofstream::out | std::ofstream::trunc);
    Encoder enc;
    m_metaInfo.accept(enc);
    const std::string &contents = enc.getData();
    f.write(contents.c_str(), contents.size());
    f.flush();
    f.close();
    return true;
}

void TorrentGenerator::setAnnounceURL(const std::string &url)
{
    // Check if key already exists in metainfo dictionary
    auto it = m_metaInfo.find("announce");
    if (it != m_metaInfo.end())
    {
        bencast<BenString*>(it->second)->setValue(url);
        return;
    }

    // Create new benstring otherwise
    std::shared_ptr<BenString> announce(std::make_shared<BenString>(url));
    m_metaInfo["announce"] = announce;
}

void TorrentGenerator::setPieceLength(int64_t length)
{
    std::shared_ptr<BenDictionary> infoDict(nullptr);

    // Get info dictionary or create one if not in metainfo yet
    auto infoItr = m_metaInfo.find("info");
    if (infoItr != m_metaInfo.end())
        infoDict = std::static_pointer_cast<BenDictionary>(infoItr->second);
    else
    {
        infoDict = std::make_shared<BenDictionary>();
        m_metaInfo["info"] = infoDict;
    }

    // Set piece length
    auto lenItr = infoDict->find("piece length");
    if (lenItr != infoDict->end())
    {
        bencast<BenInt*>(lenItr->second)->setValue(length);
        return;
    }

    std::shared_ptr<BenInt> pieceLen = std::make_shared<BenInt>(length);
    infoDict->insert(std::make_pair("piece length", pieceLen));
}

void TorrentGenerator::setTarget(const std::string &path)
{
    // Check if path points to a file or a directory, or simply doesn't exist
    boost::system::error_code ec;
    boost::filesystem::path p(path);
    if (!boost::filesystem::exists(p, ec))
        return;

    if (boost::filesystem::is_directory(p, ec))
    {
        // multi file torrent
    }
    else
    {
        // single file torrent
    }
}
