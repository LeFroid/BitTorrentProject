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

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "GUIObject.h"
#include "Singleton.h"

namespace gui 
{
    typedef std::unordered_map< uint32_t, std::shared_ptr<GUIObject> > ObjectRegistry;
    
    /**
     * @class ObjectManager
     * @author Timothy Vaccarelli
     * @date 01/19/2016
     * @file ObjectManager.h
     * @brief Manages the memory and usage of objects derived from the GUIObject base class
     */
    class ObjectManager : public Singleton<ObjectManager>
    {
        friend class Singleton<ObjectManager>;
        
    public:
        /// Default constructor
        ObjectManager();

        /// Destructor
        ~ObjectManager();

        /**
         * @brief Creates and registers a new GUIObject derived object
         * @return A new object of the type specified
         */
        template<class T, typename... Args>
        T* createObject(Args&&... args)
        {
            static_assert(std::is_base_of<GUIObject, T>::value, "ObjectManager::createObject - Must pass an object that is derived from GUIObject");

            T *obj = dynamic_cast<T*>(registerObject(std::make_shared<T>(std::forward<Args>(args)...)));
            //T *obj = static_cast<T*>(registerObject(std::unique_ptr<T>(new T(std::forward<Args>(args)...))));
            if (obj)
            {
                obj->registerWithParent();
                return obj;
            }
            return nullptr;
        }
        
        /// Deletes an object of a certain identifier, if it exists in the registry
        void deleteObject(uint32_t objectID);
        
        /// Returns true if an object exists with the provided ID, false if else
        bool existsObject(uint32_t objectID);
        
        /// Returns the pointer to an object of a given ID, if it exists in the registry. Returns nullptr if else
        GUIObject *getObject(uint32_t objectID);
    
    private:
        /// Informs the object manager that a new object has been created, adding it to the registry. May only be called once per object
        GUIObject *registerObject(std::shared_ptr<GUIObject> &&object);

        /// Contains and essentially manages pointers to objects, storing them by their unique ID
        ObjectRegistry m_objectRegistry;
        
        /// The counter for assigning object IDs to new objects
        uint32_t m_objectCounter;

        /// True if ObjectManager began to destroy its children, false if else
        bool m_deleting;
    };
}

