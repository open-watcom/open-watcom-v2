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


#include <string.h>
#include <stdlib.h>
#include <i86.h>
#include <windows.h>
#include "watcom.h"
#include "mad.h"
#include "madimp.h"
#include "madcli.h"

HMODULE MADLastHandle;  /* for Dr. WATCOM */

typedef void (DIGENTRY INTER_FUNC)();

#ifdef DEBUGGING
void Say( char *buff )
{
    MessageBox( (HWND) NULL, buff, "DEBUG",
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

mad_status MADSysLoad( char *path, mad_client_routines *cli,
                                mad_imp_routines **imp, unsigned long *sys_hdl )
{
    HANDLE              dll;
    mad_imp_routines    *(DIGENTRY *init_func)( mad_status *, mad_client_routines * );
    char                newpath[256];
    mad_status          status;
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
    struct {
        INTER_FUNC      *load;
        INTER_FUNC      *unload;
    }                   transfer_block;
    char                *p;
    UINT                prev;

    strcpy( newpath, path );
    strcat( newpath, ".dll" );
    *sys_hdl = 0;
    p = parm;
    *p++ = ' ';
    utoa( FP_SEG( &transfer_block ), p, 16 );
    p = &parm[ strlen( parm ) ];
    *p++ = ' ';
    utoa( FP_OFF( &transfer_block ), p, 16 );
    transfer_block.load = NULL;
    transfer_block.unload = NULL;
    show_block.mb2 = 2;
    show_block.show = SW_HIDE;
    parm_block.env = 0;
    parm_block.cmdline = parm;
    parm_block.show = &show_block;
    parm_block.reserved = 0;
    prev = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    dll = LoadModule( newpath, &parm_block );
    MADLastHandle = dll;
    SetErrorMode( prev );
    if( dll < 32 ) {
        return( MS_ERR|MS_FOPEN_FAILED );
    }
    *sys_hdl = (unsigned long)transfer_block.unload;
    init_func = transfer_block.load;
    if( init_func == NULL ) {
        MADSysUnload( *sys_hdl );
        return( MS_ERR|MS_INVALID_MAD );
    }
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
        MADSysUnload( *sys_hdl );
        return( status );
    }
    return( MS_OK );
}

void MADSysUnload( unsigned long sys_hdl )
{
    void        (DIGENTRY *fini_func)() = (void *)sys_hdl;

    if( fini_func != NULL ) {
        fini_func();
    }
}
