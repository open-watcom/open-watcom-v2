/****************************************************************************
*
*            Open Watcom Project
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
* Description:  intrinsic function processing
*
****************************************************************************/


extern void     GMakeCplx( void );
extern void     GMakeDCplx( void );
extern void     GMakeXCplx( void );
extern void     GModulus( void );
extern void     GSign( void );
extern void     GCharLen( void );
extern void     GImag( void );
extern void     GConjg( void );
extern void     GDProd( void );
extern void     GXProd( void );
extern void     GMin( TYPE func_type );
extern void     GMax( TYPE func_type );
extern void     GBitTest( void );
extern void     GBitSet( void );
extern void     GBitClear( void );
extern void     GBitOr( void );
extern void     GBitAnd( void );
extern void     GBitNot( void );
extern void     GBitExclOr( void );
extern void     GBitChange( void );
extern void     GBitLShift( void );
extern void     GBitRShift( void );
extern void     GMod( void );
extern void     GAbs( void );
extern void     GASin( void );
extern void     GACos( void );
extern void     GATan( void );
extern void     GATan2( void );
extern void     GLog( void );
extern void     GLog10( void );
extern void     GCos( void );
extern void     GSin( void );
extern void     GTan( void );
extern void     GSinh( void );
extern void     GCosh( void );
extern void     GTanh( void );
extern void     GSqrt( void );
extern void     GExp( void );
extern void     GLoc( void );
extern void     GVolatile( void );
