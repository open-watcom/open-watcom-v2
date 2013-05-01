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


#ifndef WRESIDS_INCLUDED
#define WRESIDS_INCLUDED

#ifdef WIN_GUI
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

/* Wes Nelson -- Added the following #ifndef to allow windows.h
 *               to be included before this file
 */
#ifndef WIN_GUI

/*** predefined type numbers ***/
#define RT_CURSOR           1
#define RT_BITMAP           2
#define RT_ICON             3
#define RT_MENU             4
#define RT_DIALOG           5
#define RT_STRING           6
#define RT_FONTDIR          7
#define RT_FONT             8
#define RT_ACCELERATOR      9
#define RT_RCDATA           10
#define RT_GROUP_CURSOR     12      /* note: no type 13 resource */
#define RT_GROUP_ICON       14

#endif

// these symbols are not defined in windows.h
#ifndef RT_VERSIONINFO
#define RT_VERSIONINFO      16
#endif

#ifndef RT_MESSAGETABLE
#define RT_MESSAGETABLE     11      /* for NT res files only */
#endif
#ifndef RT_ERRTABLE
#define RT_ERRTABLE         11
#endif
#ifndef RT_NAMETABLE
#define RT_NAMETABLE        15
#endif
#ifndef RT_VERSION
#define RT_VERSION          16
#endif
#ifndef RT_DLGINCLUDE
#define RT_DLGINCLUDE       17
#endif

#define RT_TOOLBAR          0xF1

/* style bits used by name for Dialog box headers and controls */
/* these statements same as found in windows.h */

/* Wes Nelson -- Added the following #ifndef's to allow windows.h
 *               to be included before this file
 */

#if !defined( WIN_GUI ) && !defined( RES2WXF_COMPILE )

#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_VISIBLE          0x10000000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_CAPTION          ( WS_BORDER | WS_DLGFRAME )
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define DS_SETFONT          0x00000040L
/* edit control styles */
#define ES_LEFT             0x00000000L
#define ES_WANTRETURN       0x00001000L
#define ES_MULTILINE        0x00000004L
/* button control styles */
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_AUTORADIOBUTTON  0x00000009L
/* listbox control styles */
#define LBS_NOTIFY          0x00000001L
#define LBS_SORT            0x00000002L
#define LBS_NOINTEGRALHEIGHT        0x00000100L
/* static control styles */
#define SS_LEFT             0x00000000L
#define SS_CENTER           0x00000001L
#define SS_RIGHT            0x00000002L
#define SS_ICON             0x00000003L
#define SS_LEFTNOWORDWRAP   0x0000000CL
#define SS_NOPREFIX         0x00000080L
/* combobox styles */
#define CBS_SIMPLE          0x00000001L
#define CBS_DROPDOWN        0x00000002L
#define CBS_DROPDOWNLIST    0x00000003L
#define CBS_SORT            0x00000100L
#define CBS_NOINTEGRALHEIGHT        0x00000400L
/* scrollbar styles */
#define SBS_HORZ            0x00000000L

#endif

#endif
