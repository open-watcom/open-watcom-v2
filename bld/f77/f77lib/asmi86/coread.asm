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
;; COREAD       : i/o co-routines for READ
;;

.8087

include ptypes.inc
include struct.inc
include mdef.inc

        xref    "C",DoRead
        xref    RT@SetIOCB

        extrn   IOSwitch        : near
        extrn   IOChar          : near
        extrn   IOStr           : near
        extrn   IOArr           : near
        extrn   IOChArr         : near
        extrn   RdWrCommon      : near

        modstart coread, byte

        dataseg
        assume  ES:DGROUP

        xred    "C",IORslt,       word

        enddata


        xdefp   RT@IORead
        defp    RT@IORead
        call    RT@SetIOCB              ; initialize i/o
        mov     AX,seg DoRead           ; indicate read operation
        push    AX                      ; ...
        mov     AX,offset DoRead        ; ...
        push    AX                      ; ...
        jmp     RdWrCommon              ; start i/o operation
        endproc RT@IORead


        xdefp   RT@InpLOG1
        defp    RT@InpLOG1              ; input LOGICAL*1 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_LOG_1             ; return LOGICAL*1 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpLOG1


        xdefp   RT@InpLOG4
        defp    RT@InpLOG4              ; input LOGICAL*4 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_LOG_4             ; return LOGICAL*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpLOG4


        xdefp   RT@InpINT1
        defp    RT@InpINT1              ; input INTEGER*1 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_INT_1             ; return INTEGER*1 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpINT1


        xdefp   RT@InpINT2
        defp    RT@InpINT2              ; input INTEGER*2 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_INT_2             ; return INTEGER*2 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpINT2


        xdefp   RT@InpINT4
        defp    RT@InpINT4              ; input INTEGER*4 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_INT_4             ; return INTEGER*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpINT4


        xdefp   RT@InpREAL
        defp    RT@InpREAL              ; input REAL*4 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_REAL_4            ; return REAL*4 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpREAL


        xdefp   RT@InpDBLE
        defp    RT@InpDBLE              ; input REAL*8 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_REAL_8            ; return REAL*8 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpDBLE


        xdefp   RT@InpXTND
        defp    RT@InpXTND              ; input REAL*16 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_REAL_16           ; return REAL*16 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpXTND


        xdefp   RT@InpCPLX
        defp    RT@InpCPLX              ; input COMPLEX*8 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_CPLX_8            ; return COMPLEX*8 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpCPLX


        xdefp   RT@InpDBCX
        defp    RT@InpDBCX              ; input COMPLEX*16 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_CPLX_16           ; return COMPLEX*16 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpDBCX


        xdefp   RT@InpXTCX
        defp    RT@InpXTCX              ; input COMPLEX*32 value
        push    DI                      ; save registers
        push    ES                      ; ...
        mov     DI,DGROUP               ; get segment address of DGROUP
        mov     ES,DI                   ; ...
        mov     ES:IORslt,AX            ; place destination in IORslt
if _MODEL and _BIG_DATA
        mov     ES:IORslt+2,DX          ; ...
endif
        pop     ES                      ; restore registers
        pop     DI                      ; ...
        mov     AX,PT_CPLX_32           ; return COMPLEX*32 type
        jmp     IOSwitch                ; return to caller of IOType()
        endproc RT@InpXTCX


        xdefp   RT@InpCHAR
        defp    RT@InpCHAR              ; input CHARACTER*n value
        jmp     IOChar                  ; do character read
        endproc RT@InpCHAR


        xdefp   RT@InpSTR
        defp    RT@InpSTR               ; input CHARACTER*n value
        jmp     IOStr                   ; do character read
        endproc RT@InpSTR


        xdefp   RT@InpArr
        defp    RT@InpArr               ; input array
        jmp     IOArr                   ; do array read
        endproc RT@InpArr


        xdefp   RT@InpChArr
        defp    RT@InpChArr             ; input character array
        jmp     IOChArr                 ; do character array read
        endproc RT@InpChArr

        endmod
        end
