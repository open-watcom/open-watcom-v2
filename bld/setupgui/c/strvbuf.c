/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  dynamic string (VBUF) related functions
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "setup.h"
#include "iopath.h"

#include "clibext.h"


#define MIN_VBUF_INC 32     // minimum increment allocated (power of 2)

#define BUFFER_SIZE(x)      ((x+1+(MIN_VBUF_INC-1))&(-MIN_VBUF_INC))
#define FREE_BUFFER(x)      {if(x->len>1)GUIMemFree(x->buf);}

#define EXT_SEP             '.'

// ************************************
// Vbuf functions that manage size only
// ************************************

void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
{
    char    *new_buffer;        // - old buffer

    if( reqd >= vbuf->len ) {
        reqd = BUFFER_SIZE( reqd );
        new_buffer = GUIMemAlloc( reqd );
        memcpy( new_buffer, vbuf->buf, vbuf->used + 1 ); // +1 include '\0' terminator
        FREE_BUFFER( vbuf );
        vbuf->buf = new_buffer;
        vbuf->len = reqd;
    }
}

void VbufSetLen(                // SET BUFFER LENGTH
    VBUF *vbuf,                 // - VBUF structure
    size_t size )               // - new length
{
    if( vbuf->len > 1 ) {
        vbuf->used = size;
        vbuf->buf[size] = '\0';
    }
}

void VbufInit(                  // INITIALIZE BUFFER STRUCTURE
    VBUF *vbuf )                // - VBUF structure
{
    vbuf->buf = VBUF_INIT_BUF;
    vbuf->len = VBUF_INIT_LEN;
    vbuf->used = VBUF_INIT_USED;
}

void VbufFree(                  // FREE BUFFER
    VBUF *vbuf )                // - VBUF structure
{
    FREE_BUFFER( vbuf );
    VbufInit( vbuf );
}


// ****************************************************
// Vbufs comparision
// ****************************************************

int VbufCompVbuf(           // COMPARE A VBUFs
    const VBUF *vbuf1,      // - VBUF structure
    const VBUF *vbuf2,      // - VBUF structure
    bool igncase )          // - bool ignore case
{
    if( igncase ) {
        return( stricmp( vbuf1->buf, vbuf2->buf ) );
    } else {
        return( strcmp( vbuf1->buf, vbuf2->buf ) );
    }
}

int VbufCompStr(            // COMPARE A VBUFs
    const VBUF *vbuf1,      // - VBUF structure
    const char *str,        // - string
    bool igncase )          // - bool ignore case
{
    if( igncase ) {
        return( stricmp( vbuf1->buf, str ) );
    } else {
        return( strcmp( vbuf1->buf, str ) );
    }
}

int VbufCompExt(            // COMPARE A VBUFs
    const VBUF *vbuf1,      // - VBUF structure
    const char *str,        // - file name extension
    bool igncase )          // - bool ignore case
{
    if( vbuf1->buf[0] == '.' ) {
        if( str[0] == '.' )
            str++;
        if( igncase ) {
            return( stricmp( vbuf1->buf + 1, str ) );
        } else {
            return( strcmp( vbuf1->buf + 1, str ) );
        }
    } else if( vbuf1->buf[0] < '.' ) {
        return( -1 );
    } else {
        return( 1 );
    }
}

int VbufCompBuffer(         // COMPARE A VBUFs
    const VBUF *vbuf1,      // - VBUF structure
    char const *buffer,     // - buffer
    size_t size,            // - size of buffer
    bool igncase )          // - bool ignore case
{
    if( igncase ) {
        return( strnicmp( vbuf1->buf, buffer, size ) );
    } else {
        return( strncmp( vbuf1->buf, buffer, size ) );
    }
}

// ****************************************************
// Vbuf functions which manipulate with data in buffer
// ****************************************************

void VbufConcVbuf(              // CONCATENATE A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2 )         // - VBUF structure
{
    if( vbuf2->used > 0 ) {
        VbufReqd( vbuf1, vbuf1->used + vbuf2->used );
        memcpy( vbuf1->buf + vbuf1->used, vbuf2->buf, vbuf2->used );
        vbuf1->used += vbuf2->used;
        vbuf1->buf[vbuf1->used] = '\0';
    }
}

void VbufPrepVbuf(              // PREPEND A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2 )         // - VBUF structure to be prepended
{
    VBUF    temp;

    if( vbuf2->used > 0 ) {
        VbufInit( &temp );
        VbufReqd( &temp, vbuf1->used + vbuf2->used );
        memcpy( temp.buf, vbuf2->buf, vbuf2->used );
        memcpy( temp.buf + vbuf2->used, vbuf1->buf, vbuf1->used );
        temp.used = vbuf1->used + vbuf2->used;
        temp.buf[temp.used] = '\0';
        VbufFree( vbuf1 );
        *vbuf1 = temp;
    }
}

void VbufSetVbuf(               // SET A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2 )         // - VBUF structure
{
    VbufSetLen( vbuf1, 0 );
    VbufConcVbuf(  vbuf1, vbuf2 );
}


void VbufConcVbufPos(           // CONCATENATE A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2,          // - VBUF structure
    size_t start_pos )          // - start position in second VBUF
{
    size_t  len2;

    if( vbuf2->used > start_pos ) {
        len2 = vbuf2->used - start_pos;
        VbufReqd( vbuf1, vbuf1->used + len2 );
        memcpy( vbuf1->buf + vbuf1->used, vbuf2->buf + start_pos, len2 );
        vbuf1->used += len2;
        vbuf1->buf[vbuf1->used] = '\0';
    }
}

void VbufPrepVbufPos(           // PREPEND A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2,          // - VBUF structure to be prepended
    size_t start_pos )          // - start position in second VBUF
{
    VBUF    temp;
    size_t  len2;

    if( vbuf2->used > start_pos ) {
        len2 = vbuf2->used - start_pos;
        VbufInit( &temp );
        VbufReqd( &temp, vbuf1->used + len2 );
        memcpy( temp.buf, vbuf2->buf + start_pos, len2 );
        memcpy( temp.buf + len2, vbuf1->buf, vbuf1->used );
        temp.used = vbuf1->used + len2;
        temp.buf[temp.used] = '\0';
        VbufFree( vbuf1 );
        *vbuf1 = temp;
    }
}

void VbufSetVbufPos(            // SET A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2,          // - VBUF structure
    size_t start_pos )          // - start position in second VBUF
{
    VbufSetLen( vbuf1, 0 );
    VbufConcVbufPos(  vbuf1, vbuf2, start_pos );
}


void VbufConcBuffer(            // CONCATENATE A BUFFER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *buffer,         // - size of buffer
    size_t size )               // - buffer
{
    if( size > 0 ) {
        VbufReqd( vbuf, vbuf->used + size );
        memcpy( vbuf->buf + vbuf->used, buffer, size );
        vbuf->used += size;
        vbuf->buf[vbuf->used] = '\0';
    }
}

void VbufPrepBuffer(            // PREPEND A BUFFER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *buffer,         // - size of buffer
    size_t size )               // - buffer
{
    VBUF    temp;

    if( size > 0 ) {
        VbufInit( &temp );
        VbufReqd( &temp, size + vbuf->used );
        memcpy( temp.buf, buffer, size );
        memcpy( temp.buf + size, vbuf->buf, vbuf->used );
        temp.used = size + vbuf->used;
        temp.buf[temp.used] = '\0';
        VbufFree( vbuf );
        *vbuf = temp;
    }
}

void VbufSetBuffer(             // SET A BUFFER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *buffer,         // - size of buffer
    size_t size )               // - buffer
{
    VbufSetLen( vbuf, 0 );
    VbufConcBuffer( vbuf, buffer, size );
}


void VbufConcStr(               // CONCATENATE A STRING TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
{
    VbufConcBuffer( vbuf, string, strlen( string ) );
}

void VbufPrepStr(               // PREPEND A STRING TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
{
    VbufPrepBuffer( vbuf, string, strlen( string ) );
}

void VbufSetStr(                // SET A STRING TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
{
    VbufSetLen( vbuf, 0 );
    VbufConcBuffer( vbuf, string, strlen( string ) );
}


void VbufConcChr(               // CONCATENATE A CHAR TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
{
    VbufReqd( vbuf, vbuf->used + 1 );
    vbuf->buf[vbuf->used++] = chr;
    vbuf->buf[vbuf->used] = '\0';
}

void VbufPrepChr(               // PREPEND A CHAR TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
{
    char    buffer[2];          // buffer passed to VbufPrepStr

    buffer[0] = chr;
    buffer[1] = '\0';
    VbufPrepStr( vbuf, buffer );
}

void VbufSetChr(                // SET A CHAR TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
{
    VbufSetLen( vbuf, 0 );
    VbufConcChr( vbuf, chr );
}

void VbufSetBufferAt(           // CONCATENATE A BUFFER TO VBUF AT POSITION
    VBUF *vbuf,                 // - VBUF structure
    char const *buffer,         // - size of buffer
    size_t size,                // - buffer
    size_t atpos )              // - at position
{
    if( atpos > vbuf->used )
        atpos = vbuf->used;
    VbufReqd( vbuf, atpos + size );
    if( size > 0 ) {
        memcpy( vbuf->buf + atpos, buffer, size );
    }
    vbuf->used = atpos + size;
    vbuf->buf[vbuf->used] = '\0';
}

void VbufSetStrAt(              // CONCATENATE A STRING TO VBUF AT POSITION
    VBUF *vbuf,                 // - VBUF structure
    char const *string,         // - string to be concatenated
    size_t atpos )              // - at position
{
    VbufSetBufferAt( vbuf, string, strlen( string ), atpos );
}

void VbufSetVbufAt(             // CONCATENATE A VBUF TO VBUF AT POSITION
    VBUF *vbuf1,                // - VBUF structure
    const VBUF *vbuf2,          // - VBUF structure
    size_t atpos )              // - at position
{
    if( atpos > vbuf1->used )
        atpos = vbuf1->used;
    VbufReqd( vbuf1, atpos + vbuf2->used );
    if( vbuf2->used > 0 ) {
        memcpy( vbuf1->buf + atpos, vbuf2->buf, vbuf2->used );
    }
    vbuf1->used = atpos + vbuf2->used;
    vbuf1->buf[vbuf1->used] = '\0';
}

#if 0
void VbufConcDecimal(           // CONCATENATE A DECIMAL TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    unsigned value )            // - value to be concatenated
{
    char    buffer[16];         // - temp buffer

    ultoa( value, buffer, 10 );
    VbufConcStr( vbuf, buffer );
}
#endif

void VbufConcInteger(           // CONCATENATE A INTEGER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    int value,                  // - value to be concatenated
    int digits )                // - minimal number of digits, prepend leading '0' if necessary
{
    char    buffer[16];         // - temp buffer

    if( value < 0 ) {
        VbufConcChr( vbuf, '-' );
        value = -value;
    }
    ltoa( value, buffer, 10 );
    digits -= (int)strlen( buffer );
    /* add leading '0' if necessary */
    while( digits-- > 0 ) {
        VbufConcChr( vbuf, '0' );
    }
    VbufConcStr( vbuf, buffer );
}

void VbufSetInteger(            // SET A INTEGER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    int value,                  // - value to be concatenated
    int digits )                // - minimal number of digits, prepend leading '0' if necessary
{
    VbufSetLen( vbuf, 0 );
    VbufConcInteger( vbuf, value, digits );
}


void VbufTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM VBUF
    VBUF *vbuf )                // - VBUF structure
{
    size_t  len;

    len = vbuf->used;
    while( len-- > 0 && isspace( vbuf->buf[len] ) ) {
        vbuf->buf[len] = '\0';
        vbuf->used--;
    }
}

void VbufAddDirSep(             // TERMINATE A VBUF AS PATH BY DIR_SEP
    VBUF *vbuf )                // - VBUF structure
{
    size_t  len;
    char    lastChr;

    len = VbufLen( vbuf );
    if( len == 0 ) {
        // "" -> ".[/\]"
        VbufConcChr( vbuf, '.' );
        VbufConcChr( vbuf, DIR_SEP );
    } else {
        lastChr = VbufString( vbuf )[len - 1];
        if( !IS_DIR_SEP( lastChr ) ) {
            if( lastChr == '.' && len > 1 && IS_DIR_SEP( VbufString( vbuf )[len - 2] ) ) {
                // "...nnn[/\]." -> "...nnn[/\]"
                VbufSetLen( vbuf, len - 1 );
#ifndef __UNIX__
            } else if( lastChr == ':' && len == 2 ) {
                // "x:" -> "x:.[/\]"
                VbufConcChr( vbuf, '.' );
                VbufConcChr( vbuf, DIR_SEP );
#endif
            } else {
                // "...nnn" -> "...nnn[/\]"
                // "." -> ".[/\]"
                VbufConcChr( vbuf, DIR_SEP );
            }
        }
    }
}

void VbufRemEndDirSep(          // REMOVE DIR_SEP FROM A VBUF AS PATH
    VBUF *vbuf )                // - VBUF structure
{
    size_t  len;
    char    lastChr;

    len = VbufLen( vbuf );
    if( len == 0 ) {
        // "" -> "."
        VbufConcChr( vbuf, '.' );
    } else {
        lastChr = VbufString( vbuf )[len - 1];
        if( IS_DIR_SEP( lastChr ) ) {
            if( len == 1 ) {
                // "[/\]" -> "[/\]."
                VbufConcChr( vbuf, '.' );
#ifndef __UNIX__
            } else if( len == 3 && VbufString( vbuf )[len - 2] == ':' ) {
                // "x:[/\]" -> "x:[/\]."
                VbufConcChr( vbuf, '.' );
#endif
            } else {
                // "...nnn[/\]" -> "...nnn"
                VbufSetLen( vbuf, len - 1 );
            }
#ifndef __UNIX__
        } else if( len == 2 && lastChr == ':' ) {
            // "x:" -> "x:."
            VbufConcChr( vbuf, '.' );
#endif
        }
    }
}

void VbufMakepath(              // SET A FILE PATH NAME TO VBUF
    VBUF *full,                 // - VBUF structure
    const VBUF *drive,          // - file drive
    const VBUF *dir,            // - file directory
    const VBUF *name,           // - file name
    const VBUF *ext )           // - file extension
{
    VbufRewind( full );
    if( drive != NULL ) {
        if( VbufLen( drive ) > 0 ) {
#if defined( __UNIX__ )
            VbufConcVbuf( full, drive );
            /* if node did not end in '/' then put in a provisional one */
            if( VbufString( full )[VbufLen( full ) - 1] != DIR_SEP ) {
                VbufConcChr( full, DIR_SEP );
            }
#elif defined( __NETWARE__ )
            VbufConcVbuf( full, drive );
            if( VbufString( full )[VbufLen( full ) - 1] != DRIVE_SEP ) {
                VbufConcChr( full, DRIVE_SEP );
            }
#else
            if( ( VbufString( drive )[0] == DIR_SEP ) && ( VbufString( drive )[1] == DIR_SEP ) ) {
                VbufConcVbuf( full, drive );
            } else {
                VbufConcChr( full, VbufString( drive )[0] );
                VbufConcChr( full, DRIVE_SEP );
            }
#endif
        }
    }
    if( dir != NULL ) {
        if( VbufLen( dir ) > 0 ) {
            if( VbufString( dir )[0] == DIR_SEP && VbufString( full )[VbufLen( full ) - 1] == DIR_SEP ) {
                VbufConcVbufPos( full, dir, 1 );
            } else {
                VbufConcVbuf( full, dir );
            }
            VbufAddDirSep( full );
        }
    }
    if( name != NULL ) {
        if( VbufLen( name ) > 0 ) {
            if( VbufString( name )[0] == DIR_SEP && VbufString( full )[VbufLen( full ) - 1] == DIR_SEP ) {
                VbufConcVbufPos( full, name, 1 );
            } else {
                VbufConcVbuf( full, name );
            }
        }
    }
    if( ext != NULL ) {
        if( VbufLen( ext ) > 0 ) {
            if( VbufString( ext )[0] != EXT_SEP )
                VbufConcChr( full, EXT_SEP );
            VbufConcVbuf( full, ext );
        }
    }
}

void VbufSplitpath(             // GET A FILE PATH COMPONENTS FROM VBUF
    const VBUF *full,           // - full file path
    VBUF *drive,                // - VBUF for drive
    VBUF *dir,                  // - VBUF for directory
    VBUF *name,                 // - VBUF for name
    VBUF *ext )                 // - VBUF for extension
{
/* split full QNX path name into its components */

/* Under QNX we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */

/* Under Netware, 'drive' maps to 'volume' */

    const char *dotp;
    const char *namep;
    const char *startp;
    char       ch;
    const char *path;

    if( full == NULL ) {
        if( drive != NULL )
            VbufRewind( drive );
        if( dir != NULL )
            VbufRewind( dir );
        if( name != NULL )
            VbufRewind( name );
        if( ext != NULL ) {
            VbufRewind( ext );
        }
        return;
    }
    path = VbufString( full );

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

#if defined(__UNIX__)

    /* process node/drive specification */
    startp = path;
    if( path[0] == DIR_SEP && path[1] == DIR_SEP ) {
        path += 2;
        while( (ch = *path) != '\0' ) {
            if( IS_DIR_SEP( ch ) || ch == EXT_SEP ) {
                break;
            }
            path++;
        }
    }
    if( drive != NULL ) {
        VbufSetBuffer( drive, startp, path - startp );
    }

#elif defined(__NETWARE__)

    startp = strchr( path, DRIVE_SEP );
    if( startp != NULL ) {
        if( drive != NULL ) {
            VbufSetBuffer( drive, path, startp - path + 1 );
        }
        path = startp + 1;
    } else {
        if( drive != NULL ) {
            VbufRewind( drive );
        }
    }

#else

    /* processs drive specification */
    if( path[0] != '\0' && path[1] == DRIVE_SEP ) {
        if( drive != NULL ) {
            VbufSetChr( drive, path[0] );
            VbufConcChr( drive, DRIVE_SEP );
        }
        path += 2;
    } else {
        if( drive != NULL ) {
            VbufRewind( drive );
        }
    }

#endif

    /* process /user/fred/filename.ext for QNX */
    /* process /fred/filename.ext for DOS, OS/2 */
    dotp = NULL;
    namep = path;
    startp = path;
    while( (ch = *path) != '\0' ) {
        if( ch == EXT_SEP ) {
            dotp = path;
        } else if( IS_DIR_SEP( ch ) ) {
            namep = path + 1;
            dotp = NULL;
        }
        path++;
    }
    if( dotp == NULL )
        dotp = path;
    if( dir != NULL ) {
        VbufSetBuffer( dir, startp, namep - startp );
    }
    if( name != NULL ) {
        VbufSetBuffer( name, namep, dotp - namep );
    }
    if( ext != NULL ) {
        VbufSetBuffer( ext, dotp, path - dotp );
    }
}

void VbufFullpath(              // GET A FULL FILE PATH TO VBUF
    VBUF *vbuf,                 // - full file path
    const VBUF *file )          // - file name
{
    char    buffer[_MAX_PATH];

    _fullpath( buffer, VbufString( file ), sizeof( buffer ) );
    VbufSetStr( vbuf, buffer );
}

bool VbufSetPathDrive(          // SET A DRIVE FOR FILE PATH IN VBUF
    VBUF *vbuf,                 // - full file path
    char drive )                // - drive character
{
    if( vbuf->used > 0 ) {
        vbuf->buf[0] = drive;
        return( false );
    }
    return( true );
}

void VbufSetPathExt(            // SET A FILE EXTENSION FOR FILE PATH IN VBUF
    VBUF *vbuf,                 // - full file path
    const VBUF *new_ext )       // - file extension
{
    VBUF    drive;
    VBUF    dir;
    VBUF    name;

    VbufInit( &drive );
    VbufInit( &dir );
    VbufInit( &name );

    VbufSplitpath( vbuf, &drive, &dir, &name, NULL );
    VbufMakepath( vbuf, &drive, &dir, &name, new_ext );

    VbufFree( &name );
    VbufFree( &dir );
    VbufFree( &drive );
}
