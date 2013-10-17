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


static struct STRUCT_cfloat( 4 )     MaxU8   = {
                        3,              /* exponent ten */
                        3,              /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '2','5','5',0 }
                        };


static struct STRUCT_cfloat( 4 )     MaxI8   = {
                        3,              /* exponent ten */
                        3,              /* mantissa length ten */
                        0,              /* allocation length */
                        -1,             /* negative */
                        { '1','2','8',0 }
                        };

static struct STRUCT_cfloat( 6 )     MaxNegI16  = {
                        5,              /* exponent ten */
                        5,              /* mantissa length ten */
                        0,              /* allocation length */
                        -1,             /* negative */
                        { '3','2','7','6','8',0 }
                        };

static struct STRUCT_cfloat( 6 )     MaxU16  = {
                        5,              /* exponent ten */
                        5,              /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '6','5','5','3','5',0 }
                        };

#define I16DIGITS       5


static struct STRUCT_cfloat( 11 )     MaxNegI32  = {
                        10,             /* exponent ten */
                        10,             /* mantissa length ten */
                        0,              /* allocation length */
                        -1,             /* negative */
                        { '2','1','4','7','4','8','3','6','4','8',0 }
                        };


static struct STRUCT_cfloat( 11 )     MaxU32  = {
                        10,             /* exponent ten */
                        10,             /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '4','2','9','4','9','6','7','2','9','5',0 }
                        };

#define I32DIGITS       10

#if 0
static struct STRUCT_cfloat( 4 )      MaxFS = {
                        39,             /* exponent ten */
                        3,              /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '3','3','7',0 }
                        };
#endif

static struct STRUCT_cfloat( 21 )     MaxNegI64  = {
                        20,             /* exponent ten */
                        20,             /* mantissa length ten */
                        0,              /* allocation length */
                        -1,             /* negative */
                        { '9','2','2','3','3','7','2','9','3','6','8','5','4','7','7','5','8','0','7',0 }
                        };

static struct STRUCT_cfloat( 21 )     MaxU64  = {
                        20,             /* exponent ten */
                        20,             /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '1','8','4','4','6','7','4','4','0','7','3','7','0','9','5','5','1','6','1','5',0 }
                        };

/* this is MaxU32 + 1 - just don't want to calc it at runtime */
static struct STRUCT_cfloat( 11 )     High64Mult  = {
                        10,             /* exponent ten */
                        10,             /* mantissa length ten */
                        0,              /* allocation length */
                        1,              /* positive */
                        { '4','2','9','4','9','6','7','2','9','6',0 }
                        };

#define I64DIGITS       20
