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


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdarg.h>

/*
 * This is going to get ugly, 'cause I'm adding NEC code which is TOTALLY
 * different. But I'll try to keep it nice.
 */

unsigned DbgFlags = -1;

#if defined( _NEC_PC )
#ifdef DEBUG

/*
 * Debugging output code for the NEC
 */
char FAR *              CharPtr;
char FAR *              AttrPtr;

static int SetupVideoPointers();

int _cnt;

static int _initialized=0;
static int _line=0;
static unsigned         screen_mem;

static int SetupVideoPointers()
{
    FARPROC tmpp;

    tmpp = GetProcAddress(GetModuleHandle("KERNEL"), "__A000h");
    if( tmpp == NULL ) return( TRUE );
    screen_mem = LOWORD( (DWORD)tmpp );

    CharPtr = MK_FP( screen_mem, 0 );
    AttrPtr = MK_FP( screen_mem, 0x2000 );
    _initialized = 1;
    return( FALSE );
}

void MyClearScreen()
{
    int i;

    if( !_initialized ) {
        SetupVideoPointers();
    }
    for( i=0; i < 80 * 25; i++ ) {
        CharPtr[ i * 2 ] = ' ';
        CharPtr[ i * 2 + 1 ] = 0;
        AttrPtr[ i * 2 ] = 0xE1;
        AttrPtr[ i * 2 + 1 ] = 0;
    }
}

void MyOut( unsigned f, char *str, ... )
{
    va_list     al;
    char        res[128];
    int         len,i;
    char        *scr;
    char        *atr;

    if( ( f & DbgFlags ) == 0 ) return;
    if( !_initialized ) SetupVideoPointers();
    sprintf( res,"%03d) ",++_cnt );
    va_start( al, str );
    vsprintf( &res[5],str, al );
    len = strlen( res );

    scr = &CharPtr[ _line*80*2 ];
    atr = &AttrPtr[ _line*80*2 ];

    for( i=0;i<len;i++ ) {
        scr[i*2] = res[i];
        scr[i*2+1] = 0;
        atr[ i*2 ] = 0xE1;
        atr[ i*2+1 ] = 0;
    }
    for( i=len;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 0;
        atr[i*2] = 0xE1;
        atr[i*2+1] = 0;
    }
    _line++;
    if( _line > 24 ) _line = 0;

    scr = &CharPtr[_line*80*2];
    atr = &AttrPtr[_line*80*2];
    for( i=0;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 0;
        atr[i*2] = 0xE1;
        atr[i*2+1] = 0;
    }
}

#endif                          // ifdef DEBUG

#else                           // if defined( _NEC_PC )
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
#endif                                  // if defined( _NEC_PC )
