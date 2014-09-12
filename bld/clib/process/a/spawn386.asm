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


        include mdef.inc
        include msdos.inc
        include struct.inc

        name    dospawn
        xrefp   "C",_dosretax

_TEXT   segment byte public 'CODE'
_TEXT   ends

_DATA   segment word public 'DATA'
_DATA   ends

DGROUP  group   _DATA
        assume  cs:_TEXT, ds: DGROUP, ss: DGROUP, es:nothing

_DATA   segment

l_block struc
l_envp  dp      ?                       ; pointer to environment
l_cmdline dp    ?                       ; pointer to command line
;;; The following are required by Intel CodeBuilder
l_fcb   dd      ?                       ; pointer to first FCB
l_res1  dw      ?                       ; reserved
l_fcb2  dd      ?                       ; pointer to second FCB
l_res2  dw      ?                       ; reserved
l_loadptr dd    ?                       ; Overlay: load address
l_reloc dd      ?                       ; Overlay: relocation factor
l_block ends

exeparm l_block <?,?>                   ; parm block for load

        extrn   "C",errno:dword
        extrn   __child:dword
        extrn   "C",_osmajor:byte
save_sp dd      ?               ; ESP
save_ss dw      ?               ; SS
save_ds dw      ?               ; DS
_DATA   ends

_TEXT   segment

        public  "C", _dospawn


        defp    _dospawn
        push    ebp
        mov     ebp,esp                 ; gain access to parms

frame   struc                           ; stack frame
savedbp dd      ?                       ;
retaddr dd      ?                       ;
mode    dd      ?                       ; 1 =>
path    dd      ?                       ; path of program to load
cmdline dd      ?                       ; command line to pass to child
envp    dd      ?                       ; environment to pass to child
frame   ends

        push    esi                     ; save all the registers
        push    edi                     ; . . .
        push    edx                     ; . . .
        push    ecx                     ; . . .
        push    ebx                     ; . . .
        push    es                      ; . . .
        push    ds                      ; . . .
        mov     ax,ss                   ; set ds=ss
        mov     ds,ax                   ; . . .
ifdef ERGO_DPMI
        sub     eax,eax                 ; set environment parm to 0
        mov     dword ptr exeparm.l_envp,eax ;
        mov     word ptr exeparm.l_envp+4,ax ; set segment
else
        mov     eax,envp[ebp]           ; get address of environment
        mov     dword ptr exeparm.l_envp,eax ;
        mov     word ptr exeparm.l_envp+4,ds ; set segment
        sub     eax,eax                 ; set all other fields to 0 for CB
        mov     dword ptr exeparm.l_fcb,eax
        mov     dword ptr exeparm.l_fcb2,eax
        mov     word ptr exeparm.l_res1,ax
        mov     word ptr exeparm.l_res2,ax
        mov     dword ptr exeparm.l_loadptr,eax
        mov     dword ptr exeparm.l_reloc,eax
endif
        mov     eax,cmdline[ebp]        ;
        mov     dword ptr exeparm.l_cmdline+0,eax  ;
        mov     word ptr exeparm.l_cmdline+4,ds  ;
        push    ds
        pop     es
        push    ebp
        push    es
        push    ds
        mov     save_ss,ss              ; save ss
        mov     save_sp,esp             ; save esp
        mov     save_ds,ds              ; save ds
        mov     ebx,offset DGROUP:exeparm
        cmp     dword ptr mode[ebp],01h
        _if     e
          mov   al,04h
          xor   ecx,ecx
        _else
          xor   al,al
        _endif
        clc
        push    eax                     ; Allow a ctrl-break to get through?
        callos  kbdstatus               ;
        pop     eax                     ;
        mov     dword ptr __child,0001h
        mov     edx,path[ebp]
        callos  exec
        mov     ss,save_ss              ; - restore SS:SP
        mov     esp,save_sp             ; - . . .
        mov     ds,save_ds              ;
        pop     ds
        mov     dword ptr __child,0000h
        pop     es
        pop     ebp
        _if     nc                      ; If spawn was successful
          callos wait                   ; .. wait for child to complete
        _endif                          ; Endif
        sbb     edx,edx                 ; set dx = status of carry flag
        movzx   eax,ax
ifdef __STACK__
        push    edx
        push    eax
        call    _dosretax
        add     esp,8
else
        call    _dosretax
endif
        pop     ds                      ; restore segment registers
        pop     es                      ;
        pop     ebx                     ; restore registers
        pop     ecx                     ; . . .
        pop     edx                     ; . . .
        pop     edi                     ; . . .
        pop     esi                     ; . . .
        pop     ebp                     ; . . .
        ret                             ; return
_dospawn endp

_TEXT   ends
        end
