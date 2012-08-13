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


#include "wresrtns.h"
#include "wres.h"

/*
 * delAResource - delete a single resource node and update pointers/counters
 *                This routine assumes that all language nodes for this
 *                resource have already been deleted.
 */
static void delAResource( WResDir currdir, WResTypeNode *type,
                           WResResNode *cur_res )
{
    if( cur_res->Next != NULL ) {
        cur_res->Next->Prev = cur_res->Prev;
    } else {
        type->Tail = cur_res->Prev;
    }
    if( cur_res->Prev != NULL ) {
        cur_res->Prev->Next = cur_res->Next;
    } else {
        type->Head = cur_res->Next;
    }
    type->Info.NumResources --;
    currdir->NumResources --;
    WRESFREE( cur_res );
}

/*
 * delAType - delete a type node.  This routine assumes that all
 *            resource nodes for the type have already been deleted
 */
static void delAType( WResDir currdir, WResTypeNode *type ) {

    if( type->Next != NULL ) {
        type->Next->Prev = type->Prev;
    } else {
        currdir->Tail = type->Prev;
    }
    if( type->Prev != NULL ) {
        type->Prev->Next = type->Next;
    } else {
        currdir->Head = type->Next;
    }
    currdir->NumTypes --;
    WRESFREE( type );
}

/*
 * doDelResource - delete a resource and if this leave a type with no
 *                 resources delete the type too. This routine assumes that
 *                 all language nodes for the resource have already been
 *                 deleted
 */
static void doDelResource( WResDir currdir, WResResNode *res,
                           WResTypeNode *type ) {
    delAResource( currdir, type, res );
    if( type->Info.NumResources == 0 ) {
        delAType( currdir, type );
    }
}

void WResDelResource( WResDir currdir, const WResID *type,
                      const WResID *name ) {
    WResTypeNode        *cur_type;
    WResResNode         *cur_res;

    cur_type = __FindType( type, currdir );
    if( cur_type == NULL ) return;
    cur_res = __FindRes( name, cur_type );
    if( cur_res == NULL ) return;
    __FreeLangList( cur_res );
    doDelResource( currdir, cur_res, cur_type );
}

#if(0)
//
// Not currently needed
//

/*
 * delALang - delete one language node and update pointers and counters
 */
static void delALang( WResResNode *res, WResLangType *lang )
{
    WResLangNode        *cur_lang;

    cur_lang = __FindLang( lang, cur_res );
    if( cur_lang == NULL ) return;
    if( cur_lang->Next != NULL ) {
        cur_lang->Next->Prev = cur_lang->Prev;
    } else {
        res->Tail = cur_lang->Prev;
    }
    if( cur_lang->Prev != NULL ) {
        cur_lang->Prev->Next = cur_lang->Next;
    } else {
        res->Head = cur_lang->Head;
    }
    res->Info.NumResources --;
    WRESFREE( cur_lang );
}

void WResDelLang( WResDir currdir, const WResID *type,
                        const WResID *name, WResLangType *lang ) {

    WResTypeNode        *cur_type;
    WResResNode         *cur_res;

    cur_type = __FindType( type, currdir );
    if( cur_type == NULL ) return;
    cur_res = __FindRes( name, cur_type )
    if( cur_res != NULL ) return;
    delALang( cur_res, lang )
    if( cur_res->Info.NumResources == 0 ) {
        doDelResource( currdir, cur_type, cur_res );
    }
}
#endif
