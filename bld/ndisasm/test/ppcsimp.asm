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
;* Description:  Power PC simplified mnemonics test.
;*
;*****************************************************************************


start:
        trap
        twllti  r1,0x00002781
        tweqi   r2,0x00002781
        twlgei  r3,0x00002781
        twllei  r4,0x00002781
        twgti   r5,0x00002781
        twgei   r6,0x00002781
        twlti   r7,0x00002781
        twlei   r8,0x00002781
        twnei   r9,0x00002781
        twllt   r10,r11
        tweq    r11,r12
        twlge   r12,r13
        twlle   r13,r14
        twgt    r14,r15
        twge    r15,r16
        twlt    r16,r17
        twle    r17,r18
        twne    r18,r19
        nop
        li      r1,0x00000231
        lis     r2,0x00003782
        mr      r3,r4
        not     r5,r6

        mtxer   r1
        mtlr    r2
        mtctr   r3
        mtdsisr r4
        mtdar   r5
        mtdec   r6
        mtsdr1  r7
        mtsrr0  r8
        mtsrr1  r9
        mtsprg  0x00000000,r10
        mtsprg  0x00000001,r11
        mtsprg  0x00000002,r12
        mtsprg  0x00000003,r13
        mtear   r14
        mtibatu 0x00000000,r15
        mtibatu 0x00000001,r16
        mtibatu 0x00000002,r17
        mtibatu 0x00000003,r18
        mtibatl 0x00000000,r19
        mtibatl 0x00000001,r20
        mtibatl 0x00000002,r21
        mtibatl 0x00000003,r22

        mfxer   r1
        mflr    r2
        mfctr   r3
        mfdsisr r4
        mfdar   r5
        mfdec   r6
        mfsdr1  r7
        mfsrr0  r8
        mfsrr1  r9
        mfsprg  r10,0x00000000
        mfsprg  r11,0x00000001
        mfsprg  r12,0x00000002
        mfsprg  r13,0x00000003
        mfear   r14
        mfpvr   r15
        mfibatu r16,0x00000000
        mfibatu r17,0x00000001
        mfibatu r18,0x00000002
        mfibatu r19,0x00000003
        mfibatl r20,0x00000000
        mfibatl r21,0x00000001
        mfibatl r22,0x00000002
        mfibatl r23,0x00000003

end:
        blt     end
        blta    end
        bltla   end
        bltlr
        bltlrl
        bltctr
        bltctrl
        bltl    end
        bgt     cr1,end
        bgta    cr1,end
        bgtla   cr1,end
        bgtlr   cr1
        bgtlrl  cr1
        bgtctr  cr1
        bgtctrl cr1
        bgtl    cr1,end
        beq     cr2,end
        beqa    cr2,end
        beqla   cr2,end
        beqlr   cr2
        beqlrl  cr2
        beqctr  cr2
        beqctrl cr2
        beql    cr2,end
        bso     cr3,end
        bsoa    cr3,end
        bsola   cr3,end
        bsolr   cr3
        bsolrl  cr3
        bsoctr  cr3
        bsoctrl cr3
        bsol    cr3,end
        bge     cr4,end
        bgea    cr4,end
        bgela   cr4,end
        bgelr   cr4
        bgelrl  cr4
        bgectr  cr4
        bgectrl cr4
        bgel    cr4,end
        ble     cr5,end
        blea    cr5,end
        blela   cr5,end
        blelr   cr5
        blelrl  cr5
        blectr  cr5
        blectrl cr5
        blel    cr5,end
        bne     cr6,end
        bnea    cr6,end
        bnela   cr6,end
        bnelr   cr6
        bnelrl  cr6
        bnectr  cr6
        bnectrl cr6
        bnel    cr6,end
        bns     cr7,end
        bnsa    cr7,end
        bnsla   cr7,end
        bnslr   cr7
        bnslrl  cr7
        bnsctr  cr7
        bnsctrl cr7
        bnsl    cr7,end

        bdnzf   lt,end
        bdnzfl  gt,end
        bdnzfa  eq,end
        bdnzfla so,end
        bdnzflr cr1+lt
        bdzt    cr1+gt,end
        bdztl   cr1+eq,end
        bdzta   cr1+so,end
        bdztla  cr2+lt,end
        bdztlr  cr2+gt

        bdnzt   cr2+eq,end
        bdnztl  cr2+so,end
        bdnzta  cr3+lt,end
        bdnztla cr3+gt,end
        bdnztlr cr3+eq
        bdzt    cr3+so,end
        bdztl   cr4+lt,end
        bdzta   cr4+gt,end
        bdztla  cr4+eq,end
        bdztlr  cr4+so

        bdnz    end
        bdnzl   end
        bdnza   end
        bdnzla  end
        bdnzlr
        bdz     end
        bdzl    end
        bdza    end
        bdzla   end
        bdzlr
