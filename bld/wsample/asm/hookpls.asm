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


.386
include mdef.inc
include struct.inc

dgroup  group           _data
        modstart        hook,word
        assume          ds:dgroup,ss:dgroup

        xdefp   __PRO
        xdefp   __EPI

;
;       Every time a C function is called, space is set aside on the
;       stack and __PRO is called.  __PRO uses this stack to maintain a
;       stack containing the current callgraph information.  If this is
;       the first call to __PRO and a special environment variable is set
;       to indicate the presence of the sampler, an int 3 is issued which
;       tells the sampler where to find the communications area containing
;       information on where to find the callgraph information.
;

        defp    __PRO
        push    EBP                      ; save registers
        mov     EBP,ESP
        push    EAX
        mov     AH,1                    ; turn sampler off
        xchg    AH,byte ptr in_hook
        push    EBX                     ; save more registers
        push    ESI
        mov     ESI,0[EBP]              ; caller's EBP
        mov     EAX,4[EBP]              ; top_ip = ip in function
        mov     dword ptr top_ip,EAX
        mov     word ptr top_cs,CS      ; top_cs = cs
        lea     EBX,8[EBP]
        mov     EAX,dword ptr cgraph_top
        mov     dword ptr [EBX],EAX     ; storage->ptr = cgraph_top
        mov     word ptr 4[EBX],CS      ; storage->cs = cs
        mov     EAX,4[ESI]
        mov     dword ptr 6[EBX],EAX    ; storage->ip = caller's ip
        inc     word ptr push_no        ; push_no++
        mov     dword ptr cgraph_top,EBX; cgraph_top = storage area
        cmp     byte ptr first_time,0
        jne     first_pass              ; pass common area if first entry
end_pro:
        pop     ESI
        pop     EBX
        xor     AH,AH
        xchg    AH,byte ptr in_hook     ; start sample collection again
        pop     EAX
        pop     EBP
        ret
first_pass:
        push    ECX                     ; save more registers
        push    EDX

;
; now check if our special environment variable is set (ie. the sampler
; is present).  If it is, execute this code to pass the common region to
; the sampler
;

; **************** ADD CODE HERE! **************************

        mov     EBX,offset DGROUP:in_hook
        mov     CX,DS                   ; pass common area address in CX:EBX
        xor     DX,DX                   ; set DX:EAX to zero, to distinguish
        xor     EAX,EAX                 ;     this int 3 from a mark
        int     3                       ; call sampler with common info
;

        mov     byte ptr first_time,0   ; turn off first_time flag
        pop     EDX
        pop     ECX
        jmp     short end_pro
        endproc __PRO

;
;       Just before each function return, __EPI is called.  __EPI shrinks
;       the callgraph stack by one entry, updating the necessary pointers.
;       Control is returned to the program which de-allocates the no longer
;       used stack storage space and returns.
;

        defp    __EPI
        push    EBP                     ; save registers
        mov     EBP,ESP
        push    EAX
        mov     AH,1                    ; don't collect samples in __EPI
        xchg    AH,byte ptr in_hook
        push    EDX                     ; save EDX too
        cmp     word ptr push_no,0      ; if push_no == 0
        _if     e                       ; then
        inc     word ptr pop_no         ;     pop_no++;
        _else                           ; else
        dec     word ptr push_no        ;     push_no--;
        _endif                          ; endif
        mov     EDX,dword ptr cgraph_top; top_cs = cgraph_top->cs
        mov     AX,word ptr 4[EDX]
        mov     word ptr top_cs,AX
        mov     EAX,dword ptr 6[EDX]    ; top_ip = cgraph_top->ip
        mov     dword ptr top_ip,EAX
        mov     EAX,dword ptr [EDX]     ; cgraph_top = cgraph_top->ptr
        mov     dword ptr cgraph_top,EAX
        pop     EDX                     ; restore EDX
        xor     AH,AH                   ; start collecting samples again
        xchg    AH,byte ptr in_hook
        pop     EAX                     ; restore some more registers
        pop     EBP
        ret
        endproc __EPI

        dataseg
storage label   byte
        dd      0               ; ptr to next storage
        dw      0               ; cs of calling address
        dw      0               ; ip of calling address

        db      0, 0, 0         ; alignment junk

; The following variables are shared with the sampler, by means
; of an int 3 on the first pass through __PRO.  On writing a
; sample, the sampler is responsible for resetting pop_no and
; push_no to 0.

in_hook db      0               ; flag: in __PRO/__EPI?
cgraph_top dd   0               ; ptr to top of callgraph stack
top_ip  dd      0               ; ip of most recently called function
top_cs  dw      0               ; cs of most recently called function
pop_no  dw      0               ; # of entries to pop from prev sample
push_no dw      0               ; # of entries to push from prev sample

first_time label byte
        db      1               ; flags first time through __PRO
ENV_VAR label   byte
        db      43h,20h,4dh,20h,53h,20h,00h     ; env var set when sampling
        enddata

        endmod

        end
