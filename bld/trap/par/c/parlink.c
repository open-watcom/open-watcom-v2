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
* Description:  Parallel link remote communications core.
*
****************************************************************************/


/*
 * The WATCOM Connection
 * ---------------------
 * The AT parallel port uses pins 1 to 17 for data and 18 to 25 for
 * ground.  To send 8 bits of data between two ATs, we use 8 lines in one
 * direction, 8 in the other direction, and 1 for handshaking (total 17).
 * The 5 data lines from port 3BD/379 and 3 data lines from port 3BE/37A
 * are combined for 8 input data bits.  We use pin 17 for handshaking
 * since it is bi-directional (i. e. , you can read/write this line).
 *
 * Here's how to hook up a WATCOM cable to make this work.
 *
 *             Port   Bit Pin -> Pin  Port     Bit
 *
 *             GROUND      18 <-> 18
 *             CtlPort2 3  17 <-> 17  CtlPort2 3 (SLCT IN)
 *
 *             DataPort 0   2 ->  1*  CtlPort2 0 (STROBE)
 *                "     1   3 -> 14*     "     1 (AUTO FD XT)
 *                "     2   4 -> 16      "     2 (-INIT)
 *                "     3   5 -> 15   CtlPort1 3 (-ERROR)
 *
 *                "     4   6 -> 13      "     4 (Select)
 *                "     5   7 -> 12      "     5 (PE)
 *                "     6   8 -> 10      "     6 (-ACK)
 *                "     7   9 -> 11*     "     7 (-BUSY)
 *
 *                          1* <- 2
 *                         14* <- 3
 *                         16  <- 4
 *                         15  <- 5
 *
 *                         13  <- 6
 *                         12  <- 7
 *                         10  <- 8
 *                         11* <- 9
 *
 *      (*) means logic inverted
 *
 * The Fujistsu Connection
 * -----------------------
 * The Fujitsu FMR-xx also supports the transmission and reception of 8
 * data bits.  There are enough data lines to do this.  Unfortunately,
 * there are no bidirectional data lines.  Therefore, we are short one pin
 * on the AT side (we would require 8 data + 8 data + 2 handshaking lines
 * on the AT since the Fujitsu has no bidirectional data lines).
 *
 * From the software point of view, we will be able to send 8 bits at a
 * time in one direction (to the FMR-xx) and 4 bits at a time in the
 * other direction (to the AT).  Given that the higher volume of traffic
 * is usually from the application machine to debugger machine, this
 * seems to be the best choice.
 *
 * Here's how to hook up a Fujitsu WATCOM cable to make this work.
 *
 *                 AT side              FM-R side
 *             --------------      -------------------
 *             Port   Bit Pin      Pin  Port     Bit
 *
 *             GROUND      18  <-> 23  GROUND
 *  (-SLCT IN) CtlPort2 3  17   -> 13* StatReg2  3 (*SLCT)
 *
 *  (+DB0)     DataPort 0   2   -> 14  StatReg2  0 (RINF1)
 *  (+DB1)        "     1   3   -> 15     "      1 (RINF2)
 *  (+DB2)        "     2   4   -> 16     "      2 (RINF3)
 *  (+DB3)        "     3   5   -> 17  StatReg1  3 (POW +5 VP)
 *
 *  (+DB4)        "     4   6   -> 22      "     4 (THSN)
 *  (+DB5)        "     5   7   -> 21*     "     5 (*FUSE)
 *  (+DB6)        "     6   8   -> 12      "     6 (PE)
 *  (+DB7)        "     7   9   -> 11      "     7 (BUSY)
 *
 *  (-STROBE)  CtlPort2 0   1* <- 2    DataReg   0 (PD1)
 *  (-AUTO FEED)  "     1  14* <- 3              1 (PD2)
 *  (-INIT PRT)   "     2  16  <- 4              2 (PD3)
 *  (-ERROR)   CtlPort1 3  15  <- 5              3 (PD4)

 *  (+SELECT)     "     4  13  <- 6              4 (PD5)
 *  (+P.END)      "     5  12  <- 7              5 (PD6)
 *  (-ACKNOWLEDGE)"     6  10  <- 8              6 (PD7)
 *  (+BUSY)       "     7  11* <- 9              7 (PD8)
 *
 *      (*) means logic inverted
 *
 * The LapLink Connection
 * ----------------------
 * Here's how to hook up a LapLink cable.
 *
 *              Port   Bit Pin -> Pin  Port      Bit
 *
 *           DataPort   0   2  ->  15  CtlPort1  3      (Error)
 *              "       1   3  ->  13     "      4      (Select)
 *              "       2   4  ->  12     "      5      (P End)
 *              "       3   5  ->  10     "      6      (Ack)
 *              "       4   6  ->  11*    "      7      (Busy)
 *
 *                          10 <-  5
 *                          11 <-  6
 *                          12 <-  4
 *                          13 <-  3
 *                          15 <-  2
 *
 *           Ground         25 <-> 25
 *
 * The Flying Dutchman Connection
 * ------------------------------
 * Here's how to hook up a Flying Dutchman cable.
 *
 *              Port   Bit Pin -> Pin  Port      Bit
 *
 *           CtlPort2   0   1  ->  11* CtlPort1  7
 *           DataPort   0   2  ->  15     "      3      (Error)
 *              "       1   3  ->  13     "      4      (Select)
 *              "       2   4  ->  12     "      5      (P End)
 *              "       3   5  ->  10     "      6      (Ack)
 *
 *                          10 <-  5
 *                          11 <-  1
 *                          12 <-  4
 *                          13 <-  3
 *                          15 <-  2
 *
 * The following is some technical information on the AT parallel port.
 * The pin numbers refer to the DB25 pins on the AT parallel port.
 *
 *          IBM AT Parallel Port
 * +-----------------------------------+
 * |Input/Output to 3BC/378            |
 * +-----------------------------------+
 * |Bit   7   6   5   4   3   2   1   0|
 * |Pin   9   8   7   6   5   4   3   2|
 * +-----------------------------------+
 *
 * +-----------------------------------+
 * |Output to 3BE/37A
 * +-----------------------------------+
 * |Bit   7   6   5   4   3   2   1   0|
 * |Pin   -   -   - IRQ  17  16  14   1|
 * |               Enable              |
 * +-----------------------------------+
 *
 * +-----------------------------------+
 * |Input from 3BD/379
 * +-----------------------------------+
 * |Bit   7   6   5   4   3   2   1   0|
 * |Pin  11  10  12  13  15   -   -   -|
 * +-----------------------------------+
 *
 * +-----------------------------------+
 * |Input from 3BE/37A
 * +-----------------------------------+
 * |Bit   7   6   5   4   3   2   1   0|
 * |Pin   -   -   -  IRQ 17  16  14   1|
 * |               Enable              |
 * +-----------------------------------+
 */

#include <stddef.h>
#if defined(__NETWARE__)
#include <dos.h>
#else
#include <i86.h>
#endif
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"


#if defined(__NT__)
        // We have direct port I/O for NT and Win9x
        extern unsigned outp( unsigned, unsigned );
        extern unsigned inp( unsigned );
#elif defined(__OS2__)
    #if defined(__386__)
        #define inp input_port
        #define outp output_port
        extern unsigned short __far16 _pascal outp(unsigned short, unsigned short);
        extern unsigned short __far16 _pascal inp(unsigned short);
    #else
        #define inp input_port
        #define outp output_port
        extern unsigned far outp( unsigned, unsigned );
        extern unsigned far inp( unsigned );
    #endif
#elif defined(__NETWARE__)
    #pragma aux inp =                                   \
    0xec                /* in   al,dx                           */      \
        parm    routine [ dx ];


    #pragma aux outp =                                  \
    0xee                /* out  dx,al                           */      \
        parm    routine [ dx ] [ ax ];


    extern char inp( int );
    extern void outp( int, int );
#else
    #include <conio.h>
#endif

extern void             Wait(void);
#pragma aux Wait = ;

#if defined(_DBG)
#ifdef SERVER
    #include <stdio.h>
    #define dbgrtn(x) printf( x )
#else
    #include <stdlib.h>
    #include <conio.h>
    #define dbgrtn(x) cputs( x )
#endif
char _inp( int port )
{
    char x;

    x = inp( port );
#ifdef SERVER
    printf( "in %x=%2.2x ", port, x );
#else
    {
    char buf[10];

    itoa( port, buf, 16 );
    cputs( " in " ); cputs( buf );
    itoa( x, buf, 16 );
    cputs( "=" ); cputs( buf );
    }
#endif
    return( x );
}

void _outp( int port, char x )
{

    outp( port, x );
#ifdef SERVER
    printf( "out %x=%2.2x ", port, x );
#else
    {
    char buf[10];

    itoa( port, buf, 16 );
    cputs( " out " ); cputs( buf );
    itoa( x, buf, 16 );
    cputs( "=" ); cputs( buf );
    }
#endif
}
#else
    #define _inp inp
    #define _outp outp
    #define dbgrtn(x)
#endif

extern int              NumPrinters( void );
extern unsigned         PrnAddress( int );
extern unsigned         AccessPorts( unsigned, unsigned );
extern void             FreePorts( unsigned, unsigned );
extern void             NothingToDo( void );
extern unsigned long    Ticks( void );
extern char             *InitSys( void );
extern void             FiniSys( void );

static unsigned DataPort;
static unsigned CtlPort1;
static unsigned CtlPort2;
static  char    CableType;
static  char    TwidleCount;
static  bool    TwidleOn;

/* 0x18 is used to ensure that the control lines stay in a high state
 * until Synch is called */

// #define TWIDLE_OFF      0x18
// #define TWIDLE_ON       ( TWIDLE_OFF | 2 )

// #define WATCOM_VAL      TWIDLE_ON

#define WATCOM_VAL      0x1A
#define TWIDLE_ON       WATCOM_VAL
#define TWIDLE_OFF      ( WATCOM_VAL & 0xFD )

/*
 * The Fujitsu FMR link looks like a busted WATCOM cable
 * in which RaiseCtl1()/LowerCtl1() cannot be performed
 */
#define FMR_VAL         0x2A

/*
 * How the LAPLINK_VAL calculation works:
 *
 * <<3            simulates the effect of being transmitted over the LAPLINK
 * |0x2           0x2  is the value returned from a read of ctlport 2
 *                (after appropriate bit operations have be done by
 *                ReadData).  This value is dependent on the value
 *                written when XX_RaiseCtl1 is called
 */

#define LAPLINK_VAL     ( ( TWIDLE_ON << 3 ) | 0x2 )

/*
 * The DUTCHMAN_VAL calculation works the same as the LAPLINK_VAL
 * calculation except:
 *
 * &0x0f        acounts for the fact that pin 6 is not connected in the
 *              flying dutchman cable
 */

#define DUTCHMAN_VAL    ( ( ( TWIDLE_ON & 0x0f ) << 3 ) | 0x02 )

#define NULL_VAL        0

#define TWIDLE_TIME     1
#define TWIDLE_NUM      2

#define DONE_LINE_TEST  255

/* relinquish must be less than keep */
#define RELINQUISH      0
#define KEEP            1

#define LINE_TEST_WAIT  20
#define SYNCH_WAIT      40
#define TIMEOUT         -1

/*********************** WATCOM CABLE MACROS **************************/
#define PC_CTL1 0x08
#define PC_CTL2 0x08
#define Ctl1Hi()        ( ( Wait(),_inp( CtlPort2 ) & PC_CTL1 ) != 0 )
#define Ctl1Lo()        ( ( Wait(),_inp( CtlPort2 ) & PC_CTL1 ) == 0 )
#define RaiseCtl1()     ( Wait(),_outp( CtlPort2, PC_CTL1 | 0x04 ) )
#define LowerCtl1()     ( Wait(),_outp( CtlPort2, 0x04 ) )

#define Ctl2Hi()        ( ( Wait(),_inp( CtlPort1 ) & PC_CTL2 ) != 0 )
#define Ctl2Lo()        ( ( Wait(),_inp( CtlPort1 ) & PC_CTL2 ) == 0 )
#define RaiseCtl2()     ( Wait(),_outp( DataPort, PC_CTL2 ) )
#define LowerCtl2()     ( Wait(),_outp( DataPort, 0x00 ) )

#define ReadData()      ( ( ( Wait(),_inp( CtlPort1 ) ^ 0x80 ) & 0xF8 ) \
                        | ( ( Wait(),_inp( CtlPort2 ) ^ 0x03 ) & 0x07 ) )
#define WriteData(data) ( Wait(),_outp( DataPort, data ) )

/*********************** WATCOM FMR CABLE MACROS **********************/
#define FM_CTL1 0x40
/* Can't use CtlPort2 & 0x08 (line disabled) */
#define FM_Ctl1Hi()     ( ( Wait(),_inp( CtlPort1 ) & FM_CTL1 ) != 0 )
#define FM_Ctl1Lo()     ( ( Wait(),_inp( CtlPort1 ) & FM_CTL1 ) == 0 )

/********************** LAPLINK CABLE MACROS **************************/
#define LL_Ctl1Hi()     ( ( Wait(),_inp( CtlPort1 ) & 0x80 ) == 0 )
#define LL_Ctl1Lo()     ( ( Wait(),_inp( CtlPort1 ) & 0x80 ) != 0 )

#define LL_RaiseCtl1()  ( Wait(),_outp( DataPort, 0x10 ) )
#define LL_LowerCtl1()  ( Wait(),_outp( DataPort, 0x00 ) )

#define LL_Ctl2Hi()     ( ( Wait(),_inp( CtlPort1 ) & 0x40 ) != 0 )
#define LL_Ctl2Lo()     ( ( Wait(),_inp( CtlPort1 ) & 0x40 ) == 0 )

#define LL_RaiseCtl2()  ( Wait(),_outp( DataPort, 0x08 ) )
#define LL_LowerCtl2()  ( Wait(),_outp( DataPort, 0x00 ) )

#define LL_ReadData()   ( ( Wait(),_inp( CtlPort1 ) >> 3 ) & 0x0f  )
/* write the data and raise control line 1 */
#define LL_WriteData(data) ( Wait(),_outp( DataPort, ( data | 0x10 ) ) )

/***************** FLYING DUTCHMAN CABLE MACROS ***********************/

#define FD_Ctl1Hi()      ( ( Wait(),_inp( CtlPort1 ) & 0x80 ) != 0 )
#define FD_Ctl1Lo()      ( ( Wait(),_inp( CtlPort1 ) & 0x80 ) == 0 )

#define FD_RaiseCtl1()   ( Wait(),_outp( CtlPort2, 0x01 ) )
#define FD_LowerCtl1()   ( Wait(),_outp( CtlPort2, 0x00 ) )

#define FD_Ctl2Hi()      ( ( Wait(),_inp( CtlPort1 ) & 0x40 ) != 0 )
#define FD_Ctl2Lo()      ( ( Wait(),_inp( CtlPort1 ) & 0x40 ) == 0 )

#define FD_RaiseCtl2()   ( Wait(),_outp( DataPort, 0x08 ) )
#define FD_LowerCtl2()   ( Wait(),_outp( DataPort, 0x00 ) )

#define FD_ReadData()     ( ( Wait(),_inp( CtlPort1 ) >> 3 ) & 0x0f  )
#define FD_WriteData(data) ( Wait(),_outp( DataPort, data ) )

/***************** Cable Detection MACROS ****************************/

/*
 * This operation disables bits 3,2,0 in CtrlPort2 (LowerCtl1 fixes it)
 */
#define XX_RaiseCtl1()   ( Wait(),_outp( CtlPort2, 0x01 ) )

/*********************************************************************/

#define TWIDDLE_THUMBS  if( wait == RELINQUISH ) { NothingToDo(); }\
                else if( wait != KEEP && wait < Ticks() ) return( TIMEOUT )

/*
 * if wait is not KEEP or RELINQUISH it is the latest time that this
 * operation should take before it times out
 */

static int DataGet( unsigned long wait )
{
    char                data;

    dbgrtn( "\r\n-DataGet-" );
    switch( CableType ) {
    case WATCOM_VAL:
        RaiseCtl2();            /* Hi, I'm ready to read */
        while( Ctl1Lo() ) {     /* wait till he's written the data */
            TWIDDLE_THUMBS;
        }
        data = ReadData();      /* bag the bits */
        LowerCtl2();            /* Hey you! I got the bits */
        while( Ctl1Hi() ) {     /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        break;
    case FMR_VAL:
        /* We're talking to the FMR which can't RaiseCtl1/LowerCtl1 */
        /* get the low nibble */
        RaiseCtl2();                    /* ready to read */
        while( FM_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data = ReadData() & 0x0f;       /* bag the bits */
        LowerCtl2();                    /* Hey you! I got the bits */
        while( FM_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        /*get the high nibble */
        RaiseCtl2();                    /* ready to read */
        while( FM_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data |= ( ReadData() << 4 );    /* bag the bits */
        LowerCtl2();                    /* Hey you! I got the bits */
        while( FM_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        break;
    case LAPLINK_VAL:
        /* get the low nibble */
        LL_RaiseCtl2();                 /* ready to read */
        while( LL_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data = LL_ReadData();           /* bag the bits */
        LL_LowerCtl2();                 /* Hey you! I got the bits */
        while( LL_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        /*get the high nibble */
        LL_RaiseCtl2();                 /* ready to read */
        while( LL_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data |= ( LL_ReadData() << 4 ); /* bag the bits */
        LL_LowerCtl2();                 /* Hey you! I got the bits */
        while( LL_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        break;
    case DUTCHMAN_VAL:
        /* get the low nibble */
        FD_RaiseCtl2();                 /* ready to read */
        while( FD_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data = FD_ReadData();           /* bag the bits */
        FD_LowerCtl2();                 /* Hey you! I got the bits */
        while( FD_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        /*get the high nibble */
        FD_RaiseCtl2();                 /* ready to read */
        while( FD_Ctl1Lo() ) {          /* wait for data */
            TWIDDLE_THUMBS;
        }
        data |= ( FD_ReadData() << 4 ); /* bag the bits */
        FD_LowerCtl2();                 /* Hey you! I got the bits */
        while( FD_Ctl1Hi() ) {          /* Wait till he heard us */
            TWIDDLE_THUMBS;
        }
        break;
    }
    return( data );
}


/* if wait is not KEEP or RELINQUISH it is the latest time that this
 * operation should take before it times out */

static int DataPut( unsigned char data, unsigned long wait )
{
    dbgrtn( "\r\n-DataPut-" );
    switch( CableType ) {
    case WATCOM_VAL:
        while( Ctl2Lo() ) {             /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        WriteData( data );              /* write the data */
        RaiseCtl1();                    /* tell him the data's there */
        while( Ctl2Hi() ) {             /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        LowerCtl1();                    /* clear control line */
        break;
    case FMR_VAL:
        /* We're talking to the FMR which can RaiseCtl2/LowerCtl2 */
        while( Ctl2Lo() ) {             /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        WriteData( data );              /* write the data */
        RaiseCtl1();                    /* tell him the data's there */
        while( Ctl2Hi() ) {             /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        LowerCtl1();                    /* clear control line */
        break;
   case LAPLINK_VAL:
        /* send low nibble */
        while( LL_Ctl2Lo() ) {          /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        LL_WriteData( data & 0x0f );    /* write the data */
                                        /* and tell him the data's there */
        while( LL_Ctl2Hi() ) {          /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        LL_LowerCtl1();                 /* clear control line */
        /* send high nibble */
        while( LL_Ctl2Lo() ) {          /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        LL_WriteData( data >> 4 );      /* write the data */
                                        /* and tell him the data's there */
        while( LL_Ctl2Hi() ) {          /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        LL_LowerCtl1();                 /* clear control line */
        break;
    case DUTCHMAN_VAL:
        /* send low nibble */
        while( FD_Ctl2Lo() ) {          /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        FD_WriteData( data & 0x0f );    /* write the data */
        FD_RaiseCtl1();                 /* tell him the data's there */
        while( FD_Ctl2Hi() ) {          /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        FD_LowerCtl1();                 /* clear control line */
        /* send high nibble */
        while( FD_Ctl2Lo() ) {          /* wait till he's ready to read */
            TWIDDLE_THUMBS;
        }
        FD_WriteData( data >> 4 );      /* write the data */
        FD_RaiseCtl1();                 /* tell him the data's there */
        while( FD_Ctl2Hi() ) {          /* wait till he got the bits */
            TWIDDLE_THUMBS;
        }
        FD_LowerCtl1();                 /* clear control line */
        break;
    }
    return( 0 );
}

trap_elen RemoteGet( char *rec, trap_elen len )
{
    trap_elen  get_len;
    trap_elen  i;

    len = len;
    get_len = DataGet( RELINQUISH );
    if( get_len & 0x80 ) {
        get_len = ((get_len & 0x7f) << 8) | DataGet( KEEP );
    }
    i = get_len;
    for( ; i != 0; --i, ++rec ) {
        *rec = DataGet( KEEP );
    }
    return( get_len );
}

trap_elen RemotePut( char *snd, trap_elen len )
{
    trap_elen  count;

    if( len >= 0x80 ) {
        DataPut( (len >> 8) | 0x80, RELINQUISH );
    }
    DataPut( len & 0xff, RELINQUISH );
    for( count = len; count != 0; --count, ++snd ) {
        DataPut( *snd, KEEP );
    }
    return( len );
}

/*
 * Synch - see if server and client are ready
 */

static bool Synch( void )
{
    dbgrtn( "\r\n-Synch-" );
    switch( CableType ) {
    case WATCOM_VAL:
    case FMR_VAL:
        if( Ctl2Lo() ) {
            return( TRUE );
        }
        break;
    case LAPLINK_VAL:
        if( LL_Ctl1Lo() ) return( TRUE );
        break;
    case DUTCHMAN_VAL:
        if( FD_Ctl1Lo() ) return( TRUE );
        break;
    }
    return( FALSE );
}




static bool CountTwidle( void )
{
    char                type;

    dbgrtn( "\r\n-CountTwidle-" );
    type = ReadData();
#if defined(_DBG)
#ifdef SERVER
    printf( "Type %2.2x ", type );
#else
    {
    char buf[10];

    itoa( type, buf, 16 );
    cputs( " Type " ); cputs( buf );
    }
#endif
#endif
    if( !TwidleOn ) {
        if( type == WATCOM_VAL ||
            type == FMR_VAL ||
            type == (char)LAPLINK_VAL ||
            type == DUTCHMAN_VAL ) {
            TwidleOn = TRUE;
            if( type != CableType ) {
                TwidleCount = 0;
                CableType = type;
            }
        }
    } else {
        if( type != CableType )  {
            TwidleCount ++;
            TwidleOn = FALSE;
            if( TwidleCount == TWIDLE_NUM ) return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * Twidle - send an intermittent pulse over a line to let the person
 *          at the other end know you're there
 */

static bool Twidle( bool check ) {

    unsigned            i;
    unsigned long       time;

    dbgrtn( "\r\n-Twidle-" );
    for( i = 20; i != 0; i-- ) {
        WriteData( TWIDLE_ON );
        time = Ticks() + TWIDLE_TIME;
        while( time > Ticks() ){
            if( check ) {
                if( CountTwidle() ) {
                    return( TRUE );
                }
            } else {
                if( Synch() ) {
                    return( TRUE );
                }
            }
        }
        WriteData( TWIDLE_OFF );
        time = Ticks() + TWIDLE_TIME;
        while( time > Ticks() ){
            if( check ) {
                if( CountTwidle() ) {
                    return( TRUE );
                }
            } else {
                if( Synch() ) {
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );
}

/*
 * LineTest - make sure that all lines are working
 */

static bool LineTest( void )
{
    unsigned            send;
    unsigned long       time;
#ifdef SERVER
    unsigned            ret;

    dbgrtn( "\r\n-LineTest-" );
    for( send = 1; send != 256; send *= 2 ) {
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        ret = DataPut( send, time );
        if( ret == TIMEOUT ) return( FALSE );
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        ret = DataGet( time );
        if( ret == TIMEOUT ) return( FALSE );
        if( ret != send ) {
            return( FALSE );
        }
    }
    time = Ticks() + LINE_TEST_WAIT;
    if( time == RELINQUISH ) time ++;
    if( time == KEEP ) time ++;
    ret = DataPut( DONE_LINE_TEST, time );
    if( ret == TIMEOUT ) return( FALSE );
#else
    dbgrtn( "\r\n-LineTest-" );
    send = 0;
    for( ;; ) {
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        send = DataGet( time );
        if( send == TIMEOUT ) return( FALSE );
        if( send == DONE_LINE_TEST ) break;
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        DataPut( send, time );
        if( send == TIMEOUT ) return( FALSE );
    }
#endif
    return( TRUE );
}

char RemoteConnect( void )
{
    unsigned long       time;

#ifdef SERVER
    bool                got_twidles;

    dbgrtn( "\r\n-RemoteConnect-" );
    if( !CountTwidle() ) return( 0 );
    got_twidles = Twidle( FALSE );
#else
    dbgrtn( "\r\n-RemoteConnect-" );
    if( !Twidle( TRUE ) ) return( FALSE );
#endif

    switch( CableType ) {
    case WATCOM_VAL:
        LowerCtl1();
        LowerCtl2();
    case FMR_VAL:
        LowerCtl1();
        LowerCtl2();
    case LAPLINK_VAL:
        LL_LowerCtl1();
        break;
    case DUTCHMAN_VAL:
        FD_LowerCtl1();
        break;
    }
#ifdef SERVER
    if( !got_twidles ) {
#endif
        time = Ticks() + SYNCH_WAIT;
        for( ;; ) {
            if( Synch() ) {
                break;
            } else if( time < Ticks() ) {
                return( 0 );
            }
        }
#ifdef SERVER
    }
#endif
    if( !LineTest() ) return( FALSE );
    return( TRUE );
}

void RemoteDisco( void )
{
    unsigned long       time;

    dbgrtn( "\r\n-RemoteDisco-" );
    time = Ticks() + TWIDLE_TIME;
    while( time > Ticks() ) { /* delay while other side catches up */ }
    WriteData( TWIDLE_OFF );            /* initialize control ports */
    XX_RaiseCtl1();
    TwidleCount = 0;
    CableType = NULL_VAL;
    TwidleOn = FALSE;

}

static char InvalidPort[] = TRP_ERR_invalid_parallel_port_number;

char *RemoteLink( char *name, char server )
{
    unsigned    printer;
    unsigned    ch;
    char        *err;

    dbgrtn( "\r\n-RemoteLink-" );
    server = server;

    err = InitSys();
    if( err != NULL ) {
        return( err );
    }
    if( name == NULL || name[0] == '\0' ) {
        printer = 0;
    } else if( name[0] >= '1' && name[0] <= '3' && name[1] == '\0' ) {
        printer = name[0] - '1';
    } else if( name[0] == 'p' || name[0] == 'P' ) {
        printer = 0;
        for( ;; ) {
            ++name;
            ch = *name;
            if( ch == 0 ) break;
            if( ch == ' ' ) break;
            if( ch == '\t' ) break;
            if( ch >= 'A' && ch <= 'F' ) {
                ch = ch - 'A' + 0x0a;
            } else if( ch >= 'a' && ch <= 'f' ) {
                ch = ch - 'a' + 0x0a;
            } else if( ch >= '0' && ch <= '9' ) {
                ch = ch - '0';
            } else {
                return( InvalidPort );
            }
            printer <<= 4;
            printer += ch;
        }
        DataPort = printer;
    } else {
        return( InvalidPort );
    }
    if( DataPort == 0 ) {
        if( NumPrinters() <= printer ) {
            return( TRP_ERR_parallel_port_not_present );
        }
        DataPort = PrnAddress( printer );
    }
    CtlPort1 = DataPort + 1;
    CtlPort2 = CtlPort1 + 1;

    if( !AccessPorts( DataPort, CtlPort2 ) ) {
        return( TRP_ERR_cannot_access_parallel_ports );
    }
    WriteData( TWIDLE_OFF );            /* initialize the control ports */
    XX_RaiseCtl1();
    TwidleOn = FALSE;
    return( NULL );
}

void RemoteUnLink( void )
{
    FiniSys();
    FreePorts( DataPort, CtlPort2 );
    DataPort = 0;
}
