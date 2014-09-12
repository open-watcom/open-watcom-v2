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
;* Description:  doexec implementation for 16-bit DOS
;*
;*****************************************************************************


include msdos.inc
include mdef.inc
include struct.inc

        name    doexec

        extrn   execve_         :proc
        extrn   __init_execve_  :proc
        extrn   __close_ovl_file:dword

_TEXT   segment byte public 'CODE'
_TEXT   ends

include xinit.inc

        xinit   __init_execve_,DEF_PRIORITY

        assume  cs: _TEXT, ds: DGROUP, ss: DGROUP, es: DGROUP

CR      =       00dh                    ; ASCII carriage return
NL      =       00ah                    ; ASCII line feed

common_code macro
        local   beg,badfmt,badenv,nomem
beg:
        push    cs                      ; Set up ds = cs.
        pop     ds                      ;
        callos                          ; Actually perform the load.
        _if     c                       ; If it failed,
          _guess                        ; .. determine appropriate message,
            cmp         al,E_ifunc      ;
            _quif       ne              ;
            mov         dx,badfmt-beg   ;
          _admit                        ;
            cmp         al,E_badfmt     ;
            _quif       ne              ;
            mov         dx,badfmt-beg   ;
          _admit                        ;
            cmp         al,E_badenv     ;
            _quif       ne              ;
            mov         dx,badenv-beg   ;
          _admit                        ;
            mov         dx,nomem-beg    ;
          _endguess                     ;
          callos message                ; .. print it out,
          mov   al,-1                   ; .. and terminate with a return code
          callos terminate              ; .. of -1.
badfmt    db    'Bad format on exec',CR,NL,'$'
badenv    db    'Bad environment on exec',CR,NL,'$'
nomem     db    'Not enough memory on exec',CR,NL,'$'
        _endif                          ; Endif.
        endm

        _TEXT   segment
fake    proc    far                     ; Force returns to be far returns.
exebeg:
        common_code                     ; Do load, check for errors.

        mov     si,newss-exebeg         ;
        lodsw                           ; Set up the new stack.
        mov     dx,ax                   ;
        lodsw                           ;
        mov     ss,dx                   ;
        mov     sp,ax                   ;
        lodsw                           ; Set up new cs:ip by faking up an
        push    ax                      ; address for a far return.
        lodsw                           ;
        push    ax                      ;
        push    es                      ; Set up new ds.
        pop     ds                      ;
        ret                             ; Start execution of loaded program.

newss   dw      ?                       ; Save area for ss,
newsp   dw      ?                       ; sp,
newcs   dw      ?                       ; cs,
newip   dw      ?                       ; and ip.
exeend:

combeg:
        common_code                     ; Do load, check for errors.

        mov     bx,cs                   ; Set up the stack for the load module
        mov     ax,es                   ; just below the loader (The loader is
        sub     bx,ax                   ; as high as possible in memory.)
        cmp     bx,1000h                ; Since ss = ds = cs,
        _if     a                       ; If more than 4k (64k bytes) avail
          mov   bx,1000h                ; .. limit ourselves to 4k
        _endif                          ; Endif.
        mov     cl,4                    ; Convert from paragraphs to bytes.
        shl     bx,cl                   ;
        dec     bx                      ;
        dec     bx                      ;
        mov     word ptr es:[bx],0000h  ; es:bx will be the top stack word.
        mov     ss,ax                   ; Set up new stack.
        mov     sp,bx                   ;
        push    es                      ; Set up new cs:ip by faking up an
        mov     ax,100h                 ; address for a far return.
        push    ax                      ;
        push    es                      ; Set up new ds.
        pop     ds                      ;
        mov     ax,bp                   ; ah, al == 0ffh iff fcb1, 2 are valid.
        ret                             ; Start execution of loaded program.
comend:
fake    endp
_TEXT   ends

psplen  =       100h                    ; length of psp
max_para =      (comend-combeg+80h+15)/16; .COM loader is bigger, 80h stack

        _DATA   segment

        extrn   "C",_psp:word
        public  ___p_overlay
___p_overlay  dw        2

        public  ___exec_para
___exec_para dw max_para                ; paragraphs needed for loader

loader  label   dword                   ; far pointer to loader
loaderip dw     0                       ; ip always 0
loadercs dw     ?                       ; cs will be filled in

pathlen dw      ?                       ; length of path

_DATA   ends

        _TEXT   segment
;
;       void _doexec( path, cmdline, exe, exess, exesp, execs, exeip )
;
        public  __doexec
__doexec proc    far

        mov     bp,sp                   ; gain access to parms

s_frame struc                           ; define stack frame
        dd      ?                       ; return address
path    dw      ?                       ; pointer to path
cmdline dw      ?                       ; pointer to command line
isexe   dw      ?                       ; 1 => .EXE, 0 => .COM
exess   dw      ?                       ; ss offset for .EXE
exesp   dw      ?                       ; sp value for .EXE
execs   dw      ?                       ; cs offset for .EXE
exeip   dw      ?                       ; ip value for .EXE
s_frame ends

        mov     ax,ss                   ; set ds=ss
        mov     ds,ax                   ; . . .
        mov     ax,_psp                 ;
        mov     es,ax                   ;
        xor     al,al                   ; Zero out FCB's 1 and 2.
        mov     di,5ch                  ;
        mov     cx,DOS_cmdline-5ch      ;
        rep     stosb                   ;
        mov     si,cmdline[bp]          ; Copy over command line.
        mov     cl,0[si]                ;
        inc     cx                      ; +1 to include the length byte
        inc     cx                      ; +1 more to get the trailing CR
        cmp     cx,128                  ; if too long
        _if     a                       ; then
          mov   cx,128                  ; - set to maximum
        _endif                          ; endif
        rep     movsb                   ;

        push    ds                      ; Save ds.
        mov     ax,es                   ;
        mov     ds,ax                   ;
        mov     dx,DOS_cmdline          ; Set Disk Transfer Address
        callos  setDMA                  ;
        mov     di,5ch                  ; Set up FCB 1.
        mov     si,DOS_cmdline+1        ;
        mov     al,01h                  ;
        callos  fcbparse                ;
        cmp     al,0ffh                 ;
        _if     ne                      ; If succeeded
          xor   al,al                   ; .. set flag to 0
        _endif                          ; Endif
        mov     bl,al                   ; **** Saved in bl for later.
        mov     di,6ch                  ; Set up FCB 2.
        mov     al,01h                  ;
        callos  fcbparse                ;
        cmp     al,0ffh                 ;
        _if     ne                      ; If succeeded
          xor   al,al                   ; .. set flag to 0
        _endif                          ; Endif
        mov     bh,al                   ; **** Saved in bh for later.
        pop     ds                      ; Restore ds.

        mov     ax,ds                   ;
        mov     es,ax                   ;
        mov     di,path[bp]             ; Calculate length of path.
        xor     cx,cx                   ;
        not     cx                      ; cx = 0ffffh
        xor     al,al                   ;
        repne   scasb                   ;
        not     cx                      ; cx = length
        mov     pathlen,cx              ;
        mov     dx,cx                   ;
        add     dx,15                   ; Convert to para.
        mov     cl,4                    ;
        shr     dx,cl                   ;
        mov     es,_psp                 ; Calculate where we should put loader.
        mov     ax,es:DOS_maxpara       ; top of block
        sub     ax,dx                   ; account for path
        sub     ax,max_para             ; and for loader and stack
        mov     es,ax                   ;
        mov     loadercs,ax             ; Save for eventual jump to loader.
        xor     di,di                   ; Put loader at offset 0.
        cmp     word ptr isexe[bp],0    ;
        _if     ne                      ; If exe file
          mov   si,offset exebeg        ; .. use .EXE file loader
          mov   cx,exeend-exebeg        ;
        _else                           ; Else
          mov   si,offset combeg        ; .. use .COM file loader
          mov   cx,comend-combeg        ;
        _endif                          ; Endif.
        push    ds                      ;
        mov     ax,cs                   ;
        mov     ds,ax                   ;
        rep     movsb                   ; Move loader into high memory.
        pop     ds                      ;
        mov     di,max_para*16          ;
        mov     si,path[bp]             ;
        mov     cx,pathlen              ;
        rep     movsb                   ; Move path into high memory.
        mov     cx,_psp                 ; Calculate relocation factor for load.
        add     cx,psplen/16            ;
        cmp     word ptr isexe[bp],0    ;
        _if     ne                      ; If we have an EXE file
          mov   ax,exess[bp]            ; .. fix up and save the new ss,
          add   ax,cx                   ;
          mov   word ptr es:[newss-exebeg],ax;
          mov   ax,exesp[bp]            ; .. sp,
          mov   word ptr es:[newsp-exebeg],ax;
          mov   ax,execs[bp]            ; .. cs,
          add   ax,cx                   ;
          mov   word ptr es:[newcs-exebeg],ax;
          mov   ax,exeip[bp]            ; .. and ip.
          mov   word ptr es:[newip-exebeg],ax;
        _endif                          ; Endif.
        call    dword ptr __close_ovl_file; close overlay file - if open
        mov     bp,bx                   ; Get flags for FCBs into bp.
        mov     ax,es                   ; Switch to loader stack.
        mov     dx,max_para*16          ; Pointer to path.
        mov     ss,ax                   ;
        mov     sp,dx                   ; Stack just below path.
        mov     es,_psp                 ; es points to mem block to be loaded.
        mov     bx,psplen               ;
        mov     es:0[bx],cx             ; Segment where file will be loaded.
        mov     es:2[bx],cx             ; Relocation to be applied to image.
        mov     ax,04b03h               ; Load overlay function.
        jmp     loader
__doexec endp
_TEXT   ends

        end
