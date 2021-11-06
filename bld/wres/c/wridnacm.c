/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include "wresall.h"
#include "wstrcmp.h"


int WResIDNameCmp( const WResIDName *name1, const WResIDName *name2 )
/*******************************************************************/
/* Note: don't use stricmp since the names in WResID's are not NULL terminated */
{
    int         cmp_rc;
    unsigned    len;

    len = name1->NumChars;
    if( len > name2->NumChars )
        len = name2->NumChars;
    cmp_rc = WresStrnicmp( name1->Name, name2->Name, len );
    if( cmp_rc == 0 ) {
        if( name1->NumChars == name2->NumChars ) {
            return( 0 );
        } else if( name1->NumChars > name2->NumChars ) {
            /* longer name with the same prefix are greater */
            return( 1 );
        } else {
            return( -1 );
        }
    } else {
        return( cmp_rc );
    }
} /* WResIDNameCmp */
