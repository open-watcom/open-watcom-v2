
b       equ     byte ptr
w       equ     word ptr
d       equ     dword ptr
f       equ     fword ptr

Pushm   MACRO   R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
        IRP     X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16>      ;REPEAT FOR EACH PARM
        IFNB    <X>                     ;IF THIS PARM NOT BLANK
        PUSH    X                       ;SAVE THE REGISTER
        ENDIF                           ;END IFNB
        ENDM                            ;END IRP
        ENDM

Popm    macro   R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
        IRP     X,<R16,R15,R14,R13,R12,R11,R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
        IFNB    <X>                             ;IF THIS PARM NOT BLANK
        POP     X                               ;POP THE REGISTER
        ENDIF                                   ;END IFNB
        ENDM                                    ;END IRP
        ENDM

Rep_movs        macro
        push    ecx
        shr     ecx,1
        push    ecx
        shr     ecx,1
        rep     movsd
        pop     ecx
        and     ecx,1
        rep     movsw
        pop     ecx
        and     ecx,1
        rep     movsb
        endm

Rep_stos        macro
        push    ecx
        shr     ecx,1
        push    ecx
        shr     ecx,1
        rep     stosd
        pop     ecx
        and     ecx,1
        rep     stosw
        pop     ecx
        and     ecx,1
        rep     stosb
        endm


