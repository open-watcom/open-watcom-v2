/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dos.h>
#include "batpipe.h"
#include "batcher.h"


int     pipeHdl = -1;

const char *BatchLink( const char *name )
{
    char        pipeName[PREFIX_LEN + NAME_MAXLEN + 1] = PREFIX;

    if( name == NULL )
        name = DEFAULT_LINK_NAME;
    strncpy( pipeName + PREFIX_LEN, name, NAME_MAXLEN );
    pipeName[PREFIX_LEN + NAME_MAXLEN] = '\0';
    if( _dos_open( pipeName, O_RDWR, &pipeHdl ) != 0 ) {
        return( "can not connect to batcher spawn server" );
    }
    return( NULL );
}

unsigned BatchMaxCmdLine()
{
    return( TRANS_MAXLEN - 1 );
}

static unsigned my_read( int hdl, void *buff, unsigned len )
{
    unsigned    got;

    if( _dos_read( hdl, buff, len, &got ) != 0 ) return( -1 );
    return( got );
}

static unsigned my_write( int hdl, void *buff, unsigned len )
{
    unsigned    sent;

    if( _dos_write( hdl, buff, len, &sent ) != 0 ) return( -1 );
    return( sent );
}

static char     batch_buff[TRANS_MAXLEN]; /* static to minimize stack space */

unsigned BatchChdir( const char *new_dir )
{
    batch_buff[0] = LNK_CWD;
    strcpy( &batch_buff[1], new_dir );
    my_write( pipeHdl, batch_buff, strlen( batch_buff ) + 1 );
    my_read( pipeHdl, batch_buff, sizeof( batch_buff ) );
    return( *(unsigned long *)&batch_buff[1] );
}

unsigned BatchSpawn( const char *cmd )
{

    batch_buff[0] = LNK_RUN;
    strcpy( &batch_buff[1], cmd );
    my_write( pipeHdl, batch_buff, strlen( batch_buff ) );
    return( 0 );
}

unsigned BatchCollect( void *ptr, unsigned max, unsigned long *status )
{
    int         len;
    char        *buff = ptr;

    buff[0] = LNK_QUERY;
    *(unsigned long *)&buff[1] = max;
    my_write( pipeHdl, buff, 5 );
    len = my_read( pipeHdl, buff, max ) - 1;
    if( len <= 0 ) return( 0 );
    if( *buff == LNK_STATUS ) {
        *status = *(unsigned long *)&buff[1];
        return( -1 );
    }
    memmove( buff, &buff[1], len );
    return( len );
}

unsigned BatchCancel()
{
    char        cmd;

    cmd = LNK_CANCEL;
    my_write( pipeHdl, &cmd, sizeof( cmd ) );
    return( 0 );
}

unsigned BatchAbort()
{
    char        cmd;

    cmd = LNK_ABORT;
    my_write( pipeHdl, &cmd, sizeof( cmd ) );
    return( 0 );
}


void BatchUnlink( int shutdown )
{
    char        done;

    done = shutdown ? LNK_SHUTDOWN : LNK_DONE;
    my_write( pipeHdl, &done, sizeof( done ) );
    _dos_close( pipeHdl );
}
