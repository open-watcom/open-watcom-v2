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


#ifndef __RINGNAME_H__
#define __RINGNAME_H__

// RINGNAME.H -- C++ definitions for rings of names
//
// 91/06/10 -- J.W.Welch        -- defined


// STRUCTURES:

typedef struct ringnamectl  RINGNAMECTL;
typedef struct ringname     RINGNAME;

struct ringnamectl {            // RINGNAMECTL
    RINGNAME *ring;             // - ring of elements
    unsigned case_sensitive : 1;// - TRUE ==> case sensitive names
};

struct ringname {               // RINGNAME entry
    RINGNAME *next;             // - next in ring
    char name[1];               // - the name (variable size)
};


// PROTOTYPES:

char *RingNameAllocVct(         // ALLOCATE A NAME, FOR A VECTOR
    RINGNAMECTL *ctl,           // - control for ring
    char const *vector,         // - vector to be allocated
    size_t len );               // - length of vector

void RingNameFree(              // FREE RING OF NAMES
    RINGNAMECTL *ctl );         // - control for ring

void RingNameInit(              // INITIALIZE RING OF NAMES
    RINGNAMECTL *ctl );         // - control for ring

RINGNAME *RingNameLookup(       // LOOKUP A NAME
    RINGNAMECTL *ctl,           // - control for ring
    const char *name );         // - name to be looked up

#endif
