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

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Font.h"
#include "Singleton.h"

namespace gui 
{
    /// Enumerated list of fonts that are supported (used) by the framework
    enum FontType
    {
        DROID_SANS = 0,     // DroidSans.ttf
        VERDANA    = 1,     // Verdana.ttf
    };

    /**
     * @defgroup FontSystem System for loading and rendering TTF-style fonts
     */
    
    /**
     * @class FontStorage
     * @author Timothy Vaccarelli
     * @date 01/06/2016
     * @file FontStorage.h
     * @brief Stores instances of specific fonts, which can be accessed through this class at any time
     * @ingroup FontSystem
     */
    class FontStorage : public Singleton<FontStorage>
    {
        friend class Singleton<FontStorage>;
        
    public:
        /// Default constructor
        FontStorage();
        
        /// Storage destructor
        ~FontStorage();

        /// Returns a raw pointer for the given font type at a certain font point size (defaults to 18)
        Font *getFont(FontType fontType, int pointSize = 18);
        
        /// Loads and stores the supported fonts, which are found in the `dataDir` directory path
        void loadFonts(std::string dataDir);

    private:
        /// Loads a specific font into the font map, given the type, path to the ttf file and a font point size
        void loadFont(FontType fontType, const std::string &path, int pointSize);

    private:
        /// Stores the path to the directory containing font ttf files
        std::string m_dataDir;
        
        /// Container for shared pointers to fonts
        std::vector< std::shared_ptr<Font> > m_fonts;

        /// Container of shared pointers to fonts with specific font point sizes. Key = pair of Font Type, point size
        std::map< std::pair<FontType, int>, std::shared_ptr<Font> > m_fontMap;
    };
}

