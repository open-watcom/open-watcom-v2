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


        name    dpmildr

include struct.inc

        extrn   Loader16        : near
        extrn   RM_BufSeg       : word
        extrn   RM_BufSize      : word
        extrn   IsDBCS          : word
        extrn   _PSP            : word
        extrn   _end            : byte

DGROUP  group   _DATA
_DATA   segment dword public 'DATA'

modesw  dd      0       ; far pointer to DPMI host's mode switch entry point

Enopathmsg      db "PATH environment variable not set",0Dh,0Ah,"$"
Jnopathmsg      db "PATH 環境変数が設定されていません",0Dh,0Ah,"$"

Ew32run_notfound db "This program requires W32RUN.EXE to be in your PATH",0Dh,0Ah,"$"
Jw32run_notfound db "このプログラムには PATH のディレクトリに W32RUN.EXE が必要です",0Dh,0Ah,"$"

Eout_of_memory  db "Loader error: command line too long",0Dh,0Ah,"$"
Jout_of_memory  db "ローダー・エラー: コマンド・ラインが長すぎます",0Dh,0Ah,"$"

Espawn_error    db "Loader error spawning application",0Dh,0Ah,"$"
Jspawn_error    db "ローダー・エラー: アプリケーションを起動できません",0Dh,0Ah,"$"

_DATA   ends


_TEXT   segment dword public 'CODE'
        assume  cs:_TEXT
        assume  ds:DGROUP


        public  exit_with_msg
exit_with_msg   proc    near
        mov     AX,DGROUP               ; point ds to DGROUP
        mov     DS,AX                   ; ...
        mov     AH,9                    ; display message
        int     21h                     ; ...
        mov     AX,4CFFh                ; exit with error
        int     21h                     ; ...
exit_with_msg   endp

        public  __OS2stub
__OS2stub proc near
        mov     AX,DGROUP               ; point ds to DGROUP
        mov     DS,AX                   ; ...
        mov     DX,_PSP                 ; set es=PSP
        mov     ES,DX                   ; ...
        mov     BX,offset _end          ; get end of data
        add     BX,4096                 ; plus stack
        and     BL,0F0h                 ; round down to para
        mov     SS,AX                   ; set SS=dgroup
        mov     SP,BX                   ; set stack pointer
        shr     BX,1                    ; calc # of paras to keep
        shr     BX,1                    ; ...
        shr     BX,1                    ; ...
        shr     BX,1                    ; ...
        sub     AX,DX                   ; calc. size of PSP + CODE
        add     BX,AX                   ; add this to size of DATA+STACK
        mov     AH,4Ah                  ; resize of memory block
        int     21h                     ; ...
        mov     DS,ES:[002Ch]           ; get segment of environment
        sub     SI,SI                   ; offset 0
        sub     DI,DI                   ; assume "PATH=" not found
        _loop                           ; loop (for each string)
          _guess                        ; - guess: PATH=
            cmp   byte ptr 0[SI],'P'    ; - - quit if not PATH=
            _quif ne                    ; - - ...
            cmp   byte ptr 1[SI],'A'    ; - - ...
            _quif ne                    ; - - ...
            cmp   byte ptr 2[SI],'T'    ; - - ...
            _quif ne                    ; - - ...
            cmp   byte ptr 3[SI],'H'    ; - - ...
            _quif ne                    ; - - ...
            cmp   byte ptr 4[SI],'='    ; - - ...
            _quif ne                    ; - - ...
            lea   DI,5[SI]              ; - - remember start of PATH
          _endguess                     ; - endguess
          _loop                         ; - loop (until end of string)
            lodsb                       ; - - get character
            cmp   AL,0                  ; - - check for end of string
          _until  e                     ; - until end of string
          lodsb                         ; - get next character
          dec   SI                      ; - back up the pointer
          cmp   AL,0                    ; - check for end of environment
        _until  e                       ; until all done
        add     SI,3                    ; advance to full path of exe
        mov     BX,SI                   ; remember start addr of name
        or      DI,DI                   ; make sure "PATH" found
        _if     e                       ; if no PATH found
          mov   DX,SS                   ; - set DS:DX to point to name
          mov   DS,DX                   ; - ...
          mov   DX,offset DGROUP:Enopathmsg
          cmp   IsDBCS,0                ; - if not english
          _if   ne                      ; - then
            mov DX,offset DGROUP:Jnopathmsg ; - - point to japanese msg
          _endif                        ; - endif
          jmp   exit_with_msg           ; - quit if no PATH found
        _endif                          ; endif
        dec     BX                      ; point to end of environment strings
        mov     byte ptr -2[BX],'$'     ; set "$="
        mov     byte ptr -1[BX],'='     ; ...
        _loop                           ; loop (copy name)
          lodsb                         ; - get character
          mov   [BX],AL                 ; - store it
          inc   BX                      ; - increment pointer
          cmp   AL,0                    ; - check for end of name
        _until  e                       ; until done
;
;       DI points to PATH environment varible
;
        sub     SP,256                  ; allocate space for buffer
        mov     BP,SP                   ; point to buffer
        _loop                           ; loop (for each path in PATH env)
          mov   byte ptr 0[BP],'W'      ; - append "W32RUN.EXE"
          mov   byte ptr 1[BP],'3'      ; - ...
          mov   byte ptr 2[BP],'2'      ; - ...
          mov   byte ptr 3[BP],'R'      ; - ...
          mov   byte ptr 4[BP],'U'      ; - ...
          mov   byte ptr 5[BP],'N'      ; - ...
          mov   byte ptr 6[BP],'.'      ; - ...
          mov   byte ptr 7[BP],'E'      ; - ...
          mov   byte ptr 8[BP],'X'      ; - ...
          mov   byte ptr 9[BP],'E'      ; - ...
          mov   byte ptr 10[BP],0       ; - ...
          mov   DX,SP                   ; - point to file name
          push  DS                      ; - save DS
          mov   AX,SS                   ; - point DS:DX to file name
          mov   DS,AX                   ; - ...
          mov   AX,4300h                ; - "get file attributes"
          int   21h                     ; - to see if file exists
          pop   DS                      ; - restore DS
          jnc   w32run_exists           ; - quit if file found
          mov   AL,[DI]                 ; - get character
          cmp   AL,0                    ; - check for end of string
          _quif e                       ; - quit if end of path
          mov   BP,SP                   ; - point to buffer
          _loop                         ; - loop (copy path to buffer)
            mov   AL,[DI]               ; - - get character
            cmp   AL,0                  ; - - quit if end of string
            _quif e                     ; - - ...
            inc   DI                    ; - - advance pointer
            cmp   AL,';'                ; - - quit if path separator
            _quif e                     ; - - ...
            mov   0[BP],AL              ; - - copy to buffer
            inc   BP                    ; - - ...
          _endloop                      ; - endloop
          mov   byte ptr 0[BP],'\'      ; - append '\'
          inc   BP                      ; - ...
        _endloop                        ; endloop
        mov     DX,SS                   ; set DS:DX to point to name
        mov     DS,DX                   ; ...
        mov     DX,offset DGROUP:Ew32run_notfound
        cmp     IsDBCS,0                ; if not english
        _if     ne                      ; then
          mov   DX,offset DGROUP:Jw32run_notfound ; - point to japanese msg
        _endif                          ; endif
        jmp     exit_with_msg           ; exit
w32run_exists:
        mov     DX,SS                   ; set DS:DX to point to name
        mov     DS,DX                   ; ...
        mov     DX,SP                   ; ...
        push    ES                      ; PSP
        mov     AX,006Ch                ; FCB at 6Ch
        push    AX                      ; ...
        push    ES                      ; PSP
        mov     AX,005Ch                ; FCB at 5Ch
        push    AX                      ; ...
        push    ES                      ; PSP
        mov     AX,0080h                ; command line at 80h
        push    AX                      ; ...
        sub     AX,AX                   ; inherit my environment
        push    AX                      ; ...
        mov     AX,SS                   ; set ES=SS
        mov     ES,AX                   ; ...
        mov     BX,SP                   ; point to parameter block
        mov     AX,4B00h                ; load and execute program
        int     21h                     ; ...
        _guess                          ; guess: no spawn error
          _quif c                       ; - quit if error
          mov   AH,4Dh                  ; - fetch exit code
          int   21h                     ; - ...
        _admit                          ; admit: spawn error
          mov   DX,offset DGROUP:Espawn_error ; - generic spawn error msg
          cmp   AL,8                    ; - if out of memory
          _if   e                       ; - then
            mov DX,offset DGROUP:Eout_of_memory ; - - set specific msg
          _endif                        ; - endif
          cmp   IsDBCS,0                ; - if not english
          _if   ne                      ; - then
            mov DX,offset DGROUP:Jspawn_error ; - - generic spawn error msg
            cmp   AL,8                  ; - - if out of memory
            _if   e                     ; - - then
              mov   DX,offset DGROUP:Jout_of_memory ; - - - set specific msg
            _endif                      ; - - endif
          _endif                        ; - endif
          jmp   exit_with_msg           ; - exit with message
        _endguess                       ; endguess
        mov     AH,4Ch                  ; exit
        int     21h                     ; ...

;
; allow this code to get overwritten by stack if needed
;
__OS2stub endp


        public  __DPMI_start
__DPMI_start proc near
        mov     DX,DS                   ; set es=PSP
        mov     ES,DX                   ; ...
        mov     AX,DGROUP               ; point ds to DGROUP
        mov     DS,AX                   ; ...
        mov     _PSP,ES                 ; save PSP segment value
        mov     BX,offset _end          ; get end of data
        add     BX,8192                 ; plus stack
        and     BL,0F0h                 ; round down to para
        mov     SS,AX                   ; set SS=dgroup
        mov     SP,BX                   ; set stack pointer
        shr     BX,1                    ; calc # of paras to keep
        shr     BX,1                    ; ...
        shr     BX,1                    ; ...
        shr     BX,1                    ; ...
        sub     AX,DX                   ; calc. size of PSP + CODE
        add     BX,AX                   ; add this to size of DATA+STACK
        mov     AH,4Ah                  ; resize of memory block
        int     21h                     ; ...
        push    DS                      ; save DS
        _guess                          ; guess: DBCS
          mov   AX,6300h                ; - get DBCS vector table
          int   21h                     ; - ...
          mov   AX,0                    ; - assume no DBCS support
          _quif c                       ; - quit if error
          mov   AX,[SI]                 ; - get first 2 bytes of DBCS table
        _endguess                       ; endguess
        pop     DS                      ; restore DS
        mov     IsDBCS,AX               ; save DBCS indicator
        _guess                          ; guess: Windows DPMI host
          mov   AX,1687h                ; - get addr of DPMI host's
          int   2Fh                     ; - ... mode switch entry point
          or    AX,AX                   ; - quit if DPMI host not present
          _quif ne                      ; - ...
          mov   word ptr modesw,DI      ; - save addr of mode switch
          mov   word ptr modesw+2,ES    ; - ... entry point
          mov   AX,1600h                ; - check for Windows 3.x
          int   2Fh                     ; - ...
          cmp   AL,0                    ; - quit if not Windows 3.x
          _quif e                       ; - ...
          cmp   AL,80h                  ; - ...
          _quif e                       ; - ...
          cmp   AL,0FFh                 ; - ...
          _quif e                       ; - ...
          cmp   AL,3                    ; - check version number
          _quif ne                      ; - quit if not 3.x
          mov   AX,SI                   ; - get size of private data area
          or    SI,SI                   ; - SI = size of private data area
          _if   ne                      ; - if non-zero
            mov   BX,SI                 ; - - get size
            mov   AH,48H                ; - - allocate DOS memory block
            int   21h                   ; - - ...
          _endif                        ; - endif
          _quif c                       ; - quit if allocation error
          mov   ES,AX                   ; - point ES to area
          mov   BX,1000h                ; - allocate 64K real-mode buffer
          mov   AH,48H                  ; - allocate DOS memory block
          int   21h                     ; - ...
          _if   c                       ; - if error
            mov   AH,48H                ; - - allocate whatever is available
            int   21h                   ; - - ...
            inc   BX                    ; - - add 1 paragraph
          _endif                        ; - endif
          dec   BX                      ; - subtract 1 paragraph
          mov   RM_BufSeg,AX            ; - save real-mode segment of buffer
          add   BX,BX                   ; - multiply by 16
          add   BX,BX                   ; - ...
          add   BX,BX                   ; - ...
          add   BX,BX                   ; - ...
          and   BX,0FE00h               ; - make multiple of 512
          mov   RM_BufSize,BX           ; - save size of real-mode buffer
          push  DS                      ; - save DS
          mov   DS,AX                   ; - point to real-mode buffer
          sub   DX,DX                   ; - offset 0
          mov   AH,1Ah                  ; - set DTA to real-mode buffer
          int   21h                     ; - ...
          pop   DS                      ; - restore DS
          mov   AX,0                    ; - switch to 16-bit protect mode
          call  modesw                  ; - ...
          _quif c                       ; - quit if switch to protect failed
        _admit                          ; admit: No DPMI available
;         spawn "W32RUN.EXE" to run as NT or DOS-extended application
          jmp   __OS2stub
        _endguess                       ; endguess
        jmp     Loader16                ; jump to 16-bit loader
__DPMI_start endp

_TEXT   ends
        end     __DPMI_start
