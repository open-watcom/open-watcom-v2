;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


//
// COWRITE      : i/o co-routines for WRITE
//

#include "axp_regs.h"
#include "ptypes.h"

.text

// Runtime C routines
.extern    DoWrite
.extern    __RT_SetIOCB

// data buffer
.extern    IORslt

// general coroutine functions
.extern    SwitchToRT
.extern    IOChar
.extern    IOStr
.extern    IOArr
.extern    IOChArr
.extern    RdWrCommon

.globl __RT_IOWrite
// .ent __RT_IOWrite
__RT_IOWrite:
        lda     sp, -0x10(sp)           // create stack frame
        stq     ra, (sp)                // save return address
        bsr     ra, j^__RT_SetIOCB      // ini i/o
        ldah    a0, h^DoWrite(zero)     // load pointer to write procedue
        lda     a0, l^DoWrite(a0)       // ...
        ldq     ra, (sp)                // restore ra
        lda     sp, 0x10(sp)            // ...
        br      zero, j^RdWrCommon      // start write operation
// .end __RT_IOWrite


.globl  __RT_OutLOG1
// .ent __RT_OutLOG1                    // output LOGICAL*1 value
__RT_OutLOG1:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stl     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        lda     v0, PT_LOG_1(zero)      // return LOGICAL*1 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutLOG1


.globl  __RT_OutLOG4
// .ent __RT_OutLOG4                    // output LOGICAL*4 value
__RT_OutLOG4:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stl     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        lda     v0, PT_LOG_4(zero)      // return LOGICAL*4 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutLOG4


.globl  __RT_OutINT1
// .ent __RT_OutINT1                    // output INTEGER*1 value
__RT_OutINT1:
        sll     a0, 0x38, a0            // sign extend the integer
        sra     a0, 0x38, a0            // ....
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stl     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        lda     v0, PT_INT_1(zero)      // return INTEGER*1 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutINT1


.globl  __RT_OutINT2
// .ent __RT_OutINT2                    // output INTEGER*2 value
__RT_OutINT2:
        sll     a0, 0x20, a0            // sign extend the integer
        sra     a0, 0x20, a0            // ....
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stl     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        lda     v0, PT_INT_2(zero)      // return INTEGER*2 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutINT2


.globl  __RT_OutINT4
// .ent __RT_OutINT4                    // output INTEGER*4 value
__RT_OutINT4:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stl     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        lda     v0, PT_INT_4(zero)      // return INTEGER*4 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutINT4


.globl  __RT_OutREAL
// .ent __RT_OutREAL                    // output REAL*4 value
__RT_OutREAL:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        sts     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     v0, PT_REAL_4(zero)     // return REAL*4 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutREAL


.globl  __RT_OutDBLE
// .ent __RT_OutDBLE                    // output REAL*8 value
__RT_OutDBLE:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stt     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     v0, PT_REAL_8(zero)     // return REAL*8 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutDBLE


.globl  __RT_OutXTND                    // not supported on the AXP.
// .ent __RT_OutXTND                    // output REAL*16 value
__RT_OutXTND:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stt     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     v0, PT_REAL_16(zero)    // return REAL*16 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutXTND


.globl  __RT_OutCPLX
// .ent __RT_OutCPLX                    // output COMPLEX*8 value
__RT_OutCPLX:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        sts     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     t0, 0x04(t0)            // place imaginary
        sts     f17, l^IORslt(t0)       // ...
        lda     v0, PT_CPLX_8(zero)     // return COMPLEX*8 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutCPLX


.globl  __RT_OutDBCX
// .ent __RT_OutDBCX                    // output COMPLEX*16 value
__RT_OutDBCX:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stt     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     t0, 0x08(t0)            // place imaginary
        stt     f17, l^IORslt(t0)       // ...
        lda     v0, PT_CPLX_16(zero)    // return COMPLEX*8 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutDBCX


.globl  __RT_OutXTCX            // Not supported on AXP
// .ent __RT_OutXTCX              // output COMPLEX*32 value
__RT_OutXTCX:
        ldah    t0, h^IORslt(zero)      // load IORslt address (top part only)
        stt     f16, l^IORslt(t0)       // place value in IORslt (using offset)
        lda     t0, 0x10(t0)            // place imaginary
        stt     f17, l^IORslt(t0)       // ...
        lda     v0, PT_CPLX_16(zero)    // return COMPLEX*8 type
        br      zero, j^SwitchToRT      // return to caller of IOType()
// .end __RT_OutXTCX


.globl  __RT_OutCHAR
// .ent __RT_OutCHAR                    // output CHARACTER*n value
__RT_OutCHAR:
        br      zero, j^IOChar          // do character write
// .end __RT_OutCHAR


.globl  __RT_OutSTR
// .ent __RT_OutSTR                     // output CHARACTER*n value
__RT_OutSTR:
        br      zero, j^IOStr           // do character write
// .end __RT_OutSTR


.globl  __RT_PrtArr
// .ent __RT_PrtArr                     // output array
__RT_PrtArr:
        br      zero, j^IOArr           // do array write
// .end __RT_PrtArr


.globl  __RT_PrtChArr
// .ent __RT_PrtChArr                   // output character array
__RT_PrtChArr:
        br      zero, j^IOChArr         // do character array write
// .end __RT_PrtChArr
