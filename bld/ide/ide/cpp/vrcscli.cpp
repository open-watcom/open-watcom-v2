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


#include "vpemain.hpp"
#include "vrcscli.hpp"
#include "wsystem.hpp"
#include "wstrobjs.hpp"
#include "wautodlg.hpp"
#include "system.hpp"

// This is an abortion.  It lets us get a Windows HWND from a gui_window *.
// It assumes that a gui_window * points to a structure with the HWND as
// its first element.  If the definition of a gui_window changes this
// will break.  Other projects also do this.  Including guiwind.h to
// get the proper definition of a gui_window is difficult because it would
// require that we also include headers from several other places.
#define GET_HWND( x ) (*((HWND *)(x)))

extern "C" {
int RCSAPI GetInputCB( rcsstring text, rcsstring title,
                                        char *buffer, int len,
                                        void *cookie )
{
    WStringList         prompts;
    WStringList         input;
    WAutoDialog         *dlg;
    WString             *item;

    prompts.add( new WString( text ) );
    dlg = new WAutoDialog( (VpeMain *) cookie, title, prompts, 60 );
    if( dlg->getInput( input ) ) {
        item = (WString *)input[0];
        item->truncate( len );
        strcpy( buffer, item->gets() );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

int RCSAPI DoBatchCB( rcsstring str, void *cookie ) {
    VpeMain     *parent;
    WString     cmd( str );

    parent = (VpeMain *) cookie;
    parent->executeCommand( str, EXECUTE_BATCH, "" );
    return( 1 );
}

}// extern "C"

VRcsClient::VRcsClient( WWindow *parent ) {
    _parent = parent;
    _initialized = false;
#ifdef __WINDOWS__
    _batchcb = NULL;
    _msgcb = NULL;
#endif
}

VRcsClient::~VRcsClient() {

    if( _initialized ) {
        _fini( _dllcookie );
        WSystemService::freeLibrary( _dllhdl );
    }
#ifdef __WINDOWS__
    if( _msgcb != NULL ) FreeProcInstance( _msgcb );
    _msgcb = NULL;
    if( _batchcb != NULL ) FreeProcInstance( _batchcb );
    _batchcb = NULL;
#endif
    _initialized = false;
}

bool VRcsClient::Init( void ) {

    int         rc;

    _dllhdl = WSystemService::loadLibrary( RCS_DLLNAME );
    if( _dllhdl == 0 ) return( false );

    _checkin = (RCSCheckinFn *)
        WSystemService::getProcAddr( _dllhdl, CHECKIN_FN_NAME );
    if( _checkin == NULL ) return( false );

    _checkout = (RCSCheckoutFn *)
        WSystemService::getProcAddr( _dllhdl, CHECKOUT_FN_NAME );
    if( _checkout == NULL ) return( false );

    _fini = (RCSFiniFn *)WSystemService::getProcAddr( _dllhdl, FINI_FN_NAME );
    if( _fini == NULL ) return( false );

    _getver = (RCSGetVersionFn *)
        WSystemService::getProcAddr( _dllhdl, GETVER_FN_NAME );
    if( _getver == NULL ) return( false );

    _hasshell = (RCSHasShellFn *)
        WSystemService::getProcAddr( _dllhdl, HAS_SHELL_FN_NAME );
    if( _hasshell == NULL ) return( false );

    _querysystem = (RCSQuerySystemFn *)
        WSystemService::getProcAddr( _dllhdl, GETSYS_FN_NAME );
    if( _querysystem == NULL ) return( false );

    _init = (RCSInitFn *)WSystemService::getProcAddr( _dllhdl, INIT_FN_NAME );
    if( _init == NULL ) return( false );

    _regbatchcb = (RCSRegBatchCbFn *)
        WSystemService::getProcAddr( _dllhdl, REG_BAT_CB_FN_NAME );
    if( _regbatchcb == NULL ) return( false );

    _regmsgboxcb = (RCSRegMsgBoxCbFn *)
        WSystemService::getProcAddr( _dllhdl, REG_MSGBOX_CB_FN_NAME );
    if( _regmsgboxcb== NULL ) return( false );

    _runshell = (RCSRunShellFn *)
        WSystemService::getProcAddr( _dllhdl, RUNSHELL_FN_NAME );
    if( _runshell == NULL ) return( false );

    _setsystem = (RCSSetSystemFn *)
        WSystemService::getProcAddr( _dllhdl, SETSYS_FN_NAME );
    if( _setsystem == NULL ) return( false );

    if( _getver() != RCS_DLL_VER ) return( false );

    // getting the HWND like this violates GUI
    _dllcookie = _init( (unsigned long)GET_HWND( _parent->handle() ),
                         getenv( "WATCOM" ) );
    if( _dllcookie == NULL ) return( false );

#ifdef __WINDOWS__
    _batchcb = MakeProcInstance( (FARPROC)DoBatchCB, GUIMainHInst );
    if( _batchcb == NULL ) {
        rc = 0;
    } else {
        rc = _regbatchcb( _dllcookie, (BatchCallback *)_batchcb,
                                (void *)_parent );
    }
#else
    rc = _regbatchcb( _dllcookie, DoBatchCB, (void *)_parent );
#endif
    if( !rc ) {
        _fini( _dllcookie );
        return( false );
    }

#ifdef __WINDOWS__
    _msgcb = MakeProcInstance( (FARPROC)GetInputCB, GUIMainHInst );
    if( _msgcb == NULL ) {
        rc = 0;
    } else {
        rc = _regmsgboxcb( _dllcookie, (MessageBoxCallback *)_msgcb,
                            (void *)_parent );
    }
#else
    rc = _regmsgboxcb( _dllcookie, GetInputCB, (void *)_parent );
#endif
    if( !rc ) {
        _fini( _dllcookie );
        return( false );
    }

    _initialized = true;
    return( true );
}

int VRcsClient::Checkout( WFileName *fname, WString &proj, WString &targ ) {
    int         rc;
    if( _initialized ) {
        rc = _checkout( _dllcookie, fname->gets(), proj.gets(), targ.gets() );
        return( rc );
    } else {
        return( 0 );
    }
}

int VRcsClient::Checkin( WFileName *fname, WString &proj, WString &targ ) {
    int         rc;
    if( _initialized ) {
        rc = _checkin( _dllcookie, fname->gets(), proj.gets(), targ.gets() );
        return(  rc );
    } else {
        return( 0 );
    }
}

int VRcsClient::RunShell( void ) {
    if( _initialized ) {
        return( _runshell( _dllcookie ) );
    } else {
        return( 0 );
    }
}

int VRcsClient::SetSystem( int systok ) {
    if( _initialized ) {
        return( _setsystem( _dllcookie, systok ) );
    } else {
        return( 0 );
    }
}

int VRcsClient::QuerySystem( void ) {
    if( _initialized ) {
        return( _querysystem( _dllcookie ) );
    } else {
        return( NO_RCS );
    }
}

int VRcsClient::HasShell( void ) {
    if( _initialized ) {
        return( _hasshell( _dllcookie ) );
    } else {
        return( 0 );
    }
}

