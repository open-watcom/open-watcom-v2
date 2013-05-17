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


#include "vi.h"
#include <malloc.h>
#include <dos.h>
#include "fcbmem.h"

void ResetSpawnScreen( void )
{
}

#if defined( _M_I86 )
static long minMemoryLeft;
static int  chkSwapSize;

#include "tinyio.h"
static char *fullName;

typedef enum {
    IN_EMS,
    IN_XMS,
    ON_DISK
} where;

typedef struct {
    unsigned short      envp;
    void                far *cmdline;
    void                far *fcb1;
    void                far *fcb2;
} exec_block;

typedef char _fcb[16];

typedef struct {
    char        chain;  /* 'M' memory block, 'Z' is last in chain */
    unsigned    owner;  /* 0x0000 ==> free, otherwise psp address */
    unsigned    size;   /* in paragraphs, not including header  */
} dos_mem_block;

#define MEMORY_BLOCK 'M'
#define END_OF_CHAIN 'Z'
#define NEXT_BLOCK( curr )  MK_FP( (FP_SEG( curr ) + (curr)->size + 1), 0 )
#define PUT_ITEM( item ) (TinyWrite( hdl, &(item), sizeof( item ) ) == sizeof( item ))

static dos_mem_block saveMem;
static dos_mem_block *savePtrMem;
static dos_mem_block *savePtrChk;

static unsigned long    fileHandle;
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

static void memBlockWrite( void (*rtn)( long, void*, unsigned ),
                           char *buff, unsigned *size )
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

static int memBlockRead( void (*rtn)( long, void*, unsigned ), void **buff )
{
    rtn( xHandle[currMem], *buff, xSize[currMem] );
    *buff = MK_FP( FP_SEG( *buff ) + 0x200, 0 );
    if( xSize[currMem] < MAX_IO_BUFFER ) {
        return( FALSE );
    }
    currMem++;
    return( TRUE );

} /* memBlockRead */

static void cleanUp( void )
{
    switch( isWhere ) {
    case ON_DISK:
        TinyClose( fileHandle );
        TinyDelete( fullName );
        break;
#ifndef NOXMS
    case IN_XMS:
        memGiveBack( &GiveBackXMSBlock );
        break;
#endif
#ifndef NOEMS
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
            return( FALSE );
        }
        return( TRUE );
#ifndef NOEMS
    case IN_EMS:
        memBlockWrite( &EMSBlockWrite, buff, size );
        return( TRUE );
#endif
#ifndef NOXMS
    case IN_XMS:
        memBlockWrite( &XMSBlockWrite, buff, size );
        return( TRUE );
#endif
    }
    return( FALSE ); // to quiet the compiler

} /* chkWrite */

/*
 * chkRead - read checkpoint "file"
 */
static bool chkRead( void *buf )
{
    void    **buff = buf;

    switch( isWhere ) {
    case ON_DISK:
        if( TinyRead( fileHandle, *buff, 0x8000 ) == 0x8000 ) {
            *buff = MK_FP( FP_SEG( *buff ) + 0x800, 0 );
            return( TRUE );
        }
        return( FALSE );
#ifndef NOEMS
    case IN_EMS:
        return( memBlockRead( EMSBlockRead, buff ) );
#endif
#ifndef NOXMS
    case IN_XMS:
        return( memBlockRead( XMSBlockRead, buff ) );
#endif
    }
    return( FALSE ); // to quiet the compiler

} /* chkRead */

/*
 * chkOpen - re-open a checkpoint file
 */
static bool chkOpen( void )
{
    tiny_ret_t  ret;

    switch( isWhere ) {
    case ON_DISK:
        ret = TinyOpen( fullName, TIO_READ );
        if( ret < 0 ) {
            return( FALSE );
        }
        fileHandle = ret;
        break;
#ifndef NOEMS
    case IN_EMS:
        currMem = 0;
        break;
#endif
#ifndef NOXMS
    case IN_XMS:
        currMem = 0;
        break;
#endif
    }
    return( TRUE );

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
        return( FALSE );
    }
    psp = TinyGetPSP();
    start = MK_FP( psp - 1, 0 );
    if( start->chain == END_OF_CHAIN ) {
        return( FALSE );
    }
    start = NEXT_BLOCK( start );
    mem = start;
    for( ;; ) {
        if( mem->owner == 0 && mem->size >= max ) {
            return( FALSE );
        }
        if( mem->chain == END_OF_CHAIN ) {
            break;
        }
        mem = NEXT_BLOCK( mem );
    }
    end = NEXT_BLOCK( mem );
    size = FP_SEG( end ) - FP_SEG( start );
    if( size < 0x100 ) {
        return( FALSE );
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

    next = chk;
    while( FP_SEG( next ) < FP_SEG( end ) ) {
        size = FP_SEG( end ) - FP_SEG( next );
        if( !chkWrite( next, &size ) ) {
            cleanUp();
            return( FALSE );
        }
        next = MK_FP( FP_SEG( next ) + size, 0 );
    }
    chkClose();
    mem->chain = MEMORY_BLOCK;
    mem->size = FP_SEG( chk ) - FP_SEG( mem ) - 1;
    chk->size = FP_SEG( end ) - FP_SEG( chk ) - 1;
    chk->chain = END_OF_CHAIN;
    chk->owner = 0;
    return( TRUE );
}

static void CheckPointRestore( void )
{
    dos_mem_block       *chk;

    if( !chkOpen() ) {
        return;
    }

    chk = savePtrMem;
    memcpy( chk, &saveMem, sizeof( dos_mem_block ) );
    chk = savePtrChk;

    while( chkRead( &chk ) );
    cleanUp();
}

extern int DoSpawn( void *, void * );
#pragma aux DoSpawn = \
        "push   ds" \
        "push   es" \
        "push   si" \
        "push   di" \
        "mov    ds, dx"  /*  exe segment */ \
        "mov    dx, ax"  /*  exe offset */ \
        "mov    es, cx"  /*  parm block segment (offset in bx already) */ \
        "mov    ax, 4b00h"  /*  exec process */ \
        "int    21h" \
        "jc     rcisright" \
        "mov    ax, 4d00h" \
        "int    21h" \
        "mov    ah, 0" \
        "rcisright:" \
        "pop    di" \
        "pop    si" \
        "pop    es" \
        "pop    ds" \
    parm [dx ax] [cx bx] value [ax];

extern int GetFcb( void *, void * );
#pragma aux GetFcb = \
        "push   ds" \
        "push   es" \
        "push   si" \
        "push   di" \
        "mov    ds, dx" /*  exe segment */ \
        "mov    si, ax" /*  exe offset */ \
        "mov    es, cx" /*  parm block segment (offset in bx already) */ \
        "mov    di, bx" \
        "mov    ax, 2901h" /*  parse filename/get fcb */ \
        "int    21h" \
        "pop    di" \
        "pop    si" \
        "pop    es" \
        "pop    ds" \
    parm [dx ax] [cx bx] value [ax];

long MySpawn( char *cmd )
{
    bool        cp;
    long        rc;
    exec_block  exeparm;
    _fcb        fcb1, fcb2;
    cmd_struct  cmds;
    char        path[_MAX_PATH], file[_MAX_PATH];
    tiny_ret_t  ret;
    int         i;
    char        chkfname[L_tmpnam];

    minMemoryLeft = MaxMemFree & ~((long)MAX_IO_BUFFER - 1);
    chkSwapSize = 1 + (unsigned short)
        (((minMemoryLeft + ((long)MAX_IO_BUFFER - 1)) & ~((long)MAX_IO_BUFFER - 1)) /
         (long)MAX_IO_BUFFER);

    /*
     * set up checkpoint file stuff:
     */
#ifndef NOEMS
    if( !EMSBlockTest( chkSwapSize ) ) {
        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            EMSGetBlock( &xHandle[i] );
        }
        isWhere = IN_EMS;
        currMem = 0;
        goto evil_goto;
    }
#endif
#ifndef NOXMS
    if( !XMSBlockTest( chkSwapSize ) ) {
        xHandle = alloca( chkSwapSize * sizeof( long ) );
        xSize = alloca( chkSwapSize * sizeof( short ) );
        for( i = 0; i < chkSwapSize; i++ ) {
            XMSGetBlock( &xHandle[i] );
        }
        isWhere = IN_XMS;
        currMem = 0;
        goto evil_goto;
    }
#endif
    file[0] = 0;
    tmpnam( chkfname );
    StrMerge( 3, file, EditVars.TmpDir, FILE_SEP_STR, chkfname );
    fullName = file;
    ret = TinyCreate( fullName, TIO_NORMAL );
    if( ret < 0 ) {
        return( 0 );
    }
    fileHandle = ret;
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
        CheckPointRestore();
    }

    return( rc );
}
#else
long MySpawn( char *cmd )
{
    return( system( cmd ) );
}
#endif
