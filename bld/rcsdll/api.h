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


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __NT__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
    #define RCSAPI      __export __stdcall
#elif defined( __WINDOWS__ )
    typedef const char far *rcsstring;
    typedef void far *rcsdata;
    #define RCSAPI      __export far pascal
#elif defined( __OS2__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
    #define RCSAPI      __export _syscall
#elif defined( __DOS__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
    #define RCSAPI
#elif defined( __QNX__ )
    typedef const char *rcsstring;
    typedef void *rcsdata;
    #define RCSAPI
#else
    #error Unsupported OS
    #define RCSAPI
#endif

typedef int RCSAPI ( *BatchCallbackFP )( rcsstring str, void *cookie );
typedef int RCSAPI ( *MessageBoxCallbackFP )( rcsstring text, rcsstring title, char *buffer, int len, void *cookie );


// this file should be identical for all app implementations

/* common functions */
int RCSAPI RCSGetVersion();
int RCSAPI RCSSetSystem( rcsdata, int );
int RCSAPI RCSQuerySystem( rcsdata );
int RCSAPI RCSRegisterBatchCallback( rcsdata, BatchCallbackFP, void * );
int RCSAPI RCSRegisterMessageBoxCallback( rcsdata, MessageBoxCallbackFP, void* );
/* system specific functions -- mapped to function for appropriate system */
rcsdata RCSAPI RCSInit( unsigned long window, char *cfg_dir );
int RCSAPI RCSCheckout( rcsdata, rcsstring name, rcsstring pj, rcsstring tgt );
int RCSAPI RCSCheckin( rcsdata, rcsstring name, rcsstring pj, rcsstring tgt );
int RCSAPI RCSHasShell( rcsdata );
int RCSAPI RCSRunShell( rcsdata );
void RCSAPI RCSFini( rcsdata );
void RCSAPI RCSSetPause( rcsdata, int );

/* parms to RCSSetSystem, retvals from RCSQuerySystem */
#define NO_RCS  0
#define MKS_RCS 1
#define MKS_SI  2
#define PVCS    3
#define GENERIC 4
#define O_CYCLE 5

#define RCS_DLL_VER             1
#ifdef __cplusplus
};
#endif
