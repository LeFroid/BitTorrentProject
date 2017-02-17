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

#include <cstdint>
#include <string>
#include <SDL.h>

#include "GUIProperties.h"

namespace gui 
{
    /**
     * @class Renderer
     * @author Timothy Vaccarelli
     * @date 01/05/2016
     * @file Renderer.h
     * @brief Used to render objects, images, etc. to the graphical user interface
     */
    class Renderer
    {
    public:
        /// Attempts to create a rendering object
        Renderer(SDL_Window *window);
        
        /// Renderer destructor
        ~Renderer();
        
        /// Clears the content that is currently being rendered
        bool clearContent();
        
        /**
         * @brief Creates a texture given a path to an image file, provided by the respectively calling image object instance
         * @param imagePath Path to the image file to use
         * @return A pointer to the SDL_Texture structure which represents the image data
         */
        SDL_Texture *createTextureFromImage(const std::string& imagePath);
        
        /**
         * @brief Creates a texture given a surface. This also frees the surface.
         * @param surface The SDL surface structure pointer to use
         * @return A pointer to the new SDL_Texture structure
         */
        SDL_Texture *createTextureFromSurface(SDL_Surface *surface);

        /// Attempts to destroy the renderer, if it is in use
        void destroy();
        
        /// Attempts to draw a filled rectangle, given a size, position and color. Returns true on success
        bool drawBox(const Size& size, const Position& position, const Color& color);
        
        /// Attempts to draw a filled in circle, given a size (radius), position and color. Returns true on success
        bool drawCircle(float radius, Position position, const Color& color);

        /// Attempts to draw a line with the given color, from starting position to ending position
        bool drawLine(const Position& startPos, const Position &endPos, const Color& color);
        
        /// Attempts to draw the border of a rectangle, given the size, position, and color to draw. Returns true on success
        bool drawRectOutline(const Size& size, const Position& position, const Color& color);

        /// Attempts to draw a filled rectangle with rounded corners, given a size, position, color and radius of the curves. Returns true on success
        bool drawRoundedBox(const Size& size, const Position& position, const Color& color, int roundRadius);
        
        /// Attempts to draw a rectangle with a given size, position and color. Can be used as a border for a rounded box. Returns true on success
        bool drawRoundedRectangle(const Size& size, const Position& position, const Color& color, int roundRadius);
                
        /**
         * @brief Renders a given SDL_Texture.
         * @param texture The texture to be rendered.
         * @param srcRect The source rectangle of the texture to copy
         * @param destRect The rectangle of the main rendering area to map the texture onto
         */
        void renderTexture(SDL_Texture *texture, const SDL_Rect *srcRect = NULL, const SDL_Rect *destRect = NULL);
        
        /**
         * @brief Renders a given SDL_Texture, flipped in a certain way.
         * @param texture The texture to be rendered.
         * @param srcRect The source rectangle of the texture to copy
         * @param destRect The rectangle of the main rendering area to map the texture onto
         * @param angle The angle in degrees that indicates the rotation that will be applied to destRect
         * @param center A pointer to a point indicating the point around which destRect will be rotated (if NULL, rotation done around destRect.w/2, destRect.h/2)
         * @param flip A SDL_RendererFlip value stating which flipping actions should be performed on the texture
         */
        void renderTextureFlipped(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *destRect, const double angle,
            const SDL_Point *center, const SDL_RendererFlip flip);
        
        /// Sets the current color for the renderer
        void setColor(const Color& color);
        
        /**
         * @brief Sets the current color for the renderer
         * @param red Red component
         * @param green Green component
         * @param blue Blue component
         * @param alpha Alpha component
         */
        void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
        
        /// Updates the screen with any new content
        void update();
        
    private:
        /// Sets the color of the rendering target and the blend mode to "Blend" for the renderer. Used often internally
        void setColorAndBlendMode(const Color& color);

        /// The SDL library rendering object
        SDL_Renderer *m_renderer;
    };
}

