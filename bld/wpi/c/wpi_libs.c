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
* Description:  WPI OS/2 DBCS routines.
*
****************************************************************************/


#define INCL_PM
#define INCL_DOSMODULEMGR
#include <os2.h>
#include "wpi.h"


HMODULE _wpi_loadlibrary( WPI_INST inst, LPSTR name )
/***************************************************/

{
#if 0
    return( WinLoadLibrary( (inst).hab, name ) );
#else
    HMODULE                     module;

    inst = inst;

    DosLoadModule( 0, 0, name, &module );

    return( module );
#endif
}

void _wpi_freelibrary( WPI_INST inst, HMODULE module )
/****************************************************/


{
#if 0
    WinDeleteLibrary( (inst).hab, module );
#else

    inst = inst;
    DosFreeModule( module );
#endif
}

WPI_PROC _wpi_loadprocedure( WPI_INST inst, HMODULE module, LPSTR proc )
/**********************************************************************/

{
#ifdef __386__
    PFN         addr;

    inst = inst;
    DosQueryProcAddr( module, 0, proc, &addr );

    return( (WPI_PROC)addr );
#else
    return( WinLoadProcedure( (inst).hab, module, proc ) );
#endif
}
