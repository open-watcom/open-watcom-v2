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


.8087

include osdef.inc
include mdef.inc
include struct.inc

.286c
extrn   DosExit         : far
extrn   DosWrite        : far
extrn   DosReallocSeg   : far

xrefp   main_

                modstart startup

public      _cstart_
_cstart_    label word
public      __Null_Argv_
__Null_Argv_    label word

FAR_DATA segment byte public 'FAR_DATA'
stk     db 800h dup(?)
endstk      label   word
FAR_DATA ends

DGROUP group CONST,_DATA,STRINGS,STACK,_BSS

CONST segment word public 'DATA'
CONST ends

STRINGS segment word public 'DATA'
STRINGS ends

assume ds:DGROUP,es:DGROUP,ss:DGROUP
_DATA segment word public 'DATA'
stackp  dd  stk
_DATA ends

_BSS          segment word public 'BSS'
_BSS          ends

STACK segment para stack 'STACK'

;tk             db 800h dup(?)

STACK ends


;==========================================================================
;   void Start()    - OS entry point
;==========================================================================
        xdefp   Start_
        defp    Start_
        mov     dx,ds                   ; point es at data seg
        mov     es,dx                   ; ...
    cld             ; direction bit!
    mov ax,seg stk      ; set stack pointer
    mov ss,ax           ; ...
    lea sp,endstk       ; ...
    xor ax,ax           ; ...
        docall  main_           ; ...
        endproc Start_

;==============================================================================
;       void ExitSys(int) - exit to OS
;==============================================================================
        xdefp   ExitSys_
        defp    ExitSys_
        push    1                       ; terminate process
        push    ax                      ; exit code
        call    DosExit                 ; say gnite, gracey.
        endproc ExitSys_


        endmod Start_
