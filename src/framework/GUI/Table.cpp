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

#include <iterator>
#include <stdexcept>

#include "Box.h"
#include "HBoxLayout.h"
#include "LogHelper.h"
#include "ObjectManager.h"
#include "VBoxLayout.h"

#include "Table.h"

namespace gui 
{
    Table::Table(GUIObject *parent, Position position, Size size, int numRows, int numCols, int spacing, Margins margins) :
        GUIObject(parent, position, size),
        m_numColumns(numCols),
        m_numRows(),
        m_spacing(spacing),
        m_margins(margins),
        m_tableItems(),
        m_vBoxLayout(nullptr)
    {
        m_vBoxLayout = ObjectManager::getInstance()->createObject<VBoxLayout>(this, getPosition(), getSize());
        m_vBoxLayout->setSpacing(m_spacing);
        m_vBoxLayout->setMargins(m_margins);
        setNumRows(numRows);
    }
    
    void Table::setDimensions(int rows, int columns)
    {
        setNumRows(rows);
        setNumColumns(columns);
    }
    
    void Table::setNumColumns(int count)
    {
        if (count < 0)
            return;
        
        // Correct the table
        for (auto rowItr = m_tableItems.begin(); rowItr != m_tableItems.end(); ++rowItr)
        {
            // The case of removing extra columns
            if (rowItr->size() > count)
            {
                for (int col = rowItr->size(); col > count; --col)
                {
                    GUIObjectID columnObject = rowItr->at(col - 1);
                    m_vBoxLayout->getItemAt(col - 1)->removeChild(columnObject);
                    ObjectManager::getInstance()->deleteObject(columnObject);
                }
            }
            // Having to add columns
            else
            {
                Size tableSize = getSize();
                Size rowSize(tableSize.width, (int)(tableSize.height / count));
                Size colSize((int)(tableSize.width / getNumColumns()), rowSize.height);
            
                HBoxLayout *columnLayout = 
                    static_cast<HBoxLayout*>(m_vBoxLayout->getItemAt(std::distance(m_tableItems.begin(), rowItr)));
                
                for (int col = rowItr->size(); col < count; ++col)
                {
                    Box *box = ObjectManager::getInstance()->createObject<Box>(this, columnLayout->getPosition(), colSize, 
                        Color(255, 255, 255, 255));
                    columnLayout->addItem(box);
                    rowItr->push_back(box->getObjectID());
                }
            }
            
            rowItr->resize(count);
        }
        
        m_numColumns = count;
    }
    
    void Table::setNumRows(int count)
    {
        if (count < 0)
            return;
        
        // Correct the table
        if (m_tableItems.size() > count)
        {
            for (int row = m_tableItems.size(); row > count; --row)
            {
                // Deleting the HBoxLayout will delete its children
                HBoxLayout *columnLayout = static_cast<HBoxLayout*>(m_vBoxLayout->getItemAt(row - 1));
                if (columnLayout != nullptr)
                    ObjectManager::getInstance()->deleteObject(columnLayout->getObjectID());
            }
        }
        else if (m_tableItems.size() < count)
        {
            Size tableSize = getSize();
            Size rowSize(tableSize.width, (int)(tableSize.height / count));
            Size colSize((int)(tableSize.width / getNumColumns()), rowSize.height);

            int numNeededRows = count - m_tableItems.size();
            
            // Create boxes to add to m_tableItems & HBoxLayouts, HBoxLayouts to add to m_vBoxLayout
            for (int i = 0; i < numNeededRows; ++i)
            {
                HBoxLayout *row = ObjectManager::getInstance()->createObject<HBoxLayout>(this,
                    Position(m_vBoxLayout->getPosition().x, i * rowSize.height), rowSize);
                row->setSpacing(m_spacing);
                row->setMargins({ 0, 0, 0, 0 });
                m_vBoxLayout->addItem(row);
                
                std::vector<GUIObjectID> columnItems;
                for (int c = 0; c < getNumColumns(); ++c)
                {
                    //todo: allow box color to be a variable of the table class. we should also be able to 
                    //      have even rows with one color, odd rows with another
                    Box *box = ObjectManager::getInstance()->createObject<Box>(this, row->getPosition(), colSize, 
                        Color(255, 255, 255, 255));
                    row->addItem(box);
                    columnItems.push_back(box->getObjectID());
                }
                
                m_tableItems.push_back(columnItems);
            }
        }
        
        m_tableItems.resize(count);
        
        m_numRows = count;
    }
    
    void Table::setMargins(Margins margins)
    {
        m_margins = margins;
        
        m_vBoxLayout->setMargins(margins);
    }
    
    void Table::setSpacing(int spacing)
    {
        if (m_spacing == spacing)
            return;
            
        m_vBoxLayout->setSpacing(spacing);
        for (int i = 0; i < getNumRows(); ++i)
        {
            HBoxLayout *column = static_cast<HBoxLayout*>(m_vBoxLayout->getItemAt(i));
            if (column != nullptr)
                column->setSpacing(spacing);
        }
    }
    
    GUIObject *Table::getItemAt(int row, int column)
    {
        GUIObject *item = nullptr;
        try
        {
            auto &itr = m_tableItems.at(row);
            item = ObjectManager::getInstance()->getObject(itr.at(column));
        }
        catch (const std::out_of_range& oor)
        {
            LOG_ERROR("framework.gui", "Table::getItemAt() called with out-of-range items. Error msg: ", oor.what());
        }
        return item;
    }
    
    const int& Table::getNumColumns() const
    {
        return m_numColumns;
    }
    
    const int& Table::getNumRows() const
    {
        return m_numRows;
    }
    
    const int& Table::getSpacing() const
    {
        return m_spacing;
    }
    
    const Margins& Table::getMargins() const
    {
        return m_margins;
    }
    
    void Table::draw()
    {
        m_vBoxLayout->draw();
    }
}

