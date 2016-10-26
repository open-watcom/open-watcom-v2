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

static file_stack       **fStack;
static int              fDepth;

/*
 * InitFileStack - initialize the push stack
 */
void InitFileStack( void )
{
    fDepth = 0;
    MemFree( fStack );
    fStack = MemAlloc( EditVars.MaxPush * sizeof( file_stack * ) );

} /* InitFileStack */

void FiniFileStack( void )
{
    MemFree( fStack );

} /* FiniFileStack */

/*
 * PushFileStack - add current file to file stack
 */
vi_rc PushFileStack( void )
{
    file_stack  *fs;
    int         len;
    int         i;

    if( fStack == NULL ) {
        InitFileStack();
    }

    len = strlen( CurrentFile->name );

    fs = MemAlloc( offsetof( file_stack, fname ) + len + 1 );
    memcpy( fs->fname, CurrentFile->name, len + 1 );
    fs->p = CurrentPos;

    if( fDepth == EditVars.MaxPush ) {
        for( i = 1; i < EditVars.MaxPush; i++ ) {
            fStack[i - 1] = fStack[i];
        }
    } else {
        fDepth++;
    }
    fStack[fDepth - 1] = fs;

    return( ERR_NO_ERR );

} /* PushFileStack */

/*
 * PushFileStackAndMsg - push the file stack, and display a message
 */
vi_rc PushFileStackAndMsg( void )
{
    vi_rc   rc;

    rc = PushFileStack();
    if( rc == ERR_NO_ERR ) {
        Message1( "Current position saved; %d entries on file stack", fDepth );
    }
    return( rc );

} /* PushFileStackAndMsg */

/*
 * PopFileStack - go to file at top of file stack
 */
vi_rc PopFileStack( void )
{
    file_stack  *fs;
    vi_rc       rc;

    if( fDepth == 0 ) {
        return( ERR_FILE_STACK_EMPTY );
    }
    fDepth--;
    fs = fStack[fDepth];
    fStack[fDepth] = NULL;

    rc = EditFile( fs->fname, false );
    if( rc != ERR_NO_ERR ) {
        MemFree( fs );
        return( rc );
    }
    GoToLineNoRelCurs( fs->p.line );
    GoToColumnOnCurrentLine( fs->p.column );
    MemFree( fs );
    Message2( "%d entries left on file stack", fDepth );
    return( ERR_NO_ERR );

} /* PopFileStack */
