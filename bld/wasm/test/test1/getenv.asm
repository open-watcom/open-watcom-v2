include test.inc
include osdef.inc
include mdef.inc
include struct.inc

extrn   DOSGETENV       : far
                modstart        GetEnvVar

               if 0
                dataseg
                enddata
                endif

;
; GetEnvVar( what @: char, buff@: char, len : int ) : bool
;

                xdefp   GetEnvVar_
                defp    GetEnvVar_
                push    cx
                push    si
                push    di
                push    es
                push    ds
                push    bp
                mov     bp,ax                   ; DS:SI @ reqd env variable
                push    bp                      ; save bp
                mov     bp,sp                   ; allocate stack
                push    ax                      ; ...
                push    ax                      ; ...
                lea     ax,-4[bp]               ; push@ word    EnvSegment
                push    ss                      ; ...
                push    ax                      ; ...
                lea     ax,-2[bp]               ; push@ word    CmdOffset
                push    ss                      ; ...
                push    ax                      ; ...
                call    DOSGETENV               ; get the environment segment
                test    ax,ax                   ; if error
                je      noerror                 ; - then
                pop     bp                      ; - restore bp
                jmp     notfound                ; - not found
noerror:        mov     es,-4[bp]               ; ...
                pop     ax                      ; restore stack pointer
                pop     ax                      ; ...
                pop     bp                      ; restore bp
                xor     di,di                   ; ES:DI @ env strings
theloop:                                        ; loop : theloop
                cmp     byte ptr es:[di],0      ; - quif sitting on nullchar
                je      notfound                ; - ...
                mov     si,bp                   ; - ds:si points to what
                mov     cx,bx                   ; - set up length
                repe    cmpsb                   ; - see if strings are the same
                jne     lookmore                ; - if same string
                cmp     byte ptr es:[di],'='    ; - - if next char is =
                je      found                   ; - - - goto found:
                                                ; - - endif
                                                ; - endif
lookmore:       mov     al,0                    ; - scan to next nullchar
                mov     cx,-1                   ; - ...
                repne   scasb                   ; - ...
                jmp     theloop                 ; endloop
notfound:       xor     ax,ax                   ; return FALSE
                jmp     epi                     ; return to caller
found:          inc     di                      ; bite off the =
                mov     bx,dx                   ; copy the string
copyloop:       mov     al,es:[di]              ; ... the parm buff
                mov     [bx],al                 ; ...
                inc     bx                      ; ...
                inc     di                      ; ...
                cmp     al,0                    ; ...
                jne     copyloop                ; ...
                mov     ax,-1                   ; return TRUE
epi:            pop     bp                      ; return to caller
                pop     ds
                pop     es
                pop     di
                pop     si
                pop     cx
                ret
                endproc GetEnvVar_

                endmod
