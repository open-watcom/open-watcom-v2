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
#include "necibm.h"
#include "nonibm.h"


_WCRTLINK unsigned short _bios_disk( unsigned ibmCmd, struct _ibm_diskinfo_t *ibmBuf )
{
//    unsigned                  necCmd;
//    struct _nec98_diskinfo_t  necBuf;

    if( !__NonIBM ) {
        return( __ibm_bios_disk( ibmCmd, ibmBuf ) );
    } else {
        // Currently this function is not supported on NEC98 machines,
        // since there is no known method of converting the required
        // information in the IBM buffer to NEC98 information (e.g. drive
        // number, cylinder, and sector length).  So just return error code.
        return( 0x01 );

//      /*** Initialize the NEC98 buffer ***/
//      do_whatever_must_be_done
//
//      /*** Translate IBM commands to NEC98 commands ***/
//      switch( ibmCmd ) {
//        case _IBM_DISK_READ:
//          necCmd = _NEC98_DISK_READ;
//          break;
//        case _IBM_DISK_WRITE:
//          necCmd = _NEC98_DISK_WRITE;
//          break;
//        case _IBM_DISK_VERIFY:
//          necCmd = _NEC98_DISK_VERIFY;
//          break;
//        default:
//          return( 0x01 );     // invalid command for NEC 98
//      }
//      return( __nec98_bios_disk( necCmd, &necBuf ) );
    }
}
