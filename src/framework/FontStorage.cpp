/*
Copyright (c) 2016-2017, Timothy Vaccarelli
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

#include "FontStorage.h"

namespace gui 
{
    FontStorage::FontStorage() :
        m_dataDir(),
        m_fonts(),
        m_fontMap()
    {
    }

    FontStorage::~FontStorage()
    {
    }
    
    Font *FontStorage::getFont(FontType fontType, int pointSize)
    {
        // Attempt to find existing font with the point size, if not in map, then load it into map and return pointer
        auto keyPair = std::make_pair(fontType, pointSize);
        auto font = m_fontMap.find(keyPair);
        if (font != m_fontMap.end())
            return font->second.get();

        std::string fontPath;
        switch (fontType)
        {
            case DROID_SANS: fontPath = m_dataDir + "/DroidSans.ttf"; break;
            case AMBLE:      fontPath = m_dataDir + "/Amble-Regular.ttf"; break;
            default: break;
        }

        loadFont(fontType, fontPath, pointSize);
        font = m_fontMap.find(keyPair);
        return font->second.get();
    }
    
    void FontStorage::loadFonts(std::string dataDir)
    {
        m_dataDir = dataDir;

        std::string droidSansPath = dataDir + "/DroidSans.ttf";
        std::string amblePath = dataDir + "/Amble-Regular.ttf";

        // Load fonts multiple times for common font point sizes: 12, 14, 18, 24
        loadFont(DROID_SANS, droidSansPath, 12);
        loadFont(DROID_SANS, droidSansPath, 14);
        loadFont(DROID_SANS, droidSansPath, 18);
        loadFont(DROID_SANS, droidSansPath, 24);

        loadFont(AMBLE, amblePath, 12);
        loadFont(AMBLE, amblePath, 14);
        loadFont(AMBLE, amblePath, 18);
        loadFont(AMBLE, amblePath, 24);
    }

    void FontStorage::loadFont(FontType fontType, const std::string &path, int pointSize)
    {
         m_fontMap.insert(std::make_pair(std::make_pair(fontType, pointSize), std::make_shared<Font>(path)));
    }
}

