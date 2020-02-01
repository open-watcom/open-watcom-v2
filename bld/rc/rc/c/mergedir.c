/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Merge directories function implementation.
*
****************************************************************************/


#include <stdio.h>
#include "bool.h"
#include "watcom.h"
#include "wres.h"
#include "pass2l1.h"
#include "rcrtns.h"     // for OW linker build
#include "mergedir.h"


#if !defined( INSIDE_WLINK ) || defined( _OS2 )
bool MergeDirectory( ResFileInfo *resfiles, WResMergeError **errs )
/******************************************************************
 * merge the directories of all the res files into one large directory
 * stored on the first resfileinfo node
 */
{
    ResFileInfo     *cur;

    if( errs != NULL )
        *errs = NULL;
    if( resfiles == NULL )
        return( false );
    for( cur = resfiles->next; cur != NULL; cur = cur->next ) {
        if( WResMergeDirs( resfiles->Dir, cur->Dir, errs ) ) {
            return( true );
        }
    }
    return( false );
}
#endif
