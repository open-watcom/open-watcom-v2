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


#define NEW_ICON_OP     0
#define DELETE_ICON_OP  1
#define ADD_ICON_OP     2
#define SEL_ICON_OP     3
//#define NUM_OF_ICONS    10
#define NUM_OF_ICONS   16

typedef struct {
    char        *text;
    BOOL        exists;
    short       width;
    short       height;
    short       bitcount;
} icon_info_struct;

WINEXPORT WPI_DLGRESULT CALLBACK SelNonExistingProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
WINEXPORT WPI_DLGRESULT CALLBACK SelExistingProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
