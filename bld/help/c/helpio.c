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
* Description:  Low level file I/O routines.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "helpio.h"

#ifndef __WATCOMC__
    #include "clibext.h"
#endif

#if defined(__NETWARE__)
    /* Symbolic constants for the access() function */

    #define R_OK    4       /*  Test for read permission    */
    #define W_OK    2       /*  Test for write permission   */
    #define X_OK    1       /*  Test for execute permission */
    #define F_OK    0       /*  Test for existence of file  */
#endif
#if !defined(__UNIX__)
    #include <direct.h>
#endif

static int seekTypeConvTable[] = { SEEK_SET, SEEK_CUR, SEEK_END };

HELPIO long int HelpFileLen( HelpFp fp )
{
    return( filelength( (int)fp ) );
}

HELPIO int HelpRead( HelpFp fp, void *buf, int len )
{
    return( read( (int)fp, buf, len ) );
}

HELPIO int HelpWrite( HelpFp fp, char *buf, int len )
{
    return( write( (int)fp, buf, len ) );
}

HELPIO long int HelpSeek( HelpFp fp, long int offset, HelpSeekType where )
{
    return( lseek( (int)fp, offset, seekTypeConvTable[where] ) );
}

HELPIO long int HelpTell( HelpFp fp )
{
    return( tell( (int)fp ) );
}

HELPIO HelpFp HelpOpen( char *path, unsigned long mode )
{
    int         access;

    access = 0;
    if( mode & HELP_OPEN_RDONLY ) {
        access |= O_RDONLY;
    }
    if( mode & HELP_OPEN_WRONLY ) {
        access |= O_WRONLY;
    }
    if( mode & HELP_OPEN_RDWR ) {
        access |= O_RDWR;
    }
    if( mode & HELP_OPEN_APPEND ) {
        access |= O_APPEND;
    }
    if( mode & HELP_OPEN_TRUNC ) {
        access |= O_TRUNC;
    }
#ifndef __UNIX__
    if( mode & HELP_OPEN_BINARY ) {
        access |= O_BINARY;
    }
    if( mode & HELP_OPEN_TEXT ) {
        access |= O_TEXT;
    }
#endif
    if( mode & HELP_OPEN_CREAT ) {
        access |= O_CREAT;
    }
    if( access & O_CREAT ) {
        return( (HelpFp)open( path, access, S_IRWXU | S_IRWXG | S_IRWXO ) );
    } else {
        return( (HelpFp)open( path, access ) );
    }
}

HELPIO int HelpClose( HelpFp fp )
{
    return( close( (int)fp ) );
}

HELPIO int HelpAccess( char *path, int mode )
{
    int         mode2;

    mode2 = 0;

    if( mode & HELP_ACCESS_READ ) mode2 |= R_OK;
    if( mode & HELP_ACCESS_WRITE ) mode2 |= W_OK;
    if( mode & HELP_ACCESS_EXEC ) mode2 |= X_OK;
    if( mode & HELP_ACCESS_EXIST ) mode2 |= F_OK;
    return( access( path, mode2 ) );
}

HELPIO char *HelpGetCWD( char *buf, int size )
{
    return( getcwd( buf, size ) );
}

#ifndef __NETWARE_386__     // no environment vars in NetWare
HELPIO void HelpSearchEnv( char *name, char *env_var, char *buf )
{
    _searchenv( name, env_var, buf );
}
#endif
