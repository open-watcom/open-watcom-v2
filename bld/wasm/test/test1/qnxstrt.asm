                name    TRAPSTRT


public          _small_code_
_small_code_      equ 0

dgroup group    _DATA

_TEXT           segment byte public 'CODE'

                extrn   TrapInit_               :near
                extrn   TrapAccess_             :near
                extrn   TrapFini_               :near
                extrn   StartupInit_            :near
                extrn   __qnx_exit_             :near

assume  CS:_TEXT

        org     0H

        start label near
        jmp     short   kill_myself

validate        dw      0DEAFh
init            dw      Initialize
req             dw      Request
fini            dw      Finalize
dataseg         dw      seg DGROUP

kill_myself:
        mov     ax,-1
        jmp     __qnx_exit_

;   DX:AX - parameter string buffer
;   CX:BX - error message buffer
;   stack - are we talking to a remote link?
;   stack - far pointer to _proc_spawn structure for program
;   stack - far pointer to signal table for program
; returns major version in AH, minor version in AL, remote in DX

Initialize      proc    far
                push    bp
                mov     bp,sp
                push    ds
                push    es
                push    ax
                push    dx
                push    cx
                push    bx
                mov     bx,12[bp]
                mov     cx,14[bp]
                mov     ax,8[bp]
                mov     dx,10[bp]
                mov     ds,cs:dataseg
                call    StartupInit_
                pop     bx
                pop     cx
                pop     dx
                pop     ax
                push    6[bp]
                call    TrapInit_
                pop     es
                pop     ds
                mov     sp,bp
                pop     bp
                ret     2*5H
Initialize      endp


; AX - number of input message
; DX - number of output message
; CX BX - address of input message entry


Request         proc    far
                push    bp              ; prologue
                mov     bp,sp           ; ...
                push    ds              ; save segments
                push    es
                push    8[bp]           ; copy parm4 down stack
                push    6[bp]           ; ...
                mov     ds,cs:dataseg
                call    TrapAccess_
                pop     es              ; restore segments
                pop     ds
                mov     sp,bp           ; epilogue
                pop     bp              ; ...
                ret     03+1            ; clear parms
Request         endp

Finalize        proc    far
                push    ds
                push    es
                mov     ds,cs:dataseg
                call    TrapFini_
                pop     es
                pop     ds
                ret
Finalize        endp


_TEXT   ends

_DATA           segment byte public 'DATA'
_DATA           ends

        end     start
