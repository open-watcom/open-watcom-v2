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
#include <errno.h>
#include "posix.h"
#include "source.h"
#include "win.h"

#ifdef __WIN__
extern long VScrollBarScale;
extern int  HScrollBarScale;
#endif

/*
 * SaveInfo - save file info
 */
void SaveInfo( info *ci  )
{
    if( ci == NULL ) {
        return;
    }
    ci->CurrentFile = CurrentFile;
    ci->CurrentPos = CurrentPos;
    ci->VirtualColumnDesired = VirtualColumnDesired;
    ci->LeftTopPos = LeftTopPos;
    ci->CurrentWindow = CurrentWindow;
    ci->UndoStack = UndoStack;
    ci->UndoUndoStack = UndoUndoStack;
    ci->CurrNumWindow = CurrNumWindow;
    ci->linenumflag = EditFlags.LineNumbers;
    ci->MarkList = MarkList;
    ci->CMode = EditFlags.CMode;
    ci->WriteCRLF = EditFlags.WriteCRLF;
    ci->SelRgn = SelRgn;
    ci->RealTabs = EditFlags.RealTabs;
    ci->ReadEntireFile = EditFlags.ReadEntireFile;
    ci->ReadOnlyCheck = EditFlags.ReadOnlyCheck;
    ci->EightBits = EditFlags.EightBits;
    ci->AutoIndent = EditFlags.AutoIndent;
    ci->TabAmount = TabAmount;
    ci->HardTab = HardTab;
    ci->ShiftWidth = ShiftWidth;
    CTurnOffFileDisplayBits();
#ifdef __WIN__
    ci->VScrollBarScale = VScrollBarScale;
    ci->HScrollBarScale = HScrollBarScale;
#endif
} /* SaveInfo */

/*
 * SaveCurrentInfo - save info of current file
 */
void SaveCurrentInfo( void )
{
    SaveInfo( CurrentInfo );

} /* SaveCurrentInfo */

/*
 * cRestoreFileDisplayBits - do just that
 */
static void cRestoreFileDisplayBits( void )
{
    fcb *tfcb;

    if( CurrentFile != NULL ) {
        for( tfcb = CurrentFile->fcbs.head; tfcb != NULL; tfcb = tfcb->next ) {
            tfcb->on_display = tfcb->was_on_display;
            if( tfcb->on_display && !tfcb->in_memory ) {
                FetchFcb( tfcb );
            }
        }
    }

} /* cRestoreFileDisplayBits */

/*
 * RestoreInfo - restore file info
 */
bool RestoreInfo( info *ci  )
{
    info        tmpinfo;
    CurrentInfo = ci;
    if( ci == NULL ) {
        ci = &tmpinfo;
        memset( ci, 0, sizeof( tmpinfo ) );
        ci->CurrentWindow = NO_WINDOW;
        ci->CurrNumWindow = NO_WINDOW;
        ci->CurrentPos.line = 1;
        ci->CurrentPos.column = 1;
        ci->VirtualColumnDesired = 1;
        ci->LeftTopPos.line = 1;
        ci->LeftTopPos.column = 0;
        CurrentLine = NULL;
        CurrentFcb = NULL;

        ci->HardTab = HardTab;
        ci->ShiftWidth = ShiftWidth;
        ci->TabAmount = TabAmount;
        ci->ReadEntireFile = EditFlags.ReadEntireFile;
        ci->ReadOnlyCheck = EditFlags.ReadOnlyCheck;
        ci->AutoIndent = EditFlags.AutoIndent;
        ci->EightBits = EditFlags.EightBits;

        ci->RealTabs = EditFlags.RealTabs;
    }
    CurrentFile = ci->CurrentFile;
    SelRgn = ci->SelRgn;
#ifdef __WIN__
    VScrollBarScale = ci->VScrollBarScale;
    HScrollBarScale = ci->HScrollBarScale;
#endif
    CurrentPos = ci->CurrentPos;
    VirtualColumnDesired = ci->VirtualColumnDesired;
    LeftTopPos = ci->LeftTopPos;
    CurrentWindow = ci->CurrentWindow;
    CurrNumWindow = ci->CurrNumWindow;
    UndoStack = ci->UndoStack;
    UndoUndoStack = ci->UndoUndoStack;
    MarkList = ci->MarkList;
    SetMarkContext();
    EditFlags.CMode = ci->CMode;
    EditFlags.WriteCRLF = ci->WriteCRLF;

    EditFlags.RealTabs = ci->RealTabs;
    EditFlags.ReadEntireFile = ci->ReadEntireFile;
    EditFlags.ReadOnlyCheck = ci->ReadOnlyCheck;
    EditFlags.AutoIndent = ci->AutoIndent;
    EditFlags.EightBits = ci->EightBits;
    TabAmount = ci->TabAmount;
    HardTab = ci->HardTab;
    ShiftWidth = ci->ShiftWidth;

    cRestoreFileDisplayBits();

    CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
    ValidateCurrentColumn();
    ResetLastFind( ci );

    VarAddRandC();
    if( CurrentFile == NULL ) {
        SetModifiedVar( FALSE );
    } else{
        SetModifiedVar( CurrentFile->modified );
        if( (unsigned) ci->linenumflag != EditFlags.LineNumbers ) {
            return( TRUE );
        }
    }

    return( FALSE );

} /* RestoreInfo */

static int getFileInfoString( char *st, int is_small )
{
    long        pc;
    st[0] = 0;
    if( !is_small ) {
        if( EditFlags.NewFile ) {
            strcat( st, " [new file]" );
            EditFlags.NewFile = FALSE;
        }
        if( EditFlags.DuplicateFile ) {
            MyBeep();
            strcat( st, " [duplicate file]" );
            EditFlags.DuplicateFile = FALSE;
        }
        if( CurrentFile->viewonly ) {
            strcat( st, " [view only]" );
        }
        if( CFileReadOnly() ) {
            strcat( st, " [read only]" );
        }
        if( CurrentFile->modified ) {
            strcat( st, " [modified]" );
        }
#if defined( __UNIX__ )
        if( EditFlags.WriteCRLF ) {
            strcat( st, " [crlf]" );
        }
#elif 0
        if( !EditFlags.WriteCRLF ) {
            strcat( st, " [lf]" );
        }
#endif
        pc = (CurrentPos.line * 100L) / CurrentFile->fcbs.tail->end_line;
        MySprintf( st + strlen( st ), " line %l of %l  -- %l%%%% --",
            CurrentPos.line, CurrentFile->fcbs.tail->end_line, pc );
        if( EditFlags.ColumnInFileStatus ) {
            MySprintf( st + strlen( st  ), " (col %d)", VirtualColumnOnCurrentLine( CurrentPos.column ) );
        }
    } else {
        if( EditFlags.NewFile ) {
            strcat( st, "[N]" );
            EditFlags.NewFile = FALSE;
        }
        if( EditFlags.DuplicateFile ) {
            MyBeep();
            strcat( st, "[D]" );
            EditFlags.DuplicateFile = FALSE;
        }
        if( CurrentFile->viewonly ) {
            strcat( st, "[V]" );
        }
        if( CFileReadOnly() ) {
            strcat( st, "[R]" );
        }
        if( CurrentFile->modified ) {
            strcat( st, "[M]" );
        }
#if defined(__UNIX__)
        if( EditFlags.WriteCRLF ) {
            strcat( st, " [C]" );
        }
#elif 0
        if( !EditFlags.WriteCRLF ) {
            strcat( st, " [L]" );
        }
#endif
        MySprintf( st + strlen( st ), " line %l of %l",
            CurrentPos.line, CurrentFile->fcbs.tail->end_line );
        if( EditFlags.ColumnInFileStatus ) {
            MySprintf( st + strlen( st ), " (col %d)", VirtualColumnOnCurrentLine( CurrentPos.column ) );
        }
    }
    return( strlen( st ) );
}

static void make_short_name( char *name, int len, char *buffer )
{
    char    *start;
    char    *end;
    int     newlen;

    len -= 2; /* for 2 quotes */
    strcpy( buffer, "\"" );
    start = strchr( name, '\\' );
    if( start ) {
        for( end = name + strlen( name ) - 1; *end != '\\'; end-- );
        newlen = strlen( end ) + ( start - name );
        if( newlen <= len ) {
            strncat( buffer, name, start - name + 1 );
            strcat( buffer, "..." );
            strcat( buffer, end );
            strcat( buffer, "\"" );
            return;
        }
    }
    strcat( buffer, "..." );
    strncat( buffer, name + strlen(name) - len + 3, len - 3 );
    strcat( buffer, "\"" );
}

/*
 * DisplayFileStatus - print file status
 */
vi_rc DisplayFileStatus( void )
{
    char        st[MAX_STR], data[MAX_STR];
    int         free_len;
    long        pc;

    if( CurrentFile == NULL ) {
        Message1( "No file currently loaded" );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    free_len = messagew_info.x2 - messagew_info.x1;
    if( free_len > MAX_STR ) {
        free_len = MAX_STR;
    }

    free_len -= (getFileInfoString( st, FALSE ) + 3); /* for 2 quotes + NULL */

    /* file name */
    if( strlen( CurrentFile->name ) < free_len ) {
        MySprintf( data, "\"%s\"", CurrentFile->name );
        strcat( data, st );
    } else {
        // go to short version
        free_len = messagew_info.x2 - messagew_info.x1;
        if( free_len > MAX_STR ) {
            free_len = MAX_STR;
        }
        free_len -= (getFileInfoString( st, TRUE ) + 3); /* for 2 quotes + NULL */
        if( strlen( CurrentFile->name ) < free_len ) {
            MySprintf( data, "\"%s\"", CurrentFile->name );
            strcat( data, st );
        } else {
            make_short_name( CurrentFile->name, free_len, data );
            strcat( data, st );
        }
    }
    Message1( data );

    if( CurrentFile->bytes_pending ) {
        pc = (CurrentFile->curr_pos * 100L) / CurrentFile->size;
        Message2( " partially read: %l bytes of %l -- %d%% --",
            CurrentFile->curr_pos, CurrentFile->size, pc );
    }

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* DisplayFileStatus */

/*
 * CTurnOffFileDisplayBits - do just that
 */
void CTurnOffFileDisplayBits( void )
{
    fcb *tfcb;

    if( CurrentFile != NULL ) {
        for( tfcb = CurrentFile->fcbs.head; tfcb != NULL; tfcb = tfcb->next ) {
            tfcb->was_on_display = tfcb->on_display;
            tfcb->on_display = FALSE;
        }
    }

} /* CTurnOffFileDisplayBits */

/*
 * CFileReadOnly - test if a file is read only or not
 */
bool CFileReadOnly( void )
{
    int attr;

    if( CurrentFile->check_readonly ) {
        CurrentFile->check_readonly = FALSE;
        if( CurrentFile->is_stdio ) {
            CurrentFile->read_only = FALSE;
        } else {
            ConditionalChangeDirectory( CurrentFile->home );
            attr = access( CurrentFile->name, ACCESS_WR );
            if( stricmp( CurrentFile->home, CurrentDirectory ) ) {
                ChangeDirectory( CurrentDirectory );
            }

            if( attr == -1 && errno != ENOENT ) {
                CurrentFile->read_only = TRUE;
            } else {
                CurrentFile->read_only = FALSE;
            }
        }
    }
    return( CurrentFile->read_only );

} /* CFileReadOnly */

/*
 * FileIOMessage - print message about file
 */
void FileIOMessage( char *name, linenum lnecnt, long bytecnt )
{
    if( !EditFlags.Quiet ){
        Message1( "\"%s\" %l lines, %l bytes", name, lnecnt, bytecnt );
    }

} /* FileIOMessage */

/*
 * IsTextFile - check if a file has .obj, .com,.bat, or .exe at the end
 */
bool IsTextFile( char *file )
{
    int         i, j;
    char        *fign, *fend;

    i = strlen( file );
    fend = file + i;
    fign = FIgnore;
    for( j = 0; j < CurrFIgnore; j++ ) {
         if( !strcmp( fend - strlen( fign ), fign ) ) {
             return( FALSE );
         }
         fign += EXTENSION_LENGTH;
    }
    return( TRUE );

} /* IsTextFile */

/*
 * GimmeFileCount - as it sounds
 */
int GimmeFileCount( void )
{
    info        *cinfo;
    int         cnt = 0;

    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        cnt++;
    }
    return( cnt );

} /* GimmeFileCount */
