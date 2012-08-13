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
#include "types.h"
#include "cmpcont.h"
#include "cmpfiles.h"

static int OpenFiles( WResFileID * fileid1, WResFileID * fileid2 )
/****************************************************************/
/* opens the files named in CmdLineParms and puts their file ids in */
/* fileid1 and fileid2 */
{
    int         error;

    *fileid1 = ResOpenFileRO( CmdLineParms.FileName1 );
    if (*fileid1 == -1) {
        printf( "Unable to open %s\n", CmdLineParms.FileName1 );
        *fileid2 = -1;
        return( true );
    }

    *fileid2 = ResOpenFileRO( CmdLineParms.FileName2 );
    if (*fileid2 == -1) {
        printf( "Unable to open %s\n", CmdLineParms.FileName2 );
        ResCloseFile( *fileid1 );
        *fileid1 = -1;
        return( true );
    }

    error = (!WResIsWResFile( *fileid1 ));
    if (error) {
        printf( "File %s is not a Open Watcom .RES file\n",
                    CmdLineParms.FileName1 );
    } else {
        error = (!WResIsWResFile( *fileid2 ));
        if (error) {
            printf( "File %s is not a Open Watcom .RES file\n",
                    CmdLineParms.FileName2 );
        }
    }

    return( error );
}

static void CloseFiles( WResFileID fileid1, WResFileID fileid2 )
/**************************************************************/
{
    if (fileid1 != -1) {
        ResCloseFile( fileid1 );
    }
    if (fileid2 != -1) {
        ResCloseFile( fileid2 );
    }
}

int CompareFiles( void )
/**********************/
{
    int         error;
    int         result;
    WResFileID  fileid1;
    WResFileID  fileid2;

    error = OpenFiles( &fileid1, &fileid2 );

    if (!error) {
        result = CompareContents( fileid1, fileid2 );
    } else {
        result = -1;
    }

    CloseFiles( fileid1, fileid2 );

    return( result );
}
