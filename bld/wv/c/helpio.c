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
* Description:  Help file I/O functions (used by online help viewer).
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "helpio.h"

static int seekTypeConvTable[] = { DIO_SEEK_ORG, DIO_SEEK_CUR, DIO_SEEK_END };

HELPIO long int HelpFileLen( HelpFp fp )
{
    long        old;

    old = SeekStream( (handle)fp, 0, DIO_SEEK_END );
    return( SeekStream( (handle)fp, old, DIO_SEEK_ORG ) );
}

HELPIO int HelpRead( HelpFp fp, void *buf, int len )
{
    return( ReadStream( (handle)fp, buf, len ) );
}

HELPIO int HelpWrite( HelpFp fp, char *buf, int len )
{
    return( WriteStream( (handle)fp, buf, len ) );
}

HELPIO long int HelpSeek( HelpFp fp, long int offset, HelpSeekType where ) {

    return( SeekStream( (handle)fp, offset, seekTypeConvTable[ where ] ) );
}

HELPIO long int HelpTell( HelpFp fp )
{
    return( SeekStream( (handle)fp, 0, DIO_SEEK_CUR ) );
}

HELPIO HelpFp HelpOpen( char *path, unsigned long mode )
{
    if( mode != (HELP_OPEN_RDONLY | HELP_OPEN_BINARY) ) {
        return( -1 );
    }
    return( (HelpFp)FileOpen( path, OP_READ ) );
}

HELPIO int HelpClose( HelpFp fp )
{
    FileClose( (handle)fp );
    return( 0 );
}

HELPIO int HelpAccess( char *path, int mode )
{
    handle      fp;
    int         rc;

    mode=mode;
    fp = FileOpen( path, OP_READ );
    rc = -1;
    if( fp != NIL_HANDLE ) {
        rc = 0;
        FileClose( fp );
    }
    return( rc );
}

HELPIO char *HelpGetCWD( char *buf, int size )
{
    size=size;
    buf[0] = '\0';
    return( buf );
}

HELPIO void HelpSearchEnv( char *name, char *env_var, char *buf )
{
    name=name;env_var=env_var;
    buf[0] = '\0';
}
