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

#include <cmath>
#include <SDL_image.h>
#include <SDL2_gfxPrimitives.h>

#include "LogHelper.h"
#include "Renderer.h"

namespace gui 
{
    const float PI = 3.1415926535897932384626;
    
    Renderer::Renderer(SDL_Window *window)
    {
        m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (m_renderer == nullptr)
            LOG_ERROR("framework.renderer", "Unable to create a renderer for the window! SDL Error: ", SDL_GetError());
    }

    Renderer::~Renderer()
    {
        destroy();
    }
    
    bool Renderer::clearContent()
    {
        return (SDL_RenderClear(m_renderer) == 0);
    }
    
    SDL_Texture *Renderer::createTextureFromImage(const std::string& imagePath)
    {
        return IMG_LoadTexture(m_renderer, imagePath.c_str());
    }
    
    SDL_Texture *Renderer::createTextureFromSurface(SDL_Surface *surface)
    {
        SDL_Texture *tmp = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_FreeSurface(surface);
        
        return tmp;
    }
    
    void Renderer::destroy()
    {
        if (m_renderer != nullptr)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
    }
    
    bool Renderer::drawBox(const Size& size, const Position& position, const Color& color)
    {
        setColorAndBlendMode(color);

        SDL_Rect rect = { position.x, position.y, size.width, size.height };
                    
        return (SDL_RenderFillRect(m_renderer, &rect) == 0);
    }
    
    bool Renderer::drawCircle(float radius, Position position, const Color& color)
    {
        setColorAndBlendMode(color);
        
        int centerX = position.x + radius;
        int centerY = position.y + radius;
        for (int x = position.x; x <= position.x + 2 * radius; ++x)
            for (int y = position.y; y <= position.y + 2 * radius; ++y)
            {
                float dx = x - centerX, dy = y - centerY;
                if (dx * dx + dy * dy < radius * radius)
                    SDL_RenderDrawPoint(m_renderer, x, y);
            }
        
        return true;
    }

    bool Renderer::drawLine(const Position& startPos, const Position &endPos, const Color& color)
    {
        setColorAndBlendMode(color);

        return (SDL_RenderDrawLine(m_renderer, startPos.x, startPos.y, endPos.x, endPos.y) == 0);
    }

    bool Renderer::drawRectOutline(const Size& size, const Position& position, const Color& color)
    {
        setColorAndBlendMode(color);

        SDL_Rect rect = { position.x, position.y, size.width, size.height };

        return (SDL_RenderDrawRect(m_renderer, &rect) == 0);
    }
    
    bool Renderer::drawRoundedBox(const Size& size, const Position& position, const Color& color, int roundRadius)
    {
        setColorAndBlendMode(color);

        if (roundedBoxRGBA(m_renderer, position.x, position.y, position.x + size.width,
            position.y + size.height, roundRadius, color.red, color.green, color.blue, color.alpha) == 0)
        {
            return true;
        }
        
        return false;
    }
    
    bool Renderer::drawRoundedRectangle(const Size& size, const Position& position, const Color& color, int roundRadius)
    {
        setColor(color);
        if (roundedRectangleRGBA(m_renderer, position.x, position.y, position.x + size.width,
            position.y + size.height, roundRadius, color.red, color.green, color.blue, color.alpha) == 0)
        {
            return true;
        }
        
        return false;
    }
    
    void Renderer::renderTexture(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *destRect)
    {
        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0)
            LOG_WARNING("framework.renderer", "Unable to set the texture blend mode. SDL Error: ", SDL_GetError());
            
        if (SDL_RenderCopy(m_renderer, texture, srcRect, destRect) != 0)
            LOG_ERROR("framework.renderer", "Unable to render a texture! SDL Error: ", SDL_GetError());
    }
    
    void Renderer::renderTextureFlipped(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *destRect,
            const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        if (SDL_RenderCopyEx(m_renderer, texture, srcRect, destRect, angle, center, flip) != 0)
            LOG_ERROR("framework.renderer", "Unable to render and flip a texture. SDL Error: ", SDL_GetError());
    }
    
    void Renderer::setColor(const Color& color)
    {
        setColor(color.red, color.green, color.blue, color.alpha);
    }
    
    void Renderer::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        if (SDL_SetRenderDrawColor(m_renderer, red, green, blue, alpha) != 0)
            LOG_ERROR("framework.renderer", "Unable to set a color for the renderer. SDL Error: ", SDL_GetError());
    }
    
    void Renderer::update()
    {
        SDL_RenderPresent(m_renderer);
    }

    void Renderer::setColorAndBlendMode(const Color& color)
    {
        setColor(color);

        if (SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND) != 0)
            LOG_WARNING("framework.renderer", "Unable to set the draw blend mode. SDL Error: ", SDL_GetError());
    }
}

