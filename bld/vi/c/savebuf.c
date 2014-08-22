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

#ifdef __WIN__
extern int  AddLineToClipboard( char *data, int scol, int ecol );
extern int  AddFcbsToClipboard( fcb_list *fcbs );
extern int  GetClipboardSavebuf( savebuf *clip );
extern bool IsClipboardEmpty( void );
#endif

/*
 * freeSavebuf - release savebuf data
 */
static void freeSavebuf( savebuf *tmp )
{
    fcb     *cfcb, *tfcb;

    switch( tmp->type ) {
    case SAVEBUF_NOP:
        break;
    case SAVEBUF_LINE:
        MemFree( tmp->u.data );
        break;
    case SAVEBUF_FCBS:
        for( cfcb = tmp->u.fcbs.head; cfcb != NULL; cfcb = tfcb ) {
            tfcb = cfcb->next;
            FreeEntireFcb( cfcb );
        }
        break;
    }

} /* freeSavebuf */

/*
 * rotateSavebufs - rotate save buffers forward, disposing of last
 */
static void rotateSavebufs( int start )
{
    int i;

    freeSavebuf( &Savebufs[MAX_SAVEBUFS - 1] );

    /*
     * now, rotate the buffers forward
     */
    for( i = MAX_SAVEBUFS - 1; i > start; i-- ) {
        memcpy( &(Savebufs[i]), &(Savebufs[i - 1]), SAVEBUF_SIZE );
    }

} /* rotateSavebufs */

/*
 * insertGenericSavebuf - insert contents of a savebuf before/after current pos
 */
static vi_rc insertGenericSavebuf( int buf, bool afterflag )
{
#ifdef __WIN__
    savebuf     clip;
#endif
    savebuf     *tmp;
    fcb_list    fcblist;
    fcb         *end;
    int         i, scol, len;
    int         maxCursor;
    vi_rc       rc;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    if( EditFlags.Modeless ) {
        DeleteSelectedRegion();
        SelRgn.selected = false;
    }

#ifdef __WIN__
    if( buf == CLIPBOARD_SAVEBUF ) {
        rc = GetClipboardSavebuf( &clip );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        tmp = &clip;
    } else
#endif
    if( buf >= MAX_SAVEBUFS ) {
        tmp = &SpecialSavebufs[buf - MAX_SAVEBUFS];
    } else {
        tmp = &Savebufs[buf];
    }
    switch( tmp->type ) {
    case SAVEBUF_NOP:
        rc = ERR_EMPTY_SAVEBUF;
        break;
    case SAVEBUF_LINE:
        /*
         * get starting data
         */
        len = strlen( tmp->u.data );
        if( len + CurrentLine->len >= EditVars.MaxLine ) {
            rc = ERR_LINE_FULL;
            break;
        }
        if( afterflag ) {
            scol = CurrentPos.column;
        } else {
            scol = CurrentPos.column - 1;
        }
        CurrentLineReplaceUndoStart();
        GetCurrentLine();

        /*
         * open up line and copy in data
         */
        if( WorkLine->len == 0 ) {
            scol = 0;
        }
        for( i = WorkLine->len; i >= scol; i-- ) {
            WorkLine->data[i + len] = WorkLine->data[i];
        }
        for( i = 0; i < len; i++ ) {
            WorkLine->data[i + scol] = tmp->u.data[i];
        }
        WorkLine->len += len;
        DisplayWorkLine( true );
        ReplaceCurrentLine();
        CurrentLineReplaceUndoEnd( true );
        scol += len + 1;

        maxCursor = CurrentLine->len;
        if( EditFlags.Modeless ) {
            maxCursor++;
        }
        if( scol > maxCursor ) {
            scol = maxCursor;
        }
        rc = GoToColumn( scol, maxCursor );
        break;

    case SAVEBUF_FCBS:
        fcblist.head = NULL;
        fcblist.tail = NULL;
        end = tmp->u.fcbs.tail->next;
        tmp->u.fcbs.tail->next = NULL;
        CreateDuplicateFcbList( tmp->u.fcbs.head, &fcblist );
        tmp->u.fcbs.tail->next = end;

        if( !EditFlags.LineBased ) {
            rc = InsertLinesAtCursor( &fcblist, UndoStack );
        } else {
            if( afterflag) {
                rc = InsertLines( CurrentPos.line, &fcblist, UndoStack );
            } else {
                rc = InsertLines( CurrentPos.line - 1, &fcblist, UndoStack );
            }
        }
        break;
    }
#ifdef __WIN__
    if( tmp == &clip ) {
        freeSavebuf( &clip );
    }
#endif

    EditFlags.Dotable = true;

    return( rc );

} /* insertGenericSavebuf */

/*
 * InsertSavebufBefore - insert contents of current savebuf before current pos
 */
vi_rc InsertSavebufBefore( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        return( insertGenericSavebuf( CurrentSavebuf, false ) );
    } else {
        return( insertGenericSavebuf( SavebufNumber, false ) );
    }

} /* InsertSavebufBefore */

/*
 * InsertSavebufBefore2 - alternate insert savebuf (cuz of windows)
 */
vi_rc InsertSavebufBefore2( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
#ifdef __WIN__
        return( insertGenericSavebuf( CLIPBOARD_SAVEBUF, false ) );
#else
        return( insertGenericSavebuf( CurrentSavebuf, false ) );
#endif
    } else {
        return( insertGenericSavebuf( SavebufNumber, false ) );
    }

} /* InsertSavebufBefore2 */

/*
 * InsertSavebufAfter - insert contents of current savebuf after current pos
 */
vi_rc InsertSavebufAfter( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        return( insertGenericSavebuf( CurrentSavebuf, true  ) );
    } else {
        return( insertGenericSavebuf( SavebufNumber, true  ) );
    }

} /* InsertSavebufAfter */

/*
 * InsertSavebufAfter2 - alternate insert savebuf (cuz of windows)
 */
vi_rc InsertSavebufAfter2( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
#ifdef __WIN__
        return( insertGenericSavebuf( CLIPBOARD_SAVEBUF, true ) );
#else
        return( insertGenericSavebuf( CurrentSavebuf, true ) );
#endif
    } else {
        return( insertGenericSavebuf( SavebufNumber, true ) );
    }

} /* InsertSavebufAfter2 */

/*
 * GetSavebufString - get a string made up of stuff in a savebuf
 */
vi_rc GetSavebufString( char **data )
{
#ifdef __WIN__
    savebuf     clip;
#endif
    savebuf     *tmp;
    fcb         *cfcb;
    line        *cline;
    vi_rc       rc;
    long        len;

    /*
     * fetch the savebuf
     */
    rc = DoSavebufNumber();
    if( rc != GOT_A_SAVEBUF ) {
        if( rc == ERR_NO_ERR ) {
            rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
        }
        return( rc );
    }
#ifdef __WIN__
    if( SavebufNumber == CLIPBOARD_SAVEBUF ) {
        rc = GetClipboardSavebuf( &clip );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        tmp = &clip;
    } else
#endif
    if( SavebufNumber >= MAX_SAVEBUFS ) {
        tmp = &SpecialSavebufs[SavebufNumber - MAX_SAVEBUFS];
    } else {
        tmp = &Savebufs[SavebufNumber];
    }
    SavebufNumber = NO_SAVEBUF;

    /*
     * get length of stuff
     */
    len = 0L;
    switch( tmp->type ) {
    case SAVEBUF_NOP:
        return( ERR_EMPTY_SAVEBUF );
    case SAVEBUF_LINE:
        len = strlen( tmp->u.data );
        break;
    case SAVEBUF_FCBS:
        for( cfcb = tmp->u.fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
            len += FcbSize( cfcb );
        }
        break;
    }
    rc = ERR_NO_ERR;
    if( len > MAX_STR * 4 ) {
        rc = ERR_SAVEBUF_TOO_BIG;
    } else {
        *data = MemAlloc( len );
        switch( tmp->type ) {
        case SAVEBUF_LINE:
            strcpy( *data, tmp->u.data );
            break;
        case SAVEBUF_FCBS:
            **data = 0;
            for( cfcb = tmp->u.fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
                FetchFcb( cfcb );
                for( cline = cfcb->lines.head; cline != NULL; cline = cline->next ) {
                    strcat( *data, cline->data );
                    strcat( *data, "\\n" );
                }
            }
            break;
        }
    }
#ifdef __WIN__
    if( tmp == &clip ) {
        freeSavebuf( &clip );
    }
#endif
    return( rc );

} /* GetSavebufString */

/*
 * InitSavebufs - initalize save buffers
 */
void InitSavebufs( void )
{
    int i;

    for( i = MAX_SAVEBUFS - 1; i >= 0; i-- ) {
        Savebufs[i].type = SAVEBUF_NOP;
        Savebufs[i].u.fcbs.head = NULL;
        Savebufs[i].u.fcbs.tail = NULL;
    }

} /* InitSavebufs */

void FiniSavebufs( void )
{
    int i;

    for( i = MAX_SAVEBUFS - 1; i >= 0; i-- ) {
        freeSavebuf( &Savebufs[i] );
    }

} /* FiniSavebufs */

/*
 * AddLineToSavebuf - add a single line to save buffer
 */
void AddLineToSavebuf( char *data, int scol, int ecol )
{
    savebuf     *tmp;
    int         i, len, j;

    /*
     * set up for copy
     */
    if( scol > ecol ) {
        i = scol;
        scol = ecol;
        ecol = i;
    }
    len = ecol - scol + 1;

    /*
     * set to appropriate savebuf and rotate others forward
     */
#ifdef __WIN__
    if( SavebufNumber == CLIPBOARD_SAVEBUF ) {
        AddLineToClipboard( data, scol, ecol );
        LastSavebuf = 0;
        return;
    } else {
#endif
        if( SavebufNumber == NO_SAVEBUF ) {
            j = CurrentSavebuf;
        } else {
            j = SavebufNumber;
        }
#ifdef __WIN__
    }
#endif
    if( j >= MAX_SAVEBUFS ) {
        LastSavebuf = (char) j + (char) 'a' - (char) MAX_SAVEBUFS;
        tmp = &SpecialSavebufs[j - MAX_SAVEBUFS];
    } else {
        LastSavebuf = (char) j + (char) '1';
        tmp = &Savebufs[j];
        rotateSavebufs( j );
    }
    tmp->type = SAVEBUF_LINE;

    /*
     * get and copy buffer
     */
    tmp->u.data = MemAlloc( len + 1 );
    for( i = scol; i <= ecol; i++ ) {
        tmp->u.data[i - scol] = data[i];
    }
    tmp->u.data[len] = 0;

} /* AddLineToSavebuf */

/*
 * AddSelRgnToSavebuf - copy selected text to savebuf
 */
vi_rc AddSelRgnToSavebuf( void )
{
    char    buf[] = "0";
    range   r;
    vi_rc   rc;

    rc = GetSelectedRegion( &r );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    NormalizeRange( &r );
    SetSavebufNumber( buf );
    return( Yank( &r ) );
}

/*
 * AddSelRgnToSavebufAndDelete - copy selected text to savebuf, then kill it
 */
vi_rc AddSelRgnToSavebufAndDelete( void )
{
    AddSelRgnToSavebuf();
    DeleteSelectedRegion();
    return( ERR_NO_ERR );
}

/*
 * AddFcbsToSavebuf - add fcb block to save buffer
 */
void AddFcbsToSavebuf( fcb_list *fcblist, bool duplflag )
{
    int         j;
    savebuf     *tmp;
    fcb         *cfcb;

    /*
     * set to appropriate savebuf and rotate others forward
     */
#ifdef __WIN__
    if( SavebufNumber == CLIPBOARD_SAVEBUF ) {
        AddFcbsToClipboard( fcblist );
        LastSavebuf = 0;
        return;
    } else {
#endif
        if( SavebufNumber == NO_SAVEBUF ) {
            j = CurrentSavebuf;
        } else {
            j = SavebufNumber;
        }
#ifdef __WIN__
    }
#endif
    if( j >= MAX_SAVEBUFS ) {
        LastSavebuf = (char) j + (char) 'a' - (char) MAX_SAVEBUFS;
        tmp = &SpecialSavebufs[j - MAX_SAVEBUFS];
    } else {
        LastSavebuf = (char) j + (char) '1';
        tmp = &Savebufs[j];
        rotateSavebufs( j );
    }

    tmp->type = SAVEBUF_FCBS;
    if( duplflag ) {
        tmp->u.fcbs.head = NULL;
        tmp->u.fcbs.tail = NULL;
        cfcb = fcblist->tail->next;
        fcblist->tail->next = NULL;
        CreateDuplicateFcbList( fcblist->head, &tmp->u.fcbs );
        fcblist->tail->next = cfcb;
    } else {
        tmp->u.fcbs = *fcblist;
    }

} /* AddFcbsToSavebuf */

/*
 * SwitchSavebuf - switch current save buffer
 */
vi_rc SwitchSavebuf( void )
{
    int         buf, i;
    linenum     lcnt;
    savebuf     *tmp;
    char        *data;
    fcb         *cfcb;

    /*
     * validate savebuf
     */
    buf = -1;
    for( i = 0; i < MAX_SAVEBUFS; i++ ) {
        if( LastEvent == SavebufBound[i] ){
            buf = i;
            break;
        }
    }
    if( buf < 0 ) {
        return( ERR_NO_ERR );
    }
    CurrentSavebuf = buf;
    tmp = &Savebufs[buf];
    data = NULL;
    switch( tmp->type ) {
    case SAVEBUF_NOP:
        Message1( "Buffer %d now active. (empty buffer)", buf + 1 );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    case SAVEBUF_LINE:
        data = tmp->u.data;
        Message1( "Buffer %d active, %d characters:", buf + 1,
                  strlen( tmp->u.data ) );
        break;
    case SAVEBUF_FCBS:
        cfcb = tmp->u.fcbs.head;
        FetchFcb( cfcb );
        data = cfcb->lines.head->data;
        lcnt = 0;
        for( ; cfcb != NULL; cfcb = cfcb->next ) {
            lcnt += cfcb->end_line - cfcb->start_line + 1;
        }
        Message1( "Buffer %d active, %l lines:", buf + 1, lcnt );
        break;
    }
    Message2( "\"%s\"", data );

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* SwitchSavebuf */

/*
 * DoSavebufNumber - get a savebuf number
 */
vi_rc DoSavebufNumber( void )
{
    vi_key      key;
    char        buff[2];
    vi_rc       rc;

    /*
     * get savebuf to use
     */
    key = GetNextEvent( false );
    if( key == VI_KEY( ESC ) ) {
        return( ERR_NO_ERR );
    }
    buff[0] = key;
    buff[1] = 0;
    rc = SetSavebufNumber( buff );
    if( rc == ERR_NO_ERR ) {
        rc = GOT_A_SAVEBUF;
    }
    return( rc );

} /* DoSavebufNumber */

/*
 * SetSavebufNumber - set savebuf number from a string
 */
vi_rc SetSavebufNumber( char *data )
{
    char        st[MAX_STR];

    SavebufNumber = NO_SAVEBUF;
    if( NextWord1( data, st ) > 0 ) {

        if( st[1] != 0 ) {
            Error( GetErrorMsg( ERR_INVALID_SAVEBUF), st[0] );
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
#ifdef __WIN__
        if( st[0] == '0' ) {
            SavebufNumber = CLIPBOARD_SAVEBUF;
        } else {
#endif
            if( st[0] >= '1' && st[0] <= '9' ) {
                SavebufNumber = st[0] - '1';
            } else if( st[0] >= 'a' && st[0] <= 'z' ) {
                SavebufNumber = st[0] - 'a' + MAX_SAVEBUFS;
            } else {
                Error( GetErrorMsg( ERR_INVALID_SAVEBUF), st[0] );
                return( DO_NOT_CLEAR_MESSAGE_WINDOW );
            }
#ifdef __WIN__
        }
#endif

    }
    return( ERR_NO_ERR );

} /* SetSavebufNumber */

/*
 * IsEmptySavebuf - check if a specified savebuf is empty
 */
bool IsEmptySavebuf( char ch )
{
    int bufnum;

#ifdef __WIN__
    if( ch == '0' ) {
        return( IsClipboardEmpty() );
    } else {
#endif
        if( ch >= '1' && ch <= '9' ) {
            bufnum = ch - '1';
            if( Savebufs[bufnum].type == SAVEBUF_NOP ) {
                return( true );
            }
            return( false );
        } else if( ch >= 'a' && ch <= 'z' ) {
            bufnum = ch - 'a';
            if( SpecialSavebufs[bufnum].type == SAVEBUF_NOP ) {
                return( true );
            }
        }
#ifdef __WIN__
    }
#endif

    return( false );

} /* IsEmptySavebuf */
