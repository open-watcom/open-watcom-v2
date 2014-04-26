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
* Description:  Callback machinery for Win386 with thunk generation.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <stdarg.h>
#include "cover.h"
#include "wclbproc.h"

#define MAX_CB_PARMS            50
#define MAX_CB_JUMPTABLE        512

DWORD               _CBJumpTable[MAX_CB_JUMPTABLE]; /* Callback jump table */
BYTE                _CBRefsTable[MAX_CB_JUMPTABLE]; /* Callback reference counts */
extern  CALLBACKPTR __16BitCallBackAddr;
extern  FARPROCx    __far *__32BitCallBackAddr;
extern  void        __far __32BitCallBack( void );

#pragma aux _CBJumpTable        "*";
#pragma aux __16BitCallBackAddr "*";
#pragma aux __32BitCallBackAddr "*";
#pragma aux __32BitCallBack     "*";

extern DWORD __far *_CodeSelectorBaseAddr;
extern DWORD __far *_DataSelectorBaseAddr;

static int              emitOffset;
static char             *emitWhere;
static int              MaxCBIndex;

#define CB_CODE_SIZE    4

#define CB_DWORD        4
#define CB_WORD         2
#define CB_DONE         -1
#define CB_CDECL        -2

#define PUSH_EAX        0x50
#define PUSH_CS         0x0e
#define PUSH_ES         0x06
#define POP_DS          0x1f
#define RET             0xc3


/*
 * code emitting functions follow
 */
static void emitByte( char byte )
{
    if( emitWhere != NULL ) {
        emitWhere[ emitOffset ] = byte;
    }
    emitOffset ++;
}

static void emitWord( unsigned short word )
{
    char        hi,lo;

    hi = word >> 8;
    lo = word & 0xff;

    emitByte( lo );
    emitByte( hi );
}

static void emitDword( unsigned long dword )
{
    unsigned short      hi,lo;
    hi = dword >> 16;
    lo = dword & 0xffff;
    emitWord( lo );
    emitWord( hi );
}

static void emitMOV_EAX_DWORD( int off )        // mov eax,off[ecx]
{
    emitByte( 0x8b );
    emitByte( 0x41 );
    emitByte( off );
}

static void emitMOVZX_EAX_WORD( int off )       // movzx eax,off[ecx]
{
    emitByte( 0x0f );
    emitByte( 0xb7 );
    emitByte( 0x41 );
    emitByte( off );
}

static void emitMOV_EBX_const( long con )
{
    emitByte( 0xbb );
    emitDword( con );
}
static void emitMOV_DL_const( long con )
{
    emitByte( 0xb2 );
    emitByte( con );
}
static void emitCALL_EBX( void )
{
    emitByte( 0xff );
    emitByte( 0xd3 );
}
static void emitADD_ESP_const( long con )
{
    emitByte( 0x83 );
    emitByte( 0xc4 );
    emitByte( con );
}

/*
 * emitCode - generate callback code for a specified argument list
 */
static int emitCode( int argcnt, int bytecnt, char *array,
                DWORD fn, int is_cdecl )
{
    int         i;
    int         offset;

    emitOffset = 0;
    emitMOV_EBX_const( fn );            // 32-bit callback routine

    /*
     * emit code to push parms from 16-bit stack onto 32-bit stack
     */
    offset = bytecnt + 6;
    if( is_cdecl ) {
        i = argcnt-1;
    } else {
        i = 0;
    }
    while( argcnt > 0 ) {
        if( array[i] == CB_DWORD ) {
            offset -= 4;
            emitMOV_EAX_DWORD( offset );
        } else {
            offset -= 2;
            emitMOVZX_EAX_WORD( offset );
        }
        emitByte( PUSH_EAX );
        if( is_cdecl ) {
            i--;
        } else {
            i++;
        }
        --argcnt;
    }
    emitByte( PUSH_ES );
    emitByte( POP_DS );
    emitByte( PUSH_CS );
    emitCALL_EBX();
    if( is_cdecl ) {
        emitADD_ESP_const( bytecnt );
        emitMOV_DL_const( 0 );
    } else {
        emitMOV_DL_const( bytecnt );
    }
    emitByte( RET );

    return( emitOffset );

} /* emitCode */

/*
 * DoEmitCode - build callback routine thunk
 */
static CALLBACKPTR DoEmitCode( int argcnt, int bytecnt, char *array,
                               DWORD fn, int is_cdecl )
{
    int codesize;
    int i;

    /*
     * get a callback jump table entry
     */
    for( i = 0; i < MAX_CB_JUMPTABLE; i++ ) {
        if( _CBJumpTable[i] == 0L ) {
            break;
        }
    }
    if( i == MAX_CB_JUMPTABLE ) {
        return( NULL );
    }

    /*
     * build the callback code
     */
    emitWhere = NULL;
    codesize = emitCode( argcnt, bytecnt, array, fn, is_cdecl );
    emitWhere = malloc( codesize );
    if( emitWhere == NULL ) {
        return( NULL );
    }
    emitCode( argcnt, bytecnt, array, fn, is_cdecl );

    /*
     * set up the callback jump table, and return the proper callback rtn
     */
    _CBJumpTable[i] = (DWORD)emitWhere  - *_DataSelectorBaseAddr
                                        + *_CodeSelectorBaseAddr;
    _CBRefsTable[i]++;  /* increase reference count */
    if( i > MaxCBIndex )  MaxCBIndex = i;
    *__32BitCallBackAddr = (FARPROCx)&__32BitCallBack;
    return( (char *)__16BitCallBackAddr - (i+1) * CB_CODE_SIZE );

} /* DoEmitCode */


CALLBACKPTR vGetCallbackRoutine( PROCPTR fn, va_list vl )
{
    int         type;
    int         bytecnt = 0;
    int         argcnt = 0;
    char        array[MAX_CB_PARMS];
    int         is_cdecl = FALSE;

    while( (type = va_arg( vl, int ) ) != GCB_ENDLIST ) {
        if( type == GCB_CDECL ) {
            is_cdecl = TRUE;
            continue;
        }
        array[argcnt] = type;
        if( type == GCB_DWORD ) bytecnt += 4;
        else bytecnt += 2;
        argcnt++;
        if( argcnt >= MAX_CB_PARMS ) {
            return( NULL );
        }
    }
    return( DoEmitCode( argcnt, bytecnt, array, fn, is_cdecl ) );
}

CALLBACKPTR GetCallbackRoutine( PROCPTR fn, ... )
{
    va_list     vl;
    CALLBACKPTR cbp;

    va_start( vl, fn );
    cbp = vGetCallbackRoutine( fn, vl );
    va_end( vl );
    return( cbp );
}

void ReleaseCallbackRoutine( CALLBACKPTR cbp )
{
    int         i;
    DWORD       cb;

    i = ((char *)__16BitCallBackAddr - (char *)cbp) / CB_CODE_SIZE;
    --i;
    cb = _CBJumpTable[i] - *_CodeSelectorBaseAddr + *_DataSelectorBaseAddr;
    _CBRefsTable[i]--;  /* decrease reference count */
    if( _CBRefsTable[i] == 0 ) {
        free( (void *)cb );
        _CBJumpTable[i] = 0L;
    }
} /* ReleaseCallbackRoutine */

void *SetProc( FARPROC fp, int type )
{
    int         i;
    char        *cbp;

    if( fp == NULL )  return( NULL );
    for( i = 0; i <= MaxCBIndex; i++ ) {
        cbp = (char *)(_CBJumpTable[i] - *_CodeSelectorBaseAddr
                                       + *_DataSelectorBaseAddr);
        if( *(FARPROC *)(cbp+1) == fp ) {
            _CBRefsTable[i]++;  /* increase reference count */
            return( (char *)__16BitCallBackAddr - (i+1) * CB_CODE_SIZE );
        }
    }
    return( GetProc16( (PROCPTR)fp, type ) );
}

#undef FreeProcInstance
#undef MakeProcInstance

FARPROC PASCAL _Cover_MakeProcInstance( FARPROCx proc, HINSTANCE inst )
{
    inst;
    return( (FARPROC)proc );
}

void PASCAL _Cover_FreeProcInstance( FARPROC fp )
{
    FreeProcInstance( fp );
}

void PASCAL FreeProcInstance( FARPROC fp )
{
    int         i;
    char        *cbp;

    for( i = 0; i <= MaxCBIndex  &&  _CBJumpTable[i] != 0L; i++ ) {
        cbp = (char *)(_CBJumpTable[i] - *_CodeSelectorBaseAddr + *_DataSelectorBaseAddr);
        if( *(FARPROC *)(cbp+1) == fp ) {
            _CBRefsTable[i]--;  /* decrease reference count */
            if( _CBRefsTable[i] == 0 ) {
                free( cbp );
                _CBJumpTable[i] = 0L;
            }
        }
    }
}
