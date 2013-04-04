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


#include "plusplus.h"
#include "memmgr.h"
#include "ring.h"
#include "pcheader.h"
#include "cginlibs.h"

enum {
    LIB_USER_PRIORITY   = '9',
};

typedef struct lib_list {
    struct lib_list *next;
    char            libname[1];         // first char is priority, rest is name
} lib_list;

static lib_list *libHead;

static void addNewLib( char *name, char priority )
/************************************************/
{
    lib_list    **next_owner;
    lib_list    **new_owner;
    lib_list    **old_owner;
    lib_list    *lib;
    int         len;

    new_owner = &libHead;
    old_owner = NULL;
    for( next_owner = &libHead; (lib = *next_owner) != NULL; next_owner = &lib->next ) {
        if( lib->libname[0] < priority ) {
            if( old_owner == NULL && strcmp( lib->libname + 1, name ) == 0 ) {
                old_owner = next_owner;
            }
        } else {
            new_owner = &lib->next;
            if( strcmp( lib->libname + 1, name ) == 0 ) {
                new_owner = NULL;
                break;
            }
        }
    }
    if( old_owner != NULL ) {
        lib = *old_owner;
        *old_owner = lib->next;
    } else if( new_owner != NULL ) {
        len = strlen( name );
        lib = CMemAlloc( offsetof( lib_list, libname ) + len + 2 );
        memcpy( lib->libname + 1, name, len + 1 );
    }
    if( new_owner != NULL ) {
        lib->libname[0] = priority;
        lib->next = *new_owner;
        *new_owner = lib;
    }
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
    if( h == NULL ) {
        return( libHead );
    } else {
        return( ((lib_list *)h)->next );
    }
}

char *CgInfoLibName( void *h )
/****************************/
{
    return( ((lib_list *)h)->libname );
}

void CgInfoFreeLibs( void )
/*************************/
{
    lib_list    *lib;
    lib_list    *next_lib;

    for( lib = libHead; lib != NULL; lib = next_lib ) {
        next_lib = lib->next;
        CMemFree( lib );
    }
    libHead = NULL;
}

void CgInfoLibPCHRead( void )
/***************************/
{
    unsigned    len;
    lib_list    *lib;
    lib_list    **owner;

    CgInfoFreeLibs();
    owner = &libHead;
    for( ; (len = PCHReadUInt()) != 0; ) {
        lib = CMemAlloc( len );
        *owner = lib;
        PCHRead( lib, len );
        lib->next = NULL;
        owner = &lib->next;
    }
}

void CgInfoLibPCHWrite( void )
/****************************/
{
    size_t len;
    lib_list *lib;

    for( lib = libHead; lib != NULL; lib = lib->next ) {
        len = sizeof( *lib ) + strlen( lib->libname );
        PCHWriteUInt( len );
        PCHWrite( lib, len );
    }
    len = 0;
    PCHWriteUInt( len );
}
