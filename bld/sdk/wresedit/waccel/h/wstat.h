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


#ifndef WSTAT_INCLUDED
#define WSTAT_INCLUDED

#include <stdlib.h>
#include "mstatwnd.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_STATUS_TEXT   _MAX_PATH

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct wstatbar {
    statwnd  *stat;
    char      text[2*MAX_STATUS_TEXT+2];
} wstatbar;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool      WInitStatusLines     ( HINSTANCE );
extern void      WFiniStatusLines     ( void );
extern int       WGetStatusDepth      ( void );
extern void      WResizeStatusWindows ( wstatbar *, RECT * );
extern void      WDestroyStatusLine   ( wstatbar * );
extern wstatbar *WCreateStatusLine    ( HWND, HINSTANCE );
extern Bool      WSetStatusText       ( wstatbar *, const char *, const char * );
extern Bool      WSetStatusReadyText  ( wstatbar *wsb );
extern Bool      WSetStatusByID       ( wstatbar *wsb, DWORD id1, DWORD id2 );

#endif
