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


#include <string.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include "commonui.h"
#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"
#include "exeos2.h"
#include "exedos.h"
#include "ismod32.h"

/*
 * accessSegment - access a segment to cause the windows loader to bring it in
 */
static WORD accessSegment( GLOBALHANDLE gh, WORD segment )
{
    static WORD         sel;
    static WORD         offset;
    WORD                i;

    ReadMem( (WORD)gh, 0x22, &offset, sizeof( offset ) );
#if 1
    i = 0;
    // was Eisler on drugs??
    while( i < segment ) {
        ReadMem( (WORD)gh, offset+8, &sel, sizeof( sel ) );
        offset += 10;
        i++;
    }
#else
    // not sure if this fix is safe -- so don't make it yet
    ReadMem( (WORD)gh, 10*(segment-1)+8, &sel, sizeof( sel ) );
#endif
    return( sel+1 );

} /* accessSegment */

/*
 * horkyFindSegment - runs and tries to find a segment.  load on call
 *                    segments are not identified by toolhelp,
 *                    so mapaddrs for those segments would fail with
 *                    out this putrid code.
 */
static WORD horkyFindSegment( HMODULE modid, WORD segment )
{
    static GLOBALENTRY  ge;
    static HMODULE      lastmodid;

    if( lastmodid == modid ) {
        return( accessSegment( ge.hBlock, segment ) );
    }
    lastmodid = modid;
    ge.dwSize = sizeof( ge );
    if( !MyGlobalFirst( &ge, GLOBAL_ALL ) ) {
        lastmodid = NULL;
        return( 0 );
    }
    do {
        if( ge.hOwner == modid && ge.wType == GT_MODULE ) {
            return( accessSegment( ge.hBlock, segment ) );
        }
        ge.dwSize = sizeof( ge );
    } while( MyGlobalNext( &ge, GLOBAL_ALL ) );
    lastmodid = NULL;
    return( 0 );

} /* horkyFindSegment */


/*
 * newModule - handle a new module
 */
static void newModule( HANDLE hmod, char *name, samp_block_kinds kind )
{
    GLOBALENTRY         ge;
    os2_exe_header      ne;
    dos_exe_header      de;
    seg_offset          ovl;
    int                 i;
    LPVOID              ptr;
    WORD                sel;
    int                 handle;
    int                 numsegs;
    WORD                win32ds;
    WORD                win32cs;
    DWORD               win32initialeip;
    int                 rc;

    ovl.offset = 0;
    ovl.segment = 0;

    rc = CheckIsModuleWin32App( hmod, &win32ds, &win32cs, &win32initialeip );
    if( rc ) {
        if( win32ds != 0 ) {
            WriteCodeLoad( ovl, name, kind );
            WriteAddrMap( 1, win32cs, win32initialeip );
            WriteAddrMap( 2, win32ds, 0 );
        } else if( kind == SAMP_MAIN_LOAD ) {
            FlagWin32AppAsDebugged( hmod );
            WaitForInt3 = GetCurrentTask();
        } else if( WaitForInt1 == 0 ) {
            FlagWin32AppAsDebugged( hmod );
            WaitForInt1 = hmod;
        }
        return;
    }
    WriteCodeLoad( ovl, name, kind );

    handle = open( name,O_BINARY | O_RDONLY );
    if( handle >= 0 ) {
        read( handle, &de, sizeof( de ) );
        if( de.signature == DOS_SIGNATURE ) {
            lseek( handle, (de.file_size-1L)*512L+(long)de.mod_size, SEEK_SET );
        } else {
            lseek( handle, 0, SEEK_SET );
        }
        read( handle, &ne, sizeof( ne ) );
        if( ne.signature == OS2_SIGNATURE_WORD ) {
            numsegs = ne.segments;
            if( numsegs > 8192 ) {
                // must not really be a valid OS2 sig.
                numsegs = -1;
            }
        } else {
            numsegs = -1;
        }
        close( handle );
    }

    for( i=1;i<8192;i++ ) {
        if( !MyGlobalEntryModule( &ge, hmod, i ) ) {
            if( numsegs > 0 ) {
                sel = horkyFindSegment( hmod, i );
                if( sel == 0 ) continue;
            } else {
                continue;
            }
        } else {
            if( ge.hBlock != NULL ) {
                ptr = GlobalLock( ge.hBlock );
                GlobalUnlock( ge.hBlock );
                sel = FP_SEG( ptr );
                if( sel == NULL ) {
                    sel = (WORD)ge.hBlock + 1;
                }
            } else {
                continue;
            }
        }
        WriteAddrMap( i, sel, 0 );
        numsegs--;
        if( numsegs == 0 ) {
            break;
        }
    }

} /* newModule */

/*
 * HandleLibLoad - process library loaded
 */
void HandleLibLoad( int type, HANDLE hmod )
{
    static MODULEENTRY          me;

    if( MyModuleFindHandle( &me, hmod ) ) {
        newModule( hmod, me.szExePath, type );
    }

} /* HandleLibLoad */
