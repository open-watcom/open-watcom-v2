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


#include "cvars.h"
#include "sjis2unc.h"

unsigned JIS2Unicode( unsigned dbc )
{
    unsigned short hibyte;
    unsigned short lobyte;

    hibyte = ( dbc >> 8 ) & 0x00FF;
    lobyte = dbc & 0x00FF;
    if( dbc >= 0x8140 && dbc <= 0x84be ) {
        // use table 81_84
        dbc = SJIS_81_84_UNICODE_TABLE[
                    (hibyte - 0x81) * (0xFC - 0x3F) + (lobyte - 0x40)];
    } else if( dbc >= 0x8740 && dbc <= 0x878f ) {
        // use 0x87 table, these codes are from AX version of SJIS
        dbc = SJIS_87_UNICODE_TABLE[dbc - 0x8740];
    } else if( dbc >= 0x889f && dbc <= 0x9ffc ) {
        // use 88_9f table
        dbc = SJIS_88_9F_UNICODE_TABLE[
                    (hibyte - 0x88) * (0xFC - 0x3F) + (lobyte - 0x40) -
                    (0x889f - 0x8840)];
    } else if( dbc >= 0xe040 && dbc <= 0xeaa4 ) {
        // use e0_ea table
        dbc = SJIS_E0_EA_UNICODE_TABLE[
                    (hibyte - 0xE0) * (0xFC - 0x3F) + (lobyte - 0x40)];
    } else if( dbc >= 0x00a1 && dbc <= 0x00df ) {
        // halfwidth Katakana chars
        dbc += 0xff00 - 0x0040;
    } else {
        switch( dbc ) {
//      case 0x5c:      dbc = 0x00a5;   break;  /* JBS 93/07/30 */
        case 0x7e:      dbc = 0x00af;   break;
        case 0x80:      dbc = 0x00a2;   break;
        case 0xa0:      dbc = 0x00a3;   break;
        case 0xfd:      dbc = 0x00ac;   break;
        case 0xff:      dbc = 0x007e;   break;
        }
    }
    return( dbc );
}
