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


#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <io.h>
#include "liballoc.h"
#include "stacklow.h"
#include "rtdata.h"
#include "exitwmsg.h"
#include "fileacc.h"
#include "initfini.h"

#define MAX_CMDLINE     500

extern void                 *__InitThreadProcessing( void );
extern void                 __InitMultipleThread( void );
extern int                  main( int argc, char **argv );

extern void                 ExitThread( int,int );
extern void                 __Must_Have_Three_One_Or_Greater( void );
extern int                  _TerminateNLM( void *, void *, int );
extern int                  _SetupArgv( int (*)( int, char ** ) );
extern long                 _StartNLM( void *, void *, unsigned char *,
                                       unsigned char *, long, long,
                                       long cdecl (*)(), long, long, void **,
                                       void (*)() );
static void                 InitStackLow( void );

extern char                 *_LpPgmName;
extern char                 *_LpCmdLine;
extern int                  _edata;
extern int                  _end;
extern unsigned             __ASTACKSIZ;
extern char                 *__ASTACKPTR;

int                         __ReturnCode = 5; /* TERM_BY_UNLOAD */
int                         _argc;
char                        **_argv;

static void                 *NCSp;
static unsigned short       _saved_DS;
static char                 CommandLine[ MAX_CMDLINE ];
static int                  InitFiniLevel = 0;

#define AllocSignature      0x54524C41

long AllocRTag;

extern void *Alloc( long __numberOfBytes, long __resourceTag );
extern long SizeOfAllocBlock( void* );

extern void Free( void *__address );

extern long AllocateResourceTag( void *__NLMHandle,
                                 char *__descriptionString,
                                 long  __resourceType );

#pragma aux __ASTACKPTR "*"
#pragma aux __ASTACKSIZ "*"


extern unsigned __SP( void );
#pragma aux __SP =      \
    "mov eax, esp"      \
    value [ eax ];

extern unsigned short __DS( void );
#pragma aux __DS =      \
    "mov ax, ds"        \
    value [ ax ];

static void __NullSema4Rtn(void *p) { p = p; }

static void __NullAccessRtn( int hdl ) { hdl = hdl; }
static void __NullRtn( void ) {}

void    (*_AccessFileH)( int ) = &__NullAccessRtn;
void    (*_ReleaseFileH)( int ) = &__NullAccessRtn;
void    (*_AccessIOB)( void ) = &__NullRtn;
void    (*_ReleaseIOB)( void ) = &__NullRtn;
void    (*_AccessTDList)( void ) = &__NullRtn;
void    (*_ReleaseTDList)( void ) = &__NullRtn;

static void __FiniMultipleThread(void)
{
        _AccessFileH   = &__NullAccessRtn;
        _ReleaseFileH  = &__NullAccessRtn;
        _AccessIOB     = &__NullRtn;
        _ReleaseIOB    = &__NullRtn;
        __AccessSema4  = &__NullSema4Rtn;
        __ReleaseSema4 = &__NullSema4Rtn;
        __CloseSema4   = &__NullSema4Rtn;
} /* FiniMultipleThread() */


extern void *_NW_calloc( size_t num,size_t size )
{
    void *ptr;
    long to_alloc;

    to_alloc = num * size;
    ptr = Alloc( to_alloc, AllocRTag );
    if( ptr != NULL ) memset( ptr, 0, to_alloc );
    return( ptr );
}

void *_NW_malloc( size_t size )
{
    return( Alloc( size, AllocRTag ) );
}

void *_NW_realloc( void *old, size_t size, size_t mem_size )
{
    void *new;

    if( old == NULL ) {
       return( _NW_malloc( size ) );
    }
    if( size == 0 ) {
       _NW_free( old );
       return( NULL );
    }
    if( size <= mem_size ) {
        return( old );
    }

    new = _NW_malloc( size );
    if (new == NULL) {
       return( NULL );
    }
    if( size < mem_size ) {
        mem_size = size;
    }
    memcpy( new, old, mem_size );
    _NW_free( old );
    return( new );
}

void _NW_free( void *chunk )
{
    Free( chunk ); // this might call CLIB and might call OS free. Either ok.
}

static int pre_main( int argc, char **argv )
{
    _argc = argc;
    _argv = argv;
    _LpPgmName = lib_malloc( strlen( argv[ 0 ] ) + 1 );
    if( _LpPgmName == NULL ) {
        return( -1 );
    }
    strcpy( _LpPgmName, argv[ 0 ] );
    return( main( argc, argv ) );
}

static void MyExitRtn( void )
{
    __FiniMultipleThread();
    __FiniRtns( 0, InitFiniLevel );
    lib_free( _LpPgmName );
    _LpPgmName = NULL;
}

extern int _cstart_( void )
{
    int         retcode;

    _saved_DS = __DS();
    atexit( MyExitRtn );
    if( __InitThreadProcessing() == NULL ) {
        retcode = -1;
    } else {
        __InitRtns( 1 );
        InitFiniLevel = 1;
        __InitMultipleThread();
        _RWD_stacktop = __SP();
        _RWD_stacklow = _RWD_stacktop - stackavail();
        InitStackLow();
        __InitRtns( 255 );
        InitFiniLevel = 255;
        __ASTACKPTR = (char *)alloca( __ASTACKSIZ ) + __ASTACKSIZ;
        retcode = _SetupArgv( pre_main );
    }
    ExitThread( 0, retcode );
    return( 0 );
}

static void InitStackLow( void )
{
    #undef _STACKLOW
    extern unsigned _STACKLOW;

    _STACKLOW = (unsigned)&_end;
}

extern long _Prelude( void *NLMHandle, void *initializationErrorScreenID,
                      unsigned char *cmdLineP,
                      unsigned char *loadDirectoryPath,
                      long uninitializedDataLength, long NLMfileHandle,
                      long cdecl ( *readRoutineP )(), long customDataOffset,
                      long customDataSize )
{
    /* make sure that the BSS is set to zero. This overwrites our data too!*/
    memset( (void *)&_edata, 0, (size_t)&_end - (size_t)&_edata );
    AllocRTag = AllocateResourceTag( NLMHandle, "WATCOM static CLIB",
                                     AllocSignature );
    _LpCmdLine = CommandLine;
    if( cmdLineP != NULL ) {
        strcpy( _LpCmdLine, cmdLineP );
    }
    return( _StartNLM( NLMHandle, initializationErrorScreenID, cmdLineP,
                      loadDirectoryPath, uninitializedDataLength,
                      NLMfileHandle, readRoutineP, customDataOffset,
                      customDataSize, &NCSp, _cstart_ ) );
}

/* _Stop is the exit procedure for an NLM that uses the C Lib */
extern void _Stop( void )
{
    _TerminateNLM( NCSp, NULL, __ReturnCode );
}

extern void __WATCOM_Prelude( void )
{
}

/* The function __Must_Have_Three_One_Or_Greater is not in CLIB v1.0a
   but is in CLIB v3.1, therefore programs linked with this prelude will
   only run with CLIB v3.1 */
extern void __VersionEnforcement( void )
{
    __Must_Have_Three_One_Or_Greater();
}

extern void __exit( int rc )
{
    __FiniRtns( 0, InitFiniLevel );
    _exit( rc );
}

extern unsigned short __GETDS( void )
{
    return( _saved_DS );
}
