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

#include "GUIObject.h"

#include <string>

namespace gui 
{
    /**
     * @class Image
     * @author Timothy Vaccarelli
     * @date 01/09/2016
     * @file Image.h
     * @brief Represents an image file that is mapped on to the GUI
     */
    class Image : public GUIObject
    {
    public:
        /// Image constructor, specifying only a parent object. The position defaults to (0,0), size defaults to image size.
        Image(GUIObject *parent = nullptr);
        
        /**
         * Image constructor, specifying a parent object, position, size and the path to an image file.
         * @param parent The parent object
         * @param imagePath A string that reflects the path of the image file
         * @param position The position, relative to the master parent, of the image. Defaults to (0,0)
         * @param size The size of the image (the size that will be displayed). Defaults to the actual size of the image
         */
        Image(GUIObject *parent, Position position = Position(), Size size = Size(), std::string imagePath = std::string(), bool overrideBounds = false);
        
        /// Image destructor, deallocates all used resources
        virtual ~Image();
        
        /// Attempts to load the image into a texture, returning true on success.
        bool load();

        /// Returns a string representing the full path of the image file being used
        const std::string &getImageFile() const;
        
        /// Sets this image data to the image at the path specified
        void setImageFile(std::string imagePath);
        
        /// Tells the object to keep (as best as possible) its native width:height ratio before drawing the image
        void setToNativeRatio();

        /// Sets the dimensions of this image to that of its native resolution. Must be called after the image has been loaded
        void setToNativeSize();
        
        /// Attempts to create and draw the image, flipped horizontally at a given angle
        void drawFlippedHorizontal(const double angle = 360);
        
    public:
        /// Attempts to create and draw the image
        virtual void draw() override;
        
        /// Deallocates all resources used to render the image
        virtual void destroy() override;
        
    private:
        /// The image file's path in the filesystem
        std::string m_imagePath;
        
        /// The SDL Texture that contains the image data
        SDL_Texture *m_texture;
    };
}

