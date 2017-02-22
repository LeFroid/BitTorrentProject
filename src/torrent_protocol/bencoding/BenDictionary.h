/*
Copyright (c) 2017, Timothy Vaccarelli
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

#include <unordered_map>
#include "BenObject.h"

namespace bencoding
{
    /**
     * @class BenDictionary
     * @brief Object used to store key-value pairs of strings to objects of any bencoded type
     */
    class BenDictionary : public BenObject< std::unordered_map<std::string, BenObjectBase*> >
    {
    public:
        /// Default constructor
        BenDictionary();

        /// Implements the BenObject's accept method
        virtual void accept(BenObjectVisitor &visitor) override;

    /// Wrapper methods and type definitions for the map
    public:
        typedef std::unordered_map<std::string, BenObjectBase*>::iterator iterator;
        typedef std::unordered_map<std::string, BenObjectBase*>::const_iterator const_iterator;
        typedef std::unordered_map<std::string, BenObjectBase*>::size_type size_type;

        /// Returns an iterator pointing to the first element in the BenDictionary
        iterator begin();

        /// Returns a const_iterator pointing to the first element in the BenDictionary
        const_iterator cbegin();

        /// Returns a const_iterator pointing just past the end of the BenDictionary
        const_iterator cend();

        /// Returns an iterator pointing just past end of the BenDictionary
        iterator end();

        /// Returns true if the dictionary is empty, false if else
        bool empty() const;

        /// Returns the number of elements in the list
        size_type size() const;

        /// Bracket operator to access a pointer to the value associated with the given key, or
        /// it will return a reference to the newly inserted item if the key had not been found
        /// in the map
        BenObjectBase *&operator [](std::string key);
    };
}
