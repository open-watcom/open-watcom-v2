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


MENU_ITEM( MENU_MAIN_FILE_OPEN,         XOpen_  )
MENU_ITEM( MENU_MAIN_FILE_VIEW,         XView_  )
MENU_ITEM( MENU_MAIN_FILE_COMMAND,      XCommand_  )
MENU_BAR
MENU_ITEM( MENU_MAIN_FILE_OPTIONS,      OpXtions_  )
MENU_ITEM( MENU_MAIN_WINDOW_SETTINGS,   XWindow_Options_ )
#if defined(__GUI__)
MENU_ITEM( MENU_MAIN_FILE_FONT,         XFont_  )
#endif
MENU_ITEM( MENU_MAIN_FILE_SAVE_CONFIGURATION,   XSave_Setup_  )
MENU_ITEM( MENU_MAIN_FILE_LOAD_CONFIGURATION,   XLoad_Setup_  )
MENU_ITEM( MENU_MAIN_FILE_SOURCE_PATH,          Source_XPath_  )
MENU_BAR
#if !defined(__GUI__) && !defined(__WINDOWS__) && !defined(__NT__)
MENU_ITEM( MENU_MAIN_FILE_SYSTEM,       SXystem  )
MENU_BAR
#endif
MENU_ITEM( MENU_MAIN_FILE_EXIT,         EXxit  )
