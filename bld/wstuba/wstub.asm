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
;* Description:  DOS/4GW Loader Stub for 32-bit DOS programs.
;*
;*****************************************************************************

;
; Note that this has to be an EXE program but it in reality
; is a tiny model executable (single code/data/stack segment)
; It is split into several segments to stop the linker complaining
; about missing stack segment and to eliminate a few hundred bytes
; of zeros at the end of the executable.

.8086          ; we don't want to hang those poor ol' XTs

StackSize       EQU     512

ENV_SEG         EQU     2Ch
FCB1            EQU     5Ch
FCB2            EQU     6Ch
PSP_PARAMS      EQU     80h

DGROUP  GROUP   _TEXT,_DATA,_BSS,_STACK

_TEXT   SEGMENT 'CODE'
        ASSUME  CS:DGROUP,DS:DGROUP,ES:DGROUP

;*** Program Entrypoint
main            PROC    FAR
                cld                             ; clear direction flag
                mov     dx,cs
                mov     ds,dx                   ; DS = data segment (equal to CS)
; ES - PSP segment
                mov     cx,es
                mov     PSPSeg,cx               ; store PSP segment
                mov     ax,es:[ENV_SEG]         ; environment segment
                mov     EnvSeg,ax               ; store env segment

                mov     bx,ss
                add     bx,(StackSize + 15) / 16; stack size in paras rounded up
                                                ; BX - new block size in paras
                add     bx,20
                sub     bx,cx
                                                ; ES points to block
                mov     ah,4Ah                  ; DOS Resize memory block
                int     21h
                mov     es,dx                   ; ES = data segment

                call    FindDOS4GW
; DS:DX - DOS4GW path
                call    SetupParms
; ES:BX - parameters
IFDEF QUIET
                call    SetupEnv
ELSE
                xor     ax,ax
ENDIF
; AX - environment segment
                call    Exec

                or      ax,ax
                jnz     ExecError
; Get program exit code
                mov     ah,4Dh
                int     21h
; AL=exit code
                jmp     Terminate

ExecError:      mov     dx,offset Msg_Error

;               DS:DX   error message
;               AL      return code
                mov     ah,9
                int     21h
                mov     al,0FEh                 ; AL - exit code
Terminate:      mov     ah,4Ch
                int     21h                     ; DOS terminate
main            ENDP


;*** Find DOS4GW.EXE or DOS4G.EXE
; Input:        none
; Output:       DS:DX   DOS4G[W] full path (if possible)
; Modify:       AX
FindDOS4GW      PROC    NEAR
; Try DOS4GPATH
                mov     dx,offset EnvDOS4GPATH
                call    FindProgram
                jnc     L1
; Try PATH
                mov     dx,offset EnvPATH
                call    FindProgram
                jnc     L1
; none found -- return current directory
                mov     dx,offset DOS4GW_EXE
L1:             ret
FindDOS4GW      ENDP


;*** Find DOS4GW.EXE or DOS4G.EXE using environment variable
; In:           DS:DX   Environment variable name
; Out:          CF=0    found
;               CF=1    not found
;               DS:DX   DOS4G[W] full path
; Changes:      AX,BX
FindProgram     PROC    NEAR
; Read environment
                call    GetEnv
                mov     bx,ax
                rcl     bx,1            ; Get sign bit into CF
                jc      L2              ; If CF=1, variable wasn't found
; Variable exists
                mov     si,di
                mov     di,offset DOS4GWpathName
                mov     bx,offset DOS4G_EXE     ; Try DOS/4G first
                call    ScanPath
                jnc     L2
                mov     bx,offset DOS4GW_EXE    ; and DOS/4GW next
                call    ScanPath
L2:             ret
FindProgram     ENDP


;*** Get environment string
; In:           ES:DX   environment variable name (must be uppercase,
;                       Pascal style string)
; Out:          DI      offset to variable in env segment (ASCIIZ)
;               AX = string length (-1 if not found)
GetEnv          PROC    NEAR
                push    ds
                mov     si,dx
                lodsb
                xor     cx,cx
                mov     cl,al
                jcxz    NotFound
                                        ; ES:SI - env var name
                mov     ax,EnvSeg
                mov     ds,ax
                mov     bx,-1           ; DS:BX - environment pointer
Scan:           push    cx
                push    si
L3:             lods    BYTE PTR es:[si]
                inc     bx
                cmp     al,[bx]
                loope   L3
                pop     si
                pop     cx
                jne     Next
                inc     bx
                cmp     BYTE PTR [bx],'='
                jne     Next
; variable found -- point ES:DI to it
                lea     di,[bx+1]       ; DI points to env var data
L4:             inc     bx
                mov     al,[bx]
                test    al,al           ; find the closing NULL char
                jnz     L4
                mov     ax,bx
                sub     ax,di           ; calculate string length
                jmp     L6
; skip to next
Next:           xor     al,al
L5:             inc     bx
                cmp     [bx],al      ; look for terminating NULL
                jnz     L5
                cmp     [bx+1],al    ; environment ends with two NULLs
                jnz     Scan
; variable not found
NotFound:       mov     ax,-1
L6:             pop     ds
                ret
GetEnv          ENDP


;*** Get program path from the environment
; In:           ES:DI   destination buffer
; Out:          CX      length
;               [ES:DI] filled
GetPgmPath      PROC    NEAR
                mov     ax,EnvSeg
                push    ds
                mov     ds,ax
                xor     bx,bx           ; DS:BX - environment pointer
                xor     ax,ax
                mov     cx,8000h
L7:             cmp     [bx],ax
                je      L8
                inc     bx
                loop    L7
                jmp     L10             ; not found - this should never happen
; found -- copy it to ES:DI
L8:             add     bx,4            ; skip zeros and count (1)
                xor     cx,cx           ; CX - count
L9:             mov     al,[bx]
                inc     bx
                inc     cx
                stosb
                or      al,al
                jnz     L9
                dec     cx              ; CX=length of the pathname
L10:            pop     ds
                ret
GetPgmPath      ENDP


;*** Scan directories in 'path1;path2;path3...' format for filename
; In:           DS:BX   filename to be found
;               SI      offset to path in env seg
;               DS:DI   destination buffer for full pathname
; Out:          CF=0, AX=0      found
;               CF=1, AX!=0     not found
;               ES:DX = ES:DI   full filename
ScanPath        PROC    NEAR
                push    si
                mov     dx,di           ; ES:DX now points to dest buffer

NextPath:       mov     ax,cs:[EnvSeg]
                mov     ds,ax           ; DS:SI points to the path
                xor     ax,ax
; copy one element of path
L11:            lodsb
                cmp     al,';'                  ; is it a path separator?
                je      L12
                test    al,al                   ; is it end of the string?
                je      L12
                stosb
                inc     ah
                jmp     L11
; add backslash if necessary
L12:            mov     cx,ax                   ; save last char
                test    ah,ah
                jz      L13                     ; AH = 0 -- path element is empty
                mov     al,es:[di-1]
                cmp     al,'\'
                je      L13
                cmp     al,':'
                je      L13
                mov     al,'\'                  ; append backslash
                stosb
; add filename to path
L13:            push    cs                      ; repoint DS to data segment
                pop     ds
                push    si
                mov     si,bx
L14:            lodsb
                stosb
                or      al,al
                jnz     L14
                pop     si
; DS:DX now contains full path
                mov     ax,3D40h                ; open for reading, deny none
                int     21h
                jc      L15                     ; error -- file not found
; file exists, close its handle
                mov     bx,ax
                mov     ah,3Eh
                int     21h
                xor     ax,ax
                jmp     L16
;
L15:            mov     di,dx                   ; DX = saved DI
                or      cl,cl                   ; CL = last char (00h or ';')
                jnz     NextPath
                stc
L16:            pop     si
                ret
ScanPath        ENDP


;*** Set up program parameters
; In:           none
; Out:          ES:BX   parameters
; Changes:      AX
SetupParms      PROC    NEAR
                mov     di,offset ArgBuffer+1
                call    GetPgmPath
                dec     di
; add space if necessary
                mov     si,PSP_PARAMS
                mov     ax,PSPSeg
                push    ds                      ; DS = PSP
                mov     ds,ax
                lodsb
                xor     ah,ah                   ; AX = length of orig. params
                or      ax,ax
                jz      L18
                push    ax
                mov     al,' '
                cmp     [si],al
                je      L17
                stosb
                inc     cx
L17:            pop     ax
; add parameters we were called with
L18:            mov     bx,7Eh                  ; max. length of parameters
                sub     bx,cx
                xchg    bx,cx                   ; BX = length so far
                cmp     cx,ax                   ; CX = remaining bytes
                jbe     L19
                mov     cx,ax                   ; CX = bytes to be copied
L19:            add     bx,cx                   ; BX = total length
                rep     movsb
                mov     al,0Dh
                stosb                           ; add trailing CR
                pop     ds

                mov     al,bl
                mov     bx,offset ArgBuffer
                mov     [bx],al
                ret
SetupParms      ENDP


IFDEF QUIET
;*** Set up environment -- add "DOS4G=QUIET" variable
; In:           None
; Out:          AX      Segment of updated environment
SetupEnv        PROC    NEAR
                push    es
                push    bx
                mov     ax,EnvSeg
                dec     ax
                mov     es,ax
                mov     bx,es:[3]               ; size of current environment
                mov     cl,4
                shl     bx,cl

                inc     ax
                mov     es,ax
                xor     ax,ax
                xor     di,di
L20:            cmp     es:[di],al
                je      L21
                mov     cx,8000H
                repne   scasb
                jmp     L20

L21:            mov     ax,bx
                sub     ax,di
                cmp     ax,EnvDOS4G_Len+5
                jge     L22
; create new environment block
                lea     bx,[di+EnvDOS4G_Len+5+15]
                mov     cl,4
                shr     bx,cl
                mov     ah,48h                  ; allocate memory
                int     21h
                jc      L23                     ; check for error
                push    si                      ; AX - new env segment
                mov     si,EnvSeg
                push    ds
                mov     ds,si
                mov     es,ax                   ; ES - new env segment
                mov     cx,di                   ; DS - old env segment
                xor     di,di
                xor     si,si
                rep     movsb                   ; copy old env seg across
                mov     bx,ds                   ; BX - old env segment
                pop     ds
                pop     si
                mov     EnvSeg,ax               ; AX - new env segment
                push    es
                mov     cx,PSPSeg
                mov     es,cx
                mov     es:[ENV_SEG],ax         ; update env seg in PSP
                mov     es,bx                   ; ES - old env segment
                mov     ah,49H                  ; free old env segment
                int     21h
                pop     es                      ; ES - new env segment
; add string (destroying program load path)
L22:            mov     si,offset EnvDOS4G
                mov     cx,EnvDOS4G_Len
                rep     movsb
                xor     ax,ax
                stosb
                stosw
                stosw
L23:            pop     bx
                pop     es
                mov     ax,EnvSeg
                ret
SetupEnv        ENDP
ENDIF


;*** Run program
; In:           DS:DX   complete filename (ASCIIZ)
;               ES:BX   parameters
;               AX      environment segment
; Out:          AX      exec code
Exec            PROC    NEAR
; Prepare environment
                mov     EPB_Env,ax
; Prepare Parameters
                mov     EPB_Parm_Ofs,bx
                mov     EPB_Parm_Seg,es
; Prepare FCBs (probably useless)
                mov     ax,PSPSeg
                mov     WORD PTR EPB_FCB1_Ofs,FCB1
                mov     EPB_FCB1_Seg,ax
                mov     WORD PTR EPB_FCB2_Ofs,FCB2
                mov     EPB_FCB2_Seg,ax
; Execute
                mov     bx,offset EPB
                mov     ax,4B00h
                int     21h
                jc      L24
                xor     ax,ax
L24:            ret
Exec            ENDP


_TEXT   ENDS

_DATA   SEGMENT WORD 'DATA'

Msg_Error       db      'Can''t run DOS/4G(W)',0Ah,0Dh,'$'

EnvDOS4GPATH    db      9,'DOS4GPATH'
EnvPATH         db      4,'PATH'

DOS4GW_EXE      db      'DOS4GW.EXE',0
DOS4G_EXE       db      'DOS4G.EXE',0

IFDEF QUIET
EnvDOS4G        db      'DOS4G=QUIET',0
EnvDOS4G_Len    = $ - EnvDOS4G
ENDIF

; align the file-length to be a multiple of 16
IFDEF QUIET
;                db      0 dup (?)
ELSE
                db      4 dup (?)
ENDIF

_DATA   ENDS

_BSS    SEGMENT 'BSS'

EnvSeg          dw      ?
PSPSeg          dw      ?

EPB             LABEL   BYTE                    ; EPB - Exec Parameter Block
EPB_Env         dw      ?
EPB_Parm_Ofs    dw      ?
EPB_Parm_Seg    dw      ?
EPB_FCB1_Ofs    dw      ?
EPB_FCB1_Seg    dw      ?
EPB_FCB2_Ofs    dw      ?
EPB_FCB2_Seg    dw      ?

ArgBuffer       db      128 dup (?)             ; Command line buffer
DOS4GWpathName  db      128 dup (?)             ; DOS pathnames are limited
                                                ; 128 chars
_BSS    ENDS


_STACK  SEGMENT 'STACK'
                db      StackSize dup (?)
StackEnd        LABEL   BYTE
_STACK  ENDS

                END     main
