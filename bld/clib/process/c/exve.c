/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS implementation of execve() (16-bit code only)
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "seterrno.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include "roundmac.h"
#include "rtdata.h"
#include "dospsp.h"
#include "dosmem.h"
#include "msdos.h"
#include "exe.h"
#include "tinyio.h"
#include "lseek.h"
#include "_process.h"
#include "_int23.h"
#include "doserrno.h"


#define MIN_COM_STACK   128             /* Minimum size stack for .COM file */
#define PSP_SIZE        256             /* Size of PSP */

#define _swap(i)        (((i&0xff) << 8)|((i&0xff00) >> 8))

#pragma on(check_stack);

typedef struct a_blk {
    unsigned            next;
} a_blk;

typedef char __based( __segname( "_STACK" ) ) *char_stk_ptr;

#define _blkptr( seg )  ((a_blk      _WCFAR *)((long)(seg)<<16))
#define _mcbptr( seg )  ((dosmem_blk _WCFAR *)((long)((seg)-1)<<16))
#define _pspptr( seg )  ((dospsp     _WCFAR *)((long)(seg)<<16))

#define DOS2SIZE        0x281   /* paragraphs to reserve for DOS 2.X */

extern unsigned doslowblock( void );
#pragma aux doslowblock = \
        _MOV_AH DOS_GET_LIST_OF_LIST \
        __INT_21            \
        "dec bx"            \
        "dec bx"            \
        "mov ax, es:[bx]"   \
        "inc ax"            \
    __parm              [] \
    __value             [__ax] \
    __modify __nomemory [__bx __es]

extern void         __cdecl _WCFAR _doexec( char_stk_ptr, char_stk_ptr, int, unsigned, unsigned, unsigned, unsigned );
extern void         _WCNEAR __init_execve( void );
extern unsigned     __exec_para;

static void dosexpand( unsigned block )
{
    unsigned num_of_paras;

    num_of_paras = TinyMaxSet( block );
    TinySetBlock( block, num_of_paras );
}

static unsigned dosalloc( unsigned num_of_paras )
{
    tiny_ret_t  rc;
    unsigned    block;

    rc = TinyAllocBlock( num_of_paras );
    if( TINY_ERROR( rc ) ) {
        return( 0 );
    }
    block = TINY_INFO( rc );
    dosexpand( block );
    return( block );
}

static unsigned doscalve( unsigned block, unsigned req_paras )
{
    unsigned    block_num_of_paras;

    block_num_of_paras = _mcbptr( block )->size;
    if( block_num_of_paras < req_paras + 1 ) {
        return( 0 );
    } else {
        TinySetBlock( block, block_num_of_paras - ( req_paras + 1 ) );
        return( dosalloc( req_paras ) );
    }
}

static void resetints( void )
/* reset ctrl-break, floating point, divide by 0 interrupt */
{
    (*__int23_exit)();
}

static bool doalloc( unsigned size, unsigned envdata, unsigned envsize_paras )
{
    unsigned            p;
    unsigned            q;
    unsigned            free;
    unsigned            dosseg;
    unsigned            envseg;

    dosseg = envseg = _NULLSEG;
    for( free = _NULLSEG; (p = dosalloc( 1 )) != _NULLSEG; free = p ) {
        _blkptr( p )->next = free;
    }
    if( _RWD_osmajor == 2 ) {
        if( free == _NULLSEG
          || (dosseg = doscalve( free, DOS2SIZE )) == _NULLSEG ) {
            goto error;
        }
    }
    for( p = free; p != _NULLSEG; p = _blkptr( p )->next ) {
        if( (envseg = doscalve( p, envsize_paras )) != _NULLSEG ) {
            break;
        }
    }
    if( envseg == _NULLSEG )
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
    movedata( envdata, 0, envseg, 0, envsize_paras << 4 );
    resetints();
    for( ;; ) {
        for( p = doslowblock(); ; p = p + _mcbptr( p )->size + 1 ) {
            if( _mcbptr( p )->owner == _RWD_psp
              && p != _RWD_psp
              && p != envseg ) {
                TinyFreeBlock( p );
                break;
            }
            if( _mcbptr( p )->flag != 'M' ) {
                dosexpand( _RWD_psp );
                _pspptr( _RWD_psp )->maxpara = _RWD_psp + _mcbptr( _RWD_psp )->size;
                if( _mcbptr( _RWD_psp )->size < size ) {
                    puts( "Not enough memory on exec\r\n" );
                    TinyTerminateProcess( -1 );
                }
                return( true );
            }
        }
    }
error: /* if we get an error */
    if( dosseg != _NULLSEG )
        TinyFreeBlock( dosseg );
    if( envseg != _NULLSEG )
        TinyFreeBlock( envseg );
    for( p = free; p != _NULLSEG; p = q ) {
        q = _blkptr( p )->next;
        TinyFreeBlock( p );
    }
    return( false );
}

static void save_file_handles( void )
{
    int             i;
    handle_tab_ptr  handle_table;

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
    char                *_envptr;       /* environment ptr (unaligned) */
    char                *envptr;        /* environment ptr (DOS 16-bit aligned to para) */
    unsigned            envseg;         /* environment segment (DOS 16-bit normalized, zero for others) */
    unsigned            envpara;
    size_t              cmdline_len;
    char                cmdline[128];   /* Command line build up here */
    char                pgmname[80];    /* file name */
    int                 isexe;
    unsigned            para;
    const char          **argvv;
    int                 i;

    strncpy( pgmname, path, 75 );
    name = strrchr( pgmname, '\\' );
    if( strchr( name == NULL ? pgmname : name, '.' ) != NULL ) {
        file = open( pgmname, O_BINARY|O_RDONLY, 0 );
        lib_set_errno( ENOENT );
        if( file == -1 ) {
            goto error;
        }
    } else {
        strcat( pgmname, ".com" );
        file = open( pgmname, O_BINARY|O_RDONLY, 0 );
        if( file == -1 ) {
            strcpy( strrchr( pgmname, '.' ), ".exe" );
            file = open( pgmname, O_BINARY|O_RDONLY, 0 );
            lib_set_errno( ENOENT );
            if( file == -1 ) {
                goto error;
            }
        }
    }

    if( read( file, (char *)&exe, sizeof( exe ) ) == -1 ) {
        close( file );
        lib_set_errno( ENOEXEC );
        lib_set_doserrno( E_badfmt );
        goto error;
    }
    isexe = exe.id == EXE_ID || exe.id == _swap( EXE_ID );
    if( isexe ) {
        para = ( exe.length_div_512 - 1 ) * __ROUND_DOWN_SIZE_TO_PARA( 512 )
             + __ROUND_UP_SIZE_TO_PARA( exe.length_mod_512 )
             +  exe.min_para - exe.header_para;
    } else {
        para = __ROUND_UP_SIZE_TO_PARA( __lseek( file, 0, SEEK_END ) + MIN_COM_STACK );
    }
    close( file );
    i = 1;  /* copy the NULL terminator too */
    for( argvv = (const char **)argv; *argvv != NULL; argvv++ )
        ++i;
    argvv = malloc( i * sizeof( char * ) );
    while( --i > 0 ) {
        argvv[i] = argv[i];
    }
    argvv[0] = pgmname;         /* set program name */
    envpara = __cenvarg( argvv, envp, &_envptr, &envptr, &envseg, &cmdline_len, true );
    if( envpara == -1 )
        goto error;
    para += __ROUND_DOWN_SIZE_TO_PARA( PSP_SIZE ) + __exec_para + __ROUND_UP_SIZE_TO_PARA( strlen( path ) );
    __ccmdline( pgmname, (const char * const *)argvv, cmdline, 0 );
    if( doalloc( para, envseg, envpara ) )
        save_file_handles();
    _doexec( pgmname, cmdline, isexe, exe.ss, exe.sp, exe.cs, exe.ip );

    free( _envptr );
    free( argvv );
error: /* Clean up after error */
    return( -1 );
}

void _WCNEAR __init_execve( void )      /* called from initializer segment */
{
    __Exec_addr = execve;
}
