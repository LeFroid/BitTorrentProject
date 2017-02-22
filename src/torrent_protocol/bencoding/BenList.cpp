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

#include "BenList.h"
#include "BenObjectVisitor.h"

namespace bencoding
{
    BenList::BenList() :
        BenObject()
    {
    }

    void BenList::accept(BenObjectVisitor &visitor)
    {
        visitor.visit(*this);
    }

    BenList::iterator BenList::begin()
    {
        return m_value.begin();
    }

    BenList::const_iterator BenList::cbegin()
    {
        return m_value.cbegin();
    }

    BenList::iterator BenList::end()
    {
        return m_value.end();
    }

    BenList::const_iterator BenList::cend()
    {
        return m_value.cend();
    }

    bool BenList::empty() const
    {
        return m_value.empty();
    }

    void BenList::push_back(BenObjectBase* const &val)
    {
        m_value.push_back(val);
    }

    BenList::size_type BenList::size() const
    {
        return m_value.size();
    }

    BenObjectBase *&BenList::operator [](BenList::size_type pos)
    {
        return m_value[pos];
    }
}
