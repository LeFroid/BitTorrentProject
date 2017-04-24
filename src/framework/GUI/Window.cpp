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

#include "GUIObject.h"
#include "LogHelper.h"
#include "ObjectManager.h"
#include "Renderer.h"
#include "Window.h"

namespace gui 
{
    Window::Window(std::string title, int width, int height, bool fullscreen) :
        GUIObject(nullptr, Position(0, 0), Size(width, height)),
        m_title(title),
        m_window(nullptr),
        m_renderer(),
        m_childWithKeyboardFocus(0)
    {
        m_flags = (fullscreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE;
        
        if (fullscreen)
        {
            SDL_DisplayMode displayMode;
            SDL_GetCurrentDisplayMode(0, &displayMode);
            setSize({ displayMode.w, displayMode.h - 1});
        }
    }
    
    Window::~Window()
    {
        destroy();
    }
    
    bool Window::childHasKeyboardFocus()
    {
        return (getChildWithKeyboardFocus() != 0);
    }
    
    bool Window::clearContent()
    {
        return m_renderer.get()->clearContent();
    }
    
    const GUIObjectID& Window::getChildWithKeyboardFocus() const
    {
        return m_childWithKeyboardFocus;
    }
    
    void Window::destroy()
    {
        m_renderer.get()->destroy();
            
        if (m_window != nullptr)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }
    
    void Window::draw()
    {
        m_window = SDL_CreateWindow(m_title.c_str(), m_position.x, m_position.y, m_size.width, m_size.height, m_flags);
        
        if (m_window == nullptr)
            LOG_ERROR("framework.gui", "Unable to create a window! SDL Error: ", SDL_GetError());
        else
            m_renderer = std::make_shared<Renderer>(m_window);
            
        setHidden(false);
    }
    
    void Window::redraw(bool scaleSize, Size oldSize)
    {
        if (!m_renderer->clearContent())
            LOG_WARNING("framework.gui", "In Window::redraw - Unable to successfully call Renderer::clearContent(). SDL Error: ", SDL_GetError());
        
        ObjectManager *objMgr = ObjectManager::getInstance();

        const Size &winSize = getSize();
        float widthRatio = (float)winSize.width / oldSize.width;
        float heightRatio = (float)winSize.width / oldSize.width;

        for (auto i : m_children)
        {
            GUIObject *child = objMgr->getObject(i);
            if (child == nullptr)
                continue;

            if (scaleSize)
                changeScale(child, widthRatio, heightRatio);

            if (child->isHidden())
            {
                // See if any children are not hidden
                redraw(child);
                continue;
            }

            if (child->getSize() == winSize)
                child->setSize(winSize);
            
            redraw(child);
            child->hide();
            child->draw();
        }
        getRenderer()->update();
    }
    
    void Window::redraw(GUIObject *object)
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        
        for (auto i = object->getChildIterBegin(); i != object->getChildIterEnd(); ++i)
        {
            GUIObject *child = objMgr->getObject(*i);
            if (child == nullptr)
                continue;
                
            if (child->isHidden())
            {
                redraw(child);
                continue;
            }
            
            redraw(child);
            child->hide();
            child->draw();
        }
    }

    void Window::changeScale(GUIObject *object, const float &widthRatio, const float &heightRatio)
    {
        ObjectManager *objMgr = ObjectManager::getInstance();

        for (auto i = object->getChildIterBegin(); i != object->getChildIterEnd(); ++i)
        {
            GUIObject *child = objMgr->getObject(*i);
            if (child == nullptr)
                continue;

            Position newPos(child->getPosition().x * widthRatio, child->getPosition().y * heightRatio);
            Size newSize(child->getSize().width * widthRatio, child->getSize().height * heightRatio);

            child->setPosition(newPos, false);
            child->setSize(newSize, false);

            changeScale(child, widthRatio, heightRatio);
        }
    }
    
    void Window::setKeyboardFocusObject(GUIObjectID objectID)
    {
        m_childWithKeyboardFocus = objectID;
    }

    Renderer *Window::getRenderer()
    {
        return m_renderer.get();
    }
    
    void Window::handleEvent(SDL_WindowEvent& event)
    {
        switch (event.event)
        {
            case SDL_WINDOWEVENT_RESIZED:
            {
                const Size &oldSize = getSize();
                setSize({ event.data1, event.data2 });
                redraw(true, oldSize);
                break;
            }
            case SDL_WINDOWEVENT_EXPOSED:
                redraw();
                break;
            default:
                break;
        }
    }
    
    void Window::relinquishKeyboardFocus()
    {
        setKeyboardFocusObject(0);
    }
    
    void Window::requestKeyboardFocus(GUIObjectID objectID)
    {
        //if (!childHasKeyboardFocus())
        //LOG_DEBUG("framework", "Object ", objectID, " requested kb focus");
        setKeyboardFocusObject(objectID);
    }
    
    void Window::setBackgroundColor(Color bgColor)
    {
        Renderer *render = getRenderer();
        if (render == nullptr)
            return;
            
        render->drawBox(getSize(), getPosition(), bgColor);
    }

    void Window::removeChildrenExcept(GUIObjectID child)
    {
        ObjectManager *objMgr = ObjectManager::getInstance();
        GUIObject *childObj = objMgr->getObject(child);
        if (childObj)
            childObj->setParent(this);

        for (const auto &it : m_children)
        {
            if (it == child)
                continue;

            objMgr->deleteObject(it);
        }
    }
}

