/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WPI OS/2 file routines.
*
****************************************************************************/


#include <stdio.h>
#define INCL_PM
#define INCL_DOSDEVICES
#include <os2.h>
#include "wpi.h"


HFILE _wpi_fileopen( LPSTR filename, int format )
/***********************************************/
{
    PM1632_FILESIZETYPE         action;
    HFILE                       hfile;

    if( !(format & (OPEN_SHARE_DENYREAD | OPEN_SHARE_DENYWRITE
                        | OPEN_SHARE_DENYREADWRITE | OPEN_SHARE_DENYNONE)) ) {
        format |= OPEN_SHARE_DENYNONE;
    }

    if( DosOpen( (PSZ)filename, &hfile, &action, 0L,
                FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                format,
                0 ) != 0 ) {
        hfile = (HFILE)-1;
    }
#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( hfile );

} /* _wpi_fileopen */

HFILE _wpi_filecreate( LPSTR filename, int format )
/*************************************************/
{
    PM1632_FILESIZETYPE         action;
    HFILE                       hfile;

    if( !(format & (OPEN_SHARE_DENYREAD | OPEN_SHARE_DENYWRITE
                        | OPEN_SHARE_DENYREADWRITE | OPEN_SHARE_DENYNONE)) ) {
        format |= OPEN_SHARE_DENYNONE;
    }

    if( DosOpen( (PSZ)filename, &hfile, &action, 0,
                FILE_NORMAL,
                OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                format,
                0 ) != 0 ) {
        hfile = (HFILE)-1;
    }

#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( hfile );

} /* _wpi_filecreate */

PM1632_FILESIZETYPE _wpi_fileclose( HFILE hfile )
/***********************************************/
{
    PM1632_FILESIZETYPE     ret;

    ret = (PM1632_FILESIZETYPE)DosClose( hfile );
#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( ret );

} /* _wpi_fileclose */

PM1632_FILESIZETYPE _wpi_filewrite( HFILE hfile, void *buf, PM1632_FILESIZETYPE size )
/************************************************************************************/
{
    PM1632_FILESIZETYPE len;
    PM1632_APIRET       ret;

    ret = DosWrite( hfile, (PVOID)buf, size, &len );
    if( !ret ) {
        len = 0;
    }
    return( len );

} /* _wpi_filewrite */

PM1632_FILESIZETYPE _wpi_fileread( HFILE hfile, void *buf, PM1632_FILESIZETYPE size )
/***********************************************************************************/
{
    PM1632_FILESIZETYPE     len;
    PM1632_APIRET           ret;

    ret = DosRead( hfile, (PVOID)buf, size, &len );
    if( !ret ) {
        len = 0;
    }
    return( len );

} /* _wpi_fileread */
