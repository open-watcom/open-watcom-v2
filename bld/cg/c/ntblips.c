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


#undef FAR
#include "zoiks.h"
#include "cgdefs.h"
#include "coderep.h"
#include "onexit.h"
#include "blips.h"
#include "feprotos.h"

#if 0
static    int           TGCount;
static    int           LPCount;
static    int           URCount;
static    int           SXCount;
static    int           SCCount;
static    int           EXCount;
static    int           GRCount;
static    int           IMCount;
static    int           PSCount;
static    int           PLCount;
static    int           DGCount;
static    bool          Zoiks2;
#endif


bool                    BlipsOn;
// static HANDLE        blipHandle;
// static file_mapping  *fileMapping;


void    InitBlip() {
/******************/

#if 0
    char        *src_name;

    BlipsOn = false;
    blipHandle = OpenFileMapping( FILE_MAP_WRITE, false, SHMEM_NAME );
    if( blipHandle != NULL ) {
        fileMapping = MapViewOfFile( blipHandle, FILE_MAP_WRITE, 0, 0, 0 );
        if( fileMapping == NULL ) {
            _Zoiks( ZOIKS_110 );
            return;
        }
        BlipsOn = true;
        src_name = FEAuxInfo( NULL, SOURCE_NAME );
        strncpy( &fileMapping->file_name[0], src_name, _MAX_PATH );
        strcpy( &fileMapping->rtn_name[0], "" );
        fileMapping->debug.debug_info = false;
    }
#endif
}

void    FiniBlip() {
/******************/

#if 0
    if( fileMapping != NULL ) {
        UnmapViewOfFile( fileMapping );
    }
    if( blipHandle != NULL ) {
        CloseHandle( blipHandle );
    }
#endif
}

bool    WantZoiks2() {
/********************/

    return( false );
}


void    LNBlip( source_line_number num )
/**************************************/
{
    num = num;
}


void    PGBlip( const char *name )
/********************************/
{
    name = name;
    // strncpy( &fileMapping->rtn_name[0], name, MAX_RTN );
}


void    TGBlip() {
/****************/
}

void    LPBlip() {
/****************/
}

void    URBlip() {
/****************/
}

void    SXBlip() {
/****************/
}

void    EXBlip() {
/****************/
}

void    GRBlip() {
/****************/
}

void    IMBlip() {
/****************/
}

void    SCBlip() {
/****************/
}

void    PSBlip() {
/****************/
}

void    PLBlip() {
/****************/
}

void    DGBlip() {
/****************/
}

#if 0
static  void    CheckEvents() {
/*****************************/
    if( TBreak() ) {
        FatalError( "Program interrupted from keyboard" );
    }
}
#endif
