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


#include <stdio.h>
#include "wresall.h"
#include "global.h"
#include "cmpres.h"

static int CompareHeaders( WResFileID handle1, WResFileID handle2 )
/*****************************************************************/
{
    int             error;
    int             retcode;
    WResHeader      header1;
    WResHeader      header2;

    error = WResReadHeaderRecord( &header1, handle1 );
    if (error) {
        return( -1 );
    }
    error = WResReadHeaderRecord( &header2, handle2 );
    if (error) {
        return( -1 );
    }

    retcode = 0;

    if (header1.NumResources != header2.NumResources) {
        if (!(CmdLineParms.Quiet || CmdLineParms.NoCounts)) {
            puts( "The number of resources differ." );
        }
        retcode = 1;
    }

    if (header1.NumTypes != header2.NumTypes) {
        if (!(CmdLineParms.Quiet || CmdLineParms.NoCounts)) {
            puts( "The number of types differ." );
        }
        retcode = 1;
    }

    return( retcode );
}

int CompareContents( WResFileID handle1, WResFileID handle2 )
/***********************************************************/
{
    int             retcode;        /* -1: error  0: same  1: different */
    int             oldretcode;
    int             dup_discarded;
    int             error;
    WResDir         dir1;
    WResDir         dir2;

    retcode = CompareHeaders( handle1, handle2 );
    if ((retcode == -1) || (retcode == 1 && !CmdLineParms.CheckAll)) {
        return( retcode );
    }

    oldretcode = retcode;

    dir1 = WResInitDir();
    if (dir1 == NULL) {
        return( -1 );
    }
    dir2 = WResInitDir();
    if (dir2 == NULL) {
        WResFreeDir( dir1 );
        return( -1 );
    }

    error = WResReadDir( handle1, dir1, &dup_discarded );
    if (error || dup_discarded) {
        WResFreeDir( dir1 );
        WResFreeDir( dir2 );
        return( -1 );
    }
    error = WResReadDir( handle2, dir2, &dup_discarded );
    if (error || dup_discarded) {
        WResFreeDir( dir1 );
        WResFreeDir( dir2 );
        return( -1 );
    }

    retcode = CompareResources( handle1, dir1, handle2, dir2 );

    WResFreeDir( dir1 );
    WResFreeDir( dir2 );

    if (retcode == -1) {
        return( -1 );
    } else if (retcode == 1 || oldretcode == 1) {
        return( 1 );
    } else {
        return( 0 );
    }
}

