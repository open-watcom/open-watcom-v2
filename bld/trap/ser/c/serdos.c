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


#ifndef ZDP
   #define ZDP
#endif

#include <dos.h>
#include <conio.h>
#include "trpimp.h"
#include "trperr.h"
#include "dosequip.h"
#include "serial.h"
#include "serlink.h"
#include "tinyio.h"

#define I8259  0x20 /* 8259 control register address */
#define I8259M  0x21 /* 8259 mask register */
#define EOI  0x20 /* 8259 end of interrupt command */
#define EOI_COM  (0x60+IntVector) /* specific level EOI */

#define IER  (IOBase+1) /* interrupt enable register address */
#define IIR  (IOBase+2) /* interrupt identification register address */
#define LCR  (IOBase+3) /* line control register address */
#define MCR  (IOBase+4) /* modem control register address */
#define LSR  (IOBase+5) /* line status register address */
#define MSR  (IOBase+6) /* modem status register address */

#define LCR_SETUP   0x03 /* set 8 bits, no parity, 1 stop */
#define LCR_DLAB    0x80 /* divisor latch access bit */
#define MCR_DTR     0x01 /* Data Terminal Ready control */
#define MCR_RTS     0x02 /* Request To Send control */
#define MCR_OUT2    0x08 /* out2 control bit */
#define MCR_SETUP   ( MCR_OUT2 | MCR_RTS | MCR_DTR )

#define LSR_DRDY  0x01 /* data ready */
#define LSR_ORUN  0x02 /* overrun error */
#define LSR_PRTY  0x04 /* parity error */
#define LSR_FRM   0x08 /* framing error */
#define LSR_BRK   0x10 /* break interrupt */
#define LSR_THRE  0x20 /* transmit holding register empty */
#define LSR_TSRE  0x40 /* transmit shift register empty */
#define LSR_ERR   (LSR_FRM+LSR_PRTY+LSR_ORUN) /* error conditions */

#define MSR_CTS         0x10 /* Clear To Send */
#define MSR_DSR         0x20 /* Data Set Ready */

#define BREAK_TIME      4

int IOBase; /* base com port address */
int IntVector; /* com interrupt vector to use */
int ErrorFlag;
int BreakFlag;

static int I8259mBit;
static int CurrentBaud;
static char Modem;
unsigned Ticks;

extern void ClearBuffer( void );
extern int GetBufferByte( void );
extern void InitInts( void );
extern void FiniInts( void );


void ZeroWaitCount( void )
{
    Ticks = 0;
}


unsigned WaitCount( void )
{
    return( Ticks );
}


char *InitSys( void )
{
    int i8259_val;                  /* storing I8259 value */
    int i8259m_val;                 /* storing I8259 mask register value */

    inp( IOBase );                 /* base com port: clear any characters */
    outp( IER, 0 );                /* ier <= 0 : no ints for now */
    inp( MSR );                    /* MSR : reset MSR now */

    InitInts();

    outp( LCR, LCR_SETUP );                /* set com line characteristics */
    outp( MCR, MCR_SETUP );                /* set MCR value */
    outp( IER, 0x05 );                     /* set ier, allow lsr & rx ints */

    _disable();
    i8259m_val = inp( I8259M );            /* set 8259 mask register    */
    I8259mBit = i8259m_val & ( 1 << IntVector );
    i8259m_val &= ~( 1 << IntVector );    /*    to allow com interrupts */
    outp( I8259M, i8259m_val );
    _enable();
    i8259_val = IntVector;                 /* set 8259 priority */
    --i8259_val;
    i8259_val |= 0xc0;
    outp( I8259, i8259_val );
    CurrentBaud = -1;
    Modem = 0;
    return( NULL );
}


void ResetSys( void )
{
    int i8259m_val;         /* storing I8259 mask register value */

    outp( I8259, 0xc7 );              /* reset normal 8259 priority */

    i8259m_val = inp( I8259M );               /* set 8259 mask register  */
    i8259m_val |= I8259mBit;                  /*  to previous status for */
    outp( I8259M, i8259m_val );               /*  com:-level interrupts  */

    outp( IER, 0 );                   /* ier <= 0 : all ints off */
    outp( MCR, 0 );                   /* MCR : set OUT2 & DTR off */

    FiniInts();
}


void SendByte( int value )
{
    do {
        ;
    } while( (inp( LSR ) & LSR_THRE) == 0 );
    if( Modem ) {
        /* talking over a modem - check the data set ready line */
        do {
            ;
        } while( (inp( MSR ) & MSR_CTS) == 0 );
    }
    outp( IOBase, value );
}

void StartBlockTrans( void )
{
}

void StopBlockTrans( void )
{
    if( Modem ) {
        /* talking over a modem - check the data set ready line */
        do {
            ;
        } while( (inp( MSR ) & MSR_CTS) == 0 );
    }
}

int GetByte( void )
{
    return( GetBufferByte() );
}


void ClearCom( void )
{
    ClearBuffer();
}

#if 0
void SendABreak( void )
{
    int lcr_value;             /* storing line control register value */

    lcr_value = inp( LCR );                /* LCR contents */
    outp( LCR, lcr_value | 0x40 );         /* set break bit on */
    SendByte( 0 );                         /* interrupt other side */
    Wait( BREAK_TIME );                    /* hold it there */
    lcr_value &= (~0x40);                  /* assure break bit is off */
    outp( LCR, lcr_value );                /* restore lcr content */
}

bool TestForBreak( void )
{
    _disable();
    if( BreakFlag || ( inp(LSR) & LSR_BRK ) ) {
        BreakFlag = 0;
        _enable();
        return( TRUE );
    }
    _enable();
    return( FALSE );
}
#endif


int Divisor[] = { 1, 2, 3, 6, 12, 24, 48, 96, 0 };

bool Baud( int index )
{
    int lcr_value;

    ErrorFlag = 0;
    BreakFlag = 0;
    if( index == MIN_BAUD ) {
        Modem = 1;
        return( TRUE );
    }
    Modem = 0;
    if( index == CurrentBaud ) return( TRUE );

    /* don't change baud rate while a character is still being sent */
    do {} while( (inp( LSR ) & LSR_TSRE) == 0 );
    lcr_value = inp( LCR );              /* get LCR value */
    _disable();                          /* disable interrupt */
    outp( LCR, lcr_value | LCR_DLAB );   /* set Divisor Latch Access Bit(DLAB)*/
                                         /*  to enable setting of baud rate */
    outp( IOBase, Divisor[index] );      /* LSB portion of new divisor */
    outp( IOBase+1, 0 );                 /* MSB portion of new divisor */
    lcr_value = inp( LCR );
    outp( LCR, lcr_value & ~LCR_DLAB );  /* set off DLAB bit of LCR */
    _enable();                           /* re-enable interrupts */
    CurrentBaud = index;
    return( TRUE );
}

char *ParsePortSpec( const char **spec )
{
    const char  *parm;
    int         port;

    parm = (spec == NULL) ? "" : *spec;

    switch( *parm ) {
    case '1':
        ++parm;
        /* fall through */
    case '.':
    case '\0':
        port = 0;
        IntVector = 4;
        IOBase = 0x3f8;
        break;
    case '2':
        ++parm;
        port = 1;
        IntVector = 3;
        IOBase = 0x2f8;
        break;
    default:
        return( TRP_ERR_invalid_serial_port_number );
    }
    if( *parm != '\0' && *parm != '.' )
        return( TRP_ERR_invalid_serial_port_number );
    if( port >= Equipment().num_rs232s )
        return( TRP_ERR_serial_port_does_not_exist );
    if( spec != NULL )
        *spec = parm;
    return( NULL );
}


void DonePort( void )
{
}


bool CheckPendingError( void )
{
    int old_error;

    _disable();
    old_error = ErrorFlag;
    ErrorFlag = 0;
    _enable();
    return( old_error != 0 );
}


void ClearLastChar( void )
{
    /* wait for last character to be sent */
    do {
        ;
    } while( (inp( LSR ) & LSR_TSRE) == 0 );
}
