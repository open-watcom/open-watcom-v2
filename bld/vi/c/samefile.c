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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(M_I86) || defined(M_I386)
#include <i86.h>
#endif
#include "vi.h"
#include "pragmas.h"

#if FSYS_CASE_SENSITIVE
    #define file_cmp    strcmp
#else
    #define file_cmp    stricmp
#endif

/*
 * SameFile - check if two files are the same
 */
int SameFile( char *f1, char *f2 )
{
    if( !file_cmp( f1, f2 ) ) {
        return( TRUE );
    }

{
    char        full1[_MAX_PATH];
    char        full2[_MAX_PATH];

#ifndef __WINDOWS__
    DosGetFullPath( f1, full1 );
    DosGetFullPath( f2, full2 );
#else
// this is not to be used under NT
    WinGetFullPath( f1, full1 );
    WinGetFullPath( f2, full2 );
#endif
    if( !file_cmp( full1, full2 ) ) {
        if( EditFlags.SameFileCheck ) {
            return( TRUE );
        }
        EditFlags.DuplicateFile = TRUE;
    }
}

    return( FALSE );

} /* SameFile */

#ifdef __WINDOWS__
// WILL NOT WORK FOR NT !!!

#include "dpmi.h"
#include "winvi.h"

/*
 * WinGetFullPath - use undoccumented dos int 21 60h
 *                  to resolve full file name from protected mode windows.
 *                  ( Ref. UnDocumented DOS pg. 149 )
 */
void WinGetFullPath( char *filename, char *full ){

    rm_call_struct r;

    char far *str;
    char far *str2;

    unsigned long  handle;
    unsigned short para;
    unsigned short sel;

    // Allocate a chunk of Real mode memory

    // handle  = DPMIAllocateDOSMemoryBlock( _MAX_PATH >>3 );
    handle  = GlobalDosAlloc( _MAX_PATH * 2 );

    // if( (handle == 8) || (handle == 7) ){
    if( handle == NULL ){
        // corrupt or insufficient memory
        full = NULL;
        return;
    }

    // find the paragraph and selector portions
    para = handle>>16;
    sel = handle & 0xFFFF;

    // copy the filename to be expanded into the first half of block
    str = MK_FP( sel, 0 );
    _fstrcpy( str, filename );


    // Call int21 function AH 60h (Truename)
    memset( &r,  0 , sizeof( r ) );
    r.eax = 0x6000;
    r.es = para;
    r.esi = 0;
    r.ds = para;
    r.edi = _MAX_PATH;

    DPMISimulateRealModeInterrupt( 0x21, 0, 0, &r );

    // copy the return value from the second half of the block

    str2 = MK_FP( sel, _MAX_PATH );
    _fstrcpy( full, str2 );

    // Free the allocated Real mode memory

    GlobalDosFree( sel );
    // DPMIFreeDOSMemoryBlock( sel );
}
#endif
