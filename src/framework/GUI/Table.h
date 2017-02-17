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

#include "GUIObject.h"
#include "Layout.h"

namespace gui 
{
    class VBoxLayout;
    
    // 2D array that contains identifiers for table items at a given row and column
    typedef std::vector< std::vector<GUIObjectID> > TableObjectContainer;
    
    /**
     * @class Table
     * @author Timothy Vaccarelli
     * @date 02/03/2016
     * @file Table.h
     * @brief A table object, of n rows, m columns, which are constructed with Box objects.
     */
    class Table : public GUIObject
    {
    public:
        /// Table constructor, specifying a parent object, position and size. Defaults to 1 row, 1 column, 5px spacing, 5px for all margins
        Table(GUIObject *parent, Position position, Size size, int numRows = 1, int numCols = 1,
            int spacing = 5, Margins margins = { 5, 5, 5, 5 });
        
        /// Sets the table structure to contain the given amount of rows and columns
        void setDimensions(int rows, int columns);
        
        /// Sets the number of columns in the table
        void setNumColumns(int count);
        
        /// Sets the number of rows in the table
        void setNumRows(int count);
        
        /// Sets the margins of the table
        void setMargins(Margins margins);
        
        /// Sets the spacing in between rows and columns
        void setSpacing(int spacing);
        
        /// Returns a pointer to the object at the given row and column
        GUIObject *getItemAt(int row, int column = 0);
        
        /// Returns the number of columns in the table
        const int& getNumColumns() const;
        
        /// Returns the number of rows in the table
        const int& getNumRows() const;
        
        /// Returns the spacing of items in the table
        const int& getSpacing() const;
        
        /// Returns the margins of the table
        const Margins& getMargins() const;
    
    public:
        /// Draws the table and its sub-objects
        void draw() override;
    
    protected:
        /// Number of columns in the table
        int m_numColumns;
        
        /// Number of rows in the table
        int m_numRows;
        
        /// Table spacing
        int m_spacing;
        
        /// Table margins
        Margins m_margins;
        
        /// 2D vector of item IDs in the table
        TableObjectContainer m_tableItems;
        
        /// Vertical layout object, used if the table has > 1 row. If > 1 columns, each object added to the VBox is an HBoxLayout
        VBoxLayout *m_vBoxLayout;
    };
}

