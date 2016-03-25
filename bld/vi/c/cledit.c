/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "posix.h"
#include "win.h"
#ifdef __WIN__
    #include "utils.h"
#endif

#include "clibext.h"


#if defined( __WIN__ ) && defined( __NT__ )

/* on NT, we have \0 instead of spaces to delimit single file names and \0\0 to end the string */
#define GetNextFileName GetNextWordNT

#else

#define GetNextFileName GetNextWord1

#endif

#ifdef __WIN__
#ifndef __NT__
static bool isMultipleFiles( const char *altname )
{
    while( *altname != '\0' && *altname != ' ' ) {
        altname++;
    }
    return( *altname == ' ' );
}
#else
static bool isMultipleFiles( const char *altname )
{
    while( altname[0] != '\0' ) {
        altname++;
    }
    return( altname[0] == '\0' && altname[1] != '\0' );
}

/*
 * GetNextWordNT - get next \0 delimited word in buff, final delimitier is \0\0
 */
static char *GetNextWordNT( const char *buff, char *res )
{
    char        c;

    for( ; (c = buff[0]) != '\0' || buff[1] != '\0'; ) {
        ++buff;
        if( c == '\0' ) {
            break;
        }
        *res++ = c;
    }
    *res = '\0';
    return( (char *)buff );

} /* GetNextWordNT */
#endif
#endif

/*
 * EditFile - read a file into text
 */
vi_rc EditFile( const char *name, bool dammit )
{
    char        *fn, **list, *currfn;
    int         i, cnt, ocnt;
    int         j, len;
    window_id   wid = NO_WINDOW;
    char        cdir[FILENAME_MAX];
    info        *ci, *il;
    bool        usedir = false;
    char        mask[FILENAME_MAX];
    bool        reset_dir;
    int         index;
    char        *altname = NULL;
    vi_rc       rc;

    fn = MemAlloc( FILENAME_MAX );

    /*
     * get file name
     */
    strcpy( cdir, CurrentDirectory );
    reset_dir = false;
    name = SkipLeadingSpaces( name );
    if( name[0] == '$' ) {
        ++name;
        usedir = true;
    }
    fn[0] = '\0';
//    if( NextWord1FN( name, fn ) <= 0 )
    if( GetStringWithPossibleQuote2( &name, fn, false ) != ERR_NO_ERR ) {
        usedir = true;
        mask[0] = '*';
        mask[1] = '\0';
    }
    if( usedir ) {
        if( EditFlags.ExMode ) {
            MemFree( fn );
            return( ERR_INVALID_IN_EX_MODE );
        }
        len = strlen( fn );
        if( len > 0 ) {
            strcpy( mask, fn );
            cnt = 0;
            for( i = len - 1; i >= 0; --i ) {
                if( fn[i] == FILE_SEP ) {
                    for( j = i + 1; j <= len; j++ ) {
                        mask[j - (i + 1)] = fn[j];
                    }
                    cnt = i;
                    break;
                }
            }
            fn[cnt] = '\0';
        }
        if( fn[0] != '\0' ) {
            rc = SelectFileOpen( fn, &fn, mask, true );
        } else {
#ifdef __WIN__
            if( name[0] == '\0' ) {
                altname = MemAlloc( 1000 );
                rc = SelectFileOpen( CurrentDirectory, &altname, mask, true );
                name = GetNextFileName( altname, fn );  // if multiple, kill path
                if( isMultipleFiles( name ) ) {
                    name = GetNextFileName( name, fn ); // get 1st name
                }
            } else {
                rc = SelectFileOpen( CurrentDirectory, &fn, mask, true );
            }
#else
            rc = SelectFileOpen( CurrentDirectory, &fn, mask, true );
#endif
        }

        if( rc != ERR_NO_ERR || fn[0] == '\0' ) {
            MemFree( fn );
            SetCWD( cdir );
            return( rc );
        }
    }

    /*
     * loop through all files
     */
    rc = ERR_NO_ERR;
    EditFlags.WatchForBreak = true;
#ifdef __WIN__
    ToggleHourglass( true );
#endif
    do {
        if( IsDirectory( fn ) ) {
            if( EditFlags.ExMode ) {
                rc = ERR_INVALID_IN_EX_MODE;
                reset_dir = true;
                break;
            }
            rc = SelectFileOpen( fn, &fn, "*", false );
            if( rc != ERR_NO_ERR ) {
                reset_dir = true;
                break;
            }
            if( fn[0] == '\0' ) {
                reset_dir = true;
                rc = ERR_NO_ERR;
                break;
            }
        }
        currfn = fn;
        ocnt = cnt = ExpandFileNames( currfn, &list );
        if( !cnt ) {
            cnt = 1;
        } else {
            currfn = list[0];
        }

        /*
         * loop through all expanded files
         */
        index = 1;
        while( cnt > 0 ) {
            cnt--;
            /*
             * quit current file if ! specified, else just save current state
             */
            if( dammit ) {
                ci = InfoHead;
                if( CurrentInfo == ci ) {
                    ci = ci->next;
                }
                RemoveFromAutoSaveList();
#ifdef __WIN__
                CloseAChildWindow( current_window_id );
#else
                CloseAWindow( current_window_id );
#endif
                FreeUndoStacks();
                FreeMarkList();
                FreeEntireFile( CurrentFile );
                MemFree( DeleteLLItem( (ss **)&InfoHead, (ss **)&InfoTail,
                         (ss *)CurrentInfo ) );
                CurrentInfo = NULL;
                current_window_id = NO_WINDOW;
            } else {
                ci = CurrentInfo;
                SaveCurrentInfo();
                wid = current_window_id;
            }

            /*
             * see if new file is already being edited
             */
            SaveCurrentInfo();
            for( il = InfoHead; il != NULL; il = il->next ) {
                if( SameFile( il->CurrentFile->name, currfn ) ) {
                    break;
                }
                if( strcmp( CurrentDirectory, il->CurrentFile->home ) ) {
                    /* directory has changed -- check with full path
                     * note that this will fail if an absolute path
                     * was specified thus we do the regular check first */
                    char path[FILENAME_MAX];
                    char drive[_MAX_DRIVE];
                    char dir[_MAX_DIR];
                    char fname[_MAX_FNAME];
                    char ext[_MAX_EXT];

                    _splitpath( il->CurrentFile->name, drive, dir, fname, ext );
                    if( drive[0] == '\0' ) {
                        _splitpath( il->CurrentFile->home, drive, NULL, NULL, NULL );
                    }
                    strcpy( path, il->CurrentFile->home );
                    if( path[0] != '\0' && path[strlen( path ) - 1] != ':' )
                        strcat( path, FILE_SEP_STR );
                    if( dir[0] == '\0' ) {
                        _splitpath( path, NULL, dir, NULL, NULL );
                    } else if( dir[0] != FILE_SEP ) {
                        char dir2[_MAX_DIR];
                        _splitpath( path, NULL, dir2, NULL, NULL );
                        strcat( dir2, dir );
                        strcpy( dir, dir2 );
                    }
                    _makepath( path, drive, dir, fname, ext );

                    if( SameFile( path, currfn ) ) {
                        break;
                    }
                }
            }

            if( il != NULL ) {
                BringUpFile( il, true );
            } else {
                /*
                 * file not edited, go get it
                */
                rc = NewFile( currfn, false );
                if( rc != ERR_NO_ERR && rc != NEW_FILE ) {
                    RestoreInfo( ci );
                    DCDisplayAllLines();
                    break;
                }
                if( !dammit ) {
                    InactiveWindow( wid );
                }
                if( EditFlags.BreakPressed ) {
                    break;
                }
            }
            if( cnt > 0 ) {
                currfn = list[index];
                index++;
            }
        }

        if( ocnt > 0 ) {
            MemFreeList( ocnt, list );
        }
        if( EditFlags.BreakPressed ) {
            ClearBreak();
            break;
        }
        name = GetNextFileName( name, fn );
    } while( *fn != '\0' );

    if( altname != NULL ) {
        MemFree( altname );
    }
    MemFree( fn );

#ifdef __WIN__
    ToggleHourglass( false );
#endif
    EditFlags.WatchForBreak = false;
    if( reset_dir ) {
        SetCWD( cdir );
    }
    return( rc );

} /* EditFile */

#ifndef __WIN__
static char _NEAR *_NEAR fileOpts[] =  {
    "<F1> Go To",
    "<F2> Quit",
    "<F3> Save & Quit"
};

#define NUM_OPTS sizeof( fileOpts ) / sizeof( char _NEAR * )

/*
 * EditFileFromList - edit from file in current active list
 */
vi_rc EditFileFromList( void )
{
    int         i, tmp, j, n = 0, fcnt;
    window_id   wid;
    bool        repeat = true;
    info        *cinfo;
    char        **list, modchar;
    vi_key      evlist[4] = { VI_KEY( F1 ), VI_KEY( F2 ), VI_KEY( F3 ), VI_KEY( DUMMY ) };
    bool        show_lineno;
    window_info wi;
    selectitem  si;
    vi_rc       rc;

    /*
     * set up options for file list
     */
    memcpy( &wi, &extraw_info, sizeof( window_info ) );
    wi.area.x1 = 2;
    wi.area.x2 = 19;
    rc = DisplayExtraInfo( &wi, &wid, fileOpts, NUM_OPTS );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    while( repeat > 0 ) {

        /*
         * set up for this pass
         */
        repeat = false;
        MoveWindowToFrontDammit( wid, false );
        SaveCurrentInfo();

        /*
         * allocate a buffer for strings, add strings
         */
        list = (char **) MemAlloc( GimmeFileCount() * sizeof( char * ) );
        for( j = 0, cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next, ++j ) {
            list[j] = MemAlloc( strlen( cinfo->CurrentFile->name ) + 3 );
            if( cinfo->CurrentFile->modified ) {
                modchar = '*';
            } else {
                modchar = ' ';
            }
            MySprintf( list[j], "%c %s", modchar, cinfo->CurrentFile->name );
        }
        fcnt = j;
        tmp = filelistw_info.area.y2;
        i = filelistw_info.area.y2 - filelistw_info.area.y1 + 1;
        if( filelistw_info.has_border ) {
            i -= 2;
        }
        if( j < i ) {
            filelistw_info.area.y2 -= ( i - j );
        }
        show_lineno = true;

        /*
         * get file
         */
        if( n + 1 > j ) {
            n = j - 1;
        }
        memset( &si, 0, sizeof( si ) );
        si.wi = &filelistw_info;
        si.title = "Current Files";
        si.list = list;
        si.maxlist = j;
        si.num = n;
        si.retevents = evlist;
        si.event = VI_KEY( DUMMY );
        si.show_lineno = show_lineno;
        si.cln = n + 1;
        si.eiw = wid;
        rc = SelectItem( &si );
        n = si.num;
        if( rc == ERR_NO_ERR ) {
            if( n >= 0 ) {
                cinfo = InfoHead;
                for( j = 0; j < n; ++j ) {
                    cinfo = cinfo->next;
                }
                BringUpFile( cinfo, true );
                switch( si.event ) {
                case VI_KEY( DUMMY ):
                case VI_KEY( F1 ):
                    break;
                case VI_KEY( F2 ):
                    rc = NextFile();
                    if( rc <= ERR_NO_ERR ) {
                        repeat = true;
                    }
                    break;
                case VI_KEY( F3 ):
                    rc = SaveAndExit( NULL );
                    if( rc <= ERR_NO_ERR ) {
                        repeat = true;
                    }
                    break;
                }
            }
        }

        filelistw_info.area.y2 = tmp;
        MemFreeList( fcnt, list );

    }

    /*
     * get rid of option stuff
     */
    CloseAWindow( wid );
    return( rc );

} /* EditFileFromList */
#endif

/*
 * OpenWindowOnFile - open a window on a file
 */
vi_rc OpenWindowOnFile( const char *data )
{
    vi_rc       rc;
    window_id   wid;

    data = SkipLeadingSpaces( data );
    if( data[0] == '\0' ) {
        data = NULL;
    }
    wid = current_window_id;
    rc = NewFile( data, true );
    if( rc == ERR_NO_ERR ) {
        InactiveWindow( wid );
        DCDisplayAllLines();
    }
    return( rc );

} /* OpenWindowOnFile */
