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
// COREAD       : i/o co-routines for READ
//

#include "ppc_regs.h"
#include "ptypes.h"


.data
TLink:  .long   0       // temporary link storer (no recursion shall occur)

.text

.extern IORslt
.extern DoRead
.extern __RT_SetIOCB

.extern SwitchToRT
.extern IOChar
.extern IOStr
.extern IOArr
.extern IOChArr
.extern RdWrCommon


.globl  __RT_IORead
// .ent __RT_IORead
__RT_IORead:
        lis     t0, h^TLink             // prepare to save link
        mflr    t1                      // load link register
        stw     t1, l^TLink(t0)         // save link register
        bl      j^__RT_SetIOCB          // initialize i/o
        lis     t0, h^TLink             // ...
        lwz     t1, l^TLink(t0)         // load link register
        mtlr    t1                      // restore link register
        lis     a0, h^DoRead            // indicate read
        addi    a0, a0, l^DoRead        // ...
        b       j^RdWrCommon            // start read operation
// .end __RT_IORead


.globl  __RT_InpLOG1
// .ent __RT_InpLOG1                    // input LOGICAL*1 value
__RT_InpLOG1:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_LOG_1            // return LOGICAL*1 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpLOG1


.globl  __RT_InpLOG4
// .ent __RT_InpLOG4                    // input LOGICAL*4 value
__RT_InpLOG4:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_LOG_4            // return LOGICAL*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpLOG4


.globl  __RT_InpINT1
// .ent __RT_InpINT1                    // input INTEGER*1 value
__RT_InpINT1:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_INT_1            // return INTEGER*1 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpINT1


.globl  __RT_InpINT2
// .ent __RT_InpINT2                    // input INTEGER*2 value
__RT_InpINT2:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_INT_2            // return INTEGER*2 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpINT2


.globl  __RT_InpINT4
// .ent __RT_InpINT4                    // input INTEGER*4 value
__RT_InpINT4:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_INT_4            // return INTEGER*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpINT4


.globl  __RT_InpREAL
// .ent __RT_InpREAL                    // input REAL*4 value
__RT_InpREAL:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_REAL_4           // return REAL*4 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpREAL


.globl  __RT_InpDBLE
// .ent __RT_InpDBLE                    // input REAL*8 value
__RT_InpDBLE:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_REAL_8           // return REAL*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpDBLE


.globl  __RT_InpXTND
// .ent __RT_InpXTND                    // input REAL*16 value
__RT_InpXTND:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_REAL_16          // return REAL*16 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpXTND


.globl  __RT_InpCPLX
// .ent __RT_InpCPLX                    // input REAL*8 value
__RT_InpCPLX:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_CPLX_8           // return COMPLEX*8 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpCPLX


.globl  __RT_InpDBCX
// .ent __RT_InpDBCX                    // input COMPLEX*16 value
__RT_InpDBCX:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_CPLX_16          // return COMPLEX*16 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpDBCX


.globl  __RT_InpXTCX
// .ent __RT_InpXTCX                    // input COMPLEX*32 value
__RT_InpXTCX:
        lis     t0, h^IORslt            // prepare loading register
        stw     a0, l^IORslt(t0)        // load result pointer
        li      a0, PT_CPLX_32          // return COMPLEX*32 type
        b       j^SwitchToRT            // return to caller of IOType()
// .end __RT_InpXTCX


.globl  __RT_InpCHAR
// .ent __RT_InpCHAR                    // input CHARACTER*n value
__RT_InpCHAR:
        b       j^IOChar                // do character read
// .end __RT_InpCHAR


.globl  __RT_InpSTR
// .ent __RT_InpSTR                     // input CHARACTER*n value
__RT_InpSTR:
        b       j^IOStr                 // do character read
// .end __RT_InpSTR


.globl  __RT_InpArr
// .ent __RT_InpArr                     // input array
__RT_InpArr:
        b       j^IOArr                 // do array read
// .end __RT_InpArr


.globl  __RT_InpChArr
// .ent __RT_InpChArr                   // input character array
__RT_InpChArr:
        b       j^IOChArr               // do character array read
// .end __RT_InpChArr
