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


EXTRN           getenv_                 :NEAR
EXTRN           __psp                   :WORD
EXTRN           _Envptr                 :FWORD


cr_char         equ     13                      ;carriage return ASCII code
extralen        equ     3                       ;# of extra chars added to cmd
off_fcb1        equ     5ch                     ;offset of 1st fcb addr in a PSP
off_fcb2        equ     6ch                     ;offset of 2nd fcb addr in a PSP
cmdprefix       equ     ' C'                    ;prefix added to command string
errflag         equ     0ffffh                  ;flag signaling error on fork
envseg_off      equ     2ch


PARMBLOC        STRUC                           ;needed to exec child process
envseg          dw      ?                       ;seg of environment
cmdptr          dd      ?                       ;ptr to command line
fcb1ptr         dd      ?                       ;ptr to 1st fcb
fcb2ptr         dd      ?                       ;ptr to 2nd fcb
PARMBLOC        ENDS

DGROUP GROUP CONST, _BSS

CONST           SEGMENT BYTE PUBLIC 'DATA'
Comspec         db 'comspec',0
CONST           ENDS

_BSS            SEGMENT BYTE PUBLIC 'BSS'
StdinHandle     dw ?
StdoutHandle    dw ?
ConHandle       dw ?
Fcb1            db 1000 dup(?)
Fcb2            db 1000 dup(?)
_BSS            ENDS

_TEXT           SEGMENT BYTE PUBLIC 'CODE'
ASSUME  cs:_TEXT, ds:DGROUP, ss:DGROUP
;on entry to load_and_exec_ :
;       bx      points to the command string to execute
;       dx      holds len of the cmd string (but we assume len is all in dl)
;on load_and_exec exit :
;       ax      holds a return code (0 if successfull)
load_and_exec_  PROC NEAR
                push    bp
                push    di
                push    si
                push    dx
                push    cx
                push    bx
                push    es
                mov     bp, sp

                mov     di, ss
                mov     es, di
                sub     sp, dx                  ;make room for child PSP cmdtail
                dec     sp                      ;make room for terminating char
                mov     di, sp
                mov     si, bx                  ;now esi points to cmd string
                mov     cx, dx                  ;now ecx has len of cmd string
                rep     movsb                   ;copy cmd string to PSP cmdtail
                mov     byte ptr es:[di], cr_char       ;store terminating char

                test    dx, dx                  ;if there is no cmd to execute
                jz      no_cmdline
                mov     cx, dx                  ;cmdstr len in ecx (cl really)
                mov     ax, 3700h               ;get switch char
                int     21h                     ;switch char now in dl
                mov     bx, cmdprefix
                push    bx
                mov     bl, cl
                add     bl, extralen            ;now bl holds len cmd str + 3
                mov     bh, dl                  ;now ebx holds cmdtail prefix
                push    bx                      ;add the prefix to the cmdtail
                jmp     make_parmbloc

no_cmdline:     dec     sp                      ;make room for length
                mov     di, sp
                mov     byte ptr ss:[di], 0     ;len=0

make_parmbloc:  mov     di, sp                  ;save addr of PSP cmdtail

                sub     sp, SIZE PARMBLOC
                mov     bx, sp                  ;now ebx points to parm block
                mov     ax, word ptr _Envptr+4
                ;xor    ax, ax
                mov     ss:[bx].envseg, ax
                mov     word ptr ss:[bx].cmdptr+2, ss
                mov     word ptr ss:[bx].cmdptr, di
                mov     ax, [__psp]             ;now our PSP is at ax:0

                ;mov    [bx].fcb1seg, ax        ;use our own fcb#1 as the 1st
                ;mov    [bx].fcb1offset, off_fcb1       ;fcb in the child PSP
                ;mov    [bx].fcb2seg, ax        ;use our own fcb#2 as the 2nd
                ;mov    [bx].fcb2offset, off_fcb2       ;fcb in the child PSP

                mov     word ptr ss:[bx].fcb1ptr+2, ds
                mov     word ptr ss:[bx].fcb1ptr, OFFSET DGROUP:Fcb1
                mov     word ptr ss:[bx].fcb2ptr+2, ds
                mov     word ptr ss:[bx].fcb2ptr, OFFSET DGROUP:Fcb2

                mov     ax, OFFSET DGROUP:Comspec
                call    getenv_
                mov     dx, ax
                mov     ax, ss
                mov     es, ax
                mov     ax, 4b00h               ;load/exec program
                int     21h
                jc      loadexec_done
                xor     ax, ax                  ;success

loadexec_done:  clc                             ;flag was set if there was err
                mov     sp, bp
                pop     es
                pop     bx
                pop     cx
                pop     dx
                pop     si
                pop     di
                pop     bp
                ret
load_and_exec_  ENDP

;on entry to _fork_( char *cmd, unsigned len ) :
;       ax      points to the command string to execute
;       dx      holds len of the cmd string (but we assume len is all in dl)
;on _fork_ exit :
;       ax      holds a return code
                PUBLIC _fork_
_fork_          PROC NEAR
                push    bx

                mov     bx, ax                  ;put command string ptr in ebx
                call    load_and_exec_

fork_done:      test    ax, ax
                jz      L1                      ;if ax is 0 then no errors
                or      ax, errflag             ;else signal error in ret code
L1:             pop     bx
                ret
_fork_          ENDP
_TEXT           ENDS

                END
