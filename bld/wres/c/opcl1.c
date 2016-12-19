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
* Description:  Load string resources file open/close routines.
*
****************************************************************************/


#include <stdio.h>
#include "bool.h"
#include "watcom.h"
#include "filefmt.h"
#include "wres.h"
#include "wresset2.h"
#include "opcl.h"


bool OpenResFileX( PHANDLE_INFO instance, const char *filename, bool res_file )
/******************************************************************************/
/* return true if file is open and resources are found and initialized */
/* return false otherwise */
{
    instance->status = 0;
    instance->handle = ResOpenFileRO( filename );
    if( instance->handle == WRES_NIL_HANDLE )
        return( false );
    instance->status++;
    if( FindResourcesX( instance, res_file ) )
        return( false );
    instance->status++;
    if( InitResources( instance ) )
        return( false );
    instance->status++;
    res_handle = instance->handle;
    return( true );
}

bool OpenResFile( PHANDLE_INFO instance, const char *filename )
/**************************************************************/
/* return true if file is open and resources are found and initialized */
/* return false otherwise */
{
    return( OpenResFileX( instance, filename, false ) );
}

bool CloseResFile( PHANDLE_INFO instance )
/*****************************************/
/* return true if file is succesfully closed
 * and return false otherwise
 */
{
    bool    rc;

    rc = true;
    switch( instance->status ) {
    default:
        FiniResources( instance );
        /* fall throught */
    case 1:
        rc = !ResCloseFile( instance->handle );
        instance->handle = WRES_NIL_HANDLE;
        instance->status = 0;
        res_handle = WRES_NIL_HANDLE;
        /* fall throught */
    case 0:
        return( rc );
    }
}
