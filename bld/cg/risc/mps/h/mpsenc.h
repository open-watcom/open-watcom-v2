/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS instruction encoding.
*
****************************************************************************/


#include "mipsenco.h"
#include "mpsrega.h"
#include "mpsregn.h"


extern void         GenCallLabelReg( pointer label, reg_idx reg );
extern void         GenLOADS32( int_32 value, reg_idx reg );
extern void         GenMEMINS( uint_8 opcode, reg_idx rt, reg_idx rs, int_16 displacement );
extern void         GenIType( uint_8 opcode, reg_idx rt, reg_idx rs, int_16 immed );
extern void         GenRType( uint_8 opcode, uint_8 fc, reg_idx rd, reg_idx rs, reg_idx rt );
extern void         GenIShift( uint_8 fc, reg_idx rd, reg_idx rt, uint_8 sa );
extern void         GenJType( uint_8 opcode, pointer label );
extern void         GenReturn( void );
