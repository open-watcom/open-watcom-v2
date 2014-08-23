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
* Description:  WDISASM system specific functions.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "disasm.h"
#include "banner.h"
#include "wdismsg.h"

#ifdef __OSI__
 extern char    *_Copyright;
#endif

void  SysHelp()
/*************/

{
    DirectPuts( MSG_BANNER, stdout );
    fputs(
        banner1( "", _WDISASM_VERSION_ ) "\n"
        banner2 "\n",
        banner2a( "1988" ) "\n"
        banner3 "\n",
        banner3a "\n",
        stdout
    );
    #ifdef __OSI__
        if( _Copyright != NULL ) {
            fputs( _Copyright, stdout );
            fputs( "\n", stdout );
        }
    #endif
    MsgPutUsage();
    MsgFini();
    exit( 1 );
}


bool  IsData( segment *seg )
/**************************/

{
    char                *name;
    int                 name_len;
    int                 code_len;

    if( seg->class == TYPE_COMDAT ) {
        switch( _ComdatAlloc( seg ) ) {
        case COMDAT_EXPLICIT:
            seg = seg->u.com.seg;
            break;
        case COMDAT_FAR_CODE:
        case COMDAT_CODE32:
            return( false );
        case COMDAT_FAR_DATA:
        case COMDAT_DATA32:
            return( true );
        }
    }
    name = seg->u.seg.class_name;
    if( name == NULL ) {
        return( false );
    }
    name_len = strlen( name );
    if( name_len == 4 ) {
        if( memicmp( name, "CODE", 4 ) == 0 ) {
            return( false );
        }
    }

/* for hysterical raisins */
    name = seg->name;
    if( name == NULL ) {
        return( false );
    }
    name_len = strlen( name );
    if( memicmp( &name[ name_len - 4 ], "CODE", 4 ) == 0 ) {
        return( false );
    }
    if( memicmp( &name[ name_len - 4 ], "TEXT", 4 )  == 0 ) {
        return( false );
    }
    if( CodeName != NULL ) {            /* alternate code name supplied */
        code_len = strlen( CodeName );
        if( memicmp( name, CodeName, name_len ) == 0 ) {      /* exact match */
            return( false );
        }
        if( CodeName[ 0 ] == '*' ) {
            if( memicmp( CodeName+1, &name[ name_len-(code_len-1) ], code_len - 1 ) == 0 ) {
                return( false );
            }
        } else if( CodeName[ code_len - 1 ] == '*' ) {
            if( memicmp( CodeName, name, code_len - 1 ) == 0 ) {
                return( false );
            }
        }
    }
    return( true );
}


void  *AllocMem( size_t size )
/****************************/
{
    void                *ptr;

    ptr = malloc( size );
    if( ptr == NULL && size != 0 ) {
        SysError( ERR_OUT_OF_MEM, false );
    }
    return( ptr );
}


void  *AllocNull( size_t size )
/*****************************/
{
    void                *ptr;

    ptr = AllocMem( size );
    memset( ptr, 0, size );
    return( ptr );
}


void  FreeMem( void *ptr )
/************************/
{
    free( ptr );
}


void  SysError( int msg_num, bool io_error )
/******************************************/
{

    DirectPuts( msg_num, stderr );
    if( io_error ) {
        fputs( strerror( errno ), stderr );
    }
    fputs( "\n", stderr );
    MsgFini();
    exit( 1 );
}

#if 0
#include <malloc.h>

static char Str_Out[] = { "%s block at %p of size %4.4x\n" };
static char Str_Heap_Corupt[] = { "ERROR - Heap is corrupted - %s\n" };
static char Str_Bad_Ptr[] = { "bad pointer" };
static char Str_Bad_Begin[] = { "bad header info" };
static char Str_Bad_Node[] = { "bad node" };

static char Str_Used[] = { "Unfreed" };
static char Str_Free[] = { "Free" };

void DumpMem()
{
    struct _heapinfo    h_info;
    int                 status;
    bool                unfreed;

    unfreed = false;
    h_info._pentry = NULL;
    for(;;) {
        status = _heapwalk( &h_info );
        if( status != _HEAPOK ) break;
        if( h_info._useflag == _USEDENTRY )
            unfreed = true;
        if( unfreed ) {
            printf( Str_Out,
                h_info._useflag == _USEDENTRY ? Str_Used : Str_Free,
                h_info._pentry, h_info._size );
        }
    }
    switch( status ) {
    case _HEAPBADBEGIN:
        printf( Str_Heap_Corupt, Str_Bad_Begin );
        break;
    case _HEAPBADPTR:
        printf( Str_Heap_Corupt, Str_Bad_Ptr );
        break;
    case _HEAPBADNODE:
        printf( Str_Heap_Corupt, Str_Bad_Node );
        break;
    default:
        break;
    }
}
#endif
