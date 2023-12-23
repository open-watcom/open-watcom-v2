/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#if defined( __UNIX__ ) && !defined( __QNX__ )
    #define MILLISEC_PER_TICK   1UL
    #define TICK_TYPE           unsigned short
#else
    #define MILLISEC_PER_TICK   55UL
    #define TICK_TYPE           unsigned char
#endif

#define MSEC2TICK( amt )        ((2 * (amt) + MILLISEC_PER_TICK) / (2 * MILLISEC_PER_TICK))
#define SEC2TICK( amt )         (MSEC2TICK( (amt) * 1000L ))
#define MAX_BAUD_SET_MS         330     /* ms */
#define BREAK_TIME_MS           250     /* ms */
#define FOREVER                 0       /* must be defined as 0 */

#define SDATA_HI                0x01    /* start of conversation */
#define SDATA_STX               0x02    /* start of text */
#define SDATA_ETX               0x03    /* end of text */
#define SDATA_ACK               0x06    /* acknowledge */
#define SDATA_NAK               0x15    /* non-acknowledge */
#define SDATA_TAK               0xAA    /* acknowledge test ok (TestAcK)*/
#define SDATA_RLR               0x1D    /* request last response */
#define SDATA_LOK               0xFF    /* lock step */
#define SDATA_NO_DATA           (-1)    /* no new character in receive buffer */

#define SYNC_BYTE               0x08    /* first byte to sync on */
#define SYNC_END                0x04    /* end of sync bytes */
#define SYNC_LEN                256     /* length of sync string */
#define SYNC_INC                11

/* message structure */
#define MSTX                    0       /* start of message */
#define MCRC                    1       /* CRC value */
#define MLEN                    3       /* length of data */
#define MBLKNO                  5       /* number of this block */
#define MERR                    7       /* previous BlockSend() # of Errors */
#define MDATA                   8       /* data area */

#define MAX_ERRORS              2       /* max retries before ReSync */

#define CRC_VAL                 0x1021  /* value for CRC */

/* define the time needed to send a SYNC_LEN characters back and forth */
#define BITS_PER_CHAR           12L
#define SYNC_SLOP_MS            110     /* ms */
#define TEST_SLOP_MS            550     /* ms */
#define SYNC_TIME_OUT_MS        550     /* ms */

#define TEST_TIME( x ) \
    ( MSEC2TICK( SYNC_SLOP_MS ) \
    + MSEC2TICK( TEST_SLOP_MS )             /* max time executing */ \
    + (2 * MSEC2TICK( SYNC_TIME_OUT_MS ))   /* max timeout wait */ \
    + ((2 * SYNC_LEN * BITS_PER_CHAR * MSEC2TICK(1000 + MILLISEC_PER_TICK - 1)) / x) )

#define BAUD_ENTRIES \
    BAUD_ENTRY( 115200, 0,      1 ) \
    BAUD_ENTRY( 57600,  57600,  2 ) \
    BAUD_ENTRY( 38400,  38400,  3 ) \
    BAUD_ENTRY( 19200,  19200,  6 ) \
    BAUD_ENTRY( 9600,   9600,   12 ) \
    BAUD_ENTRY( 4800,   4800,   24 ) \
    BAUD_ENTRY( 2400,   2400,   48 ) \
    BAUD_ENTRY( 1200,   1200,   96 )

typedef enum {
    UNDEF_BAUD = -1,
    #define BAUD_ENTRY(x,v,d)   Baud_ ## x,
    BAUD_ENTRIES
    #undef BAUD_ENTRY
    MODEM_BAUD,
} baud_index;
#define MAX_BAUD    Baud_115200
#define MIN_BAUD    Baud_1200

typedef struct baud_entry {
    char        *name;
    int         len;
    TICK_TYPE   full_test_ticks;
} baud_entry;
