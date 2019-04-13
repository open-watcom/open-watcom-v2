/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  16-bit Windows Event Hook related prototypes
*                SetEventHook is undocumented entry USER.321
*
****************************************************************************/


/*
 * The handler installed by SetEventHook uses non-standard calling convention.
 * Arguments are passed in ax and cx, and setting carry flag before exit
 * may cause the message to be discarded. Also, the routine has to set ds
 * to the proper value (ie. no multiple instances - but it may not be possible
 * to register multiple event hooks anyway). See Undocumented Windows.
 */

typedef void __far __loadds event_hook_fn( unsigned, unsigned );
#pragma aux event_hook_fn __parm [__ax] [__cx]

extern void __far __pascal SetEventHook( event_hook_fn * );
