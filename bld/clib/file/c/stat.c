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


#ifdef __OSI__
#define __OS2__
#endif
#include "variety.h"
#include "widechar.h"
#undef __INLINE_FUNCTIONS__
#include <stddef.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>

#include <dos.h>
#include <dosfunc.h>
#include <mbstring.h>
#include "dosdir.h"
#include "tinyio.h"
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <stdlib.h>
    #include "mbwcconv.h"
#endif

extern time_t _d2ttime();
static unsigned short at2mode();

_WCRTLINK int __F_NAME(stat,_wstat)( CHAR_TYPE const *path, struct __F_NAME(stat,_wstat) *buf )
{
    struct find_t       dta;
    const CHAR_TYPE *   ptr;
    tiny_ret_t          rc;
    CHAR_TYPE           fullpath[_MAX_PATH];
    int                 isrootdir = 0;

    /* reject null string and names that has wildcard */
    #ifdef __WIDECHAR__
    if( *path == L'\0' || wcspbrk( path, L"*?" ) != NULL )
    #else
    if( *path == '\0' || _mbspbrk( path, "*?" ) != NULL )
    #endif
    {
        __set_errno( ENOENT );
        return( -1 );
    }

    /*** Determine if 'path' refers to a root directory ***/
    if( __F_NAME(_fullpath,_wfullpath)( fullpath, path, _MAX_PATH ) != NULL ) {
        #ifdef __WIDECHAR__
        if( iswalpha( fullpath[0] )  &&  fullpath[1] == L':'  &&
            fullpath[2] == L'\\'  &&  fullpath[3] == L'\0' )
        #else
        if( isalpha( fullpath[0] )  &&  fullpath[1] == ':'  &&
            fullpath[2] == '\\'  &&  fullpath[3] == '\0' )
        #endif
        {
            isrootdir = 1;
        }
    }

    ptr = path;
    if( __F_NAME(*_mbsinc(path),path[1]) == __F_NAME(':',L':') )  ptr += 2;
    #ifdef __WIDECHAR__
    if( ( (ptr[0] == L'\\' || ptr[0] == L'/') && ptr[1] == L'\0' )  ||  isrootdir )
    #else
    if( ( (ptr[0] == '\\' || ptr[0] == '/') && ptr[1] == '\0' )  ||  isrootdir )
    #endif
    {
        /* handle root directory */
        CHAR_TYPE       cwd[_MAX_PATH];

        /* save current directory */
        __F_NAME(getcwd,_wgetcwd)( cwd, _MAX_PATH );

        /* try to change to specified root */
        if( __F_NAME(chdir,_wchdir)( path ) != 0 )  return( -1 );

        /* restore current directory */
        __F_NAME(chdir,_wchdir)( cwd );

        dta.attrib    = _A_SUBDIR;      /* fill in DTA */
        dta.wr_time   = 0;
        dta.wr_date   = 0;
        dta.size      = 0;
        dta.name[0] = NULLCHAR;
    } else {                            /* not a root directory */
        #if defined(__OSI__)
            rc = _dos_findfirst( path, _A_NORMAL | _A_RDONLY | _A_HIDDEN |
                    _A_SYSTEM | _A_SUBDIR | _A_ARCH, &dta );
        #else
            #ifdef __WIDECHAR__
                char    mbPath[MB_CUR_MAX*_MAX_PATH];
                __filename_from_wide( mbPath, path );
            #endif
            TinySetDTA( &dta );
            rc = TinyFindFirst( __F_NAME(path,mbPath),
                                _A_NORMAL | _A_RDONLY | _A_HIDDEN |
                                _A_SYSTEM | _A_SUBDIR | _A_ARCH );
        #endif
        if( rc < 0 ) {  // Try getting information another way.
            int         handle;
            int         canread = 0;
            int         canwrite = 0;
            int         fstatok = 0;

            rc = 0;
            handle = __F_NAME(open,_wopen)( path, O_WRONLY );
            if( handle != -1 ) {
                canwrite = 1;
                if( __F_NAME(fstat,_wfstat)( handle, buf ) == -1 ) {
                    rc = errno;
                } else {
                    fstatok = 1;
                }
            }
            close( handle );
            handle = __F_NAME(open,_wopen)( path, O_RDONLY );
            if( handle != -1 ) {
                canread = 1;
                if( !fstatok ) {
                    if( __F_NAME(fstat,_wfstat)( handle, buf ) == -1 ) {
                        rc = errno;
                    }
                }
            }
            close( handle );
            if( !canread && !canwrite ) {
                __set_errno( ENOENT );
                return( -1 );
            }
            __set_errno( rc );
            if( rc != 0 ) {
                return( -1 );
            }
            if( canread ) {
                buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
            }
            if( canwrite ) {
                buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
            }
            return( 0 );
        }
        #if defined(__OSI__)
            _dos_findclose( &dta );
        #endif
    }

    /* process drive number */
    if( __F_NAME(*_mbsinc(path),path[1]) == __F_NAME(':',L':') ) {
        buf->st_dev = __F_NAME(tolower,towlower)( *path ) - __F_NAME('a',L'a');
    } else {
        buf->st_dev = TinyGetCurrDrive();
    }
    buf->st_rdev = buf->st_dev;

    buf->st_size = dta.size;
    buf->st_mode = at2mode( dta.attrib, dta.name );

    buf->st_mtime = _d2ttime( dta.wr_date, dta.wr_time );
    buf->st_atime = buf->st_ctime = buf->st_btime = buf->st_mtime;

    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_attr = dta.attrib;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    __F_NAME(_mbsnbcpy,mbstowcs)( buf->st_name, dta.name, _MAX_NAME );

    return( 0 );
}


static unsigned short at2mode( int attr, char *fname )
{
    register unsigned short mode;
    register char *         ext;

    if( attr & _A_SUBDIR )
        mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    else if( attr & AT_ISCHR )
        mode = S_IFCHR;
    else {
        mode = S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = _mbschr( fname, '.' )) != NULL ) {
            ++ext;
            if( _mbscmp( ext, "EXE" ) == 0 || _mbscmp( ext, "COM" ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( !(attr & _A_RDONLY) )                   /* if file is not read-only */
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;    /* - indicate writeable     */
    return( mode );
}
