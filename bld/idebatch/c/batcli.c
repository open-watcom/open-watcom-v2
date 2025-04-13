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


/*
        BATCLI :  interface to BATSERV
*/

#include <stdlib.h>
#include <string.h>
#include "batpipe.h"


const char *BatchLink( const char *name )
{
    char    pipeName[PREFIX_LEN + NAME_MAXLEN + 1] = PREFIX;

    if( name == NULL )
        name = DEFAULT_LINK_NAME;
    strncpy( pipeName + PREFIX_LEN, name, NAME_MAXLEN );
    pipeName[PREFIX_LEN + NAME_MAXLEN] = '\0';
    if( BatservPipeOpen( pipeName ) ) {
        return( "can not connect to batcher spawn server" );
    }
    return( NULL );
}

int BatchMaxCmdLine( void )
{
    return( TRANS_DATA_MAXLEN );
}

batch_stat BatchChdir( const char *dir )
{
    BatservWriteData( LNK_CWD, dir, strlen( dir ) );
    BatservReadData();
    return( bdata.u.s.u.status );
}

int BatchSpawn( const char *cmd )
{
    BatservWriteData( LNK_RUN, cmd, strlen( cmd ) );
    return( 0 );
}

int BatchCollect( void *ptr, batch_len max, batch_stat *status )
{
    int         len;

    /*
     * reserve space for null terminate character
     */
    max--;
    BatservWriteData( LNK_QUERY, &max, sizeof( max ) );
    len = BatservReadData();
    if( len < 0 )
        return( -1 );
    if( len > 0 ) {
        if( bdata.u.s.cmd == LNK_STATUS ) {
            *status = bdata.u.s.u.status;
            return( -1 );
        }
        memcpy( ptr, bdata.u.s.u.data, len );
    }
    /*
     * add additional null terminate character
     */
    ((char *)ptr)[len] = '\0';
    return( len );
}

int BatchCancel( void )
{
    BatservWriteCmd( LNK_CANCEL );
    return( 0 );
}

int BatchAbort( void )
{
    BatservWriteCmd( LNK_ABORT );
    return( 0 );
}


void BatchUnlink( int shutdown )
{
    BatservWriteCmd( ( shutdown ) ? LNK_SHUTDOWN : LNK_DONE );
    BatservPipeClose();
}
