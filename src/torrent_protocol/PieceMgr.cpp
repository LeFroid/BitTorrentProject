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
#include <cstdlib>

#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>

#include "PieceMgr.h"

#include "BenDictionary.h"
#include "BenInt.h"
#include "BenList.h"
#include "BenString.h"
#include "SHA1Hash.h"
#include "TorrentFile.h"
#include "TorrentMgr.h"
#include "LogHelper.h"

const static uint32_t DefaultFragmentLength = 16384;

using namespace bencoding;

PieceMgr::PieceMgr(std::shared_ptr<TorrentFile> torrentFile) :
    m_pieceLength(torrentFile->getPieceLength()),
    m_finalPieceLen(0),
    m_fragmentsPerPiece(0),
    m_fragmentsInFinalPiece(0),
    m_fileSize(torrentFile->getFileSize()),
    m_bytesUploaded(0),
    m_torrentFile(torrentFile),
    m_digestString(torrentFile->getDigestString()),
    m_currentPiece(0),
    m_pieceInfo(torrentFile->getNumPieces()),
    m_piecesAvailable(torrentFile->getNumPieces()),
    m_pieceBeingDownloaded(),
    m_numPeersDownloading(0),
    m_numPeersFinishedFragment(0),
    m_singleFileHandle()
{
    m_fragmentsPerPiece = std::ceil(double(m_pieceLength) / DefaultFragmentLength);

    /// Calculate information about the final piece (size, number of fragments)
    int64_t finalPieceLen = (int64_t)m_torrentFile->getNumPieces() * (int64_t)m_torrentFile->getPieceLength();
    if (m_fileSize < finalPieceLen)
        m_finalPieceLen = finalPieceLen - m_fileSize;
    else if (m_fileSize > finalPieceLen)
        m_finalPieceLen = m_fileSize - finalPieceLen;
    else
        m_finalPieceLen = m_pieceLength;

    if (DefaultFragmentLength > finalPieceLen)
        m_fragmentsInFinalPiece = 1;
    else
        m_fragmentsInFinalPiece = (uint32_t)ceil(double(m_finalPieceLen) / DefaultFragmentLength);

    /// Initialize file handle(s) for either single- or multi-file mode
    if (m_torrentFile->isSingleFileMode())
        initializeSingleFileHandle();
    //else
    //    initializeMultiFile();
}

PieceMgr::~PieceMgr()
{
    if (m_singleFileHandle.is_open())
        m_singleFileHandle.close();
}

bool PieceMgr::havePiece(uint32_t pieceIdx) const
{
    // Make sure index is valid
    if (pieceIdx >= m_pieceInfo.size())
        return false;
    return m_pieceInfo[pieceIdx];
}

const uint32_t &PieceMgr::getCurrentPieceNum()
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    if (m_pieceInfo.all())
    {
        m_currentPiece = m_pieceInfo.size();
        return m_currentPiece;
    }
    else if (m_pieceInfo[m_currentPiece]
            || !m_piecesAvailable[m_currentPiece]
            || m_pieceBeingDownloaded.empty())
        determineNextPiece();

    return m_currentPiece;
}

uint64_t PieceMgr::getNumPiecesHave()
{
    return m_pieceInfo.count();
}

const boost::dynamic_bitset<> &PieceMgr::getBitsetHave() const
{
    return m_pieceInfo;
}

const uint64_t &PieceMgr::getNumBytesUploaded() const
{
    return m_bytesUploaded;
}

bool PieceMgr::verifyFile()
{
    BenDictionary *infoDict = m_torrentFile->getInfoDictionary();
    if (!infoDict)
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Could not fetch info dictionary to verify file contents!");
        return false;
    }

    // Only verifying single-file mode downloads for now

    // Attempt to fetch the torrent file's digest for the current piece
    const std::string &digestStr = m_digestString->getValue();
    if (digestStr.size() < SHA_DIGEST_LENGTH * m_torrentFile->getNumPieces())
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Digest string from torrent file is too small!");
        return false;
    }

    uint8_t *pieceBuf = new uint8_t[m_pieceLength];
    std::string digestStrTmp;
    SHA1Hash hash;
    for (uint32_t pieceNum = 0; pieceNum < m_pieceInfo.size(); ++pieceNum)
    {
        digestStrTmp = digestStr.substr(pieceNum * SHA_DIGEST_LENGTH, SHA_DIGEST_LENGTH);
        size_t pieceLen = (pieceNum == m_pieceInfo.size() - 1) ? m_finalPieceLen : m_pieceLength;
        memset(pieceBuf, 0, m_pieceLength);
        m_singleFileHandle.seekg(pieceNum * pieceLen);
        m_singleFileHandle.read((char*)&pieceBuf[0], pieceLen);

        hash.update(pieceBuf, pieceLen);
        hash.finalize();

        // If hashes match, store the piece onto the disk, set m_pieceInfo[m_currentPiece] to 1
        if (memcmp(hash.getDigest(), digestStrTmp.c_str(), SHA_DIGEST_LENGTH) != 0)
        {
            LOG_ERROR("torrent_protocol.PieceMgr", "Digest of piece ", pieceNum, " invalid! digestPtr = ", digestStrTmp);
            delete[] pieceBuf;
            return false;
        }
        hash.initialize();
    }

    delete[] pieceBuf;
    return true;
}

void PieceMgr::markPieceAvailable(const uint32_t &pieceIdx)
{
    // Make sure index is valid
    if (pieceIdx >= m_piecesAvailable.size())
        return;

    m_piecesAvailable[pieceIdx] = true;
}

void PieceMgr::readPeerBitset(const boost::dynamic_bitset<> &set)
{
    m_piecesAvailable |= set;
}

std::shared_ptr<TorrentFragment> PieceMgr::getFragmentToDownload()
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    std::shared_ptr<TorrentFragment> fragPtr(nullptr);

    uint32_t activeFragCount = (m_numPeersDownloading + m_numPeersFinishedFragment) % m_pieceBeingDownloaded.size();
    fragPtr = m_pieceBeingDownloaded.at(activeFragCount);
    ++m_numPeersDownloading;

    return fragPtr;
}

std::shared_ptr<TorrentFragment> PieceMgr::getFragmentToUpload(uint32_t pieceIdx, uint32_t offset, uint32_t length)
{
    std::shared_ptr<TorrentFragment> fragPtr(nullptr);

    // Bounds checks
    if (pieceIdx + 1 > m_pieceInfo.size() || !m_pieceInfo[pieceIdx])
        return fragPtr;
    if (offset + length > m_pieceLength)
        return fragPtr;

    // Load fragment from disk
    BenDictionary *infoDict = m_torrentFile->getInfoDictionary();
    if (!infoDict)
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Unable to retrieve pointer to Info Dictionary, will not be reading data from disk.");
        return fragPtr;
    }

    // Determine if single or multi file mode
    auto it = infoDict->find("files");
    if (it != infoDict->end())
    {
        //TODO: Multi-file mode
    }
    else
    {
        // Single-File mode
        it = infoDict->find("name");
        if (it != infoDict->end())
        {
            // Get file length to ensure valid request
            m_singleFileHandle.seekg(0, m_singleFileHandle.end);
            uint64_t fileLen = m_singleFileHandle.tellg();
            m_singleFileHandle.seekg(0, m_singleFileHandle.beg);

            if (fileLen < (m_pieceLength * pieceIdx) + offset + length)
                return fragPtr;

            // Seek to position, read data into fragment structure, done
            fragPtr = std::make_shared<TorrentFragment>(pieceIdx, offset, length);
            m_singleFileHandle.seekg(m_pieceLength * pieceIdx + offset);
            m_singleFileHandle.read((char*)&(fragPtr->Data[0]), length);

            // Increment bytes uploaded counter
            m_bytesUploaded += length;
        }
    }
    return fragPtr;
}

void PieceMgr::onFragmentDownloaded(uint32_t pieceIdx)
{
    std::lock_guard<std::mutex> lock(m_pieceLock);

    //LOG_DEBUG("torrent_protocol.network", "onFragmentDownloaded() has been called");

    // Sanity check
    if (m_numPeersDownloading == 0 || pieceIdx != m_currentPiece)
        return;

    // Decrement the count of peers still downloading fragments, increment the completed frag. download counter
    --m_numPeersDownloading;
    ++m_numPeersFinishedFragment;

    // Now check if all fragments have been downloaded
    if (m_numPeersFinishedFragment == m_pieceBeingDownloaded.size())
        onAllFragmentsDownloaded();
}

uint32_t PieceMgr::getNumFragments(uint32_t pieceIdx)
{
    if (pieceIdx >= m_pieceInfo.size())
        return 0;

    return (pieceIdx + 1 == m_pieceInfo.size()) ? m_fragmentsInFinalPiece : m_fragmentsPerPiece;
}

void PieceMgr::determineNextPiece()
{
    //TODO: First piece: randomly chosen, after that use rarest first algorithm
    uint32_t maybePiece;
    auto numPieces = m_pieceInfo.size();

    boost::dynamic_bitset<> piecesToChoose = (m_piecesAvailable & (~m_pieceInfo));
    if (!piecesToChoose.any())
        m_currentPiece = numPieces + 1;

    while (true)
    {
        maybePiece = uint64_t(rand()) % numPieces;
        if (piecesToChoose[maybePiece])
        {
            m_currentPiece = maybePiece;

            // Reset fragment vector and set peer download counts back to 0
            m_pieceBeingDownloaded.clear();
            m_numPeersDownloading = 0;
            m_numPeersFinishedFragment = 0;

            // Populate fragment vector for the new piece
            const uint32_t pieceLength = (m_currentPiece + 1 == m_pieceInfo.size()) ? m_finalPieceLen : m_pieceLength;
            uint32_t numFragsForPiece = getNumFragments(m_currentPiece);

            uint32_t currentFragmentLength = std::min(DefaultFragmentLength, pieceLength);
            uint32_t currentFragmentOffset = 0;
            for (uint32_t i = 0; i < numFragsForPiece; ++i)
            {
                std::shared_ptr<TorrentFragment> fragment(std::make_shared<TorrentFragment>(m_currentPiece, currentFragmentOffset, currentFragmentLength));
                m_pieceBeingDownloaded.push_back(fragment);

                currentFragmentOffset += currentFragmentLength;
                currentFragmentLength = std::min(DefaultFragmentLength, pieceLength - currentFragmentOffset);
                if (currentFragmentLength == 0)
                    return;
            }

            return;
        }
    }
    //m_currentPiece = numPieces + 1;
}

void PieceMgr::onAllFragmentsDownloaded()
{
    // Determine piece length (either standard or length of the final piece)
    size_t pieceLength = (m_currentPiece + 1 == m_pieceInfo.size()) ? m_finalPieceLen : m_pieceLength;

    // Combine data from each fragment into one array, get the digest, compare to expected value
    uint8_t *pieceData = new uint8_t[pieceLength];

    for (auto fragment : m_pieceBeingDownloaded)
        memcpy(&pieceData[fragment->Offset], fragment->Data, fragment->Length);

    // Calculate digest value
    SHA1Hash hash;
    hash.update(pieceData, pieceLength);
    hash.finalize();

    // Attempt to fetch the torrent file's digest for the current piece
    const std::string &digestStr = m_digestString->getValue();
    if (digestStr.size() < SHA_DIGEST_LENGTH * m_pieceInfo.size())
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Digest string from torrent file is too small!");
        delete[] pieceData;
        return;
    }

    /*uint8_t *digestPtr = (uint8_t*)digestStr.c_str();
    digestPtr += (SHA_DIGEST_LENGTH * m_currentPiece);*/
    std::string digestStrTmp = digestStr.substr(m_currentPiece * SHA_DIGEST_LENGTH, SHA_DIGEST_LENGTH);

    // If hashes match, store the piece onto the disk, set m_pieceInfo[m_currentPiece] to 1
    if (memcmp(hash.getDigest(), digestStrTmp.c_str(), SHA_DIGEST_LENGTH) == 0)
    {
        writePieceToDisk(pieceData, pieceLength);
        LOG_INFO("torrent_protocol.PieceMgr", "Verified piece ", m_currentPiece, ", writing to disk. Have downloaded ", m_pieceInfo.count(), " of ", m_pieceInfo.size(), " pieces.");
    }
    else
    {
        // Attempt to re-download the fragment
        m_numPeersFinishedFragment = 0;
    }

    // Free memory that was allocated ot pieceData
    delete[] pieceData;
}

void PieceMgr::writePieceToDisk(uint8_t *data, size_t pieceLength)
{
    // Get info dictionary to determine file name
    BenDictionary *infoDict = m_torrentFile->getInfoDictionary();
    if (!infoDict)
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Unable to retrieve pointer to Info Dictionary, will not be writing data to disk.");
        return;
    }

    // Determine if single or multi file mode
    auto it = infoDict->find("files");
    if (it != infoDict->end())
    {
        // Multi-File mode
        BenList *fileList = bencast<BenList*>(it->second);

        // Get the path which files will be placed into
        std::string basePathStr = eTorrentMgr.getDownloadDirectory();
        it = infoDict->find("name");
        if (it != infoDict->end())
            basePathStr.append(bencast<BenString*>(it->second)->getValue());
        boost::filesystem::path basePath(basePathStr);

        // Create base directory if not already extant
        boost::system::error_code ec;
        if (!boost::filesystem::exists(basePath, ec))
            boost::filesystem::create_directory(basePath, ec);

        // Next, iterate through list of files until first one found with offset of piece to save
        int64_t pieceOffset = m_currentPiece * m_pieceLength;
        int64_t filesLength = 0;
        for (auto fileIt = fileList->begin(); fileIt != fileList->end(); ++fileIt)
        {
            BenDictionary *fileDict = bencast<BenDictionary*>(*fileIt);
            auto lengthIt = fileDict->find("length");
            filesLength += (bencast<BenInt*>(lengthIt->second)->getValue());
            if (filesLength >= pieceOffset)
            {
                // Get full path to file
                auto pathItr = fileDict->find("path");
                if (pathItr != fileDict->end())
                {
                    // Iterate through path list, appending each item to the filePath object
                    BenList *pathList = bencast<BenList*>(pathItr->second);
                    auto pathListLen = pathList->size();
                    boost::filesystem::path filePath = basePath;

                    size_t currentPathLen = 1;
                    for (auto pathItr2 = pathList->begin(); pathItr2 != pathList->end(); ++pathItr2)
                    {
                        boost::filesystem::path subPath(bencast<BenString*>(*pathItr2)->getValue());
                        filePath /= subPath;

                        // Check if we haven't yet reached the end file, and if not, determine if the
                        // parent directory needs to be created
                        if (currentPathLen++ < pathListLen)
                        {
                            if (!boost::filesystem::exists(filePath, ec))
                                boost::filesystem::create_directory(filePath, ec);
                        }
                    }

                    // Finally, open the file, write data into it, if length of current piece + offset > length of file, add
                    // the next part of the current piece to the next file(s) until all has been written out
                }
                break;
            }
        }
    }
    else
    {
        // Single-File mode
        // Seek to appropriate position in file
        m_singleFileHandle.seekp(m_currentPiece * m_pieceLength);

        // Write data
        m_singleFileHandle.write((char*)&data[0], pieceLength);
        m_singleFileHandle.flush();
    }

    // If able to write to disk, mark it as downloaded
    m_pieceInfo[m_currentPiece] = true;
}

void PieceMgr::initializeSingleFileHandle()
{
    // Get info dictionary to determine file name
    BenDictionary *infoDict = m_torrentFile->getInfoDictionary();
    if (!infoDict)
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Unable to retrieve pointer to Info Dictionary, will not be reading or writing data to disk.");
        return;
    }

    // Single-File mode
    auto it = infoDict->find("name");
    if (it == infoDict->end())
    {
        LOG_ERROR("torrent_protocol.PieceMgr", "Unable to find \"name\" key in Info Dictionary. Will not be able to write to disk.");
        return;
    }

    // Get path to file: Should be {Download Directory} / {File Name}
    std::string pathStr = eTorrentMgr.getDownloadDirectory() + bencast<BenString*>(it->second)->getValue();
    boost::filesystem::path filePath(pathStr);

    // Open file
    m_singleFileHandle.open(pathStr, std::fstream::binary | std::fstream::in | std::fstream::out);

    // If could not open file, create file and resize it ahead of time
    if (!m_singleFileHandle)
    {
        m_singleFileHandle.open(pathStr, std::fstream::binary | std::fstream::trunc | std::fstream::out);
        m_singleFileHandle.close();

        boost::system::error_code ec;
        if (boost::filesystem::file_size(filePath, ec) < m_fileSize)
        {
            if (ec)
                LOG_ERROR("torrent_protocol.PieceMgr", "Error reported from calling boost::filesystem::file_size on ", pathStr, " , Error message: ", ec.message());

            boost::filesystem::resize_file(filePath, m_fileSize, ec);

            if (ec)
                LOG_ERROR("torrent_protocol.PieceMgr", "Error reported from calling boost::filesystem::resize_file on ", pathStr, " , Error message: ", ec.message());

            // Re-open file with original flags
            m_singleFileHandle.open(pathStr, std::fstream::binary | std::fstream::in | std::fstream::out);
        }
    }
}

