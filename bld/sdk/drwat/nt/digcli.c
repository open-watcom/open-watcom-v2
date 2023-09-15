/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "drwatcom.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "digld.h"
#include "dip.h"
#include "dipimp.h"
#include "madregs.h"
#include "pathgrp2.h"

#include "clibext.h"


#define QQSTR(x)    # x
#define QSTR(x)     QQSTR(x)

void *DIGCLIENTRY( Alloc )( size_t size )
/****************************************
 * DIGCliAlloc
 */
{
    return( MemAlloc( size ) );
}

void *DIGCLIENTRY( Realloc )( void *ptr, size_t size )
/*****************************************************
 * DIGCliRealloc
 */
{
    return( MemRealloc( ptr, size ) );
}

void DIGCLIENTRY( Free )( void *ptr )
/************************************
 * DIGCliFree
 */
{
    MemFree( ptr );
}

/*
 * Windows NT must use OS loader handle for I/O file function for DLL/multi-thread support
 * For DIG client functions appropriate Windows HANDLE oriented I/O functions must be used
 * !!! ISO or POSIX functions must not be used !!!
 */

FILE * DIGCLIENTRY( Open )( const char *path, dig_open mode )
/************************************************************
 * DIGCliOpen
 */
{
    HFILE               ret;
    int                 flags;
    OFSTRUCT            tmp;

    flags = 0;
    if( mode & DIG_OPEN_READ )
        flags |= OF_READ;
    if( mode & DIG_OPEN_WRITE )
        flags |= OF_WRITE;
    if( mode & DIG_OPEN_TRUNC )
        flags |= OF_CREATE;
    if( mode & DIG_OPEN_CREATE )
        flags |= OF_CREATE;
    /*
     * NYI: should check for DIG_OPEN_SEARCH
     */
    ret = OpenFile( path, &tmp, flags );
    if( ret == HFILE_ERROR )
        return( NULL );
    return( WH2FP( (HANDLE)ret ) );
}

int DIGCLIENTRY( Seek )( FILE *fp, unsigned long offset, dig_seek where )
/************************************************************************
 * DIGCliSeek
 */
{
    int         mode;

    switch( where ) {
    case DIG_SEEK_ORG:
        mode = FILE_BEGIN;
        break;
    case DIG_SEEK_CUR:
        mode = FILE_CURRENT;
        break;
    case DIG_SEEK_END:
        mode = FILE_END;
        break;
    }
    return( SetFilePointer( FP2WH( fp ), offset, 0, mode ) == INVALID_SET_FILE_POINTER );
}

unsigned long DIGCLIENTRY( Tell )( FILE *fp )
/********************************************
 * DIGCliTell
 */
{
    return( SetFilePointer( FP2WH( fp ), 0, 0, FILE_CURRENT ) );
}

size_t DIGCLIENTRY( Read )( FILE *fp, void *buf, size_t size )
/*************************************************************
 * DIGCliRead
 */
{
    DWORD       bytesread;

    if( !ReadFile( FP2WH( fp ), buf, size, &bytesread, NULL ) )
        return( (size_t)-1 );
    return( bytesread );
}

size_t DIGCLIENTRY( Write )( FILE *fp, const void *buf, size_t size )
/********************************************************************
 * DIGCliWrite
 */
{
    DWORD       byteswritten;

    if( !WriteFile( FP2WH( fp ), buf, size, &byteswritten, NULL ) )
        return( (size_t)-1 );
    return( byteswritten );
}

void DIGCLIENTRY( Close )( FILE *fp )
/************************************
 * DIGCliClose
 */
{
    CloseHandle( FP2WH( fp ) );
}

void DIGCLIENTRY( Remove )( const char *path, dig_open mode )
/************************************************************
 * DIGCliRemove
 */
{
    /* unused params */ (void)mode;

    DeleteFile( path );
}

size_t DIGLoader( Find )( dig_filetype ftype, const char *base_name, size_t base_name_len,
                                const char *defext, char *filename, size_t filename_maxlen )
/*******************************************************************************************
 * DIGLoaderFind
 */
{
    char        fname[256];
    size_t      len;

    /* unused parameters */ (void)ftype;

    len = 0;
    if( filename_maxlen > 0 ) {
        filename_maxlen--;
        if( base_name_len == 0 )
            base_name_len = strlen( base_name );
        strncpy( fname, base_name, base_name_len );
        strcpy( fname + base_name_len, defext );
#ifdef BLDVER
        _searchenv( fname, "WD_PATH" QSTR( BLDVER ), fname );
#endif
        len = strlen( fname );
        if( len > filename_maxlen )
            len = filename_maxlen;
        strncpy( filename, fname, len );
        filename[len] = '\0';
    }
    return( len );
}
