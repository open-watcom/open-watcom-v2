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
#include "posix.h"
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include "vi.h"
#include "keys.h"
#include "rxsupp.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#include "filelist.h"
#include "font.h"
#endif

#define MAX_DISP 60
static void fileGrep( char *, char **, int *, window_id );
static int fSearch( char *, char * );
static int eSearch( char *, char * );
static int doGREP( char * );

static regexp  *cRx;
static char *sString;
static char *origString;
static char *cTable;
static bool isFgrep,caseIgn;

/*
 * DoFGREP - do a fast grep
 */
int DoFGREP( char *dirlist, char *string, bool ci )
{
    char        table[256];
    int         i;
    int         rc;

    origString = string;
    AddString( &sString, string );
    isFgrep = TRUE;
    caseIgn = ci;
    for( i=0;i<sizeof(table);i++ ) {
        table[i] = i;
    }
    cTable = table;
    if( ci ) {
        for( i='A';i<='Z';i++) {
            table[i] = i-'A'+'a';
        }
        strlwr( sString );
    }
    rc = doGREP( dirlist );
    MemFree( sString );
    return( rc );

} /* DoFGREP */

/*
 * DoEGREP - do an extended grep
 */
int DoEGREP( char *dirlist, char *string )
{
    int rc;

    cRx = RegComp( string );
    if( RegExpError ) {
        return( RegExpError );
    }
    sString = string;
    origString = string;
    isFgrep = FALSE;
    rc = doGREP( dirlist );
    MemFree( cRx );
    return( rc );

} /* DoEGREP */

static int getFile( char *fname )
{
    char        dir[ MAX_STR ];
    char        *dirptr, ch;
    int         rc;

    NextWord1( fname, dir );
    rc = EditFile( dir, FALSE );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    dirptr = dir;
    if( isFgrep ) {
        if( caseIgn ) {
            ch = '~';
        } else {
            ch = '@';
        }
        dirptr += 2;
        strcpy( dirptr, origString );
        MakeExpressionNonRegular( dirptr );
        dirptr--;
        *dirptr = ch;
        if( !EditFlags.Magic ) {
            if( strchr( Majick, ch ) != NULL ) {
                dirptr--;
                *dirptr = '\\';
            }
        }
    } else {
        strcpy( dir, origString );
    }
    AddString2( &(FindHist.data[ FindHist.curr % FindHist.max ] ), dirptr );
    FindHist.curr += 1;
    ColorFind( dirptr, FINDFL_NOERROR );
    return( rc );
}

static int initList( window_id w, char *dirlist, char **list )
{
    char        dir[ MAX_STR ];
    int         clist;

    #ifdef __WIN__
        InitGrepDialog();
    #endif

    /*
     * go after each directory given on the command line
     */
    clist = 0;
    EditFlags.WatchForBreak = TRUE;
    if( NextWord1( dirlist, dir ) <= 0 ) {
        fileGrep( GrepDefault, list, &clist, w );
    } else {
        do {
            if( IsDirectory( dir ) ) {
                strcat( dir, FILE_SEP_STR );
                strcat( dir, GrepDefault );
            }
            fileGrep( dir, list, &clist, w );
            if( EditFlags.BreakPressed ) {
                break;
            }
        } while( NextWord1( dirlist, dir ) > 0 );
    }
    if( EditFlags.BreakPressed ) {
        #ifdef __WIN__
            EditFlags.BreakPressed = FALSE;
        #else
            ClearBreak();
        #endif
    }
    EditFlags.WatchForBreak = FALSE;
    #ifdef __WIN__
        FiniGrepDialog();
    #endif
    return( clist );
}

static void finiList( int clist, char **list )
{
    MemFreeList( clist, list );
}

#ifdef __WIN__

static void getOneFile( HWND dlg, char **files, int *count, bool leave )
{
    int         i, j;
    HWND        list_box;

    list_box = GetDlgItem( dlg, ID_FILE_LIST );
    i = SendMessage( list_box, LB_GETCURSEL, 0, 0L );
    getFile( files[ i ] );
    if( leave ) {
        EndDialog( dlg, ERR_NO_ERR );
    } else {
        /* remove it from the list box */
        j = SendMessage( list_box, LB_DELETESTRING, i, 0L );
        assert( (j+1) == (*count) );
        if( SendMessage( list_box, LB_SETCURSEL, i, 0L ) == LB_ERR ) {
            SendMessage( list_box, LB_SETCURSEL, i-1, 0L );
        }
        MemFree( files[ i ] );
        for( j = i; j < *count; j++ ) {
            files[ j ] = files[ j + 1 ];
        }
        (*count)--;
        if( *count == 0 ) {
            EndDialog( dlg, ERR_NO_ERR );
        }
    }
}

static void getAllFiles( HWND dlg, char **files, int *count )
{
    int         i;

    for( i = 0; i < *count; i++ ) {
        getFile( files[ i ] );
    }
    EndDialog( dlg, ERR_NO_ERR );

} /* editFiles */

BOOL WINEXP GrepListProc( HWND dlg, UINT msg, UINT wparam, LONG lparam )
{
    static char         **fileList;
    static int          fileCount;
    HWND                list_box;
    char                tmp[MAX_STR];
    WORD                cmd;

    switch( msg ) {
    case WM_INITDIALOG:
        list_box = GetDlgItem( dlg, ID_FILE_LIST );
        SendMessage( list_box, WM_SETFONT, (UINT)FontHandle( dirw_info.text.font ), 0L );
        MySprintf( tmp,"Files Containing \"%s\"", sString );
        SetWindowText( dlg, tmp );
        fileList = (char **)MemAlloc( sizeof( char * ) * MAX_FILES );
        fileCount = initList( list_box, (char *)lparam, fileList );
        if( fileCount == 0 ) {
            /* tell him that there are no matches and close down? */
            Message1( "String \"%s\" not found", sString );
            EndDialog( dlg, DO_NOT_CLEAR_MESSAGE_WINDOW );
        } else {
            SendMessage( list_box, LB_SETCURSEL, 0, 0L );
            BringWindowToTop( dlg );
            SetFocus( dlg );
        }
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case ID_FILE_LIST:
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_DBLCLK ) {
                getOneFile( dlg, fileList, &fileCount, TRUE );
            }
            break;
        case ID_EDIT:
        case ID_GOTO:
            getOneFile( dlg, fileList, &fileCount, cmd == ID_GOTO );
            break;
        case ID_GETALL:
            getAllFiles( dlg, fileList, &fileCount );
            break;
        case IDCANCEL:
            EndDialog( dlg, ERR_NO_ERR );
            return( TRUE );
        }
        break;
    case WM_DESTROY:
        finiList( fileCount, fileList );
        break;
    }
    return( FALSE );

} /* GrepListProc */

static int doGREP( char *dirlist )
{
    DLGPROC     grep_proc;
    int         rc;

    grep_proc = (DLGPROC) MakeProcInstance( (FARPROC) GrepListProc, InstanceHandle );
    rc = DialogBoxParam( InstanceHandle, "GREPLIST", Root, grep_proc, (LONG)(LPVOID)dirlist );
    FreeProcInstance( (FARPROC) grep_proc );
    return( rc );
}
#else
/*
 * doGREP - perform GREP on a specified file
 */
static int doGREP( char *dirlist )
{
    int         i,clist,rc,n=0;
    window_id   wn,optwin;
    char        **list;
    window_info tw,wi;
    int         evlist[4] = { VI_KEY( F1 ), VI_KEY( F2 ), VI_KEY( F3 ), -1 };
    int         s,e,cnt;
    bool        show_lineno;
    selectitem  si;

    /*
     * prepare list array
     */
    list = (char **) MemAlloc( sizeof( char *) * MAX_FILES );

    /*
     * create info. window
     */
    i = NewWindow( &wn, dirw_info.x1, dirw_info.y1+4, dirw_info.x2,
        dirw_info.y1+6, 1, dirw_info.border_color1, dirw_info.border_color2,
        &dirw_info.text );
    if( i ) {
        MemFree( list );
        return( i );
    }
    WindowTitle( wn, "File Being Searched" );


    clist = initList( wn, dirlist, list );
    /*
     * got list of matches, so lets select an item, shall we?
     */
    CloseAWindow( wn );
    rc = ERR_NO_ERR;
    if( clist ) {

        /*
         * define display window dimensions
         */
        memcpy( &tw, &dirw_info, sizeof( window_info ) );
        tw.x1 = 14;
        tw.x2 = WindMaxWidth-2;
        i = tw.y2 - tw.y1+1;
        if( tw.has_border ) {
            i -= 2;
        }
        if( clist < i ) {
            tw.y2 -= ( i-clist );
        }
        if( clist > i ) {
            show_lineno = TRUE;
        } else {
            show_lineno = FALSE;
        }

        /*
         * build options window
         */
        memcpy( &wi, &extraw_info, sizeof( window_info ) );
        wi.x1 =0;
        wi.x2 = 13;
        i = DisplayExtraInfo( &wi, &optwin, EditOpts, NumEditOpts );
        if( i ) {
            return( i );
        }

        /*
         * process selections
         */
        while( TRUE ) {

            if( n+1 > clist ) {
                n = clist-1;
            }
            memset( &si, 0, sizeof( si ) );
            si.wi = &tw;
            si.title = "Files With Matches";
            si.list = list;
            si.maxlist = clist;
            si.num = n;
            si.retevents = evlist;
            si.event = -1;
            si.show_lineno = show_lineno;
            si.cln = n+1;
            si.eiw = optwin;

            rc = SelectItem( &si );
            n = si.num;

            if( rc || n < 0 ) {
                break;
            }
            if( si.event == VI_KEY( F3 ) ) {
                s = 0;
                e = clist-1;
            } else {
                s = e = n;
            }
            for( cnt=s;cnt<=e;cnt++ ) {
                rc = getFile( list[ cnt ] );
                if( rc != ERR_NO_ERR ) break;
            }
            if( rc != ERR_NO_ERR || si.event == -1 ||
                        si.event == VI_KEY( F1 ) || si.event == VI_KEY( F3 ) ) {
                break;
            }
            MemFree( list[n] );
            for( i=n;i<clist-1;i++ ) {
                list[i] = list[i+1];
            }
            clist--;
            if( clist == 0 ) {
                break;
            }
            MoveWindowToFrontDammit( optwin, FALSE );

        }
        CloseAWindow( optwin );

    } else if( !rc ) {
        Message1( "String \"%s\" not found", sString );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }

    /*
     * cleanup
     */
    finiList( clist, list );
    return( rc );

} /* DoFGREP */
#endif

/*
 * fileGrep - search a single dir and build list of files
 */
static void fileGrep( char *dir, char **list, int *clist, window_id wn )
{
    char        fn[_MAX_PATH],data[_MAX_PATH],ts[_MAX_PATH];
    char        path[_MAX_PATH];
    char        drive[_MAX_DRIVE],directory[_MAX_DIR],name[_MAX_FNAME];
    char        ext[_MAX_EXT];
    int         i,j;

    /*
     * get file path prefix
     */
    _splitpath( dir, drive, directory, name, ext );
    strcpy( path, drive );
    strcat( path, directory );
//    _makepath( path, drive, directory, NULL,NULL );

    /*
     * run through each entry and search it; building a list of matches
     */
    i = GetSortDir( dir, FALSE );
    if( i ) {
        return;
    }
    for( i=0;i<DirFileCount;i++ ) {
        if( !(DirFiles[i]->attr & _A_SUBDIR ) ) {

            strcpy( fn,path );
            strcat( fn, DirFiles[i]->name );
            #ifdef __WIN__
                EditFlags.BreakPressed = SetGrepDialogFile( fn );
            #else
                DisplayLineInWindow( wn, 1,fn );
            #endif
            if( EditFlags.BreakPressed ) {
                return;
            }
            if( isFgrep ) {
                j = fSearch( fn, ts );
            } else {
                j = eSearch( fn, ts );
            }
            if( j==FGREP_FOUND_STRING ) {

                ExpandTabsInABuffer(ts,strlen(ts),data,MAX_DISP );
                strcpy( ts,data );
                MySprintf(data,"%X \"%s\"",fn,ts );
                #ifdef __WIN__
                    /*
                     * for windows - the handle passed in is the list box
                     * and the entire string is added to it but only the file
                     * name is added to the list
                     */
                    SendMessage( wn, LB_ADDSTRING, 0, (LONG)(LPVOID)data );
                    MySprintf( data, "%X", fn );
                #endif
                AddString( &(list[*clist]), data );
                (*clist)++;

            } else if( j ) {
                return;
            }
        }
    }

} /* fileGrep */

/*
 * eSearch - scan a file for a search string (extended)
 */
static int eSearch( char *fn, char *res )
{
    int         i;
    char        *buff;
    FILE        *f;

    /*
     * init for file i/o
     */
    f = fopen( fn,"r" );
    if( f == NULL ) {
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * read lines from the file, and search through them
     */
    buff = StaticAlloc();
    while( TRUE ) {

        if( fgets( buff,MaxLine-1,f ) == NULL ) {
            fclose( f );
            StaticFree( buff );
            return( ERR_NO_ERR );
        }
        buff[ strlen(buff)-1 ] = 0;
        i = RegExec( cRx, buff, TRUE );
        if( RegExpError != ERR_NO_ERR ) {
            StaticFree( buff );
            return( RegExpError );
        }
        if( i ) {
            for( i=0;i<MAX_DISP;i++ ) {
                res[i] = buff[i];
            }
            res[i] = 0;
            fclose( f );
            StaticFree( buff );
            return( FGREP_FOUND_STRING );
        }

    }

} /* eSearch */

/*
 * fSearch - scan a file for a search string (fast)
 */
static int fSearch( char *fn, char *r )
{
    int         i,handle,j;
    int         bytes,bcnt;
    char        *buffloc,*strloc,*buff,*res;
    int         bytecnt;
    char        context_display[MAX_DISP];

    /*
     * init for file i/o
     */
    i = FileOpen( fn, FALSE, O_BINARY | O_RDONLY, 0, &handle );
    if( i ) {
        return( i );
    }
    #ifndef __NT__
        bytecnt = 3*stackavail()/4;
    #else
        bytecnt = 2048;
    #endif
    buff = alloca( bytecnt+2 );
    if( buff == NULL ) {
        return( ERR_NO_STACK );
    }

    /*
     * read in buffers from the file, and search through them
     */
    strloc = sString; // don't reset at start of new block - could span blocks
    while( 1 ) {

        bcnt = bytes = read( handle, buff, bytecnt );
        buffloc = buff;
        while( bytes ) {

            if( *strloc == cTable[*buffloc] ) {
                buffloc++;
                bytes--;
                strloc++;
                if( ! (*strloc) ) {
                    close( handle );
                    j = 0;
                    if( buffloc - strlen( sString ) < buff ) {
                        // match spans blocks - see context_display
                        res = context_display+MAX_DISP-1;
                        while( 1 ) {
                            if( *res == LF || res == context_display ) {
                                if( *res == LF ) {
                                    res++;
                                }
                                break;
                            }
                            res--;
                        }
                        // copy the part of the string NOT in buff
                        while( 1 ) {
                           if( j == MAX_DISP || *res == CR || *res == LF || res == &context_display[MAX_DISP] ) {
                                r[j] = 0;
                                break;
                            }
                            r[j++] = *res;
                            res++;
                        }
                        res = buff;
                    } else {
                        res = &buffloc[ -strlen( sString ) ];
                        while( 1 ) {
                            if( *res == LF || res == buff ) {
                                if( *res == LF ) {
                                    res++;
                                }
                                break;
                            }
                            res--;
                        }
                    }
                    // now copy the string ( all that is in buff )
                    while( 1 ) {
                       if( j == MAX_DISP || *res == CR || *res == LF || res == &buff[bytecnt] ) {
                            r[j] = 0;
                            break;
                        }
                        r[j++] = *res;
                        res++;
                    }
                    return( FGREP_FOUND_STRING );
                }
            } else {
                if( strloc == sString ) {
                    buffloc++;
                    bytes--;
                } else {
                    strloc = sString;
                }
            }

        }
        if( bcnt != bytecnt ) {
            break;
        }
        if( strloc != sString ) {
            // partial match -- keep the last bunch of text as context
            strncpy( context_display, buffloc-MAX_DISP, MAX_DISP );
        }

    }
    close( handle );
    return( ERR_NO_ERR );

} /* fSearch */
