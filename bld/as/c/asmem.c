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


#include "as.h"
#ifdef _STANDALONE_
#include "trmem.h"
#include "trmemcvr.h"
#else
extern void *AsmAlloc( size_t amount );
extern void AsmFree( void *ptr );
#endif

#ifdef _STANDALONE_
void MemInit( void ) {
//**************

    TRMemOpen();
}
#endif

pointer MemAlloc( size_t size ) {
//*********************************

#ifdef _STANDALONE_
    return( TRMemAlloc( size ) );
#else
    return( AsmAlloc( size ) );
#endif
}

#ifdef _STANDALONE_
pointer MemRealloc( pointer p, size_t size ) {
//**********************************************

    return( TRMemRealloc( p, size ) );
}
#endif

void MemFree( pointer p ) {
//*************************

#ifdef _STANDALONE_
    TRMemFree( p );
#else
    AsmFree( p );
#endif
}

#ifdef _STANDALONE_
void MemFini( void ) {
//**************

#ifdef TRMEM
    TRMemPrtList();
#endif
    TRMemClose();
}
#endif
