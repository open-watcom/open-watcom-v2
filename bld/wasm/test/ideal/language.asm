                IDEAL
                P486
                MODEL   USE32 SMALL
                DATASEG
Value1          DD      4
Value2          DQ      8
                CODESEG
PROC            Main
                call    StandardCall STDCALL, [Value1], [Value2], 3, 4, 5, 6    ; STDCALL
                call    StandardCall STDCALL, 1, 2, 3, 4, 5, 6  ; STDCALL
                call    WatcomC WATCOM_C, 1, 2, 3, 4, 5, 6, 7   ; WATCOM_C
                call    StandardC C, 1, 2, 3, 4, 5, 6           ; C
                call    SystemCall SYSCALL, 1, 2, 3, 4, 5, 6    ; SYSCALL
                call    PascalCall PASCAL, 1, 2, 3, 4, 5, 6     ; PASCAL
                call    BasicCall BASIC, 1, 2, 3, 4, 5, 6       ; BASIC
                call    FortranCall FORTRAN, 1, 2, 3, 4, 5, 6   ; FORTRAN
                push    6                                       ; NOLANGUAGE
                push    5
                push    4
                push    3
                push    2
                push    1
                call    NoLanguageCall NOLANGUAGE
ENDP
PROC            StandardCall STDCALL
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            WatcomC WATCOM_C
                ARG     One : DWORD, Two : QWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     esi,[One]
                mov     eax,[Two]
                add     eax,[Two + 4]
                add     eax,[Three]
                add     eax,[Four]
                add     eax,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            StandardC C
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            SystemCall SYSCALL
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            PascalCall PASCAL
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            BasicCall BASIC
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            FortranCall FORTRAN
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD
                USES    esi,edi
                LOCAL   Number : DWORD
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                ret
ENDP
PROC            NoLanguageCall NOLANGUAGE
                ARG     One : DWORD, Two : DWORD, Three : DWORD, \
                        Four : DWORD, Five : DWORD, Six : DWORD = ARGSIZE;
                LOCAL   Number : DWORD = COUNT;
                push    ebp
                mov     ebp,esp
                sub     esp,COUNT
                mov     eax,[One]
                add     eax,[Two]
                add     eax,[Three]
                add     eax,[Four]
                mov     esi,[Five]
                mov     edi,[Six]
                add     eax,esi
                add     eax,edi
                mov     [Number],eax
                mov     esp,ebp
                pop     ebp
                ret     ARGSIZE
ENDP
                END
