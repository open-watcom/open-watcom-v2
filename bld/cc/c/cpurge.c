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
* Description:  Purge non-permanent memory. Mostly obsolete.
*
****************************************************************************/


#include "cvars.h"


extern  void    FreeFNames( void );
extern  void    FreeRDir( void );
extern  void    SymsPurge( void );


void InitPurge( void )
/********************/
{
}


static void SrcPurge( void )
/*******************/
{
    FCB *src_file;

    while( (src_file = SrcFile) != NULL ) {
        SrcFile = src_file->prev_file;
        CClose( src_file->src_fp );
        FEfree( src_file->src_buf );
        CMemFree( src_file );
    }
}

static void Purge( char **ptr )
{
    if( *ptr != NULL ) {
        CMemFree( *ptr );
        *ptr = NULL;
    }
}

void PurgeMemory( void )
/**********************/
{
//  MacroPurge();
    SymsPurge();        /* calls TypesPurge */
    FreeDataQuads();
    FreeFNames();
    FreeRDir();
    SrcPurge();
    SwitchPurge();
    Purge( &IncPathList );
//  Purge( &ErrSym );               /* ErrSym is in CPermArea */
    Purge( &SavedId );
    FreePreCompiledHeader();
}
