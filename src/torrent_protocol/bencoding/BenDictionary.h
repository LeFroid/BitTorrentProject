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

#include <memory>
#include <unordered_map>
#include "BenObject.h"

namespace bencoding
{
    /**
     * @class BenDictionary
     * @brief Object used to store key-value pairs of strings to objects of any bencoded type
     */
    class BenDictionary : public BenObject< std::unordered_map< std::string, std::shared_ptr<BenObjectBase> > >
    {
    public:
        /// Default constructor
        BenDictionary();

        /// Implements the BenObject's accept method
        virtual void accept(BenObjectVisitor &visitor) override;

    /// Wrapper methods and type definitions for the map
    public:
        typedef std::unordered_map< std::string, std::shared_ptr<BenObjectBase> > DataType;
        typedef DataType::iterator iterator;
        typedef DataType::const_iterator const_iterator;
        typedef DataType::size_type size_type;

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

        /**
         * Finds an element with key equivalent to key
         * @return Iterator to element with associated key, or
         *         a past-the-end iterator if the key was not found.
         */
        iterator find(const std::string &key);

        /**
         * Finds an element with key equivalent to key
         * @return Iterator to element with associated key, or
         *         a past-the-end iterator if the key was not found.
         */
        const_iterator find(const std::string &key) const;

        /**
         * @brief Inserts the key-value pair into the map 
         * @return A pair whose first element is an iterator pointing to either the
         *         newly inserted element, or the element whose key is equivalent, and
         *         a boolean value indicating whether or not the element was successfully
         *         inserted.
         */
        std::pair<iterator, bool> insert(const std::pair< const std::string, std::shared_ptr<BenObjectBase> > &val);

        /// Bracket operator to access a pointer to the value associated with the given key, or
        /// it will return a reference to the newly inserted item if the key had not been found
        /// in the map
        std::shared_ptr<BenObjectBase> &operator [](std::string key);
    };
}
