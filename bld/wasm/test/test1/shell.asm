                NAME    multiply
DGROUP          GROUP   CONST,CONST2,_DATA
_TEXT           SEGMENT BYTE PUBLIC  'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  multiply_
multiply_:              mov             ax, 3
                                push    ax
                                xor             ax, ax
                        @BeginNormalize:
                                test    word ptr [di + 2], 8000h
                                jnz     @EndNormalize
                                shl     word ptr [di], 1
                                rcl             word ptr [di + 2], 1
                                shl             word ptr [si], 1
                                rcl             word ptr [si + 2], 1
                                rcl             word ptr [si + 4], 1
                                rcl             word ptr [si + 6], 1
                                rcl             ax, 1
                                jmp             @BeginNormalize
                        @EndNormalize:
                                mov             bx, ax
                                mov             cx, 0FFFFh
                                mul             cx
                                add             ax, bx
                                adc             dx, 0
                                add             ax, word ptr [si + 6]
                                adc             dx, 0
                                mov             cx, dx
                                mov             bx, ax
                                div             word ptr [di + 2]
                                push    ax
                                mul             word ptr [di + 2]
                                sub             bx, ax
                                sbb             cx, dx
                                pop             ax
                                mov             word ptr [si + 6], ax
                                mul             word ptr [di]
                                test    cx, cx
                                jnz             @EndReduce
                        @BeginReduce:
                                cmp             dx, bx
                                jb              @EndReduce
                                ja              @NextReduce
                                cmp             ax, word ptr [si + 4]
                                jbe             @EndReduce
                        @NextReduce:
                                dec             word ptr [si + 6]
                                sub             ax, word ptr [di]
                                sbb             dx, 0
                                add             bx, word ptr [di + 2]
                                adc             cx, 0
                                jmp             @BeginReduce
                        @EndReduce:
                                sub             word ptr [si + 4], ax
                                sbb             bx, dx
                                mov             word ptr [si + 6], bx
                                sbb             cx, 0
                                pop             dx
                                dec             dx
                           cmp          dx, 2
                                je              @NextDivide
                                mov             word ptr [si + 8], cx
                           test    dx, dx
                                jz              @EndDivide
                        @NextDivide:
                                sub             si, 2
                                mov             ax, bx
                                push    dx
                                jmp             @EndNormalize
                        @EndDivide:
_TEXT           ENDS

CONST           SEGMENT WORD PUBLIC  'DATA'
CONST           ENDS

CONST2          SEGMENT WORD PUBLIC  'DATA'
CONST2          ENDS

_DATA           SEGMENT WORD PUBLIC  'DATA'
_DATA           ENDS

                END
