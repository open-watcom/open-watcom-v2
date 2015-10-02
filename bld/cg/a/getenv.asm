        extrn   "C",_Envptr : fword
        name    GetEnvVar

_text segment byte public 'code'

assume  cs:_text

;
; GetEnvVar( what @: char, buff@: char, len : int ) : bool
;

public  GetEnvVar_
GetEnvVar_ proc near
        push    ecx                     ; save some registers
        push    esi                     ; ...
        push    edi                     ; ...
        push    es                      ; ...
        push    ds                      ; ...
        push    ebp                     ; ...
        mov     ebp,eax                 ; DS:SI @ reqd env variable
        les     edi,_Envptr             ; ES:EDI point to environment block
theloop:                                ; loop : theloop
        cmp     byte ptr es:[edi],0     ; - quif sitting on nullchar
        je      noenv                   ; - ...
        mov     esi,ebp                 ; - ds:si points to what
        mov     ecx,ebx                 ; - set up length
        repe    cmpsb                   ; - see if strings are the same
        jne     lukmore                 ; - if same string
        cmp     byte ptr es:[edi],'='   ; - - if next char is =
        je      found                   ; - - - goto found:
                                        ; - - endif
                                        ; - endif
lukmore:xor     al,al                   ; - scan to next nullchar
        or      ecx,-1                  ; - ...
        repne   scasb                   ; - ...
        jmp     theloop                 ; endloop
noenv:  xor     eax,eax                 ; return FALSE
        jmp     epi                     ; return to caller
found:  inc     edi                     ; bite off the =
        test    edx,edx                 ; if dest == NULL
        je      ret1                    ; - he doesn't want the string
        mov     ebx,edx                 ; copy the string
cpylup: mov     al,es:[edi]             ; ... the parm buff
        mov     [ebx],al                ; ...
        inc     ebx                     ; ...
        inc     edi                     ; ...
        test    al,al                   ; ...
        jne     cpylup                  ; ...
ret1:   mov     eax,1                   ; return TRUE
epi:    pop     ebp                     ; return to caller
        pop     ds                      ; restore registers
        pop     es                      ; ...
        pop     edi                     ; ...
        pop     esi                     ; ...
        pop     ecx                     ; ...
        ret                             ; return to caller
GetEnvVar_      endp

_text   ends

        end
