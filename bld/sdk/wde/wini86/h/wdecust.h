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
#define WDE_PREVIEW_ID     -1

#ifndef CCINFOORD
    #define CCINFOORD       2       /* information function ordinal */
#endif

#ifndef CCSTYLEORD
    #define CCSTYLEORD      3       /* styles function ordinal */
#endif

#ifndef CCFLAGSORD
    #define CCFLAGSORD      4       /* translate flags function ordinal */
#endif

#define CTLDATALENGTH       255

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
#ifdef STRICT
typedef HGLOBAL   (CALLBACK *LPFNLOADRES)( LPCSTR, LPCSTR );
typedef BOOL      (CALLBACK *LPFNEDITRES)( LPCSTR, LPCSTR );
#else
typedef HGLOBAL   (CALLBACK *LPFNLOADRES)( LPSTR, LPSTR );
typedef BOOL      (CALLBACK *LPFNEDITRES)( LPSTR, LPSTR );
#endif

typedef HGLOBAL (*WdeCustInfoProc)( void );
typedef BOOL    (CALLBACK *WdeCustStyleProc)( HWND, HGLOBAL, LPFNSTRTOID, LPFNIDTOSTR );
typedef UINT    (CALLBACK *WdeCustFlagsProc)( DWORD, LPSTR, UINT );
typedef HGLOBAL (CALLBACK *WdeListClassesProc)( LPSTR, UINT, LPFNLOADRES, LPFNEDITRES );

typedef struct {
    UINT       wType;                  /* type style           */
    UINT       wWidth;                 /* suggested width      */
    UINT       wHeight;                /* suggested height     */
    DWORD      dwStyle;                /* default style        */
    char       szDescr[CTLDESCR];      /* description          */
    /* The following fields are Borlands extension to the custom
     * control interface. They allow the editor to add custom
     * controls to its tool bar and change the drag and drop
     * cursor.
     */
    HBITMAP    hToolBit;               /* Toolbox bitmap       */
    HCURSOR    hDropCurs;              /* Drag and drop cursor */
} WDECTLTYPE;

typedef struct {
    UINT        wVersion;              /* control version */
    UINT        wCtlTypes;             /* control types */
    char        szClass[CTLCLASS];     /* control class name */
    char        szTitle[CTLTITLE];     /* control title */
    char        szReserved[10];        /* reserved for future use */
    WDECTLTYPE  Type[CTLTYPES];        /* Resource Workshop cntl type list */
} WDECTLINFO;

typedef struct {
    UINT    wX;                       /* x origin of control   */
    UINT    wY;                       /* y origin of control   */
    UINT    wCx;                      /* width of control      */
    UINT    wCy;                      /* height of control     */
    UINT    wId;                      /* control child id      */
    DWORD   dwStyle;                  /* control style         */
    char    szClass[CTLCLASS];        /* name of control class */
    char    szTitle[CTLTITLE];        /* control text          */
    /* The following fields are Borland extensions to the Micosoft
     * CTLCTYLE data structure. It allows access to the controls
     * CTLDATA field.
     */
    BYTE    CtlDataSize;              /* Control data Size     */
    BYTE    CtlData[CTLDATALENGTH];   /* Control data          */
} WDECTLSTYLE;

typedef struct {
    WdeCustInfoProc  info_proc;
    WdeCustStyleProc style_proc;
    WdeCustFlagsProc flags_proc;
    char             class_name[CTLCLASS];
} WdeBorlandCtlClass;

typedef struct {
    uint_16            nClasses;
    WdeBorlandCtlClass Classes[1];    /* have space for at least one */
} WdeBorlandClassList;

typedef struct WdeCustLibStruct {
    HINSTANCE   inst;
    Bool        ms_lib;
    Bool        load_only;
    char        *file_name;
    char        *info_name;
    char        *style_name;
    char        *flags_name;
    LIST        *controls;
    HGLOBAL     class_list;
} WdeCustLib;

typedef struct WdeCustControlStruct {
    Bool                ms_lib;
    WdeCustLib          *lib;
    WdeCustInfoProc     info_proc;
    WdeCustStyleProc    style_proc;
    WdeCustFlagsProc    flags_proc;
    union {
        WDECTLINFO  bor;
        CTLINFO     ms;
    } control_info;
} WdeCustControl;

typedef struct {
    WdeCustControl  *control;
    UINT            type;
} WdeCurrCustControl;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool WdeIsCurrentCustControlSet( int which );
extern Bool WdeCustControlsLoaded( void );
extern void WdeGetCurrentCustControl( int, WdeCustControl **, UINT * );
extern Bool WdeSetCurrentCustControl( int );
extern Bool WdeLoadCustomLib( Bool, Bool );
extern Bool WdeFreeAllCustLibs( void );
extern void WdeFindClassInAllCustLibs( char *, LIST ** );
extern void WdeFreeCustRESProcs( void );
extern Bool WdeIsBorBtnIDSupported( uint_16 );
extern void WdeMapCustomSize( uint_32 *w, uint_32 *h, WdeResizeRatio *r );

#endif
