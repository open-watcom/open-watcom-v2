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
#include <bios.h>
#include <stddef.h>
#include "necibm.h"
#include "nonibm.h"


_WCRTLINK unsigned short _bios_keybrd( unsigned cmd )
{
    unsigned short      necRc, ret;

    if( !__NonIBM ) {
        return( __ibm_bios_keybrd( cmd ) );
    } else {
        /*** Translate IBM commands to NEC98 commands ***/
        switch( cmd ) {
          case _KEYBRD_READ:
            return( __nec98_bios_keybrd( cmd, NULL ) );
            break;
          case _KEYBRD_READY:
            return( __nec98_bios_keybrd( cmd, NULL ) );
            break;
          case _KEYBRD_SHIFTSTATUS:
            necRc = __nec98_bios_keybrd( cmd, NULL );
            ret = 0;
            if( necRc & 0x0001 )  ret |= 0x02;
            if( necRc & 0x0002 )  ret |= 0x40;
            if( necRc & 0x0008 )  ret |= 0x08;
            if( necRc & 0x0010 )  ret |= 0x04;
            return( ret );
            break;
          default:
            return( 0 );        // invalid command for NEC 98
        }
    }
}
