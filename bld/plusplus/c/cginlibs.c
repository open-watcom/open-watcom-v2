/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
    char            priority;   // priority '1'-'9'
    char            libname[1]; // library name
} lib_list;

static lib_list *libHead;

static void addNewLib( const char *libname, char priority )
/*********************************************************/
{
    lib_list    **owner;
    lib_list    *lib;

    for( owner = &libHead; (lib = *owner) != NULL; owner = &lib->next ) {
        if( lib->priority < priority ) {
            lib_list    **tmp_owner;
            /*
             * search library entry with lower priority
             */
            for( tmp_owner = owner; (lib = *tmp_owner) != NULL; tmp_owner = &lib->next ) {
                if( strcmp( lib->libname, libname ) == 0 ) {
                    /*
                     * remove library entry from linked list
                     */
                    *tmp_owner = lib->next;
                    break;
                }
            }
            break;
        }
        if( strcmp( lib->libname, libname ) == 0 ) {
            /*
             * library already exists with higher or equal priority
             * no change required
             */
            return;
        }
    }
    /*
     * if library entry not found then create new one
     */
    if( lib == NULL ) {
        lib = CMemAlloc( sizeof( lib_list ) + strlen( libname ) );
        strcpy( lib->libname, libname );
    }
    /*
     * set priority and insert library entry to proper position
     */
    lib->priority = priority;
    lib->next = *owner;
    *owner = lib;
}

void CgInfoAddUserLib( const char *libname )
/******************************************/
{
    addNewLib( libname, LIB_USER_PRIORITY );
}


void CgInfoAddCompLib( const char *name )
/***************************************/
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
    unsigned    len;
    lib_list    *lib;

    for( lib = libHead; lib != NULL; lib = lib->next ) {
        len = offsetof( lib_list, libname ) + strlen( lib->libname ) + 1;
        PCHWriteUInt( len );
        PCHWrite( lib, len );
    }
    PCHWriteUInt( 0 );
}
