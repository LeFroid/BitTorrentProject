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

#include <string>
#include <SDL_ttf.h>

#include "GUIProperties.h"

namespace gui 
{
    class Renderer;
    
    /**
     * @class Font
     * @author Timothy Vaccarelli
     * @date 01/06/2016
     * @file Font.h
     * @brief Represents a TTF font that can be used by the renderer to display text
     * @ingroup FontSystem
     */
    class Font
    {
    public:
        /// The various font styles available for rendering
        enum Style
        {
            NORMAL        = 0,    // TTF_STYLE_NORMAL
            BOLD          = 1,    // TTF_STYLE_BOLD
            ITALIC        = 2,    // TTF_STYLE_ITALIC
            UNDERLINE     = 4,    // TTF_STYLE_UNDERLINE
            STRIKETHROUGH = 8     // TTF_STYLE_STRIKETHROUGH
        };
        
        /// OR operator for font styles
        friend Style operator|(Style lhs, Style rhs)
        {
            return Style(unsigned(lhs) | unsigned(rhs));
        }
        
        /**
         * @brief Loads a TTF file that can be used to display text
         * @param fontFile The path to the TTF file that will be used
         * @param ptSize   The point size of the font, defaults to 24
         * @param fontStyle The style of the font, defaults to normal and can be changed later
         */
        Font(std::string fontFile, int ptSize = 24, Style fontStyle = NORMAL);
        
        /// Font destructor, deallocates the resources used from loading the font
        ~Font();
        
        /// Returns the recommended amount of spacing between any two, separate lines of this font
        int getLineSkip();
        
        /**
         * @brief Provides information regarding the current font point size.
         * @return The current font point size
         */
        const int& getPointSize() const;
        
        /**
         * @brief Provides information regarding the current font style
         * @return The current font style
         */
        const Style& getStyle() const;
        
        /// Returns a pointer to the SDL TTF_Font data structure
        TTF_Font *getTTF();
        
        /**
         * @brief Formats and prints any text with the given parameters
         * @param text The string to print onto the screen
         * @param textColor The color of the text to be printed
         * @param pointSize The point size of the text
         * @param textRect The rectangle that the text is bound by
         * @param renderer A pointer to the rendering object
         */
        void printText(std::string text, const Color& textColor, int pointSize, SDL_Rect textRect, Renderer *renderer);
        
        /// Adjust the point size of the font
        void setPointSize(int size);
        
        /// Adjust the style of the font
        void setStyle(Style fontStyle);

        /// Parses a block of text that contains newlines, modifying the number & position of said newlines to accomidate the
        /// text boundary
        /// Should not be called anywhere outside of the Font object itself, except within the GUIObject::setHint(...) method
        std::string reformatTextBlock(std::string text, SDL_Rect textRect);
    
    // Private members
    private:
        /// Attempts to open the font and allocate resources towards it, returning true on success
        bool openFont();
        
        /// Attempts to close the font and deallocate its resources
        void closeFont();
        
        /**
         * @brief Prints a given text with SDL_ttf's builtin line wrap function
         * @param text The string to print onto the rect
         * @param textColor The color of the text to be printed
         * @param textRect The rectangle that the text is bound by
         * @param renderer A pointer to the rendering object
         */
        void printTextLineWrap(const std::string& text, const SDL_Color& textColor, SDL_Rect textRect, Renderer *renderer);
    
    // Private font data
    private:
        /// Pointer to the SDL TTF font
        TTF_Font *m_font;
        
        /// The file which the font comes from
        std::string m_fontFile;
        
        /// The point size of the font
        int m_pointSize;
        
        /// The style of the font
        Style m_style;
    };
}

