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

.386p
.8087

include ptypes.inc
include struct.inc
include mdef.inc

        xrefp   "C",DoWrite
        xrefp   RT@SetIOCB

        dataseg

        xrefd   "C",IORslt,       word

        enddata

        modstart cowrite, byte

        extrn   SwitchToRT      : near
        extrn   IOChar          : near
        extrn   IOStr           : near
        extrn   IOArr           : near
        extrn   IOChArr         : near
        extrn   RdWrCommon      : near


        xdefp   RT@IOWrite
        defp    RT@IOWrite
        call    RT@SetIOCB              ; initialize i/o
        mov     EAX,offset DoWrite      ; indicate write
        jmp     RdWrCommon              ; start write operation
        endproc RT@IOWrite


        xdefp   RT@OutLOG1
        defp    RT@OutLOG1              ; output LOGICAL*1 value
        cbw                             ; make LOGICAL*4
        cwde                            ; ...
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     EAX,PT_LOG_1            ; return LOGICAL*1 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutLOG1


        xdefp   RT@OutLOG4
        defp    RT@OutLOG4              ; output LOGICAL*4 value
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     AX,PT_LOG_4             ; return LOGICAL*4 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutLOG4


        xdefp   RT@OutINT1
        defp    RT@OutINT1              ; output INTEGER*1 value
        cbw                             ; make INTEGER*4
        cwde                            ; ...
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     EAX,PT_INT_1            ; return INTEGER*1 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc  RT@OutINT1


        xdefp   RT@OutINT2
        defp    RT@OutINT2              ; output INTEGER*2 value
        cwde                            ; make INTEGER*4
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     EAX,PT_INT_2            ; return INTEGER*2 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutINT2


        xdefp   RT@OutINT4
        defp    RT@OutINT4              ; output INTEGER*4 value
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     EAX,PT_INT_4            ; return INTEGER*4 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutINT4


        xdefp   RT@OutREAL
        defp    RT@OutREAL              ; output REAL*4 value
if _MATH eq _8087
        fstp    dword ptr IORslt        ; put value in IORslt
else
        mov     dword ptr IORslt,EAX    ; place value in IORslt
endif
        mov     EAX,PT_REAL_4           ; return REAL*4 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutREAL


        xdefp   RT@OutDBLE
        defp    RT@OutDBLE              ; output REAL*8 value
if _MATH eq _8087
        fstp    qword ptr IORslt        ; put value in IORslt
else
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     dword ptr IORslt+4,EDX  ; ...
endif
        mov     EAX,PT_REAL_8           ; return REAL*8 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutDBLE


        xdefp   RT@OutXTND
        defp    RT@OutXTND              ; output REAL*16 value
if _MATH eq _8087
        fstp    tbyte ptr IORslt        ; put value in IORslt
else
        pop     EAX                     ; save the return address
        pop     dword ptr IORslt        ; put value in IORslt
        pop     dword ptr IORslt+4      ; ...
        pop     dword ptr IORslt+8      ; ...
        pop     dword ptr IORslt+12     ; ...
        push    EAX                     ; restore the return address
endif
        mov     EAX,PT_REAL_16          ; return REAL*16 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutXTND


        xdefp   RT@OutCPLX
        defp    RT@OutCPLX              ; output COMPLEX*8 value
if _MATH eq _8087
        fstp    dword ptr IORslt        ; put value in IORslt
        fstp    dword ptr IORslt+4      ; ...
else
        mov     dword ptr IORslt,EAX    ; place value in IORslt
        mov     dword ptr IORslt+4,EDX  ; ...
endif
        mov     EAX,PT_CPLX_8           ; return COMPLEX*8 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutCPLX


        xdefp   RT@OutDBCX
        defp    RT@OutDBCX              ; output COMPLEX*16 value
if _MATH eq _8087
        fstp    qword ptr IORslt        ; put value in IORslt
        fstp    qword ptr IORslt+8      ; ...
else
        mov     dword ptr IORslt,EAX    ; put value in IORslt
        mov     dword ptr IORslt+4,EDX  ; ...
        mov     dword ptr IORslt+8,EBX  ; ...
        mov     dword ptr IORslt+12,ECX ; ...
endif
        mov     EAX,PT_CPLX_16          ; return COMPLEX*16 type
        jmp     SwitchToRT              ; return to caller of IOType()
        endproc RT@OutDBCX


        xdefp   RT@OutXTCX
        defp    RT@OutXTCX              ; output COMPLEX*32 value
if _MATH eq _8087
        fstp    tbyte ptr IORslt        ; put value in IORslt
        fstp    tbyte ptr IORslt+16     ; ...
else
        pop     EAX                     ; save return address
        pop     dword ptr IORslt        ; put value in IORslt
        pop     dword ptr IORslt+4      ; ...
        pop     dword ptr IORslt+8      ; ...
        pop     dword ptr IORslt+12     ; ...
        pop     dword ptr IORslt+16     ; ...
        pop     dword ptr IORslt+20     ; ...
        pop     dword ptr IORslt+24     ; ...
        pop     dword ptr IORslt+28     ; ...
        push    EAX                     ; restore return address
endif
        mov     EAX,PT_CPLX_32          ; return COMPLEX*32 type
        jmp     SwitchToRT              ; return to caller of IOType()
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
