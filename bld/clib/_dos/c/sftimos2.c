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
* Description:  Implementation of _dos_setftime() for OS/2.
*
****************************************************************************/


#include "variety.h"
#include <dos.h>
#include <wos2.h>
#include "seterrno.h"


_WCRTLINK unsigned _dos_setftime( int handle, unsigned date, unsigned time )
{
    APIRET          rc;
    OS_UINT         hand_type;
    OS_UINT         device_attr;
    FILESTATUS      info;
    USHORT          *p;

    rc = DosQHandType( handle, &hand_type, &device_attr );
    if( rc == 0 ) {
        if( ( hand_type & ~HANDTYPE_NETWORK ) == HANDTYPE_FILE ) {
            rc = DosQFileInfo( handle, 1, (PBYTE)&info, sizeof( FILESTATUS ) );
            if( rc == 0 ) {
                p = (USHORT *)(&info.fdateLastWrite);
                *p = date;
                p = (USHORT *)(&info.ftimeLastWrite);
                *p = time;
                rc = DosSetFileInfo( handle, 1, (PBYTE)&info, sizeof( FILESTATUS ) );
            }
        }
    }
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    return( 0 );
}
