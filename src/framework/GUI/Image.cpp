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

#include <SDL_image.h>

#include "Image.h"
#include "LogHelper.h"
#include "Renderer.h"

#include <cmath>

namespace gui 
{
    Image::Image(GUIObject *parent) :
        GUIObject(parent),
        m_imagePath(),
        m_texture(nullptr)
    {
    }
    
    Image::Image(GUIObject *parent, Position position, Size size, std::string imagePath, bool overrideBounds) :
        GUIObject(parent, position, size, overrideBounds),
        m_imagePath(imagePath),
        m_texture(nullptr)
    {
    }
    
    Image::~Image()
    {
        destroy();
    }
    
    void Image::destroy()
    {
        if (m_texture != nullptr)
        {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }
    
    void Image::draw()
    {
        // check if there's a file to load
        if (m_imagePath.empty())
            return;

        if (m_texture == nullptr)
        {
            // Load the image
            if (!load())
                return;
        }
        
        // Render the image
        SDL_Rect imageRect = getObjectRect();
        getRenderer()->renderTexture(m_texture, NULL, &imageRect);
        setHidden(false);
    }
    
    void Image::drawFlippedHorizontal(const double angle)
    {
        if (m_texture == nullptr)
        {
            // Load the image
            if (!load())
                return;
        }
            
        // Render the image, flipped
        SDL_Rect imageRect = getObjectRect();
        getRenderer()->renderTextureFlipped(m_texture, NULL, &imageRect, angle, NULL, SDL_FLIP_HORIZONTAL);
        setHidden(false);
    }
    
    bool Image::load()
    {
        // First, check if there was a previous image in use
        destroy();
        
        Renderer *render = getRenderer();
        if (render == nullptr)
        {
            LOG_ERROR("framework.gui", "In Image::load() - Unable to fetch the rendering object on an image. SDL Error: ", IMG_GetError());
            return false;
        }
        
        m_texture = render->createTextureFromImage(m_imagePath);
        if (m_texture == nullptr)
        {
            LOG_ERROR("framework.gui", "In Image::load() - Unable to create a texture provided an image path, \"",
                m_imagePath, "\". SDL Error: ", IMG_GetError());
            return false;
        }
        
        return true;
    }

    const std::string &Image::getImageFile() const
    {
        return m_imagePath;
    }
    
    void Image::setImageFile(std::string imagePath)
    {
        m_imagePath = imagePath;
        destroy();
    }

    void Image::setToNativeRatio()
    {
        if (!load())
            return;

        int width, height;

        if (SDL_QueryTexture(m_texture, NULL, NULL, &width, &height) != 0)
        {
            LOG_ERROR("framework.gui", "In Image::setToNativeRatio() - Unable to query the image texture for its size. SDL Error: ", SDL_GetError());
            return;
        }

        Position newPos = getPosition();
        Size newSize = getSize();

        if (width > height)
        {
            newSize.height = ((float)height / width) * newSize.width;
            newPos.y += std::abs(newSize.height - getSize().height) / 2;
        }
        else
        {
            newSize.width = ((float)width / height) * newSize.height;
            newPos.x += std::abs(newSize.width - getSize().width) / 2;
        }

        m_position = newPos;
        m_size = newSize;
    }
    
    void Image::setToNativeSize()
    {
        if (m_texture == nullptr)
            return;
            
        int width, height;
        
        if (SDL_QueryTexture(m_texture, NULL, NULL, &width, &height) != 0)
        {
            LOG_ERROR("framework.gui", "In Image::setToNativeSize() - Unable to query the image texture for its size. SDL Error: ", SDL_GetError());
        }
        else
        {
            m_size.width  = width;
            m_size.height = height;
        }
    }
}

