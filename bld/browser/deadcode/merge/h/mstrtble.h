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


#ifndef _MERGE_STRING_TABLE_H
#define _MERGE_STRING_TABLE_H

// System includes --------------------------------------------------------

#include <wstd.h>
#include <string.h>
#include <wcvector.h>

// Project includes -------------------------------------------------------


class MergeStringTable {
public:
                            MergeStringTable();
                            ~MergeStringTable();

    void                    find( char * key, void ** data, int & index );
    void                    add( char * key, void * data, int index );

private:
    struct Element {
        Element( char *, void *, int );
        ~Element();

        bool operator== ( const Element& rhs) {
            return strcmp( key, rhs.key ) == 0;
        }

        char * key;
        int    index;
        void * data;
    };

    WCPtrOrderedVector<Element> _table;
};

#endif /* _MERGE_STRING_TABLE_H */

