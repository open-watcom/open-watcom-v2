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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#ifdef UNIX
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <clibext.h>
#else
#include <direct.h>
#endif
#include <malloc.h>

extern  void    _Not_Enough_Memory();
static  void    *_allocate(unsigned);
extern  char    *_LpCmdLine;
extern  char    *_LpPgmName;
extern  int     _argc;                  /* argument count  */
extern  char  **_argv;                  /* argument vector */
extern  int     ___Argc;                /* argument count */
extern  char  **___Argv;                /* argument vector */

static int _make_argv( char *p, char ***argv );
void __Init_Argv()
    {
        _argv = (char **) _allocate( 2 * sizeof( char * ) );
        _argv[0] = _LpPgmName;  /* fill in program name */
        _argc = _make_argv( _LpCmdLine, &_argv );
        _argv[_argc] = NULL;
        ___Argc = _argc;
        ___Argv = _argv;
    }


static int _make_argv( char *p, char ***argv )
    {
        int             argc;
        char            *start;
        char            *new;
        char            wildcard;
        char            lastchar;
        DIR *           dir;
        struct dirent * dirent;
        char            drive[_MAX_DRIVE];
        char            directory[_MAX_DIR];
        char            name[_MAX_FNAME];
        char            extin[_MAX_EXT];
        char            pathin[_MAX_PATH];
#ifdef UNIX
        struct stat     file_info;
        char            full_path[ _MAX_PATH ];
#endif
        argc = 1;
        for(;;) {
            while( *p == ' ' ) ++p;     /* skip over blanks */
            if( *p == '\0' ) break;
            /* we are at the start of a parm */
            wildcard = 0;
            if( *p == '\"' ) {
                p++;
                new = start = p;
                for(;;) {
                    /* end of parm: NULLCHAR or quote */
                    if( *p == '\"' ) break;
                    if( *p == '\0' ) break;
                    if( *p == '\\' ) {
                        if( p[1] == '\"'  ||  p[1] == '\\' )  ++p;
                    }
                    *new++ = *p++;
                }
            } else {
                new = start = p;
                for(;;) {
                    /* end of parm: NULLCHAR or blank */
                    if( *p == '\0' ) break;
                    if( *p == ' ' ) break;
                    if(( *p == '\\' )&&( p[1] == '\"' )) {
                        ++p;
                    } else if( *p == '?'  ||  *p == '*' ) {
                        wildcard = 1;
                    }
                    *new++ = *p++;
                }
            }
            *argv = realloc( *argv, (argc+2) * sizeof( char * ) );
            if( *argv == NULL )  _Not_Enough_Memory();
            (*argv)[ argc ] = start;
            ++argc;
            lastchar = *p;
            *new = '\0';
            ++p;
            if( wildcard ) {
                /* expand file names */
                dir = opendir( start );
                if( dir != NULL ) {
                    --argc;
                    _splitpath( start, drive, directory, name, extin );
                    for(;;) {
                        dirent = readdir( dir );
                        if( dirent == NULL ) break;
#ifdef UNIX
                        strcpy( full_path, start );
                        strcat( full_path, dirent->d_name );
                        stat( full_path, &file_info );
                        if( file_info.st_mode & S_IFDIR ) continue;
#else
                        if( dirent->d_attr &
                          (_A_HIDDEN+_A_SYSTEM+_A_VOLID+_A_SUBDIR) ) continue;
#endif
                        _splitpath( dirent->d_name, NULL, NULL, name, extin );
                        _makepath( pathin, drive, directory, name, extin );
                        *argv = realloc( *argv, (argc+2) * sizeof( char * ) );
                        if( *argv == NULL )  _Not_Enough_Memory();
                        new = _allocate( strlen( pathin ) + 1 );
                        strcpy( new, pathin );
                        (*argv)[argc++] = new;
                    }
                    closedir( dir );
                }
            }
            if( lastchar == '\0' ) break;
        }
        return( argc );
    }


static void *_allocate( unsigned amount )
    {
        void *p;

#if defined(__386__)
        p = malloc( amount );
#else
        p = _nmalloc( amount );
    #if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
        if( (void near *)p == NULL )  p = malloc( amount );
    #endif
#endif
        if( p == NULL )  _Not_Enough_Memory();
        return( p );
    }
