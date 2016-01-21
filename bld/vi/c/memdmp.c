/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal editor memory usage dump routine.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include "memdmp.h"
#ifdef __DOS__
#include "xmem.h"
#endif

#ifdef __DOS__
#if defined( USE_XTD )
extern xtd_struct XMemCtrl;
#endif
#if defined( USE_EMS )
extern ems_struct EMSCtrl;
#endif
#if defined( USE_XMS )
extern xms_struct XMSCtrl;
#endif
#endif

int     MaxStatic = 0;

static char freeBytes[] =  "%s:  %l bytes free (%d%%)";
static char twoStr[] = "%Y%s";
//extern long __undocnt;

/*
 * DumpMemory - dump memory avaliable
 */
vi_rc DumpMemory( void )
{
    int         ln = 1;
    window_id   wn;
    window_info *wi;
    char        tmp[128], tmp2[128];
#if defined( USE_XMS ) || defined( USE_EMS ) || defined( USE_XTD )
    long        mem1;
#endif
    long        mem2;
//    vi_rc       rc;

    wi = &filecw_info;
//    rc = NewWindow2( &wn, wi );
    NewWindow2( &wn, wi );
#if defined(__OS2__ )
    WPrintfLine( wn, ln++, "Mem:  (unlimited) (maxStatic=%d)", MaxStatic );
#else
    WPrintfLine( wn, ln++, "Mem:  %l bytes memory (%l for editing) (maxStatic=%d)",
        MaxMemFree, MaxMemFreeAfterInit, MaxStatic );
#endif

    mem2 = (EditVars.MaxSwapBlocks - SwapBlocksInUse) * (long)MAX_IO_BUFFER;
    MySprintf( tmp, freeBytes, "Dsk", mem2,
        (int) ((100L * mem2) / ((long)EditVars.MaxSwapBlocks * (long)MAX_IO_BUFFER)) );
#ifdef _M_I86
#if defined( USE_XTD )
    if( XMemCtrl.inuse ) {
        mem1 = XMemCtrl.amount_left - XMemCtrl.allocated * (long)MAX_IO_BUFFER;
        MySprintf( tmp2, freeBytes, "XTD", mem1,
            (int) ((100L * mem1) / XMemCtrl.amount_left) );
    } else {
#endif
        MySprintf( tmp2, "XTD: N/A" );
#if defined( USE_XTD )
    }
#endif
#else
    MySprintf( tmp2, "Flat memory addressing" );
#endif
    WPrintfLine( wn, ln++, twoStr, tmp, tmp2 );

#if defined( USE_EMS )
    if( EMSCtrl.inuse ) {
        mem1 = (long)(TotalEMSBlocks - EMSBlocksInUse) * (long)MAX_IO_BUFFER;
        MySprintf( tmp, freeBytes, "EMS", mem1,
                (int) ((100L * mem1) / ((long)TotalEMSBlocks * (long)MAX_IO_BUFFER)) );
    } else {
#endif
        MySprintf( tmp, "EMS:  N/A" );
#if defined( USE_EMS )
    }
#endif
#if defined( USE_XMS )
    if( XMSCtrl.inuse ) {
        mem1 = (long)(TotalXMSBlocks - XMSBlocksInUse) * (long)MAX_IO_BUFFER;
        MySprintf( tmp2, freeBytes, "XMS", mem1,
            (int) ((100L * mem1) / ((long)TotalXMSBlocks * (long)MAX_IO_BUFFER)) );
    } else {
#endif
        MySprintf( tmp2, "XMS: N/A" );
#if defined( USE_XMS )
    }
#endif
    WPrintfLine( wn, ln++, twoStr, tmp, tmp2 );
//    WPrintfLine( wn, ln++, "Reserved %l bytes of DOS memory", MinMemoryLeft );

    WPrintfLine( wn, ln++, "File CB's: %d", FcbBlocksInUse );
//    WPrintfLine( wn, ln++, "File CB's: %d, Undo blocks=%l(%l bytes)", FcbBlocksInUse,
//        __undocnt, __undocnt * sizeof( undo ) );

    WPrintfLine( wn, ln + 1, MSG_PRESSANYKEY );

    GetNextEvent( false );
    CloseAWindow( wn );
    return( ERR_NO_ERR );

} /* DumpMemory */
