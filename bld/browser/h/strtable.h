/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _STRING_TABLE_H
#define _STRING_TABLE_H

#include <string.h>

#include <wstring.hpp>
#include <wtptlist.hpp>

// Important : Since we're using variable argument lists, it's important
//             that you pass the correct type of arguments!
#define EmptyString ((const char *) NULL)

typedef void ( WObject::*CBStringTable ) ( WString & );

/*-----------------------------------------------------------------------
  This class defines a table of null-terminated character strings.
  The default width of each column of the table is (80/#ofcols),
  rounded down.  Actually, it's more of an abstract data type than
  a class.

  For a class, we'd probably need to define a base class of Table and
  derive StringTable from that.
  -----------------------------------------------------------------------*/
class StringTable : public WObject {
public :
                        StringTable( int, ... );
                        ~StringTable();

    void                add( const char *, ... );
    void                replaceAt( int rowNum, const char * firstString, ... );

    /*-------------------------------------------------------------------
      Outputs the table using the printRow() method.  This formats the
      table so that each string stays within its column
      specification.
      -------------------------------------------------------------------*/
    virtual void        output();
    void                setOutputCallback( WObject * client, CBStringTable cb );

    /*-------------------------------------------------------------------
      Outputs a row of data without going have to store the strings in
      the table.
      -------------------------------------------------------------------*/
    void                outputRow( const char * , ... );

protected :
    virtual void        printRow( const char *row[] );

private :
    int                 _columns;
    TemplateList<char const **> _table;
    int *               _colWidths;
    int                 _totalWidth;
    char *              _buffer;
    CBStringTable       _cbStringTable;
    WObject *           _client;
    char const **       _position;
};

#endif
