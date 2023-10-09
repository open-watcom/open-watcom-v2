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
* Description:  Intel instruction encoding, part II. Processes labels,
*               jumps and the like.
*
****************************************************************************/


extern void     DoCall( label_handle lbl, bool imported, bool big, bool pop );
extern void     GenCall( instruction *ins );
extern void     GenCallIndirect( instruction *ins );
extern void     GenCallRegister( instruction *ins );
extern void     GenSelEntry( bool starts );
extern void     Gen1ByteValue( byte value );
extern void     Gen2ByteValue( uint_16 value );
extern void     Gen4ByteValue( uint_32 value );
extern void     GenCodePtr( pointer label );
extern void     GenJmpMemory( instruction *ins );
extern void     GenJmpRegister( instruction *ins );
extern void     EyeCatchBytes( const void *src, byte_seq_len len );
extern void     GenReturn( int pop, bool is_long );
extern void     GenIRET( void );
extern void     CodeBytes( const void *src, byte_seq_len len );
