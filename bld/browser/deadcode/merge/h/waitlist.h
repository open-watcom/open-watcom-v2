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


#ifndef _WAITING_LIST_H
#define _WAITING_LIST_H

// System includes --------------------------------------------------------

#include <wstd.h>
#include <wcvector.h>

// Project includes -------------------------------------------------------

#include "ref.h"

class WaitingInfo {
friend class WaitingList;
public:
    bool                        operator==( const WaitingInfo& other ) {
                                    return this == &other;
                                }

                                WaitingInfo( int, uint_32, RefVector * );
                                ~WaitingInfo();

private:
    int                         _mbrIndex;
    uint_32                     _offset;
    RefVector *                 _refs;
};

/*
 | A table of < mbrIndex, offset > = ( list of References )
 | so that when the refs to symbol at the LHS is encountered, each of the
 | references on the RHS can be accessed.
 */

class WaitingList {
public:
                                WaitingList();
                                ~WaitingList();

    RefVector *                 findList( int mbrIndex, uint_32 offset );
    void                        add( int mbrIndex, uint_32 offset, RefVector * refs );
    void                        remove( RefVector * );

private:

    /*
     | Right now, a flat list is used.  This could certainly be improved
     | later by using a different scheme.
     */
    WCPtrOrderedVector<WaitingInfo>     _table;
};

#endif
