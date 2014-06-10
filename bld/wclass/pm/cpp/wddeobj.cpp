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


#include <string.h>
#include "wddeobj.hpp"


PDDESTRUCT WEXPORT WDDEObject::makeDDEObject( HWND hwnd,
                                              const char *item_name,
                                              USHORT fsStatus,
                                              USHORT usFormat,
                                              const void *data,
                                              size_t data_len ) {
/***************************************************************/

    DDESTRUCT   *dde;
    size_t      item_len;
    PID         pid;
    TID         tid;

    if( item_name != NULL ) {
        item_len = strlen( item_name ) + 1;
    } else {
        item_len = 1;
    }
    if( !DosAllocSharedMem( (PPVOID)&dde, NULL, sizeof( DDESTRUCT ) +
                            item_len + data_len,
                            PAG_COMMIT | PAG_READ | PAG_WRITE | OBJ_GIVEABLE ) ) {
        WinQueryWindowProcess( hwnd, &pid, &tid );
        DosGiveSharedMem( (PVOID)&dde, pid, PAG_READ | PAG_WRITE );
        dde->cbData = data_len;
        dde->fsStatus = fsStatus;
        dde->usFormat = usFormat;
        dde->offszItemName = sizeof( DDESTRUCT );
        if( (data_len != 0) && (data != NULL) ) {
            dde->offabData = (USHORT)(sizeof( DDESTRUCT ) + item_len);
        } else {
            dde->offabData = 0;
        }
        if( item_name != NULL ) {
            strcpy( (char *)DDES_PSZITEMNAME( dde ), item_name );
        } else {
            strcpy( (char *)DDES_PSZITEMNAME( dde ), "" );
        }
        if( data != NULL ) {
            memcpy( DDES_PABDATA( dde ), data, data_len );
        }
        return( dde );
    }
    return( NULL );
}
