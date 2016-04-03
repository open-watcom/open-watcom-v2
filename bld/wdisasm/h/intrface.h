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


typedef int_16              GetDataByte_func( void );
typedef int_16              GetDataWord_func( void );
typedef int_16              GetNextByte_func( void );
typedef int_32              GetDataLong_func( void );
typedef bool                EndOfSegment_func( void );
typedef uint_32             GetOffset_func( void );
typedef void                DoWtk_func( void );
typedef int                 IsWtk_func( void );
typedef char                *ToStr_func( uint_32 value, uint_16 len, uint_32 addr );
typedef char                *JmpLabel_func( uint_32 addr, uint_32 off );
typedef char                *ToBrStr_func( uint_32 value, uint_32 addr );
typedef char                *ToIndex_func( uint_32 value, uint_32 addr );
typedef char                *ToSegStr_func( uint_32 value, WORD seg, uint_32 addr );
typedef char                *GetWtkInsName_func( unsigned ins );

extern GetDataByte_func     GetDataByte;
extern GetDataWord_func     GetDataWord;
extern GetNextByte_func     GetNextByte;
extern GetDataLong_func     GetDataLong;
extern EndOfSegment_func    EndOfSegment;
extern GetOffset_func       GetOffset;
extern ToStr_func           ToStr;
extern JmpLabel_func        JmpLabel;
extern ToBrStr_func         ToBrStr;
extern ToIndex_func         ToIndex;
extern ToSegStr_func        ToSegStr;
extern GetWtkInsName_func   GetWtkInsName;
extern DoWtk_func           DoWtk;
extern IsWtk_func           IsWtk;

extern void  DoCode( instruction *curr, bool use_32 );
extern void  FormatIns( char *buf, instruction *curr_ins, form_option format );
