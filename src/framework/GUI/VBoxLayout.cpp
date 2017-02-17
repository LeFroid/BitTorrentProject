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

#include "VBoxLayout.h"
#include "ObjectManager.h"

namespace gui 
{
    VBoxLayout::VBoxLayout(GUIObject *parent, int spacing) :
        Layout(parent, spacing)
    {
    }
    
    VBoxLayout::VBoxLayout(GUIObject *parent, Position position, Size size, int spacing) :
        Layout(parent, position, size, spacing)
    {
    }
    
    void VBoxLayout::addItem(GUIObject *item)
    {
        if (item == nullptr)
            return;
        
        if (item->hasParent())
        {
            item->getParent()->removeChild(item->getObjectID());
        }
        
        addChild(item->getObjectID());
        update();
    }
    
    void VBoxLayout::update()
    {
        std::size_t numChildren = getNumChildren();
        
        const Margins& layoutMargins = getMargins();
        const Position& layoutPosition = getPosition();
        const Size& layoutSize = getSize();
        int spacing = getSpacing();
        
        // Same logic as HBoxLayout::update(...) except x and y, width and height reversed
        int childWidth = layoutSize.width - layoutMargins.Left - layoutMargins.Right;
        int childDefaultHeight = ( (layoutSize.height - layoutMargins.Top - layoutMargins.Bottom - spacing) / numChildren );
        
        int xLeftBound  = layoutPosition.x + layoutMargins.Left;
        int xRightBound = layoutPosition.x + layoutSize.width - layoutMargins.Right;
        
        GUIObject *child;
        ObjectManager *objMgr = ObjectManager::getInstance();
        
        // Keep track of excess space in case one object is < default height and another one requires extra height
        int excessHeight = getExcessHeight();
        
        // If a child object's size is <= default, it can remain. If not, it is changed
        for (int i = 0; i < numChildren; ++i)
        {
            child = objMgr->getObject(m_children.at(i));
            if (child == nullptr)
                continue;
                
            Position childPos = child->getPosition();
            Size childSize = child->getSize();
            
            // Confirm height
            if (childSize.height > childDefaultHeight + excessHeight)
            {
                childSize.height = childDefaultHeight + excessHeight;
                excessHeight = 0;
            }
                
            // Confirm size
            if (childSize.width > childWidth)
                childSize.width = childWidth;
                
            // Set the position
            if ((childPos.x < xLeftBound) || (childPos.x + childSize.width > xRightBound))
                childPos.x = xLeftBound;
            
            if (i == 0)
                childPos.y = layoutPosition.y + layoutMargins.Top;
            else
            {
                GUIObject *priorObj = objMgr->getObject(m_children.at(i-1));
                if (priorObj != nullptr)
                    childPos.y = priorObj->getPosition().y + priorObj->getSize().height + getSpacing();
                else
                    childPos.y = layoutPosition.y + (i * childDefaultHeight);
            }
                
            child->setPosition(childPos);
            child->setSize(childSize);
        }
    }
    
    int VBoxLayout::getExcessHeight()
    {
        int excess = 0;
        
        size_t numChildren = getNumChildren();
        const Margins& layoutMargins = getMargins();
        const Size& layoutSize = getSize();
        int spacing = getSpacing();
        int childDefaultHeight = ( (layoutSize.height - layoutMargins.Top - layoutMargins.Bottom - spacing) / numChildren );
        ObjectManager *objMgr = ObjectManager::getInstance();
        
        for (int i = 0; i < numChildren; ++i)
        {
            GUIObject *child = objMgr->getObject(m_children.at(i));
            if (child == nullptr)
                continue;

            Size childSize = child->getSize();
            
            if (childSize.height < childDefaultHeight)
                excess += (childDefaultHeight - childSize.height);
        }
        
        return excess;
    }
}

