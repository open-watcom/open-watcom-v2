/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


extern type_def     TNearCP;
extern type_def     TLongCP;
extern type_def     THugeCP;
extern type_def     TNearP;
extern type_def     TLongP;
extern type_def     THugeP;

extern type_def     *PTInteger;
extern type_def     *PTUnsigned;
extern type_def     *PTPointer;
extern type_def     *PTCodePointer;

extern void         TypeInit( void );
extern type_def     *TypeAddress( cg_type tipe );
extern type_length  TypeLength( cg_type tipe );
extern type_def     *TypeAlias( cg_type define, cg_type existing );
extern type_def     *TypeDef( cg_type refno, type_length length, type_length align );
extern void         TypeFini( void );
extern void         TargTypeInit( void );
