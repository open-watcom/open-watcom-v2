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


#include "rcstr.gh"

                                   //1234567890123456789012345678901234567890
#define WIE_INTERNAL_001            "Internal Error 001- No active image."
#define WIE_INTERNAL_002            "Internal Error 002- Node not deleted."
#define WIE_INTERNAL_003            "Internal Error 003- Bad fill case."
#define WIE_INTERNAL_004            "Internal Error 004- Icon not deleted."
#define WIE_INTERNAL_005            "Internal Error 005- Icon not selected."
#define WIE_INTERNAL_006            "Internal Error 006- Node not found."
#define WIE_INTERNAL_007            "Internal Error 007- Icon out of range."
#define WIE_INTERNAL_008            "Internal Error 008- Icon count imbalance"

extern void     IEPrintErrorMsg( char *title, char *msg, char *fname, UINT style );
extern void     IEPrintErrorMsgByID( DWORD title, DWORD msg, char *fname, UINT style );
extern void     IEInitDisplayError( HINSTANCE inst );
extern void     IEDisplayErrorMsg( DWORD title, DWORD msg, UINT style );
extern char     *IEAllocRCString( DWORD id );
extern void     IEFreeRCString( char *str );
extern DWORD    IECopyRCString( DWORD id, char *buf, DWORD bufsize );
extern void     IEFiniErrors( void );
extern BOOL     IEInitErrors( HINSTANCE inst );
