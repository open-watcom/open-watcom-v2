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


#include "imgedit.h"
#include "iconinfo.h"


/* Local Window callback functions prototypes */
WINEXPORT WPI_DLGRESULT CALLBACK SelNonExistingProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
WINEXPORT WPI_DLGRESULT CALLBACK SelExistingProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );

static icon_info_struct iconInfo[NUM_OF_ICONS];
static int              iconType;
static int              iconNumber[NUM_OF_ICONS];
static int              numberOfIcons = 0;
static int              iconOperation = 0;
static char             *dialogBoxText[4];

/*
 * getIconType - returns the index of the icon resource.
 */
static short getIconType( int bitcount, int width, int height )
{
#if 0
    if( bitcount == 24 ) {
        if( height == 32 ) {
             return( XX );               // True col, 32x32
        } else if( height == 16 ) {
             return( XX );               // True col, 16x16
        } else if (height == 48 ) {
             return( XX );               // True col, 48x48
        } else {
             return ( -1 );
        }
    } else
#endif
    if( bitcount == 8 ) {
        if( height == 96 ) {
            return( 15 );              // 256 col, 96x96
        } else if( height == 64 ) {
            return( 12 );              // 256 col, 64x64
        } else if( height == 32 ) {
            return( 8 );               // 256 col, 32x32
        } else if( height == 16 ) {
            return( 9 );               // 256 col, 16x16
        } else if( height == 48 ) {
            return( 5 );               // 256 col, 48x48
        } else {
            return( -1 );
        }
    } else if( bitcount == 4 ) {
        if( height == 96 ) {
            return( 14 );              // 16 col, 96x96
        } else if( height == 64 ) {
            return( 11 );              // 16 col, 64x64
        } else if( height == 32 ) {
            return( 0 );               // 16 col, 32x32
        } else if( height == 16 ) {
            return( 3 );               // 16 col, 16x16
        } else if( height == 48 ) {
            return( 6 );               // 16 col, 48x48
        } else if( height == 24 ) {
            return( 7 );               // 16 col, 24x24
        } else {
            return( -1 );
        }
    } else if( bitcount == 1 ) {
        if( height == 96 ) {
            return( 13 );              // 2 col, 96x96
        } else if( height == 64 ) {
            return( 10 );              // 2 col, 32x32
        } else if( height == 32 ) {
            return( 1 );               // 2 col, 32x32
        } else if( height == 16 ) {
            if( width == 32 ) {
                return( 2 );           // 2 col, 32x16
            } else if( width == 16 ) {      
                return( 4 );           // 2 col, 16x16
            } else {
                return( -1 );
            }
        } else {
            return( -1 );
        }
    } else {
        return( -1 );
    }

} /* getIconType */

/*
 * FiniIconInfo - free what was allocated in InitIconInfo below
 */
void FiniIconInfo( void )
{
    int i;

    for( i = 0; i < NUM_OF_ICONS; i++ ) {
        if( iconInfo[i].text != NULL ) {
            IEFreeRCString( iconInfo[i].text );
        }
    }

    for( i = 0; i < SEL_ICON_OP + 1; i++ ) {
        if( dialogBoxText[i] != NULL ) {
            IEFreeRCString( dialogBoxText[i] );
        }
    }

} /* FiniIconInfo */

/*
 * InitIconInfo - initialize some icon information needed later
 */
void InitIconInfo( void )
{
    short       i;

    for( i = 0; i < NUM_OF_ICONS; i++ ) {
        iconInfo[i].exists = FALSE;
        iconInfo[i].width = 32;
        iconInfo[i].height = 32;
    }

    iconInfo[2].height = 16;
    iconInfo[3].height = iconInfo[3].width = 16;
    iconInfo[4].height = iconInfo[4].width = 16;
    iconInfo[5].height = iconInfo[5].width = 48;
    iconInfo[6].height = iconInfo[6].width = 48;
    iconInfo[7].height = iconInfo[7].width = 24;

    // iconInfo[8].height = iconInfo[8].width = 32;
    iconInfo[9].height = iconInfo[9].width = 16;

    iconInfo[10].height = iconInfo[10].width = 64;
    iconInfo[11].height = iconInfo[11].width = 64;
    iconInfo[12].height = iconInfo[12].width = 64;
    iconInfo[13].height = iconInfo[13].width = 96;
    iconInfo[14].height = iconInfo[14].width = 96;
    iconInfo[15].height = iconInfo[15].width = 96;

    /*
    // iconInfo[XX].height = iconInfo[10].width = 32;
    iconInfo[XX].height = iconInfo[11].width = 16;
    iconInfo[XX].height = iconInfo[12].width = 48;
    */
    
    iconInfo[0].text = IEAllocRCString( WIE_16COLOR32X32 );
    iconInfo[1].text = IEAllocRCString( WIE_2COLOR32X32 );
    iconInfo[2].text = IEAllocRCString( WIE_2COLOR32X16 );
    iconInfo[3].text = IEAllocRCString( WIE_16COLOR16X16 );
    iconInfo[4].text = IEAllocRCString( WIE_2COLOR16X16 );
    iconInfo[5].text = IEAllocRCString( WIE_256COLOR48X48 );
    iconInfo[6].text = IEAllocRCString( WIE_16COLOR48X48 );
    iconInfo[7].text = IEAllocRCString( WIE_16COLOR24X24 );

    iconInfo[8].text = IEAllocRCString( WIE_256COLOR32X32 );
    iconInfo[9].text = IEAllocRCString( WIE_256COLOR16X16 );

    iconInfo[10].text = IEAllocRCString( WIE_2COLOR64X64 );
    iconInfo[11].text = IEAllocRCString( WIE_16COLOR64X64 );
    iconInfo[12].text = IEAllocRCString( WIE_256COLOR64X64 );
    iconInfo[13].text = IEAllocRCString( WIE_2COLOR96X96 );
    iconInfo[14].text = IEAllocRCString( WIE_16COLOR96X96 );
    iconInfo[15].text = IEAllocRCString( WIE_256COLOR96X96 );
    /*
    iconInfo[XX].text = IEAllocRCString( WIE_TRUECOLOR32X32 );
    iconInfo[XX].text = IEAllocRCString( WIE_TRUECOLOR16X16 );
    iconInfo[XX].text = IEAllocRCString( WIE_TRUECOLOR48X48 );
    */
    
    iconInfo[0].bitcount = 4;
    iconInfo[1].bitcount = 1;
    iconInfo[2].bitcount = 1;
    iconInfo[3].bitcount = 4;
    iconInfo[4].bitcount = 1;
    iconInfo[5].bitcount = 8;
    iconInfo[6].bitcount = 4;
    iconInfo[7].bitcount = 4;

    iconInfo[8].bitcount = 8;
    iconInfo[9].bitcount = 8;

    iconInfo[10].bitcount = 2;
    iconInfo[11].bitcount = 4;
    iconInfo[12].bitcount = 8;
    iconInfo[13].bitcount = 2;
    iconInfo[14].bitcount = 4;
    iconInfo[15].bitcount = 8;
    
    /*
    iconInfo[XX].bitcount = 24;
    iconInfo[XX].bitcount = 24;
    iconInfo[XX].bitcount = 24;
    */

    dialogBoxText[NEW_ICON_OP]    = IEAllocRCString( WIE_SELECTTARGETICON );
    dialogBoxText[ADD_ICON_OP]    = IEAllocRCString( WIE_SELECTTYPEOFNEWICON );
    dialogBoxText[DELETE_ICON_OP] = IEAllocRCString( WIE_SELECTICONTODELETE );
    dialogBoxText[SEL_ICON_OP]    = IEAllocRCString( WIE_SELECTICONTOEDIT );

} /* InitIconInfo */

/*
 * resetIconInfo - reset the icon information
 */
static void resetIconInfo( void )
{
    short       i;

    for( i = 0; i < NUM_OF_ICONS; i++ ) {
        iconInfo[i].exists = FALSE;
        iconNumber[i] = -1;
    }
    numberOfIcons = 0;

} /* resetIconInfo */

/*
 * getIconIndex - given an icon type (0-6), return that icon's index
 *              - we can only have one of each type of icon
 */
static short getIconIndex( int icon_type )
{
    short       i;

    for( i = 0; i < numberOfIcons; i++ ) {
        if( iconNumber[i] == icon_type ) {
            return( i );
        }
    }
    return( -1 );

} /* getIconIndex */

/*
 * SelNonExistingProc - select an icon that does not exist (i.e. for ADD or NEW)
 */
WPI_DLGRESULT CALLBACK SelNonExistingProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    int         index;
    WPI_MRESULT mresult;
    int         i;
    static int  lbindex[NUM_OF_ICONS];

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD( wparam ) ) {
        case DLGID_OK:
            mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_GETCURSEL, 0, 0L );
            index = _imgwpi_mresulttoint( mresult );
            iconType = lbindex[index];
            _wpi_enddialog( hwnd, DLGID_OK );
            break;

        case DLGID_CANCEL:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;

        case IDB_HELP:
            IEHelpRoutine();
            return( FALSE );

        case TARGETLISTBOX:
            if( HIWORD( wparam ) == LBN_DBLCLK ) {
                mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX,
                                                   LB_GETCURSEL, 0, 0L );
                index = _imgwpi_mresulttoint( mresult );
                iconType = lbindex[index];
                _wpi_enddialog( hwnd, DLGID_OK );
            }
            break;

        default:
            return( FALSE );
        }
    } else {
        switch( msg ) {
        case WM_INITDIALOG:
            _wpi_setdlgitemtext( hwnd, DIALOGTEXT, dialogBoxText[iconOperation] );
            index = 0;
            for( i = 0; i < NUM_OF_ICONS; i++ ) {
                if( !iconInfo[i].exists ) {
                    mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_INSERTSTRING, -1,
                                                       (LPARAM)(LPSTR)iconInfo[i].text );
                    index = _imgwpi_mresulttoint( mresult );
                    lbindex[index] = i;
                    ++index;
                }
            }
            _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_SETCURSEL, 0, 0L );
            return( TRUE );

#ifndef __OS2_PM__
        case WM_SYSCOLORCHANGE:
            IECtl3dColorChange();
            break;
#endif
        case WM_CLOSE:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;
        default:
            return( _wpi_defdlgproc( hwnd, msg, wparam, lparam ) );
        }
    }
    _wpi_dlgreturn( FALSE );

} /* SelNonExistingProc */

/*
 * SelExistingProc - select an existing icon (i.e. for edit or delete)
 */
WPI_DLGRESULT CALLBACK SelExistingProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_MRESULT mresult;
    int         index;
    int         i;
    static int  lbindex[NUM_OF_ICONS];

    if( _wpi_dlg_command( hwnd, &msg, &wparam, &lparam ) ) {
        switch( LOWORD( wparam ) ) {
        case DLGID_OK:
            mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_GETCURSEL, 0, 0L );
            index = _imgwpi_mresulttoint( mresult );
            iconType = lbindex[index];
            _wpi_enddialog( hwnd, DLGID_OK );
            break;

        case DLGID_CANCEL:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;

        case IDB_HELP:
            IEHelpRoutine();
            return( FALSE );

        case TARGETLISTBOX:
            if( HIWORD( wparam ) == LBN_DBLCLK ) {
                mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX,
                                                   LB_GETCURSEL, 0, 0L );
                index = _imgwpi_mresulttoint( mresult );
                iconType = lbindex[index];
                _wpi_enddialog( hwnd, DLGID_OK );
            }
            break;

        default:
            return( FALSE );
        }
    } else {
        switch( msg ) {
        case WM_INITDIALOG:
            _wpi_setdlgitemtext( hwnd, DIALOGTEXT, dialogBoxText[iconOperation] );
            index = 0;
            for( i = 0; i < numberOfIcons; i++ ) {
                if( iconInfo[iconNumber[i]].exists ) {
                    mresult = _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_INSERTSTRING,
                        -1, (LPARAM)(LPSTR)iconInfo[iconNumber[i]].text );
                    index = _imgwpi_mresulttoint( mresult );
                    lbindex[index] = iconNumber[i];
                    index++;
                }
            }
            _wpi_senddlgitemmessage( hwnd, TARGETLISTBOX, LB_SETCURSEL, 0, 0L );
            return( TRUE );

#ifndef __OS2_PM__
        case WM_SYSCOLORCHANGE:
            IECtl3dColorChange();
            break;
#endif

        case WM_CLOSE:
            _wpi_enddialog( hwnd, DLGID_CANCEL );
            break;
        default:
            return( _wpi_defdlgproc( hwnd, msg, wparam, lparam ) );
        }
    }
    _wpi_dlgreturn( FALSE );

} /* SelExistingProc */

/*
 * CreateNewIcon - select the icon from the listbox (used by FILE.NEW)
 *               - if is_icon is FALSE, then this is a pointer (cursor)
 */
BOOL CreateNewIcon( short *width, short *height, short *bitcount, BOOL is_icon )
{
    WPI_PROC            fp;
    WPI_DLGRESULT       button_type;

    iconOperation = NEW_ICON_OP;
    resetIconInfo();

    fp = _wpi_makeprocinstance( (WPI_PROC)SelNonExistingProc, Instance );
    if( is_icon ) {
        button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, ICONTYPE, 0L );
    } else {
        button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, CURSORTYPE, 0L );
    }
    _wpi_freeprocinstance( fp );

    if( button_type == DLGID_CANCEL ) {
        return( FALSE );
    }

    *width = iconInfo[iconType].width;
    *height = iconInfo[iconType].height;
    *bitcount = iconInfo[iconType].bitcount;

    iconInfo[iconType].exists = TRUE;
    iconNumber[numberOfIcons] = iconType;
    numberOfIcons++;
    return( TRUE );

} /* CreateNewIcon */

/*
 * AddNewIcon - add a new icon in the current icon file
 */
void AddNewIcon( void )
{
    WPI_PROC            fp;
    WPI_DLGRESULT       button_type;
    img_node            *currentnode;
    img_node            *node;
    img_node            new_icon;
    int                 i;
    int                 imagecount;
    HMENU               hmenu;

    currentnode = GetCurrentNode();

    if( currentnode->imgtype != ICON_IMG ) {
        return;
    }

    node = GetImageNode( currentnode->hwnd );

    iconOperation = ADD_ICON_OP;
    fp = _wpi_makeprocinstance( (WPI_PROC)SelNonExistingProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, ICONTYPE, 0L );
    _wpi_freeprocinstance( fp );

    if( button_type == DLGID_CANCEL ) {
        return;
    }

    imagecount = node->num_of_images;
    currentnode = node;
    for( i = 0; i < imagecount; i++ ) {
        if( currentnode == NULL ) {
            break;
        }
        currentnode->num_of_images++;
        currentnode = currentnode->nexticon;
    }

    memcpy( &new_icon, node, sizeof( img_node ) );
    new_icon.width = iconInfo[iconType].width;
    new_icon.height = iconInfo[iconType].height;
    new_icon.bitcount = iconInfo[iconType].bitcount;
    new_icon.next = NULL;
    new_icon.nexticon = NULL;

    /*
     * this will make the XOR and the AND bitmaps
     */
    MakeIcon( &new_icon, TRUE );
    AddIconToList( &new_icon, node );

    iconInfo[iconType].exists = TRUE;
    iconNumber[numberOfIcons] = iconType;
    numberOfIcons++;

    AddIconUndoStack( &new_icon );
    SelectIcon( imagecount );
    SetIsSaved( new_icon.hwnd, FALSE );

    hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
    _wpi_enablemenuitem( hmenu, IMGED_SELIMG, TRUE, FALSE );
    PrintHintTextByID( WIE_ICONADDEDTEXT, NULL );

} /* AddNewIcon */

/*
 * DeleteIconImg - delete one of the icons from the current icon file
 */
void DeleteIconImg( void )
{
    WPI_PROC            fp;
    WPI_DLGRESULT       button_type;
    int                 icon_index;
    img_node            *currentnode;
    img_node            *node;
    img_node            *newnode;
    short               i;
    HMENU               hmenu;

    currentnode = GetCurrentNode();

    if( currentnode->imgtype != ICON_IMG ) {
        return;
    }

    node = GetImageNode( currentnode->hwnd );
    iconOperation = DELETE_ICON_OP;

    fp = _wpi_makeprocinstance( (WPI_PROC)SelExistingProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, ICONTYPE, 0L );
    _wpi_freeprocinstance( fp );

    if( button_type == DLGID_CANCEL ) {
        return;
    }

    icon_index = getIconIndex( iconType );
    if( icon_index < 0 ) {
        WImgEditError( WIE_ERR_BAD_ICONINDEX, WIE_INTERNAL_004 );
        return;
    }

    iconInfo[iconType].exists = FALSE;

    currentnode = node;
    while( currentnode ) {
        currentnode->num_of_images -= 1;
        currentnode = currentnode->nexticon;
    }

    newnode = RemoveIconFromList( node, icon_index );
    DelIconUndoStack( newnode, icon_index );
    MakeIcon( newnode, FALSE );         // This will set it as active image

    for( i = icon_index; i < numberOfIcons - 1; i++ ) {
        iconNumber[i] = iconNumber[i + 1];
    }
    numberOfIcons--;
    if( numberOfIcons < 2 ) {
        hmenu = GetMenu( _wpi_getframe( HMainWindow ) );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
    }

    SelectIcon( 0 );
    PrintHintTextByID( WIE_ICONDELETEDTEXT, NULL );

} /* DeleteIconImg */

/*
 * SelectIconImg - select an icon from a multiple icon editing session
 */
void SelectIconImg( void )
{
    WPI_PROC            fp;
    WPI_DLGRESULT       button_type;
    int                 icon_index;
    img_node            *currentnode;
    img_node            *node;

    currentnode = GetCurrentNode();

    if( currentnode->imgtype != ICON_IMG ) {
        return;
    }

    node = GetImageNode( currentnode->hwnd );
    iconOperation = SEL_ICON_OP;

    fp = _wpi_makeprocinstance( (WPI_PROC)SelExistingProc, Instance );
    button_type = _wpi_dialogbox( HMainWindow, (WPI_DLGPROC)fp, Instance, ICONTYPE, 0L );
    _wpi_freeprocinstance( fp );

    if( button_type == DLGID_CANCEL ) {
        return;
    }

    icon_index = getIconIndex( iconType );
    if( icon_index < 0 ) {
        WImgEditError( WIE_ERR_BAD_ICONINDEX, WIE_INTERNAL_005 );
        return;
    }

    SelectIcon( icon_index );

} /* SelectIconImg */

/*
 * SetIconInfo - set the icon information when an icon is opened or focused on
 */
void SetIconInfo( img_node *node )
{
    int         i;
    int         icon_type;
    img_node    *icon;

    if( node->imgtype != ICON_IMG ) {
        return;
    }
    resetIconInfo();
    icon = GetImageNode( node->hwnd );

    numberOfIcons = node->num_of_images;
    for( i = 0; i < numberOfIcons; i++ ) {
        if( icon == NULL ) {
            WImgEditError( WIE_ERR_BAD_ICONINDEX, WIE_INTERNAL_008 );
            break;
        }
        icon_type = getIconType( icon->bitcount, icon->width, icon->height );
        iconNumber[i] = icon_type;
        iconInfo[icon_type].exists = TRUE;
        icon = icon->nexticon;
    }

} /* SetIconInfo */
