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


#include <i86.h>
#include "standard.h"
#define INCL_DOS
#define INCL_VIO
#include "os2.h"

static  int             TicCount;
static  unsigned_16     ScreenSelector = 0;

#if 0
static  void    Tic()
{
    for( ;; ) {
        DosSleep( 4011 );
        TicCount += 73; /* 18.2 x 4 = approx. 4.011 secs */
    }
}
#endif

uint GetTickCount()
{
    return( TicCount );
}

void GrabTimer()
{
#if 0
    TID tid;
    TicCount = 0;
    DosCreateThread( &tid, (PFNTHREAD)Tic, NULL, FALSE, 4096 );
#endif
}

void ReleTimer()
{
}

extern unsigned_8 _SelectorWritable( unsigned_16 sel );
#pragma aux _SelectorWritable = ".386p" "verw ax" "sete al" parm [ax] value [al];

void Blip(unsigned_16 loc,char ch )
{
    char far    *mem;

    mem = MK_FP( ScreenSelector, loc * 2 );
    if( _SelectorWritable( ScreenSelector ) ) {
        *mem = ch;
    }
}

void BlipInit( void )
{
    unsigned_8  mode;
    VIOPHYSBUF  buffer;

    if( ScreenSelector == 0 ) {
        DosDevConfig( &mode, DEVINFO_ADAPTER );
        if( mode ) {
            buffer.pBuf = (PBYTE)0xb8000;
        } else {
            buffer.pBuf = (PBYTE)0xb0000;
        }
        buffer.cb = 160;
        if( !VioGetPhysBuf( &buffer, 0 ) ) {
            ScreenSelector = buffer.asel[ 0 ];
        }
    }
}
