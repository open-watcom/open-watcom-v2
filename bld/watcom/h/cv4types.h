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


_LFMAC( modifier,    MODIFIER   , 0x0001 )
_LFMAC( pointer,     POINTER    , 0x0002 )
_LFMAC( array,       ARRAY      , 0x0003 )
_LFMAC( class,       CLASS      , 0x0004 )
_LFMAC( structure,   STRUCTURE  , 0x0005 )
_LFMAC( union,       UNION      , 0x0006 )
_LFMAC( enum,        ENUM       , 0x0007 )
_LFMAC( procedure,   PROCEDURE  , 0x0008 )
_LFMAC( mfunction,   MFUNCTION  , 0x0009 )
_LFMAC( vtshape,     VTSHAPE    , 0x000a )
_LFMAC( cobol0,      COBOL0     , 0x000b )
_LFMAC( cobol1,      COBOL1     , 0x000c )
_LFMAC( barray,      BARRAY     , 0x000d )
_LFMAC( label,       LABEL      , 0x000e )
_LFMAC( null,        NULL       , 0x000f )
_LFMAC( nottrans,    NOTTRANS   , 0x0010 )
_LFMAC( dimarray,    DIMARRAY   , 0x0011 )
_LFMAC( vftpath,     VFTPATH    , 0x0012 )
_LFMAC( precomp,     PRECOMP    , 0x0013 )
_LFMAC( endprecomp,  ENDPRECOMP , 0x0014 )
_LFMAC( oem,         OEM        , 0x0015 )
_LFMAC( skip,        SKIP       , 0x0200 )
_LFMAC( arglist,     ARGLIST    , 0x0201 )
_LFMAC( defarg,      DEFARG     , 0x0202 )
_LFMAC( list,        LIST       , 0x0203 )
_LFMAC( fieldlist,   FIELDLIST  , 0x0204 )
_LFMAC( derived,     DERIVED    , 0x0205 )
_LFMAC( bitfield,    BITFIELD   , 0x0206 )
_LFMAC( mlist,       MLIST      , 0x0207 )
_LFMAC( dimconu,     DIMCONU    , 0x0208 )
_LFMAC( dimconlu,    DIMCONLU   , 0x0209 )
_LFMAC( dimvaru,     DIMVARU    , 0x020a )
_LFMAC( dimvarlu,    DIMVARLU   , 0x020b )
_LFMAC( refsym,      REFSYM     , 0x020c )
_LFMAC( bclass,      BCLASS     , 0x0400 )
_LFMAC( vbclass,     VBCLASS    , 0x0401 )
_LFMAC( ivbclass,    IVBCLASS   , 0x0402 )
_LFMAC( enumerate,   ENUMERATE  , 0x0403 )
_LFMAC( friendfcn,   FRIENDFCN  , 0x0404 )
_LFMAC( index,       INDEX      , 0x0405 )
_LFMAC( member,      MEMBER     , 0x0406 )
_LFMAC( stmember,    STMEMBER   , 0x0407 )
_LFMAC( method,      METHOD     , 0x0408 )
_LFMAC( nestedtype,  NESTEDTYPE , 0x0409 )
_LFMAC( vfunctab,    VFUNCTAB   , 0x040a )
_LFMAC( friendcls,   FRIENDCLS  , 0x040b )
_LFMAC( onemethod,   ONEMETHOD  , 0x040c )
_LFMAC( vfuncoff,    VFUNCOFF   , 0x040d )
#if 0
/* not really used in defining fixed part of a type */
/* numerics */
_LFMAC( numeric,     NUMERIC    , 0x8000 )
_LFMAC( char,        CHAR       , 0x8000 )
_LFMAC( short,       SHORT      , 0x8001 )
_LFMAC( ushort,      USHORT     , 0x8002 )
_LFMAC( long,        LONG       , 0x8003 )
_LFMAC( ulong,       ULONG      , 0x8004 )
_LFMAC( real32,      REAL32     , 0x8005 )
_LFMAC( real64,      REAL64     , 0x8006 )
_LFMAC( real80,      REAL80     , 0x8007 )
_LFMAC( real128,     REAL128    , 0x8008 )
_LFMAC( quadword,    QUADWORD   , 0x8009 )
_LFMAC( uquadword,   UQUADWORD  , 0x800a )
_LFMAC( real48,      REAL48     , 0x800b )
/* padding */
_LFMAC( pad0,        PAD0       , 0x00f0 )
_LFMAC( pad1,        PAD1       , 0x00f1 )
_LFMAC( pad2,        PAD2       , 0x00f2 )
_LFMAC( pad3,        PAD3       , 0x00f3 )
_LFMAC( pad4,        PAD4       , 0x00f4 )
_LFMAC( pad5,        PAD5       , 0x00f5 )
_LFMAC( pad6,        PAD6       , 0x00f6 )
_LFMAC( pad7,        PAD7       , 0x00f7 )
_LFMAC( pad8,        PAD8       , 0x00f8 )
_LFMAC( pad9,        PAD9       , 0x00f9 )
_LFMAC( pad10,       PAD10      , 0x00fa )
_LFMAC( pad11,       PAD11      , 0x00fb )
_LFMAC( pad12,       PAD12      , 0x00fc )
_LFMAC( pad13,       PAD13      , 0x00fd )
_LFMAC( pad14,       PAD14      , 0x00fe )
_LFMAC( pad15,       PAD15      , 0x00ff )
#endif
