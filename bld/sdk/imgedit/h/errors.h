/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "rcstr.grh"

                                   //1234567890123456789012345678901234567890
#define WIE_INTERNAL_001            "Internal Error 001- No active image."
#define WIE_INTERNAL_002            "Internal Error 002- Node not deleted."
#define WIE_INTERNAL_003            "Internal Error 003- Bad fill case."
#define WIE_INTERNAL_004            "Internal Error 004- Icon not deleted."
#define WIE_INTERNAL_005            "Internal Error 005- Icon not selected."
#define WIE_INTERNAL_006            "Internal Error 006- Node not found."
#define WIE_INTERNAL_007            "Internal Error 007- Icon out of range."
#define WIE_INTERNAL_008            "Internal Error 008- Icon count imbalance"

extern void     IEPrintErrorMsg( const char *title, const char *msg, const char *fname, UINT style );
extern void     IEPrintErrorMsgByID( msg_id title, msg_id msg, const char *fname, UINT style );
extern void     IEInitDisplayError( HINSTANCE inst );
extern void     IEDisplayErrorMsg( msg_id title, msg_id msg, UINT style );
extern char     *IEAllocRCString( msg_id id );
extern void     IEFreeRCString( char *str );
extern int      IECopyRCString( msg_id id, char *buf, int bufsize );
extern void     IEFiniErrors( void );
extern bool     IEInitErrors( HINSTANCE inst );
