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
#include "keys.h"

#ifdef __WIN__
extern int AddLineToClipboard( char *data, int scol, int ecol );
extern int AddFcbsToClipboard( fcb *head, fcb *tail );
extern int GetClipboardSavebuf( savebuf *clip );
extern bool IsClipboardEmpty( void );
#endif

/*
 * freeSavebuf - release savebuf data
 */
static void freeSavebuf( savebuf *tmp )
{
    fcb *cfcb,*tfcb;

    switch( tmp->type ) {
    case SAVEBUF_NOP:
        break;
    case SAVEBUF_LINE:
        MemFree( tmp->first.data );
        break;
    case SAVEBUF_FCBS:
        cfcb = tmp->first.fcb_head;
        while( cfcb != NULL ) {
            tfcb = cfcb->next;
            FreeEntireFcb( cfcb );
            cfcb = tfcb;
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

    freeSavebuf( &Savebufs[ MAX_SAVEBUFS-1 ] );

    /*
     * now, rotate the buffers forward
     */
    for( i=MAX_SAVEBUFS-1; i > start ;i-- ) {
        memcpy( &(Savebufs[i]),&(Savebufs[i-1]), SAVEBUF_SIZE );
    }

} /* rotateSavebufs */

/*
 * insertGenericSavebuf - insert contents of a savebuf before/after current pos
 */
static int insertGenericSavebuf( int buf, int afterflag )
{
#ifdef __WIN__
    savebuf     clip;
#endif
    savebuf     *tmp;
    fcb         *head=NULL,*tail=NULL,*end;
    int         rc,i,scol,len;
    int         maxCursor;

    if( rc = ModificationTest() ) {
        return( rc );
    }

    if( EditFlags.Modeless ) {
        DeleteSelectedRegion();
        SelRgn.selected = FALSE;
    }

#ifdef __WIN__
    if( buf == CLIPBOARD_SAVEBUF ) {
        rc = GetClipboardSavebuf( &clip );
        if( rc ) {
            return( rc );
        }
        tmp = &clip;
    } else
#endif
    if( buf >= MAX_SAVEBUFS ) {
        tmp = &SpecialSavebufs[buf-MAX_SAVEBUFS];
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
        len = strlen( tmp->first.data );
        if( len + CurrentLine->len >= MaxLine ) {
            rc = ERR_LINE_FULL;
            break;
        }
        if( afterflag ) {
            scol = CurrentColumn;
        } else {
            scol = CurrentColumn-1;
        }
        CurrentLineReplaceUndoStart();
        GetCurrentLine();

        /*
         * open up line and copy in data
         */
        if( WorkLine->len == 0 ) {
            scol = 0;
        }
        for( i=WorkLine->len;i>=scol;i-- ) {
            WorkLine->data[i+len] = WorkLine->data[i];
        }
        for( i=0;i<len;i++ ) {
            WorkLine->data[i+scol] = tmp->first.data[i];
        }
        WorkLine->len += len;
        DisplayWorkLine( TRUE );
        ReplaceCurrentLine();
        CurrentLineReplaceUndoEnd( TRUE );
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
        end = tmp->fcb_tail->next;
        tmp->fcb_tail->next = NULL;
        CreateDuplicateFcbList( tmp->first.fcb_head, &head, &tail );
        tmp->fcb_tail->next = end;

        if( !EditFlags.LineBased ) {
            rc = InsertLinesAtCursor( head, tail, UndoStack );
        } else {
            if( afterflag) {
                rc = InsertLines( CurrentLineNumber, head, tail, UndoStack );
            } else {
                rc = InsertLines( CurrentLineNumber-1, head, tail, UndoStack );
            }
        }
        break;
    }
#ifdef __WIN__
    if( tmp == &clip ) {
        freeSavebuf( &clip );
    }
#endif

    EditFlags.Dotable = TRUE;

    return( rc );

} /* insertGenericSavebuf */

/*
 * InsertSavebufBefore - insert contents of current savebuf before current pos
 */
int InsertSavebufBefore( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        return( insertGenericSavebuf( CurrentSavebuf, FALSE ) );
    } else {
        return( insertGenericSavebuf( SavebufNumber, FALSE ) );
    }

} /* InsertSavebufBefore */

/*
 * InsertSavebufBefore2 - alternate insert savebuf (cuz of windows)
 */
int InsertSavebufBefore2( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        #ifdef __WIN__
            return( insertGenericSavebuf( CLIPBOARD_SAVEBUF, FALSE ) );
        #else
            return( insertGenericSavebuf( CurrentSavebuf, FALSE ) );
        #endif
    } else {
        return( insertGenericSavebuf( SavebufNumber, FALSE ) );
    }

} /* InsertSavebufBefore2 */

/*
 * InsertSavebufAfter - insert contents of current savebuf after current pos
 */
int InsertSavebufAfter( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        return( insertGenericSavebuf( CurrentSavebuf, TRUE  ) );
    } else {
        return( insertGenericSavebuf( SavebufNumber, TRUE  ) );
    }

} /* InsertSavebufAfter */

/*
 * InsertSavebufAfter2 - alternate insert savebuf (cuz of windows)
 */
int InsertSavebufAfter2( void )
{
    if( SavebufNumber == NO_SAVEBUF ) {
        #ifdef __WIN__
            return( insertGenericSavebuf( CLIPBOARD_SAVEBUF, TRUE ) );
        #else
            return( insertGenericSavebuf( CurrentSavebuf, TRUE ) );
        #endif
    } else {
        return( insertGenericSavebuf( SavebufNumber, TRUE ) );
    }

} /* InsertSavebufAfter2 */

/*
 * GetSavebufString - get a string made up of stuff in a savebuf
 */
int GetSavebufString( char **data )
{
#ifdef __WIN__
    savebuf     clip;
#endif
    savebuf     *tmp;
    fcb         *cfcb;
    line        *cline;
    int         rc;
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
        if( rc ) {
            return( rc );
        }
        tmp = &clip;
    } else
#endif
    if( SavebufNumber >= MAX_SAVEBUFS ) {
        tmp = &SpecialSavebufs[SavebufNumber-MAX_SAVEBUFS];
    } else {
        tmp = &Savebufs[SavebufNumber];
    }
    SavebufNumber = NO_SAVEBUF;

    /*
     * get length of stuff
     */
    switch( tmp->type ) {
    case SAVEBUF_NOP:
        return( ERR_EMPTY_SAVEBUF );
    case SAVEBUF_LINE:
        len = strlen( tmp->first.data );
        break;
    case SAVEBUF_FCBS:
        cfcb = tmp->first.fcb_head;
        len = 0L;
        while( cfcb != NULL ) {
            len += FcbSize( cfcb );
            cfcb = cfcb->next;
        }
        break;
    }
    rc = ERR_NO_ERR;
    if( len > MAX_STR*4 ) {
        rc = ERR_SAVEBUF_TOO_BIG;
    } else {
        *data = MemAlloc( len );
        switch( tmp->type ) {
        case SAVEBUF_LINE:
            strcpy( *data, tmp->first.data );
            break;
        case SAVEBUF_FCBS:
            cfcb = tmp->first.fcb_head;
            **data = 0;
            while( cfcb != NULL ) {
                FetchFcb( cfcb );
                cline = cfcb->line_head;
                while( cline != NULL ) {
                    strcat( *data, cline->data );
                    strcat( *data, "\\n" );
                    cline = cline->next;
                }
                cfcb = cfcb->next;
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

    for( i=MAX_SAVEBUFS-1;i>=0;i-- ) {
        Savebufs[i].type = SAVEBUF_NOP;
        Savebufs[i].first.data = NULL;
        Savebufs[i].fcb_tail = NULL;
    }

} /* InitSavebufs */

void FiniSavebufs( void )
{
    int i;

    for( i=MAX_SAVEBUFS-1;i>=0;i-- ) {
        freeSavebuf( &Savebufs[i] );
    }

} /* FiniSavebufs */

/*
 * AddLineToSavebuf - add a single line to save buffer
 */
void AddLineToSavebuf( char *data, int scol, int ecol )
{
    savebuf     *tmp;
    int         i,len,j;

    /*
     * set up for copy
     */
    if( scol > ecol ) {
        i = scol;
        scol = ecol;
        ecol = i;
    }
    len = ecol-scol+1;

    /*
     * set to appropriate savebuf and rotate others forward
     */
#ifdef __WIN__
    if( SavebufNumber == CLIPBOARD_SAVEBUF ) {
        AddLineToClipboard( data, scol, ecol );
        LastSavebuf = 0;
        return;
    } else
#endif
    if( SavebufNumber == NO_SAVEBUF ) {
        j = CurrentSavebuf;
    } else {
        j = SavebufNumber;
    }
    if( j >= MAX_SAVEBUFS ) {
        LastSavebuf = (char) j + (char) 'a' - (char) MAX_SAVEBUFS;
        tmp = &SpecialSavebufs[ j - MAX_SAVEBUFS ];
    } else {
        LastSavebuf = (char) j + (char) '1';
        tmp = &Savebufs[ j ];
        rotateSavebufs( j );
    }
    tmp->type = SAVEBUF_LINE;

    /*
     * get and copy buffer
     */
    tmp->first.data = MemAlloc( len+1 );
    for( i=scol;i<=ecol;i++ ) {
        tmp->first.data[i-scol] = data[i];
    }
    tmp->first.data[len] = 0;

} /* AddLineToSavebuf */

/*
 * AddSelRgnToSavebuf - copy selected text to savebuf
 */
void AddSelRgnToSavebuf( void )
{
    char buf[] = "0";
    range r;

    if( GetSelectedRegion( &r ) != ERR_NO_ERR ) {
        return;
    }
    NormalizeRange( &r );
    SetSavebufNumber( buf );
    Yank( &r );
}

/*
 * AddSelRgnToSavebufAndDelete - copy selected text to savebuf, then kill it
 */
void AddSelRgnToSavebufAndDelete( void )
{
    AddSelRgnToSavebuf();
    DeleteSelectedRegion();
}

/*
 * AddFcbsToSavebuf - add fcb block to save buffer
 */
void AddFcbsToSavebuf( fcb *head, fcb *tail, int duplflag )
{
    int         j;
    savebuf     *tmp;
    fcb         *cfcb,*nhead=NULL,*ntail=NULL;

    /*
     * set to appropriate savebuf and rotate others forward
     */
#ifdef __WIN__
    if( SavebufNumber == CLIPBOARD_SAVEBUF ) {
        AddFcbsToClipboard( head, tail );
        LastSavebuf = 0;
        return;
    } else
#endif
    if( SavebufNumber == NO_SAVEBUF ) {
        j = CurrentSavebuf;
    } else {
        j = SavebufNumber;
    }
    if( j >= MAX_SAVEBUFS ) {
        LastSavebuf = (char) j + (char) 'a' - (char) MAX_SAVEBUFS;
        tmp = &SpecialSavebufs[ j - MAX_SAVEBUFS ];
    } else {
        LastSavebuf = (char) j + (char) '1';
        tmp = &Savebufs[ j ];
        rotateSavebufs( j );
    }

    tmp->type = SAVEBUF_FCBS;
    if( duplflag ) {
        cfcb = tail->next;
        tail->next = NULL;
        CreateDuplicateFcbList( head, &nhead, &ntail );
        tail->next = cfcb;
        tmp->first.fcb_head = nhead;
        tmp->fcb_tail = ntail;
    } else {
        tmp->first.fcb_head = head;
        tmp->fcb_tail = tail;
    }

} /* AddFcbsToSavebuf */

/*
 * SwitchSavebuf - switch current save buffer
 */
int SwitchSavebuf( void  )
{
    int         buf,i;
    linenum     lcnt;
    savebuf     *tmp;
    char        *data;
    fcb         *cfcb;

    /*
     * validate savebuf
     */
    buf = -1;
    for( i=0;i< MAX_SAVEBUFS;i++ ) {
        if( LastEvent == SavebufBound[i] ){
            buf = i;
            break;
        }
    }
    if( buf < 0 ) {
        return( ERR_NO_ERR );
    }
    CurrentSavebuf = buf;
    tmp = &Savebufs[ buf ];
    switch( tmp->type ) {
    case SAVEBUF_NOP:
        Message1( "Buffer %d now active. (empty buffer)",buf+1 );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    case SAVEBUF_LINE:
        data = tmp->first.data;
        Message1( "Buffer %d active, %d characters:",buf+1, strlen( tmp->first.data ) );
        break;
    case SAVEBUF_FCBS:
        cfcb = tmp->first.fcb_head;
        FetchFcb( cfcb );
        data = cfcb->line_head->data;
        lcnt = 0;
        while( cfcb != NULL ) {
            lcnt += cfcb->end_line - cfcb->start_line+1;
            cfcb = cfcb->next;
        }
        Message1( "Buffer %d active, %l lines:",buf+1, lcnt );
        break;
    }
    Message2( "\"%s\"",data );

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* SwitchSavebuf */

/*
 * DoSavebufNumber - get a savebuf number
 */
int DoSavebufNumber( void )
{
    int         i,rc;
    char        buff[2];

    /*
     * get savebuf to use
     */
    i = GetNextEvent( FALSE );
    if( i == VI_KEY( ESC ) ) {
        return( ERR_NO_ERR );
    }
    buff[0] = i;
    buff[1] = 0;
    rc = SetSavebufNumber( buff );
    if( !rc ) {
        rc = GOT_A_SAVEBUF;
    }
    return( rc );

} /* DoSavebufNumber */

/*
 * SetSavebufNumber - set savebuf number from a string
 */
int SetSavebufNumber( char *data )
{
    char        st[MAX_STR];

    SavebufNumber = NO_SAVEBUF;
    if( NextWord1( data, st ) > 0 ) {

        if( st[1] != 0 ) {
            Error( GetErrorMsg( ERR_INVALID_SAVEBUF) , st[0] );
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
#ifdef __WIN__
        if( st[0] == '0' ) {
            SavebufNumber = CLIPBOARD_SAVEBUF;
        } else
#endif
        if( st[0] >= '1' && st[0] <= '9' ) {
            SavebufNumber = st[0] - '1';
        } else if( st[0] >= 'a' && st[0] <= 'z' ) {
            SavebufNumber = st[0] - 'a' + MAX_SAVEBUFS;
        } else {
            Error( GetErrorMsg( ERR_INVALID_SAVEBUF) , st[0] );
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }

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
    } else
#endif
    if( ch >= '1' && ch <= '9' ) {
        bufnum = ch - '1';
        if( Savebufs[bufnum].type == SAVEBUF_NOP ) {
            return( TRUE );
        }
        return( FALSE );
    } else if( ch >= 'a' && ch <= 'z' ) {
        bufnum = ch - 'a';
        if( SpecialSavebufs[bufnum].type == SAVEBUF_NOP ) {
            return( TRUE );
        }
    }
    return( FALSE );

} /* IsEmptySavebuf */
