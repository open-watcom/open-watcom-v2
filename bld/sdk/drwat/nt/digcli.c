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


#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "drwatcom.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "mem.h"


/*
 * DIGCliAlloc
 */
void *DIGCLIENTRY( Alloc )( size_t size )
{
    return( MemAlloc( size ) );
}

/*
 * DIGCliRealloc
 */
void *DIGCLIENTRY( Realloc )( void *ptr, size_t size )
{
    return( MemReAlloc( ptr, size ) );
}

/*
 * DIGCliFree
 */
void DIGCLIENTRY( Free )( void *ptr )
{
    MemFree( ptr );
}

/*
 * DIGCliOpen
 */
FILE * DIGCLIENTRY( Open )( const char *path, dig_open mode )
{
    const char  *access;

    if( mode & DIG_APPEND ) {
        access = "ab";
    } else if( mode & (DIG_WRITE | DIG_CREATE) ) {
        access = "wb";
    } else {
        access = "rb";
    }
    return( fopen( path, access ) );
}

/*
 * DIGCliSeek
 */
int DIGCLIENTRY( Seek )( FILE *fp, unsigned long offset, dig_seek dipmode )
{
    int         mode;

    switch( dipmode ) {
    case DIG_ORG:
        mode = SEEK_SET;
        break;
    case DIG_CUR:
        mode = SEEK_CUR;
        break;
    case DIG_END:
        mode = SEEK_END;
        break;
    }
    return( fseek( fp, offset, mode ) );
}

/*
 * DIGCliTell
 */
unsigned long DIGCLIENTRY( Tell )( FILE *fp )
{
    return( ftell( fp ) );
}

/*
 * DIGCliRead
 */
size_t DIGCLIENTRY( Read )( FILE *fp, void *buf, size_t size )
{
    return( fread( buf, 1, size, fp ) );
}

/*
 * DIGCliWrite
 */
size_t DIGCLIENTRY( Write )( FILE *fp, const void *buf, size_t size )
{
    return( fwrite( buf, 1, size, fp ) );
}

/*
 * DIGCliClose
 */
void DIGCLIENTRY( Close )( FILE *fp )
{
    fclose( fp );
}

/*
 * DIGCliRemove
 */
void DIGCLIENTRY( Remove )( const char *path, dig_open mode )
{
    mode = mode;
    remove( path );
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    enum x86_addr_characteristics       *a_char;

    switch( SysConfig.mad ) {
    case MAD_X86:
        a_char = out;
        *a_char = X86AC_BIG;
        return( sizeof( *a_char ) );
    }
    return( 0 );
}
