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
* Description:  Declaration for OBJ output queues
*
****************************************************************************/

#ifndef _QUEUES_H_
#define _QUEUES_H_

#include "objrec.h"

typedef struct line_num_info {
    unsigned_16 number;
    unsigned_32 offset;
    const FNAME *srcfile;
} line_num_info;

extern void     AddPublicData( dir_node *data );
extern void     AddPublicProc( dir_node *data );
extern void     AddLnameData( dir_node *data );
extern void     AddAliasData( char *data );
extern void     AddLinnumData( struct line_num_info *data );

extern bool     GetLnameData( obj_rec * );
extern char     *GetAliasData( bool );
extern bool     GetPublicData( void );
extern int      GetLinnumData( int limit, struct linnum_data **ldata, bool *need32 );

extern void     FreeAllQueues( void );

#endif
