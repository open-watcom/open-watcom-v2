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
#include "fts.h"
#include "source.h"
#include <assert.h>

static  ft_src  *ftsHead;
static  ft_src  *ftsTail;

/*
 * FTSStart - start a new fts; EditFlags.FileTypeSource will suck in commands
 */
vi_rc FTSStart( char *data )
{
    char        template_data[MAX_STR];
    template_ll *templatePtr;
    ft_src      *fts, *ftsOld;

    fts = MemAlloc( sizeof( ft_src ) );
    fts->cmd_head = fts->cmd_tail = NULL;
    fts->template_head = fts->template_tail = NULL;
    while( NextWord1( data, template_data ) > 0 ) {
        templatePtr = MemAlloc( sizeof( template_ll ) + strlen( template_data ) ); // +1 not needed
        strcpy( templatePtr->data, template_data );
        AddLLItemAtEnd( (ss **)&fts->template_head, (ss **)&fts->template_tail, (ss *)templatePtr );
    }

    if( ftsOld = FTSMatchTemplate( fts->template_head ) ) {
        FTSKill( ftsOld );
    }
    AddLLItemAtEnd( (ss **)&ftsHead, (ss **)&ftsTail, (ss *)fts );

    EditFlags.FileTypeSource = TRUE;

    return( ERR_NO_ERR );

} /* FTSStart */

/*
 * FTSAddCmd - add a 1-line command to the current (tail) fts
 */
vi_rc FTSAddCmd( char *data, int setkilled )
{
    char    cmd_data[MAX_STR];
    cmd_ll  *cmd;

    assert( ftsTail );
    assert( EditFlags.FileTypeSource );

    cmd_data[0] = '\0';
    // Source gets cute & trashes "set "...
    if( setkilled >= SRC_T_NULL + 1 ) {
        strcpy( cmd_data, "set " );
        if( EditFlags.ScriptIsCompiled ) {
            NextWord1( data, cmd_data + 4 );
            ExpandTokenSet( cmd_data + 4, cmd_data + 4 );
        }
    }
    strcat( cmd_data, data );

    cmd = MemAlloc( sizeof( cmd_ll ) + strlen( cmd_data ) );
    strcpy( cmd->data, cmd_data );

    AddLLItemAtEnd( (ss **)&ftsTail->cmd_head, (ss **)&ftsTail->cmd_tail, (ss *)cmd );

    return( ERR_NO_ERR );

} /* FTSAddCmd */

/*
 * FTSAddBoolean - add a boolean set to the current (tail) fts
 */
vi_rc FTSAddBoolean( bool val, char *name )
{
    char    cmd[MAX_SRC_LINE] = "set ";

    ADD_BOOL_PREFIX( cmd, val );
    strcat( cmd, name );
    return( FTSAddCmd( cmd, 0 ) );

} /* FTSAddBoolean */

/*
 * FTSAddInt - add an integral set to the current (tail) fts
 */
vi_rc FTSAddInt( int val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    sprintf( cmd, "set %s %d", name, val );
    return( FTSAddCmd( cmd, 0 ) );

} /* FTSAddInt */

/*
 * FTSAddChar - add a character set to the current (tail) fts
 */
vi_rc FTSAddChar( char val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    sprintf( cmd, "set %s %c", name, val );
    return( FTSAddCmd( cmd, 0 ) );

} /* FTSAddChar */

/*
 * FTSAddStr - add a string set to the current (tail) fts
 */
vi_rc FTSAddStr( char *val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    sprintf( cmd, "set %s %s", name, val );
    return( FTSAddCmd( cmd, 0 ) );

} /* FTSAddStr */

/*
 * FTSEnd - done with this filetypesource entry
 */
vi_rc FTSEnd( void )
{
    EditFlags.FileTypeSource = FALSE;
    return( ERR_NO_ERR );

} /* FTSEnd */

static ft_src *searchFT( const char *name )
{
    template_ll *template;
    ft_src      *fts;

    name = StripPath( name );
    if( name != NULL ) {
        for( fts = ftsHead; fts != NULL; fts = fts->next ) {
            for( template = fts->template_head; template != NULL; template = template->next ) {
                if( FileTemplateMatch( name, template->data ) ) {
                    return( fts );
                }
            }
        }
    }
    return( NULL );
}

/*
 * FTSSearchFTIndex - search if 'name' has a registered file type
 */
int FTSSearchFTIndex( const char *name )
{
    template_ll *template;
    ft_src      *fts;
    int         index;

    name = StripPath( name );
    if( name != NULL ) {
        for( index = 0, fts = ftsHead; fts != NULL; fts = fts->next, ++index ) {
            for( template = fts->template_head; template != NULL; template = template->next ) {
                if( FileTemplateMatch( name, template->data ) ) {
                    return( index );
                }
            }
        }
    }
    return( -1 );

} /* FTSSearchFTIndex */

static vi_rc runCmds( ft_src *fts )
{
    char    cmd_data[MAX_STR];
    vi_rc   rc;
    cmd_ll  *cmd;
    bool    oldScript, oldQuiet, oldHold;

    oldScript = EditFlags.ScriptIsCompiled;
    oldQuiet = EditFlags.Quiet;
    oldHold = EditFlags.DisplayHold;

    EditFlags.ScriptIsCompiled = FALSE;
    EditFlags.Quiet = TRUE;
    EditFlags.DisplayHold = TRUE;

    for( cmd = fts->cmd_head; cmd != NULL; cmd = cmd->next ) {
        strcpy( cmd_data, cmd->data );
        rc = RunCommandLine( cmd_data );
#if 0
        if( rc != ERR_NO_ERR && rc != DO_NOT_CLEAR_MESSAGE_WINDOW ) {
            break;
        }
#endif
    }
    EditFlags.ScriptIsCompiled = oldScript;
    EditFlags.Quiet = oldQuiet;
    EditFlags.DisplayHold = oldHold;

    return( rc );
}

/*
 * FTSRunCmds - run commands if 'name' has a registered file type
 */
vi_rc FTSRunCmds( char *name )
{
    ft_src      *fts;

    fts = searchFT( name );
    if( fts != NULL ) {
        runCmds( fts );
    }
    return( ERR_NO_ERR );

} /* FTSRunCmds */

/*
 * FTSGetFirst - return first fts entry
 */
ft_src *FTSGetFirst( void )
{
    return( ftsHead );

} /* FTSGetFirst */

/*
 * FTSGetNext - return entry after given
 */
ft_src *FTSGetNext( ft_src *this )
{
    if( this ) {
        return( this->next );
    }
    return( this );

} /* FTSGetNext */

/*
 * FTSGetFirstTemplate - return 1st entry of template linked list
 */
template_ll *FTSGetFirstTemplate( ft_src *this )
{
    return( this->template_head );

} /* FTSGetFirstTemplate */

/*
 * FTSGetNextTemplate - return next entry in template linked list
 */
template_ll *FTSGetNextTemplate( template_ll *this )
{
    if( this ) {
        return( this->next );
    }
    return( this );

} /* FTSGetNextTemplate */

/*
 * FTSBarfData - generate source for current state
 */
void FTSBarfData( FILE *f )
{
    ft_src      *fts;
    cmd_ll      *cmd;
    template_ll *template;

    for( fts = ftsHead; fts != NULL; fts = fts->next ) {
        MyFprintf( f, "filetypesource" );

        template = fts->template_head;
        while( template ) {
            MyFprintf( f, " %s", template->data );
            template = template->next;
        }
        MyFprintf( f, "\n" );

        cmd = fts->cmd_head;
        while( cmd ) {
            MyFprintf( f, "    %s\n", cmd->data );
            cmd = cmd->next;
        }
        MyFprintf( f, "endfiletypesource\n\n" );
    }

} /* FTSBarfData */

/*
 * FTSMatchTemplate - return fts of an existing entry with given template list
 */
ft_src *FTSMatchTemplate( template_ll *template_head )
{
    ft_src      *fts;
    template_ll *tpCur, *tpNew;

    for( fts = ftsHead; fts != NULL; fts = fts->next ) {
        tpCur = fts->template_head;
        tpNew = template_head;
        do {
            if( strcmp( tpCur->data, tpNew->data ) ) {
                break;
            }
            tpCur = tpCur->next;
            tpNew = tpNew->next;
        } while( tpCur && tpNew );
        if( tpCur == NULL && tpNew == NULL ) {
            return( fts );
        }
    }
    return( NULL );

} /* FTSMatchTemplate */

void deleteTemplateList( template_ll *template_head )
{
    template_ll *tp, *tpnext;
    tpnext = template_head;
    do {
        tp = tpnext;
        tpnext = tp->next;
        MemFree( tp );
    } while( tpnext );
}

/*
 * FTSMatchTemplateData - return fts of entry with given template data
 */
ft_src *FTSMatchTemplateData( char *data )
{
    char            template_data[MAX_STR];
    template_ll     *templatePtr;
    template_ll     *template_head, *template_tail;
    ft_src          *fts;

    // build a template list
    template_head = template_tail = NULL;
    while( NextWord1( data, template_data ) > 0 ) {
        templatePtr = MemAlloc( sizeof( template_ll ) + strlen( template_data ) ); // +1 not needed
        strcpy( templatePtr->data, template_data );
        AddLLItemAtEnd( (ss **)&template_head, (ss **)&template_tail, (ss *)templatePtr );
    }

    fts = FTSMatchTemplate( template_head );

    // kill template list
    deleteTemplateList( template_head );

    return( fts );

} /* FTSMatchTemplateData */

void FTSKill( ft_src *fts )
{
    cmd_ll      *cmd, *cmdnext;

    DeleteLLItem( (ss **)&ftsHead, (ss **)&ftsTail, (ss *)fts );

    deleteTemplateList( fts->template_head );

    // cmd list may have 0 entries
    for( cmd = fts->cmd_head; cmd != NULL; cmd = cmdnext ) {
        cmdnext = cmd->next;
        MemFree( cmd );
    }
    MemFree( fts );
}

/*
 * FTSInit - call at program startup
 */
void FTSInit( void )
{
    ftsHead = NULL;
    ftsTail = NULL;

} /* FTSInit */

/*
 * FTSFini - call at program startup
 */
void FTSFini( void )
{
    while( ftsHead ) {
        FTSKill( ftsHead );
    }

} /* FTSFini */

