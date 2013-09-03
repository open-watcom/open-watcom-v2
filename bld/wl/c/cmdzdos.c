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

/*
 *  CMDZDOS : command line parsing for the ZDOS load file format.
 *
*/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "dbgall.h"
#include "cmdall.h"
#include "overlays.h"
#include "objcalc.h"
#include "cmdline.h"
#include "cmdzdos.h"

extern bool ProcZdos( void )
/*************************/
{
    FmtData.base = 0x1000;                      // assume user application
    LinkState |= MAKE_RELOCS | FMT_DECIDED;     // make relocations;
    ProcOne( ZdosOptions, SEP_NO, FALSE );
    return( TRUE );
}

extern bool ProcZdosSYS( void )
/*************************/
{
    FmtData.base = 0;                           // it's a driver, so reset base
    Extension = E_SYS;
    return( TRUE );
}

extern bool ProcZdosHWD( void )
/*************************/
{
    FmtData.base = 0;                           // it's a driver, so reset base
    Extension = E_HWD;
    return( TRUE );
}

extern bool ProcZdosFSD( void )
/*************************/
{
    FmtData.base = 0;                           // it's a driver, so reset base
    Extension = E_FSD;
    return( TRUE );
}
