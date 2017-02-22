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

namespace bencoding
{
    class BenObjectVisitor;

    /**
     * @brief The BenObjectBase class is used as a base class of BenObject so
     *        the BenList and BenDictionary classes can store pointers to
     *        BenObject-derived classes.
     */
    class BenObjectBase
    {
    public:
        BenObjectBase() = default;
    };

    /**
     * @class BenObject
     * @brief Base class of all bencoded data types
     */
    template <class T>
    class BenObject : public BenObjectBase
    {
    public:
        /// Default constructor
        BenObject() : BenObjectBase(), m_value() { }

        /// Instantiates the BenObject with the given value
        explicit BenObject(T value) : m_value(value) { }

        /// Returns the value associated with the BenObject
        const T &getValue() const { return m_value; }

        /// Sets the value associated with the BenObject
        void setValue(T value) { m_value = value; }

    public:
        /// Accepts an object of any class derived from the \ref BenObjectVisitor and
        /// must be implemented in all derived classes
        virtual void accept(BenObjectVisitor &visitor) = 0;

    protected:
        /// The value associated with the BenObject-derived class
        T m_value;
    };
}
