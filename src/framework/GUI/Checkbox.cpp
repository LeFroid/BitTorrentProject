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

#include "Checkbox.h"
#include "LogHelper.h"
#include "Renderer.h"

namespace gui 
{
    Checkbox::Checkbox(GUIObject *parent, Position position, Size size, Color color, Checkbox::Mode mode) :
        gui::Box(parent, position, size, color),
        m_isChecked(false),
        m_mode(mode),
        m_radius(0)
    {
        // Pick the largest side if we weren't given a square, for the circle radius
        if (mode == Circle)
            setRadius();
        
        registerMouseEvent(MouseEvent::ReleasedLeft, std::bind(&Checkbox::onMouseClick, this));
    }
    
    const Checkbox::Mode& Checkbox::getDisplayMode() const
    {
        return m_mode;
    }
    
    bool Checkbox::isChecked() const
    {
        return m_isChecked;
    }
    
    void Checkbox::setChecked(bool checked)
    {
        m_isChecked = checked;
    }
    
    void Checkbox::setDisplayMode(Checkbox::Mode displayMode)
    {
        m_mode = displayMode;
        
        if (displayMode == Circle)
            setRadius();
    }
    
    void Checkbox::setRadius()
    {
        Size objSize = getSize();
        m_radius = (objSize.width >= objSize.height) ? ((float)objSize.width / 2.0) : ((float)objSize.height / 2.0);
    }
    
    void Checkbox::draw()
    {
        Renderer *render = getRenderer();
        if (render == nullptr)
        {
            LOG_ERROR("framework.gui", "In Checkbox::draw() - Unable to get the checkbox's rendering object!");
            return;
        }
        
        const Color &checkboxColor = getColor();
        const Size &objSize = getSize();
        const Position &objPos = getPosition();
        
        if (getDisplayMode() == Circle)
        {
            Position innerCir(objPos.x + objSize.width / 7, objPos.y + objSize.height / 7);
            // Draw a white inner circle with a slightly larger black circle around it
            if (!render->drawCircle(m_radius, objPos, { 0, 0, 0, 255 })
                || !render->drawCircle(0.85 * m_radius, innerCir, checkboxColor))
            {
                LOG_ERROR("framework.gui", "In Checkbox::draw() - Unable to draw the circle for a checkbox. SDL Error: ", SDL_GetError());
                return;
            }
            
            if (isChecked())
            {
                float centerRad = m_radius / 3.0;
                Position centerPos(objPos.x + m_radius / 1.3, objPos.y + m_radius / 1.3);
                if (!render->drawCircle(centerRad, centerPos, { 0, 0, 0, 255 }))
                {
                    LOG_ERROR("framework.gui", "In Checkbox::draw() - Unable to draw the circle for a checkbox. SDL Error: ", SDL_GetError());
                    return;
                }
            }
        }
        else
        {
            // Draw a white box with a black outline
            Color black(0, 0, 0, 255);
            if (!render->drawBox(objSize, objPos, checkboxColor)
                || !render->drawRectOutline(objSize, objPos, black))
            {
                LOG_ERROR("framework.gui", "In Checkbox::draw() - Unable to draw a checkbox. SDL Error: ", SDL_GetError());
                return;
            }
            
            // Draw an 'X' inside the box if been checked
            if (isChecked())
            {
                // First: drawLine(Top Left Corner to Bottom Right Corner), Then: drawLine(Bottom Left Corner to Top Right Corner)
                if (!render->drawLine( { objPos.x + 1, objPos.y + 1 }, { objPos.x + objSize.width - 1, objPos.y + objSize.height - 1}, black)
                    || !render->drawLine( { objPos.x + 1, objPos.y + objSize.height - 1 }, { objPos.x + objSize.width - 1, objPos.y + 1}, black))
                {
                    LOG_ERROR("framework.gui", "In Checkbox::draw() - Unable to draw the 'X' in a checkbox. SDL Error: ", SDL_GetError());
                    return;
                }
            }
        }
        
        setHidden(false);
    }
    
    void Checkbox::onMouseClick()
    {
        bool currentVal = isChecked();
        setChecked((!currentVal));
        if (!isHidden())
        {
            draw();
            getRenderer()->update();
        }
    }
}

