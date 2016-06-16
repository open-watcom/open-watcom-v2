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


/*    name,     tk,         tm,         tt         */
pick( radix,    INTEGER,    UNSIGNED,   unsigned_16 )
pick( monitor,  INTEGER,    UNSIGNED,   unsigned_16 )
pick( top,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( bottom,   INTEGER,    UNSIGNED,   unsigned_16 )
pick( psp,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( pid,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( remote,   INTEGER,    UNSIGNED,   unsigned_16 )
pick( code,     ADDRESS,    FAR,        address )
pick( data,     ADDRESS,    FAR,        address )
pick( machine,  INTEGER,    UNSIGNED,   unsigned_16 )
pick( cpu,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( fpu,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( os,       INTEGER,    UNSIGNED,   unsigned_16 )
pick( 32,       INTEGER,    UNSIGNED,   unsigned_16 )
pick( left,     INTEGER,    UNSIGNED,   unsigned_16 )
pick( right,    INTEGER,    UNSIGNED,   unsigned_16 )
pick( etid,     INTEGER,    UNSIGNED,   unsigned_16 )
pick( ctid,     INTEGER,    UNSIGNED,   unsigned_16 )
pick( ntid,     INTEGER,    UNSIGNED,   unsigned_16 )
pick( ip,       ADDRESS,    FAR,        address )
pick( sp,       ADDRESS,    FAR,        address )
pick( bp,       ADDRESS,    FAR,        address )
pick( loaded,   INTEGER,    UNSIGNED,   unsigned_16 )
pick( WV_TNG,   INTEGER,    UNSIGNED,   unsigned_16 )
pick( NIL,      ADDRESS,    FAR,        address )
pick( src,      INTEGER,    UNSIGNED,   unsigned_16 )
pick( kanji,    INTEGER,    UNSIGNED,   unsigned_16 )
