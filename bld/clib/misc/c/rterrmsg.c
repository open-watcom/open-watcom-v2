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
* Description:  Output a runtime-error message.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __WINDOWS__ ) || defined( __WINDOWS_386__ ) || defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
  #if defined( _M_I86 )
    #include <wos2.h>
  #elif defined( __386__ )
    #define INCL_DOSPROCESS
    #define INCL_WINDIALOGS
    #define INCL_DOSERRORS
    #define INCL_DOSMODULEMGR
    #define INCL_ORDINALS
    #include <wos2.h>
    #ifdef DEFAULT_WINDOWING
        extern unsigned (*_WindowsStdout)();
    #endif
  #endif
#endif
#include "rtdata.h"
#include "rterrmsg.h"
#include "errorno.h"
#include "thread.h"

#define STR_SIZE        256

/* Output error message 'msg' with header text 'hdr'. The message will
 * be printed on stderr, or, if appropriate in a windowed environment,
 * in a standard GUI error message box.
 *
 * If printed on console, the text will be printed in format
 *     <hdr>": "<msg>
 */
void __F_NAME(__rterr_msg,__wrterr_msg)( const CHAR_TYPE *hdr, const CHAR_TYPE *msg )
{
#if defined( __WINDOWS__ ) || defined( __WINDOWS_386__ )
    #ifdef __WIDECHAR__
        char    outhdr[ MB_CUR_MAX * STR_SIZE ];
        char    outmsg[ MB_CUR_MAX * STR_SIZE ];

        wcstombs( outhdr, hdr, sizeof( outhdr ) );
        wcstombs( outmsg, msg, sizeof( outmsg ) );
    #else
        const char  *outhdr = hdr;
        const char  *outmsg = msg;
    #endif
    MessageBox( (HWND)NULL, outmsg, outhdr, MB_OK | MB_TASKMODAL );
#elif defined( __NT__ )
    int     rc;

    rc = __F_NAME(fputs,fputws)( hdr, stderr );
    rc = __F_NAME(fputs,fputws)( STRING( ": " ), stderr );
    rc = __F_NAME(fputs,fputws)( msg, stderr );
    if( (rc == EOF) && (_RWD_errno == EBADF) ) {
        MessageBox( NULL, msg, hdr, MB_OK | MB_TASKMODAL );
    }
#elif defined( __OS2__ ) && defined( __386__ )
    TIB     *ptib;
    PIB     *ppib;
    ULONG   (APIENTRY *pfnWinMessageBox)( HWND, HWND, PCSZ, PCSZ, ULONG, ULONG );
    HMODULE hmodPMWIN;

    DosGetInfoBlocks( &ptib, &ppib );
    if( (ppib->pib_ultype == PT_PM) &&
    #ifdef DEFAULT_WINDOWING
        (_WindowsStdout == 0) &&
    #endif
        (DosLoadModule( NULL, 0, "PMWIN", &hmodPMWIN ) == NO_ERROR) &&
        (DosQueryProcAddr( hmodPMWIN, ORD_WIN32MESSAGEBOX, NULL, (PFN*)&pfnWinMessageBox ) == NO_ERROR)
    ) {
    #ifdef __WIDECHAR__
        char    outhdr[ MB_CUR_MAX * STR_SIZE ];
        char    outmsg[ MB_CUR_MAX * STR_SIZE ];

        wcstombs( outhdr, hdr, sizeof( outhdr ) );
        wcstombs( outmsg, msg, sizeof( outmsg ) );
    #else
        const char  *outhdr = hdr;
        const char  *outmsg = msg;
    #endif
        pfnWinMessageBox( HWND_DESKTOP, NULLHANDLE, outmsg, outhdr, 0,
                          MB_SYSTEMMODAL | MB_OK );
        DosFreeModule( hmodPMWIN );
    } else {
        __F_NAME(fputs,fputws)( hdr, stderr );
        __F_NAME(fputs,fputws)( STRING( ": " ), stderr );
        __F_NAME(fputs,fputws)( msg, stderr );
    }
#else
    __F_NAME(fputs,fputws)( hdr, stderr );
    __F_NAME(fputs,fputws)( STRING( ": " ), stderr );
    __F_NAME(fputs,fputws)( msg, stderr );
#endif
}
