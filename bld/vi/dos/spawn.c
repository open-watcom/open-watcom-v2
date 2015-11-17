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
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include "wio.h"
#include "fcbmem.h"
#if defined( _M_I86 )
    #include "pragmas.h"
    #include "tinyio.h"
    #include "doschk.h"
#endif

void ResetSpawnScreen( void )
{
}

#if defined( _M_I86 )

#define SWAP_FILE_NAME      "SWXXXXXX"

#define MEMORY_BLOCK        'M'
#define END_OF_CHAIN        'Z'
#define PTR(curr)           ((void __based( curr ) *)0)
#define MCB_PTR(curr)       ((dos_mem_block __based( curr ) *)0)
#define NEXT_MCB( curr )    (curr + MCB_PTR( curr )->size + 1)

typedef struct {
    unsigned short      envp;
    void                __far *cmdline;
    void                __far *fcb1;
    void                __far *fcb2;
} exec_block;

typedef char _fcb[16];

/* Mis-aligned struct! */
#include "pushpck1.h"
typedef struct {
    char            chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned short  owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned short  size;   /* in paragraphs, not including header  */
} dos_mem_block;
#include "poppck.h"

static char             *fullName = NULL;
static int              fileHandle = -1;

static dos_mem_block    saveMem;
static __segment        savePtrMem;
static __segment        savePtrChk;

static where            isWhere;

static void cleanUp( void )
{
    switch( isWhere ) {
    case ON_DISK:
        TinyClose( fileHandle );
        fileHandle = -1;
        TinyDelete( fullName );
        break;
#if defined( USE_XMS )
    case IN_XMS:
        XmemGiveBack( &GiveBackXMSBlock );
        break;
#endif
#if defined( USE_EMS )
    case IN_EMS:
        XmemGiveBack( &GiveBackEMSBlock );
        break;
#endif
    }

} /* cleanUp */

/*
 * chkWrite - write checkpoint data
 */
static bool chkWrite( __segment buff, unsigned *size )
{
    unsigned    bytes;

    switch( isWhere ) {
    case ON_DISK:
        if( *size >= 0x1000 ) {
            *size = 0x0800;
        }
        bytes = *size << 4;
        if( TinyFarWrite( fileHandle, PTR( buff ), bytes) != bytes ) {
            return( false );
        }
        return( true );
#if defined( USE_EMS )
    case IN_EMS:
        XmemBlockWrite( &EMSBlockWrite, buff, size );
        return( true );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        XmemBlockWrite( &XMSBlockWrite, buff, size );
        return( true );
#endif
    }
    return( false );

} /* chkWrite */

/*
 * chkRead - read checkpoint "file"
 */
static bool chkRead( __segment *buff )
{
    switch( isWhere ) {
    case ON_DISK:
        if( TinyFarRead( fileHandle, MK_FP( *buff, 0 ), 0x8000 ) == 0x8000 ) {
            *buff += 0x800;
            return( true );
        }
        return( false );
#if defined( USE_EMS )
    case IN_EMS:
        return( XmemBlockRead( EMSBlockRead, buff ) );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        return( XmemBlockRead( XMSBlockRead, buff ) );
#endif
    }
    return( false );

} /* chkRead */

/*
 * chkOpen - (re)open a checkpoint file
 */
static bool chkOpen( char *fname_buff )
{
    tiny_ret_t  ret;

    switch( isWhere ) {
    case ON_DISK:
        if( fname_buff != NULL ) {
            MakeTmpPath( fname_buff, SWAP_FILE_NAME );
            fileHandle = mkstemp( fname_buff );
            if( fileHandle == -1 ) {
                fullName = NULL;
                return( 0 );
            }
            fullName = fname_buff;
        } else {
            ret = TinyOpen( fullName, TIO_READ );
            if( TINY_ERROR( ret ) ) {
                fileHandle = -1;
                return( false );
            }
            fileHandle = TINY_INFO( ret );
        }
        break;
#if defined( USE_EMS )
    case IN_EMS:
        Xopen();
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        Xopen();
        break;
#endif
    }
    return( true );

} /* chkOpen */

/*
 * chkClose
 */
static void chkClose( void )
{
    switch( isWhere ) {
    case ON_DISK:
        TinyClose( fileHandle );
        fullName = NULL;
        break;
    }

} /* chkClose */

/*
 * checkPointMem - checkpoint up to max bytes of memory
 */
static bool checkPointMem( unsigned max, char *fname_buff )
{
    __segment   mem, start, end, next, chk;
    unsigned    size, psp;

    if( max == 0 ) {
        return( false );
    }
    psp = TinyGetPSP();
    start = psp - 1;
    if( MCB_PTR( start )->chain == END_OF_CHAIN ) {
        return( false );
    }
    start = NEXT_MCB( start );
    mem = start;
    for( ;; ) {
        if( MCB_PTR( mem )->owner == 0 && MCB_PTR( mem )->size >= max ) {
            return( false );
        }
        if( MCB_PTR( mem )->chain == END_OF_CHAIN ) {
            break;
        }
        mem = NEXT_MCB( mem );
    }
    end = NEXT_MCB( mem );
    size = end - start;
    if( size < 0x100 ) {
        return( false );
    }

    if( size > max ) {
        size = max;
    }
    chk = end - size - 1;
    mem = start;
    for( ;; ) {
        next = NEXT_MCB( mem );
        if( next > chk ) {
            break;
        }
        mem = next;
    }

    savePtrMem = mem;
    _fmemcpy( &saveMem, PTR( mem ), sizeof( dos_mem_block ) );
    savePtrChk = chk;

    if( !chkOpen( fname_buff ) ) {
        cleanUp();
        return( false );
    }
    next = chk;
    while( next < end ) {
        size = end - next;
        if( !chkWrite( next, &size ) ) {
            cleanUp();
            return( false );
        }
        next = next + size;
    }
    chkClose();
    MCB_PTR( mem )->chain = MEMORY_BLOCK;
    MCB_PTR( mem )->size = chk - mem - 1;
    MCB_PTR( chk )->size = end - chk - 1;
    MCB_PTR( chk )->chain = END_OF_CHAIN;
    MCB_PTR( chk )->owner = 0;
    return( true );
}

static void checkPointRestore( void )
{
    __segment   chk;

    if( !chkOpen( NULL ) ) {
        return;
    }

    _fmemcpy( PTR( savePtrMem ), &saveMem, sizeof( dos_mem_block ) );

    chk = savePtrChk;
    while( chkRead( &chk ) )
        ;
    cleanUp();
}

long MySpawn( const char *cmd )
{
    bool        cp;
    long        rc;
    exec_block  exeparm;
    _fcb        fcb1, fcb2;
    cmd_struct  cmds;
    char        path[_MAX_PATH], fname_buff[_MAX_PATH];
    int         i;
    int         chkSwapSize;
    long        minMemoryLeft;

    minMemoryLeft = MaxMemFree & ~((long)MAX_IO_BUFFER - 1);
    chkSwapSize = 1 + (unsigned short)
        (((minMemoryLeft + ((long)MAX_IO_BUFFER - 1)) & ~((long)MAX_IO_BUFFER - 1)) /
         (long)MAX_IO_BUFFER);

    /*
     * set up checkpoint file stuff:
     */
#if defined( USE_EMS )
    if( !EMSBlockTest( chkSwapSize ) ) {
        unsigned short   *xSize;
        long             *xHandle;

        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            EMSGetBlock( &xHandle[i] );
        }
        XSwapInit( chkSwapSize, xHandle, xSize );
        isWhere = IN_EMS;
        goto evil_goto;
    }
#endif
#if defined( USE_XMS )
    if( !XMSBlockTest( chkSwapSize ) ) {
        unsigned short   *xSize;
        long             *xHandle;

        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            XMSGetBlock( &xHandle[i] );
        }
        XSwapInit( chkSwapSize, xHandle, xSize );
        isWhere = IN_XMS;
        goto evil_goto;
    }
#endif
    isWhere = ON_DISK;

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
    cp = checkPointMem( minMemoryLeft / 16, fname_buff );
    rc = DoSpawn( path, &exeparm );
    if( cp ) {
        checkPointRestore();
    }

    return( rc );
}

#else

long MySpawn( const char *cmd )
{
    return( system( cmd ) );
}

#endif
