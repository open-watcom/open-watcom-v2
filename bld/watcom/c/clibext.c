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
* Description:  Implementation of non-standard functions used by bootstrap
*
****************************************************************************/


#ifdef __WATCOMC__
    /* We don't need any of this stuff, but being able to build this
     * module simplifies makefiles.
     */
#else

#include <string.h>
#include <ctype.h>
#if defined(__UNIX__)
    #include <dirent.h>
  #if defined(__QNX__)
    #include <sys/io_msg.h>
  #endif
#else
    #include <direct.h>
  #if defined(__OS2__)
    #include <wos2.h>
  #elif defined(__NT__)
    #include <windows.h>
    #include <mbstring.h>
  #endif
#endif
#include "wio.h"
#include "wreslang.h"
#include "clibext.h"

#define __set_errno( err ) errno = (err)

char **_argv;
int  _argc;

/****************************************************************************
*
* Description:  Shared alphabet array for conversion of integers to ASCII.
*
****************************************************************************/

#ifndef _MSC_VER

static const char __Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";


char *utoa( unsigned value, char *buffer, int radix )
{
    char   *p = buffer;
    char        *q;
    unsigned    rem;
    unsigned    quot;
    char        buf[34];    // only holds ASCII so 'char' is OK

    buf[0] = '\0';
    q = &buf[1];
    do {
        rem = value % radix;
        quot = value / radix;
        *q = __Alphabet[rem];
        ++q;
        value = quot;
    } while( value != 0 );
    while( (*p++ = (char)*--q) )
        ;
    return( buffer );
}


char *itoa( int value, char *buffer, int radix )
{
    char   *p = buffer;

    if( radix == 10 ) {
        if( value < 0 ) {
            *p++ = '-';
            value = - value;
        }
    }
    utoa( value, p, radix );
    return( buffer );
}

/****************************************************************************
*
* Description:  Implementation of ltoa().
*
****************************************************************************/

char *ultoa( unsigned long value, char *buffer, int radix )
{
    char   *p = buffer;
    char        *q;
    unsigned    rem;
    char        buf[34];        // only holds ASCII so 'char' is OK

    buf[0] = '\0';
    q = &buf[1];
    do {
        rem = value % radix;
        value = value / radix;
        *q = __Alphabet[rem];
        ++q;
    } while( value != 0 );
    while( (*p++ = (char)*--q) )
        ;
    return( buffer );
}


char *ltoa( long value, char *buffer, int radix )
{
    char   *p = buffer;

    if( radix == 10 ) {
        if( value < 0 ) {
            *p++ = '-';
            value = - value;
        }
    }
    ultoa( value, p, radix );
    return( buffer );
}

/****************************************************************************
*
* Description:  Platform independent _splitpath() implementation.
*
****************************************************************************/

#if defined(__UNIX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows, Netware */
  #define PC '\\'
  #define ALT_PC '/'
#endif

#ifdef __NETWARE__
  #undef _MAX_PATH
  #undef _MAX_SERVER
  #undef _MAX_VOLUME
  #undef _MAX_DRIVE
  #undef _MAX_DIR
  #undef _MAX_FNAME
  #undef _MAX_EXT
  #undef _MAX_NAME

  #define _MAX_PATH    255 /* maximum length of full pathname */
  #define _MAX_SERVER  48  /* maximum length of server name */
  #define _MAX_VOLUME  16  /* maximum length of volume component */
  #define _MAX_DRIVE   3   /* maximum length of drive component */
  #define _MAX_DIR     255 /* maximum length of path component */
  #define _MAX_FNAME   9   /* maximum length of file name component */
  #define _MAX_EXT     5   /* maximum length of extension component */
  #define _MAX_NAME    13  /* maximum length of file name (with extension) */
#endif


static void copypart( char *buf, const char *p, int len, int maxlen )
{
    if( buf != NULL ) {
        if( len > maxlen ) len = maxlen;
            #ifdef __UNIX__
                memcpy( buf, p, len );
                buf[len] = '\0';
            #else
                len = _mbsnccnt( p, len );          /* # chars in len bytes */
                _mbsncpy( buf, p, len );            /* copy the chars */
                buf[ _mbsnbcnt(buf,len) ] = '\0';
            #endif
    }
}

#if !defined(_MAX_NODE)
#define _MAX_NODE   _MAX_DRIVE  /*  maximum length of node name w/ '\0' */
#endif

/* split full QNX path name into its components */

/* Under QNX we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */

/* Under Netware, 'drive' maps to 'volume' */

void _splitpath( const char *path,
    char *drive, char *dir, char *fname, char *ext )
{
    const char *dotp;
    const char *fnamep;
    const char *startp;
    unsigned    ch;
#ifdef __NETWARE__
    const char *ptr;
#endif

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

#if defined(__UNIX__)

    /* process node/drive specification */
    startp = path;
    if( path[0] == PC  &&  path[1] == PC ) {
        path += 2;
        for( ;; ) {
            if( *path == '\0' ) break;
            if( *path == PC ) break;
            if( *path == '.' ) break;
                #ifdef __UNIX__
                    path++;
                #else
                    path = _mbsinc( path );
                #endif
        }
    }
    copypart( drive, startp, path - startp, _MAX_NODE );

#elif defined(__NETWARE__)

        #ifdef __UNIX__
            ptr = strchr( path, ':' );
        #else
            ptr = _mbschr( path, ':' );
        #endif
    if( ptr != NULL ) {
        if( drive != NULL ) {
            copypart( drive, path, ptr - path + 1, _MAX_SERVER +
                      _MAX_VOLUME + 1 );
        }
            #ifdef __UNIX__
                path = ptr + 1;
            #else
                path = _mbsinc( ptr );
            #endif
    } else if( drive != NULL ) {
        *drive = '\0';
    }

#else

    /* processs drive specification */
    if( path[0] != '\0'  &&  path[1] == ':' ) {
        if( drive != NULL ) {
            drive[0] = path[0];
            drive[1] = ':';
            drive[2] = '\0';
        }
        path += 2;
    } else if( drive != NULL ) {
        drive[0] = '\0';
    }

#endif

    /* process /user/fred/filename.ext for QNX */
    /* process /fred/filename.ext for DOS, OS/2 */
    dotp = NULL;
    fnamep = path;
    startp = path;

    for(;;) {           /* 07-jul-91 DJG -- save *path in ch for speed */
        if( *path == '\0' )  break;
            #ifdef __UNIX__
                ch = *path;
            #else
                ch = _mbsnextc( path );
            #endif
        if( ch == '.' ) {
            dotp = path;
            ++path;
            continue;
        }
            #ifdef __UNIX__
                path++;
            #else
                path = _mbsinc( path );
            #endif
#if defined(__UNIX__)
        if( ch == PC ) {
#else /* DOS, OS/2, Windows, Netware */
        if( ch == PC  ||  ch == ALT_PC ) {
#endif
            fnamep = path;
            dotp = NULL;
        }
    }
    copypart( dir, startp, fnamep - startp, _MAX_DIR - 1 );
    if( dotp == NULL ) dotp = path;
    copypart( fname, fnamep, dotp - fnamep, _MAX_FNAME - 1 );
    copypart( ext,   dotp,   path - dotp,   _MAX_EXT - 1);
}

#endif /* !_MSC_VER */

/****************************************************************************
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

#if defined(__UNIX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows */
  #define PC '\\'
  #define ALT_PC '/'
#endif

/* split full Unix path name into its components */

/* Under Unix we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */


static unsigned char *pcopy( unsigned char **pdst, unsigned char *dst, const unsigned char *b_src, const unsigned char *e_src ) {
/*========================================================================*/

    size_t    len;

    if( pdst == NULL ) return( dst );
    *pdst = dst;
    len = e_src - b_src;
    if( len >= _MAX_PATH2 )
    {
        len = _MAX_PATH2 - 1;
    }
#ifdef __UNIX__
    memcpy( dst, b_src, len );
    dst[len] = '\0';
    return( dst + len + 1 );
#else
    len = _mbsnccnt( b_src, len );          /* # chars in len bytes */
    _mbsncpy( dst, b_src, len );            /* copy the chars */
    dst[ _mbsnbcnt(dst,len) ] = '\0';
    return( dst + _mbsnbcnt(dst,len) + 1 );
#endif
}

void  _splitpath2( char const *inp, char *outp, char **drive, char **path, char **fn, char **ext )
/*==============================================================================================*/
{
    unsigned char const *dotp;
    unsigned char const *fnamep;
    unsigned char const *startp;
    unsigned        ch;

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> \\disk2\fred\filename.ext for UNC names */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

    /* process node/drive/UNC specification */
    startp = (const unsigned char *)inp;
    #ifdef __UNIX__
        if( inp[0] == PC  &&  inp[1] == PC )
    #else
        if( (inp[0] == PC || inp[0] == ALT_PC)
         && (inp[1] == PC || inp[1] == ALT_PC) )
    #endif
    {
        inp += 2;
        for( ;; )
        {
            if( *inp == '\0' )
                break;
            if( *inp == PC )
                break;
            #ifndef __UNIX__
                if( *inp == ALT_PC )
                    break;
            #endif
            if( *inp == '.' )
                break;
                #ifdef __UNIX__
                    inp++;
                #else
                    inp = (const char *)_mbsinc( (const unsigned char *)inp );
                #endif
        }
        outp = (char *)pcopy( (unsigned char **)drive, (unsigned char *)outp, startp, (const unsigned char *)inp );
#if !defined(__UNIX__)
    /* process drive specification */
    }
    else if( inp[0] != '\0' && inp[1] == ':' )
    {
        if( drive != NULL )
        {
            *drive = outp;
            outp[0] = inp[0];
            outp[1] = ':';
            outp[2] = '\0';
            outp += 3;
        }
        inp += 2;
#endif
    }
    else if( drive != NULL )
    {
        *drive = outp;
        *outp = '\0';
        ++outp;
    }

    /* process /user/fred/filename.ext for QNX */
    /* process \fred\filename.ext for DOS, OS/2 */
    /* process /fred/filename.ext for DOS, OS/2 */
    dotp = NULL;
    fnamep = (const unsigned char *)inp;
    startp = (const unsigned char *)inp;

    for(;;)
    {
            #ifdef __UNIX__
                ch = *inp;
            #else
                ch = _mbsnextc( (const unsigned char *)inp );
            #endif
        if( ch == 0 )
            break;
        if( ch == '.' )
        {
            dotp = (const unsigned char *)inp;
            ++inp;
            continue;
        }
            #ifdef __UNIX__
                inp++;
            #else
                inp = (const char *)_mbsinc( (const unsigned char *)inp );
            #endif
#if defined(__UNIX__)
        if( ch == PC )
        {
#else /* DOS, OS/2, Windows */
        if( ch == PC  ||  ch == ALT_PC )
        {
#endif
            fnamep = (const unsigned char *)inp;
            dotp = NULL;
        }
    }
    outp = (char *)pcopy( (unsigned char **)path, (unsigned char *)outp, startp, fnamep );
    if( dotp == NULL )
        dotp = (const unsigned char *)inp;
    outp = (char *)pcopy( (unsigned char **)fn, (unsigned char *)outp, fnamep, dotp );
    outp = (char *)pcopy( (unsigned char **)ext, (unsigned char *)outp, dotp, (const unsigned char *)inp );
}

#ifndef _MSC_VER

/****************************************************************************
*
* Description:  Platform independent _makepath() implementation.
*
****************************************************************************/

#undef _makepath

#if defined(__UNIX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows, Netware */
  #define PC '\\'
  #define ALT_PC '/'
#endif


#if defined(__UNIX__)

/* create full Unix style path name from the components */

void _makepath(
        char           *path,
        const char  *node,
        const char  *dir,
        const char  *fname,
        const char  *ext )
{
    *path = '\0';

    if( node != NULL ) {
        if( *node != '\0' ) {
            strcpy( path, node );
            path = strchr( path, '\0' );

            /* if node did not end in '/' then put in a provisional one */
            if( path[-1] == PC )
                path--;
            else
                *path = PC;
        }
    }
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            /*  if dir does not start with a '/' and we had a node then
                    stick in a separator
            */
            if( (*dir != PC) && (*path == PC) ) path++;

            strcpy( path, dir );
            path = strchr( path, '\0' );

            /* if dir did not end in '/' then put in a provisional one */
            if( path[-1] == PC )
                path--;
            else
                *path = PC;
        }
    }

    if( fname != NULL ) {
        if( (*fname != PC) && (*path == PC) ) path++;

        strcpy( path, fname );
        path = strchr( path, '\0' );

    } else {
        if( *path == PC ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            strcpy( path, ext );
            path = strchr( path, '\0' );
        }
    }
    *path = '\0';
}

#elif defined( __NETWARE__ )

/*
    For silly two choice DOS path characters / and \,
    we want to return a consistent path character.
*/

static char pickup( char c, char *pc_of_choice )
{
    if( c == PC || c == ALT_PC ) {
        if( *pc_of_choice == '\0' ) *pc_of_choice = c;
        c = *pc_of_choice;
    }
    return( c );
}

extern void _makepath( char *path, const char *volume,
                const char *dir, const char *fname, const char *ext )
{
    char first_pc = '\0';

    if( volume != NULL ) {
        if( *volume != '\0' ) {
            do {
                *path++ = *volume++;
            } while( *volume != '\0' );
            if( path[ -1 ] != ':' ) {
                *path++ = ':';
            }
        }
    }
    *path = '\0';
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            do {
                *path++ = pickup( *dir++, &first_pc );
            } while( *dir != '\0' );
            /* if no path separator was specified then pick a default */
            if( first_pc == '\0' ) first_pc = PC;
            /* if dir did not end in path sep then put in a provisional one */
            if( path[-1] == first_pc ) {
                path--;
            } else {
                *path = first_pc;
            }
        }
    }
    /* if no path separator was specified thus far then pick a default */
    if( first_pc == '\0' ) first_pc = PC;
    if( fname != NULL ) {
        if( (pickup( *fname, &first_pc ) != first_pc)
            && (*path == first_pc) ) path++;
        while( *fname != '\0' ) *path++ = pickup( *fname++, &first_pc );
    } else {
        if( *path == first_pc ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            while( *ext != '\0' ) *path++ = *ext++;
        }
    }
    *path = '\0';
}

#else

/*
    For silly two choice DOS path characters / and \,
    we want to return a consistent path character.
*/

static unsigned pickup( unsigned c, unsigned *pc_of_choice )
{
    if( c == PC || c == ALT_PC ) {
        if( *pc_of_choice == '\0' ) *pc_of_choice = c;
        c = *pc_of_choice;
    }
    return( c );
}

/* create full MS-DOS path name from the components */

void _makepath( char *path, const char *drive,
                const char *dir, const char *fname, const char *ext )
{
    unsigned            first_pc = '\0';
    char *              pathstart = path;
    unsigned            ch;

    if( drive != NULL ) {
        if( *drive != '\0' ) {
            if ((*drive == '\\') && (drive[1] == '\\')) {
                strcpy(path, drive);
                path += strlen(drive);
            } else {
                *path++ = *drive;                               /* OK for MBCS */
                *path++ = ':';
            }
        }
    }
    *path = '\0';
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            do {
                    ch = pickup( _mbsnextc(dir), &first_pc );
                    _mbvtop( ch, path );
                    path[_mbclen(path)] = '\0';
                    path = _mbsinc( path );
                    dir = _mbsinc( dir );
            } while( *dir != '\0' );
            /* if no path separator was specified then pick a default */
            if( first_pc == '\0' ) first_pc = PC;
            /* if dir did not end in '/' then put in a provisional one */
                if( *(_mbsdec(pathstart,path)) == first_pc )
                    path--;
                else
                    *path = first_pc;
        }
    }

    /* if no path separator was specified thus far then pick a default */
    if( first_pc == '\0' ) first_pc = PC;
    if( fname != NULL ) {
            ch = _mbsnextc( fname );
            if( pickup(ch,&first_pc) != first_pc  &&  *path == first_pc )
                path++;

        while (*fname != '\0')
        {
        //do {
                ch = pickup( _mbsnextc(fname), &first_pc );
                _mbvtop( ch, path );
                path[_mbclen(path)] = '\0';
                path = _mbsinc( path );
                fname = _mbsinc( fname );
        } //while( *fname != '\0' );
    } else {
        if( *path == first_pc ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            while( *ext != '\0' ) *path++ = *ext++;     /* OK for MBCS */
        }
    }
    *path = '\0';
}

#endif

/****************************************************************************
*
* Description:  Implementation of fullpath() - returns fully qualified
*               pathname of a file.
*
****************************************************************************/

#define _WILL_FIT( c )  if(( (c) + 1 ) > size ) {       \
                            __set_errno( ERANGE );      \
                            return( NULL );             \
                        }                               \
                        size -= (c);

#ifdef __UNIX__
#define _IS_SLASH( c )  ((c) == '/')
#else
#define _IS_SLASH( c )  (( (c) == '/' ) || ( (c) == '\\' ))
#endif

#if !defined( __NT__ ) && !defined( __NETWARE__ ) && !defined( __UNIX__ )
#pragma on (check_stack);
#endif

#ifdef __NETWARE__
extern char *ConvertNameToFullPath( const char *, char * );
#endif

#if defined(__QNX__)
static char *__qnx_fullpath(char *fullpath, const char *path)
{
    struct {
            struct _io_open _io_open;
            char  m[_QNX_PATH_MAX];
    } msg;
    int             fd;

    msg._io_open.oflag = _IO_HNDL_INFO;
    fd = __resolve_net( _IO_HANDLE, 1, &msg._io_open, path, 0, fullpath );
    if( fd != -1) {
        close(fd);
    } else if (errno != ENOENT) {
        return 0;
    } else {
        __resolve_net( 0, 0, &msg._io_open, path, 0, fullpath );
    }
    return fullpath;
}
#endif

char *_sys_fullpath( char *buff, const char *path, size_t size )
/*********************************************************************/
{

#if defined(__NT__)
    char *         filepart;
    DWORD               rc;

    if( stricmp( path, "con" ) == 0 ) {
        _WILL_FIT( 3 );
        return( strcpy( buff, "con" ) );
    }

    /*** Get the full pathname ***/
    rc = GetFullPathNameA( path, (DWORD)size, buff, &filepart );
    // If the buffer is too small, the return value is the size of
    // the buffer, in TCHARs, required to hold the path.
    // If the function fails, the return value is zero. To get extended error
    // information, call GetLastError.
    if( (rc == 0) || (rc > (DWORD)size) ) {
        __set_errno( ERANGE );
        return( NULL );
    }

    return( buff );
#elif defined(__WARP__)
    APIRET      rc;
    char        root[4];    /* SBCS: room for drive, ':', '\\', and null */

    if (isalpha( path[0] ) && ( path[1] == ':' )
            && ( path[2] == '\\' ) )
    {
        int i;
        i = strlen( path );
        _WILL_FIT(i);
        strcpy( buff, path );
        return( buff );
    }

    /*
     * Check for x:filename.ext when drive x doesn't exist.  In this
     * case, return x:\filename.ext, not NULL, to be consistent with
     * MS and with the NT version of _fullpath.
     */
    if( isalpha( path[0] )  &&  path[1] == ':' ) {
        /*** We got this far, so path can't start with letter:\ ***/
        root[0] = (char) path[0];
        root[1] = ':';
        root[2] = '\\';
        root[3] = '\0';
        rc = DosQueryPathInfo( root, FIL_QUERYFULLNAME, buff, size );
        if( rc != NO_ERROR ) {
            /*** Drive does not exist; return x:\filename.ext ***/
            _WILL_FIT( strlen( &path[2] ) + 3 );
            buff[0] = root[0];
            buff[1] = ':';
            buff[2] = '\\';
            strcpy( &buff[3], &path[2] );
            return( buff );
        }
    }

    rc = DosQueryPathInfo( (PSZ)path, FIL_QUERYFULLNAME, buff, size );
    if( rc != 0 ) {
        __set_errno_dos( rc );
        return( NULL );
    }
    return( buff );
#elif defined(__QNX__) || defined( __NETWARE__ )
    size_t len;
    char temp_dir[_MAX_PATH];

    #if defined(__NETWARE__)
        if( ConvertNameToFullPath( path, temp_dir ) != 0 ) {
            return( NULL );
        }
    #else
        if( __qnx_fullpath( temp_dir, path ) == NULL ) {
            return( NULL );
        }
    #endif
    len = strlen( temp_dir );
    if( len >= size ) {
        __set_errno( ERANGE );
        return( NULL );
    }
    return( strcpy( buff, temp_dir ) );
#elif defined(__UNIX__)
    const char  *p;
    char        *q;
    size_t      len;
    char        curr_dir[_MAX_PATH];

    p = path;
    q = buff;
    if( ! _IS_SLASH( p[0] ) ) {
        if( getcwd( curr_dir, sizeof(curr_dir) ) == NULL ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        len = strlen( curr_dir );
        _WILL_FIT( len );
        strcpy( q, curr_dir );
        q += len;
        if( q[-1] != '/' ) {
            _WILL_FIT( 1 );
            *(q++) = '/';
        }
        for(;;) {
            if( p[0] == '\0' ) break;
            if( p[0] != '.' ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;
            if( _IS_SLASH( p[0] ) ) {
                /* ignore "./" in directory specs */
                if( ! _IS_SLASH( q[-1] ) ) {
                    *q++ = '/';
                }
                ++p;
                continue;
            }
            if( p[0] == '\0' ) break;
            if( p[0] == '.' && _IS_SLASH( p[1] ) ) {
                /* go up a directory for a "../" */
                p += 2;
                if( ! _IS_SLASH( q[-1] ) ) {
                    return( NULL );
                }
                q -= 2;
                for(;;) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( _IS_SLASH( *q ) ) break;
                    --q;
                }
                ++q;
                *q = '\0';
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = '.';
        }
        *q = '\0';
    } else {
        len = strlen( p );
        _WILL_FIT( len );
        strcpy( q, p );
    }
    return( buff );
#else
    const char *   p;
    char *         q;
    size_t              len;
    unsigned            path_drive_idx;
    char                curr_dir[_MAX_PATH];

    p = path;
    q = buff;
    _WILL_FIT( 2 );
    if( isalpha( p[0] ) && p[1] == ':' ) {
        path_drive_idx = ( tolower( p[0] ) - 'a' ) + 1;
        q[0] = p[0];
        q[1] = p[1];
        p += 2;
    } else {
  #if defined(__OS2__)
        ULONG   drive_map;
        OS_UINT os2_drive;

        if( DosQCurDisk( &os2_drive, &drive_map ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        path_drive_idx = os2_drive;
  #else
        path_drive_idx = TinyGetCurrDrive() + 1;
  #endif
        q[0] = 'A' + ( path_drive_idx - 1 );
        q[1] = ':';
    }
    q += 2;
    if( ! _IS_SLASH( p[0] ) ) {
  #if defined(__OS2__)
        OS_UINT dir_len = sizeof( curr_dir );

        if( DosQCurDir( path_drive_idx, curr_dir, &dir_len ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #else
        tiny_ret_t rc;

        rc = TinyGetCWDir( curr_dir, path_drive_idx );
        if( TINY_ERROR( rc ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #endif
        len = strlen( curr_dir );
        if( curr_dir[0] != '\\' ) {
            _WILL_FIT( 1 );
            *(q++) = '\\';
        }
        _WILL_FIT( len );
        strcpy( q, curr_dir );
        q += len;
        if( q[-1] != '\\' ) {
            _WILL_FIT( 1 );
            *(q++) = '\\';
        }
        for(;;) {
            if( p[0] == '\0' ) break;
            if( p[0] != '.' ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;     // at least '.'
            if( _IS_SLASH( p[0] ) ) {
                /* ignore "./" in directory specs */
                if( ! _IS_SLASH( q[-1] ) ) {            /* 14-jan-93 */
                    *q++ = '\\';
                }
                ++p;
                continue;
            }
            if( p[0] == '\0' ) break;
            if( p[0] == '.' ) {  /* .. */
                ++p;
                if( _IS_SLASH( p[0] ) ){ /* "../" */
                    ++p;
                }
                if( ! _IS_SLASH( q[-1] ) ) {
                    return( NULL );
                }
                q -= 2;
                for(;;) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( _IS_SLASH( *q ) ) break;
                    if( *q == ':' ) {
                        ++q;
                        *q = '\\';
                        break;
                    }
                    --q;
                }
                ++q;
                *q = '\0';
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = '.';
        }
        *q = '\0';
    } else {
        len = strlen( p );
        _WILL_FIT( len );
        strcpy( q, p );
    }
    /* force to all backslashes */
    for( q = buff; *q; ++q ) {
        if( *q == '/' ) {
            *q = '\\';
        }
    }
    return( buff );
#endif
}

char *_fullpath( char *buff, const char *path, size_t size )
/**********************************************************/
{
    char *ptr = NULL;

    if( buff == NULL ) {
        size = _MAX_PATH;
        ptr = malloc( size );
        if( ptr == NULL ) __set_errno( ENOMEM );
        buff = ptr;
    }
    if( buff != NULL ) {
        buff[0] = '\0';
        if( path == NULL || path[0] == '\0' ) {
            buff = getcwd( buff, (int)size );
        } else {
            buff = _sys_fullpath( buff, path, size );
        }
        if( buff == NULL ) {
            if( ptr != NULL ) free( ptr );
        }
    }
    return buff;
}

/****************************************************************************
*
* Description:  Implementation of strlwr(). 
*
****************************************************************************/

 char *strlwr( char *str ) {
    char    *p;
    unsigned char   c;

    p = str;
    while( (c = *p) ) {
        c -= 'A';
        if( c <= 'Z' - 'A' ) {
            c += 'a';
            *p = c;
        }
        ++p;
    }
    return( str );
}
/****************************************************************************
*
* Description:  Implementation of strupr().
*
****************************************************************************/

 char *strupr( char *str ) {
    char    *p;
    unsigned char   c;

    p = str;
    while( (c = *p) ) {
        c -= 'a';
        if( c <= 'z' - 'a' ) {
            c += 'A';
            *p = c;
        }
        ++p;
    }
    return( str );
}

/****************************************************************************
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

int memicmp( const void *in_s1, const void *in_s2, size_t len )
    {
        const unsigned char *   s1 = (const unsigned char *)in_s1;
        const unsigned char *   s2 = (const unsigned char *)in_s2;
        unsigned char           c1;
        unsigned char           c2;

        for( ; len; --len )  {
            c1 = *s1;
            c2 = *s2;
            if( c1 >= 'A'  &&  c1 <= 'Z' )  c1 += 'a' - 'A';
            if( c2 >= 'A'  &&  c2 <= 'Z' )  c2 += 'a' - 'A';
            if( c1 != c2 ) return( c1 - c2 );
            ++s1;
            ++s2;
        }
        return( 0 );    /* both operands are equal */
    }

/****************************************************************************
*
* Description:  Implementation of tell(). 
*
****************************************************************************/

off_t tell( int handle )
{
    return( lseek( handle, 0L, SEEK_CUR ) );
}

/****************************************************************************
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

char *strnset( char *str, int c, size_t len )
    {
        char *p;

        for( p = str; len; --len ) {
            if( *p == '\0' ) break;
            *p++ = (char)c;
        }
        return( str );
    }

/****************************************************************************
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

char *strrev( char *str ) {  /* reverse characters in string */
        char       *p1;
        char       *p2;
        char       c1;
        char       c2;

        p1 = str;
        p2 = p1 + strlen( p1 ) - 1;
        while( p1 < p2 ) {
            c1 = *p1;
            c2 = *p2;
            *p1 = c2;
            *p2 = c1;
            ++p1;
            --p2;
        }
        return( str );
}

/****************************************************************************
*
* Description:  Implements POSIX filelength() function
*
****************************************************************************/

long filelength( int handle )
{
    long                current_posn, file_len;

    current_posn = lseek( handle, 0L, SEEK_CUR );
    if( current_posn == -1L )
    {
        return( -1L );
    }
    file_len = lseek( handle, 0L, SEEK_END );
    lseek( handle, current_posn, SEEK_SET );

    return( file_len );
}

/****************************************************************************
*
* Description:  Implementation of eof().
*
****************************************************************************/

int eof( int handle )         /* determine if at EOF */
{
    off_t   current_posn, file_len;

    file_len = filelength( handle );
    if( file_len == -1L )
        return( -1 );
    current_posn = tell( handle );
    if( current_posn == -1L )
        return( -1 );
    if( current_posn == file_len )
        return( 1 );
    return( 0 );
}

#endif /* !_MSC_VER */

/****************************************************************************
*
* Description:  Implementation of _cmdname().
*
****************************************************************************/

/* NOTE: This file isn't used for QNX. It's got its own version. */

#ifdef __APPLE__

#include <mach-o/dyld.h>

/* No procfs on Darwin, have to use special API */ 

char *_cmdname( char *name )
{
    uint32_t    len = 4096;

    _NSGetExecutablePath( name, &len );
    return( name );
}

#elif defined( __BSD__ )

#include <sys/sysctl.h>

char *_cmdname( char *name )
{
    int     mib[4];
    size_t  cb;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    cb = PATH_MAX;
    sysctl( mib, 4, name, &cb, NULL, 0 );
    return( name );
}

#elif defined( __UNIX__ )

char *_cmdname( char *name )
{
    int save_errno = errno;
    int result = readlink( "/proc/self/exe", name, PATH_MAX );
    if( result == -1 ) {
        /* try another way for BSD */
        result = readlink( "/proc/curproc/file", name, PATH_MAX );
    }
    errno = save_errno;

    /* fall back to argv[0] if readlink doesn't work */
    if( result == -1 || result == PATH_MAX )
        return( strcpy( name, _argv[0] ) );

    /* readlink does not add a NUL so we need to do it ourselves */
    name[result] = '\0';
    return( name );
}

#elif defined( _MSC_VER )

char *_cmdname( char *name )
{
    char    *pgm;

    _get_pgmptr( &pgm );
    return( strcpy( name, pgm ) );
}

#else

char *_cmdname( char *name )
{
    return( strcpy( name, _argv[0] ) );
}

#endif

/****************************************************************************
*
* Description:  Implementation of getcmd() and _bgetcmd().
*
****************************************************************************/

#if defined( _MSC_VER )

int _bgetcmd( char *buffer, int len )
{
    int         cmdlen;
    char        *cmd;
    char        *tmp;

    if( ( buffer != NULL ) && ( len > 0 ) )
        *buffer = '\0';

    cmd = GetCommandLine();
    if( *cmd == '"' ) {
        cmd++;
        while( *cmd && *cmd != '"' ) {
            cmd++;
        }
        if( *cmd ) {
            cmd++;
        }
    } else {
        while( *cmd && *cmd != ' ' && *cmd != '\t' ) {
            ++cmd;
        }
    }

    while( *cmd == ' ' || *cmd == '\t' )
        ++cmd;

    for( cmdlen = 0, tmp = cmd; *tmp; ++tmp, ++cmdlen )
        ;

    if( !buffer || (len <= 0) )
        return( cmdlen );

    len--;
    len = (len < cmdlen) ? len : cmdlen;

    while( len ) {
        *buffer++ = *cmd++;
        --len;
    }
    buffer[len] = '\0';

    return( cmdlen );
}


char *getcmd( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}

#else

int (_bgetcmd)( char *buffer, int len )
{
    int     total;
    int     i;
    char    *word;
    char    *p     = NULL;
    char    **argv = &_argv[1];

    if( ( buffer != NULL ) && ( len > 0 ) ) {
        p  = buffer;
        *p = '\0';
        --len;          // reserve space for NULL byte
    }

    /* create approximation of original command line */
    for( word = *argv++, i = 0, total = 0; word != '\0'; word = *argv++ ) {
        i      = (int)strlen( word );
        total += i;

        if( p != NULL ) {
            if( i >= len ) {
                strncpy( p, word, len );
                p[len] = '\0';
                p      = NULL;
                len    = 0;
            } else {
                strcpy( p, word );
                p   += i;
                len -= i;
            }
        }

        /* account for at least one space separating arguments */
        if( *argv != NULL ) {
            if( p != NULL ) {
                *p++ = ' ';
                --len;
            }
            ++total;
        }
    }
    return( total );
}

char *(getcmd)( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}

#endif

/****************************************************************************
*
* Description:  Implementation of spawn.. functions for Unix.
*
****************************************************************************/

#ifdef __UNIX__

int spawnvp( int mode, const char *cmd, const char * const *args )
{
    pid_t       pid;
    int         status;

    pid = fork();
    if( pid == -1 )
        return( -1 );
    if( pid == 0 ) {
        execvp( cmd, (char * const *)args );
        _exit( 127 );
    }
    if( waitpid( pid, &status, 0 ) == -1 ) {
        status = -1;
    }
    return( status );
}

int spawnlp( int mode, const char *path, const char *cmd, ... )
{
    va_list     ap;
    const char  *arg;
    const char  *args[8];
    int         i;

    va_start( ap, cmd );
    args[0] = cmd;
    i = 1;
    while( (arg = va_arg( ap, const char * )) != NULL ) {
        args[i++] = arg;
    }
    args[i] = NULL;
    va_end( ap );
    return( spawnvp( mode, cmd, (const char * const *)args ) );
}
#endif

/****************************************************************************
*
* Description:  This function searches for the specified file in the
*               1) current directory or, failing that,
*               2) the paths listed in the specified environment variable
*               until it finds the first occurrence of the file.
*
****************************************************************************/

#ifndef _MSC_VER

#if defined(__UNIX__)
        #define PATH_SEPARATOR '/'
        #define LIST_SEPARATOR ':'
#else
        #define PATH_SEPARATOR '\\'
        #define LIST_SEPARATOR ';'
#endif

void _searchenv( const char *name, const char *env_var, char *buffer )
{
    char        *p, *p2;
    int         prev_errno;
    size_t      len;

    prev_errno = errno;
    if( access( name, F_OK ) == 0 ) {
        p = buffer;                                 /* JBS 90/3/30 */
        len = 0;                                    /* JBS 04/1/06 */
        for( ;; ) {
            if( name[0] == PATH_SEPARATOR ) break;
            if( name[0] == '.' ) break;
#ifndef __UNIX__
            if( name[0] == '/' ) break;
            if( (name[0] != '\0') && (name[1] == ':') ) break;
#endif
            getcwd( buffer, _MAX_PATH );
            len = strlen( buffer );
            p = &buffer[ len ];
            if( p[-1] != PATH_SEPARATOR ) {
                if( len < (_MAX_PATH - 1) ) {
                    *p++ = PATH_SEPARATOR;
                    len++;
                }
            }
            break;
        }
        *p = '\0';
        strncat( p, name, (_MAX_PATH - 1) - len );
        return;
    }
    p = getenv( env_var );
    if( p != NULL ) {
        for( ;; ) {
            if( *p == '\0' ) break;
            p2 = buffer;
            len = 0;                                /* JBS 04/1/06 */
            while( *p ) {
                if( *p == LIST_SEPARATOR ) break;
                if( *p != '"' ) {
                    if( len < (_MAX_PATH-1) ) {
                        *p2++ = *p; /* JBS 00/9/29 */
                        len++;
                    }
                }
                p++;
            }
            /* check for zero-length prefix which represents CWD */
            if( p2 != buffer ) {                    /* JBS 90/3/30 */
                if( p2[-1] != PATH_SEPARATOR
#ifndef __UNIX__
                    &&  p2[-1] != '/'
                    &&  p2[-1] != ':'
#endif
                    ) {
                    if( len < (_MAX_PATH - 1) ) {
                        *p2++ = PATH_SEPARATOR;
                        len++;
                    }
                }
                *p2 = '\0';
                len += strlen( name );/* JBS 04/12/23 */
                if( len < _MAX_PATH ) {
                    strcat( p2, name );
                    /* check to see if file exists */
                    if( access( buffer, 0 ) == 0 ) {
                        __set_errno( prev_errno );
                        return;
                    }
                }
            }
            if( *p == '\0' ) break;
            ++p;
        }
    }
    buffer[0] = '\0';
}

#endif /* !_MSC_VER */

/****************************************************************************
*
* Description:  POSIX fnmatch.
*
****************************************************************************/

#ifdef _MSC_VER

/* Implementation note: On non-UNIX systems, backslashes in the string
 * (but not in the pattern) are considered to be path separators and
 * identical to forward slashes when FNM_PATHNAME is set.
 */

#ifdef __UNIX__
  #define IS_PATH_SEP(c)   (c == '/')
#else
  #define IS_PATH_SEP(c)   (c == '/' || c == '\\')
#endif

static const struct my_wctypes {
    const char  *name;
    int         mask;
} my_wctypes[] = {
    { "alnum", _ALPHA | _DIGIT },
    { "alpha", _ALPHA },
    { "cntrl", _CONTROL },
    { "digit", _DIGIT },
    { "graph", _PUNCT | _ALPHA | _DIGIT },
    { "lower", _LOWER },
    { "print", _BLANK | _PUNCT | _ALPHA | _DIGIT },
    { "punct", _PUNCT },
    { "space", _SPACE },
    { "upper", _UPPER },
    { "xdigit", _HEX }
};

#define WCTYPES_SIZE (sizeof( my_wctypes ) / sizeof( my_wctypes[0] ))

static int my_wctype( const char *name )
{
    int i;

    for( i = 0; i < WCTYPES_SIZE; ++i ) {
        if( strcmp( my_wctypes[i].name, name ) == 0 ) {
            return( my_wctypes[i].mask );
        }
    }
    return( 0 );
}

static int icase( int ch, int flags )
{
    if( flags & FNM_IGNORECASE ) {
        return( tolower( ch ) );
    } else {
        return( ch );
    }
}

/* Maximum length of character class name. 
 * The longest is currently 'xdigit' (6 chars).
 */
#define CCL_NAME_MAX    8

/* Note: Using wctype()/iswctype() may seem odd, but that way we can avoid 
 * hardcoded character class lists.
 */
static int sub_bracket( const char *p, int c )
{
    const char      *s = p;
    char            sname[CCL_NAME_MAX + 1];
    int             i;
    int             type;

    switch( *++p ) {
    case ':':
        ++p;
        for( i = 0; i < CCL_NAME_MAX; i++ ) {
            if( !isalpha(*p ) )
                break;
            sname[i] = *p++;
        }
        sname[i] = '\0';
        if( *p++ != ':' )
            return( 0 );
        if( *p++ != ']' )
            return( 0 );
        type = my_wctype( sname );
        if( type ) {
            return( iswctype( (wint_t)c, (wctype_t)type ) ? (int)( p - s ) : (int)( s - p ) );
        }
        return( 0 );
    case '=':
        return( 0 );
    case '.':
        return( 0 );
    default:
        return( 0 );
    }
}

static const char *cclass_match( const char *patt, int c )
{
    int         ok = 0;
    int         lc = 0;
    int         state = 0;
    int         invert = 0;
    int         sb;

    /* Meaning of '^' is unspecified in POSIX - consider it equal to '!' */
    if( *patt == '!' || *patt == '^' ) {
        invert = 1;
        ++patt;
    }
    while( *patt ) {
        if( *patt == ']' )
            return( ok ^ invert ? patt + 1 : NULL );

        if( *patt == '[' ) {
             sb = sub_bracket( patt, c );
             if( sb < 0 ) {
                 patt -= sb;
                 ok = 0;
                 continue;
             } else if( sb > 0 ) {
                 patt += sb;
                 ok = 1;
                 continue;
             }
        }

        switch( state ) {
        case 0:
            if( *patt == '\\' )
                ++patt;
            if( *patt == c )
                ok = 1;
            lc = (int)*patt++;
            state = 1;
            break;
        case 1:
            if( *patt == '-' ) {
                state = 2;
                ++patt;
                break;
            }
            state = 0;
            break;
        case 2: 
            if( *patt == '\\' )
                ++patt;
            if( lc <= c && c <= *patt )
                ok = 1;
            ++patt;     
            state = 0;
            break;
        default:
            return( NULL );
        }
    }
    return( NULL );
}

int   fnmatch( const char *patt, const char *s, int flags )
/*********************************************************/
{
    int         c, cl;
    const char  *start = s;

    while( (c = icase( *patt++, flags )) != '\0' ) {
        switch( c ) {
        case '?':
            if( flags & FNM_PATHNAME && IS_PATH_SEP( *s ) )
                return( FNM_NOMATCH );
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            ++s;
            break;
        case '[':
            if( flags & FNM_PATHNAME && IS_PATH_SEP( *s ) )
                return( FNM_NOMATCH );
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            patt = cclass_match( patt, *s );
            if( patt == NULL )
                return( FNM_NOMATCH );
            ++s;
            break;
        case '*':
            if( *s == '\0' )
                return( 0 );
            if( flags & FNM_PATHNAME && ( *patt == '/' ) ) {
                while( *s && !IS_PATH_SEP( *s ) )
                    ++s;
                break;
            }
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            if( *patt == '\0' ) {
                /* Shortcut - don't examine every remaining character. */
                if( flags & FNM_PATHNAME ) {
                    if( flags & FNM_LEADING_DIR || !strchr( s, '/' ) )
                        return( 0 );
                    else
                        return( FNM_NOMATCH );
                } else {
                    return( 0 );
                }
            }
            while( (cl = icase( *s, flags )) != '\0' ) {
                if( !fnmatch( patt, s, flags & ~FNM_PERIOD ) )
                    return( 0 );
                if( flags & FNM_PATHNAME && IS_PATH_SEP( cl ) ) {
                    start = s + 1;
                    break;
                }
                ++s;
            }
            return( FNM_NOMATCH );
        case '\\':
            if( !( flags & FNM_NOESCAPE ) ) {
                c = icase( *patt++, flags );
            }
            /* Fall through */
        default:
            if( IS_PATH_SEP( *s ) )
                start = s + 1;
            cl = icase( *s++, flags );
#ifndef __UNIX__
            if( flags & FNM_PATHNAME && cl == '\\' )
                cl = '/';
#endif
            if( c != cl )
                return( FNM_NOMATCH );
        }
    }
    if( flags & FNM_LEADING_DIR && IS_PATH_SEP( *s ) )
        return( 0 );
    return( *s ? FNM_NOMATCH : 0 );
}

#endif

/****************************************************************************
*
* Description:  Determine resource language from system environment.
*
****************************************************************************/

res_language_enumeration _WResLanguage(void)
{
    return( RLE_ENGLISH );
}


#ifdef _MSC_VER

int setenv( const char *name, const char *newvalue, int overwrite )
/*****************************************************************/
{
    char    *buff;
    size_t  len;

    len = strlen( name ) + strlen( newvalue ) + 16;
    buff = malloc( len );
    overwrite = overwrite;
    sprintf( buff, "%s=%s", name, newvalue );
    putenv( buff );
    free( buff );
    return( 0 );
}

int unsetenv( const char *name )
/******************************/
{
    char    *buff;
    size_t  len;

    len = strlen( name ) + 16;
    buff = malloc( len );
    sprintf( buff, "%s=", name );
    putenv( buff );
    free( buff );
    return( 0 );
}

void _dos_getdrive( unsigned *drive )
{
    char        buff[MAX_PATH];

    GetCurrentDirectory( sizeof( buff ), buff );
    *drive = tolower( buff[0] ) - 'a'+1;
}

void _dos_setdrive( unsigned drivenum, unsigned *drives )
{
    char        dir[4];

    dir[0] = (char)drivenum + 'a' - 1;
    dir[1] = ':';
    dir[2] = '.';
    dir[3] = 0;

    SetCurrentDirectory( dir );
    *drives = (unsigned)-1;
}

unsigned _dos_getfileattr( const char *path, unsigned *attribute )
{
    HANDLE              h;
    WIN32_FIND_DATA     ffd;

    h = FindFirstFile( (LPTSTR)path, &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        return( __set_errno( ENOENT ) );
    }
    *attribute = ffd.dwFileAttributes;
    FindClose( h );
    return( 0 );
}

unsigned _dos_setfileattr( const char *path, unsigned attribute )
{
    if( attribute == 0 )
        attribute = FILE_ATTRIBUTE_NORMAL;

    if( !SetFileAttributes( (LPTSTR) path, attribute ) ) {
        __set_errno( ENOENT );
    }
    return( 0 );
}

typedef struct __nt_dta {
    HANDLE      hndl;
    DWORD       attr;
} __nt_dta;

#define DIR_HANDLE_OF(__dirp)   (((__nt_dta *)(__dirp)->d_dta)->hndl)
#define DIR_ATTR_OF(__dirp)     (((__nt_dta *)(__dirp)->d_dta)->attr)
#define FIND_HANDLE_OF(__find)  (((__nt_dta *)(__find)->reserved)->hndl)
#define FIND_ATTR_OF(__find)    (((__nt_dta *)(__find)->reserved)->attr)

#define GET_CHAR(p)       _mbsnextc((const unsigned char *)p)
#define NEXT_CHAR_PTR(p)  _mbsinc((const unsigned char *)p)

#define _DIR_ISFIRST            0
#define _DIR_NOTFIRST           1
#define _DIR_CLOSED             2

#define OPENMODE_ACCESS_MASK    0x0007
#define OPENMODE_SHARE_MASK     0x0070

#define OPENMODE_ACCESS_RDONLY  0x0000

#define OPENMODE_DENY_COMPAT    0x0000
#define OPENMODE_DENY_ALL       0x0010
#define OPENMODE_DENY_WRITE     0x0020
#define OPENMODE_DENY_READ      0x0030
#define OPENMODE_DENY_NONE      0x0040

static void __GetNTCreateAttr( int mode, LPDWORD desired_access, LPDWORD attr )
{
    if( mode & _A_RDONLY ) {
        *desired_access = GENERIC_READ;
        *attr = FILE_ATTRIBUTE_READONLY;
    } else {
        *desired_access = GENERIC_READ | GENERIC_WRITE;
        *attr = FILE_ATTRIBUTE_NORMAL;
    }
    if( mode & _A_HIDDEN ) {
        *attr |= FILE_ATTRIBUTE_HIDDEN;
    }
    if( mode & _A_SYSTEM ) {
        *attr |= FILE_ATTRIBUTE_SYSTEM;
    }
}

void __GetNTAccessAttr( int rwmode, LPDWORD desired_access, LPDWORD attr )
{
    if( rwmode == O_RDWR ) {
        *desired_access = GENERIC_READ | GENERIC_WRITE;
        *attr = FILE_ATTRIBUTE_NORMAL;
    } else if( rwmode == O_WRONLY ) {
        *desired_access = GENERIC_WRITE;
        *attr = FILE_ATTRIBUTE_NORMAL;
    } else {
        *desired_access = GENERIC_READ;
        *attr = FILE_ATTRIBUTE_READONLY;
    }
}

void __GetNTShareAttr( int mode, LPDWORD share_mode )
{
    int share;
    int rwmode;

    share  = mode & OPENMODE_SHARE_MASK;
    rwmode = mode & OPENMODE_ACCESS_MASK;

    switch( share ) {
    case OPENMODE_DENY_COMPAT:
        /*
         * Always allow reopening for read.  Since we don't want the same
         * file opened twice for writing, only allow the file to be opened
         * for writing hereafter if we're opening it now in read-only mode.
         *                      -- M. Hildebrand, 14-jun-96
         */
        *share_mode = FILE_SHARE_READ;  /* can always open again for read */
        if( rwmode == OPENMODE_ACCESS_RDONLY ) {
            *share_mode |= FILE_SHARE_WRITE;    /* can open again for write */
        }
        break;
    case OPENMODE_DENY_ALL:
        *share_mode = 0;
        break;
    case OPENMODE_DENY_READ:
        *share_mode = FILE_SHARE_WRITE;
        break;
    case OPENMODE_DENY_WRITE:
        *share_mode = FILE_SHARE_READ;
        break;
    case OPENMODE_DENY_NONE:
        *share_mode = FILE_SHARE_READ|FILE_SHARE_WRITE;
        break;
    }
}

static void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp )
/*******************************************************************************/
{
    FILETIME local_ft;

    DosDateTimeToFileTime( d, t, &local_ft );
    LocalFileTimeToFileTime( &local_ft, NT_stamp );
}

static void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t )
/*********************************************************************************/
{
    FILETIME local_ft;

    FileTimeToLocalFileTime( NT_stamp, &local_ft );
    FileTimeToDosDateTime( &local_ft, d, t );
}

static int is_directory( const char *name )
/*****************************************/
{
    unsigned    curr_ch;
    unsigned    prev_ch;

    curr_ch = '\0';
    for(;;) {
        prev_ch = curr_ch;
        curr_ch = GET_CHAR( name );
        if( curr_ch == '\0' ) {
            if( prev_ch == '\\' || prev_ch == '/' || prev_ch == ':' ){
                /* directory, need add "*.*" */
                return( 2 );
            }
            if( prev_ch == '.' ){
                /* directory, need add "\\*.*" */
                return( 1 );
            }
            /* without wildcards maybe file or directory, need next check */
            /* need add "\\*.*" if directory */
            return( 0 );
        }
        if( curr_ch == '*' )
            break;
        if( curr_ch == '?' )
            break;
        name = (const char *)NEXT_CHAR_PTR( name );
    }
    /* with wildcard must be file */
    return( -1 );
}

static void get_nt_dir_info( DIR *dirp, LPWIN32_FIND_DATA ffd )
/*************************************************************/
{
    __MakeDOSDT( &ffd->ftLastWriteTime, &dirp->d_date, &dirp->d_time );
    dirp->d_attr = (char)ffd->dwFileAttributes;
    dirp->d_size = ffd->nFileSizeLow;
    strncpy( dirp->d_name, ffd->cFileName, NAME_MAX );
    dirp->d_name[NAME_MAX] = 0;
}

static DIR *__opendir( const char *dirname, DIR *dirp )
/*****************************************************/
{
    WIN32_FIND_DATA     ffd;
    HANDLE              h;

    if( dirp->d_first != _DIR_CLOSED ) {
        FindClose( DIR_HANDLE_OF( dirp ) );
        dirp->d_first = _DIR_CLOSED;
    }
    h = FindFirstFileA( dirname, &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        __set_errno( ENOENT );
        return( NULL );
    }
    DIR_HANDLE_OF( dirp ) = h;
    get_nt_dir_info( dirp, &ffd );
    dirp->d_first = _DIR_ISFIRST;
    return( dirp );
}

DIR *opendir( const char *dirname )
/*********************************/
{
    DIR         tmp;
    DIR         *dirp;
    int         i;
    char        pathname[MAX_PATH+6];

    tmp.d_attr = _A_SUBDIR;               /* assume sub-directory */
    tmp.d_first = _DIR_CLOSED;
    i = is_directory( dirname );
    if( i <= 0 ) {
        if( __opendir( pathname, &tmp ) == NULL ) {
            return( NULL );
        }
    }
    if( i >= 0 && (tmp.d_attr & _A_SUBDIR) ) {
        size_t          len;

        /* directory, add wildcard */
        len = strlen( dirname );
        memcpy( pathname, dirname, len );
        if( i < 2 ) {
            pathname[len++] = '\\';
        }
        strcpy( &pathname[len], "*.*" );
        if( __opendir( pathname, &tmp ) == NULL ) {
            return( NULL );
        }
        dirname = pathname;
    }
    dirp = malloc( sizeof( DIR ) );
    if( dirp == NULL ) {
        FindClose( DIR_HANDLE_OF( &tmp ) );
        __set_errno( ENOMEM );
        return( NULL );
    }
    tmp.d_openpath = strdup( dirname );
    *dirp = tmp;
    return( dirp );
}

struct dirent *readdir( DIR *dirp )
/*********************************/
{
    WIN32_FIND_DATA     ffd;
    HANDLE              h;

    if( dirp == NULL || dirp->d_first == _DIR_CLOSED )
        return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {
        dirp->d_first = _DIR_NOTFIRST;
    } else {
        h = DIR_HANDLE_OF( dirp );
        if( !FindNextFileA( h, &ffd ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        get_nt_dir_info( dirp, &ffd );
    }
    return( dirp );
}

int closedir( DIR *dirp )
/***********************/
{
    if( dirp == NULL || dirp->d_first == _DIR_CLOSED ) {
        return( __set_errno( ERANGE ) );
    }
    if( !FindClose( DIR_HANDLE_OF( dirp ) ) ) {
        return( -1 );
    }
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )
        free( dirp->d_openpath );
    free( dirp );
    return( 0 );
}

unsigned _dos_open( const char *name, unsigned mode, HANDLE *h )
{
    HANDLE      handle;
    DWORD       rwmode, share_mode;
    DWORD       desired_access, attr;

    rwmode = mode & OPENMODE_ACCESS_MASK;

    __GetNTAccessAttr( rwmode, &desired_access, &attr );
    __GetNTShareAttr( mode & (OPENMODE_SHARE_MASK|OPENMODE_ACCESS_MASK), &share_mode );
    handle = CreateFile( (LPTSTR) name, desired_access, share_mode, 0, OPEN_EXISTING, attr, NULL );
    if( handle == INVALID_HANDLE_VALUE ) {
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    *h = handle;
    return( 0 );
}

unsigned _dos_creat( const char *name, unsigned mode, HANDLE *h )
{
    HANDLE      handle;
    DWORD       desired_access;
    DWORD       attr;

    __GetNTCreateAttr( mode, &desired_access, &attr );
    handle = CreateFile( (LPTSTR) name, desired_access, 0, 0, CREATE_ALWAYS, attr, NULL );
    if( handle == INVALID_HANDLE_VALUE ) {
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    *h = handle;
    return( 0 );
}

unsigned _dos_close( HANDLE h )
{
    if( !CloseHandle( h ) ) {
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    return( 0 );
}

unsigned _dos_getftime( HANDLE h, unsigned *date, unsigned *time )
{
    FILETIME        ctime, atime, wtime;
    unsigned short  d, t;

    if( GetFileTime( h, &ctime, &atime, &wtime ) ) {
        __MakeDOSDT( &wtime, &d, &t );
        *date = d;
        *time = t;
        return( 0 );
    }
    __set_errno( ENOENT );
    return( (unsigned)-1 );
}

unsigned _dos_setftime( HANDLE h, unsigned date, unsigned time )
{
    FILETIME    ctime, atime, wtime;

    if( GetFileTime( h, &ctime, &atime, &wtime ) ) {
        __FromDOSDT( (unsigned short)date, (unsigned short)time, &wtime );
        if( SetFileTime( h, &ctime, &wtime, &wtime ) ) {
            return( 0 );
        }
    }
    __set_errno( ENOENT );
    return( (unsigned)-1 );
}

unsigned _dos_read( HANDLE h, void *buffer, unsigned count, unsigned *bytes )
{
    if( !ReadFile( h, buffer, count, (LPDWORD)bytes, NULL ) ) {
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    return( 0 );
}

unsigned _dos_write( HANDLE h, void const *buffer, unsigned count, unsigned *bytes )
{
    if( !WriteFile( h, buffer, count, (LPDWORD)bytes, NULL ) ) {
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    return( 0 );
}

static BOOL __NTFindNextFileWithAttr( HANDLE h, DWORD attr, LPWIN32_FIND_DATA ffd )
{
    for(;;) {
        if( ffd->dwFileAttributes == 0 ) {
            // Win95 seems to return 0 for the attributes sometimes?
            // In that case, treat as a normal file
            ffd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        }
        if( (attr & _A_HIDDEN) || (ffd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 ) {
            if( (attr & _A_SYSTEM) || (ffd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == 0 ) {
                if( (attr & _A_SUBDIR) || (ffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )  {
                    return ( TRUE );
                }
            }
        }
        if( !FindNextFileA( h, ffd ) ) {
            return( FALSE );
        }
    }
}

static void __GetNTDirInfo( struct dirent *dirp, LPWIN32_FIND_DATA ffd )
{
    __MakeDOSDT( &ffd->ftLastWriteTime, &dirp->d_date, &dirp->d_time );
    dirp->d_attr = (char)ffd->dwFileAttributes;
    dirp->d_size = ffd->nFileSizeLow;
    strncpy( dirp->d_name, ffd->cFileName, NAME_MAX );
    dirp->d_name[NAME_MAX] = 0;
}

unsigned _dos_findfirst( const char *path, unsigned attr, struct find_t *buf )
{
    HANDLE              h;
    int                 error;
    WIN32_FIND_DATA     ffd;

    h = FindFirstFile( (LPTSTR)path, &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        FIND_HANDLE_OF( buf ) = h;
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    if( !__NTFindNextFileWithAttr( h, attr, &ffd ) ) {
        error = GetLastError();
        FIND_HANDLE_OF( buf ) = INVALID_HANDLE_VALUE;
        FindClose( h );
        __set_errno( ENOENT );
        return( (unsigned)-1 );
    }
    FIND_HANDLE_OF( buf ) = h;
    FIND_ATTR_OF( buf ) = attr;
    __GetNTDirInfo( (struct dirent *)buf, &ffd );
    return( 0 );
}

unsigned _dos_findclose( struct find_t *buf )
{
    if( FIND_HANDLE_OF( buf ) != INVALID_HANDLE_VALUE ) {
        if( !FindClose( FIND_HANDLE_OF( buf ) ) ) {
            __set_errno( ENOENT );
            return( (unsigned)-1 );
        }
    }
    return( 0 );
}

char        *optarg;            // pointer to option argument
int         optind = 1;         // current argv[] index
int         optopt;             // currently processed chracter
int         opterr = 1;         // error output control flag

char        __altoptchar = '/'; // alternate option character
char        __optchar;          // matched option char ('-' or altoptchar)

static int  opt_offset = 0;     // position in currently parsed argument

// Error messages suggested by Single UNIX Specification
#define NO_ARG_MSG      "%s: option requires an argument -- %c\n"
#define BAD_OPT_MSG     "%s: illegal option -- %c\n"

int getopt( int argc, char * const argv[], const char *optstring )
/****************************************************************/
{
    char        *ptr;
    char        *curr_arg;

    argc = argc;
    optarg = NULL;
    curr_arg = argv[optind];
    if( curr_arg == NULL ) {
        return( -1 );
    }
    for( ;; ) {
        optopt = curr_arg[opt_offset];
        if( isspace( optopt ) ) {
            opt_offset++;
            continue;
        }
        break;
    }
    if( opt_offset > 1 || optopt == '-' || optopt == __altoptchar ) {
        if( opt_offset > 1 ) {
            optopt = curr_arg[opt_offset];
            if( optopt == '-' || optopt == __altoptchar ) {
                __optchar = (char)optopt;
                opt_offset++;
                optopt = curr_arg[opt_offset];
            }
        } else {
            __optchar = (char)optopt;
            opt_offset++;
            optopt = curr_arg[opt_offset];
        }
        if( optopt == '\0' ) {  // option char by itself should be
            return( -1 );       // left alone
        }
        if( optopt == '-' && curr_arg[opt_offset + 1] == '\0' ) {
            opt_offset = 0;
            ++optind;
            return( -1 );   // "--" POSIX end of options delimiter
        }
        ptr = strchr( optstring, optopt );
        if( ptr == NULL ) {
            if( opterr && *optstring != ':' ) {
                fprintf( stderr, BAD_OPT_MSG, argv[0], optopt );
            }
            return( '?' );  // unrecognized option
        }
        if( *(ptr + 1) == ':' ) {   // check if option requires argument
            if( curr_arg[opt_offset + 1] == '\0' ) {
                if( argv[optind + 1] == NULL ) {
                    if( *optstring == ':' ) {
                        return( ':' );
                    } else {
                        if( opterr ) {
                            fprintf( stderr, NO_ARG_MSG, argv[0], optopt );
                        }
                        return( '?' );
                    }
                }
                optarg = argv[optind + 1];
                ++optind;
            } else {
                optarg = &curr_arg[opt_offset + 1];
            }
            opt_offset = 0;
            ++optind;
        } else {
            opt_offset++;
            if( curr_arg[opt_offset] == '\0' ) {    // last char in argv element
                opt_offset = 0;
                ++optind;
            }
        }
        return( optopt );   // return recognized option char
    } else {
        return( -1 );       // no more options
    }
}

static int is_valid_template( char *template, char **xs )
{
    size_t              len;
    char                *p;

    /*** Ensure the last 6 characters form the string "XXXXXX" ***/
    len = strlen( template );
    if( len < 6 ) {
        return( 0 );        /* need room for six exes */
    }
    p = template + len - 6;
    if( strcmp( p, "XXXXXX" ) ) {
        return( 0 );
    }
    *xs = p;

    return( 1 );
}

int mkstemp( char *template )
{
    char                letter;
    unsigned            pid;
    char                *xs;
    int                 fh;

    /*** Ensure the template is valid ***/
    if( !is_valid_template( template, &xs ) ) {
        return( -1 );
    }

    /*** Get the process/thread ID ***/
    pid = GetCurrentProcessId();
    pid %= 100000;      /* first few digits could be repeated */

    /*** Try to build a unique filename ***/
    for( letter = 'a'; letter <= 'z'; letter++ ) {
        sprintf( xs, "%c%05u", letter, pid );
        if( access( template, F_OK ) != 0 ) {
            fh = open( template, O_RDWR | O_CREAT | O_TRUNC | O_EXCL | O_BINARY, S_IREAD | S_IWRITE );
            if( fh != -1 ) {
                return( fh );       /* file successfully created */
            }
            /* EEXIST may occur in case of a race condition or if we simply
             * created that temp file earlier, and we'll try again. If however
             * the creation failed for some other reason, it will almost
             * certainly fail again no matter how many times we try. So don't.
             */
            if( errno != EEXIST ) {
                return( -1 );
            }
        }
    }
    return( -1 );
}

unsigned sleep( unsigned time )
{
    Sleep( time * 1000UL );
    return( 0 );
}

#endif /* _MSC_VER */

char *get_dllname( char *buf, int len )
{
#ifdef _MSC_VER
    HMODULE hnd = NULL;
#endif

    *buf = '\0';
#ifdef _MSC_VER
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&optind, &hnd);
    GetModuleFileName( hnd, buf, len );
#endif
    return( buf );
}

int _vbprintf( char *s, size_t bufsize, const char *format, __va_list arg )
{
    int rc;

    --bufsize;
    rc = vsnprintf( s, bufsize, format, arg );
    if( rc < 0 ) {
        rc = (int)bufsize;
    }
    s[bufsize] = '\0';
    return( rc );
}

#endif /* ! __WATCOMC__ */
