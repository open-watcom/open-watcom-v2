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
;* Description:  Power PC instruction set test.
;*
;*****************************************************************************


.globl _abs
.globl _add
.globl _and
.globl _branch
.globl _compare
.globl _count
.globl _condition
.globl _cache
.globl _div
.globl _stuff
.globl _float
.globl _load
.globl _mask
.globl _move
.globl _mul
.globl _not
.globl _or
.globl _system
.globl _rotate
.globl _shift
.globl _store
.globl _sub
.globl _trap
.globl _xor
// all basic opcodes:
_abs:
        abs         r1,r2
        abs.        r3,r4
        abso        r5,r6
        abso.       r7,r8
_add:
        add         r9,r10,r11
        add.        r12,r13,r14
        addo        r15,r16,r17
        addo.       r18,r19,r20
        addc        r21,r22,r23
        addc.       r24,r25,r26
        addco       r27,r28,r29
        addco.      r30,r31,r31
        adde        r1,r2,r3
        adde.       r4,r5,r6
        addeo       r7,r8,r9
        addeo.      r10,r11,r12
        addi        r13,r14,0xffff9837
        addic       r17,r18,0x00007382
        addic.      r19,r20,0x00003672
        addis       r21,r22,0x00004982
        addme       r23,r24
        addme.      r25,r26
        addmeo      r27,r28
        addmeo.     r29,r30
        addze       r31,r1
        addze.      r2,r3
        addzeo      r4,r5
        addzeo.     r6,r7
_and:
        and         r8,r9,r10
        and.        r11,r12,r13
        andc        r14,r15,r16
        andc.       r17,r18,r19
        andi.       r20,r21,0x00000016
        andis.      r23,r24,0x00000019
_branch:
        b           _abs
        ba          _add
        bl          _and
        bla         _branch
        bc          0x12,0x07,_compare
        bca         0x02,0x04,_count
        bcl         0x0b,0x12,_condition
        bcla        0x09,0x1b,_cache
        bcctr       0x1f,0x0e
        bcctrl      0x03,0x1a
        bclr        0x07,0x10
        bclrl       0x04,0x01
_compare:
        cmp         cr0,0,r1,r2
        cmpi        cr1,1,r3,0x00000004
        cmpl        cr2,0,r5,r6
        cmpli       cr3,1,r7,0x00000008
_count:
        cntlzw      r9,r10
_condition:
        crand       crb0,crb1,crb2
        crandc      crb3,crb4,crb5
        creqv       crb6,crb7,crb8
        crnand      crb9,crb10,crb11
        crnor       crb12,crb13,crb14
        cror        crb15,crb16,crb17
        crorc       crb18,crb19,crb20
        crxor       crb21,crb22,crb23
_cache:
        clcs        r1,r2
        dcbf        r3,r4
        dcbi        r5,r6
        dcbst       r7,r8
        dcbt        r9,r10
        dcbtst      r11,r12
        dcbz        r13,r14
_div:
        div         r15,r16,r17
        div.        r18,r19,r20
        divo        r21,r22,r23
        divo.       r24,r25,r26
        divs        r27,r28,r29
        divs.       r30,r31,r1
        divso       r2,r3,r4
        divso.      r5,r6,r7
        divw        r8,r9,r10
        divw.       r11,r12,r13
        divwo       r14,r15,r16
        divwo.      r17,r18,r19
        divwu       r20,r21,r22
        divwu.      r23,r24,r25
        divwuo      r26,r27,r28
        divwuo.     r29,r30,r31
_stuff:
        doz         r1,r2,r3
        doz.        r4,r5,r6
        dozo        r7,r8,r9
        dozo.       r10,r11,r12
        dozi        r13,r14,0x00003728
        eciwx       r15,r16,r17
        ecowx       r18,r19,r20
        eieio
        eqv         r21,r22,r23
        eqv.        r24,r25,r26
        extsb       r27,r28
        extsb.      r29,r30
        extsh       r31,r1
        extsh.      r2,r3
_float:
        fabs        fr1,fr2
        fabs.       fr3,fr4
        fadd        fr5,fr6,fr7
        fadd.       fr8,fr9,fr10
        fcmpo       cr0,fr11,fr12
        fcmpu       cr1,fr13,fr14
        fctiw       fr15,fr16
        fctiw.      fr17,fr18
        fctiwz      fr19,fr20
        fctiwz.     fr21,fr22
        fdiv        fr23,fr24,fr25
        fdiv.       fr25,fr26,fr27
        fdivs       fr27,fr28,fr29
        fdivs.      fr29,fr30,fr31
        fmadd       fr31,fr1,fr2,fr3
        fmadd.      fr4,fr5,fr6,fr7
        fmadds      fr8,fr9,fr10,fr11
        fmadds.     fr12,fr13,fr14,fr15
        fmr         fr16,fr17
        fmr.        fr18,fr19
        fmsub       fr20,fr21,fr22,fr23
        fmsub.      fr24,fr25,fr26,fr27
        fmsubs      fr28,fr29,fr30,fr31
        fmsubs.     fr1,fr2,fr3,fr4
        fmul        fr5,fr6,fr7
        fmul.       fr8,fr9,fr10
        fmuls       fr11,fr12,fr13
        fmuls.      fr14,fr15,fr16
        fnabs       fr17,fr18
        fnabs.      fr19,fr20
        fneg        fr21,fr22
        fneg.       fr23,fr24
        fnmadd      fr25,fr26,fr27,fr28
        fnmadd.     fr29,fr30,fr31,fr1
        fnmadds     fr2,fr3,fr4,fr5
        fnmadds.    fr6,fr7,fr8,fr9
        fnmsub      fr10,fr11,fr12,fr13
        fnmsub.     fr14,fr15,fr16,fr17
        fnmsubs     fr18,fr19,fr20,fr21
        fnmsubs.    fr22,fr23,fr24,fr25
        frsp        fr26,fr27
        frsp.       fr28,fr29
        fsub        fr30,fr31,fr1
        fsub.       fr2,fr3,fr4
        fsubs       fr4,fr5,fr6
        fsubs.      fr6,fr7,fr8
_load:
        lbz         r1,0x47(r2)
        lbzu        r3,0x378(r4)
        lbzux       r5,r6,r7
        lbzx        r8,r9,r10
        lfd         fr11,0x837(r12)
        lfdu        fr13,0x3727(r14)
        lfdux       fr15,r16,r17
        lfdx        fr18,r19,r20
        lfs         fr21,0x273(r22)
        lfsu        fr23,-0x78ce(r24)
        lfsux       fr25,r26,r27
        lfsx        fr27,r28,r29
        lha         r30,0x873(r31)
        lhau        r1,0x872(r2)
        lhaux       r3,r4,r5
        lhax        r6,r7,r8
        lhbrx       r9,r10,r11
        lhz         r12,0x782(r13)
        lhzu        r15,0x1827(r16)
        lhzux       r18,r19,r20
        lhzx        r21,r22,r23
        lmw         r24,0x873(r25)
        lscbx       r26,r27,r28
        lscbx.      r29,r30,r31
        lswi        r1,r2,0x0000000c
        lswx        r3,r4,r5
        lwarx       r6,r7,r8
        lwbrx       r9,r10,r11
        lwz         r12,0x283(r13)
        lwzu        r14,-0x788d(r15)
        lwzux       r16,r17,r18
        lwzx        r19,r20,r21
_mask:
        maskg       r22,r23,r24
        maskg.      r25,r26,r27
        maskir      r28,r29,r30
        maskir.     r31,r1,r2
_move:
        mcrf        cr0,cr1
        mcrfs       cr2,cr3
        mcrxr       cr4
        mfcr        r5
        mffs        fr1
        mffs.       fr2
        mfmsr       r3
        mfspr       r4,0x000003f2
        mfsr        r5,0x0000000c
        mfsrin      r6,r7
        mtcrf       0x00000074,r8
        mtfsb0      crb9
        mtfsb0.     crb10
        mtfsb1      crb11
        mtfsb1.     crb12
        mtfsf       0x00000083,fr13
        mtfsfi      cr7,0x00000005
        mtmsr       r14
        mtspr       0x00000214,r15
        mtsr        0x00000006,r17
        mtsrin      r18,r19
_mul:
        mul         r1,r2,r3
        mul.        r4,r5,r6
        mulo        r7,r8,r9
        mulo.       r10,r11,r12
        mulhw       r13,r14,r15
        mulhw.      r16,r17,r18
        mulhwu      r19,r20,r21
        mulhwu.     r22,r23,r24
        mullw       r25,r26,r27
        mullw.      r28,r29,r30
        mullwo      r31,r1,r2
        mullwo.     r3,r4,r5
        mulli       r6,r7,0x00000746
_not:
        nabs        r8,r9
        nabs.       r10,r11
        nabso       r12,r13
        nabso.      r14,r15
        nand        r16,r17,r18
        nand.       r19,r20,r21
        neg         r22,r23
        neg.        r24,r25
        nego        r26,r27
        nego.       r28,r29
        nor         r30,r31,r1
        nor.        r2,r3,r4
_or:
        or          r5,r6,r7
        or.         r8,r9,r10
        orc         r11,r12,r13
        orc.        r14,r15,r16
        ori         r17,r18,0x00008374
        oris        r19,r20,0x00007362
_system:
        icbi        r1,r2
        isync
        rfi
        sc
        sync
        tlbie       r3
_rotate:
        rlmi        r1,r2,r3,0x00000003,0x00000006
        rlmi.       r4,r5,r6,0x00000004,0x00000009
        rlwimi      r7,r8,0x00000009,0x00000010,0x00000014
        rlwimi.     r9,r10,0x00000004,0x00000002,0x00000007
        rlwinm      r11,r12,0x00000008,0x00000001,0x00000009
        rlwinm.     r13,r14,0x00000012,0x00000004,0x00000007
        rlwnm       r15,r16,r17,0x00000003,0x00000004
        rlwnm.      r18,r19,r20,0x00000005,0x00000008
        rrib        r21,r22,r23
        rrib.       r24,r25,r26
_shift:
        sle         r27,r28,r29
        sle.        r30,r31,r1
        sleq        r2,r3,r4
        sleq.       r5,r6,r7
        sliq        r8,r9,0x00000003
        sliq.       r10,r11,0x00000006
        slliq       r12,r13,0x00000008
        slliq.      r14,r15,0x00000002
        sllq        r16,r17,r18
        sllq.       r19,r20,r21
        slq         r22,r23,r24
        slq.        r25,r26,r27
        slw         r28,r29,r30
        slw.        r31,r1,r2
        sraq        r3,r4,r5
        sraq.       r6,r7,r8
        sraiq       r9,r10,0x00000004
        sraiq.      r11,r12,0x00000008
        sraw        r13,r14,r15
        sraw.       r16,r17,r18
        srawi       r19,r20,0x00000003
        srawi.      r21,r22,0x00000004
        sre         r23,r24,r25
        sre.        r26,r27,r28
        srea        r29,r30,r31
        srea.       r1,r2,r3
        sreq        r4,r5,r6
        sreq.       r7,r8,r9
        sriq        r10,r11,0x00000013
        sriq.       r12,r13,0x00000019
        srliq       r14,r15,0x00000011
        srliq.      r16,r17,0x0000001f
        srlq        r18,r19,r20
        srlq.       r21,r22,r23
        srq         r24,r25,r26
        srq.        r27,r28,r29
        srw         r30,r31,r1
        srw.        r2,r3,r4
_store:
        stb         r1,0x83(r2)
        stbu        r3,0x847(r4)
        stbux       r5,r6,r7
        stbx        r8,r9,r10
        stfd        fr11,0x837(r12)
        stfdu       fr14,0x4728(r15)
        stfdux      fr16,r17,r18
        stfdx       fr19,r20,r21
        stfs        fr22,-0x7d8d(r23)
        stfsu       fr25,0x251(r26)
        stfsux      fr27,r28,r29
        stfsx       fr30,r31,r1
        sth         r2,0x2347(r3)
        sthbrx      r3,r4,r5
        sthu        r6,0x892(r7)
        sthux       r8,r9,r10
        sthx        r11,r12,r13
        stmw        r14,0x2871(r15)
        stswi       r16,r17,0x00000003
        stswx       r18,r19,r20
        stw         r21,0xb23(r22)
        stwbrx      r23,r24,r25
        stwcx.      r26,r27,r28
        stwu        r29,-0x7181(r30)
        stwux       r31,r1,r2
        stwx        r3,r4,r5
_sub:
        subf        r1,r2,r3
        subf.       r4,r5,r6
        subfo       r7,r8,r9
        subfo.      r10,r11,r12
        subfc       r13,r14,r15
        subfc.      r16,r17,r18
        subfco      r19,r20,r21
        subfco.     r22,r23,r24
        subfe       r25,r26,r27
        subfe.      r28,r29,r30
        subfeo      r31,r1,r2
        subfeo.     r3,r4,r5
        subfic      r6,r7,0xffff8272
        subfme      r8,r9
        subfme.     r10,r11
        subfmeo     r12,r13
        subfmeo.    r14,r15
        subfze      r16,r17
        subfze.     r18,r19
        subfzeo     r20,r21
        subfzeo.    r22,r23
_trap:
        tw          0x00000012,r1,r2
        twi         0x00000005,r3,0x00002891
_xor:
        xor         r4,r5,r6
        xor.        r7,r8,r9
        xori        r10,r11,0x00007272
        xoris       r12,r13,0x00005621
