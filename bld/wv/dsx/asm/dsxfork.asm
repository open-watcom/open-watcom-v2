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

EXTRN           GrabHandlers_           :NEAR
EXTRN           RestoreHandlers_        :NEAR
EXTRN           DOSEnvFind_             :NEAR
EXTRN           __psp                   :WORD


HSTDIN          equ     0
HSTDOUT         equ     1
CR_CHAR         equ     13              ;carriage return ASCII code
EXTRALEN        equ     3               ;length of '/C '
OFF_FCB1        equ     5ch             ;offset of 1st fcb in a PSP
OFF_FCB2        equ     6ch             ;offset of 2nd fcb in a PSP
CMD_OPTION      equ     ' C'            ;the '/C ' option for COMMAND.COM
ERRFLAG         equ     0ffff0000h      ;flag signaling error on return

PARMBLOC        STRUC                           ;needed for int 21h ah=4b
envptr          dp      ?                       ;ptr to environment
cmdptr          dp      ?                       ;ptr to cmd line to execute
fcb1ptr         dp      ?                       ;ptr to 1st FCB
fcb2ptr         dp      ?                       ;ptr to 2nd FCB
PARMBLOC        ENDS


DGROUP GROUP CONST, _BSS


CONST           SEGMENT BYTE PUBLIC 'DATA'
Comspec         db 'COMSPEC', 0
ConName         db 'CON', 0
CONST           ENDS


_BSS            SEGMENT BYTE PUBLIC 'BSS'
StdinHandle     dw ?
StdoutHandle    dw ?
ConHandle       dw ?
_BSS            ENDS


_TEXT           SEGMENT BYTE PUBLIC 'CODE'
ASSUME  cs:_TEXT, ds:DGROUP, ss:DGROUP

;on entry to RedirectInOut_ :
;       takes no parameters
;on RedirectInOut_ exit :
;       eax     holds return code (0 if successfull)
                PUBLIC RedirectInOut_
RedirectInOut_  PROC NEAR
                push    edx
                push    ecx
                push    ebx

                mov     ebx, HSTDIN
                mov     ah, 45h                 ;get another handle for
                int     21h                     ;stdin - returned in ax
                jc      redir_done
                mov     StdinHandle, ax

                mov     ebx, HSTDOUT
                mov     ah, 45h                 ;get another handle for
                int     21h                     ;stdout - returned in ax
                jc      redir_done
                mov     StdoutHandle, ax

                mov     edx, OFFSET ConName
                mov     ax, 3d02h               ;open console for read/write
                int     21h                     ;on return, ax holds handle
                jc      redir_done              ;or error code
                mov     ConHandle, ax

                movzx   ebx, ax
                mov     ecx, HSTDIN
                mov     ah, 46h                 ;redirect stdin
                int     21h
                jc      redir_done

                mov     ecx, HSTDOUT
                mov     ah, 46h                 ;redirect stdout
                int     21h
                jc      redir_done

                mov     ah, 3eh                 ;close duplicate console
                int     21h
                jc      redir_done
                xor     eax, eax                ;return 0 to indicate success

redir_done:     cwde                            ;ret code now in eax
                clc                             ;flag was set if there was err
                pop     ebx
                pop     ecx
                pop     edx
                ret
RedirectInOut_  ENDP

;on entry to RestoreInOut_ :
;       takes no parameters
;on RestoreInOut_ exit :
;       eax     holds return code (0 if successfull)
                PUBLIC RestoreInOut_
RestoreInOut_   PROC NEAR
                push    ecx
                push    ebx

                movzx   ebx, StdinHandle
                mov     ecx, HSTDIN
                mov     ah, 46h                 ;restore stdin
                int     21h
                jc      restore_done

                mov     ah, 3eh                 ;close duplicate stdin
                int     21h
                jc      restore_done

                movzx   ebx, StdoutHandle       ;restore stdout
                mov     ecx, HSTDOUT
                mov     ah, 46h
                int     21h
                jc      restore_done

                mov     ah, 3eh                 ;close duplicate stdout
                int     21h
                jc      restore_done
                xor     eax, eax                ;return 0 to indicate success

restore_done:   cwde                            ;ret code now in eax
                clc                             ;flag was set if there was err
                pop     ebx
                pop     ecx
                ret
RestoreInOut_   ENDP

;on entry to load_and_exec_ :
;       ebx     points to the command string to execute
;       edx     holds len of the cmd string (but we assume len is all in dl)
;on load_and_exec exit :
;       eax     holds a return code (0 if successfull)
load_and_exec_  PROC NEAR
                push    ebp
                push    edi
                push    esi
                push    edx
                push    ecx
                push    ebx
                push    es
                mov     ebp, esp

                sub     esp, edx                ;make room for child PSP cmdtail
                dec     esp                     ;make room for terminating char
                mov     edi, esp
                mov     ax, ss
                mov     es, ax
                mov     esi, ebx                ;now esi points to cmd string
                mov     ecx, edx                ;now ecx has len of cmd string
                rep     movsb                   ;copy cmd string to PSP cmdtail
                mov     byte ptr es:[edi], CR_CHAR ;store terminating char

                test    edx, edx                ;if there is no cmd to execute
                jz      no_cmdline              ;then don't add '/C '
                mov     ecx, edx                ;cmdstr len in ecx (cl really)
                mov     ax, 3700h               ;get switch char
                int     21h                     ;switch char now in dl
                push    word ptr CMD_OPTION
                mov     bl, cl
                add     bl, EXTRALEN            ;now bl holds len cmd str + 3
                mov     bh, dl                  ;now ebx holds cmdtail prefix
                push    bx                      ;add the prefix to the cmdtail
                jmp     make_parmbloc

no_cmdline:     dec     esp                     ;make room for length
                mov     byte ptr [esp], 0       ;len=0

make_parmbloc:  mov     edi, esp                ;save addr of PSP cmdtail

                sub     esp, SIZE PARMBLOC
                mov     ebx, esp                ;now ebx pts to our parm block
                mov     dword ptr ss:[ebx].envptr, 0
                mov     word ptr ss:[ebx].envptr+4, 0
                mov     dword ptr ss:[ebx].cmdptr, edi
                mov     word ptr ss:[ebx].cmdptr+4, ss
                mov     ax, __psp
                mov     dword ptr ss:[ebx].fcb1ptr, OFF_FCB1
                mov     word ptr ss:[ebx].fcb1ptr+4, ax
                mov     dword ptr ss:[ebx].fcb2ptr, OFF_FCB2
                mov     word ptr ss:[ebx].fcb2ptr+4, ax

                mov     eax, OFFSET Comspec
                call    DOSEnvFind_             ;get value of the COMSPEC dosvar
                mov     edx, eax                ;ds:edx points to command shell
                mov     ax, 4b00h               ;load/exec command shell
                int     21h
                jc      loadexec_done
                xor     eax, eax                ;return 0 to indicate success

loadexec_done:  cwde                            ;now eax has ret code
                clc                             ;flag was set if there was err
                mov     esp, ebp
                pop     es
                pop     ebx
                pop     ecx
                pop     edx
                pop     esi
                pop     edi
                pop     ebp
                ret
load_and_exec_  ENDP

;on entry to _fork_( char *cmd, unsigned len ) :
;       eax     points to the command string to execute
;       edx     holds len of the cmd string (but we assume len is all in dl)
;on _fork_ exit :
;       eax     holds a return code
                PUBLIC _fork_
_fork_          PROC NEAR
                push    ebx
                mov     ebx, eax                ;put command string ptr in ebx

                call    RestoreHandlers_
                call    RedirectInOut_
                test    eax, eax
                jnz     fork_done
                call    load_and_exec_
                call    RestoreInOut_

fork_done:      push    eax                     ;save eax because GrabHandlers
                call    GrabHandlers_           ;destroys it
                pop     eax
                test    eax, eax
                jz      L1                      ;if eax is 0 then no errors
                or      eax, ERRFLAG            ;else signal error in ret code
L1:             pop     ebx
                ret
_fork_          ENDP

_TEXT           ENDS
                END
