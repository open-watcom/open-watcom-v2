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
#include <string.h>
#include <time.h>
#include "clibxw32.h"
#include "dodoscal.h"
#include "dosret.h"
#include "dointr.h"


#ifdef DLL32
#define STATIC static
#else
#define STATIC
#endif


/*
 * here lie all interrupt functions
 */

int __far __pascal _clib_intdos( union REGS __far *inregs, union REGS __far *outregs )
{
    int         status;

    status = DoDosCall( inregs, outregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}

int __far __pascal _clib_intdosx( union REGS __far *inregs, union REGS __far *outregs,
                              struct SREGS __far *segregs )
{
    int         status;

    status = DoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}

static int __int86x( int intno, union REGS __far *inregs, union REGS __far *outregs,
                                                       struct SREGS __far *segregs )
{
    union REGPACK regs;

    regs.x.ax = inregs->x.ax;
    regs.x.bx = inregs->x.bx;
    regs.x.cx = inregs->x.cx;
    regs.x.dx = inregs->x.dx;
    regs.x.si = inregs->x.si;
    regs.x.di = inregs->x.di;
    regs.x.ds = segregs->ds;
    regs.x.es = segregs->es;
//    regs.x.bp = 0;             /* no bp in REGS union, set to 0 */
//    regs.x.flags = ( inregs->x.cflag ) ? INTR_CF : 0;

    _DoINTR( intno, &regs, 0 );

    outregs->x.ax = regs.x.ax;
    outregs->x.bx = regs.x.bx;
    outregs->x.cx = regs.x.cx;
    outregs->x.dx = regs.x.dx;
    outregs->x.si = regs.x.si;
    outregs->x.di = regs.x.di;
    outregs->x.cflag = ( (regs.x.flags & INTR_CF) != 0 );
    segregs->ds = regs.x.ds;
    segregs->es = regs.x.es;
    return( regs.x.ax );
}

int __far __pascal _clib_int86( int intno, union REGS __far *inregs,
                            union REGS __far *outregs )
{
#ifdef DLL32
    static struct SREGS segregs;
#else
    struct SREGS        segregs;
#endif

    segread( &segregs );
    return( __int86x( intno, inregs, outregs, &segregs ) );
}

int __far __pascal _clib_int86x( int intno, union REGS __far *inregs,
              union REGS __far *outregs, struct SREGS __far *segregs )
{
    return( __int86x( intno, inregs, outregs, segregs ) );
}

void __far __pascal _clib_intr( int intno, union REGPACK __far *regs )
{
    _DoINTR( intno, regs, 0 );
}

void __far __pascal _clib_intrf( int intno, union REGPACK __far *regs )
{
    _DoINTR( intno, regs, regs->w.flags );
}

/*
 * _dos functions are here - note that all pointers to ints must be
 * pointers to longs, since we are passing the results back to a 32
 * bit application
 */

unsigned __far __pascal _clib_dos_findfirst( char __far *__path, unsigned __attr, struct find_t __far *__buf )
{
#if 1
    /* unused parameters */ (void)__path; (void)__attr; (void)__buf;

    return 0;
#else
    char                        *path;
    unsigned                    rc;
    STATIC struct find_t        buf;

    path = malloc( _fstrlen( __path ) + 1 );
    if( path == NULL )
        return( -1 );
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
    /* unused parameters */ (void)__buf;

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
    for( ; ms > 0xFFFFL; ms -= 0xFFFFL ) {
        delay( 0xFFFFL );
    }
    delay( ms );
}

int __far __pascal _clib_errno( void )
{
    return( errno );
}
