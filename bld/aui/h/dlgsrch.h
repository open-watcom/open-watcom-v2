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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define DIALOG_SEARCH 502
#define DIALOG_SEARCH_ALL 503

#define CTL_SRCH_RX 106
#define CTL_SRCH_EDIT_RX 107
#define CTL_SRCH_CASE 105
#define CTL_SRCH_NEXT 100
#define CTL_SRCH_PREV 101
#define CTL_SRCH_CANCEL 102
#define CTL_SRCH_EDIT 103
#define CTL_SRCH_LIST 104


extern int  DlgSearch( a_window *wnd, void *history );
extern bool DlgSearchAll( char **expr, void *history );
