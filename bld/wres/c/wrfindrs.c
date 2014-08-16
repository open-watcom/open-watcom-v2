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


#include "layer0.h"
#include "wres.h"
#include "layer1.h"
#include "util.h"

WResTypeNode *__FindType( const WResID *type, WResDir currdir )
{
    WResTypeNode        *currtype;

    for( currtype = currdir->Head; currtype != NULL; currtype = currtype->Next ) {
        if( WResIDCmp( type, &(currtype->Info.TypeName) ) ) {
            break;
        }
    }

    return( currtype );
}

WResResNode *__FindRes( const WResID *name, WResTypeNode *currtype )
{
    WResResNode         *currres;

    for( currres = currtype->Head; currres != NULL; currres = currres->Next ) {
        if( WResIDCmp( name, &( currres->Info.ResName ) ) ) {
            break;
        }
    }

    return( currres );
}

WResLangNode *__FindLang( const WResLangType *lang, WResResNode *curres ) {
/***********************************************************************/

    WResLangNode        *curlang;
    WResLangType        deflang;

    if( lang == NULL ) {
        deflang.lang = DEF_LANG;
        deflang.sublang = DEF_SUBLANG;
        lang = &deflang;
    }
    curlang = curres->Head;
    while( curlang != NULL ) {
        if( curlang->Info.lang.lang == lang->lang
            && curlang->Info.lang.sublang == lang->sublang ) break;
        curlang = curlang->Next;
    }
    return( curlang );
}

/*
 * WresFindResource - find a given resource in the directory.  If lang
 *                    is NULL the first resource with the given name and
 *                    type is returned
 */
WResDirWindow WResFindResource( const WResID *type, const WResID *name,
                        WResDir currdir, const WResLangType *lang )
{
    WResDirWindow   newwind;

    newwind.CurrType = __FindType( type, currdir );
    if( newwind.CurrType != NULL ) {
        newwind.CurrRes = __FindRes( name, newwind.CurrType );
        if( newwind.CurrRes != NULL ) {
            if( lang == NULL ) {
                newwind.CurrLang = newwind.CurrRes->Head;
            } else {
                newwind.CurrLang = __FindLang( lang, newwind.CurrRes );
            }
        } else {
            newwind.CurrLang = NULL;
            newwind.CurrType = NULL;
        }
    } else {
        newwind.CurrRes = NULL;
        newwind.CurrLang = NULL;
    }

    return( newwind );
}
