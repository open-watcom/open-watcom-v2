.386p
                NAME    c2asm
                EXTRN   _cstart_ :BYTE
                EXTRN   _ASMMain :BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  main_ 
main_:          push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                mov     ebp,esp
                sub     esp,00000004H
                call    near ptr _ASMMain
                mov     dword ptr -4H[ebp],eax
                mov     eax,dword ptr -4H[ebp]
                mov     esp,ebp
                pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                ret     
_TEXT           ENDS

CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
_BSS            ENDS

$$SYMBOLS       SEGMENT BYTE USE16 'DEBSYM'
                DB      08H,32H
                DP      main_
                DB      1aH,24H,00H,00H,00H,00H,22H,00H
                DB      00H,00H,00H,00H,0eH,09H,18H,00H
                DB      00H,00H,0cH,30H,1cH,00H,6dH,61H
                DB      69H,6eH
$$SYMBOLS       ENDS

$$TYPES         SEGMENT BYTE USE16 'DEBTYP'
                DB      06H,13H,7dH,00H,00H,00H,08H,11H
                DB      73H,74H,72H,75H,63H,74H,07H,11H
                DB      75H,6eH,69H,6fH,6eH,06H,11H,65H
                DB      6eH,75H,6dH,0eH,10H,00H,73H,69H
                DB      67H,6eH,65H,64H,20H,63H,68H,61H
                DB      72H,10H,10H,10H,75H,6eH,73H,69H
                DB      67H,6eH,65H,64H,20H,63H,68H,61H
                DB      72H,08H,10H,01H,73H,68H,6fH,72H
                DB      74H,11H,10H,11H,75H,6eH,73H,69H
                DB      67H,6eH,65H,64H,20H,73H,68H,6fH
                DB      72H,74H,06H,10H,03H,69H,6eH,74H
                DB      0fH,10H,13H,75H,6eH,73H,69H,67H
                DB      6eH,65H,64H,20H,69H,6eH,74H,08H
                DB      12H,00H,05H,63H,68H,61H,72H,08H
                DB      12H,00H,05H,63H,68H,61H,72H,04H
                DB      72H,08H,00H,02H,14H
$$TYPES         ENDS

$$TYPES         SEGMENT BYTE USE16 'DEBTYP'
                DB      00H,00H,01H,00H,00H,00H,44H,3aH
                DB      5cH,4cH,49H,42H,5cH,53H,59H,53H
                DB      54H,45H,4dH,5cH,63H,32H,61H,73H
                DB      6dH,2eH,63H,00H
$$TYPES         ENDS

                END
