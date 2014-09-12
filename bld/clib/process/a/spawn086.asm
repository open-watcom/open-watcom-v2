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
        xrefp   __dosretax
        extrn   __close_ovl_file    :dword
if _MODEL eq _SMALL
        extrn   __GETDS         :near
endif

_TEXT   segment byte public 'CODE'
_TEXT   ends

_DATA   segment word public 'DATA'
_DATA   ends

DGROUP  group   _DATA
        assume  cs:_TEXT, ds: DGROUP, ss: DGROUP, es:nothing

_DATA   segment

l_block struc
l_envp  dw      ?                       ; segment of environment
l_cmdline dd    ?                       ; pointer to command line
l_fcb1  dd      ?                       ; pointer to default FCB 1
l_fcb2  dd      ?                       ; pointer to default FCB 2
l_block ends

fcb1    db      16 dup(?)               ; room for FCB 1
fcb2    db      16 dup(?)               ; room for FCB 2

exeparm l_block <?,?,?,?>               ; parm block for load

        extrn   __child:word
        extrn   "C",_osmajor:byte
save_sp dw      ?               ; SP
save_ss dw      ?               ; SS
save_ptr dd     ?               ; some far pointer
save_ds dw      ?               ; DS
_DATA   ends

_TEXT   segment

        public  __dospawn


        defp    __dospawn
        push    bp
        mov     bp,sp                   ; gain access to parms
if _MODEL and _BIG_CODE

frame   struc                           ; stack frame
savedbp dw      ?                       ;
retaddr dd      ?                       ;
mode    dw      ?                       ; 1 =>
path    dw      ?                       ; path of program to load
cmdline dw      ?                       ; command line to pass to child
envp    dw      ?                       ; environment to pass to child
frame   ends

else

frame   struc                           ; stack frame
savedbp dw      ?                       ;
retaddr dw      ?                       ;
mode    dw      ?                       ; 1 =>
path    dw      ?                       ; path of program to load
cmdline dw      ?                       ; command line to pass to child
envp    dw      ?                       ; environment to pass to child
frame   ends

endif
        push    si                      ; save all the registers
        push    di                      ; . . .
        push    dx                      ; . . .
        push    cx                      ; . . .
        push    bx                      ; . . .
        push    es                      ; . . .
        push    ds                      ; . . .
;       mov     ax,ds                   ; get segment of environment
;       mov     bx,envp[bp]             ;
;       mov     cl,04h                  ;
;       shr     bx,cl                   ;
;       add     ax,bx                   ;
        mov     ax,ss                   ; set ds=ss
        mov     ds,ax                   ; . . .
        mov     ax,envp[bp]             ; get segment of environment
        mov     exeparm.l_envp,ax       ;
        mov     ax,cmdline[bp]          ;
        mov     word ptr exeparm.l_cmdline+0,ax  ;
        mov     word ptr exeparm.l_cmdline+2,ds  ;
        mov     word ptr exeparm.l_fcb2+2,ds  ;
        push    ds
        pop     es
        mov     si,ax
        inc     si
        mov     di,offset DGROUP:fcb1
        mov     word ptr exeparm.l_fcb1+0,di  ;
        mov     word ptr exeparm.l_fcb1+2,ds  ;
        mov     al,001h
        callos  fcbparse
        mov     di,offset DGROUP:fcb2
        mov     word ptr exeparm.l_fcb2+0,di  ;
        mov     word ptr exeparm.l_fcb2+2,ds  ;
        mov     al,001h
        callos  fcbparse
        push    bp
        push    es
        push    ds
          mov   save_ss,ss              ; - save ss
          mov   save_sp,sp              ; - save sp
          mov   save_ds,ds              ; - save ds
        cmp     _osmajor,02h
        _if     e                       ; If DOS 2.X
          mov   di,002eh                ; .. and some far pointer
          mov   si,[di]                 ;
          mov   word ptr save_ptr+0,si
          mov   si,+2h[di]              ;
          mov   word ptr save_ptr+2,si
        _endif                          ; Endif
        mov     bx,offset DGROUP:exeparm
        cmp     word ptr mode[bp],01h
        _if     e
          mov   al,04h
          xor   cx,cx
        _else
          xor   al,al
        _endif
        clc
        push    ax                      ; Allow a ctrl-break to get through?
        callos  kbdstatus               ;
        pop     ax                      ;
        mov     word ptr __child,0001h
        mov     dx,path[bp]
        push    ax
        call    dword ptr __close_ovl_file
        pop     ax
        callos  exec
        push    ax
        lahf
        pop     bx
        push    ax                      ; save ax
if _MODEL eq _SMALL
        call    __GETDS                 ; point ds to DGROUP
else
        mov     ax,DGROUP               ; point to DGROUP
        mov     ds,ax                   ; ...
endif
        pop     ax                      ; restore ax
          mov   ss,save_ss              ; - restore SS:SP
          mov   sp,save_sp              ; - . . .
          mov   ds,save_ds              ;
        cmp     _osmajor,02h
        _if     e                       ; If DOS 2.X
          mov   di,002eh                ; .. and some far pointer
          mov   si,word ptr save_ptr+2  ;
          mov   +2h[di],si              ;
          mov   si,word ptr save_ptr+0  ;
          mov   [di],si                 ;
        _endif                          ; Endif
        sahf
        xchg    ax,bx
        pop     ds
        mov     word ptr __child,0000h
        pop     es
        pop     bp
        _if     nc                      ; If spawn was successful
          callos wait                   ; .. wait for child to complete
        _endif                          ; Endif
        sbb     dx,dx                   ; set dx = status of carry flag
        call    __dosretax
        pop     ds                      ; restore segment registers
        pop     es                      ;
        pop     bx                      ; restore registers
        pop     cx                      ; . . .
        pop     dx                      ; . . .
        pop     di                      ; . . .
        pop     si                      ; . . .
        pop     bp                      ; . . .
        ret                             ; return
__dospawn endp

_TEXT   ends
        end
