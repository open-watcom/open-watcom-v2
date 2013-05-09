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



#define SEC( amt )              (18*(amt)+ 1*((amt)/5))
#define MAX_BAUD_SET_TICKS      6
#define FOREVER                 0 /* must be defined as 0 */

#define SDATA_HI                      0x01 /* start of conversation */
#define SDATA_STX                     0x02 /* start of text */
#define SDATA_ETX                     0x03 /* end of text */
#define SDATA_ACK                     0x06 /* acknowledge */
#define SDATA_NAK                     0x15 /* non-acknowledge */
#define SDATA_TAK                     0xAA /* acknowledge test ok (TestAcK)*/
#define SDATA_RLR                     0x1D /* request last response */
#define SDATA_LOK                     0xFF /* lock step */

#define SDATA_NO_DATA                 (-1) /* no new character in receive buffer */

#define SYNC_BYTE               0x08 /* first byte to sync on */
#define SYNC_END                0x04 /* end of sync bytes */
#define SYNC_LEN                256 /* length of sync string */
#define SYNC_INC                11

/* message structure */
#define MSTX                    0 /* start of message */
#define MCRC                    1 /* CRC value */
#define MLEN                    3 /* length of data */
#define MBLKNO                  5 /* number of this block */
#define MERR                    7 /* previous BlockSend() # of Errors */
#define MDATA                   8 /* data area */

#define MAX_ERRORS              2 /* max retries before ReSync */

#define CRC_VAL                 0x1021 /* value for CRC */

#define SUCCESS                 1  /* this MUST be defined as non-zero */
#define FAIL                    0  /* this MUST be defined as 0 */

typedef struct baud_entry {
    char        *name;
    int         len;
    char        full_test_ticks;
} baud_entry;

/* define the time needed to send a SYNC_LEN characters back and forth */
#define TICKS_PER_SEC   19L       /* really 18.2 but 19 is conservative */
#define BITS_PER_CHAR   12L       /* */
#define SYNC_SLOP       2         /* 10 */
#define TEST_SLOP       10
#define SYNC_TIME_OUT   10

#define TEST_TIME( x ) \
    ( SYNC_SLOP \
    + TEST_SLOP                                 /* max time executing */ \
    + ( 2*SYNC_TIME_OUT )                       /* max timeout wait */ \
    + ( ( SYNC_LEN*2 )                          /* number characters sent */ \
      / ( x / (BITS_PER_CHAR*TICKS_PER_SEC) ) ) )/* characters per timer tick */

#define LOW_BAUD    7
#define MIN_BAUD    8

