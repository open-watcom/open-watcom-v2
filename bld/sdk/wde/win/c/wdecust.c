/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wdeglbl.h"
#include <ctype.h>
#include "wdecust.h"
#include "wdemain.h"
#include "wdeinfo.h"
#include "wdefont.h"
#include "wdegetfn.h"
#include "wdegeted.h"
#include "wdelist.h"
#include "wdemsgbx.h"
#include "wdestat.h"
#include "wdedebug.h"
#include "wde.rh"
#include "jdlg.h"
#include "wrdll.h"
#include "wresdefn.h"
#include "wclbproc.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_NUM_CUSTOMS         2
#define WDE_CHECK_WHICH( w )    (((w < 0) || (w >= WDE_NUM_CUSTOMS)) ? FALSE : TRUE)
#define WDE_PREVIEW_PAD         10

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT INT_PTR   CALLBACK WdeSelectCustDlgProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT INT_PTR   CALLBACK WdeLoadCustDlgProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT HGLOBAL   CALLBACK WdeCustLOADRES( LPCSTR, LPCSTR );
WINEXPORT BOOL      CALLBACK WdeCustEDITRES( LPCSTR, LPCSTR );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool             WdeLoadMSCustomControls( WdeCustLib *cust_lib );
static bool             WdeLoadBorCustomControls( WdeCustLib *cust_lib );
static WdeCustLib       *WdeAllocCustLib( void );
static WdeCustControl   *WdeAllocCustControl( void );
static WdeCustControl   *WdeAllocCustControlFromWDECTLINFO( WDECTLINFO *, bool );
static bool             WdeCreateAndAddCustControl( WdeCustLib *cust_lib, WdeCustInfoProc, WdeCustStyleProc, WdeCustFlagsProc );
static bool             WdeAddBorControlsToCustLib( WdeCustLib *cust_lib, WdeBorlandClassList * );
static void             WdeAddCustControlToCustLib( WdeCustLib *cust_lib, WdeCustControl *cust_ctrl );
static bool             WdeFreeCustLib( WdeCustLib *cust_lib );
static void             WdeFreeCustLibControls( LIST ** );
static bool             WdeFreeCustControl( WdeCustControl *cust_ctrl );
static void             WdeFindClassInCustLibControls( const char *, LIST **, LIST * );
static void             WdeFindClassInCustLib( const char *, LIST **, WdeCustLib *cust_lib );
static bool             WdeQueryUnsafeMSLoad( void );
static bool             WdeQueryAssumeMS( void );
static void             WdeFreeSelectWinCBox( HWND );
static bool             WdeSetSelectWinCBox( HWND, WdeCustControl *cust_ctrl );
static bool             WdeSetSelectWin( HWND );
static bool             WdeSetCurrentControl( HWND, int );
static bool             WdePreviewSelected( HWND );
static void             WdeSetLoadCustInfo( HWND, WdeCustLib *cust_lib );
static void             WdeSetLoadCustInfoOrd( HWND hDlg );
static bool             WdeGetLoadCustInfo( HWND, WdeCustLib *cust_lib );
static void             WdeEnableNonStandardCustInfo( HWND, bool, bool );

/****************************************************************************/
/* extern variables                                                         */
/****************************************************************************/
extern char     *WdeCustOpenTitle;
extern char     *WdeCustFilter;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HFONT WdeSelectFont = NULL;

static LIST *WdeCustomLibList   = NULL;
static LIST *WdeLibList         = NULL;

static FARPROC  WdeCustLOADRESInst = NULL;
static FARPROC  WdeCustEDITRESInst = NULL;

WdeCurrCustControl WdeCurrControl[WDE_NUM_CUSTOMS] = {
    { NULL, 0 },
    { NULL, 0 }
};

static uint_16 WdeSupportedBorBtnIDs[] =
{
#if 0
    1, 2, 3, 4, 5, 6, 7, 201, 901, 902, 903, 904, 998,
#endif
    WDE_PREVIEW_ID
};

#define NUM_BOR_BTNS    (sizeof( WdeSupportedBorBtnIDs ) / sizeof( uint_16 ))


bool WdeIsBorBtnIDSupported( uint_16 id )
{
    int i;

    for( i = 0; i < NUM_BOR_BTNS; i++ ) {
        if( WdeSupportedBorBtnIDs[i] == id ) {
            return( true );
        }
    }

    return( false );
}

void WdeGetCurrentCustControl( int which, WdeCustControl **pcust_ctrl, UINT *type )
{
    if( !WDE_CHECK_WHICH( which ) ) {
        WdeWriteTrail( "WdeGetCurrentCustControl: bad which!" );
        return;
    }
    *pcust_ctrl = WdeCurrControl[which].control;
    *type = WdeCurrControl[which].type;
}

bool WdeIsCurrentCustControlSet( int which )
{
    if( WDE_CHECK_WHICH( which ) ) {
        if( WdeCurrControl[which].control != NULL ) {
            return( true );
        }
    }
    return( false );
}

bool WdeCustControlsLoaded( void )
{
    return( WdeCustomLibList != NULL );
}

bool WdeSetCurrentCustControl( int which )
{
    INT_PTR   ret;
    HINSTANCE inst;

    if( WdeCustomLibList == NULL ) {
        WdeSetStatusByID( 0, WDE_NOCUSTLOADED );
        return( true );
    }

    if( !WDE_CHECK_WHICH( which ) ) {
        WdeWriteTrail( "WdeSetCurrentCustControl: bad which!" );
        return( false );
    }

    inst = WdeGetAppInstance();
    {
        DLGPROC dlgproc = MakeProcInstance_DLG( WdeSelectCustDlgProc, inst );
        if( dlgproc == NULL ) {
            WdeWriteTrail( "WdeSetCurrentCustomControl: MakeProcInstance failed!" );
            return( false );
        }
        ret = JDialogBoxParam( inst, "WdeSelectCustom", WdeGetMainWindowHandle(), dlgproc, (LPARAM)(LPVOID)&which );
        FreeProcInstance_DLG( dlgproc );
    }
    /* if the window could not be created return FALSE */
    if( ret == -1 ) {
        WdeWriteTrail( "WdeSetCurrentCustomControl: "
                       "Could not create selection window!" );
        return( false );
    }

    return( true );
}

bool WdeLoadCustomLib( bool ms_lib, bool load_only )
{
    char                *name;
    HINSTANCE           inst;
    WdeCustLib          *cust_lib;
    bool                ret;
    WdeGetFileStruct    gf;

    gf.file_name = NULL;
    gf.title = WdeCustOpenTitle;
    gf.filter = WdeCustFilter;

    name = WdeGetOpenFileName( &gf );

    if( name == NULL ) {
        return( false );
    }

    inst = LoadLibrary( name );

    if( inst <= HINSTANCE_ERROR ) {
        WdeWriteTrail( "WdeLoadCustomLib: LoadLibrary call failed!" );
        MemFree( name );
        return( false );
    }

    cust_lib = WdeAllocCustLib();
    if( cust_lib == NULL ) {
        WdeWriteTrail( "WdeLoadCustomLib: WdeAllocCustLib failed!" );
        MemFree( name );
        FreeLibrary( inst );
        return( false );
    }

    cust_lib->inst = inst;
    cust_lib->ms_lib = ms_lib;
    cust_lib->load_only = load_only;
    cust_lib->file_name = name;

    if( load_only ) {
        WdeSetStatusByID( 0, WDE_LIBRARYLOADED );
        ret = true;
    } else {
        if( ms_lib ) {
            ret = WdeLoadMSCustomControls( cust_lib );
        } else {
            ret = WdeLoadBorCustomControls( cust_lib );
        }
    }

    if( !ret ) {
        WdeFreeCustLib( cust_lib );
        return( false );
    }

    if( load_only ) {
        WdeInsertObject( &WdeLibList, (OBJPTR)cust_lib );
    } else {
        WdeInsertObject( &WdeCustomLibList, (OBJPTR)cust_lib );
    }

    return( true );
}

bool WdeLoadMSCustomControls( WdeCustLib *cust_lib )
{
    WdeCustInfoProc  info_proc;
    WdeCustStyleProc style_proc;
    WdeCustFlagsProc flags_proc;
    HWND             dialog_owner;
    HINSTANCE        app_inst;
    INT_PTR          rc;

    dialog_owner = WdeGetMainWindowHandle();
    app_inst = WdeGetAppInstance();
    {
        DLGPROC dlgproc = MakeProcInstance_DLG( WdeLoadCustDlgProc, app_inst );
        rc = JDialogBoxParam( app_inst, "WdeLoadCustom", dialog_owner, dlgproc, (LPARAM)(LPVOID)cust_lib );
        FreeProcInstance_DLG( dlgproc );
    }
    if( rc == -1 ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: Dialog not created!" );
        return( false );
    }

    if( rc == 0 ) {
        return( false );
    }

    if( IS_INTRESOURCE( cust_lib->info_name ) ) {
        if( !WdeQueryUnsafeMSLoad() ) {
            WdeWriteTrail( "WdeLoadMSCustomControls: User aborted unsafe load!" );
            return( false );
        }
    }

    info_proc = (WdeCustInfoProc)GetProcAddress( cust_lib->inst, cust_lib->info_name );
    style_proc = (WdeCustStyleProc)GetProcAddress ( cust_lib->inst, cust_lib->style_name );
    flags_proc = (WdeCustFlagsProc)GetProcAddress ( cust_lib->inst, cust_lib->flags_name );

    if( info_proc == NULL || style_proc == NULL || flags_proc == NULL ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: Could not get proc address of ordinal!" );
        return( false );
    }

    if( !WdeCreateAndAddCustControl( cust_lib, info_proc, style_proc, flags_proc ) ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: Control not added to list!" );
        return( false );
    }

    WRMemValidate( cust_lib, _TRMEM_WHO( 6 ) );

    return( true );
}

bool WdeLoadBorCustomControls( WdeCustLib *cust_lib )
{
    WdeListClassesProc  list_proc;
    HGLOBAL             list_global;
    uint_8              *list_locked;
    uint_16             num_classes;
    WdeBorlandClassList *class_list;
    size_t              class_list_size;

    list_proc = (WdeListClassesProc)GetProcAddress( cust_lib->inst, "ListClasses" );

    if( list_proc == NULL ) {
        if( WdeQueryAssumeMS() ) {
            cust_lib->ms_lib = true;
            return( WdeLoadMSCustomControls( cust_lib ) );
        } else {
            WdeWriteTrail( "WdeLoadBorCustomControls: User aborted load!" );
            return( false );
        }
    }

    if( WdeCustLOADRESInst == NULL ) {
        WdeCustLOADRESInst = MakeProcInstance( (FARPROC)WdeCustLOADRES, WdeGetAppInstance() );
    }

    if( WdeCustEDITRESInst == NULL ) {
        WdeCustEDITRESInst = MakeProcInstance( (FARPROC)WdeCustEDITRES, WdeGetAppInstance() );
    }

#if 0
    list_global = (*list_proc)( "WdeMainClass", WDE_VERSION, (LPFNLOADRES)WdeCustLOADRES, (LPFNEDITRES)WdeCustEDITRES );
#else
    list_global = (*list_proc)( "WdeMainClass", WDE_VERSION, (LPFNLOADRES)WdeCustLOADRESInst, (LPFNEDITRES)WdeCustEDITRESInst );
#endif

    if( list_global == NULL ) {
        WdeWriteTrail( "WdeLoadBorCustomControls: ListClasses returned NULL!" );
        return( false );
    }

    list_locked = (uint_8 *)GlobalLock( list_global );
    if( list_locked == NULL ) {
        WdeWriteTrail( "WdeLoadBorCustomControls: Could lock global memory!" );
        GlobalFree( list_global );
        return( false );
    }

    num_classes = *(uint_16 *)list_locked;

    class_list_size = sizeof( WdeBorlandClassList ) +
                      (num_classes - 1) * sizeof( WdeBorlandCtlClass );

    class_list = MemAlloc( class_list_size );
    if( class_list == NULL ) {
        WdeWriteTrail( "WdeLoadBorCustomControls: class list alloc failed!" );
        GlobalUnlock( list_global );
        GlobalFree( list_global );
        return( false );
    }

    memcpy( class_list, list_locked, class_list_size );

    cust_lib->class_list = list_global;

    if( !WdeAddBorControlsToCustLib( cust_lib, class_list ) ) {
        WdeWriteTrail( "WdeLoadBorCustomControls: Add to CustLib failed!" );
        MemFree( class_list );
        GlobalUnlock( list_global );
        GlobalFree( list_global );
        return( false );
    }

    MemFree( class_list );
    GlobalUnlock( list_global );

    WRMemValidate( cust_lib, _TRMEM_WHO( 7 ) );

    return( true );
}

bool WdeAddBorControlsToCustLib( WdeCustLib *cust_lib, WdeBorlandClassList *class_list )
{
    uint_16 num;

    for( num = 0; num < class_list->nClasses; num++ ) {
        if( !WdeCreateAndAddCustControl( cust_lib, class_list->Classes[num].info_proc,
                                         class_list->Classes[num].style_proc,
                                         class_list->Classes[num].flags_proc ) ) {
            WdeWriteTrail( "WdeAddBorControlsToCustLib: Control not added to list!" );
            return( false );
        }
    }

    WRMemValidate( cust_lib, _TRMEM_WHO( 8 ) );

    return( true );
}

bool WdeCreateAndAddCustControl( WdeCustLib *cust_lib, WdeCustInfoProc info_proc,
                                 WdeCustStyleProc style_proc, WdeCustFlagsProc flags_proc )
{
    HGLOBAL         info_global;
    uint_8          *info_locked;
    WdeCustControl  *cust_ctrl;

    if( cust_lib == NULL || info_proc == NULL || style_proc == NULL || flags_proc == NULL ) {
        WdeWriteTrail( "WdeCreateAndAddCustControl: NULL param!" );
        return( false );
    }

    info_global = (*info_proc)();

    if( info_global == NULL ) {
        WdeWriteTrail( "WdeCreateAndAddCustControl: Info proc returned NULL!" );
        return( false );
    }

    info_locked = (uint_8 *)GlobalLock( info_global );
    if( info_locked == NULL ) {
        WdeWriteTrail( "WdeCreateAndAddCustControl: GlobalLock failed!" );
        GlobalFree( info_global );
        return( false );
    }

    cust_ctrl = WdeAllocCustControlFromWDECTLINFO( (WDECTLINFO *)info_locked, cust_lib->ms_lib );
    if( cust_ctrl == NULL ) {
        WdeWriteTrail( "WdeCreateAndAddCustControl: Could alloc control!" );
        GlobalUnlock( info_global );
        GlobalFree( info_global );
        return( false );
    }

    cust_ctrl->lib = cust_lib;

    cust_ctrl->info_proc = info_proc;
    cust_ctrl->style_proc = style_proc;
    cust_ctrl->flags_proc = flags_proc;

    WdeAddCustControlToCustLib( cust_lib, cust_ctrl );

    GlobalUnlock( info_global );
    GlobalFree( info_global );

    WRMemValidate( cust_lib, _TRMEM_WHO( 9 ) );

    return( true );
}

void WdeAddCustControlToCustLib( WdeCustLib *cust_lib, WdeCustControl *cust_ctrl )
{
    LIST *end;

    if( cust_lib->controls == NULL ) {
        ListAddElt( &cust_lib->controls, (OBJPTR)cust_ctrl );
    } else {
        /* make sure item is inserted at end of list */
        WdeListLastElt( cust_lib->controls, &end );
        ListInsertElt( end, (OBJPTR)cust_ctrl );
    }

    WRMemValidate( cust_lib, _TRMEM_WHO( 10 ) );
    WRMemValidate( cust_ctrl, _TRMEM_WHO( 11 ) );
}

WdeCustLib *WdeAllocCustLib( void )
{
    WdeCustLib  *cust_lib;

    cust_lib = MemAlloc( sizeof( WdeCustLib ) );

    if( cust_lib == NULL ) {
        WdeWriteTrail( "WdeAllocCustLib: WdeCustLib alloc failed!" );
        return( NULL );
    }

    memset( cust_lib, 0, sizeof( WdeCustLib ) );

    WRMemValidate( cust_lib, _TRMEM_WHO( 12 ) );

    return( cust_lib );
}

WdeCustControl *WdeAllocCustControlFromWDECTLINFO( WDECTLINFO *info, bool ms_lib )
{
    WdeCustControl  *cust_ctrl;

    cust_ctrl = WdeAllocCustControl();
    if( cust_ctrl == NULL ) {
        WdeWriteTrail( "WdeAllocCustControlFromWDECTLINFO: "
                       "WdeCustControl alloc failed!" );
        return( NULL );
    }

    cust_ctrl->ms_lib = ms_lib;

    if( ms_lib ) {
        memcpy( &cust_ctrl->control_info.ms, info, sizeof( CTLINFO ) );
    } else {
        memcpy( &cust_ctrl->control_info.bor, info, sizeof( WDECTLINFO ) );
    }

    return( cust_ctrl );
}

WdeCustControl *WdeAllocCustControl( void )
{
    WdeCustControl  *cust_ctrl;

    cust_ctrl = MemAlloc( sizeof( WdeCustControl ) );

    if( cust_ctrl == NULL ) {
        WdeWriteTrail( "WdeAllocCustControl: WdeCustControl alloc failed!" );
        return( NULL );
    }

    memset( cust_ctrl, 0, sizeof( WdeCustControl ) );

    return( cust_ctrl );
}

void WdeFreeCustRESProcs( void )
{
    if( WdeCustLOADRESInst != NULL ) {
        FreeProcInstance( (FARPROC)WdeCustLOADRESInst );
    }

    if( WdeCustEDITRESInst != NULL ) {
        FreeProcInstance( (FARPROC)WdeCustEDITRESInst );
    }
}

bool WdeFreeAllCustLibs( void )
{
    LIST        *llist;
    WdeCustLib  *cust_lib;

    if( WdeCustomLibList != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            cust_lib = (WdeCustLib *)ListElement( llist );
            WdeFreeCustLib( cust_lib );
        }
        ListFree( WdeCustomLibList );
        WdeCustomLibList = NULL;
    }

    if( WdeLibList != NULL ) {
        for( llist = WdeLibList; llist != NULL; llist = ListNext( llist ) ) {
            cust_lib = (WdeCustLib *)ListElement( llist );
            WdeFreeCustLib( cust_lib );
        }
        ListFree( WdeLibList );
        WdeLibList = NULL;
    }

    return( true );
}

bool WdeFreeCustLib( WdeCustLib *cust_lib )
{
    if( cust_lib != NULL ) {
        if( cust_lib->controls != NULL ) {
            WdeFreeCustLibControls( &cust_lib->controls );
        }
        if( cust_lib->file_name != NULL ) {
            MemFree( cust_lib->file_name );
        }
        if( cust_lib->info_name != NULL && !IS_INTRESOURCE( cust_lib->info_name ) ) {
            MemFree( (void *)cust_lib->info_name );
        }
        if( cust_lib->style_name != NULL && !IS_INTRESOURCE( cust_lib->style_name ) ) {
            MemFree( (void *)cust_lib->style_name );
        }
        if( cust_lib->flags_name != NULL && !IS_INTRESOURCE( cust_lib->flags_name ) ) {
            MemFree( (void *)cust_lib->flags_name );
        }
        if( cust_lib->inst != NULL ) {
            FreeLibrary( cust_lib->inst );
        }
        if( !cust_lib->ms_lib && cust_lib->class_list != NULL ) {
            GlobalFree( cust_lib->class_list );
        }
        MemFree( cust_lib );
    } else {
        WdeWriteTrail( "WdeFreeCustLib: NULL lib!" );
        return( false );
    }

    return( true );
}

void WdeFreeCustLibControls( LIST **control_list )
{
    LIST           *clist;
    WdeCustControl *cust_ctrl;

    if( control_list != NULL && *control_list != NULL ) {
        for( clist = *control_list; clist != NULL; clist = ListNext( clist ) ) {
            cust_ctrl = (WdeCustControl *)ListElement( clist );
            WdeFreeCustControl( cust_ctrl );
        }
        ListFree( *control_list );
        *control_list = NULL;
    } else {
        WdeWriteTrail( "WdeFreeCustLibControls: NULL control_list!" );
    }
}

bool WdeFreeCustControl( WdeCustControl *cust_ctrl )
{
    uint_16 type;
    uint_16 num_types;

    if( cust_ctrl != NULL ) {
        if( !cust_ctrl->ms_lib ) {
            num_types = cust_ctrl->control_info.bor.wCtlTypes;
            for( type = 0; type < num_types; type++ ) {
                if( cust_ctrl->control_info.bor.Type[type].hToolBit ) {
                    DeleteObject( cust_ctrl->control_info.bor.Type[type].hToolBit );
                }
            }
        }
        MemFree( cust_ctrl );
    } else {
        WdeWriteTrail( "WdeFreeCustControl: NULL control!" );
        return( false );
    }

    return( true );
}

void WdeFindClassInAllCustLibs( const char *class, LIST **list )
{
    LIST        *llist;
    WdeCustLib  *cust_lib;

    if( list == NULL ) {
        return;
    }

    *list = NULL;

    if( class != NULL && WdeCustomLibList != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            cust_lib = (WdeCustLib *)ListElement( llist );
            WdeFindClassInCustLib( class, list, cust_lib );
        }
    }
}

void WdeFindClassInCustLib( const char *class, LIST **list, WdeCustLib *cust_lib )
{
    if( cust_lib != NULL && cust_lib->controls != NULL ) {
        WdeFindClassInCustLibControls( class, list, cust_lib->controls );
    }
}

void WdeFindClassInCustLibControls( const char *class, LIST **list, LIST *control_list )
{
    LIST           *clist;
    WdeCustControl *cust_ctrl;

    if( control_list != NULL ) {
        for( clist = control_list; clist != NULL; clist = ListNext( clist ) ) {
            cust_ctrl = (WdeCustControl *)ListElement( clist );
            if( stricmp( class, cust_ctrl->control_info.ms.szClass ) == 0 ) {
                ListAddElt( list, (OBJPTR)cust_ctrl );
            }
        }
    }
}

bool WdeQueryUnsafeMSLoad( void )
{
    int         ret;
    char        *text;
    char        *title;

    text = WdeAllocRCString( WDE_MSWARNINGTEXT );
    title = WdeAllocRCString( WDE_MSWARNINGTITLE );

    ret = MessageBox( WdeGetMainWindowHandle(), text, title,
                      MB_APPLMODAL | MB_OKCANCEL | MB_ICONEXCLAMATION );

    if( title != NULL ) {
        WdeFreeRCString( title );
    }

    if( text != NULL ) {
        WdeFreeRCString( text );
    }

    if( ret != IDOK ) {
        return( false );
    }

    return( true );
}

bool WdeQueryAssumeMS( void )
{
    int         ret;
    char        *text;
    char        *title;

    text = WdeAllocRCString( WDE_BORWARNINGTEXT );
    title = WdeAllocRCString( WDE_BORWARNINGTITLE );

    ret = MessageBox( WdeGetMainWindowHandle(), text, title,
                      MB_APPLMODAL | MB_OKCANCEL | MB_ICONEXCLAMATION );

    if( title != NULL ) {
        WdeFreeRCString( title );
    }

    if( text != NULL ) {
        WdeFreeRCString( text );
    }

    if( ret != IDOK ) {
        return( false );
    }

    return( true );
}

HGLOBAL CALLBACK WdeCustLOADRES( LPCSTR type_name, LPCSTR res_name )
{
    HGLOBAL     res;
    HRSRC       hres;
    HINSTANCE   inst;
    uint_32     res_int;
    char        out[160];
    bool        strange;
    bool        unknown;
    bool        unhandled;

    sprintf( out, "Request to load type:%lu res:%lu",
             (unsigned long)type_name, (unsigned long)res_name );
    WdeWriteTrail( out );

    inst = WdeGetAppInstance();

    strange = false;
    unknown = false;
    unhandled = false;

    hres = (HRSRC)NULL;
    res  = (HGLOBAL)NULL;

    if( IS_INTRESOURCE( type_name ) ) {
        switch( RESOURCE2INT( type_name ) ) {
        case RESOURCE2INT( RT_CURSOR ):
            strange = true;
            res = (HRSRC)LoadCursor( inst, res_name );
            break;
        case RESOURCE2INT( RT_BITMAP ):
            if( !IS_INTRESOURCE( res_name ) || !WdeIsBorBtnIDSupported( RESOURCE2INT( res_name ) % 1000 ) ) {
                res_int = (uint_32)res_name;
                res_name = MAKEINTRESOURCE( res_int - (res_int % 1000) + WDE_PREVIEW_ID );
            }
            hres = FindResource( inst, res_name, type_name );
            if( hres != NULL ) {
                res = LoadResource( inst, hres );
            }
            break;
        case RESOURCE2INT( RT_ICON ):
            strange = true;
            res = (HRSRC)LoadIcon( inst, res_name );
            break;
        case RESOURCE2INT( RT_MENU ):
            strange = true;
            res = (HRSRC)LoadMenu( inst, res_name );
            break;
        case RESOURCE2INT( RT_ACCELERATOR ):
            strange = true;
            res = (HRSRC)LoadAccelerators( inst, res_name );
            break;
        case RESOURCE2INT( RT_STRING ):
        case RESOURCE2INT( RT_DIALOG ):
        case RESOURCE2INT( RT_FONTDIR ):
        case RESOURCE2INT( RT_FONT ):
        case RESOURCE2INT( RT_RCDATA ):
        case RESOURCE2INT( RT_GROUP_CURSOR ):
        case RESOURCE2INT( RT_GROUP_ICON ):
            unhandled = true;
            break;
        default:
            unknown = true;
            break;
        }
    } else {
        unknown = true;
    }

    if( strange ) {
        WdeWriteTrail( "WdeCustLOADRES: Strange LOADRES request!" );
        return( NULL );
    } else if( unknown ) {
        WdeWriteTrail( "WdeCustLOADRES: Unknown LOADRES request!" );
    } else if( unhandled ) {
        WdeWriteTrail( "WdeCustLOADRES: Unhandled LOADRES request!" );
    }
    if( res == NULL ) {
        WdeWriteTrail( "WdeCustLOADRES: res == NULL!" );
    }

    return( res );
}

BOOL CALLBACK WdeCustEDITRES( LPCSTR type_name, LPCSTR res_name )
{
    char      out[160];

    /* unused parameters */ (void)type_name; (void)res_name;

    sprintf( out, "Request to edit type:%lu res:%lu",
             (unsigned long)(uint_32)type_name, (unsigned long)(uint_32)res_name );
    WdeWriteTrail( out );

    return( FALSE );
}

void WdeFreeSelectWinCBox( HWND win )
{
    HWND                cbox;
    int                 count;
    int                 i;
    WdeCurrCustControl  *current;

    cbox = GetDlgItem( win, IDB_CUST_DESC );

    count = (int)SendMessage( cbox, CB_GETCOUNT, 0, 0 );

    for( i = 0; i < count; i++ ) {
        current = (WdeCurrCustControl *)SendMessage( cbox, CB_GETITEMDATA, i, 0 );
        if( current != NULL ) {
            MemFree( current );
            SendMessage( cbox, CB_SETITEMDATA, i, 0 );
        } else {
            WdeWriteTrail( "WdeFreeSelectWinCBox: NULL current!" );
        }
    }
}

bool WdeSetSelectWinCBox( HWND cbox, WdeCustControl *cust_ctrl )
{
    WdeCurrCustControl  *current;
    uint_16             type;
    LRESULT             index;

    for( type = 0; type < cust_ctrl->control_info.ms.wCtlTypes; type++ ) {
        current = MemAlloc( sizeof( WdeCurrCustControl ) );
        if( current == NULL ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: alloc failed!" );
            return( false );
        }
        current->control = cust_ctrl;
        current->type = type;
        if( cust_ctrl->ms_lib ) {
            index = SendMessage( cbox, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)cust_ctrl->control_info.ms.Type[type].szDescr );
        } else {
            index = SendMessage( cbox, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)cust_ctrl->control_info.bor.Type[type].szDescr );
        }
        if( index == CB_ERR || index == CB_ERRSPACE ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: CB_ADDSTRING failed!" );
            MemFree( current );
            return( false );
        }

        if( SendMessage( cbox, CB_SETITEMDATA, (WPARAM)index, (LPARAM)(LPVOID)current ) == CB_ERR ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: CB_SETITEMDATA failed!" );
            MemFree( current );
            return( false );
        }
    }

    return( true );
}

bool WdeSetSelectWin( HWND win )
{
    LIST            *llist;
    LIST            *clist;
    WdeCustLib      *cust_lib;
    WdeCustControl  *cust_ctrl;
    HWND            cbox;

    cbox = GetDlgItem( win, IDB_CUST_DESC );

    if( WdeCustomLibList != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            cust_lib = (WdeCustLib *)ListElement( llist );
            if( cust_lib->controls != NULL ) {
                for( clist = cust_lib->controls; clist != NULL; clist = ListNext( clist ) ) {
                    cust_ctrl = (WdeCustControl *)ListElement( clist );
                    if( !WdeSetSelectWinCBox( cbox, cust_ctrl ) ) {
                        return( false );
                    }
                }
            }
        }
    }

    return( true );
}

bool WdeSetCurrentControl( HWND win, int which )
{
    LRESULT             index;
    WdeCurrCustControl  *current;

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCOUNT, 0, 0L );
    if( index == 0 || index == CB_ERR ) {
        return( true );
    }

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCURSEL, 0, 0L );
    current = (WdeCurrCustControl *)SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETITEMDATA, (WPARAM)index, 0 );
    if( current == NULL ) {
        WdeWriteTrail( "WdeSetCurrentControl: CB_GETITEMDATA failed!" );
        return( false );
    }

    WdeCurrControl[which].control = current->control;
    WdeCurrControl[which].type = current->type;

    return( true );
}

void WdeMapCustomSize( int *w, int *h, WdeResizeRatio *r )
{
    DialogSizeInfo      SizeInfo;
    RECT                trect;

    if( *w >= 0 && *h >= 0 ) {
        SizeInfo.x = 0;
        SizeInfo.y = 0;
        SizeInfo.width = *w;
        SizeInfo.height = *h;
        WdeDialogToScreen( NULL, r, &SizeInfo, &trect );
        *w = trect.right;
        *h = trect.bottom;
    } else if( *w >= 0 ) {
        SizeInfo.x = 0;
        SizeInfo.y = 0;
        SizeInfo.width = *w;
        SizeInfo.height = 0;
        WdeDialogToScreen( NULL, r, &SizeInfo, &trect );
        *w = trect.right;
    } else if( *h >= 0 ) {
        SizeInfo.x = 0;
        SizeInfo.y = 0;
        SizeInfo.width = 0;
        SizeInfo.height = *h;
        WdeDialogToScreen( NULL, r, &SizeInfo, &trect );
        *h = trect.bottom;
    }
    if( *w < 0 ) {
        *w *= -1;
    }
    if( *h < 0 ) {
        *h *= -1;
    }
}

bool WdePreviewSelected( HWND win )
{
    WdeCurrCustControl  *current;
    LPARAM              index;
    char                *class;
    char                *caption;
    HWND                pwin;
    HWND                parea;
    RECT                psize;
    RECT                trect;
    uint_32             pstyle;
    POINT               p;
    int                 pwidth;
    int                 pheight;
    WdeResizeRatio      resizer;

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCOUNT, 0, 0L );
    if( index == 0 || index == CB_ERR ) {
        return( true );
    }

    parea = GetDlgItem( win, IDB_CUST_PREVIEW );

    pwin = GetDlgItem( win, WDE_PREVIEW_ID );
    if( pwin != NULL ) {
        DestroyWindow( pwin );
    }

    GetClientRect( parea, &psize );

    InflateRect( &psize, -WDE_PREVIEW_PAD, -WDE_PREVIEW_PAD );

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCURSEL, 0, 0L );
    current = (WdeCurrCustControl *)SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETITEMDATA, (WPARAM)index, 0 );
    if( current == NULL ) {
        WdeWriteTrail( "WdePreviewSelected: CB_GETITEMDATA failed!" );
        return( false );
    }

    if( current->control->ms_lib ) {
        class = current->control->control_info.ms.szClass;
        caption = current->control->control_info.ms.szTitle;
        pstyle = current->control->control_info.ms.Type[current->type].dwStyle;
        pwidth = current->control->control_info.ms.Type[current->type].wWidth;
        pheight = current->control->control_info.ms.Type[current->type].wHeight;
    } else {
        class = current->control->control_info.bor.szClass;
        caption = current->control->control_info.bor.szTitle;
        pstyle = current->control->control_info.bor.Type[current->type].dwStyle;
        pwidth = current->control->control_info.bor.Type[current->type].wWidth;
        pheight = current->control->control_info.bor.Type[current->type].wHeight;
    }

    WdeGetResizerFromFont( &resizer, "System", 10 );

    WdeMapCustomSize( &pwidth, &pheight, &resizer );

    if( pwidth > psize.right - psize.left ) {
        pwidth = psize.right - psize.left;
    }

    if( pheight > psize.bottom - psize.top ) {
        pheight = psize.bottom - psize.top;
    }

    p.x = 0;
    p.y = 0;

    SendDlgItemMessage( win, IDB_CUST_CLASSNAME, WM_SETTEXT, 0, (LPARAM)(LPCSTR)class );

    pstyle &= ~(WS_POPUP | WS_VISIBLE | WS_TABSTOP);
    pstyle |= WS_CHILD;

    pwin = CreateWindow( class, caption, pstyle, p.x, p.y, pwidth, pheight, win,
                         (HMENU)WDE_PREVIEW_ID, WdeGetAppInstance(), NULL );

    if( pwin == NULL ) {
        WdeWriteTrail( "WdePreviewSelected: CreateWindow failed!" );
        return( false );
    }

    GetClientRect( pwin, &trect );

    pwidth = trect.right;
    pheight = trect.bottom;

    p.x = ((psize.right - psize.left) - pwidth) / 2 + WDE_PREVIEW_PAD;
    p.y = ((psize.bottom - psize.top) - pheight) / 2 + WDE_PREVIEW_PAD;

    MapWindowPoints( parea, win, &p, 1 );

    SetWindowPos( pwin, (HWND)NULL, p.x, p.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

    ShowWindow( pwin, SW_SHOW );

    return( true );
}

INT_PTR CALLBACK WdeSelectCustDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    static int which = 0;
    bool       ret;

    ret = false;

    switch( message ) {
    case WM_SETFONT:
        WdeSelectFont = (HFONT)wParam;
        break;

    case WM_INITDIALOG:
        which = *(int *)lParam;
        if( !WdeSetSelectWin( hDlg ) ) {
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, FALSE );
        }
        SendDlgItemMessage( hDlg, IDB_CUST_DESC, CB_SETCURSEL, 0, 0 );
        WdePreviewSelected( hDlg );
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WinHelp( WdeGetMainWindowHandle(), "resdlg.hlp", HELP_CONTENTS, 0 );
            break;

        case IDOK:
            WdeSetCurrentControl( hDlg, which );
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, TRUE );
            ret = true;
            break;

        case IDCANCEL:
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, FALSE );
            ret = true;
            break;

        case IDB_CUST_DESC:
            switch( GET_WM_COMMAND_CMD( wParam, lParam ) ) {
            case CBN_CLOSEUP:
            case CBN_SELCHANGE:
                WdePreviewSelected( hDlg );
                break;
            }
        }
    }

    return( ret );
}

void WdeSetLoadCustInfo( HWND hDlg, WdeCustLib *cust_lib )
{
    pgroup2     pg;
    char        info[_MAX_FNAME + 4];
    char        style[_MAX_FNAME + 5];
    char        flags[_MAX_FNAME + 5];

    _splitpath2( cust_lib->file_name, pg.buffer, NULL, NULL, &pg.fname, NULL );

    strlwr( pg.fname );
    pg.fname[0] = toupper( pg.fname[0] );

    strcpy( info, pg.fname );
    strcpy( style, pg.fname );
    strcpy( flags, pg.fname );

    strcat( info, "Info" );
    strcat( style, "Style" );
    strcat( flags, "Flags" );

    WdeSetEditWithStr( info,  hDlg, IDB_CUST_INFO );
    WdeSetEditWithStr( style, hDlg, IDB_CUST_STYLE );
    WdeSetEditWithStr( flags, hDlg, IDB_CUST_FLAGS );
}

void WdeSetLoadCustInfoOrd( HWND hDlg )
{
    char str[2];

    str[1] = '\0';

    str[0] = '0' + CCINFOORD;
    WdeSetEditWithStr( str, hDlg, IDB_CUST_INFO );
    str[0] = '0' + CCSTYLEORD;
    WdeSetEditWithStr( str, hDlg, IDB_CUST_STYLE );
    str[0] = '0' + CCFLAGSORD;
    WdeSetEditWithStr( str, hDlg, IDB_CUST_FLAGS );
}

void WdeEnableNonStandardCustInfo( HWND hDlg, bool flag, bool init )
{
    int i;

    for( i = IDB_CUST_INFO; i <= IDB_CUST_GBOX; i++ ) {
        if( init ) {
            SendMessage( GetDlgItem( hDlg, i ), WM_ENABLE, flag, 0 );
        } else {
            EnableWindow( GetDlgItem( hDlg, i ), flag );
        }
    }
}

bool WdeGetLoadCustInfo( HWND hDlg, WdeCustLib *cust_lib )
{
    char    *info;
    char    *style;
    char    *flags;
    bool    ok;

    ok = true;

    if( !IsDlgButtonChecked( hDlg, IDB_CUST_NONSTANDARD ) ) {
        cust_lib->info_name = MAKEINTRESOURCE( CCINFOORD );
        cust_lib->style_name = MAKEINTRESOURCE( CCSTYLEORD );
        cust_lib->flags_name = MAKEINTRESOURCE( CCFLAGSORD );
    } else if( IsDlgButtonChecked( hDlg, IDB_CUST_ORD ) ) {
        cust_lib->info_name = (char *)WdeGetUINT32FromEdit( hDlg, IDB_CUST_INFO, NULL );
        cust_lib->style_name = (char *)WdeGetUINT32FromEdit( hDlg, IDB_CUST_STYLE, NULL );
        cust_lib->flags_name = (char *)WdeGetUINT32FromEdit( hDlg, IDB_CUST_FLAGS, NULL );
        if( cust_lib->info_name == NULL || cust_lib->style_name == NULL || cust_lib->flags_name == NULL ) {
            WdeDisplayErrorMsg( WDE_BADPROCEDUREORDINAL );
            ok = false;
        }
    } else {
        info = WdeGetStrFromEdit( hDlg, IDB_CUST_INFO, NULL );
        style = WdeGetStrFromEdit( hDlg, IDB_CUST_STYLE, NULL );
        flags = WdeGetStrFromEdit( hDlg, IDB_CUST_FLAGS, NULL );
        if( GetProcAddress( cust_lib->inst, info ) == NULL ) {
            WdeDisplayErrorMsg( WDE_CANTFINDINFOPROC );
            ok = false;
        }
        if( ok && GetProcAddress( cust_lib->inst, style ) == NULL ) {
            WdeDisplayErrorMsg( WDE_CANTFINDSTYLEPROC );
            ok = false;
        }
        if( ok && GetProcAddress( cust_lib->inst, flags ) == NULL ) {
            WdeDisplayErrorMsg( WDE_CANTFINDFLAGSPROC );
            ok = false;
        }
        if( !ok ) {
            MemFree( info );
            MemFree( style );
            MemFree( flags );
        } else {
            cust_lib->info_name = info;
            cust_lib->style_name = style;
            cust_lib->flags_name = flags;
        }
    }

    return( ok );
}

INT_PTR CALLBACK WdeLoadCustDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    static WdeCustLib   *cust_lib;
    bool                checked;
    bool                ret;

    ret = false;

    switch( message ) {
    case WM_INITDIALOG:
        cust_lib = (WdeCustLib *)lParam;
        WdeSetLoadCustInfo( hDlg, cust_lib );
        WdeEnableNonStandardCustInfo( hDlg, false, true );
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WinHelp( WdeGetMainWindowHandle(), "resdlg.hlp", HELP_CONTENTS, 0 );
            break;

        case IDOK:
            if( WdeGetLoadCustInfo( hDlg, cust_lib ) ) {
                EndDialog( hDlg, TRUE );
                ret = true;
            }
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = true;
            break;

        case IDB_CUST_NONSTANDARD:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
                checked = ( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) != 0 );
                if( !checked ) {
                    WdeSetLoadCustInfo( hDlg, cust_lib );
                }
                WdeEnableNonStandardCustInfo( hDlg, checked, false );
            }
            break;

        case IDB_CUST_ORD:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
                checked = ( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) != 0 );
                if( checked ) {
                    WdeSetLoadCustInfoOrd( hDlg );
                }
            }
            break;
        }
    }

    return( ret );
}
