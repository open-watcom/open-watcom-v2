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


#include "variety.h"
#include "necibm.h"
#include "nonibm.h"

#define SECONDS_PER_MINUTE      60
#define SECONDS_PER_HOUR        ( 60 * SECONDS_PER_MINUTE )
#define BCD_GET_HIGH( w )       ( w >> 4 )
#define BCD_GET_LOW( w )        ( w & 0x000F )


_WCRTLINK unsigned short _bios_timeofday( unsigned cmd, long *timeval )
{
    unsigned char       necBuf[6];

    if( !__NonIBM ) {
        return( __ibm_bios_timeofday( cmd, timeval ) );
    } else {
        /*** Translate IBM commands to NEC98 commands ***/
        switch( cmd ) {
          case _IBM_TIME_GETCLOCK:
            __nec98_bios_timeofday( _NEC98_TIME_GETCLOCK, necBuf );
            nec_to_ibm( necBuf, timeval );
            break;
          case _IBM_TIME_SETCLOCK:
            __nec98_bios_timeofday( _NEC98_TIME_GETCLOCK, necBuf );
            ibm_to_nec( necBuf, timeval );
            __nec98_bios_timeofday( _NEC98_TIME_SETCLOCK, necBuf );
            break;
          default:
            return( -1 );       // invalid command for NEC 98
        }
        return( 0 );
    }
}


static void nec_to_ibm( const unsigned char *necBuf, long *timeval )
{
    unsigned            hours;
    unsigned            minutes;
    unsigned            seconds;
    long                time;

    /*** Extract info from BCD encoding ***/
    hours = BCD_GET_HIGH( necBuf[3] ) * 10;
    hours += BCD_GET_LOW( necBuf[3] );
    minutes = BCD_GET_HIGH( necBuf[4] ) * 10;
    minutes += BCD_GET_LOW( necBuf[4] );
    seconds = BCD_GET_HIGH( necBuf[5] ) * 10;
    seconds += BCD_GET_LOW( necBuf[5] );

    /*** Convert to seconds from hours/minutes/seconds ***/
    time = (long)hours * SECONDS_PER_HOUR;
    time += (long)minutes * SECONDS_PER_MINUTE;
    time += seconds;

    /*** About 18.2 ticks per second ***/
    time = (time * 182) / 10;
    *timeval = time;
}


static void ibm_to_nec( unsigned char *necBuf, const long *timeval )
{
    unsigned            hours;
    unsigned            minutes;
    unsigned            seconds;
    long                time;
    unsigned            high, low;

    /*** Convert from 18.2 Hz ticks to hours/minutes/seconds ***/
    time = (*timeval * 10)  / 182;
    hours = time / SECONDS_PER_HOUR;
    time %= SECONDS_PER_HOUR;
    minutes = time / SECONDS_PER_MINUTE;
    time %= SECONDS_PER_MINUTE;
    seconds = time;

    /*** Store info in BCD encoding ***/
    high = hours / 10;
    low = hours % 10;
    necBuf[3] = (high<<4) | low;
    high = minutes / 10;
    low = minutes % 10;
    necBuf[4] = (high<<4) | low;
    high = seconds / 10;
    low = seconds % 10;
    necBuf[5] = (high<<4) | low;
}
