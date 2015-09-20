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
#include <stddef.h>
#include "rxsupp.h"

#include "clibext.h"


#ifndef VICOMP
/*
 * VarAddGlobalStr
 */
void VarAddGlobalStr( const char *name, const char *val )
{
    VarAddStr( name, val, NULL );

} /* VarAddGlobalStr */


/*
 * VarAddRandC - add row and column vars
*/
void VarAddRandC( void )
{
    int vc;
    int len;

    if( CurrentLine == NULL ) {
        len = 0;
    } else {
        len = CurrentLine->len;
    }

    VarAddGlobalLong( "R", CurrentPos.line );
    VarAddGlobalLong( "Linelen", len );
    vc = VirtualColumnOnCurrentLine( CurrentPos.column );
    VarAddGlobalLong( "C", (long) vc );
    // VarDump( );

} /* VarAddRandC */

/*
 * SetModifiedVar - set the modified variable
 */
void SetModifiedVar( bool val )
{
    VarAddGlobalLong( "M", (long) val );

} /* SetModifiedVar */

/*
 * VarAddGlobalLong
 */
void VarAddGlobalLong( const char *name, long val )
{
    char ibuff[MAX_NUM_STR];

    VarAddStr( name, ltoa( val, ibuff, 10 ), NULL );

} /* VarAddGlobalLong */
#endif /* VICOMP */

/*
 * VarAddStr - add a new variable
 */
void VarAddStr( const char *name, const char *val, vlist *vl )
{
    vars        *new, *curr, *head;
    bool        glob;
    var_len     len;
    int         name_len;

    /*
     * get local/global setting
     */
    if( isupper( name[0] ) || vl == NULL ) {
        head = VarHead;
        glob = true;
    } else {
        head = vl->head;
        glob = false;
    }

    /*
     * see if we can just update an existing copy
     */
    len = strlen( val );
    curr = head;
    while( curr != NULL ) {
        if( !strcmp( curr->name, name ) ) {
            ReplaceString( &curr->value, val );
            curr->len = len;
#ifndef VICOMP
            if( glob && !EditFlags.CompileAssignmentsDammit ) {
                EditFlags.CompileAssignments = false;
            }
#endif
            return;
        }
        curr = curr->next;
    }

    /*
     * create and add a new variable
     */
    name_len = strlen( name );
    new = MemAlloc( offsetof( vars, name ) + name_len + 1 );
    memcpy( new->name, name, name_len + 1 );
    new->value = DupString( val );
    new->len = len;

    if( glob ) {
        AddLLItemAtEnd( (ss **)&VarHead, (ss **)&VarTail, (ss *)new );
#ifndef VICOMP
        EditFlags.CompileAssignments = false;
#endif
    } else {
        AddLLItemAtEnd( (ss **)&vl->head, (ss **)&vl->tail, (ss *)new );
    }

} /* VarAddStr */

/*
 * VarListDelete - delete a local variable list
 */
void VarListDelete( vlist *vl )
{
    vars *curr, *next;

    curr = vl->head;
    while( curr != NULL ) {
        next = curr->next;
        MemFree( curr->value );
        MemFree( curr );
        curr = next;
    }

} /* VarListDelete */

#ifndef VICOMP
/*
 * VarName - parse a variable name of the form %(foo)
 */
bool VarName( char *name, vlist *vl )
{
    if( name[0] != '%' || name[1] == 0 ) {
        return( false );
    }
    EliminateFirstN( name, 1 );
    if( name[0] == '(' ) {
        EliminateFirstN( name, 1 );
        name[strlen( name ) - 1] = 0;
    }
    if( strchr( name, '%' ) != NULL ) {
        Expand( name, name, vl );
    }
    return( true );

} /* VarName */

/*
 * VarFind - locate data for a specific variable name
 */
vars * VarFind( const char *name, vlist *vl )
{
    vars        *curr;

    /*
     * search locals
     */
    if( name[0] < 'A' || name[0] > 'Z' ) {
        if( vl != NULL ) {
            curr = vl->head;
            while( curr != NULL ) {
                if( !strcmp( name, curr->name ) ) {
                    return( curr );
                }
                curr = curr->next;
            }
        }
        return( NULL );
    }

    /*
     * search globals
     */
    curr = VarHead;
    while( curr != NULL ) {
        if( !strcmp( name, curr->name ) ) {
            return( curr );
        }
        curr = curr->next;
    }
    return( NULL );

} /* VarFind */


/* Free the globals */
void VarFini( void )
{
    vars *curr, *next;

    curr = VarHead;
    while( curr != NULL ) {
        next = curr->next;
        MemFree( curr->value );
        MemFree( curr );
        curr = next;
    }
}


void VarDump( void ){
    vars        *curr;
    int         count = 0;
    FILE        *f = fopen( "C:\\vi.out", "a+t" );

    curr = VarHead;
    while( curr != NULL ) {
        // fprintf( f,"N:%s V:%s %x\n", curr->name, curr->value, curr->next );
        count++;
        curr = curr->next;
    }
    if( count == 13 ) {
        count = 13;
    }
    fprintf( f, "count %d\n", count );
    fclose( f );
}

void VarSC( char *str )
{
    /// DEBUG BEGIN
    {
        vars    *currn = VarHead;
        if( currn ) {
            while( currn->next != NULL ) {
                currn = currn->next;
            }
            if( VarTail != currn ) {
               printf( "%s\n", str );
            }
        }
    }
    /// DEBUG END
}
#endif /* VICOMP */
