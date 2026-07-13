/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include "layer0.h"
#include "wres.h"
#include "layer1.h"
#include "util.h"

WResTypeNode *__FindType( const WResID *type_id, WResDir dir )
{
    WResTypeNode        *typenode;

    for( typenode = dir->Head; typenode != NULL; typenode = typenode->Next ) {
        if( WResIDCmp( type_id, &(typenode->Info.TypeName) ) ) {
            break;
        }
    }

    return( typenode );
}

WResResNode *__FindRes( const WResID *res_id, WResTypeNode *typenode )
{
    WResResNode         *resnode;

    for( resnode = typenode->Head; resnode != NULL; resnode = resnode->Next ) {
        if( WResIDCmp( res_id, &( resnode->Info.ResName ) ) ) {
            break;
        }
    }

    return( resnode );
}

WResLangNode *__FindLang( const WResLangType *lang, WResResNode *resnode )
/************************************************************************/
{
    WResLangNode        *langnode;
    WResLangType        deflang;

    if( lang == NULL ) {
        deflang.lang = DEF_LANG;
        deflang.sublang = DEF_SUBLANG;
        lang = &deflang;
    }
    langnode = resnode->Head;
    while( langnode != NULL ) {
        if( langnode->Info.lang.lang == lang->lang
            && langnode->Info.lang.sublang == lang->sublang ) break;
        langnode = langnode->Next;
    }
    return( langnode );
}

/*
 * WresFindResource - find a given resource in the directory.  If lang
 *                    is NULL the first resource with the given name and
 *                    type is returned
 */
WResDirWindow WResFindResource( const WResID *type_id, const WResID *res_id,
                        WResDir dir, const WResLangType *lang )
{
    WResDirWindow   wind;

    wind.CurrType = __FindType( type_id, dir );
    if( wind.CurrType != NULL ) {
        wind.CurrRes = __FindRes( res_id, wind.CurrType );
        if( wind.CurrRes != NULL ) {
            if( lang == NULL ) {
                wind.CurrLang = wind.CurrRes->Head;
            } else {
                wind.CurrLang = __FindLang( lang, wind.CurrRes );
            }
        } else {
            wind.CurrLang = NULL;
            wind.CurrType = NULL;
        }
    } else {
        wind.CurrRes = NULL;
        wind.CurrLang = NULL;
    }

    return( wind );
}
