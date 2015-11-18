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
* Description:  Spawn external commands (DOS host).
*
****************************************************************************/


#include "vi.h"
#include <malloc.h>
#include <dos.h>
#include "wio.h"
#include "fcbmem.h"
#if defined( _M_I86 )
    #include "pragmas.h"
    #include "tinyio.h"
#endif
#include "doschk.h"

void ResetSpawnScreen( void )
{
}

#if defined( _M_I86 )

typedef struct {
    unsigned short      envp;
    void                __far *cmdline;
    void                __far *fcb1;
    void                __far *fcb2;
} exec_block;

typedef char _fcb[16];

long MySpawn( const char *cmd )
{
    bool        cp;
    long        rc;
    exec_block  exeparm;
    _fcb        fcb1, fcb2;
    cmd_struct  cmds;
    char        path[_MAX_PATH], f_buff[_MAX_PATH];
    int         i;
    where_parm  where;
    long        minMemoryLeft;
    int         chkSwapSize;
#if defined( USE_XMS ) || defined( USE_EMS )
    long        *xHandle;
    unsigned short *xSize;
#endif


    minMemoryLeft = MaxMemFree & ~((long)MAX_IO_BUFFER - 1);
    chkSwapSize = 1 + (unsigned short)
        (((minMemoryLeft + ((long)MAX_IO_BUFFER - 1)) & ~((long)MAX_IO_BUFFER - 1)) /
         (long)MAX_IO_BUFFER);

    /*
     * set up checkpoint file stuff:
     */
#if defined( USE_EMS )
    if( !EMSBlockTest( chkSwapSize ) ) {
        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            EMSGetBlock( &xHandle[i] );
        }
        XSwapInit( chkSwapSize, xHandle, xSize );
        where = IN_EMS;
        goto evil_goto;
    }
#endif
#if defined( USE_XMS )
    if( !XMSBlockTest( chkSwapSize ) ) {
        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            XMSGetBlock( &xHandle[i] );
        }
        XSwapInit( chkSwapSize, xHandle, xSize );
        where = IN_XMS;
        goto evil_goto;
    }
#endif
    where = ON_DISK;

    /*
     * build command line
     */
evil_goto:
    GetSpawnCommandLine( path, cmd, &cmds );

    /*
     * set up parm block
     */
    exeparm.envp = 0;
    exeparm.cmdline = &cmds;
    exeparm.fcb1 = fcb1;
    exeparm.fcb2 = fcb2;
    GetFcb( &cmds.cmd, &fcb1 );
    GetFcb( &cmds.cmd, &fcb2 );

    /*
     * spawn the command
     */
    cp = CheckPointMem( where, minMemoryLeft / 16, f_buff );
    rc = DoSpawn( path, &exeparm );
    if( cp ) {
        CheckPointRestore( where );
    }
    return( rc );
}

#else

long MySpawn( const char *cmd )
{
    return( system( cmd ) );
}

#endif
