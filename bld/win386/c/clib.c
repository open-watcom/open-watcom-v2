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
* Description:  Wrappers for miscellaneous C library functions.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <io.h>
#include <bios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dos.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include "fints.h"
#include "clibxw32.h"


#ifdef DLL32
#define STATIC static
#else
#define STATIC
#endif

/*
 * here lie all interrupt functions
 */

int __far __pascal _clib_intdos( union REGS __far *in_regs,
                             union REGS __far *out_regs )
{
    return( _fintdos( in_regs, out_regs ) );
}

int __far __pascal _clib_intdosx( union REGS __far *in_regs, union REGS __far *out_regs,
                              struct SREGS __far *seg_regs ) {
        return( _fintdosx( in_regs, out_regs, seg_regs ) );
}

int __far __pascal _clib_int86( int inter_no, union REGS __far *in_regs,
                            union REGS __far *out_regs )
{
    return( _fint86( inter_no, in_regs, out_regs ) );
}

int __far __pascal _clib_int86x( int inter_no, union REGS __far *in_regs,
                            union REGS __far *out_regs,
                            struct SREGS __far *seg_regs )
{
    return( _fint86x( inter_no, in_regs, out_regs, seg_regs ) );
}

void __far __pascal _clib_intr( int inter_no, union REGPACK __far *regs )
{
    _fintr( inter_no, regs );
}

/*
 * _dos functions are here - note that all pointers to ints must be
 * pointers to longs, since we are passing the results back to a 32
 * bit application
 */

unsigned __far __pascal _clib_dos_findfirst( char __far *__path,unsigned __attr,
                                struct find_t __far *__buf )
{
#if 1
    return 0;
#else
    char                        *path;
    unsigned                    rc;
    STATIC struct find_t        buf;

    path = malloc( _fstrlen( __path ) + 1 );
    if( path == NULL ) return( -1 );
    _fstrcpy( path, __path );

    rc = _dos_findfirst( path, __attr, &buf );
    _fmemcpy( __buf, &buf, sizeof( struct find_t ) );

    free( path );
    return( rc );
#endif

} /* _clib_dos_findfirst */

unsigned __far __pascal _clib_dos_findnext( struct find_t __far *__buf )
{
#if 1
    return 0;
#else
    STATIC struct find_t        buf;
    unsigned                    rc;

    _fmemcpy( &buf, __buf, sizeof( struct find_t ) );
    rc = _dos_findnext( &buf );
    _fmemcpy( __buf, &buf, sizeof( struct find_t ) );
    return( rc );
#endif

} /* _clib_dos_findnext */


unsigned __far __pascal _clib_dos_read( int __handle, void __far *__buf, unsigned __count, unsigned long __far *__bytes  )
{
    unsigned            rc;
    STATIC unsigned     bytes;

    rc = _dos_read( __handle, __buf, __count, &bytes  );
    *__bytes = (unsigned long) bytes;
    return( rc );

} /* _clib_dos_read */

unsigned __far __pascal _clib_dos_write( int __handle, void __far *__buf, unsigned __count, unsigned long __far *__bytes  )
{
    unsigned            rc;
    STATIC unsigned     bytes;

    rc = _dos_write(  __handle, __buf, __count, &bytes  );
    *__bytes = (unsigned long) bytes;
    return( rc );

} /* _clib_dos_write */

/*
 * BIOS functions here
 */
unsigned short __far __pascal _clib_bios_disk( unsigned __cmd, struct diskinfo_t __far *__diskinfo )
{
    STATIC struct diskinfo_t    diskinfo;
    unsigned short              rc;

    rc = _bios_disk( __cmd, &diskinfo );
    _fmemcpy( __diskinfo, &diskinfo, sizeof( struct diskinfo_t ) );
    return( rc );
}

unsigned short __far __pascal _clib_bios_equiplist(void)
{
        return( _bios_equiplist() );
}

unsigned short __far __pascal _clib_bios_keybrd( unsigned __cmd )
{
    return( _bios_keybrd( __cmd ) );
}

unsigned short __far __pascal _clib_bios_memsize( void )
{
    return( _bios_memsize() );
}

unsigned short __far __pascal _clib_bios_printer( unsigned __cmd, unsigned __port, unsigned __data )
{
    return( _bios_printer( __cmd, __port, __data ) );
}

unsigned short __far __pascal _clib_bios_serialcom( unsigned __cmd, unsigned __port, unsigned __data )
{
    return( _bios_serialcom( __cmd, __port, __data ) );
}

unsigned short __far __pascal _clib_bios_timeofday( unsigned __cmd, long __far *__timeval )
{
    STATIC long     timeval;
    unsigned short  rc;

    if( __cmd == _TIME_GETCLOCK ) {
        rc = _bios_timeofday( __cmd, &timeval );
        *__timeval = timeval;
    } else {
        timeval = *__timeval;
        rc = _bios_timeofday( __cmd, &timeval );
    }
    return( rc );
}

/*
 * various and sundry clib functions here
 */
clock_t __far __pascal  _clib_clock( void )
{
    return( clock() );
}
void __far __pascal _clib_delay( unsigned long ms )
{
    while( ms > 0xFFFFL ) {
        delay( 0xFFFFL );
        ms -= 0xFFFFL;
    }
    delay( ms );
}

int __far __pascal _clib_errno( void )
{
    return( errno );
}
