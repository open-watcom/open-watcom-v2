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
* Description:  Windows 3.x DIP loader.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <i86.h>
#include <windows.h>
#include "watcom.h"
#include "digld.h"
#include "dip.h"
#include "dipimp.h"
#include "dipsys.h"
#include "diplasth.h"


typedef void DIGENTRY dip_fini_func( void );

HINSTANCE   DIPLastHandle;  /* for Dr. WATCOM */

#ifdef DEBUGGING
void Say( const char *buff )
{
    MessageBox( (HWND) NULL, buff, "DEBUG", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

void DIPSysUnload( dip_sys_handle *sys_hdl )
{
    if( *sys_hdl != NULL_SYSHDL ) {
        (*sys_hdl)();
        *sys_hdl = NULL_SYSHDL;
    }
}

dip_status DIPSysLoad( const char *base_name, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    HINSTANCE           mod_hdl;
    char                filename[256];
    dip_status          ds;
    char                parm[10];
    struct {
        WORD            mb2;
        WORD            show;
    }                   show_block;
    struct {
        WORD            env;
        LPSTR           cmdline;
        LPVOID          show;
        WORD            reserved;
    }                   parm_block;
    dip_link_block      transfer_block;
    char                *p;
    UINT                prev;

    *sys_hdl = NULL_SYSHDL;
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, 0, ".dll", filename, sizeof( filename ) ) == 0 ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    p = parm;
    *p++ = ' ';
    utoa( _FP_SEG( &transfer_block ), p, 16 );
    p = &parm[ strlen( parm ) ];
    *p++ = ' ';
    utoa( _FP_OFF( &transfer_block ), p, 16 );
    transfer_block.load = NULL;
    transfer_block.unload = NULL;
    show_block.mb2 = 2;
    show_block.show = SW_HIDE;
    parm_block.env = 0;
    parm_block.cmdline = parm;
    parm_block.show = &show_block;
    parm_block.reserved = 0;
    prev = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    mod_hdl = LoadModule( filename, &parm_block );
    DIPLastHandle = mod_hdl;
    SetErrorMode( prev );
    if( mod_hdl < HINSTANCE_ERROR ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    ds = DS_ERR | DS_INVALID_DIP;
    if( transfer_block.load != NULL && (*imp = transfer_block.load( &ds, cli )) != NULL ) {
        *sys_hdl = transfer_block.unload;
        return( DS_OK );
    }
    DIPSysUnload( &transfer_block.unload );
    return( ds );
}
