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
#include "vi.h"
#include "keys.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#include "utils.h"
#endif

#ifdef __WIN__
BOOL isMultipleFiles( char *altname )
{
    while( *altname && *altname != ' ' ) {
        altname++;
    }
    if( *altname == ' ' ) {
        return( TRUE );
    }
    return( FALSE );
}
#endif

/*
 * EditFile - read a file into text
 */
int EditFile( char *name, int dammit )
{
    char        *fn,**list,*currfn;
    int         rc=ERR_NO_ERR,i,cnt,ocnt;
    int         j,len;
    window_id   wn;
    char        cdir[_MAX_PATH];
    info        *ci,*il;
    bool        usedir = FALSE;
    char        mask[_MAX_PATH];
    bool        reset_dir;
    int         index;
    char        *altname = NULL;

    fn = MemAlloc( _MAX_PATH );

    /*
     * get file name
     */
    strcpy( cdir, CurrentDirectory );
    reset_dir = FALSE;
    RemoveLeadingSpaces( name );
    if( name[0] == '$' ) {
        EliminateFirstN( name, 1 );
        usedir = TRUE;
    }
    fn[0] = 0;
//    if( NextWord1( name, fn ) <= 0 )
    if( GetStringWithPossibleQuote2( name, fn, FALSE ) != ERR_NO_ERR ) {
        usedir = TRUE;
        mask[0] = '*';
        mask[1] = 0;
    }
    if( usedir ) {
        if( EditFlags.ExMode ) {
            MemFree( fn );
            return( ERR_INVALID_IN_EX_MODE );
        }
        len = strlen( fn );
        if( len > 0 ) {
            i = len-1;
            strcpy( mask, fn );
            cnt = 0;
            while( i >= 0 ) {
                if( fn[i] == FILE_SEP ) {
                    for( j=i+1;j<=len;j++ ) {
                        mask[j-(i+1)] = fn[j];
                    }
                    cnt = i;
                    break;
                }
                i--;
            }
            fn[ cnt ] = 0;
        }
        if( fn[0] != 0 ) {
            i = SelectFileOpen( fn, &fn, mask, TRUE );
        } else {
            #ifdef __WIN__
                if( name[ 0 ] == '\0' ) {
                    altname = MemAlloc( 1000 );
                    i = SelectFileOpen( CurrentDirectory, &altname, mask, TRUE );
                    NextWord1( altname, fn );  // if multiple, kill path
                    if( isMultipleFiles( altname ) ) {
                        NextWord1( altname, fn ); // get 1st name
                    }
                } else {
                    i = SelectFileOpen( CurrentDirectory, &fn, mask, TRUE );
                }
            #else
                i = SelectFileOpen( CurrentDirectory, &fn, mask, TRUE );
            #endif
        }
        if( altname ) {
            name = altname;
        }

        if( i || fn[0] == 0 ) {
            MemFree( fn );
            SetCWD( cdir );
            return( i );
        }
    }

    /*
     * loop through all files
     */
    EditFlags.WatchForBreak = TRUE;
    #ifdef __WIN__
        ToggleHourglass( TRUE );
    #endif
    do {
        if( IsDirectory( fn ) ) {
            if( EditFlags.ExMode ) {
                rc = ERR_INVALID_IN_EX_MODE;
                reset_dir = TRUE;
                break;
            }
            rc = SelectFileOpen( fn, &fn, "*", FALSE );
            if( rc ) {
                reset_dir = TRUE;
                break;
            }
            if( fn[0] == 0 ) {
                reset_dir = TRUE;
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
                    ci=ci->next;
                }
                RemoveFromAutoSaveList();
                CloseAWindow( CurrentWindow );
                FreeUndoStacks();
                FreeMarkList();
                FreeEntireFile( CurrentFile );
                MemFree( DeleteLLItem( &InfoHead, &InfoTail, CurrentInfo ) );
                CurrentInfo = NULL;
                CurrentWindow = NO_WINDOW;
            } else {
                ci = CurrentInfo;
                SaveCurrentInfo();
                wn = CurrentWindow;
            }

            /*
             * see if new file is already being edited
             */
            SaveCurrentInfo();
            il = InfoHead;
            while( il != NULL ) {
                if( SameFile( il->CurrentFile->name, currfn ) ) {
                    BringUpFile( il, TRUE );
                    goto EVIL_CONTINUE;
                }
                if( strcmp( CurrentDirectory, il->CurrentFile->home ) ) {
                    /* directory has changed -- check with full path
                     * note that this will fail if an absolute path
                     * was specified thus we do the regular check first */
                    char path[_MAX_PATH];
                    char drive[_MAX_DRIVE];
                    char dir[_MAX_DIR];
                    char fname[_MAX_FNAME];
                    char ext[_MAX_EXT];

                    _splitpath( il->CurrentFile->name, drive, dir, fname, ext );
                    if( !strlen( drive ) ) {
                        _splitpath( il->CurrentFile->home,
                                    drive, NULL, NULL, NULL );
                    }
                    if( !strlen( dir ) ) {
                        _splitpath( il->CurrentFile->home,
                                    NULL, dir, NULL, NULL );
                    } else if( dir[0] != '\\' ) {
                        char dir2[_MAX_DIR];
                        _splitpath( il->CurrentFile->home,
                                    NULL, dir2, NULL, NULL );
                        strcat( dir2, dir );
                        strcpy( dir, dir2 );
                    }
                    _makepath( path, drive, dir, fname, ext );

                    if( SameFile( path, currfn ) ) {
                        BringUpFile( il, TRUE );
                        goto EVIL_CONTINUE;
                    }
                }

                il = il->next;
            }

            /*
             * file not edited, go get it
             */
            rc = NewFile( currfn, FALSE );
            if( rc && rc != NEW_FILE ) {
                RestoreInfo( ci );
                DCDisplayAllLines();
                break;
            }

            if( !dammit ) {
                InactiveWindow( wn );
            }

            if( EditFlags.BreakPressed ) {
                break;
            }

EVIL_CONTINUE:
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

    } while( NextWord1( name, fn ) > 0 );

    if( altname ) {
        MemFree( altname );
    }
    MemFree( fn );

    #ifdef __WIN__
        ToggleHourglass( FALSE );
    #endif
    EditFlags.WatchForBreak = FALSE;
    if( reset_dir ) {
        SetCWD( cdir );
    }
    return( rc );

} /* EditFile */

#ifndef __WIN__
static char near *near fileOpts[] =  {
"<F1> Go To",
"<F2> Quit",
"<F3> Save & Quit"
};

#define NUM_OPTS sizeof( fileOpts )/sizeof( char near * )

/*
 * EditFileFromList - edit from file in current active list
 */
int EditFileFromList( void )
{
    int         i,tmp,j,n=0,rc,fcnt;
    window_id   optwin;
    bool        repeat=TRUE;
    info        *cinfo;
    char        **list,modchar;
    int         evlist[4] = { VI_KEY( F1 ), VI_KEY( F2 ), VI_KEY( F3 ), -1 };
    bool        show_lineno;
    window_info wi;
    selectitem  si;

    /*
     * set up options for file list
     */
    memcpy( &wi, &extraw_info, sizeof( window_info ) );
    wi.x1 =2;
    wi.x2 = 19;
    i = DisplayExtraInfo( &wi, &optwin, fileOpts, NUM_OPTS );
    if( i ) {
        return( i );
    }

    while( repeat ) {

        /*
         * set up for this pass
         */
        repeat = FALSE;
        MoveWindowToFrontDammit( optwin, FALSE );
        SaveCurrentInfo();

        /*
         * allocate a buffer for strings, add strings
         */
        list = (char **) MemAlloc( GimmeFileCount()*sizeof( char * ) );
        j = 0;
        cinfo = InfoHead;
        while( cinfo != NULL ) {
            list[j] = MemAlloc( strlen( cinfo->CurrentFile->name ) +3 );
            if( cinfo->CurrentFile->modified ) {
                modchar = '*';
            }
            else modchar = ' ';
            MySprintf(list[j],"%c %s",modchar, cinfo->CurrentFile->name );
            j++;
            cinfo = cinfo->next;
        }
        fcnt = j;
        tmp = filelistw_info.y2;
        i = filelistw_info.y2 - filelistw_info.y1+1;
        if( filelistw_info.has_border ) {
            i -= 2;
        }
        if( j < i ) {
            filelistw_info.y2 -= ( i-j );
        }
        show_lineno = TRUE;

        /*
         * get file
         */
        if( n+1 > j ) {
            n = j-1;
        }
        memset( &si, 0, sizeof( si ) );
        si.wi = &filelistw_info;
        si.title = "Current Files";
        si.list = list;
        si.maxlist = j;
        si.num = n;
        si.retevents = evlist;
        si.event = -1;
        si.show_lineno = show_lineno;
        si.cln = n+1;
        si.eiw = optwin;
        rc = SelectItem( &si );
        n = si.num;
        if( !rc ) {
            if( n >= 0 ) {
                j = 0;
                cinfo = InfoHead;
                while( n != j ) {
                    j++;
                    cinfo = cinfo->next;
                }
                BringUpFile( cinfo, TRUE );
                switch( si.event ) {
                case -1: case VI_KEY( F1 ):
                    break;
                case VI_KEY( F2 ):
                    rc = NextFile();
                    if( rc <= 0 ) {
                        repeat = TRUE;
                    }
                    break;
                case VI_KEY( F3 ):
                    rc = SaveAndExit( NULL );
                    if( rc <= 0 ) {
                        repeat = TRUE;
                    }
                    break;
                }
            }
        }

        filelistw_info.y2 = tmp;
        MemFreeList( fcnt, list );

    }

    /*
     * get rid of option stuff
     */
    CloseAWindow( optwin );
    return( rc );

} /* EditFileFromList */
#endif

/*
 * OpenWindowOnFile - open a window on a file
 */
int OpenWindowOnFile( char *data )
{
    char        *name;
    int         rc;
    window_id   wn;

    RemoveLeadingSpaces( data );
    name = data;
    if( data[0] == 0 ) {
        name = NULL;
    }
    wn = CurrentWindow;
    rc = NewFile( name, TRUE );
    if( !rc ) {
        InactiveWindow( wn );
        DCDisplayAllLines();
    }
    return( rc );

} /* OpenWindowOnFile */
