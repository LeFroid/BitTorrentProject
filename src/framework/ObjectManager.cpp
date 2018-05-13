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

#include "ObjectManager.h"
#include "LogHelper.h"

namespace gui 
{
    ObjectManager::ObjectManager() :
        m_objectRegistry(),
        m_objectCounter(0),
        m_deleting(false)
    {
    }

    ObjectManager::~ObjectManager()
    {
        m_deleting = true;
    }
    
    void ObjectManager::deleteObject(uint32_t objectID)
    {
        if (m_deleting)
            return;

        auto it = m_objectRegistry.find(objectID);
        if (it != m_objectRegistry.end())
        {
            GUIObject *obj = it->second.get();
            if (obj)
                m_objectRegistry.erase(it);
            //GUIObject *obj = it->second.release();
            //if (obj != nullptr)
            //    delete obj;
            //m_objectRegistry.erase(it);
        }
    }
    
    bool ObjectManager::existsObject(uint32_t objectID)
    {
        return (m_objectRegistry.find(objectID) != m_objectRegistry.end());
    }
    
    GUIObject *ObjectManager::getObject(uint32_t objectID)
    {
        auto it = m_objectRegistry.find(objectID);
        if (it != m_objectRegistry.end())
            return it->second.get();
        
        return nullptr;
    }
    
    GUIObject *ObjectManager::registerObject(std::shared_ptr<GUIObject> &&object)
    {
        object->m_objectID = m_objectCounter;
        m_objectRegistry[m_objectCounter] = std::move(object);
        return m_objectRegistry[m_objectCounter++].get();
    }
}

