/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#if !defined(__UNIX__)
    #include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "helpio.h"

#include "clibext.h"


static int seekTypeConvTable[] = { SEEK_SET, SEEK_CUR, SEEK_END };

HELPIO long HelpFileLen( FILE *fp )
{
    unsigned long   old;
    long            len;

    old = fseek( fp, 0, SEEK_CUR );
    len = fseek( fp, 0, SEEK_END );
    fseek( fp, old, SEEK_SET );
    return( len );
}

HELPIO size_t HelpRead( FILE *fp, void *buf, size_t len )
{
    return( fread( buf, 1, len, fp ) );
}

HELPIO long HelpSeek( FILE *fp, long offset, HelpSeekType where )
{
    return( fseek( fp, offset, seekTypeConvTable[where] ) );
}

HELPIO long HelpTell( FILE *fp )
{
    return( ftell( fp ) );
}

HELPIO FILE *HelpOpen( const char *path )
{
    return( fopen( path, "rb" ) );
}

HELPIO int HelpClose( FILE *fp )
{
    return( fclose( fp ) );
}

HELPIO int HelpFileAccess( const char *path )
{
    return( access( path, F_OK ) );
}

HELPIO char *HelpGetCWD( char *buf, int size )
{
    return( getcwd( buf, size ) );
}

#ifndef __NETWARE_386__     // no environment vars in NetWare
HELPIO void HelpSearchEnv( const char *name, const char *env_var, char *buf )
{
    _searchenv( name, env_var, buf );
}
#endif
