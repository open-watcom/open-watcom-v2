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


#ifndef _ins_h
#define _ins_h

#include <iostream>
#include "basics.h"

const uint nChars = 256;
typedef uchar Char;

const uint CHAR = 0;
const uint GOTO = 1;
const uint FORK = 2;
const uint TERM = 3;
const uint CTXT = 4;

union Ins {
    struct {
        byte    tag;
        byte    marked;
        void    *link;
    }                   i;
    struct {
        ushort  value;
        ushort  bump;
        void    *link;
    }                   c;
};

inline bool isMarked(Ins *i){
    return (bool) i->i.marked;
}

inline void mark(Ins *i){
    i->i.marked = true;
}

inline void unmark(Ins *i){
    i->i.marked = false;
}

#endif
