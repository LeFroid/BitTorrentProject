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

#include "FontHolder.h"
#include "GUIObject.h"

namespace gui 
{
    /**
     * @class Button
     * @author Timothy Vaccarelli
     * @date 01/11/2016
     * @file Button.h
     * @brief Represents a button that can display text and be clicked, hovered over, etc by the user
     */
    class Button : public GUIObject, public FontHolder
    {
    public:
        /// Constructs a button with a given parent, position and size. Does not set color, text, font, etc.
        Button(GUIObject *parent, Position position, Size size);
        
        /**
         * @brief 
         * @param parent The parent object
         * @param position The position of the button
         * @param size The size of the button
         * @param buttonColor The background color of the button
         * @param textColor The color of the text on the button (if any). Defaults to black
         * @param font The font of the text on the button (if any). Defaults to droid sans
         * @praam fontPointSize The text's font point size. Defaults to 24
         * @param text The text on the button (if any). Defaults to an empty string
         */
        Button(GUIObject *parent, Position position, Size size, Color buttonColor, Color textColor = { 0, 0, 0, 255 },
            FontType fontType = DROID_SANS, int fontPointSize = 24, std::string text = std::string());
        
        /// Button destructor. Deallocates any resources that were used up
        virtual ~Button();
                
        /// Returns the color of the button
        const Color& getColor() const;
        
        /// Returns the color of the button, when the mouse is hovering above it
        const Color& getMouseHoverColor() const;

        /// Returns the path of the button's associated image, or an empty string if none
        const std::string& getImagePath() const;
        
        /// Returns a reference to the text contained within the button
        const std::string& getText() const;
        
        /// Returns a reference to the color of the text within the button
        const Color& getTextColor() const;
        
        /// If an image was added to the button, this removes the image
        void removeImage();
        
        /// Sets the color of the button to that of the parameter
        void setColor(Color buttonColor);
        
        /// Adds an image file to the button, either as the entire appearance of the button or only covering a certain area
        void setImage(std::string imageFile, Position imagePos, Size imageSize);
        
        /// Defines a color to be assigned to the button when the mouse has hovered above the button
        void setMouseHoverColor(Color hoverColor);
        
        /// Sets the text to be displayed within the button
        void setText(std::string text);
        
        /// Sets the color of the text in the button
        void setTextColor(Color textColor);
        
    public:
        /// Attempts to draw the button onto the screen
        virtual void draw() override;
        
        /// Deallocates any resources that were used by the button
        virtual void destroy() override;
        
    protected:
        /// Informs the button that it has gained focus from the mouse
        void informGainOfMouseFocus();
        
        /// Informs the button that it has lost focus from the mouse
        void informLossOfMouseFocus();
        
    protected:
        /// The rgba colors for the button, when its in its normal state, and (optionally) when the mouse has hovered over it
        Color m_buttonColor, m_buttonHoverColor;
        
        /// The rgba color of the button's text, if there is any
        Color m_textColor;

        /// True if the mouse is hovering over this object, false if else
        bool m_isMouseHovering;
        
        /// The text, if any, that is shown on the button
        std::string m_text;
        
        /// True if the button displays an image on it
        bool m_hasImage;
        
        /// If the button displays an image, this represents the file path of said image
        std::string m_imagePath;
        
        /// The rectangle that the button's image, if it has one, is bounded by
        SDL_Rect m_imageRect;
        
        /// If the button displays an image, this contains a pointer to its texture
        SDL_Texture *m_imageTexture;
    };
}

