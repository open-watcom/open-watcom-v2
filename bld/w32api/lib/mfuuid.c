/***************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
*  Description: Implementation of the mfuuid.lib library.
*
**************************************************************************/

#include <windows.h>

/* evntrace.h */
EXTERN_C const GUID EventTraceGuid =
    { 0x68FDD900, 0x4A3E, 0x11D1, { 0x84, 0xF4, 0x00, 0x00, 0xF8, 0x04, 0x64, 0xE3 } };
EXTERN_C const GUID SystemTraceControlGuid =
    { 0x9E814AAD, 0x3204, 0x11D2, { 0x9A, 0x82, 0x00, 0x60, 0x08, 0xA8, 0x69, 0x39 } };
EXTERN_C const GUID EventTraceConfigGuid =
    { 0x01853A65, 0x418F, 0x4F36, { 0xAE, 0xFC, 0xDC, 0x0F, 0x1D, 0x2F, 0xD2, 0x35 } };
EXTERN_C const GUID DefaultTraceSecurityGuid =
    { 0x0811C1AF, 0x7A07, 0x4A06, { 0x82, 0xED, 0x86, 0x94, 0x55, 0xCD, 0xF7, 0x13 } };
