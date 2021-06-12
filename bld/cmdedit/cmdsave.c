/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "cmdedit.h"

#define _ModIndex( ind, adj )   (ind += adj, ind &= (SAVE_SIZE-1));

#ifdef __DOS__
char SaveArea[SAVE_SIZE];
#else
unsigned int SaveArea[SAVE_SIZE];
#endif
unsigned FirstCmd;
unsigned FirstFree;
unsigned CurrCmd;
int     LastWasNext;

#ifdef __DOS__
#include <dos.h>

static union REGPACK r;

/*
 * RestSave -- restore the saved commands from a file
 */

static int RestSave( char __far *file )
{
    USHORT len;
    USHORT hdl;

    len = 0;
    while( file[len] != '\0' && file[len] != '\r' )
        ++len;
    while( len-- > 0 ) {
        if( file[len] != ' ' ) {
            break;
        }
    }
    ++len;
    file[len] = '\0';
    if( file[0] == '\0' )
        return( 0 );
    r.x.dx = _FP_OFF( file );
    r.x.ds = _FP_SEG( file );
    r.h.ah = 0x3d;
    r.h.al = 0;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF )
        return( 0 );
    hdl = r.x.ax;
    if( DosRead( hdl, SaveArea, SAVE_SIZE, &len ) != 0 )
        return( 0 );
    if( DosRead( hdl, (char __far *)&FirstCmd, sizeof( FirstCmd ), &len ) != 0 )
        return( 0 );
    if( DosRead( hdl, (char __far *)&FirstFree, sizeof( FirstFree ), &len ) != 0 )
        return( 0 );
    if( DosRead( hdl, (char __far *)&CurrCmd, sizeof( CurrCmd ), &len ) != 0 )
        return( 0 );
    if( DosRead( hdl, (char __far *)&LastWasNext, sizeof( LastWasNext ), &len ) != 0 )
        return( 0 );
    r.h.ah = 0x3e;
    r.x.bx = hdl;
    intr( 0x21, &r );
    return( 1 );
}

/*
 * SaveSave -- save the saved commands to a file
 */

void __far SaveSave( char __far *file )
{
    USHORT len;
    USHORT hdl;

    file += 2;
    while( *file == ' ' || *file == '=' )
        ++file;
    len = 0;
    while( file[len] != '\0' && file[len] != '\r' )
        ++len;
    while( len-- > 0 ) {
        if( file[len] != ' ' ) {
            break;
        }
    }
    ++len;
    file[len] = '\0';
    if( file[0] == '\0' )
        return;
    r.x.dx = _FP_OFF( file );
    r.x.ds = _FP_SEG( file );
    r.h.ah = 0x3c;
    r.x.cx = 0;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF )
        return;
    hdl = r.x.ax;
    DosWrite( hdl, SaveArea, SAVE_SIZE, &len );
    DosWrite( hdl, (char __far *)&FirstCmd, sizeof( FirstCmd ), &len );
    DosWrite( hdl, (char __far *)&FirstFree, sizeof( FirstFree ), &len );
    DosWrite( hdl, (char __far *)&CurrCmd, sizeof( CurrCmd ), &len );
    DosWrite( hdl, (char __far *)&LastWasNext, sizeof( LastWasNext ), &len );
    r.h.ah = 0x3e;
    r.x.bx = hdl;
    intr( 0x21, &r );
}

#else

void __far SaveSave( char __far *file )
{
    file = file;
}

static int RestSave( char __far *file )
{
    file = file;
    return( 0 );
}
#endif

void InitSave( char __far *name )
{
    if( *name == '\0' || !RestSave( name ) ) {
        FirstCmd = 0;
        CurrCmd =0;
        SaveArea[0] = 0;
        SaveArea[1] = 0;
        LastWasNext = FALSE;
        FirstFree = 2;
    }
}


static void SaveByte( int ch )    /* must be an int (consider OS/2) */
{
    if( FirstFree == FirstCmd ) {
        _ModIndex( FirstCmd, SaveArea[FirstCmd] + 2 );
    }
    SaveArea[FirstFree] = ch;
    _ModIndex( FirstFree, 1 );
}


static void SaveIt( char *cmd, unsigned len )
{
    unsigned    save_len;

    CurrCmd = FirstFree;
    save_len = len;
    SaveByte( save_len );
    do {
        SaveByte( *cmd );
        ++cmd;
    } while( --len != 0 );
    SaveByte( save_len );
}



int DelCmd( char *cmd )
{
    unsigned    len;
    unsigned    src,dst;
    unsigned    cnt,curr;


    if( !LastWasNext ) {
        _ModIndex( CurrCmd, SaveArea[CurrCmd] + 2 );
    }
    src = CurrCmd;
    len = SaveArea[CurrCmd] + 2;
    _ModIndex( src, len );
    dst = CurrCmd;
    while( src != FirstFree ) {
        SaveArea[dst] = SaveArea[src];
        _ModIndex( dst, 1 );
        _ModIndex( src, 1 );
    }
    _ModIndex( FirstFree, -len );
    curr = CurrCmd;
    len = SaveArea[curr];
    cnt = len;
    do {
        _ModIndex( curr, 1 );
        *cmd = SaveArea[curr];
        ++cmd;
    } while( !( --cnt == 0 ) );
    LastWasNext = TRUE;
    if( CurrCmd == FirstFree )
        return( 0 );
    return( len );
}


void SaveCmd( char *cmd, unsigned len )
{
    unsigned  chk, chk_len;

    LastWasNext = FALSE;
    CurrCmd = FirstFree;
    _ModIndex( CurrCmd, -1 );
    _ModIndex( CurrCmd, -(SaveArea[CurrCmd] + 1) );
    if( len != 0 ) {
        chk = CurrCmd;
        if( SaveArea[chk] != len ) {
            SaveIt( cmd, len );
        } else {
            for( chk_len = 0; chk_len < len; ++chk_len ) {
                _ModIndex( chk, 1 );
                if( SaveArea[chk] != cmd[chk_len] ) {
                    SaveIt( cmd, len );
                    return;
                }
            }
        }
    }
}


int PrevCmd( char *cmd )
{
    unsigned  len;
    unsigned  cnt;
    unsigned  curr;

    if( CurrCmd == FirstCmd ) {
        LastWasNext = TRUE;
        return( 0 );
    }
    curr = CurrCmd;
    _ModIndex( CurrCmd, -1 );
    _ModIndex( CurrCmd, -(SaveArea[CurrCmd] + 1) );
    if( LastWasNext ) {
        if( CurrCmd == FirstCmd ) return( 0 );
        curr = CurrCmd;
        _ModIndex( CurrCmd, -1 );
        _ModIndex( CurrCmd, -(SaveArea[CurrCmd] + 1) );
    }
    LastWasNext = FALSE;
    len = SaveArea[curr];
    cnt = len;
    do {
        _ModIndex( curr, 1 );
        *cmd = SaveArea[curr];
        ++cmd;
    } while( !( --cnt == 0 ) );
    return( len );
}


int NextCmd( char *cmd )
{
    unsigned  len;
    unsigned  cnt;
    unsigned  curr;

    curr = CurrCmd;
    _ModIndex( curr, SaveArea[curr] + 2 );
    if( !LastWasNext ) {
        if( curr == FirstFree )
            return( 0 );
        CurrCmd = curr;
        _ModIndex( curr, SaveArea[curr] + 2 );
    }
    LastWasNext = FALSE;
    if( curr == FirstFree )
        return( 0 );
    LastWasNext = TRUE;
    CurrCmd = curr;
    len = SaveArea[curr];
    cnt = len;
    do {
        _ModIndex( curr, 1 );
        *cmd = SaveArea[curr];
        ++cmd;
    } while( !( --cnt == 0 ) );
    return( len );
}

void ListCommands( void )
/***********************/
{
    int         i;
    char        prompt[80];
    int         prevs;

    SaveLine();
    SavePrompt( prompt );
    prevs = 0;
    while( PrevCmd( Line ) )
        --prevs;
    PutNL();
    i = 0;
    for( ;; ) {
        Line[NextCmd( Line )] = '\0';
        if( Line[0] == '\0' )
            break;
        ++prevs;
        if( ++i == 23 ) {
            if( !PutMore() )
                break;
            i = 0;
            continue;
        }
        PutString( Line );
        PutNL();
    }
    while( prevs-- )
        PrevCmd( Line );
    RestorePrompt( prompt );
    RestoreLine();
    Draw = TRUE;
}


void RetrieveACommand( int (* advance)(char *) )
/**********************************************/
{
    MaxCursor = advance( Line );
    FirstNextOrPrev = 0;
    Cursor = MaxCursor;
    Draw = TRUE;
    Base = 0;
    Edited = FALSE;
}
