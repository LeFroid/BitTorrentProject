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

#include <algorithm>
#include <vector>
#include <cmath>

#include "Font.h"
#include "LogHelper.h"
#include "Renderer.h"

namespace gui 
{
    Font::Font(std::string fontFile, int ptSize, Font::Style fontStyle) :
        m_font(nullptr),
        m_fontFile(fontFile),
        m_pointSize(ptSize),
        m_style(fontStyle)
    {
        if (!openFont())
            LOG_ERROR("framework.font", "Error in loading the font\"", fontFile, "\". SDL Error: ", SDL_GetError());
    }
    
    Font::~Font()
    {
        closeFont();
    }
    
    int Font::getLineSkip()
    {
        if (m_font == nullptr)
            return -1;
        else
            return TTF_FontLineSkip(m_font);
    }
    
    const int& Font::getPointSize() const
    {
        return m_pointSize;
    }
    
    const Font::Style& Font::getStyle() const
    {
        return m_style;
    }
    
    TTF_Font *Font::getTTF()
    {
        return m_font;
    }
    
    void Font::printText(std::string text, const Color& textColor, int pointSize, SDL_Rect textRect, Renderer *renderer)
    {
        if (text.empty())
            return;

        setPointSize(pointSize);
        TTF_SetFontStyle(m_font, static_cast<int>(m_style));

        SDL_Color color = { textColor.red, textColor.green, textColor.blue, textColor.alpha };

        // If no newlines, default to SDL_ttf rendering function
        if (text.find('\n', 0) == std::string::npos)
        {
            text.push_back('\n');
            //printTextLineWrap(text, color, textRect, renderer);
            //return;
        }

        int lineSkip = getLineSkip();
        size_t numLines = std::count(text.begin(), text.end(), '\n');
        if (numLines * lineSkip > textRect.h)
        {
            setPointSize(pointSize * 3 / 5);
        }

        std::string finalText = reformatTextBlock(text, textRect);

        int height = (textRect.h <= getPointSize()) ? textRect.h : getPointSize();
        lineSkip = getLineSkip();

        // Break into lines, print the text
        int i = 0;
        std::size_t n = 0;
        while (n != std::string::npos)
        {
            std::string subString;
            
            n = finalText.find('\n', 0);
            subString = finalText.substr(0, n);
            
            if(n != std::string::npos)
                finalText = finalText.substr(n + 1, std::string::npos);
                
            if (subString.empty())
            {
                ++i;
                continue;
            }

            // Get the width of the line
            int w = 0;
            TTF_SizeText(m_font, subString.c_str(), &w, &height);

            SDL_Surface *textSurface = TTF_RenderText_Blended(getTTF(), subString.c_str(), color);
            SDL_Texture *textTexture = renderer->createTextureFromSurface(textSurface);
            
            SDL_Rect tmpRect = textRect;
            tmpRect.y += i * lineSkip;
            tmpRect.h = height;
            tmpRect.w = w;
            renderer->renderTexture(textTexture, NULL, &tmpRect);
            SDL_DestroyTexture(textTexture);
            
            ++i;
        }
    }
    
    void Font::printTextLineWrap(const std::string& text, const SDL_Color& textColor, SDL_Rect textRect, Renderer *renderer)
    {
        // Get the width and height of the text, if less than SDL_Rect, make the rect smaller
        int w = 0, h = 0;
        TTF_SizeText(m_font, text.c_str(), &w, &h);
        if (w < textRect.w)
            textRect.w = w;

        // Render the text
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(getTTF(), text.c_str(), textColor, textRect.w);

        // Adjust scale
        int scale;
        if (textSurface->h > textRect.h)
            scale = getPointSize() * std::sqrt((float)textRect.h / textSurface->h);
        else
            scale = getPointSize() * std::sqrt((float)textSurface->h / textRect.h);

        setPointSize(scale);
        TTF_SizeText(m_font, text.c_str(), &w, &h);
        if (w < textRect.w)
            textRect.w = w;

        SDL_FreeSurface(textSurface);
        textSurface = TTF_RenderText_Blended_Wrapped(getTTF(), text.c_str(), textColor, textRect.w);

        if (textSurface->h < textRect.h)
            textRect.h = textSurface->h;

        SDL_Texture *textTexture = renderer->createTextureFromSurface(textSurface);
        if (textTexture)
            renderer->renderTexture(textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    }

    std::string Font::reformatTextBlock(std::string text, SDL_Rect textRect)
    {
        std::string finalText;
        std::size_t n = 0;
        while (n != std::string::npos)
        {
            std::string line;

            n = text.find('\n');
            line = text.substr(0, n);

            if (n != std::string::npos)
                text = text.substr(n + 1);
            else
                text.clear();

            if (line.empty())
            {
                finalText += '\n';
                continue;
            }

            // Get the width of the line
            int w = 0, h = 0;
            TTF_SizeText(m_font, line.c_str(), &w, &h);

            while (w > textRect.w)
            {
                // chop one word off at a time until dimensions are correct
                std::size_t lastWordPos = line.find_last_of(' ');
                if (lastWordPos != std::string::npos)
                {
                    std::string lastWord = line.substr(lastWordPos + 1, std::string::npos);
                    line = line.substr(0, lastWordPos);

                    text = lastWord + ' ' + text;

                    if (text.find('\n') == std::string::npos)
                    {
                        text.push_back('\n');
                        n = text.size() - 1;
                    }

                    TTF_SizeText(m_font, line.c_str(), &w, &h);
                }
                else
                    w = textRect.w;
            }

            finalText += line + '\n';
        }
        return finalText;
    }
    
    void Font::setPointSize(int size)
    {
        if (size != m_pointSize && size > 0)
        {
            m_pointSize = size;
            
            // this also closes the font in order to reopen it with the new point size
            openFont();
        }
    }
    
    void Font::setStyle(Style fontStyle)
    {
        m_style = fontStyle;
    }
    
    bool Font::openFont()
    {
        closeFont();
            
        m_font = TTF_OpenFont(m_fontFile.c_str(), m_pointSize);
        return (m_font != nullptr);
    }
    
    void Font::closeFont()
    {
        if (m_font != nullptr)
        {
            TTF_CloseFont(m_font);
            m_font = nullptr;
        }
    }
}

