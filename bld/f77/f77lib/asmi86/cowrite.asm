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


;;
;; COWRITE      : i/o co-routines for WRITE
;;

.8087

include ptypes.inc
include struct.inc
include mdef.inc

        xrefp   "C",DoWrite
        xrefp   RT@SetIOCB

        extrn   IOSwitch        : near
        extrn   IOChar          : near
        extrn   IOStr           : near
        extrn   IOArr           : near
        extrn   IOChArr         : near
        extrn   RdWrCommon      : near

        modstart cowrite, byte

        dataseg
        assume  ES:DGROUP

        xrefd   "C",IORslt,       word

if _MATH eq _EMULATION
        saveSI          dw      ?
        saveES          dw      ?
        saveReturn      dw      ?, ?
endif

        enddata


        xdefp   RT@IOWrite
        defp    RT@IOWrite
        call    RT@SetIOCB              ; initialize i/o
        mov     AX,seg DoWrite          ; indicate write operation
        push    AX                      ; ...
        mov     AX,offset DoWrite       ; ...
        push    AX                      ; ...
        jmp     RdWrCommon              ; start i/o operation
        endproc RT@IOWrite


        xdefp   RT@OutLOG1
        defp    RT@OutLOG1              ; output LOGICAL*1 value
        push    DX                      ; save registers
        push    ES                      ; ...
        mov     DX,DGROUP               ; get segment address of DGROUP
        mov     ES,DX                   ; ...
        cbw                             ; make LOGICAL*4
        cwd                             ; ...
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        pop     ES                      ; restore registers
        pop     DX                      ; ...
        mov     AX,PT_LOG_1             ; return LOGICAL*1 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutLOG1


        xdefp   RT@OutLOG4
        defp    RT@OutLOG4              ; output LOGICAL*4 value
        push    CX                      ; save registers
        push    ES                      ; ...
        mov     CX,DGROUP               ; get segment address of DGROUP
        mov     ES,CX                   ; ...
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        pop     ES                      ; restore registers
        pop     CX                      ; ...
        mov     AX,PT_LOG_4             ; return LOGICAL*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutLOG4


        xdefp   RT@OutINT1
        defp    RT@OutINT1              ; output INTEGER*1 value
        push    DX                      ; save registers
        push    ES                      ; ...
        mov     DX,DGROUP               ; get segment address of DGROUP
        mov     ES,DX                   ; ...
        cbw                             ; make INTEGER*4
        cwd                             ; ...
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        pop     ES                      ; restore registers
        pop     DX                      ; ...
        mov     AX,PT_INT_1             ; return INTEGER*1 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc  RT@OutINT1


        xdefp   RT@OutINT2
        defp    RT@OutINT2              ; output INTEGER*2 value
        push    DX                      ; save registers
        push    ES                      ; ...
        mov     DX,DGROUP               ; get segment address of DGROUP
        mov     ES,DX                   ; ...
        cwd                             ; make INTEGER*4
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        pop     ES                      ; restore registers
        pop     DX                      ; ...
        mov     AX,PT_INT_2             ; return INTEGER*2 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutINT2


        xdefp   RT@OutINT4
        defp    RT@OutINT4              ; output INTEGER*4 value
        push    CX                      ; save registers
        push    ES                      ; ...
        mov     CX,DGROUP               ; get segment address of DGROUP
        mov     ES,CX                   ; ...
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        pop     ES                      ; restore registers
        pop     CX                      ; ...
        mov     AX,PT_INT_4             ; return INTEGER*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutINT4


        xdefp   RT@OutREAL
        defp    RT@OutREAL              ; output REAL*4 value
        push    CX                      ; save registers
        push    ES                      ; ...
        mov     CX,DGROUP               ; get segment address of DGROUP
        mov     ES,CX                   ; ...
if _MATH eq _8087
        fstp    dword ptr ES:IORslt     ; place value in IORslt
else
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     CX                      ; ...
        mov     AX,PT_REAL_4            ; return REAL*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutREAL


        xdefp   RT@OutDBLE
        defp    RT@OutDBLE              ; output REAL*8 value
        push    SI                      ; save registers
        push    ES                      ; ...
        mov     SI,DGROUP               ; get segment address of DGROUP
        mov     ES,SI                   ; ...
if _MATH eq _8087
        fstp    qword ptr ES:IORslt     ; place value in IORslt
else
        mov     ES:IORslt,DX            ; place value in IORslt
        mov     ES:IORslt+2,CX          ; ...
        mov     ES:IORslt+4,BX          ; ...
        mov     ES:IORslt+6,AX          ; ...
endif
        pop     ES                      ; restore registers
        pop     SI                      ; ...
        mov     AX,PT_REAL_8            ; return REAL*8 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutDBLE


        xdefp   RT@OutXTND
        defp    RT@OutXTND              ; output REAL*16 value
if _MATH eq _8087
        push    SI                      ; save registers
        push    ES                      ; ...
else
        mov     saveSI,SI               ; save regs and return
        mov     saveES,ES               ; ...
        pop     saveReturn              ; ...
if _MODEL and _BIG_CODE
        pop     saveReturn+2            ; ...
endif
endif
        mov     SI,DGROUP               ; get segment address of DGROUP
        mov     ES,SI                   ; ...
if _MATH eq _8087
        fstp    tbyte ptr ES:IORslt     ; place value in IORslt
        pop     ES                      ; restore registers
        pop     SI                      ; ...
else
        pop     ES:IORslt               ; place value in IORslt
        pop     ES:IORslt+2             ; ...
        pop     ES:IORslt+4             ; ...
        pop     ES:IORslt+6             ; ...
        pop     ES:IORslt+8             ; ...
        pop     ES:IORslt+10            ; ...
        pop     ES:IORslt+12            ; ...
        pop     ES:IORslt+14            ; ...
if _MODEL and _BIG_CODE
        push    saveReturn+2            ; resotre regs and stuff
endif
        push    saveReturn              ; ...
        mov     SI, saveSI              ; ...
        mov     ES, saveES              ; ...
endif
        mov     AX,PT_REAL_16           ; return REAL*16 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutXTND


        xdefp   RT@OutCPLX
        defp    RT@OutCPLX              ; output COMPLEX*8 value
        push    SI                      ; save registers
        push    ES                      ; ...
        mov     SI,DGROUP               ; get segment address of DGROUP
        mov     ES,SI                   ; ...
if _MATH eq _8087
        fstp    dword ptr ES:IORslt     ; place value in IORslt
        fstp    dword ptr ES:IORslt+4   ; ...
else
        mov     ES:IORslt,AX            ; place value in IORslt
        mov     ES:IORslt+2,DX          ; ...
        mov     ES:IORslt+4,BX          ; ...
        mov     ES:IORslt+6,CX          ; ...
endif
        pop     ES                      ; restore registers
        pop     SI                      ; ...
        mov     AX,PT_CPLX_8            ; return COMPLEX*8 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutCPLX


        xdefp   RT@OutDBCX
        defp    RT@OutDBCX              ; output COMPLEX*16 value
        push    SI                      ; save registers
        push    ES                      ; ...
        mov     SI,DGROUP               ; get segment address of DGROUP
        mov     ES,SI                   ; ...
if _MATH eq _8087
        fstp    qword ptr ES:IORslt     ; place value in IORslt
        fstp    qword ptr ES:IORslt+8   ; ...
        pop     ES                      ; restore registers
        pop     SI                      ; ...
else
        mov     ES:IORslt,DX            ; place real part in IORslt
        mov     ES:IORslt+2,CX          ; ...
        mov     ES:IORslt+4,BX          ; ...
        mov     ES:IORslt+6,AX          ; ...
        pop     ES                      ; restore registers
        pop     SI                      ; ...
        pop     AX                      ; get return address
if _MODEL and _BIG_CODE
        pop     BX                      ; ...
endif
        mov     DX,ES                   ; save registers
        mov     CX,DGROUP               ; get segment address of DGROUP
        mov     ES,CX                   ; ...
        pop     ES:IORslt+8             ; place imaginary part in IORslt
        pop     ES:IORslt+10            ; ...
        pop     ES:IORslt+12            ; ...
        pop     ES:IORslt+14            ; ...
        mov     ES,DX                   ; restore registers
if _MODEL and _BIG_CODE
        push    BX                      ; restore return address
endif
        push    AX                      ; ...
endif
        mov     AX,PT_CPLX_16           ; return COMPLEX*16 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutDBCX


        xdefp   RT@OutXTCX
        defp    RT@OutXTCX              ; output COMPLEX*32 value
if _MATH eq _8087
        push    SI                      ; save registers
        push    ES                      ; ...
else
        mov     saveSI, SI              ; save regs and return
        mov     saveES, ES              ; ...
        pop     saveReturn              ; ...
if _MODEL and _BIG_CODE
        pop     saveReturn+2            ; ...
endif
endif
        mov     SI,DGROUP               ; get segment address of DGROUP
        mov     ES,SI                   ; ...
if _MATH eq _8087
        fstp    tbyte ptr ES:IORslt     ; place value in IORslt
        fstp    tbyte ptr ES:IORslt+16  ; ...
        pop     ES                      ; restore registers
        pop     SI                      ; ...
else
        pop     ES:IORslt               ; place real part in IORslt
        pop     ES:IORslt+2             ; ...
        pop     ES:IORslt+4             ; ...
        pop     ES:IORslt+6             ; ...
        pop     ES:IORslt+8             ; ...
        pop     ES:IORslt+10            ; ...
        pop     ES:IORslt+12            ; ...
        pop     ES:IORslt+14            ; ...
        pop     ES:IORslt+16            ; ...
        pop     ES:IORslt+18            ; ...
        pop     ES:IORslt+20            ; ...
        pop     ES:IORslt+22            ; ...
        pop     ES:IORslt+24            ; ...
        pop     ES:IORslt+26            ; ...
        pop     ES:IORslt+28            ; ...
        pop     ES:IORslt+30            ; ...

if _MODEL and _BIG_CODE
        push    saveReturn+2            ; ...
endif
        push    saveReturn              ; ...
        mov     SI, saveSI              ; restore regs and return
        mov     ES, saveES              ; ...
endif
        mov     AX,PT_CPLX_32           ; return COMPLEX*32 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@OutXTCX


        xdefp   RT@OutCHAR
        defp    RT@OutCHAR              ; output CHARACTER*n value
        jmp     IOChar                  ; do character write
        endproc RT@OutCHAR


        xdefp   RT@OutSTR
        defp    RT@OutSTR               ; output CHARACTER*n value
        jmp     IOStr                   ; do character write
        endproc RT@OutSTR


        xdefp   RT@PrtArr
        defp    RT@PrtArr               ; output array
        jmp     IOArr                   ; do array write
        endproc RT@PrtArr


        xdefp   RT@PrtChArr
        defp    RT@PrtChArr             ; output character array
        jmp     IOChArr                 ; do character array write
        endproc RT@PrtChArr

        endmod
        end
