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


#if defined(__OS_dosos2__) || defined(__OS_os2v2__)
#include <dos.h>
#include <wos2.h>

extern int __set_errno( int );


unsigned _dos_setftime( int handle,
                        unsigned short date,
                        unsigned short time )
/********************************************/
    {
        APIRET          error;
        OS_UINT         hand_type;
        OS_UINT         device_attr;
        FILESTATUS      info;
        USHORT          *p;

        error = DosQHandType( handle, &hand_type, &device_attr );
        if( error ) {
            __set_errno( error );
            return( error );
        }
        if( hand_type == 0 ) {          /* if File-system file */
            error = DosQFileInfo( handle, 1, (PBYTE)&info, sizeof( FILESTATUS ) );
            if( error ) {
                __set_errno( error );
                return( error );
            }
            p = (USHORT *)(&info.fdateLastWrite);
            *(unsigned short *)p = date;
            p = (USHORT *)(&info.ftimeLastWrite);
            *(unsigned short *)p = time;
            error = DosSetFileInfo( handle, 1, (PBYTE)&info, sizeof( FILESTATUS ) );
        }
        return( 0 );
    }

#endif
