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
#include "wresrtns.h"

void __FreeLangList( WResResNode *curres )
{
    WResLangNode        *currnode;
    WResLangNode        *nextnode;

    for( currnode = curres->Head; currnode != NULL; currnode = nextnode ) {
        nextnode = currnode->Next;
        WRESFREE( currnode );
    }
    curres->Head = NULL;
    curres->Tail = NULL;
}

void __FreeResList( WResTypeNode *currtype )
{
    WResResNode         *currnode;
    WResResNode         *nextnode;

    for( currnode = currtype->Head; currnode != NULL; currnode = nextnode ) {
        nextnode = currnode->Next;
        __FreeLangList( currnode );
        WRESFREE( currnode );
    }

    currtype->Head = NULL;
    currtype->Tail = NULL;
}

void __FreeTypeList( WResDir dir )
{
    WResTypeNode        *currtype;
    WResTypeNode        *nexttype;

    for( currtype = dir->Head; currtype != NULL; currtype = nexttype ) {
        nexttype = currtype->Next;
        __FreeResList( currtype );
        WRESFREE( currtype );
    }

    dir->Head = NULL;
    dir->Tail = NULL;
}

void WResFreeDir( WResDir dir )
/*****************************/
{
    if( dir != NULL ) {
        __FreeTypeList( dir );
        WRESFREE( dir );
    }
}
