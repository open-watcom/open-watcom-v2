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

#define SPAWN_FILE_NAME    "swXXXXXX"

void ResetSpawnScreen( void )
{
}

#if defined( _M_I86 )

static long minMemoryLeft;
static int  chkSwapSize;

static char *fullName;

typedef enum {
    IN_EMS,
    IN_XMS,
    ON_DISK
} where;

typedef struct {
    unsigned short      envp;
    void                __far *cmdline;
    void                __far *fcb1;
    void                __far *fcb2;
} exec_block;

typedef char _fcb[16];

/* Mis-aligned struct! */
#pragma pack( push, 1 )
typedef struct {
    char        chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned    owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned    size;   /* in paragraphs, not including header  */
} dos_mem_block;
#pragma pack( pop )

#define MEMORY_BLOCK 'M'
#define END_OF_CHAIN 'Z'
#define NEXT_BLOCK( curr )  MK_FP( (FP_SEG( curr ) + (curr)->size + 1), 0 )

static dos_mem_block saveMem;
static dos_mem_block *savePtrMem;
static dos_mem_block *savePtrChk;

static int              fileHandle;
static unsigned short   *xSize;
static long             *xHandle;
static unsigned short   currMem;
static where            isWhere;

static void memGiveBack( void (*rtn)( long ) )
{
    int i;

    for( i = 0; i < chkSwapSize; i++ ) {
        rtn( xHandle[i] );
    }

} /* memGiveBack */

static void memBlockWrite( void (*rtn)(long, void*, unsigned), char *buff, unsigned *size )
{
    unsigned    bytes;

    if( *size >= 0x0200 ) {
        *size = 0x0200;
    }
    bytes = *size << 4;
    rtn( xHandle[currMem], buff, bytes );
    xSize[currMem] = bytes;
    currMem++;

} /* memBlockWrite */

static bool memBlockRead( void (*rtn)(long, void*, unsigned), void **buff )
{
    rtn( xHandle[currMem], *buff, xSize[currMem] );
    *buff = MK_FP( FP_SEG( *buff ) + 0x200, 0 );
    if( xSize[currMem] < MAX_IO_BUFFER ) {
        return( false );
    }
    currMem++;
    return( true );

} /* memBlockRead */

#if defined( USE_EMS )
int EMSInit( int count )
{
    if( !EMSBlockTest( count ) ) {
        xHandle = alloca( count * sizeof( long ) );
        xSize = alloca( count * sizeof( short ) );
        for( i = 0; i < count; i++ ) {
            EMSGetBlock( &xHandle[i] );
        }
        return( 1 );
    }
    return( 0 );
}
#endif

#if defined( USE_XMS )
int XMSInit( int count )
{
    if( !XMSBlockTest( count ) ) {
        xHandle = alloca( count * sizeof( long ) );
        xSize = alloca( count * sizeof( short ) );
        for( i = 0; i < count; i++ ) {
            XMSGetBlock( &xHandle[i] );
        }
        return( 1 );
    }
    return( 0 );
}
#endif

static void cleanUp( void )
{
    switch( isWhere ) {
    case ON_DISK:
        TinyClose( fileHandle );
        TinyDelete( fullName );
        break;
#if defined( USE_XMS )
    case IN_XMS:
        memGiveBack( &GiveBackXMSBlock );
        break;
#endif
#if defined( USE_EMS )
    case IN_EMS:
        memGiveBack( &GiveBackEMSBlock );
        break;
#endif
    }

} /* cleanUp */

/*
 * chkWrite - write checkpoint data
 */
static bool chkWrite( void *buff, unsigned *size )
{
    unsigned    bytes;

    switch( isWhere ) {
    case ON_DISK:
        if( *size >= 0x1000 ) {
            *size = 0x0800;
        }
        bytes = *size << 4;
        if( TinyWrite( fileHandle, buff, bytes) != bytes ) {
            return( false );
        }
        return( true );
#if defined( USE_EMS )
    case IN_EMS:
        memBlockWrite( &EMSBlockWrite, buff, size );
        return( true );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        memBlockWrite( &XMSBlockWrite, buff, size );
        return( true );
#endif
    }
    return( false );

} /* chkWrite */

/*
 * chkRead - read checkpoint "file"
 */
static bool chkRead( void **buff )
{
    switch( isWhere ) {
    case ON_DISK:
        if( TinyRead( fileHandle, *buff, 0x8000 ) == 0x8000 ) {
            *buff = MK_FP( FP_SEG( *buff ) + 0x800, 0 );
            return( true );
        }
        return( false );
#if defined( USE_EMS )
    case IN_EMS:
        return( memBlockRead( EMSBlockRead, buff ) );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        return( memBlockRead( XMSBlockRead, buff ) );
#endif
    }
    return( false );

} /* chkRead */

/*
 * chkOpen - re-open a checkpoint file
 */
static bool chkOpen( open_attr attr )
{
    tiny_ret_t  ret;

    switch( isWhere ) {
    case ON_DISK:
        ret = TinyOpen( fullName, attr );
        if( TINY_ERROR( ret ) ) {
            return( false );
        }
        fileHandle = TINY_INFO( ret );
        break;
#if defined( USE_EMS )
    case IN_EMS:
        currMem = 0;
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        currMem = 0;
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
        break;
    }

} /* chkClose */

/*
 * checkPointMem - checkpoint up to max bytes of memory
 */
static bool checkPointMem( unsigned max )
{
    dos_mem_block       *mem, *start, *end, *next, *chk;
    unsigned            size, psp;

    if( max == 0 ) {
        return( false );
    }
#if defined( USE_EMS )
    if( EMSInit( chkSwapSize ) ) {
        isWhere = IN_EMS;
        goto evil_goto;
    }
#endif
#if defined( USE_XMS )
    if( XMSInit( chkSwapSize ) ) {
        isWhere = IN_XMS;
        goto evil_goto;
    }
#endif
    psp = TinyGetPSP();
    start = MK_FP( psp - 1, 0 );
    if( start->chain == END_OF_CHAIN ) {
        return( false );
    }
    start = NEXT_BLOCK( start );
    mem = start;
    for( ;; ) {
        if( mem->owner == 0 && mem->size >= max ) {
            return( false );
        }
        if( mem->chain == END_OF_CHAIN ) {
            break;
        }
        mem = NEXT_BLOCK( mem );
    }
    end = NEXT_BLOCK( mem );
    size = FP_SEG( end ) - FP_SEG( start );
    if( size < 0x100 ) {
        return( false );
    }

    if( size > max ) {
        size = max;
    }
    chk = MK_FP( FP_SEG( end ) - size - 1, 0 );
    mem = start;
    for( ;; ) {
        next = NEXT_BLOCK( mem );
        if( FP_SEG( next ) > FP_SEG( chk ) ) {
            break;
        }
        mem = next;
    }

    savePtrMem = mem;
    memcpy( &saveMem, mem, sizeof( dos_mem_block ) );
    savePtrChk = chk;

    if( !chkOpen( TIO_WRITE ) ) {
        cleanUp();
        return( false );
    }
    next = chk;
    while( FP_SEG( next ) < FP_SEG( end ) ) {
        size = FP_SEG( end ) - FP_SEG( next );
        if( !chkWrite( next, &size ) ) {
            cleanUp();
            return( false );
        }
        next = MK_FP( FP_SEG( next ) + size, 0 );
    }
    chkClose();
    mem->chain = MEMORY_BLOCK;
    mem->size = FP_SEG( chk ) - FP_SEG( mem ) - 1;
    chk->size = FP_SEG( end ) - FP_SEG( chk ) - 1;
    chk->chain = END_OF_CHAIN;
    chk->owner = 0;
    return( true );
}

static void checkPointRestore( void )
{
    dos_mem_block       *chk;

    if( !chkOpen( TIO_READ ) ) {
        return;
    }

    chk = savePtrMem;
    memcpy( chk, &saveMem, sizeof( dos_mem_block ) );
    chk = savePtrChk;

    while( chkRead( (void **)&chk ) );
    cleanUp();
}

long MySpawn( const char *cmd )
{
    bool        cp;
    long        rc;
    exec_block  exeparm;
    _fcb        fcb1, fcb2;
    cmd_struct  cmds;
    char        path[_MAX_PATH], file[_MAX_PATH];
    int         i;

    minMemoryLeft = MaxMemFree & ~((long)MAX_IO_BUFFER - 1);
    chkSwapSize = 1 + (unsigned short)
        (((minMemoryLeft + ((long)MAX_IO_BUFFER - 1)) & ~((long)MAX_IO_BUFFER - 1)) /
         (long)MAX_IO_BUFFER);

    /*
     * set up checkpoint file stuff:
     */
    MakeTmpPath( file, SPAWN_FILE_NAME );
    fileHandle = mkstemp( file );
    if( fileHandle == -1 ) {
        return( 0 );
    }
    close( fileHandle );
    fileHandle = -1;
    fullName = file;
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
    cp = checkPointMem( minMemoryLeft / 16 );
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
