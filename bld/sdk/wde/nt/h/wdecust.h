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


#ifndef WDECUST_INCLUDED
#define WDECUST_INCLUDED

#include "wdeglbl.h"
#include <custcntl.h>
#include "list.def"
#include "wderesiz.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
/* we do not support UNICODE yet */
#ifdef UNICODE
#undef UNICODE
#endif

#define WDE_PREVIEW_ID     -1

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

typedef struct WdeCustLibStruct {
    HANDLE     inst;
    Bool       nt_lib;
    Bool       load_only;
    char      *file_name;
    LPCCINFO   lpcci;
    UINT       num_classes;
} WdeCustLib;

typedef struct {
    WdeCustLib *lib;
    UINT        index;
} WdeCurrCustControl;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool     WdeIsCurrentCustControlSet( int which );
extern Bool     WdeCustControlsLoaded     ( void );
extern Bool    WdeIsBorBtnIDSupported    ( uint_16 );
extern void    WdeGetCurrentCustControl  ( int, WdeCustLib **, UINT * );
extern Bool    WdeSetCurrentCustControl  ( int );
extern Bool    WdeLoadCustomLib          ( Bool, Bool );
extern Bool    WdeFreeAllCustLibs        ( void );
extern void    WdeFindClassInAllCustLibs ( char *, LIST ** );
extern Bool    WdeFindLibIndexFromInfo   ( LPCCINFO, WdeCustLib **, UINT * );
extern void    WdeMapCustomSize          ( UINT *w, UINT *h, WdeResizeRatio *r );

#endif
