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


#ifndef RCS_CLIENT_DEFINED
#define RCS_CLIENT_DEFINED

#include "rcscli.h"
#include "wfilenam.hpp"
#ifdef __WINDOWS__
#include "system.hpp"
#endif

WCLASS VRcsClient : public WObject {
    public:
        VRcsClient( WWindow * );
        ~VRcsClient();
        bool Init( void );
        int Checkout( WFileName *, WString &, WString & );
        int Checkin( WFileName *, WString &, WString & );
        int RunShell( void );
        int HasShell( void );
        int SetSystem( int );
        int QuerySystem( void );
    private:
        bool            _initialized;
        WModuleHandle   _dllhdl;
        rcsdata         _dllcookie;
        WWindow         *_parent;
#ifdef __WINDOWS__
        FARPROC         _batchcb;
        FARPROC         _msgcb;
#endif
        // function pointers
        RCSGetVersionFn         *_getver;
        RCSInitFn               *_init;
        RCSCheckoutFn           *_checkout;
        RCSCheckinFn            *_checkin;
        RCSHasShellFn           *_hasshell;
        RCSRunShellFn           *_runshell;
        RCSSetSystemFn          *_setsystem;
        RCSQuerySystemFn        *_querysystem;
        RCSRegBatchCbFn         *_regbatchcb;
        RCSRegMsgBoxCbFn        *_regmsgboxcb;
        RCSFiniFn               *_fini;
};

#endif
