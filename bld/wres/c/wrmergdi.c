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


#include <string.h>
#include "wresall.h"
#include "reserr.h"
#include "wrmergdi.h"
#include "wresrtns.h"


bool WResMergeDirs( WResDir dstdir, WResDir srcdir, WResMergeError **errs )
/*************************************************************************/
{
    WResDirWindow       wind;
    WResDirWindow       wind_dup;
    WResLangInfo        *langinfo;
    WResResInfo         *resinfo;
    WResTypeInfo        *typeinfo;
    void                *fileinfo;
    WResMergeError      *err;

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
                        &wind_dup, fileinfo );
        if( !WResIsEmptyWindow( wind_dup ) && errs != NULL ) {
            err = WRESALLOC( sizeof( WResMergeError ) );
            err->next = *errs;
            *errs = err;
            err->dstres = wind_dup;
            err->srcres = wind;
        }
        wind = WResNextResource( wind, srcdir );
    }
    return( false );
}

void WResFreeMergeErrors( WResMergeError *errs )
/**********************************************/
{
    WResMergeError      *err;

    while( (err = errs) != NULL ) {
        errs = errs->next;
        WRESFREE( err );
    }
}
