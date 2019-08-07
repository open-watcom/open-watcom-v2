/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Debugger internal variables definition data.
*
****************************************************************************/


/*    name,     tk,            tm,            tt         */
pick( radix,    TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( monitor,  TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( top,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( bottom,   TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( psp,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( pid,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( remote,   TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( code,     TK_ADDRESS,    TM_FAR,        address )
pick( data,     TK_ADDRESS,    TM_FAR,        address )
pick( machine,  TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( cpu,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( fpu,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( os,       TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( 32,       TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( left,     TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( right,    TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( etid,     TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( ctid,     TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( ntid,     TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( ip,       TK_ADDRESS,    TM_FAR,        address )
pick( sp,       TK_ADDRESS,    TM_FAR,        address )
pick( bp,       TK_ADDRESS,    TM_FAR,        address )
pick( loaded,   TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( WV_TNG,   TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( NIL,      TK_ADDRESS,    TM_FAR,        address )
pick( src,      TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
pick( kanji,    TK_INTEGER,    TM_UNSIGNED,   unsigned_16 )
