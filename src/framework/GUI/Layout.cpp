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

#include "Layout.h"
#include "ObjectManager.h"

namespace gui 
{
    Layout::Layout(GUIObject *parent, int spacing) :
        GUIObject(parent),
        m_margins(),
        m_spacing(spacing)
    {
        if (parent != nullptr)
        {
            setPosition(parent->getPosition());
            setSize(parent->getSize());
        }
    }
    
    Layout::Layout(GUIObject *parent, Position position, Size size, int spacing) :
        GUIObject(parent, position, size),
        m_margins(),
        m_spacing(spacing)
    {
    }
    
    GUIObject *Layout::getItemAt(int index)
    {
        if (index + 1 > m_children.size())
            return nullptr;
        
        return ObjectManager::getInstance()->getObject(m_children.at(index));
    }
            
    const Margins& Layout::getMargins() const
    {
        return m_margins;
    }
    
    const int& Layout::getSpacing() const
    {
        return m_spacing;
    }
    
    void Layout::setMargins(Margins margins)
    {
        m_margins = margins;
        
        if (!m_children.empty())
            update();
    }
    
    void Layout::setMargins(int top, int bottom, int left, int right)
    {
        m_margins = { top, bottom, left, right };
        
        if (!m_children.empty())
            update();
    }
    
    void Layout::setSpacing(int spacing)
    {
        m_spacing = spacing;
        
        if (!m_children.empty())
            update();
    }
    
    void Layout::draw()
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        
        for (auto itr = getChildIterBegin(); itr != getChildIterEnd(); ++itr)
        {
            GUIObject *child = objMgr->getObject(*itr);
            if (child != nullptr)
            {
                child->draw();
            }
        }
        
        setHidden(false);
    }
}

