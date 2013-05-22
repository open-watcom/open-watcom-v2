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


#include "cgstd.h"
#include <string.h>
#include "coderep.h"
#include "cgmem.h"
#include "data.h"
#include <assert.h>


extern  void    InitFrl( pointer **head ) {
/*****************************************/

    *head = NULL;
}


extern  pointer AllocFrl( pointer **head, size_t size ) {
/****************************************************/

    pointer     new;

    if( *head == NULL ) {
        new = CGAlloc( size );
   } else {
        new = *head;
        *head = **head;
        FrlSize -= size;
    }
#ifndef NDEBUG
    memset( new, 0xda, size );
#endif
    _AlignmentCheck( new, 8 );
    return( new );
}


extern  void    FrlFreeSize( pointer **head, pointer *what, size_t size ) {
/**********************************************************************/

#ifndef NDEBUG
    memset( what, 0xdf, size );
#endif
    FrlSize += size;
    *what = *head;
    *head = what;
}


extern  bool    FrlFreeAll( pointer **head, size_t size ) {
/******************************************************/

    pointer     junk;

    junk = NULL;
    while( *head != NULL ) {
        junk = *head;
        *head = **head;
        CGFree( junk );
        FrlSize -= size;
    }
    return( junk != NULL );
}
