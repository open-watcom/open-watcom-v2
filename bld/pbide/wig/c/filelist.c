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
#include "filelist.h"
#include "mem.h"

typedef struct flist{
    struct flist        *next;
    FILE                *fhdl;
}FileList;

static FileList         *openFiles;

static void freeFileList( void ) {
/*********************************/

    FileList    *tmp;

    while( openFiles != NULL ) {
        fclose( openFiles->fhdl );
        tmp = openFiles;
        openFiles = openFiles->next;
        MemFree( tmp );
    }
}

void InitFileList( void ) {
/**************************/
    freeFileList();
}

FILE *WigOpenFile( char *name, char *mode ) {
/*****************************************/
    FileList    *node;

    node = MemMalloc( sizeof( FileList ) );
    node->fhdl = fopen( name, mode );
    if( node->fhdl == NULL ) {
        MemFree( node );
        return( NULL );
    } else {
        node->next = openFiles;
        openFiles = node;
        return( node->fhdl );
    }
}

void WigCloseFile( FILE *fhdl ) {
/*****************************/

    FileList    **ptr;
    FileList    *tmp;

    if( fhdl != NULL ) {
        fclose( fhdl );
        ptr = &openFiles;
        while( *ptr != NULL ) {
            if( (*ptr)->fhdl == fhdl ) {
                tmp = *ptr;
                *ptr = (*ptr)->next;
                MemFree( tmp );
                break;
            }
            ptr = &( (*ptr)->next );
        }
    }
}

void FiniFileList( void ) {
/**************************/
    freeFileList();
}
