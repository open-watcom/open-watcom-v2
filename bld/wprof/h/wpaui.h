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


enum {
    MENU_FILE               = 1000,
    MENU_OPEN_SAMPLE,
    MENU_CLOSE_SAMPLE,
    MENU_SYSTEM,
    MENU_MEMPRT,
    MENU_OPTIONS,
    MENU_EXIT,

    MENU_CONVERT            = 2000,
    MENU_CONVERT_MODULE,
    MENU_CONVERT_IMAGE,
    MENU_CONVERT_ALL,

    MENU_WINDOWS            = 3000,

    MENU_ACTIONS            = 4000,

    MENU_SAMP_ZOOM_IN       = 5000,
    MENU_SAMP_BACK_OUT,
    MENU_SAMP_GATHER,
    MENU_SAMP_BAR,
    MENU_SAMP_SORT,

    MENU_SAMP_BAR_MAX_TIME  = 6000,
    MENU_SAMP_ABS,
    MENU_SAMP_REL,

    MENU_SORT_COUNT         = 7000,
    MENU_SORT_NAME,

    MENU_HELP               = 8000,
    MENU_HELP_CONTENTS,
#if defined( __WINDOWS__ ) || defined( __NT__ ) || defined( __OS2_PM__ )
    MENU_HELP_ONHELP,
    MENU_HELP_SEARCH,
#endif
    MENU_ABOUT,

//    MENU_IMAGE_CHG_EXEC     = 3000,
//    MENU_IMAGE_CHG_SYMBOLS,
//    MENU_IMAGE_DEL_SYMBOLS,

//    MENU_SIMAGE_SHOW_SAMPS  = 6000,
//    MENU_SIMAGE_SHOW_IMAGE_TYPE,
//    MENU_SIMAGE_CHANGE_EXE,
//    MENU_SIMAGE_CHANGE_SYM,
//    MENU_SIMAGE_DELETE_SYM,
};

enum {
    WPA_PLAIN = WND_FIRST_UNUSED,
    WPA_TABSTOP,
    WPA_SELECTED,
    WPA_STANDOUT,
    WPA_HOTSPOT,
    WPA_STANDOUT_TABSTOP,
    WPA_STATUS_LINE,
    WPA_ABS_BAR,
    WPA_REL_BAR,
    WPA_PLAIN_INACTIVE,
    WPA_OVERVIEW_NAME,
    WPA_CLEAR_EXTRA,
    WPA_NUMBER_OF_COLOURS
};

enum {
    WP_NO_EVENT,
    WP_SETSAMPS_EVENT,
};
