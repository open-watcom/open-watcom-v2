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
* Description:  Check for Win386 modules.
*
****************************************************************************/


#include "precomp.h"
#include <string.h>
#include "segmem.h"
#include "ismod32.h"

static WORD win386Sig[2] = { 0xDEAD, 0xBEEF };
static WORD win386Sig2[2] = { 0xBEEF, 0xDEAD };

typedef struct {
    WORD        sig[2];
    WORD        dataseg_off;
    WORD        codeinfo_off;
    WORD        stacksize_off;
    BYTE        call_cleanup[3];
    BYTE        mov_ax_4c00[3];
    BYTE        int_21[2];
    WORD        new_sig[2];
} winext_data;

/*
 * CheckIsModuleWin32App - check if a given module handle is a win32 app
 */
BOOL CheckIsModuleWin32App( HMODULE hmod, WORD *win32ds, WORD *win32cs,
                            DWORD *win32initialeip )
{
    GLOBALENTRY ge;
    winext_data wedata;
    int         segnum;

    *win32cs = *win32ds = 0;
    ge.dwSize = sizeof( GLOBALENTRY );
    if( !GlobalEntryModule( &ge, hmod, 1 ) ) {
        return( 0 );
    }
    ReadMem( (WORD)ge.hBlock, 0, (LPVOID) &wedata, sizeof( wedata ) );
    if( memcmp( wedata.sig, win386Sig, sizeof( win386Sig) ) == 0 ||
        memcmp( wedata.sig, win386Sig2, sizeof( win386Sig2) ) == 0 ) {
        if( memcmp( wedata.new_sig, win386Sig, sizeof( win386Sig ) ) == 0 ) {
            segnum = 2;
        } else {
            segnum = 3;
        }
        if( !GlobalEntryModule( &ge, hmod, segnum ) ) {
            return( 0 );
        }
        ReadMem( (WORD)ge.hBlock, wedata.dataseg_off, (LPVOID)win32ds, sizeof( WORD ) );
        ReadMem( (WORD)ge.hBlock, wedata.stacksize_off, (LPVOID)win32initialeip,
                 sizeof( DWORD ) );
        ReadMem( (WORD)ge.hBlock, wedata.codeinfo_off + 4, (LPVOID)win32cs,
                 sizeof( WORD ) );
        return( 1 );
    }
    return( 0 );

} /* CheckIsModuleWin32App */

/*
 * FlagWin32AppAsDebugged - check if a given module handle is a Win32 application
 */
void FlagWin32AppAsDebugged( HMODULE hmod )
{
    GLOBALENTRY ge;
    winext_data wedata;

    ge.dwSize = sizeof( GLOBALENTRY );
    if( !GlobalEntryModule( &ge, hmod, 1 ) ) {
        return;
    }
    ReadMem( (WORD)ge.hBlock, 0, (LPVOID)&wedata, sizeof( wedata ) );
    if( !memcmp( wedata.sig, win386Sig, sizeof( win386Sig ) ) ) {
        WriteMem( (WORD)ge.hBlock, 0, win386Sig2, 4 );
    }

} /* FlagWin32AppAsDebugged */
