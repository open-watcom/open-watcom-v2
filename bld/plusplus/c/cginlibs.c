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

#include "plusplus.h"
#include "memmgr.h"
#include "ring.h"
#include "pcheader.h"
#include "cginlibs.h"

enum {
    LIB_USER_PRIORITY   = '9',
};

typedef struct lib_list LIB_LIST;
struct lib_list {
    LIB_LIST    *next;
    char        name[1];        // first char is priority, rest is name
};

static LIB_LIST *libRing;

static LIB_LIST *addNewLib( char *name, char priority )
{
    size_t len;
    LIB_LIST *new_lib;

    len = strlen( name );
    new_lib = CMemAlloc( sizeof( *new_lib ) + 1 + len );
    new_lib->name[0] = priority;
    strcpy( &(new_lib->name[1]), name );
    RingAppend( &libRing, new_lib );
    return( new_lib );
}

void CgInfoAddUserLib( char *name )
/*********************************/
{
    addNewLib( name, LIB_USER_PRIORITY );
}


void CgInfoAddCompLib( char *name )
/*********************************/
{
    addNewLib( name + 1, name[0] );
}

void *CgInfoLibNext( void *h )
/****************************/
{
    return( RingStep( libRing, h ) );
}

char *CgInfoLibName( void *h )
/****************************/
{
    return( ((LIB_LIST *)h)->name );
}

void CgInfoFreeLibs( void )
/*************************/
{
    RingFree( &libRing );
}

void CgInfoLibPCHRead( void )
/***************************/
{
    size_t len;
    LIB_LIST *lib_entry;

    CgInfoFreeLibs();
    for(;;) {
        len = PCHReadUInt();
        if( len == 0 ) break;
        lib_entry = CMemAlloc( len );
        PCHRead( lib_entry, len );
        RingAppend( &libRing, lib_entry );
    }
}

void CgInfoLibPCHWrite( void )
/****************************/
{
    size_t len;
    LIB_LIST *lib;

    RingIterBeg( libRing, lib ) {
        len = sizeof( *lib ) + strlen( lib->name );
        PCHWriteUInt( len );
        PCHWrite( lib, len );
    } RingIterEnd( lib )
    len = 0;
    PCHWriteUInt( len );
}
