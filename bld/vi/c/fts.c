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
#include <assert.h>
#include "vi.h"
#include "fts.h"
#include "source.h"

static  ft_src  *ftsHead;
static  ft_src  *ftsTail;

/*
 * FTSStart - start a new fts; EditFlags.FileTypeSource will suck in commands
 */
int FTSStart( char *data )
{
    char        template_data[ MAX_STR ];
    template_ll *templatePtr;
    ft_src      *fts, *ftsOld;

    fts = MemAlloc( sizeof( ft_src ) );
    fts->cmd_head = fts->cmd_tail = NULL;
    fts->template_head = fts->template_tail = NULL;
    while( NextWord1( data, template_data ) > 0 ) {
        templatePtr = MemAlloc( sizeof( template_ll ) +
                                strlen( template_data ) ); // +1 not needed
        strcpy( templatePtr->data, template_data );
        AddLLItemAtEnd( &fts->template_head,
                        &fts->template_tail, templatePtr );
    }

    if( ftsOld = FTSMatchTemplate( fts->template_head ) ) {
        FTSKill( ftsOld );
    }
    AddLLItemAtEnd( &ftsHead, &ftsTail, fts );

    EditFlags.FileTypeSource = TRUE;

    return( ERR_NO_ERR );
}

/*
 * FTSAddCmd - add a 1-line command to the current (tail) fts
 */
int FTSAddCmd( char *data, int setkilled )
{
    char    cmd_data[ MAX_STR ];
    cmd_ll  *cmd;

    assert( ftsTail );
    assert( EditFlags.FileTypeSource );

    // Source gets cute & trashes "set "...
    if( setkilled >= SRC_T_NULL + 1 ) {
        strcpy( cmd_data, "set" );
        strcat( cmd_data, data );
    } else {
        strcpy( cmd_data, data );
    }

    cmd = MemAlloc( sizeof( cmd_ll ) + strlen( cmd_data ) );
    strcpy( cmd->data, cmd_data );

    AddLLItemAtEnd( &ftsTail->cmd_head, &ftsTail->cmd_tail, cmd );

    return( ERR_NO_ERR );
}

/*
 * FTSAddBoolean - add a boolean set to the current (tail) fts
 */
int FTSAddBoolean( bool val, char *name )
{
    char    cmd[ MAX_SRC_LINE ] = "set ";
    if( !val ) {
        strcat( cmd, "no" );
    }
    strcat( cmd, name );
    return( FTSAddCmd( cmd, 0 ) );
}

/*
 * FTSAddInt - add an integral set to the current (tail) fts
 */
int FTSAddInt( int val, char *name )
{
    char    cmd[ MAX_SRC_LINE ];
    sprintf( cmd, "set %s %d", name, val );
    return( FTSAddCmd( cmd, 0 ) );
}

/*
 * FTSAddChar - add a character set to the current (tail) fts
 */
int FTSAddChar( char val, char *name )
{
    char    cmd[ MAX_SRC_LINE ];
    sprintf( cmd, "set %s %c", name, val );
    return( FTSAddCmd( cmd, 0 ) );
}

/*
 * FTSAddStr - add a string set to the current (tail) fts
 */
int FTSAddStr( char *val, char *name )
{
    char    cmd[ MAX_SRC_LINE ];
    sprintf( cmd, "set %s %s", name, val );
    return( FTSAddCmd( cmd, 0 ) );
}

/*
 * FTSEnd - done with this filetypesource entry
 */
int FTSEnd( void )
{
    EditFlags.FileTypeSource = FALSE;
    return( ERR_NO_ERR );
}

int runCmds( ft_src *fts )
{
    char    cmd_data[ MAX_STR ];
    int     rc;
    cmd_ll  *cmd;

    cmd = fts->cmd_head;
    while( cmd ) {
        strcpy( cmd_data, cmd->data );
        rc = RunCommandLine( cmd_data );
#if 0
        if( rc != ERR_NO_ERR && rc != DO_NOT_CLEAR_MESSAGE_WINDOW ) {
            break;
        }
#endif
        cmd = cmd->next;
    }
    return( rc );
}

/*
 * FTSRunCmds - run commands if 'name' has a registered file type
 */
int FTSRunCmds( char *name )
{
    template_ll *template;
    ft_src      *fts;
    bool        oldScript, oldQuiet, oldHold;

    oldScript = EditFlags.ScriptIsCompiled;
    oldQuiet = EditFlags.Quiet;
    oldHold = EditFlags.DisplayHold;
    EditFlags.ScriptIsCompiled = FALSE;
    EditFlags.Quiet = TRUE;
    EditFlags.DisplayHold = TRUE;

    name = StripPath( name );

    if( name != NULL ) {
        fts = ftsHead;
        while( fts ) {
            template = fts->template_head;
            while( template ) {
                if( FileTemplateMatch( name, template->data ) ) {
                    runCmds( fts );
                    fts = NULL;
                    break;
                }
                template = template->next;
            }
            if( fts ) {
                fts = fts->next;
            }
        }
    }

    EditFlags.ScriptIsCompiled = oldScript;
    EditFlags.Quiet = oldQuiet;
    EditFlags.DisplayHold = oldHold;

    return( ERR_NO_ERR );
}

/*
 * FTSGetFirst - return first fts entry
 */
ft_src *FTSGetFirst( void )
{
    return( ftsHead );
}

/*
 * FTSGetNext - return entry after given
 */
ft_src *FTSGetNext( ft_src *this )
{
    if( this ) {
        return( this->next );
    }
    return( this );
}

/*
 * FTSGetFirstTemplate - return 1st entry of template linked list
 */
template_ll *FTSGetFirstTemplate( ft_src *this )
{
    return( this->template_head );
}

/*
 * FTSGetNextTemplate - return next entry in template linked list
 */
template_ll *FTSGetNextTemplate( template_ll *this )
{
    if( this ) {
        return( this->next );
    }
    return( this );
}

/*
 * FTSBarfData - generate source for current state
 */
void FTSBarfData( FILE *f )
{
    ft_src      *fts;
    cmd_ll      *cmd;
    template_ll *template;

    fts = ftsHead;
    while( fts ) {
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

        fts = fts->next;
    }
}

/*
 * FTSMatchTemplate - return fts of an existing entry with given template list
 */
ft_src *FTSMatchTemplate( template_ll *template_head )
{
    ft_src      *fts;
    template_ll *tpCur, *tpNew;

    fts = ftsHead;
    while( fts ) {
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
        fts = fts->next;
    }
    return( NULL );
}

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
    char            template_data[ MAX_STR ];
    template_ll     *templatePtr;
    template_ll     *template_head, *template_tail;
    ft_src          *fts;

    // build a template list
    template_head = template_tail = NULL;
    while( NextWord1( data, template_data ) > 0 ) {
        templatePtr = MemAlloc( sizeof( template_ll ) +
                                strlen( template_data ) ); // +1 not needed
        strcpy( templatePtr->data, template_data );
        AddLLItemAtEnd( &template_head, &template_tail, templatePtr );
    }

    fts = FTSMatchTemplate( template_head );

    // kill template list
    deleteTemplateList( template_head );

    return( fts );
}

void FTSKill( ft_src *fts )
{
    cmd_ll      *cmd, *cmdnext;

    DeleteLLItem( &ftsHead, &ftsTail, fts );

    deleteTemplateList( fts->template_head );

    cmdnext = fts->cmd_head;
    do {
        if( !cmdnext ) {
            // cmd list may have 0 entries
            break;
        }
        cmd = cmdnext;
        cmdnext = cmd->next;
        MemFree( cmd );
    } while( cmdnext );

    MemFree( fts );
}

/*
 * FTSInit - call at program startup
 */
void FTSInit( void )
{
    ftsHead = NULL;
    ftsTail = NULL;
}

/*
 * FTSFini - call at program startup
 */
void FTSFini( void )
{
    while( ftsHead ) {
        FTSKill( ftsHead );
    }
}

