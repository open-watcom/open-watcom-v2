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
* Description:  Routines needed for both compression & decompression.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wpack.h"
#include "txttable.h"

// external function declarations
extern void             QSeek( int, signed long, int );
extern int              QRead( int, void *, int );
extern int              QClose( int );
extern int              NoErrOpen( char * );
extern void             PackExit( void );

int             infile, outfile;

unsigned char      text_buf[N + F - 1];

byte        len[ NUM_CHARS ];       // the length of the code value
int         indicies[ NUM_CHARS ];  // indicies to make len look sorted

#ifdef PATCH
extern unsigned_32 ReadInternal( char *srcfullpath )
/**************************************************/
{
    arc_header          header;

    infile = NoErrOpen( srcfullpath );
    if( infile < 0 ) {
        return 0;
    }

    if( QRead( infile, &header, sizeof( arc_header ) ) == -1 ) {
        QClose( infile );
        return 0;
    }

    if( header.major_ver == MAJOR_VERSION + (byte) header.internal ) {
        QClose( infile );
        return header.internal;
    }

    QClose( infile );
    return 0;
}

#endif

extern file_info ** ReadHeader( arccmd *cmd, arc_header *header )
/***************************************************************/
// read the archive information & parse it into the internal structure.
{
    int             result;
    file_info *     block;
    file_info **    currfile;
    file_info **    filedata;
    int             numfiles;
    char      *     msg;

    infile = NoErrOpen( cmd->arcname );
    if( infile < 0 ) return( NULL );
    result = QRead( infile, header, sizeof( arc_header ) );

    if( result == -1 || header->signature != SIGNATURE ) {
        msg = LookupText( NULL, TXT_INV_ARC );
        Error( TXT_INV_ARC, msg );
    }

    if( cmd->flags & SECURE_PACK
        && header->internal != cmd->internal ) {
        msg = LookupText( NULL, TXT_INC_VER_ARC );
        Error( TXT_INC_VER_ARC, msg );
    }
    if( header->major_ver != ( MAJOR_VERSION + (byte) cmd->internal )
        || header->minor_ver > MINOR_VERSION ) {
        msg = LookupText( NULL, TXT_INC_VER_ARC );
        Error( TXT_INC_VER_ARC, msg );
    }

    QSeek( infile, header->info_offset, SEEK_SET );
    block = WPMemAlloc( header->info_len );
    result = QRead( infile, block, header->info_len );
    if( result == -1 )  {
        PackExit();
    }
    filedata = WPMemAlloc( sizeof( file_info * ) * (header->num_files + 1) );
    currfile = filedata;
    numfiles = header->num_files;
    while( numfiles > 0 ) {
        *currfile = block;
        currfile++;
        block = (file_info *)((char *)block + sizeof(file_info) +
                                       (block->namelen & NAMELEN_MASK) - 1);
        numfiles--;
    }
    *currfile = NULL;
    return( filedata );
}

void    FreeHeader( file_info **filedata )
/****************************************/

{
    WPMemFree( *filedata );
    WPMemFree( filedata );
}

typedef struct node {
    struct node *   next;
} node;

extern void LinkList( void *in_head, void *newnode )
/**************************************************/
/* Link a new node into a linked list (new node goes at the end of the list) */
{
    node                **owner;

    owner = in_head;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = newnode;
}
