/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  in-line assembly instructions bytes (encoding)
*
****************************************************************************/


#ifndef _ASMENC_H_INCLUDED
#define _ASMENC_H_INCLUDED

/*
 * machine code instructions
 */

/*
 * 16-bit operand prefix (for 386)
 */
#if defined( _M_I86 )
    #define _USE16
#else
    #define _USE16      0x66
#endif

/*
 * General registers encoding
 *
 *
 * 000     ax
 * 001     cx
 * 010     dx
 * 011     bx
 * 100     sp
 * 101     bp
 * 110     si
 * 111     di
 */

#define _PUSH_CS        0x0e
#define _PUSH_DS        0x1e
#define _POP_DS         0x1f
#define _PUSH_ES        0x06
#define _POP_ES         0x07
#define _PUSH_SS        0x16
#define _POP_SS         0x17
#define _PUSH_FS        0x0f 0xa0
#define _POP_FS         0x0f 0xa1
#define _PUSH_GS        0x0F 0xa8
#define _POP_GS         0x0F 0xa9

#define _PUSHF          0x9c
#define _POPF           0x9d
#define _PUSHFD         0x9c
#define _POPFD          0x9d

#define _STC            0xf9
#define _CLC            0xf8

#define _CBW            0x98
#define _CWD            0x99

#define _PUSH_AX        0x50
#define _PUSH_CX        0x51
#define _PUSH_DX        0x52
#define _PUSH_BX        0x53
#define _PUSH_BP        0x55
#define _POP_AX         0x58
#define _POP_CX         0x59
#define _POP_DX         0x5a
#define _POP_BX         0x5b
#define _POP_BP         0x5d

#define _MOV_AL         0xb0
#define _MOV_AH         0xb4
#define _MOV_AX         0xb8

#define _MOV_AX_W       _USE16 _MOV_AX

#define _MOV_ES_AX      0x8e 0xc0
#define _MOV_ES_CX      0x8e 0xc1
#define _MOV_ES_DX      0x8e 0xc2
#define _MOV_DS_AX      0x8e 0xd8
#define _MOV_DS_CX      0x8e 0xd9
#define _MOV_DS_DX      0x8e 0xda
#define _MOV_AX_ES      0x8c 0xc0
#define _MOV_CX_ES      0x8c 0xc1
#define _MOV_DX_ES      0x8c 0xc2
#define _MOV_AX_SS      0x8c 0xd0

#define _MOV_AX_CX      0x89 0xc8
#define _MOV_AX_DX      0x89 0xd0
#define _MOV_AX_BX      0x89 0xd8
#define _MOV_AX_BP      0x89 0xe8
#define _MOV_AX_SI      0x89 0xf0
#define _MOV_CX_AX      0x89 0xc1
#define _MOV_CX_DX      0x89 0xd1
#define _MOV_CX_BX      0x89 0xd9
#define _MOV_CX_SI      0x89 0xf1
#define _MOV_DX_AX      0x89 0xc2
#define _MOV_DX_CX      0x89 0xca
#define _MOV_DX_BX      0x89 0xda
#define _MOV_BX_AX      0x89 0xc3
#define _MOV_BX_CX      0x89 0xcb
#define _MOV_BX_SI      0x89 0xf3
#define _MOV_BP_AX      0x89 0xc5
#define _MOV_SI_AX      0x89 0xc6
#define _MOV_SI_BX      0x89 0xde
#define _MOV_DI_BX      0x89 0xdf
#define _MOV_DI_SI      0x89 0xf7

#define _MOV_CL_AL      0x88 0xc1
#define _MOV_CH_AL      0x88 0xc5
#define _MOV_AL_BL      0x88 0xd8

#define _NOT_AX         0xf7 0xd0
#define _NOT_DX         0xf7 0xd2
#define _NOT_BX         0xf7 0xd3
#define _NOT_DI         0xf7 0xd7
#define _INC_AX         0x40
#define _INC_DX         0x42
#define _INC_BX         0x43
#define _DEC_AX         0x48
#define _DEC_DX         0x4a
#define _DEC_BX         0x4b
#define _SUB_CX_N       0x81 0xe9
#define _SBB_DX_DX      0x19 0xd2
#define _SBB_CX_CX      0x19 0xc9
#define _SBB_BX_BX      0x19 0xdb
#define _SBB_AX_AX      0x19 0xc0
#define _SBB_BP_BP      0x19 0xed
#define _SBB_DI_DI      0x19 0xff
#define _AND_AX_DX      0x21 0xd0
#define _AND_AX_BX      0x21 0xd8
#define _AND_AX_CX      0x21 0xc8
#define _AND_AX_DI      0x21 0xf8
#define _AND_BX_AX      0x21 0xc3
#define _AND_BX_DX      0x21 0xd3
#define _AND_CX_AX      0x21 0xc1
#define _AND_DX_AX      0x21 0xc2
#define _AND_DX_BX      0x21 0xda
#define _AND_DI_AX      0x21 0xc7
#define _OR_AX_BX       0x09 0xd8
#define _OR_DX_AX       0x09 0xc2
#define _OR_DX_BX       0x09 0xda
#define _OR_CX_AX       0x09 0xc1
#define _OR_BX_AX       0x09 0xc3
#define _XOR_AX_AX      0x31 0xc0
#define _XOR_BX_BX      0x31 0xdb
#define _XOR_CX_CX      0x31 0xc9
#define _XOR_DH_DH      0x30 0xf6
#define _XOR_DX_DX      0x31 0xd2
#define _XOR_DI_DI      0x31 0xff

#define _ADD_SP         0x83 0xc4

#define _AND_AX_B       0x83 0xe0

#define _XCHG_AX_DX     0x92
#define _XCHG_AX_SI     0x96
#define _XCHG_SI_DI     0x87 0xf7
#define _XCHG_BX_CX     0x87 0xd9
#define _XCHG_BX_DX     0x87 0xda

#define _MUL_BX         0xf7 0xe3
#define _MUL_CX         0xf7 0xe1

#define _RCL_DX_1       0xd1 0xd2
#define _ROR_DX_1       0xd1 0xca
#define _RCL_AX_1       0xd1 0xd0
#define _ROR_AX_1       0xd1 0xc8

#define _TEST_CL        0xf6 0xc1
#define _TEST_BL        0xf6 0xc3
#define _TEST_BH_BH     0x84 0xff

#define _JC             0x72
#define _JNC            0x73
#define _JZ             0x74
#define _JNZ            0x75
#define _JS             0x78
#define _JNS            0x79
#define _JMP_SHORT      0xeb

#define _INT            0xcd

#define VECTOR_DOS      0x21
#define VECTOR_MULTIPLEX 0x2F
#define VECTOR_DPMI     0x31

#define _INT_21         _INT VECTOR_DOS
#define _INT_25         _INT 0x25
#define _INT_26         _INT 0x26
#define _INT_2F         _INT VECTOR_MULTIPLEX
#define _INT_31         _INT VECTOR_DPMI

#define _GET_           0x00
#define _SET_           0x01

#define _SHR_EAX_N      0xc1 0xe8
#define _SHR_ECX_N      0xc1 0xe9
#define _SHR_EDX_N      0xc1 0xea
#define _SHR_EBX_N      0xc1 0xeb
#define _SHR_ESI_N      0xc1 0xee
#define _SHL_EAX_N      0xc1 0xe0
#define _SHL_ECX_N      0xc1 0xe1
#define _SHL_EDX_N      0xc1 0xe2
#define _SHL_EBX_N      0xc1 0xe3
#define _SHL_ESI_N      0xc1 0xe6

#if defined( _M_I86 )

 #if defined( __COMPACT__ ) \
   || defined( __LARGE__ ) \
   || defined( __HUGE__ )
  #define _SET_ES
  #define _RST_ES
  #define _SAVE_ES
  #define _REST_ES
  #define _MODIF_ES     __es
 #else
  #define _SET_ES       _PUSH_ES _PUSH_DS _POP_ES
  #define _RST_ES       _POP_ES
  #define _SAVE_ES      _PUSH_ES
  #define _REST_ES      _POP_ES
  #define _MODIF_ES
 #endif

 #if defined( __COMPACT__ ) \
   || defined( __LARGE__ ) \
   || defined( __HUGE__ )
  #define _SET_DSDX     _PUSH_DS _XCHG_AX_DX _MOV_DS_AX
  #define _SET_DSSI     _PUSH_DS _XCHG_AX_SI _MOV_DS_AX
  #define _RST_DS       _POP_DS
 #else
  #define _SET_DSDX
  #define _SET_DSSI
  #define _RST_DS
 #endif
 #if defined( __SMALL__ ) \
   || defined( __MEDIUM__ ) \
   || defined( __SW_ZDP ) \
   || defined( __WINDOWS__ )
  #define _SET_DS_DGROUP
  #define _SET_DS_DGROUP_SAFE
  #define _RST_DS_DGROUP
  #define _SET_DS_SREG          _PUSH_DS _MOV_AX_ES _MOV_DS_AX
  #define _SET_DS_SREG_SAFE     _PUSH_DS _PUSH_ES _POP_DS
  #define _RST_DS_SREG          _POP_DS
  #define _SREG                 __es
  #define _SAVE_DSCX            _PUSH_DS _MOV_DS_CX
  #define _SAVE_DSDX            _PUSH_DS _MOV_DS_DX
  #define _SAVE_DSCS            _PUSH_DS _PUSH_CS _POP_DS
  #define _SAVE_DS              _PUSH_DS
  #define _REST_DS              _POP_DS
  #define _MODIF_DS
 #else
  #define _SET_DS_DGROUP        _PUSH_DS _MOV_AX_SS _MOV_DS_AX
  #define _SET_DS_DGROUP_SAFE   _PUSH_DS _PUSH_SS _POP_DS
  #define _RST_DS_DGROUP        _POP_DS
  #define _SET_DS_SREG
  #define _SET_DS_SREG_SAFE
  #define _RST_DS_SREG
  #define _SREG                 __ds
  #define _SAVE_DSCX            _MOV_DS_CX
  #define _SAVE_DSDX            _MOV_DS_DX
  #define _SAVE_DSCS            _PUSH_CS _POP_DS
  #define _SAVE_DS
  #define _REST_DS
  #define _MODIF_DS             __ds
 #endif

#else       /* __386__ */

 #if defined( __FLAT__ )
  #define _SET_ES
  #define _RST_ES
 #else
  #define _SET_ES               _PUSH_ES _PUSH_DS _POP_ES
  #define _RST_ES               _POP_ES
 #endif
 #define _SAVE_ES               _PUSH_ES
 #define _REST_ES               _POP_ES
 #define _MODIF_ES

 #define _SET_DSDX
 #define _SET_DSSI
 #define _RST_DS
 #define _SET_DS_DGROUP
 #define _SET_DS_DGROUP_SAFE
 #define _RST_DS_DGROUP
 #define _SET_DS_SREG
 #define _SET_DS_SREG_SAFE
 #define _RST_DS_SREG
 #define _SREG
 #define _SAVE_DSCX             _PUSH_DS _MOV_DS_CX
 #define _SAVE_DSDX             _PUSH_DS _MOV_DS_DX
 #define _SAVE_DSCS             _PUSH_DS _PUSH_CS _POP_DS
 #define _SAVE_DS               _PUSH_DS
 #define _REST_DS               _POP_DS
 #define _MODIF_DS

#endif

#endif
