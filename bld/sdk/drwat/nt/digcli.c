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


#if defined( _M_IX86 )
#define MD_x86
#elif defined __AXP__
#define MD_axp
#endif

#include "drwatcom.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "madregs.h"


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
    return( MemRealloc( ptr, size ) );
}

/*
 * DIGCliFree
 */
void DIGCLIENTRY( Free )( void *ptr )
{
    MemFree( ptr );
}

/*
 * Windows NT must use OS loader handle for I/O file function for DLL/multi-thread support
 * For DIG client functions appropriate Windows HANDLE oriented I/O functions must be used
 * !!! ISO or POSIX functions must not be used !!!
 */

/*
 * DIGCliOpen
 */
FILE * DIGCLIENTRY( Open )( const char *path, dig_open mode )
{
    HFILE               ret;
    int                 flags;
    OFSTRUCT            tmp;

    flags = 0;
    if( mode & DIG_READ )
        flags |= OF_READ;
    if( mode & DIG_WRITE )
        flags |= OF_WRITE;
    if( mode & DIG_TRUNC )
        flags |= OF_CREATE;
    if( mode & DIG_CREATE )
        flags |= OF_CREATE;
    //NYI: should check for DIG_SEARCH
    ret = OpenFile( path, &tmp, flags );
    if( ret == HFILE_ERROR )
        return( NULL );
    return( WH2FP( (HANDLE)ret ) );
}

/*
 * DIGCliSeek
 */
int DIGCLIENTRY( Seek )( FILE *fp, unsigned long offset, dig_seek dipmode )
{
    int         mode;

    switch( dipmode ) {
    case DIG_ORG:
        mode = FILE_BEGIN;
        break;
    case DIG_CUR:
        mode = FILE_CURRENT;
        break;
    case DIG_END:
        mode = FILE_END;
        break;
    }
    return( SetFilePointer( FP2WH( fp ), offset, 0, mode ) == INVALID_SET_FILE_POINTER );
}

/*
 * DIGCliTell
 */
unsigned long DIGCLIENTRY( Tell )( FILE *fp )
{
    return( SetFilePointer( FP2WH( fp ), 0, 0, FILE_CURRENT ) );
}

/*
 * DIGCliRead
 */
size_t DIGCLIENTRY( Read )( FILE *fp, void *buf, size_t size )
{
    DWORD       bytesread;

    if( !ReadFile( FP2WH( fp ), buf, size, &bytesread, NULL ) )
        return( (size_t)-1 );
    return( bytesread );
}

/*
 * DIGCliWrite
 */
size_t DIGCLIENTRY( Write )( FILE *fp, const void *buf, size_t size )
{
    DWORD       byteswritten;

    if( !WriteFile( FP2WH( fp ), buf, size, &byteswritten, NULL ) )
        return( (size_t)-1 );
    return( byteswritten );
}

/*
 * DIGCliClose
 */
void DIGCLIENTRY( Close )( FILE *fp )
{
    CloseHandle( FP2WH( fp ) );
}

/*
 * DIGCliRemove
 */
void DIGCLIENTRY( Remove )( const char *path, dig_open mode )
{
    mode = mode;
    DeleteFile( path );
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
#if defined( _M_IX86 )
    /* unused parameters */ (void)addr; (void)info_type; (void)in_size; (void)in; (void)out_size;
#else
    /* unused parameters */ (void)addr; (void)info_type; (void)in_size; (void)in; (void)out_size; (void)out;
#endif

#if defined( _M_IX86 )
    enum x86_addr_characteristics   *a_char;

    switch( SysConfig.arch ) {
    case DIG_ARCH_X86:
        a_char = out;
        *a_char = X86AC_BIG;
        return( sizeof( *a_char ) );
    }
#elif defined( __AXP__ )
  #if 0
    switch( SysConfig.arch ) {
    case DIG_ARCH_AXP:
        memcpy( out, in, sizeof( axp_data ) );
        return( sizeof( axp_data ) );
    }
  #endif
#endif
    return( 0 );
}
