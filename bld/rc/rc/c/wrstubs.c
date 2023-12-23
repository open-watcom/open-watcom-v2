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
* Description:  Stub routines for GUI resource editors (those only support
*               Win16 and Win32 resources, not OS/2 resources).
*
****************************************************************************/


#include "global.h"
#include "rccore_2.h"
#include "exelxobj.h"
#include "iortns.h"
#include "exereslx.h"
#include "exeres.h"


bool RcBuildLXResourceObjects( ExeFileInfo *dst, ResFileInfo *resfiles )
{
    /* unused parameters */ (void)dst; (void)resfiles;
    return( true );
}

bool CopyLXExeObjects( ExeFileInfo *src, ExeFileInfo *dst )
{
    /* unused parameters */ (void)src; (void)dst;
    return( true );
}

RcStatus RcWriteLXResourceObjects( ExeFileInfo *dst, ResFileInfo *res )
{
    /* unused parameters */ (void)dst; (void)res;
    return( RS_WRITE_ERROR );
}

RcStatus CopyOS2Resources( ExeFileInfo *dst, ResFileInfo *res )
{
    /* unused parameters */ (void)dst; (void)res;
    return( RS_READ_ERROR );
}

RcStatus InitOS2ResTable( ExeFileInfo *dst, ResFileInfo *res, int *err_code )
{
    /* unused parameters */ (void)dst; (void)res; (void)err_code;

    return( RS_READ_ERROR );
}

RcStatus WriteOS2ResTable( FILE *fp, OS2ResTable *restab, int *err_code )
{
    /* unused parameters */ (void)fp; (void)restab; (void)err_code;

    return( RS_READ_ERROR );
}

uint_32 ComputeOS2ResSegCount( WResDir dir )
{
    /* unused parameters */ (void)dir;

    return( 0 );
}
