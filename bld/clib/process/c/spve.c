/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of spawnve() for DOS, OS/2, and Win32.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <io.h>
#include <string.h>
#include <process.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __DOS__ )
    #include <dos.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "doserrno.h"
#include "liballoc.h"
#include "filestr.h"
#include "msdos.h"
#include "_process.h"
#include "thread.h"
#include "pathmac.h"


#ifdef __USE_POSIX_HANDLE_STRINGS
    #define _POSIX_HANDLE_CLEANUP   {                                   \
                                        if( doFreeFlag ) {              \
                                            lib_free( (void *)envp );   \
                                            lib_free( fileinfo );       \
                                            doFreeFlag = 0;             \
                                        }                               \
                                    }

#else
    #define _POSIX_HANDLE_CLEANUP
#endif

#if defined( __DOS_086__ )
    #define ENV_ARG     unsigned
    #define ENVPARM     envseg
#else
    #define ENV_ARG     CHAR_TYPE *
    #define ENVPARM     envptr
#endif

#if defined( __OS2__ ) && defined( __BIG_DATA__ )
    #define LIB_ALLOC   lib_fmalloc
    #define LIB_FREE    lib_ffree
#else
    #define LIB_ALLOC   lib_nmalloc
    #define LIB_FREE    lib_nfree
#endif

#if defined( __DOS__ )
    typedef CHAR_TYPE __based( __segname( "_STACK" ) )  *char_type_stk_ptr;
#else
    typedef CHAR_TYPE                                   *char_type_stk_ptr;
#endif

#if defined( __DOS__ )
extern int  __cdecl __dospawn( int mode, char_type_stk_ptr pgmname, char_type_stk_ptr cmdline, ENV_ARG env );
#pragma aux __dospawn "_*"
#endif

#define FALSE   0

static int file_exists( const CHAR_TYPE *filename )
{
#if defined( __DOS__ )
    /* should use _dos_findfirst to avoid DOS APPEND bug */
    struct find_t  find_buf;

    if( _dos_findfirst( filename,
            _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM, &find_buf ) == 0 )
        return( 1 );
#else       /* __OS2__, __NT__ */
    if( __F_NAME(access,_waccess)( filename, 0 ) == 0 )
        return( 1 );
#endif
    return( 0 );
}

#if defined( __DOS__ )
static int _dospawn( int mode, char_type_stk_ptr pgmname, char_type_stk_ptr cmdline, ENV_ARG env, const char * const *argv )
{
    /* do this here instead of in the .asm files */
    __ccmdline( pgmname, argv, cmdline, 0 );
    return( __dospawn( mode, pgmname, cmdline, env ) );
}
#endif


#pragma on(check_stack);

_WCRTLINK int __F_NAME(spawnve,_wspawnve)( int mode, const CHAR_TYPE * path,
                                          const CHAR_TYPE * const argv[], const CHAR_TYPE * const in_envp[] )
{
    const CHAR_TYPE * const *envp = (const CHAR_TYPE * const *)in_envp;
    CHAR_TYPE               *_envptr;       /* environment ptr (unaligned) */
    CHAR_TYPE               *envptr;        /* environment ptr (DOS 16-bit aligned to para) */
    unsigned                envseg;         /* environment segment (DOS 16-bit normalized, zero for others) */
    int                     len;
    char_type_stk_ptr       np;
    char_type_stk_ptr       p;
    char_type_stk_ptr       end_of_p;
    int                     retval;
#if defined( __DOS_086__ )
    unsigned                envsize_paras;  /* for environment */
#endif
    size_t                  cmdline_len;
    char_type_stk_ptr       cmdline_mem;
    char_type_stk_ptr       cmdline;
    CHAR_TYPE               switch_c[4];
    bool                    prot_mode286;
    bool                    use_cmd;
#if defined( __DOS__ )
    _87state                _87save;
#endif
    CHAR_TYPE               *drive;
    CHAR_TYPE               *dir;
    CHAR_TYPE               *fname;
    CHAR_TYPE               *ext;
    int                     rc;

#ifdef __USE_POSIX_HANDLE_STRINGS
    CHAR_TYPE               **newEnvp;
    int                     count;
    CHAR_TYPE               *fileinfo;
    int                     doFreeFlag = 0;

    if( _fileinfo != 0 ) {
        fileinfo = __F_NAME(__FormPosixHandleStr,__wFormPosixHandleStr)();
        if( fileinfo != NULL ) {
            count = 1;
            if( envp != NULL ) {
                newEnvp = (CHAR_TYPE **)envp;
                while( *newEnvp != NULL ) { /* count 'em, including NULL */
                    newEnvp++;
                    count++;
                }
            }
            count++;                        /* one more for the new entry */
            newEnvp = lib_malloc( count * sizeof(CHAR_TYPE*) );
            if( newEnvp != NULL ) {
                if( envp != NULL ) {
                    memcpy( newEnvp, envp, (count-1)*sizeof(CHAR_TYPE*) );
                }
                newEnvp[count-2] = fileinfo;/* add C_FILE_INFO entry */
                newEnvp[count-1] = NULL;    /* terminate pointer list */
                envp = (const CHAR_TYPE **)newEnvp;/* use new environment */
                doFreeFlag = 1;             /* do cleanup on exit */
            } else {
                lib_free( fileinfo );
            }
        }
    }
#endif

#if defined( _M_I86 )
 #if defined( __OS2__ )
    prot_mode286 = _osmode_PROTMODE();
    if( mode == OLD_P_OVERLAY ) {
        rc = execve(path, argv, envp);
        _POSIX_HANDLE_CLEANUP;
        return( rc );
    }
 #else      /* __DOS_086__ */
    prot_mode286 = false;
    if( mode == OLD_P_OVERLAY ) {
        execveaddr_type    execve;
        execve = __execaddr();
        if( execve != NULL ) {
            rc = (*execve)( path, argv, envp );
            _POSIX_HANDLE_CLEANUP;
            return( rc );
        }
        lib_set_errno( EINVAL );
        _POSIX_HANDLE_CLEANUP;
        return( -1 );
    }
 #endif
    use_cmd = prot_mode286;
#else   // 32-bit
    prot_mode286 = false;

 #if defined(__DOS__)
    use_cmd = false;
    if( mode >= OLD_P_OVERLAY ) {
        lib_set_errno( EINVAL );
        rc = -1;
        _POSIX_HANDLE_CLEANUP;
        return( rc );
    }
 #else      /* __OS2__, __NT__ */
    use_cmd = true;
    if( mode == OLD_P_OVERLAY ) {
        rc = __F_NAME(execve,_wexecve)(path, argv, envp);
        _POSIX_HANDLE_CLEANUP;
        return( rc );
    }
 #endif
#endif
    retval = __F_NAME(__cenvarg,__wcenvarg)( argv, envp, &_envptr, &envptr, &envseg, &cmdline_len, false );
    if( retval == -1 ) {
        _POSIX_HANDLE_CLEANUP;
        return( -1 );
    }
#if defined( __DOS_086__ )
    envsize_paras = retval;
#endif
    len = __F_NAME(strlen,wcslen)( path ) + 7 + _MAX_PATH2;
    np = LIB_ALLOC( len * sizeof( CHAR_TYPE ) );
    if( np == NULL ) {
        p = alloca( len * sizeof( CHAR_TYPE ) );
        if( p == NULL ) {
            lib_free( _envptr );
            _POSIX_HANDLE_CLEANUP;
            return( -1 );
        }
    } else {
        p = np;
    }
    __F_NAME(_splitpath2,_wsplitpath2)( path, p + ( len - _MAX_PATH2 ), &drive, &dir, &fname, &ext );
#if defined( __DOS__ )
    _RWD_Save8087( &_87save );
#endif
#if defined( __DOS_086__ )
    if( _osmode_PROTMODE() ) {      /* if protect-mode e.g. DOS/16M */
        unsigned    segment;

        if( _dos_allocmem( envsize_paras, &segment ) != 0 ) {
            LIB_FREE( np );
            lib_free( _envptr );
            _POSIX_HANDLE_CLEANUP;
            return( -1 );
        }
        envseg = segment;
        _fmemcpy( (void __far *)(((long)(segment))<<16), envptr, envsize_paras << 4 );
    }
#endif
    /* allocate the cmdline buffer */
    cmdline_mem = LIB_ALLOC( cmdline_len * sizeof( CHAR_TYPE ) );
    if( cmdline_mem == NULL ) {
        cmdline = alloca( cmdline_len * sizeof( CHAR_TYPE ) );
        if( cmdline == NULL ) {
            retval = -1;
            lib_set_errno( E2BIG );
            lib_set_doserrno( E_badenv );
        }
    } else {
        cmdline = cmdline_mem;
    }
    if( cmdline != NULL ) {
#if defined( __DOS__ )
        /* 17-oct-88, check version #
         * 'append' program in DOS has a bug, so we avoid it by putting '.\'
         * on the front of a filename that doesn't have a path or drive
         */
        if( _RWD_osmajor >= 3 ) {
            if( drive[0] == NULLCHAR && dir[0] == NULLCHAR ) {
                dir = STRING( ".\\" );
            }
        }
#endif
        __F_NAME(_makepath,_wmakepath)( p, drive, dir, fname, ext );
        lib_set_errno( ENOENT );
        if( ext[0] != NULLCHAR ) {
#if defined( __OS2__ )
            if( _stricmp( ext, STRING( ".cmd" ) ) == 0 || _stricmp( ext, STRING( ".bat" ) ) == 0 ) {
#else
            if( __F_NAME(_stricmp,_wcsicmp)( ext, STRING( ".bat" ) ) == 0 ) {
#endif
                retval = -1; /* assume file doesn't exist */
                if( file_exists( p ) ) {
#if defined( __DOS__ )
                    /* the environment will have to be reconstructed */
                    lib_free( _envptr );
                    _envptr = NULL;
#endif
                    __F_NAME(__ccmdline,__wccmdline)( p, argv, cmdline, 1 );
                    retval = __F_NAME(spawnl,_wspawnl)( mode,
                        __F_NAME(getenv,_wgetenv)( STRING( "COMSPEC" ) ),
                        use_cmd ? STRING( "CMD" ) : STRING( "COMMAND" ),
                        __F_NAME(__Slash_C,__wSlash_C)( switch_c, use_cmd ),
                        p, cmdline, NULL );
                }
            } else {
                lib_set_errno( 0 );
                /* user specified an extension, so try it */
                retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, ENVPARM, argv );
            }
#if defined( __DOS__ )
        } else {
#else       /* __OS2__, __NT__ */
        }
        /*
         * consider the following valid executable filenames:
         *      a.b.exe  a.cmd.exe  a.exe.cmd  a.cmd
         * we must always try to add .exe, etc.
         */
        if( lib_get_errno() == ENOENT || lib_get_errno() == EINVAL ) {
#endif
            end_of_p = p + __F_NAME(strlen,wcslen)( p );
            if( prot_mode286 ) {
                lib_set_errno( ENOENT );
            } else {
                __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".com" ) );
                lib_set_errno( 0 );
                retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, ENVPARM, argv );
            }
            if( lib_get_errno() == ENOENT || lib_get_errno() == EINVAL ) {
                lib_set_errno( 0 );
                __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".exe" ) );
                retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, ENVPARM, argv );
                if( lib_get_errno() == ENOENT || lib_get_errno() == EINVAL ) {
                    /* try for a .BAT file */
                    lib_set_errno( 0 );
#if defined( __OS2__ )
                    strcpy( end_of_p, STRING( ".cmd" ) );
                    if( !file_exists( p ) )
                        strcpy( end_of_p, STRING( ".bat" ) );
#else
                    __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".bat" ) );
#endif
                    if( file_exists( p ) ) {
                        /* the environment will have to be reconstructed */
                        lib_free( _envptr );
                        _envptr = NULL;
                        __F_NAME(__ccmdline,__wccmdline)( p, argv, cmdline, 1 );
                        retval = __F_NAME(spawnl,_wspawnl)( mode,
                            __F_NAME(getenv,_wgetenv)( STRING( "COMSPEC" ) ),
                            use_cmd ? STRING( "CMD" ) : STRING( "COMMAND" ),
                            __F_NAME(__Slash_C,__wSlash_C)( switch_c, use_cmd ),
                            p, cmdline, NULL );
                    }
                }
            }
        }
    }
    _POSIX_HANDLE_CLEANUP;
    LIB_FREE( cmdline_mem );
    LIB_FREE( np );
    lib_free( _envptr );
#if defined( __DOS_086__ )
    if( _osmode_PROTMODE() ) {      /* if protect-mode e.g. DOS/16M */
        _dos_freemem( envseg );
    }
#endif
#if defined( __DOS__ )
    _RWD_Rest8087( &_87save );
#endif
    return( retval );
}
