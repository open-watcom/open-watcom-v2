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
#include <stdlib.h>
#include <string.h>
#include "vi.h"

static alias_list *alHead,*alTail;
static alias_list *abHead,*abTail;
/*
 * setGenericAlias - define an alias/abbreviation
 */
static int setGenericAlias( char *what, alias_list **head, alias_list **tail )
{
    alias_list  *curr;
    char        str[MAX_STR];

    if( NextWord1( what, str ) <=0 ) {
        return( ERR_INVALID_ALIAS );
    }
    RemoveLeadingSpaces( what );

    /*
     * see if alias is already in list: if so, and there is expansion data,
     * then replace the data, else delete the item
     */
    curr = *head;
    while( curr != NULL ) {
        if( !strcmp( str,curr->alias ) ) {
            MemFree( curr->expand );
            if( what == NULL ) {
                MemFree( curr->alias );
                MemFree( DeleteLLItem( head,tail, curr ) );
            } else {
                AddString( &(curr->expand), what );
            }
        }
        curr = curr->next;
    }

    /*
     * add the new item
     */
    curr = MemAlloc( sizeof( alias_list ) );
    AddString( &(curr->alias), str );
    AddString( &(curr->expand), what );

    AddLLItemAtEnd( head, tail, curr );
    Message1( "%s set to \"%s\"", str, what );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* setGenericAlias */

/*
 * checkGenericAlias - check command line for aliases/abbrevs
 */
static alias_list *checkGenericAlias( char *str, alias_list *head )
{
    alias_list  *curr;

    curr = head;
    while( curr != NULL ) {
        if( !strcmp( str, curr->alias ) ) {
            return( curr );
        }
        curr = curr->next;
    }
    return( NULL );

} /* checkGenericAlias */

/*
 * removeGenericAlias
 */
static int removeGenericAlias( char *which, alias_list **head, alias_list **tail )
{
    alias_list  *curr;

    RemoveLeadingSpaces( which );
    curr = checkGenericAlias( which, *head );
    if( curr == NULL ) {
        return( ERR_NO_SUCH_ALIAS );
    }
    DeleteLLItem( head, tail, curr );
    Message1( "%s removed", which );
    return( ERR_NO_ERR );

} /* removeGenericAlias */


/*
 * SetAlias - set an alias
 */
int SetAlias( char *what )
{
    return( setGenericAlias( what, &alHead, &alTail ));
} /* SetAlias */

/*
 * UnAlias
 */
int UnAlias( char *what )
{
    return( removeGenericAlias( what, &alHead, &alTail ) );
} /* UnAlias */

/*
 * CheckAlias - check for an alias
 */
int CheckAlias( char *str, char *what )
{
    alias_list  *al;

    al = checkGenericAlias( str, alHead );
    if( al == NULL ) {
        return( ALIAS_NOT_FOUND );
    }
    strcpy( what, al->expand );
    return( ERR_NO_ERR );

} /* CheckAlias */

/*
 * Abbrev - set an abbreviation
 */
int Abbrev( char *what )
{
    int rc;

    rc = setGenericAlias( what, &abHead, &abTail );
    if( rc > 0 ) {
        return( ERR_INVALID_ABBREV );
    }
    return( rc );

} /* Abbrev */

/*
 * UnAbbrev - remove an abbreviation
 */
int UnAbbrev( char *abbrev )
{
    int rc;

    rc = removeGenericAlias( abbrev, &abHead, &abTail );
    if( rc ) {
        return( ERR_NO_SUCH_ABBREV );
    }
    return( ERR_NO_ERR );

} /* UnAbbrev */

/*
 * CheckAbbrev - look for an abbreviation, and expand it
 */
bool CheckAbbrev( char *data, int *ccnt )
{
    int         i,j,owl,col;
    alias_list  *curr;

    if( EditFlags.Modeless == TRUE ) {
        return( FALSE );
    }

    /*
     * get the current string, and matching alias
     */
    if( *ccnt == 0 ) {
        return( FALSE );
    }
    data[*ccnt] = 0;
    curr = checkGenericAlias( data, abHead );
    if( curr == NULL ) {
        return( FALSE );
    }

    i = CurrentColumn-1- (*ccnt);
    j = CurrentColumn-2;
    *ccnt = 0;

    /*
     * replace with full form
     */
    owl = WorkLine->len;
    WorkLine->len = ReplaceSubString( WorkLine->data, WorkLine->len,
                      i, j, curr->expand, strlen( curr->expand) );
    col = CurrentColumn + WorkLine->len - owl;
    GoToColumn( col, WorkLine->len+1 );
    return( TRUE );

} /* CheckAbbrev */
