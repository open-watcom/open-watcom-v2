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
* Description:  linker spill file i/o  routines
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "spillio.h"

static char *           TFileName;
static unsigned long    TmpFSize;

void InitSpillFile( void )
/*******************************/
{
    TempFile = NIL_FHANDLE;
    TFileName = NULL;
    TmpFSize = 0;
    SetBreak();
}

#define TEMPFNAME "WLK02112.xx`"        // "'" will be an "a" when processed.
#define TEMPFNAME_SIZE 13

static char * MakeTempName( char *name )
/**************************************/
{
    memcpy( name, TEMPFNAME, sizeof(TEMPFNAME) );   // includes nullchar
    return( name + sizeof(TEMPFNAME) - 2 );         // pointer to "a"
}

f_handle OpenTempFile( char **fname )
/******************************************/
{
    char *      ptr;
    unsigned    tlen;
    char *      tptr;
    f_handle    fhdl;

    ptr = GetEnvString( "WLINKTMP" );
    if( ptr == NULL ) ptr = GetEnvString( "TMP" );
    if( ptr == NULL ) ptr = GetEnvString( "TMPDIR" );
    if( ptr == NULL ) {
        _ChkAlloc( *fname, TEMPFNAME_SIZE );
        tptr = *fname;
    } else {
        tlen = strlen( ptr );
        _ChkAlloc( *fname, tlen + 1 + TEMPFNAME_SIZE );
        memcpy( *fname, ptr, tlen );
        switch( (*fname)[tlen-1] ) {
        CASE_PATH_SEP:
            break;
        default:
            (*fname)[tlen++] = PATH_SEP;
            break;
        }
        tptr = *fname + tlen;
    }
    ptr = MakeTempName( tptr );
    tlen = 0;
    for( ;; ) {
        if( tlen >= 26 ) {
            LnkMsg( FTL+MSG_CANT_OPEN_SPILL, NULL );
        }
        *ptr += 1;                          // change temp file extension
        fhdl = TempFileOpen( *fname );
        if( fhdl == NIL_FHANDLE ) break;
        QClose( fhdl, *fname );
        ++tlen;
    }
    return QOpenRW( *fname );
}

unsigned long SpillAlloc( unsigned amt )
/*********************************************/
{
    unsigned long           stg;

    if( TempFile == NIL_FHANDLE ) {
        TempFile = OpenTempFile( &TFileName );
        LnkMsg( INF+MSG_USING_SPILL, NULL );
    }
    /* round up storage start to a disk sector boundry -- assumed power of 2 */
    TmpFSize += SECTOR_SIZE-1;
    TmpFSize &= ~(SECTOR_SIZE-1);
    stg = TmpFSize;
    TmpFSize += amt;
    return( stg + 1 );  /* add 1 to prevent a NULL handle */
}

void SpillNull( unsigned long base, unsigned off, unsigned size )
/**********************************************************************/
{
    QSeek( TempFile, base + off - 1, TFileName );
    WriteNulls( TempFile, size, TFileName );
}

void SpillWrite( unsigned long base, unsigned off, void *mem,
                                                          unsigned size )
/***********************************************************************/
{
    QSeek( TempFile, base + off - 1, TFileName );
    QWrite( TempFile, mem, size, TFileName );
}

void SpillRead( unsigned long base, unsigned off, void *mem,
                                                         unsigned size )
/**********************************************************************/
{
    QSeek( TempFile, base + off - 1, TFileName );
    QRead( TempFile, mem, size, TFileName );
}

void CloseSpillFile( void )
/********************************/
/*  Close temporary file.  */
{
    if( TempFile != NIL_FHANDLE ) {
        RestoreBreak();
        QClose( TempFile, TFileName );
        QDelete( TFileName );
        _LnkFree( TFileName );
        TFileName = NULL;
        TempFile = NIL_FHANDLE;
    }
}

