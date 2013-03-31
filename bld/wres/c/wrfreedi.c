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

void __FreeLangList( WResResNode *curres ) {

    WResLangNode        *oldnode;
    WResLangNode        *currnode;

    currnode = curres->Head;
    while( currnode != NULL ) {
        oldnode = currnode;
        currnode = currnode->Next;
        WRESFREE( oldnode );
    }
    curres->Head = NULL;
    curres->Tail = NULL;
}

void __FreeResList( WResTypeNode *currtype )
{
    WResResNode         *oldnode;
    WResResNode         *currnode;

    currnode = currtype->Head;
    while( currnode != NULL ) {
        oldnode = currnode;
        currnode = currnode->Next;
        __FreeLangList( oldnode );
        WRESFREE( oldnode );
    }

    currtype->Head = NULL;
    currtype->Tail = NULL;
}

void __FreeTypeList( WResDirHead *currdir )
{
    WResTypeNode        *oldtype;
    WResTypeNode        *currtype;

    currtype = currdir->Head;
    while( currtype != NULL ) {
        oldtype = currtype;
        currtype = currtype->Next;
        __FreeResList( oldtype );
        WRESFREE( oldtype );
    }

    currdir->Head = NULL;
    currdir->Tail = NULL;
}

void WResFreeDir( WResDir currdir )
/*********************************/
{
    if( currdir != NULL ) {
        __FreeTypeList( currdir );

        WRESFREE( currdir );
    }
}
