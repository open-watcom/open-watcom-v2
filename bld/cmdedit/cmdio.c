/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 cover functions for running under DOS.
*
****************************************************************************/


#include "cmdedit.h"
#include <string.h>


#ifdef __DOS__

static char buff[MAX_FNAME];

union REGPACK r;

static void CurrentPageInBH( void )
{
    static union REGPACK l;
    l.h.ah = 15;
    intr( 0x10, &l );
    r.h.bh = l.h.bh;
}

USHORT DosGetEnv( USHORT PASPTR *segment, USHORT PASPTR *offset )
{
    r.h.ah = 0x62;
    intr( 0x21, &r );
    *segment = *(unsigned __far *)_MK_FP( r.x.bx, 0x2c );
    *offset = 0;
    return( 0 );
}

USHORT DKbdGetStatus( KBDDESC PASPTR *kbd )
{
    kbd->fsMask = KBD_ASCII | KBD_ECHO_ON;
    kbd->chTurnAround = '\r';
    return( 0 );
}

static int Extended = { 1 };
static int First = { 1 };

#define RSH(x)  ( ( ( x ) & 0x0002 ) != 0 )
#define LSH(x)  ( ( ( x ) & 0x0002 ) != 0 )
#define CT(x)   ( ( ( x ) & 0x0004 ) != 0 )
#define AL(x)   ( ( ( x ) & 0x0008 ) != 0 )
#define LCT(x)  ( ( ( x ) & 0x0100 ) != 0 )
#define LAL(x)  ( ( ( x ) & 0x0200 ) != 0 )
#define RCT(x)  ( ( ( x ) & 0x0400 ) != 0 )
#define RAL(x)  ( ( ( x ) & 0x0800 ) != 0 )

static int ChkExtendedKbd( void )
{
    unsigned    x;

    r.h.ah = 0x12;
    r.h.al = 0xff;
    intr( 0x16, &r );
    if( r.h.al == 0xff ) return( 0 ); /* too many damn keys pressed! */
    x = r.x.ax;
    if( AL( x ) != ( RAL( x ) || LAL( x ) ) ) return( 0 );
    if( CT( x ) != ( RCT( x ) || LCT( x ) ) ) return( 0 );
    return( 1 );
}

USHORT DKbdCharIn( KBDCHAR PASPTR *k )
{
    if( First ) {
        Extended = ChkExtendedKbd();
        First = 0;
    }
    for( ;; ) {
        if( Extended ) {
            r.h.ah = 0x11;
        } else {
            r.h.ah = 0x01;
        }
        intr( 0x16, &r );
        if( (r.x.flags & INTR_ZF) == 0 )
            break;
        intr( 0x28, &r );
    }
    if( Extended ) {
        r.h.ah = 0x10;
    } else {
        r.h.ah = 0x00;
    }
    intr( 0x16, &r );
    k->chChar = r.h.al;
    k->chScan = r.h.ah;
    r.h.ah = 0x30;
    intr( 0x21, &r );
    return( 0 );
}


USHORT DVioGetCurPos( USHORT PASPTR *row, USHORT PASPTR *col )
{
    r.h.ah = 3;
    CurrentPageInBH();
    intr( 0x10, &r );
    *row = r.h.dh;
    *col = r.h.dl;
    return( 0 );
}

USHORT DVioSetCurPos( USHORT row, USHORT col )
{
    r.h.ah = 2;
    CurrentPageInBH();
    r.h.dh = row;
    r.h.dl = col;
    intr( 0x10, &r );
    return( 0 );
}

USHORT DVioWrtCharStr( CHAR PASPTR *ch, USHORT len, USHORT row, USHORT col )
{
    USHORT oldrow, oldcol;

    DVioGetCurPos( &oldrow, &oldcol );
    DVioSetCurPos( row, col );
    while( len-- > 0 ) {
        CurrentPageInBH();
        r.h.ah = 14;
        r.h.al = *ch;
        intr( 0x10, &r );
        ++ch;
    }
    DVioSetCurPos( oldrow, oldcol );
    return( 0 );
}

USHORT DVioReadCharStr( CHAR PASPTR *ch, USHORT PASPTR *len_p, USHORT row, USHORT col )
{
    int len;
    USHORT oldrow, oldcol;

    DVioGetCurPos( &oldrow, &oldcol );
    len = *len_p;
    while( --len >= 0 ) {
        DVioSetCurPos( row, col );
        r.h.ah = 0x08;
        CurrentPageInBH();
        intr( 0x10, &r );
        *ch = r.h.al;
        ++ch;
        ++col;
        if( col == SCREEN_WIDTH ) {
            col = 0;
            ++row;
        }
    }
    DVioSetCurPos( oldrow, oldcol );
    return( 0 );
}

USHORT DVioWrtNChar( UCHAR PASPTR *ch, int times, USHORT row, USHORT col )
{
    USHORT oldrow, oldcol;

    DVioGetCurPos( &oldrow, &oldcol );
    DVioSetCurPos( row, col );
    while( --times >= 0 ) {
        VioWrtCharStr( (CHAR PASPTR *)ch, 1, row, col, 0 );
        ++col;
        if( col == SCREEN_WIDTH ) {
            col = 0;
            ++row;
        }
    }
    DVioSetCurPos( oldrow, oldcol );
    return( 0 );
}

USHORT DVioSetCurType( CURSOR PASPTR *cur )
{
    r.h.ah = 1;
    r.h.ch = cur->yStart;
    r.h.cl = cur->cEnd;
    CurrentPageInBH();
    intr( 0x10, &r );
    return( 0 );
}

USHORT DVioGetCurType( CURSOR PASPTR *cur )
{
    r.h.ah = 3;
    CurrentPageInBH();
    intr( 0x10, &r );
    cur->cEnd = r.h.cl;
    cur->yStart = r.h.ch;
    return( 0 );
}

USHORT DVioReadCellStr( CHAR PASPTR *buff, USHORT PASPTR *plen, USHORT row, USHORT col )
{
    int len;
    USHORT oldrow, oldcol;

    DVioGetCurPos( &oldrow, &oldcol );
    DVioSetCurPos( row, col );
    for( len = *plen; len >= 2; len -= 2 ) {
        CurrentPageInBH();
        r.h.ah = 8;
        intr( 0x10, &r );
        buff[0] = r.h.al;
        buff[1] = r.h.ah;
        buff += 2;
    }
    DVioSetCurPos( oldrow, oldcol );
    return( 0 );
}

USHORT DVioWrtCellStr( CHAR PASPTR *buff, USHORT len, USHORT row, USHORT col )
{
    USHORT oldrow, oldcol;

    DVioGetCurPos( &oldrow, &oldcol );
    DVioSetCurPos( row, col );
    for( ; len >= 2; len -= 2 ) {
        CurrentPageInBH();
        r.h.ah = 9;
        r.h.al = buff[0];
        r.h.bl = buff[1];
        r.x.cx = 1;
        intr( 0x10, &r );
        buff += 2;
    }
    DVioSetCurPos( oldrow, oldcol );
    return( 0 );
}

USHORT DosClose( USHORT hdl )
{
    r.h.ah = 0x3e;
    r.x.bx = hdl;
    intr( 0x21, &r );
    return( 0 );
}

USHORT DDosOpen( char PASPTR *name, USHORT PASPTR *hdl )
{
    r.x.dx = _FP_OFF( name );
    r.x.ds = _FP_SEG( name );
    r.h.al = 0;
    r.h.ah = 0x3d;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF )
        return( -1 );
    *hdl = r.x.ax;
    return( 0 );
}


USHORT DosChgFilePtr( int hdl, long offset, int typ, unsigned long PASPTR *newp )
{
    r.x.bx = hdl;
    r.x.cx = offset >> 16;
    r.x.dx = offset & 0xFFFF;
    r.h.al = typ;
    r.h.ah = 0x42;
    intr( 0x21, &r );
    offset = r.x.dx;
    offset <<= 16;
    offset |= r.x.ax;
    *newp = offset;
    return( 0 );
}


USHORT DosRead( USHORT hdl, char __far *buff, USHORT len, USHORT PASPTR *readlen )
{
    r.x.bx = hdl;
    r.x.ds = _FP_SEG( buff );
    r.x.dx = _FP_OFF( buff );
    r.x.cx = len;
    r.h.ah = 0x3F;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF ) {
        *readlen = 0;
        return( -1 );
    } else {
        *readlen = r.x.ax;
        return( 0 );
    }
}

USHORT DosWrite( USHORT hdl, char __far *buff, USHORT len, USHORT PASPTR *writelen )
{
    r.x.bx = hdl;
    r.x.ds = _FP_SEG( buff );
    r.x.dx = _FP_OFF( buff );
    r.x.cx = len;
    r.h.ah = 0x40;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF ) {
        *writelen = 0;
        return( -1 );
    } else {
        *writelen = r.x.ax;
        return( 0 );
    }
}

USHORT DDosAllocSeg( unsigned size, int PASPTR *segp )
{
    size += 16;
    size /= 16;
    r.x.bx = size;
    r.h.ah = 0x48;
    intr( 0x21, &r );
    *segp = r.x.ax;
    return( 0 );
}

int DosFreeEnv( void )
{
    r.h.ah = 0x62;
    intr( 0x21, &r );
    r.h.ah = 0x49;
    r.x.es = *(unsigned __far *)_MK_FP( r.x.bx, 0x2c );
    intr( 0x21, &r );
    return( 0 );
}

static int SavDTAOff;
static int SavDTASeg;

static void setdta( int seg, int off )
{
    r.h.ah = 0x2F;
    intr( 0x21, &r );
    SavDTAOff = r.x.bx;
    SavDTASeg = r.x.es;
    r.x.dx = (unsigned)off;
    r.x.ds = seg;
    r.h.ah = 0x1A;
    intr( 0x21, &r );
}

static void resetdta( void )
{
    union REGPACK r;

    r.x.dx = SavDTAOff;
    r.x.ds = SavDTASeg;
    r.h.ah = 0x1A;
    intr( 0x21, &r );
}

static short findHandle;

USHORT DDosFindFirst( char PASPTR *spec, int attr, DIRINFO PASPTR *buf )
{
    char *p = buff;
    char *q = spec;
    for( ;; ) {
        if( *q != '"' )
            *p++ = *q;
        if( *q == '\0' )
            break;
        ++q;
    }
    r.x.ax = 0x714E;
    r.x.dx = _FP_OFF( buff );
    r.x.ds = _FP_SEG( buff );
    r.x.di = _FP_OFF( buf );
    r.x.es = _FP_SEG( buf );
    r.x.cx = attr;
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF ) {
        setdta( _FP_SEG( buf ), _FP_OFF( buf ) );
        r.x.dx = _FP_OFF( spec );
        r.x.ds = _FP_SEG( spec );
        r.x.cx = attr;
        r.h.ah = 0x4E;
        intr( 0x21, &r );
        resetdta();
        findHandle = 0;
    } else {
        memmove( buf->achName, ((WIN32_FIND_DATA*)buf)->cFileName, strlen( (char const *)( ((WIN32_FIND_DATA*)buf)->cFileName + 1 ) ) );
        buf->attrFile = ((WIN32_FIND_DATA*)buf)->dwFileAttributes;
        findHandle = r.x.ax;
    }
    return( ( r.x.flags & INTR_CF ) != 0 );
}

USHORT DDosFindNext( DIRINFO PASPTR *buf )
{
    if( findHandle != 0 ) {
        r.x.ax = 0x714F;
        r.x.bx = findHandle;
        r.x.di = _FP_OFF( buf );
        r.x.es = _FP_SEG( buf );
        intr( 0x21, &r );
        memmove( buf->achName, ((WIN32_FIND_DATA*)buf)->cFileName, strlen( (char const *)( ((WIN32_FIND_DATA*)buf)->cFileName + 1 ) ) );
        buf->attrFile = ((WIN32_FIND_DATA*)buf)->dwFileAttributes;
    } else {
        setdta( _FP_SEG( buf ), _FP_OFF( buf ) );
        r.h.ah = 0x4F;
        intr( 0x21, &r );
        resetdta();
    }
    return( (r.x.flags & INTR_CF) != 0 );
}

USHORT DDosQCurDisk( USHORT PASPTR *drive )
{
    r.h.ah = 0x19;
    intr( 0x21, &r );
    *drive = r.h.al + 1;
    return( 0 );
}

USHORT DosSelectDisk( int drive )
{
    r.h.ah = 0x0E;
    r.h.dl = drive - 1;
    intr( 0x21, &r );
    return( 0 );
}

USHORT DosQCurDir( int drive_num, char PASPTR *buff, int PASPTR *size )
{
    int         length;

    r.h.ah = 0x47;
    r.h.dl = drive_num;
    r.x.si = _FP_OFF( buff );
    r.x.ds = _FP_SEG( buff );
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF ) {
        return( 1 );
    } else {
        length = 0;
        while( *buff != '\0' ) {
            ++length;
            ++buff;
        }
        *size = length;
        return( 0 );
    }
}

USHORT DDosChDir( char PASPTR *dir )
{
    char *p = dir;
    char *q = buff;
    for( ;; ) {
        if( *p != '"' )
            *q++ = *p;
        if( *p == '\0' )
            break;
        ++p;
    }
    r.x.ax = 0x713B;
    r.x.ds = _FP_SEG( buff );
    r.x.dx = _FP_OFF( buff );
    intr( 0x21, &r );
    if( r.x.flags & INTR_CF ) {
        r.h.ah = 0x3B;
        r.x.ds = _FP_SEG( dir );
        r.x.dx = _FP_OFF( dir );
        intr( 0x21, &r );
    }
    return( ( r.x.flags & INTR_CF ) != 0 );
}

#endif
