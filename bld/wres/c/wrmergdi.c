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


#include <string.h>
#include "wresall.h"
#include "reserr.h"

typedef struct WResMergeError {
    struct WResMergeError       *next;
    WResDirWindow               dstres;
    WResDirWindow               srcres;
} WResMergeError;

int WResMergeDirs( WResDir dstdir, WResDir srcdir, WResMergeError **errs ) {
/****************************************************************************/

    WResDirWindow       wind;
    WResDirWindow       dup;
    WResLangInfo        *langinfo;
    WResResInfo         *resinfo;
    WResTypeInfo        *typeinfo;
    void                *fileinfo;
    WResMergeError      *newerr;

    if( errs != NULL ) *errs = NULL;
    wind = WResFirstResource( srcdir );
    while( !WResIsEmptyWindow( wind ) ) {
        typeinfo = WResGetTypeInfo( wind );
        resinfo = WResGetResInfo( wind );
        langinfo = WResGetLangInfo( wind );
        fileinfo = WResGetFileInfo( wind );
        WResAddResource2( &typeinfo->TypeName, &resinfo->ResName,
                        langinfo->MemoryFlags, langinfo->Offset,
                        langinfo->Length, dstdir, &langinfo->lang,
                        &dup, fileinfo );
        if( !WResIsEmptyWindow( dup ) && errs != NULL ) {
            newerr = WRESALLOC( sizeof( WResMergeError ) );
            newerr->next = *errs;
            *errs = newerr;
            newerr->dstres = dup;
            newerr->srcres = wind;
        }
        wind = WResNextResource( wind, srcdir );
    }
    return( FALSE );
}

void WResFreeMergeErrors( WResMergeError *errs ) {
/***********************************************/
    WResMergeError      *tmp;

    while( errs != NULL ) {
        tmp = errs;
        errs = errs->next;
        WRESFREE( tmp );
    }
}
