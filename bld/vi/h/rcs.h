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


#ifdef __WIN__
    #include <windows.h>
#endif

#if defined( __NT__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
#if defined( __AXP__ )
    #define RCSAPI      __export __stdcall
#else
    #pragma aux __fortran "*" parm [] modify [ eax ecx edx ];
    #define RCSAPI      __export __fortran
#endif
#elif defined( __WINDOWS__ )
    typedef const char far *rcsstring;
    typedef void far *rcsdata;
    #define RCSAPI      __export far pascal
#elif defined( __OS2__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
    #define RCSAPI      __export _syscall
#elif defined( __DOS__ ) || defined( __QNX__ )
    #define RCSAPI // typedefs in api.h
#else
    #error Unsupported OS
    #define RCSAPI
#endif

#if defined( __NT__ ) || defined( __WINDOWS__ ) || defined( __OS2__ )

typedef int RCSAPI ( *BatchCallbackFP )( rcsstring str, void *cookie );
typedef int RCSAPI ( *MessageBoxCallbackFP )( rcsstring text, rcsstring title, char *buffer, int len, void *cookie );

typedef int RCSAPI(*RCSGetVerFn)( void );
typedef rcsdata RCSAPI (*RCSInitFn)( unsigned long, char *cfg_dir );
typedef int RCSAPI (*RCSCheckoutFn)( rcsdata, rcsstring, rcsstring, rcsstring );
typedef int RCSAPI (*RCSCheckinFn)( rcsdata, rcsstring, rcsstring, rcsstring );
typedef int RCSAPI (*RCSHasShellFn)( rcsdata );
typedef int RCSAPI (*RCSRunShellFn)( rcsdata );
typedef int RCSAPI (*RCSSetSystemFn)( rcsdata, int );
typedef int RCSAPI (*RCSQuerySystemFn)( rcsdata );
typedef int RCSAPI (*RCSRegBatchCbFn)( rcsdata, BatchCallbackFP, void * );
typedef int RCSAPI (*RCSRegMsgBoxCbFn)( rcsdata, MessageBoxCallbackFP, void * );
typedef void RCSAPI (*RCSSetPauseFn)( rcsdata, int );
typedef void RCSAPI (*RCSFiniFn)( rcsdata );

extern RCSGetVerFn              RCSGetVersion;
extern RCSInitFn                RCSInit;
extern RCSCheckoutFn            RCSCheckout;
extern RCSCheckinFn             RCSCheckin;
extern RCSHasShellFn            RCSHasShell;
extern RCSRunShellFn            RCSRunShell;
extern RCSSetSystemFn           RCSSetSystem;
extern RCSQuerySystemFn         RCSQuerySystem;
extern RCSRegBatchCbFn          RCSRegisterBatchCallback;
extern RCSRegMsgBoxCbFn         RCSRegisterMessageBoxCallback;
extern RCSSetPauseFn            RCSSetPause;
extern RCSFiniFn                RCSFini;
#endif

#ifdef __WINDOWS__
 #define RCS_DLLNAME                    "rcsdll.dll"
 #define CHECKIN_FN_NAME                "RCSCHECKIN"
 #define CHECKOUT_FN_NAME               "RCSCHECKOUT"
 #define GETSYS_FN_NAME                 "RCSQUERYSYSTEM"
 #define GETVER_FN_NAME                 "RCSGETVERSION"
 #define FINI_FN_NAME                   "RCSFINI"
 #define HAS_SHELL_FN_NAME              "RCSHASSHELL"
 #define INIT_FN_NAME                   "RCSINIT"
 #define REG_BAT_CB_FN_NAME             "RCSREGISTERBATCHCALLBACK"
 #define REG_MSGBOX_CB_FN_NAME          "RCSREGISTERMESSAGEBOXCALLBACK"
 #define RUNSHELL_FN_NAME               "RCSRUNSHELL"
 #define SETSYS_FN_NAME                 "RCSSETSYSTEM"
 #define SET_PAUSE_FN_NAME              "RCSSetPause"
#elif defined( __NT__ )
 #define RCS_DLLNAME                    "rcsdll.dll"
 #define CHECKIN_FN_NAME                "_RCSCheckin@16"
 #define CHECKOUT_FN_NAME               "_RCSCheckout@16"
 #define GETSYS_FN_NAME                 "_RCSQuerySystem@4"
 #define GETVER_FN_NAME                 "_RCSGetVersion@0"
 #define FINI_FN_NAME                   "_RCSFini@4"
 #define HAS_SHELL_FN_NAME              "_RCSHasShell@4"
 #define INIT_FN_NAME                   "_RCSInit@8"
 #define REG_BAT_CB_FN_NAME             "_RCSRegisterBatchCallback@12"
 #define REG_MSGBOX_CB_FN_NAME          "_RCSRegisterMessageBoxCallback@12"
 #define RUNSHELL_FN_NAME               "_RCSRunShell@4"
 #define SETSYS_FN_NAME                 "_RCSSetSystem@8"
 #define SET_PAUSE_FN_NAME              "_RCSSetPause@8"
#elif defined( __OS2__ )
 #define RCS_DLLNAME                    "rcsdll"
 #define CHECKIN_FN_NAME                "RCSCheckin"
 #define CHECKOUT_FN_NAME               "RCSCheckout"
 #define GETSYS_FN_NAME                 "RCSQuerySystem"
 #define GETVER_FN_NAME                 "RCSGetVersion"
 #define FINI_FN_NAME                   "RCSFini"
 #define HAS_SHELL_FN_NAME              "RCSHasShell"
 #define INIT_FN_NAME                   "RCSInit"
 #define REG_BAT_CB_FN_NAME             "RCSRegisterBatchCallback"
 #define REG_MSGBOX_CB_FN_NAME          "RCSRegisterMessageBoxCallback"
 #define RUNSHELL_FN_NAME               "RCSRunShell"
 #define SETSYS_FN_NAME                 "RCSSetSystem"
 #define SET_PAUSE_FN_NAME              "RCSSetPause"
#endif

extern int ViRCSInit(); // just stubs for dos
extern int ViRCSFini();
