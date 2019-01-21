/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of _getdiskfree().
*
****************************************************************************/


#include "variety.h"
#include <direct.h>
#if defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#elif defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #define INCL_DOS
    #include <wos2.h>
#else
    #include <dos.h>
#endif
#include "seterrno.h"


_WCRTLINK unsigned _getdiskfree( unsigned dnum, struct diskfree_t *df )
{
#if defined( __RDOS__ ) || defined( __RDOSDEV__ )
    unsigned    ok;
    long        free_units;
    int         bytes_per_unit;
    long        total_units;
    int         disc;
    long        start_sector;
    long        drive_total_sectors;
    long long   total_sectors;
    int         sector_size;
    int         bios_sectors_per_cyl;
    int         bios_heads;

    ok = RdosGetDriveInfo( dnum, &free_units, &bytes_per_unit, &total_units );
    if( ok ) {
        ok = RdosGetDriveDiscParam( dnum, &disc, &start_sector, &drive_total_sectors );
    }
    if( ok ) {
        ok = RdosGetDiscInfo( disc, &sector_size, &total_sectors, &bios_sectors_per_cyl, &bios_heads );
    }
    if( !ok ) {
        return( -1 );
    }
    df->total_clusters = total_units;
    df->avail_clusters = free_units;
    df->sectors_per_cluster = bytes_per_unit;
    df->bytes_per_sector = sector_size;
    return( 0 );
#elif defined(__NT__)
    DWORD   spc;
    DWORD   bps;
    DWORD   nfc;
    DWORD   tnc;
    char    path[4];
    char    *pname;

    if( dnum != 0 ) {
        path[0] = dnum + 'A' - 1;
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
    return( 0 );
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
    return( 0 );
#else
    return( _dos_getdiskfree( dnum, df ) );
#endif
}
