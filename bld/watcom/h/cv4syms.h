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


/* various things */
SLMAC( compile,        COMPILE,      0x0001 )
SLMAC( register,       REGISTER,     0x0002 )
SLMAC( constant,       CONSTANT,     0x0003 )
SLMAC( udt,            UDT,          0x0004 )
SLMAC( ssearch,        SSEARCH,      0x0005 )
SLMAC( end,            END,          0x0006 )
SLMAC( skip,           SKIP,         0x0007 )
SLMAC( cvreserve,      CVRESERVE,    0x0008 )
SLMAC( objname,        OBJNAME,      0x0009 )
SLMAC( endarg,         ENDARG,       0x000a )
SLMAC( coboludt,       COBOLUDT,     0x000b )
SLMAC( manyreg,        MANYREG,      0x000c )
SLMAC( return,         RETURN,       0x000d )
SLMAC( entrythis,      ENTRYTHIS,    0x000e )
 /* 16:16 Segmented Architectures */
SLMAC( bprel16,        BPREL16,      0x0100 )
SLMAC( ldata16,        LDATA16,      0x0101 )
SLMAC( gdata16,        GDATA16,      0x0102 )
SLMAC( pub16,          PUB16,        0x0103 )
SLMAC( lproc16,        LPROC16,      0x0104 )
SLMAC( gproc16,        GPROC16,      0x0105 )
SLMAC( thunk16,        THUNK16,      0x0106 )
SLMAC( block16,        BLOCK16,      0x0107 )
SLMAC( with16,         WITH16,       0x0108 )
SLMAC( label16,        LABEL16,      0x0109 )
SLMAC( cexmodel16,     CEXMODEL16,   0x010a )
SLMAC( vftpath16,      VFTPATH16,    0x010b )
SLMAC( regrel16,       REGREL16,     0x010c )
/* 16:32 Segmented Architectures */
SLMAC( bprel32,        BPREL32,      0x0200 )
SLMAC( ldata32,        LDATA32,      0x0201 )
SLMAC( gdata32,        GDATA32,      0x0202 )
SLMAC( pub32,          PUB32,        0x0203 )
SLMAC( lproc32,        LPROC32,      0x0204 )
SLMAC( gproc32,        GPROC32,      0x0205 )
SLMAC( thunk32,        THUNK32,      0x0206 )
SLMAC( block32,        BLOCK32,      0x0207 )
SLMAC( with32,         WITH32,       0x0208 )
SLMAC( label32,        LABEL32,      0x0209 )
SLMAC( cexmodel32,     CEXMODEL32,   0x020a )
SLMAC( vftpath32,      VFTPATH32,    0x020b )
SLMAC( regrel32,       REGREL32,     0x020c )
SLMAC( lthread32,      LTHREAD32,    0x020d )
SLMAC( gthread32,      GTHREAD32,    0x020e )
/* CV Pack Optimizations */
SLMAC( procref,        PROCREF,      0x0400 )
SLMAC( dataref,        DATAREF,      0x0401 )
SLMAC( align,          ALIGN,        0x0402 )
