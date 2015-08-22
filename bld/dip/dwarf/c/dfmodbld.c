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


#include "dfdip.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfmodbld.h"


extern void  InitModList( mod_list *list ){
/*********************************/
    list->count = 0;
    list->wat_producer_ver = VER_NONE;
    list->head = NULL;
    list->curr = NULL;
    list->lnk = &list->head;
}


extern  mod_info  *NextModInfo( mod_list *list ){
/************************************************************/
// Get next mod info in list
// bump the item count
    mod_blk     *blk;
    mod_info    *next;
    unsigned_16 rem;    /*#entries in last blk */

    rem = list->count % MODS_PER_BLK;
    if( rem == 0 ){
        blk = DCAlloc( sizeof( *blk ) );
        blk->next = NULL;
        *list->lnk = blk;
        list->lnk =  &blk->next;
        blk->info = DCAlloc( sizeof( mod_info ) * MODS_PER_BLK );
        list->curr = blk;
    }
    blk = list->curr;
    next = &blk->info[rem];
    ++list->count;
    return( next );
}

extern mod_info  *FiniModInfo( mod_list *list )
/*****************************************/
//Free all offset blocks for a line segment
//Free all line segments
{
    mod_info    *new, *cpy;
    mod_blk     *curr, *next;
    size_t      size;

    size = sizeof( mod_info ) * list->count;
    if( size > 0 ) {
        new = DCAlloc( size );
        curr = list->head;
        cpy = new;
        while( curr->next != NULL ){
            size -= sizeof( mod_info ) * MODS_PER_BLK;
            memcpy( cpy, curr->info, sizeof( mod_info ) * MODS_PER_BLK );
            cpy = &cpy[MODS_PER_BLK];
            DCFree( curr->info );
            next = curr->next;
            DCFree( curr );
            curr = next;
       }
       memcpy( cpy, curr->info, size );
       DCFree( curr->info );
       DCFree( curr );
    } else {
        new = NULL;
    }
    list->head = NULL;
    list->count = 0;
    return( new );
}
