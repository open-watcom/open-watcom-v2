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
;* Description:  Miscellaneous BIOS services, 32-bit version.
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc

        modstart biosfunc

;unsigned __ibm_bios_equiplist(void);
;unsigned __ibm_bios_keybrd(unsigned __service);
;unsigned __ibm_bios_memsize(void);
;unsigned __ibm_bios_printer(unsigned __service,unsigned __port,unsigned __data);
;unsigned __ibm_bios_serialcom(unsigned __service,unsigned __port,unsigned __data);

        xdefp   "C",__ibm_bios_equiplist
        xdefp   "C",__ibm_bios_keybrd
        xdefp   "C",__ibm_bios_memsize
        xdefp   "C",__ibm_bios_printer
        xdefp   "C",__ibm_bios_serialcom

        defp    __ibm_bios_equiplist
;
        int     11h             ; get bios equipment list
        ret                     ; return
        endproc __ibm_bios_equiplist

        defp    __ibm_bios_keybrd
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get function code
endif
        push    EDX             ; save EDX
        mov     AH,AL           ; get service code              12-jun-90
        mov     DH,AH           ; save function code
        int     16h             ; access keyboard services
        _if     e               ; if no character
          and   DH,0Fh          ; - isolate low order func code (18-jul-91)
          cmp   DH,1            ; - if '_KEYBRD_READY' function
          _if   e               ; - then
            sub   EAX,EAX       ; - - indicate no character available
          _endif                ; - endif
        _endif                  ; endif
        pop     EDX             ; restore EDX
        ret                     ; return
        endproc __ibm_bios_keybrd

        defp    __ibm_bios_memsize
;
        int     12h             ; get memory size information
        ret                     ; return
        endproc __ibm_bios_memsize

        defp    __ibm_bios_printer
;
        push    EBX             ; save EBX
ifdef __STACK__
        mov     EAX,8[ESP]      ; get function code
        mov     EDX,12[ESP]     ; get port number
        mov     EBX,16[ESP]     ; get data
endif
        mov     AH,AL           ; get function code
        mov     AL,BL           ; get data
        int     17h             ; access printer services
        mov     AL,AH           ; get status
        and     EAX,000000FFh   ; zero top bytes
        pop     EBX             ; restore EBX
        ret                     ; return
        endproc __ibm_bios_printer

        defp    __ibm_bios_serialcom
;
        push    EBX             ; save EBX
ifdef __STACK__
        mov     EAX,8[ESP]      ; get function code
        mov     EDX,12[ESP]     ; get port number
        mov     EBX,16[ESP]     ; get data
endif
        mov     AH,AL           ; get function code
        mov     AL,BL           ; get data
        int     14h             ; access serialcom services
        pop     EBX             ; restore EBX
        ret                     ; return
        endproc __ibm_bios_serialcom


        endmod
        end
