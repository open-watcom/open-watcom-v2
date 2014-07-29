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


.386p

                name            adstrap

extrn   DumpRegs        : near

zapscreen       macro   addr,num
                push    ds
                push    eax
                push    037H
                pop     ds
                mov     eax,0B0000H
                add     eax,2*&addr
                mov     byte ptr [eax],060H
                add     byte ptr [eax],&num
                pop     eax
                pop     ds
                endm

reg_group       struc
                regEAX  dd 0
                regEBX  dd 0
                regECX  dd 0
                regEDX  dd 0
                regESI  dd 0
                regEDI  dd 0
                regEBP  dd 0
                regESP  dd 0
                regEIP  dd 0
                regEFL  dd 0
                regCR0  dd 0
                regCR2  dd 0
                regCR3  dd 0
                regDS   dw 0
                regES   dw 0
                regSS   dw 0
                regCS   dw 0
                regFS   dw 0
                regGS   dw 0
reg_group       ends

idtentry        struc
                loffs   dw      0
                select  dw      0
                wcount  db      0
                arights db      0
                hoffs   dw      0
idtentry        ends

intvars         macro           name,num
i&num&off       dd              0
i&num&seg       dw              0
old_eip_&num    dd              0
old_cs_&num     dw              0
                endm

allexceptions   macro   macname
                &macname zerodiv,0
                &macname tracepoint,1
                &macname breakpoint,3
                &macname boundscheck,5
                &macname invalidop,6
;               &macname doublefault,8          ; timer ???
                &macname badtss,10
                &macname segnotpresent,11
                &macname stackfault,12
                &macname gpfault,13
                endm

_data segment word public 'data'
dbgregs         reg_group       <>
                allexceptions   intvars
dataseg         dw              0
FirstTime       db              1
extrn           IntNum          : dword
extrn           Regs            : byte
_data ends


dgroup group _data

assume  ds:dgroup,cs:_text

_text segment byte public 'code'

saveregs        macro   regs,retoff,retseg
                push    ds                      ; ...
                mov     ds,cs:dataseg           ; get addressability
                pushfd
                pop     &regs.regEFL            ; save flags
                mov     &regs.regEAX,eax        ; save one reg
                pop     eax
                mov     &regs.regDS,ax          ; restore some regs
    ifidn       <retoff>,<POP>
                pop     &regs.regEIP
                mov     ax,cs
    else
                mov     eax,&retoff
                mov     &regs.regEIP,eax
                mov     ax,&retseg
    endif
                mov     &regs.regCS,ax
                mov     &regs.regEBX,ebx        ; save the registers
                mov     &regs.regECX,ecx        ; save the registers
                mov     &regs.regEDX,edx        ; save the registers
                mov     &regs.regESI,esi        ; save the registers
                mov     &regs.regEDI,edi        ; save the registers
                mov     &regs.regESP,esp        ; save the registers
                mov     &regs.regEBP,ebp        ; save the registers
                mov     &regs.regES,es          ; save the registers
                mov     &regs.regSS,ss          ; save the registers
                mov     &regs.regFS,fs          ; save the registers
                mov     &regs.regGS,gs          ; save the registers
                endm

setregs         macro   regs
                mov     ebx,&regs.regEBX
                mov     ecx,&regs.regECX
                mov     edx,&regs.regEDX
                mov     esi,&regs.regESI
                mov     edi,&regs.regEDI
                mov     ebp,&regs.regEBP
                mov     es,&regs.regES
                mov     fs,&regs.regFS
                mov     gs,&regs.regGS
                mov     eax,&regs.regESP
                mov     ss,&regs.regSS
                mov     esp,eax
                push    &regs.regEFL
                push    &regs.regCS
                push    &regs.regEIP
                mov     eax,&regs.regEAX
                mov     ds,&regs.regDS
                iretd
                endm

public          DumpDbgRegs
DumpDbgRegs     proc            near
                push            offset dbgregs
                call            DumpRegs
                add             esp,4
                ret
DumpDbgRegs     endp

handler         macro   name,num
public          hdl_&num
hdl_&num        proc far
                saveregs        Regs,old_eip_&num,old_cs_&num
                mov             IntNum,&num
                setregs         dbgregs
hdl_&num        endp
                endm

                allexceptions   handler

public          DoRunProg
DoRunProg       proc far
                saveregs        dbgregs,POP,cs
                call            DumpDbgRegs
                setregs         Regs
DoRunProg       endp

intproc         macro   name,num
                db      "WVIDEO!!"
int&num         proc    near
                sti
                push    ds                      ; save regs
                push    eax                     ; ...
                mov     ds,cs:dataseg           ; get addressability
                mov     eax,0ch+8[esp]          ; get original interrupt eip
                mov     old_eip_&num,eax        ; stash it
                mov     eax,10h+8[esp]          ; get original interrupt cs
                mov     old_cs_&num,ax          ; stash it
                mov     eax,offset hdl_&num     ; set up to return to handler
                mov     0ch+8[esp],eax          ; ...
                xor     eax,eax                 ; ...
                mov     ax,cs                   ; ...
                mov     10h+8[esp],eax          ; ...
                mov     eax,0100h               ; ... turn of T bit
                not     eax                     ; ...
                and     14h+8[esp],eax          ; ...
                pop     eax                     ; restore regs
                pop     ds                      ; ...
                iretd                           ; return to hdl_&num
int&num         endp
                endm

                allexceptions   intproc

public          GrabVects
GrabVects       proc near
                push    ecx                     ; save registers
                push    ebx                     ; ...
                push    edx                     ; ...
                push    es                      ; ...
                push    ds                      ; ...
                mov     dataseg,ds              ; stash ds for addressability

setvect         macro   name,num
                mov     ax,2532h                ; remember old int3
                mov     cl,&num                 ; ...
                int     21h                     ; ...
                mov     i&num&off,ebx           ; ...
                mov     i&num&seg,es            ; ...

                push    ds                      ; set new int3
                mov     ax,cs                   ; ...
                mov     ds,ax                   ; ...
                lea     edx,int&num             ; ...
                mov     ax,2533h                ; ...
                mov     cl,&num                 ; ...
                int     21h                     ; ...
                pop     ds                      ; ...
                endm

                allexceptions   setvect

                mov     eax,1                   ; return true
                pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
GrabVects       endp

public          ReleVects
ReleVects       proc near
                push    ecx                     ; save registers
                push    edx                     ; ...
                push    ebx                     ; ...
                push    ds                      ; ...
                push    es                      ; ...

oldvect         macro   name,num
                mov     ds,cs:dataseg           ; get addressability
                mov     edx,i&num&off           ; restore old int3 handler
                mov     ds,i&num&seg            ; ...
                mov     ax,2533h                ; ...
                mov     cl,&num                 ; ...
                int     21h                     ; ...
                endm

                allexceptions   oldvect

                pop     es                      ; restore registers
                pop     ds                      ; ...
                pop     ebx                     ; ...
                pop     edx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
ReleVects      endp

public          GetFL
GetFL           proc    near
                pushfd
                pop     eax
                ret
GetFL           endp

public          GetLinear
GetLinear       proc    near
                push    ebx             ; save regs
                push    ecx             ; ...
                mov     ebx,12[esp]     ; load selector
                mov     ax,2508H        ; get segment linear base address
                int     21h             ; ...
                add     ecx,16[esp]     ; add offset to base address
                mov     eax,ecx         ; return it
                pop     ecx
                pop     ebx
                ret
GetLinear       endp


public          SegLimit
SegLimit        proc    near
                lsl     eax,word ptr 4[esp]; get segment limit of selector
                jne     nogood          ; if good selector
                ret                     ; - return segment limit
nogood:         xor     eax,eax         ; else
                ret                     ; - return( 0 )
SegLimit        endp

public          DoReadMem
DoReadMem       proc    near
                mov     ax,4[esp]       ; load segment
                verr    ax              ; if not ok for read
                je      readok          ; - then
                xor     eax,eax         ; - return( 0 )
                ret                     ; - ...
readok:         push    ebx             ; save regs
                push    edx             ; ...
                push    ds              ; ...
                mov     ds,16[esp]      ; load seg
                mov     edx,20[esp]     ; load offset
                mov     ebx,24[esp]     ; address of buffer
                mov     al,[edx]        ; get byte at seg:offset
                pop     ds              ; restore reg
                mov     [ebx],al        ; save byte in buffer
                pop     edx             ; restore regs
                pop     ebx             ; ...
                mov     eax,1           ; return( 1 )
                ret                     ; ...
DoReadMem       endp

public          WriteOk
WriteOk         proc    near
                mov     ax,4[esp]       ; load segment
                verw    ax              ; if ok for write
                sete    al              ; return TRUE
                movzx   eax,al          ; ...
                ret                     ; ...
WriteOk         endp


public          DoWriteMem
DoWriteMem      proc    near
                mov     ax,4[esp]       ; load segment
                verw    ax              ; if not ok for read
                je      writeisok       ; - then
                xor     eax,eax         ; - return( 0 )
                ret                     ; - ...
writeisok:      push    edx             ; save regs
                push    ds              ; ...
                mov     edx,20[esp]     ; get address of buffer
                mov     al,[edx]        ; load byte from buffer
                mov     ds,12[esp]      ; get seg
                mov     edx,16[esp]     ; load offset
                mov     [edx],al        ; store byte at seg:offset
                pop     ds              ; restore regs
                pop     edx             ; ...
                mov     eax,1           ; return( 1 )
                ret                     ; ...
DoWriteMem      endp

sysregs         macro   write
                push    ebx                     ; save regs
                push    edx                     ; ...
                mov     edx,12[esp]             ; get buffer address
                mov     ax,02535h               ; system call
                mov     ebx,&write              ; read/write
                int     21h                     ; ...
                pop     edx                     ; restore regs
                pop     ebx                     ; ...
                ret                             ; return to caller
                endm

public          GetSysRegs
GetSysRegs      proc near
                sysregs 0
GetSysRegs      endp

public          SetSysRegs
SetSysRegs      proc near
                sysregs 1
SetSysRegs      endp

_text           ends

                end
