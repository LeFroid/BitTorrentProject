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


#include "Box.h"
#include "LogHelper.h"
#include "ObjectManager.h"
#include "Renderer.h"

namespace gui 
{
    Box::Box(GUIObject *parent, Position position, Size size, Color color) :
        GUIObject(parent, position, size),
        m_color(color),
        m_hoverColor(),
        m_isMouseHovering(false)
    {
    }
    
    void Box::draw()
    {
        Renderer *render = getRenderer();
        if (render == nullptr)
        {
            LOG_WARNING("framework.gui", "In Box::draw() - Unable to fetch the rendering object on a box. SDL Error: ", SDL_GetError());
            return;
        }
            
        if (!render->drawBox(getSize(), getPosition(), getColor()))
            LOG_ERROR("framework.gui", "In Box::draw() - Unable to have the renderer draw a box. SDL Error: ", SDL_GetError());

        ObjectManager *objMgr = ObjectManager::getInstance();
        for (const auto& id : m_children)
        {
            GUIObject *obj = objMgr->getObject(id);
            if (obj && !obj->isHidden())
                obj->draw();
        }

        setHidden(false);
    }
    
    const Color& Box::getColor() const
    {
        return m_color;
    }
    
    const Color& Box::getMouseHoverColor() const
    {
        return m_hoverColor;
    }
    
    void Box::setColor(Color boxColor)
    {
        m_color = boxColor;
    }
    
    void Box::setMouseHoverColor(Color hoverColor)
    {
        m_hoverColor = hoverColor;
        
        if (!hasMouseEvent(MouseEvent::Moved))
        {
            registerMouseEvent(MouseEvent::Moved, std::bind(&Box::informGainOfMouseFocus, this));
            registerMouseEvent(MouseEvent::LostFocus, std::bind(&Box::informLossOfMouseFocus, this));
        }
    }
    
    void Box::informGainOfMouseFocus()
    {
        if (!m_isMouseHovering && !isHidden())
        {
            Color tmpColor = m_color;
            
            setColor(m_hoverColor);
            draw();
            
            // Draw children as well
            for (auto i = getChildIterBegin(); i != getChildIterEnd(); ++i)
            {
                GUIObject *obj = ObjectManager::getInstance()->getObject(*i);
                if (obj == nullptr)
                    continue;
                
                if (!obj->isHidden())
                    obj->draw();
            }
            
            getRenderer()->update();
            
            setColor(tmpColor);
            
            // Parent object sets this to false when we lose focus
            m_isMouseHovering = true;
        }
    }
    
    void Box::informLossOfMouseFocus()
    {
        if (m_isMouseHovering && !isHidden())
        {
            draw();
            
            // Draw children as well
            for (auto i = getChildIterBegin(); i != getChildIterEnd(); ++i)
            {
                GUIObject *obj = ObjectManager::getInstance()->getObject(*i);
                if (obj == nullptr)
                    continue;
                
                if (!obj->isHidden())
                    obj->draw();
            }
            getRenderer()->update();
            m_isMouseHovering = false;
        }
    }
}

