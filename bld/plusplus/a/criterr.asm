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


;
; CRITERR:      Critical Error handler for 386
;
        name    criterr

        public  InitCritErr_

CRITICAL_INT    =       24h
;
;       The Critical Error Interrupt Handler must be located in Real Memory
;
rm_code   segment word public 'CODE' use16
                assume  cs:rm_code

CriticalErrorInterrupt  proc    far
        sub     al,al           ; ignore critical error
        iret
CriticalErrorInterrupt  endp

rm_code ends

;
;       The Initialization routine is in Protected Memory
;
pm_code   segment para public 'CODE' use32
                assume  cs:pm_code

InitCritErr_   proc  near
        push    ebx                     ; save registers
        push    ecx                     ; ...
        push    es                      ; ...
        lea     ebx,CriticalErrorInterrupt ; get address of interrupt rtn
        mov     ecx,20                  ; length in ECX
        mov     ax,cs                   ; set ES=CS
        mov     es,ax                   ; ...
        mov     ax,250fh                ; convert protected mode address
        int     21h                     ; ... to real-mode address
        mov     eax,-1                  ; assume error
        jc      error                   ; if no error
          mov   ebx,ecx                 ; - get the real-mode address
          mov   cl,CRITICAL_INT         ; - interrupt 24 is critical error
          mov   ax,2505h                ; - set interrupt for real-mode
          int   21h                     ; - ...
          sub   eax,eax                 ; - indicate success
error:; endif                           ; endif
        pop     es                      ; restore ES
        pop     ecx                     ; restore registers
        pop     ebx                     ; ...
        ret                             ; return
InitCritErr_   endp

pm_code ends

        end
