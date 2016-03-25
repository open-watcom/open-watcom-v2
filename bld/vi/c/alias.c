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

static alias_list *alHead, *alTail;
static alias_list *abHead, *abTail;
/*
 * setGenericAlias - define an alias/abbreviation
 */
static vi_rc setGenericAlias( const char *what, alias_list **head, alias_list **tail )
{
    alias_list  *curr;
    char        str[MAX_STR];

    what = GetNextWord1( what, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_ALIAS );
    }
    what = SkipLeadingSpaces( what );

    /*
     * see if alias is already in list: if so, and there is expansion data,
     * then replace the data, else delete the item
     */
    for( curr = *head; curr != NULL; curr = curr->next ) {
        if( strcmp( str, curr->alias ) == 0 ) {
            MemFree( curr->expand );
            if( *what == '\0' ) {
                MemFree( curr->alias );
                MemFree( DeleteLLItem( (ss **)head, (ss **)tail, (ss *)curr ) );
            } else {
                curr->expand = DupString( what );
            }
        }
    }

    /*
     * add the new item
     */
    curr = MemAlloc( sizeof( alias_list ) );
    curr->alias = DupString( str );
    curr->expand = DupString( what );

    AddLLItemAtEnd( (ss **)head, (ss **)tail, (ss *)curr );
    Message1( "%s set to \"%s\"", str, what );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* setGenericAlias */

/*
 * checkGenericAlias - check command line for aliases/abbrevs
 */
static alias_list *checkGenericAlias( const char *str, size_t len, alias_list *head )
{
    alias_list  *curr;

    if( len == 0 )
        len = strlen( str );
    for( curr = head; curr != NULL; curr = curr->next ) {
        if( memcmp( str, curr->alias, len ) == 0 && curr->alias[len] == '\0' ) {
            break;
        }
    }
    return( curr );

} /* checkGenericAlias */

/*
 * removeGenericAlias
 */
static vi_rc removeGenericAlias( const char *which, alias_list **head, alias_list **tail )
{
    alias_list  *curr;

    which = SkipLeadingSpaces( which );
    curr = checkGenericAlias( which, 0, *head );
    if( curr == NULL ) {
        return( ERR_NO_SUCH_ALIAS );
    }
    DeleteLLItem( (ss **)head, (ss **)tail, (ss *)curr );
    Message1( "%s removed", which );
    return( ERR_NO_ERR );

} /* removeGenericAlias */


/*
 * SetAlias - set an alias
 */
vi_rc SetAlias( const char *what )
{
    return( setGenericAlias( what, &alHead, &alTail ) );

} /* SetAlias */

/*
 * UnAlias
 */
vi_rc UnAlias( const char *what )
{
    return( removeGenericAlias( what, &alHead, &alTail ) );

} /* UnAlias */

/*
 * CheckAlias - check for an alias
 */
vi_rc CheckAlias( const char *str, char *what )
{
    alias_list  *al;

    al = checkGenericAlias( str, 0, alHead );
    if( al == NULL ) {
        return( ALIAS_NOT_FOUND );
    }
    strcpy( what, al->expand );
    return( ERR_NO_ERR );

} /* CheckAlias */

/*
 * Abbrev - set an abbreviation
 */
vi_rc Abbrev( const char *what )
{
    vi_rc   rc;

    rc = setGenericAlias( what, &abHead, &abTail );
    if( rc > ERR_NO_ERR ) {
        return( ERR_INVALID_ABBREV );
    }
    return( rc );

} /* Abbrev */

/*
 * UnAbbrev - remove an abbreviation
 */
vi_rc UnAbbrev( const char *abbrev )
{
    vi_rc   rc;

    rc = removeGenericAlias( abbrev, &abHead, &abTail );
    if( rc != ERR_NO_ERR ) {
        return( ERR_NO_SUCH_ABBREV );
    }
    return( ERR_NO_ERR );

} /* UnAbbrev */

/*
 * CheckAbbrev - look for an abbreviation, and expand it
 */
bool CheckAbbrev( const char *data, int *ccnt )
{
    int         i, j, owl, col;
    alias_list  *curr;
    size_t      len;

    if( EditFlags.Modeless ) {
        return( false );
    }

    /*
     * get the current string, and matching alias
     */
    len = *ccnt;
    *ccnt = 0;
    if( len == 0 ) {
        return( false );
    }
    curr = checkGenericAlias( data, len, abHead );
    if( curr == NULL ) {
        return( false );
    }

    i = CurrentPos.column - 1 - len;
    j = CurrentPos.column - 2;

    /*
     * replace with full form
     */
    owl = WorkLine->len;
    WorkLine->len = ReplaceSubString( WorkLine->data, WorkLine->len,
                      i, j, curr->expand, strlen( curr->expand ) );
    col = CurrentPos.column + WorkLine->len - owl;
    GoToColumn( col, WorkLine->len + 1 );
    return( true );

} /* CheckAbbrev */
