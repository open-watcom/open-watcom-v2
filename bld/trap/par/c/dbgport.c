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

#include <ntddk.h>
#pragma disable_message ( 202 )
#include "parallel.h"
#include "dbgioctl.h"

#if defined( __WATCOMC__ )
typedef __int64 _int64;
#define PORTSTDCALL __stdcall
#endif

#ifdef TRUE
typedef UCHAR bool;
#else
#include "bool.h"
#endif

#define _inp(p)         READ_PORT_UCHAR((PUCHAR)(ext->Controller + (p)))
#define _outp(p,v)      WRITE_PORT_UCHAR((PUCHAR)(ext->Controller + (p)), (UCHAR)(v))

/* 0x18 is used to ensure that the control lines stay in a high state
 * until Synch is called
 */

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
#define Ctl1Hi()        ( ( _inp( ext->CtlPort2 ) & PC_CTL1 ) != 0 )
#define Ctl1Lo()        ( ( _inp( ext->CtlPort2 ) & PC_CTL1 ) == 0 )
#define RaiseCtl1()     ( _outp( ext->CtlPort2, PC_CTL1 | 0x04 ) )
#define LowerCtl1()     ( _outp( ext->CtlPort2, 0x04 ) )

#define Ctl2Hi()        ( ( _inp( ext->CtlPort1 ) & PC_CTL2 ) != 0 )
#define Ctl2Lo()        ( ( _inp( ext->CtlPort1 ) & PC_CTL2 ) == 0 )
#define RaiseCtl2()     ( _outp( ext->DataPort, PC_CTL2 ) )
#define LowerCtl2()     ( _outp( ext->DataPort, 0x00 ) )

#define ReadData()      ( ( ( _inp( ext->CtlPort1 ) ^ 0x80 ) & 0xF8 ) \
                                                | ( ( _inp( ext->CtlPort2 ) ^ 0x03 ) & 0x07 ) )
#define WriteData(data) ( _outp( ext->DataPort, data ) )

/*********************** WATCOM FMR CABLE MACROS **********************/
#define FM_CTL1 0x40
/* Can't use ext->CtlPort2 & 0x08 (line disabled) */
#define FM_Ctl1Hi()     ( ( _inp( ext->CtlPort1 ) & FM_CTL1 ) != 0 )
#define FM_Ctl1Lo()     ( ( _inp( ext->CtlPort1 ) & FM_CTL1 ) == 0 )

/********************** LAPLINK CABLE MACROS **************************/
#define LL_Ctl1Hi()     ( ( _inp( ext->CtlPort1 ) & 0x80 ) == 0 )
#define LL_Ctl1Lo()     ( ( _inp( ext->CtlPort1 ) & 0x80 ) != 0 )

#define LL_RaiseCtl1()  ( _outp( ext->DataPort, 0x10 ) )
#define LL_LowerCtl1()  ( _outp( ext->DataPort, 0x00 ) )

#define LL_Ctl2Hi()     ( ( _inp( ext->CtlPort1 ) & 0x40 ) != 0 )
#define LL_Ctl2Lo()     ( ( _inp( ext->CtlPort1 ) & 0x40 ) == 0 )

#define LL_RaiseCtl2()  ( _outp( ext->DataPort, 0x08 ) )
#define LL_LowerCtl2()  ( _outp( ext->DataPort, 0x00 ) )

#define LL_ReadData()   ( ( _inp( ext->CtlPort1 ) >> 3 ) & 0x0f  )
/* write the data and raise control line 1 */
#define LL_WriteData(data) ( _outp( ext->DataPort, ( data | 0x10 ) ) )

/***************** FLYING DUTCHMAN CABLE MACROS ***********************/

#define FD_Ctl1Hi()      ( ( _inp( ext->CtlPort1 ) & 0x80 ) != 0 )
#define FD_Ctl1Lo()      ( ( _inp( ext->CtlPort1 ) & 0x80 ) == 0 )

#define FD_RaiseCtl1()   ( _outp( ext->CtlPort2, 0x01 ) )
#define FD_LowerCtl1()   ( _outp( ext->CtlPort2, 0x00 ) )

#define FD_Ctl2Hi()      ( ( _inp( ext->CtlPort1 ) & 0x40 ) != 0 )
#define FD_Ctl2Lo()      ( ( _inp( ext->CtlPort1 ) & 0x40 ) == 0 )

#define FD_RaiseCtl2()   ( _outp( ext->DataPort, 0x08 ) )
#define FD_LowerCtl2()   ( _outp( ext->DataPort, 0x00 ) )

#define FD_ReadData()     ( ( _inp( ext->CtlPort1 ) >> 3 ) & 0x0f  )
#define FD_WriteData(data) ( _outp( ext->DataPort, data ) )

/***************** Cable Detection MACROS ****************************/

/*
 * This operation disables bits 3,2,0 in CtrlPort2 (LowerCtl1 fixes it)
 */
#define XX_RaiseCtl1()   ( _outp( ext->CtlPort2, 0x01 ) )

/*********************************************************************/

#define TWIDDLE_THUMBS  if( wait == RELINQUISH ) { NothingToDo(); }\
                else if( wait != KEEP && wait < Ticks() ) return( TIMEOUT )

#define PARALLEL_REGISTER_SPAN 3

typedef struct _DEVICE_EXTENSION {
    // Points to the device object that contains
    // this device extension.
    PDEVICE_OBJECT                  DeviceObject;
    
    // Points to the port device object that this class device is
    // connected to.
    PDEVICE_OBJECT                  PortDeviceObject;
    
    // This holds the result of the get parallel port info
    // request to the port driver.
    PHYSICAL_ADDRESS                OriginalController;
    PUCHAR                          Controller;
    ULONG                           SpanOfController;
    PPARALLEL_FREE_ROUTINE          FreePort;
    PPARALLEL_TRY_ALLOCATE_ROUTINE  TryAllocatePort;
    PVOID                           AllocFreePortContext;
    
    // Records whether we actually created the symbolic link name
    // at driver load time and the symbolic link itself.  If we didn't
    // create it, we won't try to destroy it when we unload.
    BOOLEAN                         CreatedSymbolicLink;
    UNICODE_STRING                  SymbolicLinkName;
    
    // Internal variables used by the driver
    unsigned                        DataPort;
    unsigned                        CtlPort1;
    unsigned                        CtlPort2;
    UCHAR                           CableType;
    UCHAR                           TwidleCount;
    bool                            TwidleOn;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// This is the "structure" of the IOPM.  It is just a simple
// character array of length 0x2000.
//
// This holds 8K * 8 bits -> 64K bits of the IOPM, which maps the
// entire 64K I/O space of the x86 processor.  Any 0 bits will give
// access to the corresponding port for user mode processes.  Any 1
// bits will disallow I/O access to the corresponding port.
#define IOPM_SIZE       0x2000
typedef UCHAR IOPM[IOPM_SIZE];

// This will hold simply an array of 0's which will be copied
// into our actual IOPM in the TSS by Ke386SetIoAccessMap().
// The memory is allocated at driver load time.
IOPM *IOPM_local = 0;
IOPM *IOPM_saved = 0;

/*-------------------------- Implementation -------------------------------*/

// These are the three undocumented calls that we will use to give
// the calling process I/O access.
//
// Ke386IoSetAccessMap() copies the passed map to the TSS.
//
// Ke386IoSetAccessProcess() adjusts the IOPM offset pointer so that
// the newly copied map is actually used.  Otherwise, the IOPM offset
// points beyond the end of the TSS segment limit, causing any I/O
// access by the user mode process to generate an exception.
void PORTSTDCALL Ke386SetIoAccessMap(int, IOPM *);
void PORTSTDCALL Ke386QueryIoAccessMap(int, IOPM *);
void PORTSTDCALL Ke386IoSetAccessProcess(PEPROCESS, int);

void NTAPI ZwYieldExecution(void);

void NothingToDo(void)
{
    ZwYieldExecution();
}

unsigned long Ticks(void)
{
    _int64 ticks;

    KeQueryTickCount((PLARGE_INTEGER)&ticks);
    return (unsigned long)(ticks / 10);
}

/*
 * if wait is not KEEP or RELINQUISH it is the latest time that this
 * operation should take before it times out
 */

static int DataGet(
    PDEVICE_EXTENSION ext,
    unsigned long wait )
{
    UCHAR                data;
    
    switch( ext->CableType ) {
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
 * operation should take before it times out
 */

static int DataPut(
    PDEVICE_EXTENSION ext,
    unsigned data,
    unsigned long wait )
{
    switch( ext->CableType ) {
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

unsigned RemoteGet(
    PDEVICE_EXTENSION ext,
    char *rec,
    unsigned len )
{
    unsigned    get_len;
    unsigned    i;
    
    get_len = DataGet( ext, RELINQUISH );
    if( get_len & 0x80 ) {
        get_len = ((get_len & 0x7f) << 8) | DataGet( ext, KEEP );
    }
    i = get_len;
    for( ; i != 0; --i, ++rec ) {
        *rec = DataGet( ext, KEEP );
    }
    return( get_len );
}

unsigned RemotePut(
    PDEVICE_EXTENSION ext,
    char *snd,
    unsigned len )
{
    unsigned    count;
    
    if( len >= 0x80 ) {
        DataPut( ext, ((len >> 8) | 0x80), RELINQUISH );
    }
    DataPut( ext, (len & 0xff), RELINQUISH );
    for( count = len; count != 0; --count, ++snd ) {
        DataPut( ext, *snd, KEEP );
    }
    return( len );
}

/*
 * Synch - see if server and client are ready
 */

static bool Synch(
    PDEVICE_EXTENSION ext)
{
    switch( ext->CableType ) {
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

static bool CountTwidle(
    PDEVICE_EXTENSION ext)
{
    UCHAR                type;
    
    type = ReadData();
    if( !ext->TwidleOn ) {
        if( type == WATCOM_VAL ||
            type == FMR_VAL ||
            type == LAPLINK_VAL ||
            type == DUTCHMAN_VAL ) {
            ext->TwidleOn = TRUE;
            if( type != ext->CableType ) {
                ext->TwidleCount = 0;
                ext->CableType = type;
            }
        }
    } else {
        if( type != ext->CableType )  {
            ext->TwidleCount ++;
            ext->TwidleOn = FALSE;
            if( ext->TwidleCount == TWIDLE_NUM ) return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * Twidle - send an intermittent pulse over a line to let the person
 *          at the other end know you're there
 */

static bool Twidle(
    PDEVICE_EXTENSION ext,
    bool check )
{
    unsigned            i;
    unsigned long       time;
    
    for( i = 20; i != 0; i-- ) {
        WriteData( TWIDLE_ON );
        time = Ticks() + TWIDLE_TIME;
        while( time > Ticks() ){
            if( check ) {
                if( CountTwidle(ext) ) {
                    return( TRUE );
                }
            } else {
                if( Synch(ext) ) {
                    return( TRUE );
                }
            }
        }
        WriteData( TWIDLE_OFF );
        time = Ticks() + TWIDLE_TIME;
        while( time > Ticks() ){
            if( check ) {
                if( CountTwidle(ext) ) {
                    return( TRUE );
                }
            } else {
                if( Synch(ext) ) {
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

static bool LineTestServer(
    PDEVICE_EXTENSION ext)
{
    unsigned            send;
    unsigned long       time;
    unsigned            ret;
    
    for( send = 1; send != 256; send *= 2 ) {
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        ret = DataPut( ext, send, time );
        if( ret == TIMEOUT ) return( FALSE );
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        ret = DataGet( ext, time );
        if( ret == TIMEOUT ) return( FALSE );
        if( ret != send ) {
            return( FALSE );
        }
    }
    time = Ticks() + LINE_TEST_WAIT;
    if( time == RELINQUISH ) time ++;
    if( time == KEEP ) time ++;
    ret = DataPut( ext, DONE_LINE_TEST, time );
    if( ret == TIMEOUT ) return( FALSE );
    return( TRUE );
}

/*
 * LineTest - make sure that all lines are working
 */

static bool LineTestClient(
    PDEVICE_EXTENSION ext)
{
    unsigned            send;
    unsigned long       time;
    
    send = 0;
    for( ;; ) {
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        send = DataGet( ext, time );
        if( send == TIMEOUT ) return( FALSE );
        if( send == DONE_LINE_TEST ) break;
        time = Ticks() + LINE_TEST_WAIT;
        if( time == RELINQUISH ) time ++;
        if( time == KEEP ) time ++;
        DataPut( ext, send, time );
        if( send == TIMEOUT ) return( FALSE );
    }
    return( TRUE );
}

int RemoteConnectServer(
    PDEVICE_EXTENSION ext)
{
    unsigned long       time;
    bool                got_twidles;
    
    if( !CountTwidle(ext) ) return( 0 );
    got_twidles = Twidle( ext, FALSE );
    switch( ext->CableType ) {
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
    if( !got_twidles ) {
        time = Ticks() + SYNCH_WAIT;
        for( ;; ) {
            if( Synch(ext) ) {
                break;
            } else if( time < Ticks() ) {
                return( 0 );
            }
        }
    }
    if( !LineTestServer(ext) ) return( FALSE );
    return( TRUE );
}

int RemoteConnectClient(
    PDEVICE_EXTENSION ext)
{
    unsigned long       time;
    
    if( !Twidle( ext, TRUE ) )
        return( FALSE );
    switch( ext->CableType ) {
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
    time = Ticks() + SYNCH_WAIT;
    for( ;; ) {
        if( Synch(ext) ) {
            break;
        } else if( time < Ticks() ) {
            return( 0 );
        }
    }
    if( !LineTestClient(ext) ) return( FALSE );
    return( TRUE );
}

void RemoteDisco(
    PDEVICE_EXTENSION ext)
{
    unsigned long       time;
    
    time = Ticks() + TWIDLE_TIME;
    while( time > Ticks() ) { /* delay while other side catches up */ }
    WriteData( TWIDLE_OFF );            /* initialize control ports */
    XX_RaiseCtl1();
    ext->TwidleCount = 0;
    ext->CableType = NULL_VAL;
    ext->TwidleOn = FALSE;
}

void RemoteLink(
    PDEVICE_EXTENSION ext)
{
    ext->DataPort = 0;
    ext->CtlPort1 = ext->DataPort + 1;
    ext->CtlPort2 = ext->CtlPort1 + 1;
    WriteData( TWIDLE_OFF );            /* initialize the control ports */
    XX_RaiseCtl1();
    ext->TwidleCount = 0;
    ext->CableType = NULL_VAL;
    ext->TwidleOn = FALSE;
}

/****************************************************************************
PARAMETERS:
ParallelPortNumber  - Supplies the port number.
PortName            - Returns the port name.
ClassName           - Returns the class name.
LinkName            - Returns the symbolic link name.

RETURNS:
FALSE on Failure, TRUE on Success.

REMARKS:
This routine generates the names \Device\ParallelPortN and
\Device\ParallelDebugN, \DosDevices\DBGPORTn.
****************************************************************************/
BOOLEAN ParMakeNames(
    IN  ULONG           ParallelPortNumber,
    OUT PUNICODE_STRING PortName,
    OUT PUNICODE_STRING ClassName,
    OUT PUNICODE_STRING LinkName)
{
    UNICODE_STRING  prefix, digits, linkPrefix, linkDigits;
    WCHAR           digitsBuffer[10], linkDigitsBuffer[10];
    UNICODE_STRING  portSuffix, classSuffix, linkSuffix;
    NTSTATUS        status;
    
    // Put together local variables for constructing names.
    RtlInitUnicodeString(&prefix, L"\\Device\\");
    RtlInitUnicodeString(&linkPrefix, L"\\DosDevices\\");
    RtlInitUnicodeString(&portSuffix, DD_PARALLEL_PORT_BASE_NAME_U);
    RtlInitUnicodeString(&classSuffix, L"ParallelDebug");
    RtlInitUnicodeString(&linkSuffix, L"DBGPORT");
    digits.Length = 0;
    digits.MaximumLength = 20;
    digits.Buffer = digitsBuffer;
    linkDigits.Length = 0;
    linkDigits.MaximumLength = 20;
    linkDigits.Buffer = linkDigitsBuffer;
    status = RtlIntegerToUnicodeString(ParallelPortNumber, 10, &digits);
    if (!NT_SUCCESS(status))
        return FALSE;
    status = RtlIntegerToUnicodeString(ParallelPortNumber + 1, 10, &linkDigits);
    if (!NT_SUCCESS(status))
        return FALSE;
    
    // Make the port name.
    PortName->Length = 0;
    PortName->MaximumLength = prefix.Length + portSuffix.Length + digits.Length + sizeof(WCHAR);
    PortName->Buffer = ExAllocatePool(PagedPool, PortName->MaximumLength);
    if (!PortName->Buffer)
        return FALSE;
    RtlZeroMemory(PortName->Buffer, PortName->MaximumLength);
    RtlAppendUnicodeStringToString(PortName, &prefix);
    RtlAppendUnicodeStringToString(PortName, &portSuffix);
    RtlAppendUnicodeStringToString(PortName, &digits);
    
    // Make the class name.
    ClassName->Length = 0;
    ClassName->MaximumLength = prefix.Length + classSuffix.Length + digits.Length + sizeof(WCHAR);
    ClassName->Buffer = ExAllocatePool(PagedPool, ClassName->MaximumLength);
    if (!ClassName->Buffer) {
        ExFreePool(PortName->Buffer);
        return FALSE;
    }
    RtlZeroMemory(ClassName->Buffer, ClassName->MaximumLength);
    RtlAppendUnicodeStringToString(ClassName, &prefix);
    RtlAppendUnicodeStringToString(ClassName, &classSuffix);
    RtlAppendUnicodeStringToString(ClassName, &digits);
    
    // Make the link name.
    LinkName->Length = 0;
    LinkName->MaximumLength = linkPrefix.Length + linkSuffix.Length + linkDigits.Length + sizeof(WCHAR);
    LinkName->Buffer = ExAllocatePool(PagedPool, LinkName->MaximumLength);
    if (!LinkName->Buffer) {
        ExFreePool(PortName->Buffer);
        ExFreePool(ClassName->Buffer);
        return FALSE;
    }
    RtlZeroMemory(LinkName->Buffer, LinkName->MaximumLength);
    RtlAppendUnicodeStringToString(LinkName, &linkPrefix);
    RtlAppendUnicodeStringToString(LinkName, &linkSuffix);
    RtlAppendUnicodeStringToString(LinkName, &linkDigits);
    return TRUE;
}

/****************************************************************************
PARAMETERS:
Extension   - Supplies the device extension.

RETURNS:
STATUS_SUCCESS on Success, !STATUS_SUCCESS on Failure.

REMARKS:
This routine will request the port information from the port driver
and fill it in the device extension.
****************************************************************************/
NTSTATUS ParGetPortInfoFromPortDevice(
    IN OUT  PDEVICE_EXTENSION   Extension)
{
    KEVENT                      event;
    PIRP                        irp;
    PARALLEL_PORT_INFORMATION   portInfo;
    IO_STATUS_BLOCK             ioStatus;
    NTSTATUS                    status;
    
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO,
        Extension->PortDeviceObject,
        NULL, 0, &portInfo,
        sizeof(PARALLEL_PORT_INFORMATION),
        TRUE, &event, &ioStatus);
    if (!irp)
        return STATUS_INSUFFICIENT_RESOURCES;
    status = IoCallDriver(Extension->PortDeviceObject, irp);
    if (!NT_SUCCESS(status))
        return status;
    status = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    if (!NT_SUCCESS(status))
        return status;
    Extension->OriginalController = portInfo.OriginalController;
    Extension->Controller = portInfo.Controller;
    Extension->SpanOfController = portInfo.SpanOfController;
    Extension->TryAllocatePort = portInfo.TryAllocatePort;
    Extension->FreePort = portInfo.FreePort;
    Extension->AllocFreePortContext = portInfo.Context;
    if (Extension->SpanOfController < PARALLEL_REGISTER_SPAN)
        return STATUS_INSUFFICIENT_RESOURCES;
    return status;
}

/****************************************************************************
PARAMETERS:
DriverObject        - Supplies the driver object.
ParallelPortNumber  - Supplies the number for this port.

REMARKS:
This routine is called for every parallel port in the system.  It
will create a class device upon connecting to the port device
corresponding to it.
****************************************************************************/
VOID ParInitializeDeviceObject(
    IN  PDRIVER_OBJECT  DriverObject,
    IN  ULONG           ParallelPortNumber)
{
    UNICODE_STRING      portName, className, linkName;
    NTSTATUS            status;
    PDEVICE_OBJECT      deviceObject;
    PDEVICE_EXTENSION   ext;
    PFILE_OBJECT        fileObject;
    
    // Cobble together the port and class device names.
    if (!ParMakeNames(ParallelPortNumber, &portName, &className, &linkName))
        return;
    
    // Create the device object.
    status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION),
        &className, FILE_DEVICE_PARALLEL_PORT, 0, TRUE,
        &deviceObject);
    if (!NT_SUCCESS(status)) {
        ExFreePool(linkName.Buffer);
        goto Cleanup;
    }
    
    // Now that the device has been created,
    // set up the device extension.
    ext = deviceObject->DeviceExtension;
    RtlZeroMemory(ext, sizeof(DEVICE_EXTENSION));
    ext->DeviceObject = deviceObject;
    deviceObject->Flags |= DO_BUFFERED_IO;
    status = IoGetDeviceObjectPointer(&portName, FILE_READ_ATTRIBUTES,
        &fileObject,
        &ext->PortDeviceObject);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        ExFreePool(linkName.Buffer);
        goto Cleanup;
    }
    ObDereferenceObject(fileObject);
    ext->DeviceObject->StackSize = ext->PortDeviceObject->StackSize + 1;
    
    // Get the port information from the port device object.
    status = ParGetPortInfoFromPortDevice(ext);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        ExFreePool(linkName.Buffer);
        goto Cleanup;
    }
    
    // Set up the symbolic link for windows apps.
    status = IoCreateSymbolicLink(&linkName, &className);
    if (!NT_SUCCESS(status)) {
        ext->CreatedSymbolicLink = FALSE;
        ExFreePool(linkName.Buffer);
        goto Cleanup;
    }
    
    // We were able to create the symbolic link, so record this
    // value in the extension for cleanup at unload time.
    ext->CreatedSymbolicLink = TRUE;
    ext->SymbolicLinkName = linkName;
    
Cleanup:
    ExFreePool(portName.Buffer);
    ExFreePool(className.Buffer);
}

/****************************************************************************
REMARKS:
Set the IOPM (I/O permission map) of the calling process so that it
is given full I/O access.  Our IOPM_local[] array is all zeros, so
the IOPM will be all zeros.  If OnFlag is 1, the process is given I/O
access.  If it is 0, access is removed.
****************************************************************************/
VOID SetIOPermissionMap( int OnFlag )
{
    if (OnFlag) {
        /* Enable I/O for the process */
        Ke386QueryIoAccessMap(1,IOPM_saved);
        Ke386IoSetAccessProcess(PsGetCurrentProcess(), 1);
        Ke386SetIoAccessMap(1, IOPM_local);
    } else {
        /* Disable I/O for the process, restoring old IOPM table */
        Ke386IoSetAccessProcess(PsGetCurrentProcess(), 0);
        Ke386SetIoAccessMap(1, IOPM_saved);
    }
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

RETURNS:
STATUS_SUCCESS          - Success.
STATUS_NOT_A_DIRECTORY  - This device is not a directory.

REMARKS:
This routine is the dispatch for create requests.
****************************************************************************/
NTSTATUS PORTSTDCALL ParCreate(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PIO_STACK_LOCATION  irpSp;
    NTSTATUS            status;
    PDEVICE_EXTENSION   ext;
    
    // Give the debugger process full I/O port access. Ideally we should
    // restrict this to the actual I/O ports in use, and this can be done
    // in the future if desired.
    SetIOPermissionMap(1);
    
    // Now create the parallel port extension device
    ext = DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    if (irpSp->Parameters.Create.Options & FILE_DIRECTORY_FILE)
        status = STATUS_NOT_A_DIRECTORY;
    else if (!ext->TryAllocatePort(ext->AllocFreePortContext))
        status = STATUS_DEVICE_BUSY;
    else
        status = STATUS_SUCCESS;
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the cancel routine for this driver.
****************************************************************************/
VOID ParCancel(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the IOCtl routine for this driver.
****************************************************************************/
NTSTATUS PORTSTDCALL ParIOCTL(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PIO_STACK_LOCATION  irpSp;
    NTSTATUS            status;
    PDEVICE_EXTENSION   ext;
    DBGPORT_IO                      *IOBuffer;
    
    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof( *IOBuffer );
    ext = DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    IOBuffer = (DBGPORT_IO *)Irp->AssociatedIrp.SystemBuffer;
    
    // NT copies inbuf here before entry and copies this to outbuf after
    // return, for METHOD_BUFFERED IOCTL's.
    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_DBG_READ_PORT_U8:
        IOBuffer->data.u8 = _inp(IOBuffer->port);
        break;
    case IOCTL_DBG_READ_PORT_U16:
        IOBuffer->data.u16 = READ_PORT_USHORT((PUSHORT)(ext->Controller + IOBuffer->port));
        break;
    case IOCTL_DBG_READ_PORT_U32:
        IOBuffer->data.u32 = READ_PORT_ULONG((PULONG)(ext->Controller + IOBuffer->port));
        break;
    case IOCTL_DBG_WRITE_PORT_U8:
        _outp(IOBuffer->port,IOBuffer->data.u8);
        break;
    case IOCTL_DBG_WRITE_PORT_U16:
        WRITE_PORT_USHORT((PUSHORT)(ext->Controller + IOBuffer->port), IOBuffer->data.u16);
        break;
    case IOCTL_DBG_WRITE_PORT_U32:
        WRITE_PORT_ULONG((PULONG)(ext->Controller + IOBuffer->port), IOBuffer->data.u32);
        break;
    case IOCTL_DBG_REMOTE_GET:
        IOBuffer->status = RemoteGet(ext,IOBuffer->buffer,IOBuffer->len);
        break;
    case IOCTL_DBG_REMOTE_PUT:
        IOBuffer->status = RemotePut(ext,IOBuffer->buffer,IOBuffer->len);
        break;
    case IOCTL_DBG_REMOTE_CONNECT_SERV:
        IOBuffer->status = RemoteConnectServer(ext);
        break;
    case IOCTL_DBG_REMOTE_CONNECT_CLIENT:
        IOBuffer->status = RemoteConnectClient(ext);
        break;
    case IOCTL_DBG_REMOTE_DISCO:
        RemoteDisco(ext);
        break;
    case IOCTL_DBG_REMOTE_LINK:
        RemoteLink(ext);
        break;
    default:
        Irp->IoStatus.Information = 0;
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the cleanup routine for this driver.
****************************************************************************/
NTSTATUS PORTSTDCALL ParCleanup(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_CANCELLED;
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the close routine for this driver.
****************************************************************************/
NTSTATUS PORTSTDCALL ParClose(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PDEVICE_EXTENSION   ext;
    
    // Restore the original I/O port mappings
    SetIOPermissionMap(0);
    
    ext = DeviceObject->DeviceExtension;
    ext->FreePort(ext->AllocFreePortContext);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

/****************************************************************************
PARAMETERS:
DriverObject    - Supplies the driver object.

REMARKS:
This routine loops through the device list and cleans up after
each of the devices.
****************************************************************************/
VOID PORTSTDCALL ParUnload(
    IN  PDRIVER_OBJECT  DriverObject)
{
    PDEVICE_OBJECT          currentDevice;
    PDEVICE_EXTENSION   ext;
    
    while (currentDevice = DriverObject->DeviceObject) {
        ext = currentDevice->DeviceExtension;
        if (ext->CreatedSymbolicLink) {
            IoDeleteSymbolicLink(&ext->SymbolicLinkName);
            ExFreePool(ext->SymbolicLinkName.Buffer);
        }
        IoDeleteDevice(currentDevice);
    }
    
    // Free the local IOPM table if allocated
    if (IOPM_local)
        MmFreeNonCachedMemory(IOPM_local, sizeof(IOPM));
    if (IOPM_saved) {
        MmFreeNonCachedMemory(IOPM_saved, sizeof(IOPM));
    }
}

/****************************************************************************
PARAMETERS:
DriverObject    - Supplies the driver object.
RegistryPath    - Supplies the registry path for this driver.

REMARKS:
This routine is called at system initialization time to initialize
this driver.
****************************************************************************/
NTSTATUS PORTSTDCALL DriverEntry(
    IN  PDRIVER_OBJECT  DriverObject,
    IN  PUNICODE_STRING RegistryPath)
{
    ULONG       i;
    
    // TODO: We should be able to re-code this driver to use a call-gate
    //               to give the calling process full IOPL access, without needing
    //               the gross IOPM hack we currently use. This would make it
    //               slightly faster also.
    
    // Allocate a buffer for the local IOPM and zero it.
    IOPM_local = MmAllocateNonCachedMemory(sizeof(IOPM));
    IOPM_saved = MmAllocateNonCachedMemory(sizeof(IOPM));
    if (!IOPM_local || !IOPM_saved)
        return STATUS_INSUFFICIENT_RESOURCES;
    RtlZeroMemory(IOPM_local, sizeof(IOPM));
    Ke386QueryIoAccessMap(1,IOPM_saved);
    
    // Initialise all the device objects
    for (i = 0; i < IoGetConfigurationInformation()->ParallelCount; i++)
        ParInitializeDeviceObject(DriverObject, i);
    if (!DriverObject->DeviceObject)
        return STATUS_NO_SUCH_DEVICE;
    
    // Initialize the Driver Object with driver's entry points
    DriverObject->MajorFunction[IRP_MJ_CREATE] = ParCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = ParClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = ParCleanup;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ParIOCTL;
    DriverObject->DriverUnload = ParUnload;
    return STATUS_SUCCESS;
}
