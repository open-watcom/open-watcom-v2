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

#include "ppc_regs.h"
#include "ptypes.h"

.data
TLink:  .long   0       // temporary link storer (no recursion shall occur)

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
        lis     t0, h^TLink             // prepare to save link
        mflr    t1                      // load link register
        stw     t1, l^TLink(t0)         // save link register
        bl      j^__RT_SetIOCB          // initialize i/o
        lis     t0, h^TLink             // ...
        lwz     t1, l^TLink(t0)         // load link register
        mtlr    t1                      // restore link register
        lis     a0, h^DoRead            // indicate write
        addi    a0, a0, l^DoRead        // ...
        b       j^RdWrCommon            // start write operation
// .end __RT_IOWrite


.globl  __RT_OutLOG1
// .ent __RT_OutLOG1                    // output LOGICAL*1 value
__RT_OutLOG1:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stw     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_LOG_1            // return LOGICAL*1 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutLOG1


.globl  __RT_OutLOG4
// .ent __RT_OutLOG4                    // output LOGICAL*4 value
__RT_OutLOG4:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stw     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_LOG_4            // return LOGICAL*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutLOG4


.globl  __RT_OutINT1
// .ent __RT_OutINT1                    // output INTEGER*1 value
__RT_OutINT1:
        extsb   a0, a0                  // sign extend the integer
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stw     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_INT_1            // return INTEGER*1 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutINT1


.globl  __RT_OutINT2
// .ent __RT_OutINT2                    // output INTEGER*2 value
__RT_OutINT2:
        extsh   a0, a0                  // sign extend the integer
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stw     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_INT_2            // return INTEGER*2 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutINT2


.globl  __RT_OutINT4
// .ent __RT_OutINT4                    // output INTEGER*4 value
__RT_OutINT4:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stw     a0, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_INT_4            // return INTEGER*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutINT4


.globl  __RT_OutREAL
// .ent __RT_OutREAL                    // output REAL*4 value
__RT_OutREAL:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stfs    f1, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_REAL_4           // return REAL*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutREAL


.globl  __RT_OutDBLE
// .ent __RT_OutDBLE                    // output REAL*8 value
__RT_OutDBLE:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stfd    f1, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_REAL_8           // return REAL*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutDBLE


.globl  __RT_OutXTND                    // not supported on the PPC
// .ent __RT_OutXTND                    // output REAL*16 value
__RT_OutXTND:
        lis     t0, h^IORslt            // load IORslt address (top part only)
        stfd    f1, l^IORslt(t0)        // place value in IORslt (using offset)
        li      a0, PT_REAL_16          // return REAL*16 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutXTND


.globl  __RT_OutCPLX
// .ent __RT_OutCPLX                    // output COMPLEX*8 value
__RT_OutCPLX:
        lis     t1, h^IORslt            // load IORslt address (top part only)
        stfs    f1, l^IORslt(t0)        // place value in IORslt (using offset)
        addi    t1, t1, 0x04            // place imaginary
        stfs    f2, l^IORslt(t1)        // ...
        li      a0, PT_CPLX_8           // return COMPLEX*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutCPLX


.globl  __RT_OutDBCX
// .ent __RT_OutDBCX                    // output COMPLEX*16 value
__RT_OutDBCX:
        lis     t1, h^IORslt            // load IORslt address (top part only)
        stfd    f1, l^IORslt(t1)        // place value in IORslt (using offset)
        addi    t1, t1, 0x08            // place imaginary
        stfd    f2, l^IORslt(t1)        // ...
        li      a0, PT_CPLX_16          // return COMPLEX*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutDBCX


.globl  __RT_OutXTCX            // Not supported on AXP
// .ent __RT_OutXTCX              // output COMPLEX*32 value
__RT_OutXTCX:
        lis     t1, h^IORslt            // load IORslt address (top part only)
        stfd    f1, l^IORslt(t1)        // place value in IORslt (using offset)
        addi    t1, t1, 0x10            // place imaginary
        stfd    f2, l^IORslt(t1)        // ...
        li      a0, PT_CPLX_16          // return COMPLEX*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_OutXTCX


.globl  __RT_OutCHAR
// .ent __RT_OutCHAR                    // output CHARACTER*n value
__RT_OutCHAR:
        b       j^IOChar                // do character write
// .end __RT_OutCHAR


.globl  __RT_OutSTR
// .ent __RT_OutSTR                     // output CHARACTER*n value
__RT_OutSTR:
        b       j^IOStr                 // do character write
// .end __RT_OutSTR


.globl  __RT_PrtArr
// .ent __RT_PrtArr                     // output array
__RT_PrtArr:
        b       j^IOArr                 // do array write
// .end __RT_PrtArr


.globl  __RT_PrtChArr
// .ent __RT_PrtChArr                   // output character array
__RT_PrtChArr:
        b       j^IOChArr               // do character array write
// .end __RT_PrtChArr
