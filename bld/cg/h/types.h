/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Prototypes for types.c
*
****************************************************************************/


extern const type_def   TNearCP;
extern const type_def   TLongCP;
extern const type_def   THugeCP;
extern const type_def   TNearP;
extern const type_def   TLongP;
extern const type_def   THugeP;

extern const type_def   *PTInteger;
extern const type_def   *PTUnsigned;
extern const type_def   *PTPointer;
extern const type_def   *PTCodePointer;

extern void             TypeInit( void );
extern const type_def   *TypeAddress( cg_type tipe );
extern type_length      TypeLength( cg_type tipe );
extern const type_def   *TypeAlias( cg_type define, cg_type existing );
extern const type_def   *TypeDef( cg_type refno, type_length length, type_length align );
extern void             TypeFini( void );
extern void             TargTypeInit( void );
