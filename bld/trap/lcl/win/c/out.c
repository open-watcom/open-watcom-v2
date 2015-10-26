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
* Description:  Windows 3.x debug output.
*
****************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdarg.h>


unsigned DbgFlags = -1;

#ifdef DEBUG

/*
 * Debugging output code for AT
 */

int _cnt;

static int _line=0;

#define MONO

// extern char _B000H[];
//  MK_FP( _B000H, 0 );

#include "dpmi.h"
char *GetScreenPointer( void )
{
#if 0
    static short sel;

    if( sel == 0 ) sel = _DPMISegmentToDescriptor( 0xB000 );
    return( MK_FP( sel, 0 ) );
#else
    extern char _B000h[];
    return( MK_FP( _B000h, 0 ) );
#endif
}

void MyClearScreen()
{
#ifdef MONO
    int i;

    char *scrn=GetScreenPointer();

    for( i=0;i<80*25;i++ ) {
        scrn[i*2] = ' ';
        scrn[i*2+1] = 7;
    }
#endif
}

void MyOut( unsigned f, char *str, ... )
{
    va_list     al;
    char        res[128];
    int         len,i;
    char        *scr;
    char        *scrn=GetScreenPointer();

    if( ( f & DbgFlags ) == 0 ) return;
    sprintf( res,"%03d) ",++_cnt );
    va_start( al, str );
    vsprintf( &res[5],str, al );
#ifndef MONO
    MessageBox( NULL, res, "FOO", MB_SYSTEMMODAL | MB_OK );
#else
    len = strlen( res );

    scr = &scrn[_line*80*2];

    for( i=0;i<len;i++ ) {
        scr[i*2] = res[i];
        scr[i*2+1] = 7;
    }
    for( i=len;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 7;
    }
    _line++;
    if( _line > 24 ) _line = 0;

    scr = &scrn[_line*80*2];
    for( i=0;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 7;
    }

#endif
}
#endif                                  // ifdef DEBUG
