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

#include <utility>
#include "BenDictionary.h"
#include "BenObjectVisitor.h"

namespace bencoding
{
    BenDictionary::BenDictionary() :
        BenObject()
    {
    }

    void BenDictionary::accept(BenObjectVisitor &visitor)
    {
        visitor.visit(*this);
    }

    BenDictionary::iterator BenDictionary::begin()
    {
        return m_value.begin();
    }

    BenDictionary::const_iterator BenDictionary::cbegin()
    {
        return m_value.cbegin();
    }

    BenDictionary::const_iterator BenDictionary::cend()
    {
        return m_value.cend();
    }

    BenDictionary::iterator BenDictionary::end()
    {
        return m_value.end();
    }

    bool BenDictionary::empty() const
    {
        return m_value.empty();
    }

    BenDictionary::size_type BenDictionary::size() const
    {
        return m_value.size();
    }

    std::pair<BenDictionary::iterator, bool> BenDictionary::insert(const std::pair< const std::string, std::shared_ptr<BenObjectBase> > &val)
    {
        return m_value.insert(val);
    }

    std::shared_ptr<BenObjectBase> &BenDictionary::operator [](std::string key)
    {
        auto it = m_value.find(key);
        if (it == m_value.end())
        {
            auto pair = m_value.insert(std::make_pair< std::string, std::shared_ptr<BenObjectBase> >(std::move(key), std::shared_ptr<BenObjectBase>()));
            return pair.first->second;
        }
        return it->second;
    }
}
