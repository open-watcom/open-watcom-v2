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
#include <stdio.h>
#include <dos.h>
#include "seterrno.h"
#if defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #define INCL_DOS
    #include <wos2.h>
#endif

_WCRTLINK unsigned _dos_getdiskfree( unsigned dnum, struct _diskfree_t *df )
{
#if defined(__NT__)
    DWORD   spc;
    DWORD   bps;
    DWORD   nfc;
    DWORD   tnc;
    char    path[4];
    char    *pname;

    if( dnum != 0 ) {
        path[0] = 'A' + dnum - 1;
        path[1] = ':';
        path[2] = '\\';
        path[3] = 0;
        pname = path;
    } else {
        pname = NULL;
    }

    if( !GetDiskFreeSpace( pname, &spc, &bps, &nfc, &tnc ) ) {
        return( __set_errno_nt_reterr() );
    }
    df->total_clusters = tnc;
    df->avail_clusters = nfc;
    df->sectors_per_cluster = spc;
    df->bytes_per_sector = bps;
#elif defined(__OS2__)
    FSALLOCATE  fsinfo;
    APIRET      rc;

    rc = DosQFSInfo( dnum, FSIL_ALLOC, (PVOID)&fsinfo, sizeof( fsinfo ) );
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    df->total_clusters = fsinfo.cUnit;
    df->avail_clusters = fsinfo.cUnitAvail;
    df->sectors_per_cluster = fsinfo.cSectorUnit;
    df->bytes_per_sector = fsinfo.cbSector;
#endif
    return( 0 );
}
