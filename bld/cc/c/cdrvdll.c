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


/* FAKEIDE -- fake IDE driver
 *
 * 95/01/25 --  J.W.Welch       Defined
 * 95/02/28 --  Jim Randall     Updated for new idedll.h
 * 95/09/01 --  MJC grabbed
*/

#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <process.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include "idedll.h"
#include "bool.h"

//#define HEAP_DUMP 0

#define MAX_CMD_LEN     (2 * 1024)

static IDEBool __stdcall printMessage     // PRINT A MESSAGE
    ( IDECBHdl hdl              // - handle
    , const char *message )     // - message
{
    hdl = hdl;
    fputs( message, stdout );
    fputc( '\n', stdout );
    fflush( stderr );
    return FALSE;
}

static IDEBool __stdcall printInfo( IDECBHdl hdl , IDEMsgInfo *info )
{
     fprintf( stdout, "%s(%u.%u): ", info->src_file, info->src_line,info->src_col );
    fputs( info->msg, stdout );
    fputc( '\n', stdout );
    fflush( stdout );
    return FALSE;
}

static IDEBool __stdcall getInfo          // GET INFORMATION  (NYI)
    ( IDECBHdl hdl              // - handle
    , IDEInfoType type          // - request type
    , unsigned long wparam      // - param(1)
    , unsigned long lparam )    // - param(2)
{
    IDEBool ret;
    hdl = hdl;
    switch( type ) {
    case IDE_GET_SOURCE_FILE:
    case IDE_GET_TARGET_FILE:
    case IDE_GET_OBJ_FILE:
    case IDE_GET_LIB_FILE:
    case IDE_GET_RES_FILE:
        ret = TRUE;
        break;
    case IDE_GET_ENV_VAR:{
        char *name;
        char **rval;

        name = (char*)wparam;
        rval = (char**)lparam;
        *rval = getenv( name );
        ret = FALSE;
        }break;
    }
    return( ret );
}

static IDEBool __stdcall runBatch               // RUN A BATCH COMMAND (NYI)
    ( IDECBHdl hdl              // - handle
    , const char *cmdline       // - command line
    , BatchFilter cb            // - batch message filter
    , void *cookie )            // - magic cookie
{
    hdl = hdl;
    cmdline = cmdline;
    cb = cb;
    cookie = cookie;
    return TRUE;
}


IDECallBacks callbacks = // DLL call-backs
{   &runBatch
,   &printMessage
,   &printMessage
,   &printInfo
,   &getInfo
};

//-------------------------------------------------------------------
// DLL DRIVER
//-------------------------------------------------------------------
#ifdef HEAP_DUMP
static void heap_dump( void ) {
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    for(;;) {
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK )break;
        printf( " %s block at %Fp of size %4.4X\n",
        (h_info._useflag == _USEDENTRY ? "USED" : "FREE" ),
        h_info._pentry, h_info._size );
    }
    switch( heap_status ) {
    case _HEAPEND:
        printf( "end of heap\n" );
        break;
    case _HEAPEMPTY:
        printf( "heap empty\n" );
        break;
    case _HEAPBADBEGIN:
        printf( "heap hurt\n" );
        break;
    case _HEAPBADPTR:
        printf( "bad heap ptr\n" );
        break;
    case _HEAPBADNODE:
        printf( "bad heap node\n" );
        break;
    }
}
#endif
struct heap_stat {
    int free;
    int used;
};

static int heap_size( struct heap_stat *stat ) {
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    stat->free = 0;
    stat->used = 0;
    for(;;) {
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK )break;
        if( h_info._useflag ) {
            stat->used += h_info._size;
        } else {
            stat->free += h_info._size;
        }
    }
    return( heap_status );
}

int main( int argc, char **argv ) {
    FILE                *in;
    char                buff[140];
    char                *cmdline;
    struct heap_stat    before;
    struct heap_stat    after;
    IDEDllHdl   hdl;
    IDEBool     fatal;
    IDEBool     retn;               // - return code
    IDEInitInfo info;

    if( argc < 2 ) {
        printf( "Useage wcc386f fn\n" );
        exit( 1 );
    }
    if( (in = fopen( argv[1], "r"))== NULL ) {
        printf( "Coundn't open %s\n", argv[1]  );
        exit( 1 );
    }


//  setmode( STDOUT_FILENO, O_BINARY );
    fatal = FALSE;

    if(  IDE_CUR_DLL_VER != IDEGetVersion() ) {
         printf( "invalid DLL version\n" );
    }
    IDEInitDLL( NULL, &callbacks, &hdl );
    info.ver = IDE_CUR_INFO_VER;
    info.ignore_env = FALSE;
    info.cmd_line_has_files = TRUE;
    IDEPassInitInfo( NULL, &info );
    while( fgets( buff, sizeof( buff ), in ) != NULL ) {
        char *tmp;
        cmdline = buff;
        printf( "%s", cmdline );
        while( isspace( *cmdline ) )++cmdline; // skip wcc386 whatever
        while( isalnum( *cmdline ) )++cmdline;
        tmp = cmdline;
        while( *tmp != '\0' ) {
            if( *tmp == '\n' ) {
                *tmp = '\0';
                break;
            }
            ++tmp;
        }
        if( *cmdline == '\0' )continue; // skip blank lines
        heap_size( &before );
        retn = IDERunYourSelf( hdl, cmdline, &fatal );
        heap_size( &after );
        if( before.free != after.free ) {
            printf( "Free different\n" );
        }
        if( before.used != after.used ) {
            printf( "Used different\n" );
        }
#ifdef HEAP_DUMP
        heap_dump();
#endif
    }
    IDEFiniDLL( NULL );
    fclose( in );
    return( 0 );
}
