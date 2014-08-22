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


#include "vi.h"
#include "clibext.h"

#if FSYS_CASE_SENSITIVE
    #define file_cmp    strcmp
#else
    #define file_cmp    stricmp
#endif

/*
 * SameFile - check if two files are the same
 */
bool SameFile( char *f1, char *f2 )
{
    char        full1[FILENAME_MAX];
    char        full2[FILENAME_MAX];

    if( !file_cmp( f1, f2 ) ) {
        return( true );
    }
    _fullpath( full1, f1, FILENAME_MAX );
    _fullpath( full2, f2, FILENAME_MAX );
    if( !file_cmp( full1, full2 ) ) {
        if( EditFlags.SameFileCheck ) {
            return( true );
        }
        EditFlags.DuplicateFile = true;
    }
    return( false );

} /* SameFile */

