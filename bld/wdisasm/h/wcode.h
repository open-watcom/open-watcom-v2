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


#ifdef DO_ENUM
#define replace2( n, x ) n
#else
#undef  replace2
#define replace2( n, x ) #x
#endif

#ifdef  DO_ENUM
    enum {
#else
    char *  CommonWcode[] = {
#endif
 replace2( WFADD,    wfadd )
,replace2( WFLD,     wfld )
,replace2( WFMUL,    wfmul )
,replace2( WFST,     wfst )
,replace2( WFSUBR,   wfsubr )
,replace2( WFDIVR,   wfdivr )
,replace2( WFMULN,   wfmuln )
,replace2( WFLOAT,   wfloat )
,replace2( WFCMPRT,  wfcmprt )
,replace2( WFTSTT,   wftstt )
,replace2( WFNEG,    wfneg )
,replace2( WFABS,    wfabs )
,replace2( WFCMPR,   wfcmpr )
,replace2( WFTST,    wftst )
,replace2( WFAMUL,   wfamul )
,replace2( WFIX,     wfix )
};


#ifndef  DO_ENUM

#define replace( x ) #x

    char *  SingleWcode[] = {
     replace( wfcvt )
    ,replace( wfcvt )
    ,replace( wfmac )
    ,replace( wfsqrt )
    ,replace( wfmacdd )
    ,replace( wfsub )
    ,replace( wfpush )
    ,replace( wfpop )
    };
    #define WFCVT   0x10
/* there 's a second WFCVT == 0x11 */
    #define WFMAC   0x12
    #define WFSQRT  0x13
    #define WFMACDD 0x14
    #define WFSUB   0x15
    #define WFPUSH  0x16
    #define WFPOP   0x17

    char *  DoubleWcode[] = {
     replace( wfldctx )
    ,replace( wfstctx )
    ,replace( wfmacd )
    ,replace( wfsqrt )
    ,replace( wfldd )     /* shares the same code number as WFSTD */
    ,replace( wfsub )
    };

    #define WFLDCTX 0x10
    #define WFSTCTX 0x11
    #define WFMACD  0x12
/* WFSQRT same as above */
    #define WFLDD  0x14
/* WFSUB same as above */

#endif
