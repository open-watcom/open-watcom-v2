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


#include <stdio.h>
#include <string.h>
#include "vi.h"
#include "source.h"

static int findLabel( labels *, char * );

/*
 * SrcGoTo - goto processor
 */
int SrcGoTo( sfile **sf, char *data, labels *lab )
{
    int         i;
    char        dest[MAX_SRC_LINE];

    /*
     * get label
     */
    if( NextWord1( data, dest ) <= 0 ) {
        return( ERR_SRC_INVALID_GOTO );
    }
    i = findLabel( lab, dest );
    if( i >= 0 ) {
        if( (*sf)->branchcond == 2 || (*sf)->branchcond == (*sf)->prev->branchres ) {
            (*sf) = lab->pos[i];
        }
        return( ERR_NO_ERR );
    }

    return( ERR_SRC_LABEL_NOT_FOUND );

} /* SrcGoTo */

/*
 * AddLabel - add a label in the file
 */
int AddLabel( sfile *sf, labels *lab, char *data )
{
    int         j,i;
    char        tmp[MAX_SRC_LINE];

    /*
     * find label name
     */
    if( (j=NextWord1( data, tmp )) <= 0 ) {
        return( ERR_SRC_INVALID_LABEL );
    }
    if( (i=findLabel( lab, tmp )) >= 0 ) {
        return( ERR_NO_ERR );
    }

    /*
     * reallocate buffers
     */
    lab->name = MemReAlloc( lab->name, (lab->cnt+1) * sizeof(char *) );
    lab->pos = MemReAlloc( lab->pos, (lab->cnt+1) * sizeof( struct sfile *) );

    /*
     * set name and position of label
     */
    AddString( &(lab->name[ lab->cnt ]), tmp );
    lab->pos[lab->cnt] = sf;
    lab->cnt ++;
    return( ERR_NO_ERR );

} /* AddLabel */

/*
 * findLabel - locate a label
 */
static int findLabel( labels *lab, char *dest )
{
    int i;

    for( i=0;i<lab->cnt;i++ ) {
        if( !stricmp( dest, lab->name[i] ) )  {
            return( i );
        }
    }
    return( -1 );

} /* findLabel */
