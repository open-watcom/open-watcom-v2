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
* Description:  BIOS serial port access.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <bios.h>
#include "necibm.h"
#include "rtdata.h"


/* DOS only, so no multithreading issues with static data */
static struct {
    unsigned char       pad[18];
    unsigned char       data;
    unsigned char       status;
} buf1, buf2, buf3;                     /* one for each com port */

_WCRTLINK unsigned short _bios_serialcom( unsigned cmd, unsigned port, unsigned data )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        struct com_t        necBuf;
        unsigned short      necRc;
        unsigned short      ret;
        unsigned short      buf;

        /*** Initialize the NEC98 com_t buffer ***/
        necBuf.baud = _NEC98_COM_DEFAULT_BAUD;
        necBuf.mode = _NEC98_COM_DEFAULT_MODE;
        necBuf.command = _NEC98_COM_DEFAULT_CMD;
        necBuf.tx_time = _NEC98_COM_DEFAULT_TIME;
        necBuf.rx_time = _NEC98_COM_DEFAULT_TIME;
        necBuf.size = 1;

        /*** Translate port numbers ***/
        switch( port ) {
        case _IBM_COM_CH1:
            port = _NEC98_COM_CH1;
            necBuf.buffer = &buf1;
            break;
        case _IBM_COM_CH2:
            port = _NEC98_COM_CH2;
            necBuf.buffer = &buf2;
            break;
        case _IBM_COM_CH3:
            port = _NEC98_COM_CH3;
            necBuf.buffer = &buf3;
            break;
        default:
            return( 0 );        // invalid port for NEC 98
        }

        /*** Translate IBM commands to NEC98 commands ***/
        buf = data;
        ret = 0;    // invalid command for NEC 98
        switch( cmd ) {
        case _IBM_COM_SEND:
            necBuf.buffer = &buf;
            necRc = __nec98_bios_serialcom( _NEC98_COM_SEND, port, &necBuf );
            if( necRc == 0 ) {
                ret = buf & 0x00FF;
            } else {
                ret = 0x8000;           // error: set bit 15
            }
            break;
        case _IBM_COM_RECEIVE:
            necBuf.buffer = &buf;
            necRc = __nec98_bios_serialcom( _NEC98_COM_RECEIVE, port, &necBuf );
            if( necRc == 0 ) {
                ret = buf & 0x00FF;
            } else {
                if( buf & 0x2000 )
                    ret |= 0x0800;      // framing error
                if( buf & 0x1000 )
                    ret |= 0x0200;      // overrun error
                if( buf & 0x0800 )
                    ret |= 0x0400;      // parity error
                if( buf & 0x0400 )
                    ret |= 0x2000;      // TXE
                if( ret == 0 ) {
                    ret |= 0x8000;      // assume timed out
                }
            }
            break;
        case _IBM_COM_INIT:
            /*** Translate baud ***/
            if( (data & _IBM_COM_9600) == _IBM_COM_9600 ) {
                necBuf.baud = _NEC98_COM_9600;
            } else if( (data & _IBM_COM_4800) == _IBM_COM_4800 ) {
                necBuf.baud = _NEC98_COM_4800;
            } else if( (data & _IBM_COM_2400) == _IBM_COM_2400 ) {
                necBuf.baud = _NEC98_COM_2400;
            } else if( (data & _IBM_COM_1200) == _IBM_COM_1200 ) {
                necBuf.baud = _NEC98_COM_1200;
            } else if( (data & _IBM_COM_600) == _IBM_COM_600 ) {
                necBuf.baud = _NEC98_COM_600;
            } else if( (data & _IBM_COM_300) == _IBM_COM_300 ) {
                necBuf.baud = _NEC98_COM_300;
            } else if( (data & _IBM_COM_150) == _IBM_COM_150 ) {
                necBuf.baud = _NEC98_COM_150;
            } else if( (data & _IBM_COM_110) == _IBM_COM_110 ) {
                break;      // invalid baud for NEC 98
            } else {
                necBuf.baud = _NEC98_COM_DEFAULT_BAUD;
            }
            __nec98_bios_serialcom( _NEC98_COM_INIT, port, &necBuf );
            /* fall through */
            /* to _IBM_COM_STATUS get status bits */
        case _IBM_COM_STATUS:
            necBuf.buffer = &buf;
            necRc = __nec98_bios_serialcom( _NEC98_COM_STATUS, port, &necBuf );
            if( buf & 0x0080 )
                ret |= 0x0020;   // data set ready
            if( buf & 0x0040 )
                ret |= 0x1000;   // break detected
            if( buf & 0x0020 )
                ret |= 0x0800;   // framing error
            if( buf & 0x0010 )
                ret |= 0x0200;   // overrun error
            if( buf & 0x0008 )
                ret |= 0x0400;   // parity error
            if( buf & 0x0008 )
                ret |= 0x0400;   // parity error
            break;
        }
        return( ret );
    }
    /* IBM PC */
    return( __ibm_bios_serialcom( cmd, port, data ) );
}
