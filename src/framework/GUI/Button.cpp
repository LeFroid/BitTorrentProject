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

#include "Button.h"
#include "Renderer.h"
#include "LogHelper.h"

namespace gui 
{
    Button::Button(GUIObject *parent, Position position, Size size) :
        GUIObject(parent, position, size),
        FontHolder(),
        m_buttonColor(),
        m_textColor(),
        m_isMouseHovering(false),
        m_text(),
        m_hasImage(false),
        m_imagePath(),
        m_imageRect(),
        m_imageTexture(nullptr)
    {
    }
    
    Button::Button(GUIObject *parent, Position position, Size size, Color buttonColor, Color textColor,
        FontType fontType, int fontPointSize, std::string text) :
            GUIObject(parent, position, size),
            FontHolder(fontType, fontPointSize),
            m_buttonColor(buttonColor),
            m_textColor(textColor),
            m_isMouseHovering(false),
            m_text(text),
            m_hasImage(false),
            m_imagePath(),
            m_imageRect(),
            m_imageTexture(nullptr)
    {
    }
    
    Button::~Button()
    {
        destroy();
    }
        
    const Color& Button::getColor() const
    {
        return m_buttonColor;
    }
    
    const Color& Button::getMouseHoverColor() const
    {
        return m_buttonHoverColor;
    }

    const std::string& Button::getImagePath() const
    {
        return m_imagePath;
    }
    
    const std::string& Button::getText() const
    {
        return m_text;
    }
        
    const Color& Button::getTextColor() const
    {
        return m_textColor;
    }
    
    void Button::removeImage()
    {
        destroy();
        m_hasImage = false;
    }
        
    void Button::setColor(Color buttonColor)
    {
        m_buttonColor = buttonColor;
    }

    void Button::setImage(std::string imageFile, Position imagePos, Size imageSize)
    {
        // If there was a prior image, delete it
        destroy();
        
        // Make sure size & position are valid
        const Position &buttonPos = getPosition();
        const Size &buttonSize    = getSize();
        
        int fullButtonWidth  = buttonPos.x + buttonSize.width;
        int fullButtonHeight = buttonPos.y + buttonSize.height;
        
        if (imagePos.x < buttonPos.x)
            imagePos.x = buttonPos.x;
        if (imagePos.y < buttonPos.y)
            imagePos.y = buttonPos.y;
        
        if (imagePos.x + imageSize.width > fullButtonWidth)
            imageSize.width -= imageSize.width + imagePos.x - fullButtonWidth;
        if (imagePos.y + imageSize.height > fullButtonHeight)
            imageSize.height -= imageSize.height + imagePos.y - fullButtonHeight;
            
        m_imageRect = { imagePos.x, imagePos.y, imageSize.width, imageSize.height };
        m_imagePath = imageFile;
        m_hasImage = true;
    }
    
    void Button::setMouseHoverColor(Color hoverColor)
    {
        m_buttonHoverColor = hoverColor;
        
        if (!hasMouseEvent(MouseEvent::Moved))
        {
            registerMouseEvent(MouseEvent::Moved, std::bind(&Button::informGainOfMouseFocus, this));
            registerMouseEvent(MouseEvent::LostFocus, std::bind(&Button::informLossOfMouseFocus, this));
        }
    }
        
    void Button::setText(std::string text)
    {
        m_text = text;
    }
        
    void Button::setTextColor(Color textColor)
    {
        m_textColor = textColor;
    }
    
    void Button::draw()
    {
        Renderer *render = getRenderer();
        
        // Draw the button rectangle with a black border, making the radius default to 4
        if (!render->drawRoundedBox(getSize(), getPosition(), getColor(), 4))
        {
            LOG_ERROR("framework.gui", "In Button::draw() - Unable to draw the rounded rectangle for a button. SDL Error: ", SDL_GetError());
            return;
        }
        
        // Temporarily calculate a button border and then render it. this will be changed in the future
        Color border(255-m_buttonColor.red, 255-m_buttonColor.green, 255-m_buttonColor.blue, 255);
        if (!render->drawRoundedRectangle(getSize(), getPosition(), border, 4))
        {
            LOG_WARNING("framework.gui", "In Button::draw() - Unable to draw a border for a button. SDL Error: ", SDL_GetError());
        }

        // Draw the image, if applicable
        if (m_hasImage)
        {
            //destroy();
            if (!m_imageTexture)
                m_imageTexture = render->createTextureFromImage(m_imagePath);

            if (m_imageTexture == nullptr)
                LOG_WARNING("framework.gui", "In Button::draw() - Unable to load the image file that was specified. SDL Error: ", SDL_GetError());
            else
                render->renderTexture(m_imageTexture, NULL, &m_imageRect);
        }
        
        // Draw the text, if applicable
        Font *font = getFont();
        if (!font || getText().empty())
        {
            setHidden(false);
            return;
        }
        
        SDL_Rect textRect = getObjectRect();
        textRect.x += (textRect.w/16);
        textRect.w -= (textRect.w/8);
        textRect.y += (textRect.h/8);
        textRect.h -= (textRect.h/8);
        getFont()->printText(getText(), getTextColor(), getFontPointSize(), textRect, getRenderer());
        
        setHidden(false);
    }
    
    void Button::destroy()
    {
        if (m_imageTexture != nullptr)
        {
            SDL_DestroyTexture(m_imageTexture);
            m_imageTexture = nullptr;
        }
    }
    
    void Button::informGainOfMouseFocus()
    {
        if (!m_isMouseHovering && !isHidden())
        {
            Color tmpColor = m_buttonColor;
            
            setColor(m_buttonHoverColor);
            draw();
            getRenderer()->update();
            
            setColor(tmpColor);
            
            // Parent object sets this to false when we lose focus
            m_isMouseHovering = true;
        }
    }
    
    void Button::informLossOfMouseFocus()
    {
        if (m_isMouseHovering && !isHidden())
        {
            draw();
            getRenderer()->update();
            m_isMouseHovering = false;
        }
    }
}

