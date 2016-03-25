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
* Description:  16-bit DOS implementation of execve().
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <stdlib.h>
#include <stddef.h>
#include "rtdata.h"
#include "psp.h"
#include "msdos.h"
#include "exe.h"
#include "tinyio.h"
#include "lseek.h"
#include "_process.h"
#include "_int23.h"
#include "rterrno.h"

extern execveaddr_type  __Exec_addr;
extern void _WCFAR __cdecl _doexec(char _WCI86NEAR *,char _WCI86NEAR *,int,unsigned,unsigned,unsigned,unsigned );

#define TRUE            1
#define FALSE           0
#define MIN_COM_STACK   128             /* Minimum size stack for .COM file */
#define PSP_SIZE        256             /* Size of PSP */

#define _swap(i)        (((i&0xff) << 8)|((i&0xff00) >> 8))


#pragma on(check_stack);

#pragma pack( __push, 1 )
typedef struct a_memblk {
    char                flag;           /* 'Z' if last; 'M' otherwise */
    unsigned            owner;          /* segment of psp; 0 if free */
    unsigned            size;           /* in paragraphs */
    char                unknown[11];    /* rest of header */
    char                data[1];        /* size paragraphs of data */
} a_memblk;

typedef struct a_blk {
    unsigned            next;
} a_blk;
#pragma pack( __pop )

#define _blkptr( seg )  ((a_blk    _WCFAR *)((long)(seg)<<16))
#define _mcbptr( seg )  ((a_memblk _WCFAR *)((long)((seg)-1)<<16))
#define _pspptr( seg )  ((a_psp    _WCFAR *)((long)(seg)<<16))

#define DOS2SIZE        0x281   /* paragraphs to reserve for DOS 2.X */

extern unsigned doslowblock( void );
#pragma aux doslowblock =\
    "mov ah, 52h"       \
    "int 21h"           \
    "dec bx"            \
    "dec bx"            \
    "mov ax, es:[bx]"   \
    "inc ax"            \
    modify [bx es] nomemory;

static void dosexpand( unsigned block )
{
    unsigned new_size;

    new_size = TinyMaxSet( block );
    TinySetBlock( new_size, block );
}

static unsigned dosalloc( unsigned min )
{
    tiny_ret_t rc;
    unsigned block;

    rc = TinyAllocBlock( min );
    if( TINY_ERROR( rc ) ) {
        return( 0 );
    }
    block = TINY_INFO( rc );
    dosexpand( block );
    return( block );
}

static unsigned doscalve( block, size )
    unsigned            block;
    unsigned            size;
{
    unsigned            have;

    if( (have = _mcbptr( block )->size) < size + 1 ) {
        return( 0 );
    } else {
        TinySetBlock( have - ( size + 1 ), block );
        return( dosalloc( size ) );
    }
}

static void resetints( void )
/* reset ctrl-break, floating point, divide by 0 interrupt */
{
    (*__int23_exit)();
}

static int doalloc( size, envdata, envsize )
    unsigned            size;
    unsigned            envdata;
    unsigned            envsize;
{
    unsigned            p, q, free, dosseg, envseg;

    dosseg = envseg = 0;
    for( free = 0; (p = dosalloc( 1 )) != 0; free = p ) {
        _blkptr( p )->next = free;
    }
    if( _RWD_osmajor == 2 ) {
        if( free == 0 || (dosseg = doscalve( free, DOS2SIZE )) == 0 ) {
            goto error;
        }
    }
    for( p = free; p != 0; p = _blkptr( p )->next ) {
        if( (envseg = doscalve( p, envsize )) != 0 ) {
            break;
        }
    }
    if( envseg == 0 )
        goto error;
    for( p = _RWD_psp; p < envseg && _mcbptr( p )->owner == _RWD_psp; p = q + 1 ) {
        q = p + _mcbptr( p )->size;
        if( _mcbptr( p )->flag != 'M' ) {
            if( q - _RWD_psp < size )
                goto error;
            break;
        }
    }
    _pspptr( _RWD_psp )->envp = envseg;
    movedata( envdata, 0, envseg, 0, envsize*16 );
    resetints();
    for( ;; ) {
        for( p = doslowblock(); ; p = p + _mcbptr( p )->size + 1 ) {
            if( _mcbptr( p )->owner == _RWD_psp && p != _RWD_psp && p != envseg ) {
                TinyFreeBlock( p );
                break;
            }
            if( _mcbptr( p )->flag != 'M' ) {
                dosexpand( _RWD_psp );
                _pspptr( _RWD_psp )->maxpara = _RWD_psp + _mcbptr( _RWD_psp )->size;
                if( _mcbptr( _RWD_psp )->size < size ) {
                    puts( "Not enough memory on exec\r\n" );
                    TinyTerminateProcess( 0xff );
                }
                return( TRUE );
            }
        }
    }
error: /* if we get an error */
    if( dosseg != 0 )
        TinyFreeBlock( dosseg );
    if( envseg != 0 )
        TinyFreeBlock( envseg );
    for( p = free; p != 0; p = q ) {
        q = _blkptr( p )->next;
        TinyFreeBlock( p );
    }
    return( FALSE );
}

static void save_file_handles( void )
{
    int i;
    _byte _WCFAR *handle_table;

    handle_table = _pspptr( _RWD_psp )->handle_table;
    if( handle_table != _pspptr( _RWD_psp )->sft_indices ) {
        /* save first 20 handles */
        for( i = 0 ; i < 20 ; i++ ) {
            _pspptr( _RWD_psp )->sft_indices[i] = handle_table[i];
        }
        /* close the rest of the handles */
        for( i = 20 ; i < _pspptr( _RWD_psp )->num_handles ; i++ ) {
            if( handle_table[i] != 0xff ) {
                close( i );
            }
        }
        _pspptr( _RWD_psp )->num_handles = 20;
        _pspptr( _RWD_psp )->handle_table = _pspptr( _RWD_psp )->sft_indices;
    }
}

_WCRTLINK int execve( path, argv, envp )
    const char          *path;          /* Path name of file to be executed */
    const char * const  argv[];         /* Array of pointers to arguments */
    const char * const  envp[];         /* Array of pointers to environment settings */
{
    char                *name;
    int                 file;
    an_exe_header       exe;            /* Room for exe file header */
    char                *envptr;
    char                *envstrings;
    unsigned            envseg;
    unsigned            envpara;
    size_t              cmdline_len;
    char                cmdline[128];   /* Command line build up here */
    char                buffer[80];     /* file name */
    int                 isexe;
    extern unsigned     __exec_para;
    unsigned            para;
    const char          **argvv;
    int                 i;

    strncpy( buffer, path, 75 );
    name = strrchr( buffer, '\\' );
    if( strchr( name == NULL ? buffer : name, '.' ) != NULL ) {
        file = open( buffer, O_BINARY|O_RDONLY, 0 );
        _RWD_errno = ENOENT;
        if( file == -1 ) {
            goto error;
        }
    } else {
        strcat( buffer, ".com" );
        file = open( buffer, O_BINARY|O_RDONLY, 0 );
        if( file == -1 ) {
            strcpy( strrchr( buffer, '.' ), ".exe" );
            file = open( buffer, O_BINARY|O_RDONLY, 0 );
            _RWD_errno = ENOENT;
            if( file == -1 ) {
                goto error;
            }
        }
    }

    if( read( file, (char *)&exe, sizeof( exe ) ) == -1 ) {
        close( file );
        _RWD_errno = ENOEXEC;
        _RWD_doserrno = E_badfmt;
        goto error;
    }
    isexe = exe.id == EXE_ID || exe.id == _swap( EXE_ID );
    if( isexe ) {
        para = (exe.length_div_512 - 1 )*(512/16)
             + (exe.length_mod_512 + 15)/16
             +  exe.min_para - exe.header_para;
    } else {
        para = (__lseek( file, 0, SEEK_END ) + MIN_COM_STACK + 15)/16;
    }
    close( file );
    i = 1;  /* copy the NULL terminator too */
    for( argvv = (const char **)argv; *argvv != NULL; argvv++ )
        ++i;
    argvv = malloc( i * sizeof( char * ) );
    while( --i > 0 ) {
        argvv[i] = argv[i];
    }
    argvv[0] = buffer;           /* 22-jan-88 set program name */
    envpara = __cenvarg( argvv, envp, &envptr, &envstrings,
                         &envseg, &cmdline_len, TRUE );
    if( envpara == -1 )
        goto error;
    para += PSP_SIZE/16 + __exec_para + (strlen( path ) + 15)/16;
    __ccmdline( buffer, (const char * const *)argvv, cmdline, 0 );
    if( doalloc( para, envseg, envpara ) )
        save_file_handles();
    _doexec( (char _WCI86NEAR *)buffer, (char _WCI86NEAR *)cmdline, isexe, exe.ss, exe.sp, exe.cs, exe.ip );

    free( envptr );
    free( argvv );
error: /* Clean up after error */
    return( -1 );
}

void __init_execve( void )              /* called from initializer segment */
{
    __Exec_addr = execve;
}
