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


/* used in defining imbedded values */
/* numerics */
_LFNUM( numeric,     NUMERIC    , 0x8000 )
_LFNUM( char,        CHAR       , 0x8000 )
_LFNUM( short,       SHORT      , 0x8001 )
_LFNUM( ushort,      USHORT     , 0x8002 )
_LFNUM( long,        LONG       , 0x8003 )
_LFNUM( ulong,       ULONG      , 0x8004 )
_LFNUM( real32,      REAL32     , 0x8005 )
_LFNUM( real64,      REAL64     , 0x8006 )
_LFNUM( real80,      REAL80     , 0x8007 )
_LFNUM( real128,     REAL128    , 0x8008 )
_LFNUM( quadword,    QUADWORD   , 0x8009 )
_LFNUM( uquadword,   UQUADWORD  , 0x800a )
_LFNUM( real48,      REAL48     , 0x800b )
_LFNUM( complex32,   COMPLEX32  , 0x800c )
_LFNUM( complex64,   COMPLEX64  , 0x800d )
_LFNUM( complex80,   COMPLEX80  , 0x800e )
_LFNUM( complex128,  COMPLEX128 , 0x800f )
_LFNUM( varstring,   VARSTRING  , 0x8010 )
