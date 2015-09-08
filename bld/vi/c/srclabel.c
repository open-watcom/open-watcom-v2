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
#include "source.h"

#include "clibext.h"


/*
 * FindLabel - locate a label
 */
int FindLabel( labels *labs, label lbl )
{
    int i;

    for( i = 0; i < labs->cnt; i++ ) {
        if( !stricmp( lbl, labs->name[i] ) )  {
            return( i );
        }
    }
    return( -1 );

} /* findLabel */

/*
 * AddLabel - add a label in the file
 */
vi_rc AddLabel( sfile *sf, labels *labs, label lbl )
{
    /*
     * find label name
     */
    if( FindLabel( labs, lbl ) >= 0 ) {
        return( ERR_NO_ERR );
    }

    /*
     * reallocate buffers
     */
    labs->name = MemReAlloc( labs->name, (labs->cnt + 1) * sizeof( char * ) );
    labs->pos = MemReAlloc( labs->pos, (labs->cnt + 1) * sizeof( struct sfile * ) );

    /*
     * set name and position of label
     */
    AddString( &(labs->name[labs->cnt]), lbl );
    labs->pos[labs->cnt] = sf;
    labs->cnt++;
    return( ERR_NO_ERR );

} /* AddLabel */
