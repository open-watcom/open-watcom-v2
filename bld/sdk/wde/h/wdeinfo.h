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


#ifndef WDEINFO_INCLUDED
#define WDEINFO_INCLUDED

#include "fmedit.def"
#include "wresall.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    OBJ_ID          obj_id;
    char           *symbol;
    WdeResInfo     *res_info;
    void           *obj;
    DialogSizeInfo  size;
    union {
        struct {
            char             *caption;
            WResID           *name;
        } d;
        struct {
            ResNameOrOrdinal *text;
            uint_16           id;
        } c;
    };
} WdeInfoStruct;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void   WdeSetFocusToInfo        ( void );
extern HWND   WdeGetInfoWindowHandle   ( void );
extern int    WdeGetInfoWindowDepth    ( void );
extern Bool   WdeCreateInfoWindow      ( HWND, HINSTANCE );
extern void   WdeResizeInfoWindow      ( RECT * );
extern void   WdeInfoFini              ( void );
extern void   WdeWriteObjectDimensions ( int, int, int, int );
extern void   WdeWriteInfo             ( WdeInfoStruct * );
extern void   WdeDestroyInfoWindow     ( void );
extern void   WdeShowInfoWindow        ( Bool );
extern BOOL   WdeIsInfoMessage         ( MSG *msg );

#endif
