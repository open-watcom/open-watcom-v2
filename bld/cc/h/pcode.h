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


/*
;
;       P-Code Interpreter
;
;       Layout of stack frame upon entry to a function
;
;       ParmN
;       .
;       .
;       .
;       Parm 2                  8[BP]
;       Parm 1                  6[BP]
;       CS of ret address
;       IP of ret address
;       CS from far call, saved BP <------ BP points here
;       IP from far call, PARM_AX
;                         PARM_BX
;                         PARM_CX
;                         PARM_DX
;       saved ES
;       saved DS
;       saved DI
;       saved SI
;       saved CX
;       saved BX
;       saved DX
;       icall_seg
;       icall_off
;       locals                  10[DI][BP]
;       locals                  8[DI][BP]
;       locals          CX      6[DI][BP]
;       locals          BX      4[DI][BP]
;       locals          DX      2[DI][BP]
;       locals          AX      0[DI][BP]
;
*/
#if defined(_DEBUG_)
 #define pcode(p,n)  #p
 static char *P_Codes[] = {
#elif defined(_PARMS_)
 #define pcode(p,n) n
 static unsigned char P_Parms[] = {
#else
 #define pcode(p,n) p
 enum pcodes {
#endif

  pcode(PushIP6,0)      // push integer parm at offset 6 from BP
, pcode(PushIP8,0)      // ...                         8
, pcode(PushIP10,0)     // ...                         10
, pcode(PushIP12,0)     // ...                         12
, pcode(PushIP14,0)     // ...                         14
, pcode(PushIP16,0)     // ...                         16
, pcode(PushIP18,0)     // ...                         18
, pcode(PushIP20,0)     // ...                         20
, pcode(PushIPn,1)      // ...                         n
, pcode(PushLP6,0)      // push long parm at offset 6 from BP
, pcode(PushLP8,0)      // ...                      8
, pcode(PushLP10,0)     // ...                      10
, pcode(PushLP12,0)     // ...                      12
, pcode(PushLP14,0)     // ...                      14
, pcode(PushLP16,0)     // ...                      16
, pcode(PushLP18,0)     // ...                      18
, pcode(PushLP20,0)     // ...                      20
, pcode(PushLPn,1)      // ...                      n
, pcode(PushIA0,0)      // push integer auto at offset 0 from [BP][DI]
, pcode(PushIA2,0)      // ...                         2
, pcode(PushIA4,0)      // ...                         4
, pcode(PushIA6,0)      // ...                         6
, pcode(PushIA8,0)      // ...                         8
, pcode(PushIA10,0)     // ...                         10
, pcode(PushIA12,0)     // ...                         12
, pcode(PushIA14,0)     // ...                         14
, pcode(PushIA16,0)     // ...                         16
, pcode(PushIA18,0)     // ...                         18
, pcode(PushIA20,0)     // ...                         20
, pcode(PushIAn,1)      // ...                         n
, pcode(PushIAnn,2)     // ...                 2 byte offset nn
, pcode(PushLA0,0)      // push long auto at offset 0 from [BP][DI]
, pcode(PushLA2,0)      // ...                      2
, pcode(PushLA4,0)      // ...                      4
, pcode(PushLA6,0)      // ...                      6
, pcode(PushLA8,0)      // ...                      8
, pcode(PushLA10,0)     // ...                      10
, pcode(PushLA12,0)     // ...                      12
, pcode(PushLA14,0)     // ...                      14
, pcode(PushLA16,0)     // ...                      16
, pcode(PushLA18,0)     // ...                      18
, pcode(PushLA20,0)     // ...                      20
, pcode(PushLAn,1)      // ...                      n
, pcode(PopIA0,0)       // pop integer auto at offset 0 from [BP][DI]
, pcode(PopIA2,0)       // ...                        2
, pcode(PopIA4,0)       // ...                        4
, pcode(PopIA6,0)       // ...                        6
, pcode(PopIA8,0)       // ...                        8
, pcode(PopIA10,0)      // ...                        10
, pcode(PopIA12,0)      // ...                        12
, pcode(PopIA14,0)      // ...                        14
, pcode(PopIA16,0)      // ...                        16
, pcode(PopIA18,0)      // ...                        18
, pcode(PopIA20,0)      // ...                        20
, pcode(PopIAn,1)       // ...                        n
, pcode(PopIAnn,2)      // ...                 2 byte offset nn
, pcode(PopLA0,0)       // pop long auto at offset 0 from [BP][DI]
, pcode(PopLA2,0)       // ...                     2
, pcode(PopLA4,0)       // ...                     4
, pcode(PopLA6,0)       // ...                     6
, pcode(PopLA8,0)       // ...                     8
, pcode(PopLA10,0)      // ...                     10
, pcode(PopLA12,0)      // ...                     12
, pcode(PopLA14,0)      // ...                     14
, pcode(PopLA16,0)      // ...                     16
, pcode(PopLA18,0)      // ...                     18
, pcode(PopLA20,0)      // ...                     20
, pcode(PopLAn,1)       // ...                     n
, pcode(PopIP6,0)       // pop integer parm at offset 6 from [BP]
, pcode(PopIP8,0)       // ...                        8
, pcode(PopIP10,0)      // ...                        10
, pcode(PopIP12,0)      // ...                        12
, pcode(PopIP14,0)      // ...                        14
, pcode(PopIP16,0)      // ...                        16
, pcode(PopIP18,0)      // ...                        18
, pcode(PopIP20,0)      // ...                        20
, pcode(PopIPn,1)       // ...                        n
, pcode(PopLP6,0)       // pop long parm at offset 6 from [BP]
, pcode(PopLP8,0)       // ...                     8
, pcode(PopLP10,0)      // ...                     10
, pcode(PopLP12,0)      // ...                     12
, pcode(PopLP14,0)      // ...                     14
, pcode(PopLP16,0)      // ...                     16
, pcode(PopLP18,0)      // ...                     18
, pcode(PopLP20,0)      // ...                     20
, pcode(PopLPn,1)       // ...                     n
, pcode(IPushChar,0)    // push signed char pointed at by addr on stack
, pcode(IPushUChar,0)   // push unsigned char pointed at by addr on stack
, pcode(IPushInt,0)     // push integer pointed at by addr on stack
, pcode(IPushLong,0)    // push long pointed at by addr on stack
, pcode(IPushDouble,0)  // push double pointed at by addr on stack
, pcode(IPushStruct,2)  // push struct
, pcode(IPushBitField,2)// push value of bit field
, pcode(Push0Const,0)   // push integer 0
, pcode(Push0LConst,0)  // push long integer 0
, pcode(Push1Const,0)   // push integer 1
, pcode(Push1LConst,0)  // push long integer 1
, pcode(PushBConst,1)   // push one byte constant
, pcode(PushWConst,2)   // push two byte constant
, pcode(PushLConst,4)   // push four byte constant
, pcode(PushParmAddr,1) // push address of parm
, pcode(PushTempAddr,1) // push address of local variable
, pcode(PushGblAddr,4)  // push address of global variable
, pcode(DupAddr,0)      // duplicate address on stack
, pcode(IPopChar,0)     // pop char into addr on stack
, pcode(IPopInt,0)      // pop integer into addr on stack
, pcode(IPopLong,0)     // pop long into addr on stack
, pcode(IPopDouble,0)   // pop double into addr on stack
, pcode(ICopyStruct,2)  // copy struct
, pcode(IPopBitField,2) // pop value into bit field
, pcode(AddBOffset,1)   // add byte offset to address
, pcode(AddWOffset,2)   // add word offset to address
, pcode(PtrSubtract,0)  // subtract two pointers
, pcode(HugePtrAdd,0)   // add long to huge pointer
, pcode(HugePtrSub,0)   // subtract two huge pointers
, pcode(SignExtend,0)   // sign extend integer to a long
, pcode(ZeroExtend,0)   // zero extend integer to an unsigned long
, pcode(Long2Int,0)     // convert long to integer
, pcode(AddInt,0)       // add 2 integers
, pcode(SubInt,0)       // subtract
, pcode(MulInt,0)       // signed multiply
, pcode(MulUInt,0)      // unsigned multiply
, pcode(DivInt,0)       // signed divide
, pcode(DivUInt,0)      // unsigned divide
, pcode(ModInt,0)       // signed modulus
, pcode(ModUInt,0)      // unsigned modulus
, pcode(OrInt,0)        // or     |
, pcode(AndInt,0)       // and    &
, pcode(XorInt,0)       // xor    ^
, pcode(NegInt,0)       // negate -
, pcode(NotInt,0)       // not    !
, pcode(ComInt,0)       // complement ~
, pcode(IShiftL1,0)     // shift integer left by 1
, pcode(IShiftLn,0)     // shift integer left by n
, pcode(IShiftRn,0)     // shift integer right by n
, pcode(IAShiftRn,0)    // shift integer arithmetic right by n
, pcode(AddLong,0)      // add 2 longs
, pcode(SubLong,0)      // subtract 2 longs
, pcode(MulLong,0)      // multiply 2 longs
, pcode(DivLong,0)      // divide 2 longs
, pcode(ModLong,0)      // modulus of 2 longs
, pcode(DivULong,0)     // divide 2 unsigned longs
, pcode(ModULong,0)     // modulus of 2 unsigned longs
, pcode(OrLong,0)       // or     |
, pcode(AndLong,0)      // and    &
, pcode(XorLong,0)      // xor    ^
, pcode(NegLong,0)      // negate -
, pcode(NotLong,0)      // not    !
, pcode(ComLong,0)      // complement ~
, pcode(LShiftL1,0)     // shift long left by 1
, pcode(LShiftLn,0)     // shift long left by n
, pcode(LShiftRn,0)     // shift long right by n
, pcode(LAShiftRn,0)    // shift long arithmetic right by n
, pcode(CmpInt,0)       // compare integers
, pcode(CmpInt0,0)      // compare with 0
, pcode(CmpLong,0)      // compare longs
, pcode(CmpNULL,0)      // compare pointer with NULL
, pcode(jmp_short,1)    // unconditional jump (1 byte offset)
, pcode(jmp_long,2)     // unconditional jump (2 byte offset)
, pcode(VoidFunc,4)     // void function call
, pcode(IntFunc,4)      // int function call
, pcode(LongFunc,4)     // long function call
, pcode(VoidHFunc,1)    // void handle function call
, pcode(IntHFunc,1)     // int handle function call
, pcode(LongHFunc,1)    // long handle function call
, pcode(RetVoid0,0)     // return void
, pcode(RetVoid2,0)     // return void (pop 2 bytes)
, pcode(RetVoid4,0)     // return void (pop 4 bytes)
, pcode(RetVoid6,0)     // return void (pop 6 bytes)
, pcode(RetVoid8,0)     // return void (pop 8 bytes)
, pcode(RetVoidn,3)     // return void followed by "ret far n" ins.
, pcode(RetInt0,0)      // return integer
, pcode(RetInt2,0)      // return integer (pop 2 bytes)
, pcode(RetInt4,0)      // return integer (pop 4 bytes)
, pcode(RetInt6,0)      // return integer (pop 6 bytes)
, pcode(RetInt8,0)      // return integer (pop 8 bytes)
, pcode(RetIntn,3)      // return int followed by "ret far n" ins.
, pcode(RetLong0,0)     // return long
, pcode(RetLong2,0)     // return long (pop 2 bytes)
, pcode(RetLong4,0)     // return long (pop 4 bytes)
, pcode(RetLong6,0)     // return long (pop 6 bytes)
, pcode(RetLong8,0)     // return long (pop 8 bytes)
, pcode(RetLongn,3)     // return long followed by "ret far n" ins.
, pcode(DiscardParm,1)  // discard n words from stack
, pcode(ByteIndex,0)    // char array[index]
, pcode(IntIndex,0)     // short array[index]
, pcode(LongIndex,0)    // long array[index]
, pcode(Cast2Char,0)    // cast value to signed char
, pcode(Cast2UChar,0)   // cast value to unsigned char
, pcode(BSwitch,2)      // switch table with byte constants
, pcode(ISwitch,2)      // switch table with integer constants
, pcode(LSwitch,2)      // switch table with long constants
, pcode(XPushChar,1)    // push signed char at offset from addr on stack
, pcode(XPushUChar,1)   // push unsigned char at offset from addr on stk
, pcode(XPushInt,1)     // push integer at offset from addr on stack
, pcode(XPushLong,1)    // push long at offset from addr on stack
, pcode(XPushDouble,1)  // push double at offset from addr on stack
, pcode(XPopChar,1)     // pop char at offset from addr on stack
, pcode(XPopInt,1)      // pop integer at offset from addr on stack
, pcode(XPopLong,1)     // pop long at offset from addr on stack
, pcode(XPopDouble,1)   // pop double at offset from addr on stack
, pcode(Pop_AX,0)       // pop parm into AX
, pcode(Pop_AX_DX,0)    // pop parms into AX and DX
, pcode(Pop_AX_DX_BX,0) // pop parms into AX and DX and BX
, pcode(Pop_AX_DX_BX_CX,0)// pop parms into AX and DX and BX and CX
, pcode(Pop_AX_BX_CX,0)   // pop parms into AX and BX and CX
, pcode(Pop_AX_BX_CX_DX,0)// pop parms into AX and BX and CX and DX
, pcode(Pop_DX_CX_BX_AX,0)// pop parms into DX and CX and BX and AX
, pcode(PushNearAddr,2) // push DGROUP + offset given in instruction
, pcode(XPushNearPtr,1) // push DGROUP + offset indirect from stack
, pcode(VoidIFunc,0)    // void indirect function call
, pcode(IntIFunc,0)     // int indirect function call
, pcode(LongIFunc,0)    // long indirect function call
, pcode(PopICall,0)     // pop indirect call address from stack
, pcode(PopINCall,0)    // pop indirect near call address from stack
, pcode(LoadCodeAddr,2) // load near addr of func for indirect call
, pcode(PushRetAddr,0)  // push far return address to pcode interpreter
, pcode(VoidINFunc,1)   // void indirect near function call
, pcode(IntINFunc,1)    // int indirect near function call
, pcode(LongINFunc,1)   // long indirect near function call
, pcode(FloatINFunc,1)  // float indirect near function call
, pcode(DoubleINFunc,1) // double indirect near function call
, pcode(StructINFunc,1) // struct indirect near function call
, pcode(TwoByteOp,1)    // prefix for 2 byte opcode
, pcode(DoubleFunc,4)   // double function call
, pcode(DoubleHFunc,1)  // double handle function call
, pcode(AddDouble,0)    // add double
, pcode(SubDouble,0)    // subtract
, pcode(MulDouble,0)    // multiply
, pcode(DivDouble,0)    // divide
, pcode(NegDouble,0)    // negate
, pcode(CmpDouble,0)    // compare
, pcode(AddFloat,0)     // add float
, pcode(SubFloat,0)     // subtract
, pcode(MulFloat,0)     // multiply
, pcode(DivFloat,0)     // divide
, pcode(NegFloat,0)     // negate
, pcode(CmpFloat,0)     // compare
};
#if defined(_DEBUG_)
 static char *PJmp_Codes[] = {
#elif defined(_PARMS_)
 static unsigned char PJmp_Parms[] = {
#else
 enum pjmpcodes {
#endif
  pcode(hop_a,1)        // hop if above
, pcode(hop_ae,1)       // hop if above or equal
, pcode(hop_b,1)        // hop if below
, pcode(hop_be,1)       // hop if below or equal
, pcode(hop_e,1)        // hop if equal
, pcode(hop_ne,1)       // hop if not equal
, pcode(hop_g,1)        // hop if greater than
, pcode(hop_ge,1)       // hop if greater than or equal
, pcode(hop_l,1)        // hop if less than
, pcode(hop_le,1)       // hop if less than or equal
, pcode(jmp_a,2)        // jump if above
, pcode(jmp_ae,2)       // jump if above or equal
, pcode(jmp_b,2)        // jump if below
, pcode(jmp_be,2)       // jump if below or equal
, pcode(jmp_e,2)        // jump if equal
, pcode(jmp_ne,2)       // jump if not equal
, pcode(jmp_g,2)        // jump if greater than
, pcode(jmp_ge,2)       // jump if greater than or equal
, pcode(jmp_l,2)        // jump if less than
, pcode(jmp_le,2)       // jump if less than or equal
};
#if defined(_DEBUG_)
 static char *P2_Codes[] = {
#elif defined(_PARMS_)
 static unsigned char P2_Parms[] = {
#else
 enum p2codes {
#endif
  pcode(StructFunc,4)     // struct function call
, pcode(StructIFunc,0)    // struct indirect function call
, pcode(RetStructn,3)     // struct return
, pcode(PostIncChar,0)    // c++
, pcode(PostIncUChar,0)   // uc++
, pcode(PostIncInt,0)     // i++
, pcode(PostIncLong,0)    // l++
, pcode(PostIncPtr2Char,0)// pc++
, pcode(PostIncPtr2Int,0) // pi++
, pcode(PostIncPtr2Long,0)// pl++
, pcode(PostIncPtr2Struct,2)// ps++
, pcode(PostDecChar,0)    // c--
, pcode(PostDecUChar,0)   // uc--
, pcode(PostDecInt,0)     // i--
, pcode(PostDecLong,0)    // l--
, pcode(PostDecPtr2Char,0)// pc--
, pcode(PostDecPtr2Int,0) // pi--
, pcode(PostDecPtr2Long,0)// pl--
, pcode(PostDecPtr2Struct,2)// ps--
, pcode(RetFloatn,3)      // return float
, pcode(RetDoublen,3)     // return double
, pcode(FloatFunc,4)      // float function call
, pcode(FloatIFunc,0)     // float indirect function call
, pcode(DoubleIFunc,0)    // double indirect function call
, pcode(PostIncFloat,0)   // f++
, pcode(PostIncDouble,0)  // d++
, pcode(PostDecFloat,0)   // f--
, pcode(PostDecDouble,0)  // d--
, pcode(Float2Int,0)      // convert float to int
, pcode(Double2Int,0)     // convert double to int
, pcode(Float2Long,0)     // convert float to long
, pcode(Double2Long,0)    // convert double to long
, pcode(Int2Float,0)      // convert int to float
, pcode(Long2Float,0)     // convert long to float
, pcode(Double2Float,0)   // convert double to float
, pcode(Int2Double,0)     // convert int to double
, pcode(Long2Double,0)    // convert long to double
, pcode(ULong2Double,0)   // convert unsigned long to double
, pcode(Float2Double,0)   // convert float to double
};
