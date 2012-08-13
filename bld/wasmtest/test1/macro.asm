;retf            MACRO   POP_COUNT
;                LOCAL   DUMMY
;                DUMMY   PROC FAR
;                RET     POP_COUNT
;                DUMMY   ENDP
;                ENDM
                NAME    macro
                EXTRN   _big_code_ :BYTE
                EXTRN   get_curr_filename_ :BYTE
                EXTRN   dir_insert_ :BYTE
                EXTRN   PushMacro_ :BYTE
                EXTRN   AsmScan_ :BYTE
                EXTRN   _StringBuf :BYTE
                EXTRN   EvalExpr_ :BYTE
                EXTRN   ExpandSymbol_ :BYTE
                EXTRN   itoa_ :BYTE
                EXTRN   DoDebugMsg_ :BYTE
                EXTRN   stackavail_ :BYTE
                EXTRN   atoi_ :BYTE
                EXTRN   strncat_ :BYTE
                EXTRN   ReadTextLine_ :BYTE
                EXTRN   strcmp_ :BYTE
                EXTRN   _Token_Count :BYTE
                EXTRN   AsmGetSymbol_ :BYTE
                EXTRN   _Parse_Pass :BYTE
                EXTRN   InputQueueLine_ :BYTE
                EXTRN   _MacroLocalVarCounter :BYTE
                EXTRN   PushLineQueue_ :BYTE
                EXTRN   AsmError_ :BYTE
                EXTRN   _AsmBuffer :BYTE
                EXTRN   strnicmp_ :BYTE
                EXTRN   wipe_space_ :BYTE
                EXTRN   __IsTable :BYTE
                EXTRN   AsmFree_ :BYTE
                EXTRN   InternalError_ :BYTE
                EXTRN   sprintf_ :BYTE
                EXTRN   strcat_ :BYTE
                EXTRN   strncpy_ :BYTE
                EXTRN   strncmp_ :BYTE
                EXTRN   strcpy_ :BYTE
                EXTRN   strlen_ :BYTE
                EXTRN   AsmAlloc_ :BYTE
                EXTRN   __STK :BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
macro_TEXT      SEGMENT BYTE PUBLIC  'CODE'
                ASSUME  CS:macro_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  ExpandMacro_
                PUBLIC  MacroDef_
L1              DB      4dH,61H,63H,72H,6fH,20H,65H,78H
                DB      70H,61H,6eH,73H,69H,6fH,6eH,3aH
                DB      20H,20H,25H,73H,20H,0aH,00H
L2              DB      2eH,2eH,5cH,63H,5cH,6dH,61H,63H
                DB      72H,6fH,2eH,63H,00H
L3              DB      20H,54H,45H,58H,54H,45H,51H,55H
                DB      20H,00H
L4              DB      3fH,3fH,25H,23H,30H,34H,64H,00H
L5              DB      6cH,6fH,63H,61H,6cH,00H
L6              DB      6dH,61H,63H,72H,6fH,00H
L7              DB      65H,6eH,64H,6dH,00H
L8              DB      52H,45H,51H,00H
L9              DB      25H,32H,64H,00H
L10             DB      23H,00H
asmline_insert_: push    bp
                mov     bp,sp
                push    ax
                mov     ax,001cH
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,0014H
                mov     word ptr -18H[bp],ax
                mov     word ptr -16H[bp],dx
                mov     word ptr -14H[bp],bx
                mov     word ptr -12H[bp],cx
                mov     ax,word ptr -18H[bp]
                mov     word ptr -0cH[bp],ax
                mov     ax,word ptr -16H[bp]
                mov     word ptr -0aH[bp],ax
L11:            lds     bx,dword ptr -0cH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -0cH[bp]
                mov     ax,word ptr +2H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L13
                cmp     dx,cx
                jne     short L13
                jmp     short L14
L12:            lds     bx,dword ptr -0cH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -0cH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -0cH[bp],dx
                mov     word ptr -0aH[bp],ax
                jmp     short L11
L13:            jmp     short L12
L14:            mov     ax,0009H
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr -8H[bp],cx
                mov     word ptr -6H[bp],bx
                lds     bx,dword ptr -8H[bp]
                mov     word ptr [bx],0000H
                lds     bx,dword ptr -8H[bp]
                mov     word ptr +2H[bx],0000H
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                lds     si,dword ptr -8H[bp]
                mov     word ptr +4H[si],cx
                lds     si,dword ptr -8H[bp]
                mov     word ptr +6H[si],bx
                lds     bx,dword ptr -8H[bp]
                mov     byte ptr +8H[bx],00H
                mov     si,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                lds     bx,dword ptr -8H[bp]
                mov     bx,word ptr +4H[bx]
                lds     di,dword ptr -8H[bp]
                mov     dx,word ptr +6H[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     bx,word ptr -8H[bp]
                lds     si,dword ptr -0cH[bp]
                mov     word ptr [si],bx
                mov     bx,word ptr -6H[bp]
                lds     si,dword ptr -0cH[bp]
                mov     word ptr +2H[si],bx
                mov     bx,word ptr -8H[bp]
                mov     word ptr -10H[bp],bx
                mov     bx,word ptr -6H[bp]
                mov     word ptr -0eH[bp],bx
                mov     ax,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret
replace_parm_:  push    bp
                mov     bp,sp
                push    ax
                mov     ax,0038H
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,0026H
                mov     word ptr -1cH[bp],ax
                mov     word ptr -1aH[bp],dx
                mov     word ptr -20H[bp],bx
                mov     word ptr -1eH[bp],cx
                mov     byte ptr -8H[bp],00H
                lds     bx,dword ptr +6H[bp]
                mov     ax,word ptr +4H[bx]
                mov     word ptr -18H[bp],ax
                lds     bx,dword ptr +6H[bp]
                mov     ax,word ptr +6H[bx]
                mov     word ptr -16H[bp],ax
                mov     ax,word ptr -1cH[bp]
                mov     word ptr -10H[bp],ax
                mov     ax,word ptr -1aH[bp]
                mov     word ptr -0eH[bp],ax
L15:            mov     cx,word ptr -10H[bp]
                mov     bx,word ptr -0eH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L17
                cmp     cx,dx
                jne     short L17
                jmp     near ptr L28
L16:            lds     bx,dword ptr -10H[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -10H[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -10H[bp],dx
                mov     word ptr -0eH[bp],ax
                jmp     short L15
L17:            lds     bx,dword ptr -10H[bp]
                mov     dx,word ptr +4H[bx]
                lds     bx,dword ptr -10H[bp]
                mov     ax,word ptr +6H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L18
                cmp     dx,cx
                je      short L19
L18:            lds     bx,dword ptr -10H[bp]
                mov     bx,word ptr +4H[bx]
                lds     si,dword ptr -10H[bp]
                mov     dx,word ptr +6H[si]
                mov     ax,bx
                call    far ptr strlen_
                mov     dl,byte ptr +4H[bp]
                xor     dh,dh
                cmp     ax,dx
                je      short L20
L19:            jmp     short L21
L20:            mov     dl,byte ptr +4H[bp]
                xor     dh,dh
                push    dx
                lds     si,dword ptr -10H[bp]
                mov     si,word ptr +4H[si]
                lds     bx,dword ptr -10H[bp]
                mov     cx,word ptr +6H[bx]
                mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strncmp_
                test    ax,ax
                je      short L22
L21:            jmp     near ptr L27
L22:            mov     bx,word ptr -18H[bp]
                mov     dx,word ptr -16H[bp]
                mov     ax,bx
                call    far ptr strlen_
                mov     dx,ax
                mov     al,byte ptr +4H[bp]
                xor     ah,ah
                sub     dx,ax
                add     dx,0004H
                mov     ax,dx
                call    far ptr AsmAlloc_
                mov     bx,dx
                mov     word ptr -14H[bp],ax
                mov     word ptr -12H[bp],bx
                mov     dx,word ptr -20H[bp]
                mov     ax,word ptr -1eH[bp]
                sub     dx,word ptr -18H[bp]
                mov     byte ptr -6H[bp],dl
                lds     bx,dword ptr -20H[bp]
                cmp     byte ptr -1H[bx],26H
                jne     short L23
                mov     al,byte ptr -6H[bp]
                dec     byte ptr -6H[bp]
L23:            mov     dl,byte ptr -6H[bp]
                xor     dh,dh
                push    dx
                mov     si,word ptr -18H[bp]
                mov     cx,word ptr -16H[bp]
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strncpy_
                mov     dl,byte ptr -6H[bp]
                xor     dh,dh
                lds     bx,dword ptr -14H[bp]
                add     bx,dx
                mov     byte ptr [bx],00H
                mov     cx,cs
                mov     dx,offset L10
                mov     si,dx
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     dl,byte ptr -8H[bp]
                xor     dh,dh
                push    dx
                mov     dx,cs
                mov     bx,offset L9
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-2aH[bp]
                push    dx
                push    bx
                call    far ptr sprintf_
                add     sp,000aH
                cmp     ax,0002H
                je      short L24
                mov     cx,0094H
                mov     dx,cs
                mov     ax,offset L2
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                jmp     far ptr InternalError_
L24:            cmp     byte ptr -2aH[bp],20H
                jne     short L25
                mov     byte ptr -2aH[bp],30H
L25:            mov     cx,ss
                lea     dx,-2aH[bp]
                mov     si,dx
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     dl,byte ptr +4H[bp]
                xor     dh,dh
                lds     bx,dword ptr -20H[bp]
                add     bx,dx
                cmp     byte ptr [bx],26H
                jne     short L26
                mov     al,byte ptr +4H[bp]
                inc     byte ptr +4H[bp]
L26:            mov     al,byte ptr +4H[bp]
                xor     ah,ah
                mov     cx,word ptr -1eH[bp]
                mov     si,word ptr -20H[bp]
                add     si,ax
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     ax,word ptr -14H[bp]
                lds     bx,dword ptr +6H[bp]
                mov     word ptr +4H[bx],ax
                mov     ax,word ptr -12H[bp]
                lds     bx,dword ptr +6H[bp]
                mov     word ptr +6H[bx],ax
                lds     bx,dword ptr +6H[bp]
                inc     byte ptr +8H[bx]
                mov     bx,word ptr -18H[bp]
                mov     dx,word ptr -16H[bp]
                mov     ax,bx
                call    far ptr AsmFree_
                mov     ax,word ptr -12H[bp]
                mov     word ptr -0aH[bp],ax
                mov     ax,word ptr -14H[bp]
                add     ax,0003H
                mov     word ptr -0cH[bp],ax
                jmp     short L29
L27:            mov     al,byte ptr -8H[bp]
                inc     byte ptr -8H[bp]
                jmp     near ptr L16
L28:            mov     al,byte ptr +4H[bp]
                xor     ah,ah
                mov     dx,ax
                mov     ax,word ptr -1eH[bp]
                mov     word ptr -0aH[bp],ax
                mov     bx,word ptr -20H[bp]
                add     bx,dx
                mov     word ptr -0cH[bp],bx
L29:            mov     ax,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret     0006H
put_parm_placeholders_in_line_:
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0026H
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,0018H
                mov     word ptr -1cH[bp],ax
                mov     word ptr -1aH[bp],dx
                mov     word ptr -18H[bp],bx
                mov     word ptr -16H[bp],cx
                mov     byte ptr -6H[bp],00H
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr +4H[bx]
                mov     word ptr -10H[bp],ax
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr +6H[bx]
                mov     word ptr -0eH[bp],ax
                mov     ax,word ptr -10H[bp]
                mov     word ptr -14H[bp],ax
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -12H[bp],ax
L30:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L32
                jmp     near ptr L73
L31:            jmp     short L30
L32:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L34
                jmp     near ptr L50
L33:            mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                inc     word ptr -14H[bp]
                jmp     short L32
L34:            lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],0c0H
                jne     short L35
                lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L36
L35:            jmp     short L37
L36:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],5fH
                jne     short L38
L37:            jmp     short L39
L38:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],40H
                jne     short L40
L39:            jmp     short L41
L40:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],24H
                jne     short L42
L41:            jmp     short L43
L42:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],3fH
                jne     short L45
L43:            mov     cx,word ptr -14H[bp]
                mov     bx,word ptr -12H[bp]
                mov     dx,word ptr -10H[bp]
                mov     ax,word ptr -0eH[bp]
                cmp     bx,ax
                jne     short L44
                cmp     cx,dx
                je      short L50
L44:            jmp     short L33
L45:            lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L48
L46:            lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L47
                mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                inc     word ptr -14H[bp]
                jmp     short L46
L47:            jmp     short L50
L48:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],22H
                jne     short L49
                mov     al,byte ptr -6H[bp]
                xor     ah,ah
                inc     ax
                mov     bx,0002H
                cwd
                idiv    bx
                mov     byte ptr -6H[bp],dl
                mov     ax,word ptr -14H[bp]
                mov     bx,word ptr -12H[bp]
                inc     word ptr -14H[bp]
                jmp     short L50
L49:            mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                inc     word ptr -14H[bp]
L50:            mov     bx,word ptr -14H[bp]
                mov     word ptr -0cH[bp],bx
                mov     bx,word ptr -12H[bp]
                mov     word ptr -0aH[bp],bx
L51:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L53
                jmp     near ptr L64
L52:            mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                inc     word ptr -14H[bp]
                jmp     short L51
L53:            lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],0c0H
                jne     short L54
                lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L55
L54:            jmp     short L56
L55:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],5fH
                jne     short L57
L56:            jmp     short L58
L57:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],40H
                jne     short L59
L58:            jmp     short L60
L59:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],24H
                jne     short L61
L60:            jmp     short L62
L61:            lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],3fH
                jne     short L63
L62:            jmp     short L52
L63:            lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                jne     short L64
                lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],22H
                jne     short L64
                mov     bl,byte ptr -6H[bp]
                xor     bh,bh
                lea     ax,+1H[bx]
                mov     bx,0002H
                cwd
                idiv    bx
                mov     byte ptr -6H[bp],dl
L64:            mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                sub     dx,word ptr -0cH[bp]
                mov     byte ptr -8H[bp],dl
                cmp     byte ptr -6H[bp],00H
                je      short L65
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],26H
                jne     short L66
L65:            jmp     short L67
L66:            lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr -1H[bx],26H
                jne     short L68
L67:            jmp     short L69
L68:            mov     bl,byte ptr -8H[bp]
                xor     bh,bh
                mov     ds,word ptr -0aH[bp]
                add     bx,word ptr -0cH[bp]
                cmp     byte ptr +1H[bx],26H
                jne     short L72
L69:            lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],00H
                je      short L70
                cmp     byte ptr -8H[bp],00H
                ja      short L71
L70:            jmp     short L72
L71:            mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                push    dx
                push    bx
                mov     dl,byte ptr -8H[bp]
                xor     dh,dh
                push    dx
                mov     si,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr -18H[bp]
                mov     dx,word ptr -16H[bp]
                mov     ax,bx
                mov     bx,si
                call    near ptr replace_parm_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr -14H[bp],cx
                mov     word ptr -12H[bp],bx
L72:            jmp     near ptr L31
L73:            lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret
lineis_:        push    bp
                mov     bp,sp
                push    ax
                mov     ax,0016H
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,000cH
                mov     word ptr -10H[bp],ax
                mov     word ptr -0eH[bp],dx
                mov     word ptr -0cH[bp],bx
                mov     word ptr -0aH[bp],cx
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    far ptr strlen_
                mov     word ptr -8H[bp],ax
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                call    far ptr wipe_space_
                mov     ax,word ptr -8H[bp]
                push    ax
                mov     si,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strnicmp_
                test    ax,ax
                je      short L74
                mov     byte ptr -6H[bp],00H
                jmp     short L78
L74:            lds     bx,dword ptr -10H[bp]
                add     bx,word ptr -8H[bp]
                cmp     byte ptr [bx],00H
                je      short L75
                lds     bx,dword ptr -10H[bp]
                add     bx,word ptr -8H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L76
L75:            jmp     short L77
L76:            mov     byte ptr -6H[bp],00H
                jmp     short L78
L77:            mov     byte ptr -6H[bp],01H
L78:            mov     al,byte ptr -6H[bp]
                lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret
macro_local_:   push    bp
                mov     bp,sp
                mov     ax,011cH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0104H
                mov     word ptr -0cH[bp],0000H
                mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     word ptr +2H[bx],0000H
                jne     short L79
                cmp     word ptr [bx],016bH
                je      short L80
L79:            mov     ax,0033H
                call    far ptr AsmError_
                mov     word ptr -0eH[bp],0ffffH
                jmp     near ptr L87
L80:            call    far ptr PushLineQueue_
                mov     ax,word ptr -0cH[bp]
                inc     word ptr -0cH[bp]
L81:            mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],00H
                jne     short L83
                jmp     near ptr L86
L82:            mov     ax,word ptr -0cH[bp]
                inc     word ptr -0cH[bp]
                jmp     short L81
L83:            mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],03H
                je      short L84
                mov     ax,0067H
                call    far ptr AsmError_
                mov     word ptr -0eH[bp],0ffffH
                jmp     near ptr L87
L84:            mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     di,dword ptr _AsmBuffer[si]
                mov     si,word ptr +5H[di]
                mov     cx,word ptr +7H[di]
                mov     dx,ss
                lea     ax,-10eH[bp]
                mov     di,ax
                mov     ax,di
                mov     bx,si
                call    far ptr strcpy_
                mov     si,cs
                mov     ax,offset L3
                mov     di,ax
                mov     dx,ss
                lea     ax,-10eH[bp]
                mov     cx,ax
                mov     ax,cx
                mov     bx,di
                mov     cx,si
                call    far ptr strcat_
                push    word ptr ss:_MacroLocalVarCounter
                mov     dx,cs
                mov     cx,offset L4
                push    dx
                push    cx
                mov     dx,ss
                lea     si,-10eH[bp]
                mov     cx,si
                mov     ax,cx
                call    far ptr strlen_
                mov     cx,ax
                mov     ax,ss
                lea     dx,-10eH[bp]
                add     cx,dx
                push    ax
                push    cx
                call    far ptr sprintf_
                add     sp,000aH
                inc     word ptr ss:_MacroLocalVarCounter
                mov     dx,ss
                lea     ax,-10eH[bp]
                mov     cx,ax
                mov     ax,cx
                call    far ptr InputQueueLine_
                mov     ax,word ptr -0cH[bp]
                inc     word ptr -0cH[bp]
                mov     di,word ptr -0cH[bp]
                shl     di,1
                shl     di,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     si,dword ptr _AsmBuffer[di]
                cmp     byte ptr +4H[si],00H
                je      short L86
                mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                je      short L85
                mov     ax,0021H
                call    far ptr AsmError_
                mov     word ptr -0eH[bp],0ffffH
                jmp     short L87
L85:            jmp     near ptr L82
L86:            mov     word ptr -0eH[bp],0001H
L87:            mov     ax,word ptr -0eH[bp]
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
macro_exam_:    push    bp
                mov     bp,sp
                push    ax
                mov     ax,0138H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,012aH
                mov     word ptr -0eH[bp],ax
                mov     word ptr -10H[bp],0000H
                cmp     byte ptr ss:_Parse_Pass,00H
                jne     short L88
                mov     si,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     ax,word ptr +5H[bx]
                mov     word ptr -2cH[bp],ax
                mov     ax,word ptr +7H[bx]
                mov     word ptr -2aH[bp],ax
                mov     bx,word ptr -2cH[bp]
                mov     dx,word ptr -2aH[bp]
                mov     ax,bx
                call    far ptr AsmGetSymbol_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -28H[bp],bx
                mov     word ptr -26H[bp],ax
                lds     bx,dword ptr -28H[bp]
                mov     ax,word ptr +1aH[bx]
                mov     word ptr -18H[bp],ax
                lds     bx,dword ptr -28H[bp]
                mov     ax,word ptr +1cH[bx]
                mov     word ptr -16H[bp],ax
L88:            mov     ax,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                cmp     byte ptr ss:_Parse_Pass,00H
                je      short L89
                jmp     near ptr L106
L89:            mov     ax,word ptr -0eH[bp]
                cmp     ax,word ptr ss:_Token_Count
                jl      short L91
                jmp     near ptr L106
L90:            jmp     short L89
L91:            mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     ax,word ptr +5H[bx]
                mov     word ptr -14H[bp],ax
                mov     ax,word ptr +7H[bx]
                mov     word ptr -12H[bp],ax
                mov     ax,0011H
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -30H[bp],bx
                mov     word ptr -2eH[bp],ax
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +0dH[bx],0000H
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +0fH[bx],0000H
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +8H[bx],0000H
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +0aH[bx],0000H
                lds     bx,dword ptr -30H[bp]
                mov     byte ptr +0cH[bx],00H
                mov     cx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,cx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     ax,dx
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +4H[bx],cx
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +6H[bx],ax
                mov     si,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                lds     bx,dword ptr -30H[bp]
                mov     bx,word ptr +4H[bx]
                lds     di,dword ptr -30H[bp]
                mov     dx,word ptr +6H[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],3aH
                je      short L92
                jmp     near ptr L96
L92:            mov     bx,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                les     si,dword ptr +5H[bx]
                cmp     byte ptr es:[si],3dH
                je      short L93
                jmp     near ptr L95
L93:            mov     bx,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],05H
                je      short L94
                mov     ax,0033H
                call    far ptr AsmError_
                mov     word ptr -0cH[bp],0ffffH
                jmp     near ptr L122
L94:            mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     ax,word ptr +5H[bx]
                mov     word ptr -14H[bp],ax
                mov     ax,word ptr +7H[bx]
                mov     word ptr -12H[bp],ax
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                lds     si,dword ptr -30H[bp]
                mov     word ptr +0dH[si],bx
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +0fH[bx],ax
                mov     si,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                lds     bx,dword ptr -30H[bp]
                mov     bx,word ptr +0dH[bx]
                lds     di,dword ptr -30H[bp]
                mov     dx,word ptr +0fH[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                jmp     short L96
L95:            mov     cx,cs
                mov     dx,offset L8
                mov     si,dx
                mov     bx,word ptr -0eH[bp]
                shl     bx,1
                shl     bx,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     di,dword ptr _AsmBuffer[bx]
                mov     bx,word ptr +5H[di]
                mov     dx,word ptr +7H[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcmp_
                test    ax,ax
                jne     short L96
                lds     bx,dword ptr -30H[bp]
                mov     byte ptr +0cH[bx],01H
                mov     bx,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
L96:            mov     ax,word ptr -0eH[bp]
                cmp     ax,word ptr ss:_Token_Count
                jge     short L97
                mov     si,word ptr -0eH[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                jne     short L98
L97:            jmp     short L99
L98:            mov     ax,0021H
                call    far ptr AsmError_
                mov     word ptr -0cH[bp],0ffffH
                jmp     near ptr L122
L99:            mov     ax,word ptr -0eH[bp]
                inc     word ptr -0eH[bp]
                lds     bx,dword ptr -30H[bp]
                mov     word ptr [bx],0000H
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +2H[bx],0000H
                lds     bx,dword ptr -18H[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -18H[bp]
                mov     ax,word ptr +2H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L100
                cmp     dx,cx
                jne     short L100
                mov     bx,word ptr -30H[bp]
                lds     si,dword ptr -18H[bp]
                mov     word ptr [si],bx
                mov     bx,word ptr -2eH[bp]
                lds     si,dword ptr -18H[bp]
                mov     word ptr +2H[si],bx
                jmp     short L105
L100:           lds     bx,dword ptr -18H[bp]
                mov     ax,word ptr [bx]
                mov     word ptr -24H[bp],ax
                lds     bx,dword ptr -18H[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -22H[bp],ax
                jmp     short L102
L101:           lds     bx,dword ptr -24H[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -24H[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -24H[bp],dx
                mov     word ptr -22H[bp],ax
L102:           lds     bx,dword ptr -24H[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -24H[bp]
                mov     ax,word ptr +2H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L103
                cmp     dx,cx
                je      short L104
L103:           jmp     short L101
L104:           mov     bx,word ptr -30H[bp]
                lds     si,dword ptr -24H[bp]
                mov     word ptr [si],bx
                mov     bx,word ptr -2eH[bp]
                lds     si,dword ptr -24H[bp]
                mov     word ptr +2H[si],bx
L105:           jmp     near ptr L90
L106:           mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                call    far ptr ReadTextLine_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -1cH[bp],bx
                mov     word ptr -1aH[bp],ax
                mov     dx,word ptr -1cH[bp]
                mov     ax,word ptr -1aH[bp]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L107
                cmp     dx,cx
                jne     short L107
                mov     ax,0087H
                call    far ptr AsmError_
                mov     word ptr -0cH[bp],0ffffH
                jmp     near ptr L122
L107:           mov     cx,cs
                mov     dx,offset L7
                mov     si,dx
                mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                mov     bx,si
                call    near ptr lineis_
                test    al,al
                je      short L109
                cmp     word ptr -10H[bp],0000H
                je      short L108
                mov     ax,word ptr -10H[bp]
                dec     word ptr -10H[bp]
                jmp     short L109
L108:           mov     word ptr -0cH[bp],0001H
                jmp     near ptr L122
L109:           mov     ax,word ptr -1cH[bp]
                mov     word ptr -134H[bp],ax
                mov     ax,word ptr -1aH[bp]
                mov     word ptr -132H[bp],ax
L110:           lds     bx,dword ptr -134H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L111
                mov     dx,word ptr -134H[bp]
                mov     ax,word ptr -132H[bp]
                inc     word ptr -134H[bp]
                jmp     short L110
L111:           mov     cx,word ptr -134H[bp]
                mov     bx,word ptr -132H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L112
                cmp     cx,dx
                je      short L113
L112:           lds     bx,dword ptr -134H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L114
L113:           jmp     short L115
L114:           mov     dx,word ptr -134H[bp]
                mov     ax,word ptr -132H[bp]
                inc     word ptr -134H[bp]
                jmp     short L111
L115:           mov     cx,word ptr -134H[bp]
                mov     bx,word ptr -132H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L116
                cmp     cx,dx
                je      short L117
L116:           lds     bx,dword ptr -134H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                jne     short L118
L117:           jmp     short L119
L118:           mov     dx,word ptr -134H[bp]
                mov     ax,word ptr -132H[bp]
                inc     word ptr -134H[bp]
                jmp     short L115
L119:           mov     cx,cs
                mov     dx,offset L6
                mov     si,dx
                mov     bx,word ptr -134H[bp]
                mov     dx,word ptr -132H[bp]
                mov     ax,bx
                mov     bx,si
                call    near ptr lineis_
                test    al,al
                je      short L120
                mov     ax,word ptr -10H[bp]
                inc     word ptr -10H[bp]
L120:           cmp     byte ptr ss:_Parse_Pass,00H
                jne     short L121
                mov     si,word ptr -1cH[bp]
                mov     cx,word ptr -1aH[bp]
                mov     dx,word ptr -16H[bp]
                mov     ax,word ptr -18H[bp]
                add     ax,0004H
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    near ptr asmline_insert_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr -20H[bp],cx
                mov     word ptr -1eH[bp],bx
                lds     si,dword ptr -18H[bp]
                mov     si,word ptr [si]
                lds     bx,dword ptr -18H[bp]
                mov     cx,word ptr +2H[bx]
                mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                mov     bx,si
                call    near ptr put_parm_placeholders_in_line_
L121:           jmp     near ptr L106
L122:           mov     ax,word ptr -0cH[bp]
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
my_sprintf_:    push    bp
                mov     bp,sp
                push    ax
                mov     ax,0026H
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,001cH
                mov     word ptr -1cH[bp],ax
                mov     word ptr -1aH[bp],dx
                mov     word ptr -18H[bp],bx
                mov     word ptr -16H[bp],cx
                mov     byte ptr -6H[bp],00H
                lds     bx,dword ptr -1cH[bp]
                mov     byte ptr [bx],00H
                mov     ax,word ptr -18H[bp]
                mov     word ptr -10H[bp],ax
                mov     ax,word ptr -16H[bp]
                mov     word ptr -0eH[bp],ax
                mov     ax,word ptr -10H[bp]
                mov     word ptr -14H[bp],ax
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -12H[bp],ax
L123:           lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L125
                jmp     near ptr L141
L124:           mov     ax,word ptr -12H[bp]
                mov     word ptr -0eH[bp],ax
                mov     bx,word ptr -14H[bp]
                add     bx,0003H
                mov     word ptr -10H[bp],bx
                jmp     short L123
L125:           mov     bx,word ptr -10H[bp]
                mov     word ptr -14H[bp],bx
                mov     bx,word ptr -0eH[bp]
                mov     word ptr -12H[bp],bx
L126:           lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],23H
                jne     short L127
                lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr +1H[bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                jne     short L128
L127:           jmp     short L129
L128:           lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr +2H[bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                jne     short L130
L129:           mov     word ptr -1eH[bp],0001H
                jmp     short L131
L130:           mov     word ptr -1eH[bp],0000H
L131:           cmp     word ptr -1eH[bp],0000H
                je      short L132
                lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L134
L132:           jmp     short L135
L133:           mov     dx,word ptr -14H[bp]
                mov     ax,word ptr -12H[bp]
                inc     word ptr -14H[bp]
                jmp     short L126
L134:           jmp     short L133
L135:           lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L136
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                sub     bx,word ptr -10H[bp]
                mov     ax,bx
                push    ax
                mov     si,word ptr -10H[bp]
                mov     cx,word ptr -0eH[bp]
                mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strncat_
                mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                call    far ptr strlen_
                mov     word ptr -8H[bp],ax
                jmp     near ptr L142
L136:           lds     bx,dword ptr -14H[bp]
                mov     al,byte ptr +1H[bx]
                mov     byte ptr -0cH[bp],al
                lds     bx,dword ptr -14H[bp]
                mov     al,byte ptr +2H[bx]
                mov     byte ptr -0bH[bp],al
                mov     byte ptr -0aH[bp],00H
                mov     dx,ss
                lea     cx,-0cH[bp]
                mov     bx,cx
                mov     ax,bx
                call    far ptr atoi_
                mov     byte ptr -6H[bp],al
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                sub     bx,word ptr -10H[bp]
                push    bx
                mov     si,word ptr -10H[bp]
                mov     cx,word ptr -0eH[bp]
                mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strncat_
                mov     dl,byte ptr -6H[bp]
                xor     dh,dh
                cmp     dx,word ptr +4H[bp]
                jg      short L137
                mov     word ptr -20H[bp],0000H
                jmp     short L138
L137:           mov     cx,01a0H
                mov     dx,cs
                mov     ax,offset L2
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                jmp     far ptr InternalError_
L138:           mov     al,byte ptr -6H[bp]
                xor     ah,ah
                shl     ax,1
                shl     ax,1
                mov     bx,ax
                mov     ds,word ptr +8H[bp]
                add     bx,word ptr +6H[bp]
                mov     dx,word ptr [bx]
                mov     ax,word ptr +2H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L139
                cmp     dx,cx
                je      short L140
L139:           mov     dl,byte ptr -6H[bp]
                inc     byte ptr -6H[bp]
                xor     dh,dh
                shl     dx,1
                shl     dx,1
                mov     bx,dx
                mov     ds,word ptr +8H[bp]
                add     bx,word ptr +6H[bp]
                mov     si,word ptr [bx]
                mov     cx,word ptr +2H[bx]
                mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
L140:           jmp     near ptr L124
L141:           mov     bx,word ptr -1cH[bp]
                mov     dx,word ptr -1aH[bp]
                mov     ax,bx
                call    far ptr strlen_
                mov     word ptr -8H[bp],ax
L142:           mov     ax,word ptr -8H[bp]
                lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret     0006H
fill_in_parms_: push    bp
                mov     bp,sp
                push    ax
                mov     ax,012cH
                call    far ptr __STK
                pop     ax
                push    si
                push    di
                sub     sp,011eH
                mov     word ptr -16H[bp],ax
                mov     word ptr -14H[bp],dx
                mov     word ptr -1eH[bp],bx
                mov     word ptr -1cH[bp],cx
                mov     byte ptr -6H[bp],00H
                mov     ax,word ptr -1eH[bp]
                mov     word ptr -0aH[bp],ax
                mov     ax,word ptr -1cH[bp]
                mov     word ptr -8H[bp],ax
L143:           mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr -8H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L145
                cmp     cx,dx
                jne     short L145
                jmp     short L146
L144:           lds     bx,dword ptr -0aH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -0aH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -0aH[bp],dx
                mov     word ptr -8H[bp],ax
                jmp     short L143
L145:           mov     al,byte ptr -6H[bp]
                inc     byte ptr -6H[bp]
                jmp     short L144
L146:           mov     al,byte ptr -6H[bp]
                xor     ah,ah
                shl     ax,1
                shl     ax,1
                inc     ax
                and     al,0feH
                mov     dx,ax
                call    far ptr stackavail_
                cmp     dx,ax
                jae     short L147
                mov     al,byte ptr -6H[bp]
                xor     ah,ah
                shl     ax,1
                shl     ax,1
                inc     ax
                and     al,0feH
                sub     sp,ax
                mov     ax,sp
                mov     dx,ss
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -122H[bp],bx
                mov     word ptr -120H[bp],ax
                jmp     short L148
L147:           mov     word ptr -122H[bp],0000H
                mov     word ptr -120H[bp],0000H
L148:           mov     ax,word ptr -122H[bp]
                mov     word ptr -12H[bp],ax
                mov     ax,word ptr -120H[bp]
                mov     word ptr -10H[bp],ax
                mov     byte ptr -6H[bp],00H
                mov     bx,word ptr -1eH[bp]
                mov     word ptr -0aH[bp],bx
                mov     ax,word ptr -1cH[bp]
                mov     word ptr -8H[bp],ax
L149:           mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr -8H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L151
                cmp     cx,dx
                jne     short L151
                jmp     short L152
L150:           lds     bx,dword ptr -0aH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -0aH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -0aH[bp],dx
                mov     word ptr -8H[bp],ax
                jmp     short L149
L151:           mov     al,byte ptr -6H[bp]
                xor     ah,ah
                shl     ax,1
                shl     ax,1
                lds     bx,dword ptr -12H[bp]
                add     bx,ax
                les     si,dword ptr -0aH[bp]
                mov     dx,word ptr es:+8H[si]
                les     si,dword ptr -0aH[bp]
                mov     ax,word ptr es:+0aH[si]
                mov     word ptr [bx],dx
                mov     word ptr +2H[bx],ax
                mov     al,byte ptr -6H[bp]
                inc     byte ptr -6H[bp]
                jmp     short L150
L152:           mov     dx,word ptr -12H[bp]
                mov     ax,word ptr -10H[bp]
                push    ax
                push    dx
                mov     al,byte ptr -6H[bp]
                xor     ah,ah
                dec     ax
                push    ax
                lds     si,dword ptr -16H[bp]
                mov     si,word ptr +4H[si]
                lds     bx,dword ptr -16H[bp]
                mov     cx,word ptr +6H[bx]
                mov     dx,ss
                lea     ax,-11eH[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    near ptr my_sprintf_
                mov     dx,ss
                lea     ax,-11eH[bp]
                mov     cx,ax
                mov     ax,cx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -0eH[bp],bx
                mov     word ptr -0cH[bp],ax
                mov     cx,ss
                lea     ax,-11eH[bp]
                mov     si,ax
                mov     bx,word ptr -0eH[bp]
                mov     dx,word ptr -0cH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -1aH[bp],ax
                mov     ax,word ptr -0cH[bp]
                mov     word ptr -18H[bp],ax
                mov     ax,word ptr -1aH[bp]
                mov     dx,word ptr -18H[bp]
                lea     sp,-4H[bp]
                pop     di
                pop     si
                pop     bp
                ret
free_parmlist_: push    bp
                mov     bp,sp
                push    ax
                mov     ax,0014H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,0008H
                mov     word ptr -10H[bp],ax
                mov     word ptr -0eH[bp],dx
                mov     ax,word ptr -10H[bp]
                mov     word ptr -0cH[bp],ax
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -0aH[bp],ax
L153:           mov     cx,word ptr -0cH[bp]
                mov     bx,word ptr -0aH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L155
                cmp     cx,dx
                jne     short L155
                jmp     short L156
L154:           lds     bx,dword ptr -0cH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -0cH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -0cH[bp],dx
                mov     word ptr -0aH[bp],ax
                jmp     short L153
L155:           lds     bx,dword ptr -0cH[bp]
                mov     bx,word ptr +8H[bx]
                lds     si,dword ptr -0cH[bp]
                mov     dx,word ptr +0aH[si]
                mov     ax,bx
                call    far ptr AsmFree_
                lds     bx,dword ptr -0cH[bp]
                mov     word ptr +8H[bx],0000H
                lds     bx,dword ptr -0cH[bp]
                mov     word ptr +0aH[bx],0000H
                jmp     short L154
L156:           lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                ret
ExpandMacro_:   inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0140H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0128H
                mov     word ptr -18H[bp],ax
                mov     word ptr -28H[bp],0000H
                mov     word ptr -26H[bp],0000H
                mov     word ptr -14H[bp],0000H
                mov     byte ptr -0cH[bp],00H
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],00H
                jne     short L157
                mov     ax,word ptr -18H[bp]
                mov     word ptr -16H[bp],ax
                jmp     near ptr L222
L157:           mov     byte ptr -11H[bp],00H
L158:           mov     ax,word ptr -14H[bp]
                cmp     ax,word ptr -18H[bp]
                jl      short L160
                jmp     short L166
L159:           mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                jmp     short L158
L160:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],03H
                jne     short L161
                mov     bx,word ptr -14H[bp]
                shl     bx,1
                shl     bx,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     si,dword ptr _AsmBuffer[bx]
                mov     bx,word ptr +5H[si]
                mov     dx,word ptr +7H[si]
                mov     ax,bx
                call    far ptr AsmGetSymbol_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -28H[bp],bx
                mov     word ptr -26H[bp],ax
L161:           mov     cx,word ptr -28H[bp]
                mov     bx,word ptr -26H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L162
                cmp     cx,dx
                je      short L163
L162:           lds     bx,dword ptr -28H[bp]
                cmp     byte ptr +15H[bx],07H
                je      short L164
L163:           jmp     short L165
L164:           jmp     short L166
L165:           jmp     short L159
L166:           mov     cx,word ptr -28H[bp]
                mov     bx,word ptr -26H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L167
                cmp     cx,dx
                je      short L168
L167:           lds     bx,dword ptr -28H[bp]
                cmp     byte ptr +15H[bx],07H
                je      short L169
L168:           mov     ax,word ptr -18H[bp]
                mov     word ptr -16H[bp],ax
                jmp     near ptr L222
L169:           mov     ax,word ptr -14H[bp]
                mov     word ptr -10H[bp],ax
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer+4H[si]
                cmp     byte ptr +4H[bx],06H
                jne     short L170
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer+4H[si]
                cmp     word ptr +2H[bx],0000H
                jne     short L170
                cmp     word ptr [bx],017dH
                je      short L171
L170:           jmp     short L172
L171:           mov     ax,word ptr -18H[bp]
                mov     word ptr -16H[bp],ax
                jmp     near ptr L222
L172:           cmp     word ptr -10H[bp],0000H
                je      short L177
                call    far ptr PushLineQueue_
                mov     byte ptr -130H[bp],00H
                mov     word ptr -14H[bp],0000H
L173:           mov     ax,word ptr -14H[bp]
                cmp     ax,word ptr -10H[bp]
                jl      short L175
                jmp     short L176
L174:           mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                jmp     short L173
L175:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     si,word ptr +5H[bx]
                mov     cx,word ptr +7H[bx]
                mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                jmp     short L174
L176:           mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                call    far ptr InputQueueLine_
L177:           mov     ax,word ptr -10H[bp]
                inc     ax
                mov     word ptr -14H[bp],ax
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer+0fffcH[si]
                push    word ptr +7H[bx]
                push    word ptr +5H[bx]
                mov     ax,cs
                mov     bx,offset L1
                push    ax
                push    bx
                call    far ptr DoDebugMsg_
                add     sp,0008H
                mov     bx,word ptr -28H[bp]
                mov     word ptr -24H[bp],bx
                mov     ax,word ptr -26H[bp]
                mov     word ptr -22H[bp],ax
                lds     bx,dword ptr -24H[bp]
                mov     bx,word ptr +1aH[bx]
                mov     word ptr -1cH[bp],bx
                lds     bx,dword ptr -24H[bp]
                mov     ax,word ptr +1cH[bx]
                mov     word ptr -1aH[bp],ax
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr [bx]
                mov     word ptr -30H[bp],ax
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -2eH[bp],ax
L178:           mov     cx,word ptr -30H[bp]
                mov     bx,word ptr -2eH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L180
                cmp     cx,dx
                jne     short L180
                jmp     near ptr L215
L179:           lds     bx,dword ptr -30H[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -30H[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -30H[bp],dx
                mov     word ptr -2eH[bp],ax
                jmp     short L178
L180:           mov     byte ptr -130H[bp],00H
                mov     ax,word ptr -14H[bp]
                cmp     ax,word ptr -18H[bp]
                jl      short L181
                jmp     near ptr L213
L181:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                je      short L183
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],05H
                jne     short L182
                mov     bx,word ptr -14H[bp]
                shl     bx,1
                shl     bx,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     si,dword ptr _AsmBuffer[bx]
                mov     bx,word ptr +5H[si]
                mov     dx,word ptr +7H[si]
                mov     ax,bx
                call    far ptr strlen_
                test    ax,ax
                je      short L183
L182:           jmp     near ptr L190
L183:           lds     bx,dword ptr -30H[bp]
                cmp     byte ptr +0cH[bx],00H
                je      short L184
                mov     ax,0459H
                call    far ptr AsmError_
                mov     word ptr -16H[bp],0ffffH
                jmp     near ptr L222
L184:           lds     bx,dword ptr -30H[bp]
                mov     dx,word ptr +0dH[bx]
                lds     bx,dword ptr -30H[bp]
                mov     ax,word ptr +0fH[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L185
                cmp     dx,cx
                je      short L186
L185:           lds     bx,dword ptr -30H[bp]
                mov     bx,word ptr +0dH[bx]
                lds     si,dword ptr -30H[bp]
                mov     dx,word ptr +0fH[si]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                lds     si,dword ptr -30H[bp]
                mov     word ptr +8H[si],cx
                lds     si,dword ptr -30H[bp]
                mov     word ptr +0aH[si],bx
                lds     si,dword ptr -30H[bp]
                mov     si,word ptr +0dH[si]
                lds     bx,dword ptr -30H[bp]
                mov     cx,word ptr +0fH[bx]
                lds     bx,dword ptr -30H[bp]
                mov     bx,word ptr +8H[bx]
                lds     di,dword ptr -30H[bp]
                mov     dx,word ptr +0aH[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L186:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                je      short L189
                mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                mov     ax,word ptr -14H[bp]
                cmp     ax,word ptr -18H[bp]
                jge     short L187
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                jne     short L188
L187:           jmp     short L189
L188:           mov     ax,0021H
                call    far ptr AsmError_
                mov     word ptr -16H[bp],0ffffH
                jmp     near ptr L222
L189:           mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                jmp     near ptr L212
L190:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],00H
                jne     short L191
                jmp     near ptr L211
L191:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                les     si,dword ptr +5H[bx]
                cmp     byte ptr es:[si],25H
                jne     short L192
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                les     si,dword ptr +5H[bx]
                mov     byte ptr es:[si],20H
                mov     byte ptr -0cH[bp],01H
                mov     ax,word ptr -14H[bp]
                mov     word ptr -0eH[bp],ax
L192:           cmp     byte ptr -0cH[bp],00H
                je      short L193
                jmp     near ptr L201
L193:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                je      short L194
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     dx,word ptr +5H[bx]
                mov     ax,word ptr +7H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L195
                cmp     dx,cx
                jne     short L195
L194:           jmp     short L196
L195:           mov     ax,word ptr -14H[bp]
                cmp     ax,word ptr -18H[bp]
                jne     short L197
L196:           jmp     near ptr L211
L197:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],11H
                jbe     short L198
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                les     si,dword ptr +5H[bx]
                mov     al,byte ptr es:[si]
                mov     byte ptr -12H[bp],al
                mov     cx,ss
                lea     ax,-12H[bp]
                mov     si,ax
                mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                jmp     short L200
L198:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],0cH
                jne     short L199
                mov     di,000aH
                mov     dx,ss
                lea     ax,-130H[bp]
                mov     cx,ax
                mov     ax,cx
                call    far ptr strlen_
                mov     bx,ax
                mov     ax,ss
                lea     dx,-130H[bp]
                mov     cx,ax
                add     dx,bx
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     ax,word ptr [bx]
                mov     bx,dx
                mov     dx,di
                call    far ptr itoa_
                jmp     short L200
L199:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     si,word ptr +5H[bx]
                mov     cx,word ptr +7H[bx]
                mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
L200:           jmp     near ptr L210
L201:           xor     dx,dx
                mov     ax,word ptr -14H[bp]
                call    far ptr ExpandSymbol_
                mov     word ptr -132H[bp],ax
                cmp     word ptr -132H[bp],0ffffH
                jl      short L202
                cmp     word ptr -132H[bp],0ffffH
                jle     short L203
                cmp     word ptr -132H[bp],0002H
                je      short L204
                jmp     short L205
L202:           jmp     short L205
L203:           lds     bx,dword ptr -1cH[bp]
                mov     bx,word ptr [bx]
                lds     si,dword ptr -1cH[bp]
                mov     dx,word ptr +2H[si]
                mov     ax,bx
                call    near ptr free_parmlist_
                mov     word ptr -16H[bp],0ffffH
                jmp     near ptr L222
L204:           jmp     near ptr L190
L205:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer[si]
                cmp     byte ptr +4H[bx],2cH
                je      short L206
                mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     dx,word ptr +5H[bx]
                mov     ax,word ptr +7H[bx]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L207
                cmp     dx,cx
                jne     short L207
L206:           jmp     short L208
L207:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer+4H[si]
                cmp     byte ptr +4H[bx],00H
                jne     short L210
L208:           mov     si,word ptr -14H[bp]
                shl     si,1
                shl     si,1
                mov     bx,seg _AsmBuffer
                mov     ds,bx
                lds     bx,dword ptr _AsmBuffer+4H[si]
                cmp     byte ptr +4H[bx],00H
                jne     short L209
                mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
L209:           mov     bx,word ptr -14H[bp]
                dec     bx
                mov     dx,word ptr -0eH[bp]
                mov     ax,word ptr -18H[bp]
                call    far ptr EvalExpr_
                mov     word ptr -18H[bp],ax
                mov     byte ptr -0cH[bp],00H
                mov     ax,word ptr -18H[bp]
                mov     word ptr ss:_Token_Count,ax
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -14H[bp],ax
L210:           mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                jmp     near ptr L190
L211:           mov     ax,word ptr -14H[bp]
                inc     word ptr -14H[bp]
                mov     dx,ss
                lea     ax,-130H[bp]
                mov     bx,ax
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                lds     si,dword ptr -30H[bp]
                mov     word ptr +8H[si],bx
                lds     bx,dword ptr -30H[bp]
                mov     word ptr +0aH[bx],ax
                mov     cx,ss
                lea     ax,-130H[bp]
                mov     si,ax
                lds     bx,dword ptr -30H[bp]
                mov     bx,word ptr +8H[bx]
                lds     di,dword ptr -30H[bp]
                mov     dx,word ptr +0aH[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L212:           jmp     short L214
L213:           lds     bx,dword ptr -30H[bp]
                cmp     byte ptr +0cH[bx],00H
                je      short L214
                mov     ax,0459H
                call    far ptr AsmError_
                mov     word ptr -16H[bp],0ffffH
                jmp     near ptr L222
L214:           jmp     near ptr L179
L215:           call    far ptr PushLineQueue_
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr +4H[bx]
                mov     word ptr -2cH[bp],ax
                lds     bx,dword ptr -1cH[bp]
                mov     ax,word ptr +6H[bx]
                mov     word ptr -2aH[bp],ax
L216:           mov     cx,word ptr -2cH[bp]
                mov     bx,word ptr -2aH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L218
                cmp     cx,dx
                jne     short L218
                jmp     near ptr L221
L217:           lds     bx,dword ptr -2cH[bp]
                mov     dx,word ptr [bx]
                lds     bx,dword ptr -2cH[bp]
                mov     ax,word ptr +2H[bx]
                mov     word ptr -2cH[bp],dx
                mov     word ptr -2aH[bp],ax
                jmp     short L216
L218:           lds     si,dword ptr -1cH[bp]
                mov     si,word ptr [si]
                lds     bx,dword ptr -1cH[bp]
                mov     cx,word ptr +2H[bx]
                mov     bx,word ptr -2cH[bp]
                mov     dx,word ptr -2aH[bp]
                mov     ax,bx
                mov     bx,si
                call    near ptr fill_in_parms_
                mov     si,ax
                mov     cx,dx
                mov     word ptr -20H[bp],si
                mov     word ptr -1eH[bp],cx
                mov     si,cs
                mov     dx,offset L5
                mov     di,dx
                mov     cx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,cx
                mov     bx,di
                mov     cx,si
                call    near ptr lineis_
                test    al,al
                je      short L220
                mov     cx,seg _StringBuf
                mov     dx,offset _StringBuf
                mov     si,dx
                mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr AsmScan_
                call    near ptr macro_local_
                cmp     ax,0ffffH
                jne     short L219
                mov     word ptr -16H[bp],0ffffH
                jmp     short L222
L219:           mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                call    far ptr AsmFree_
                jmp     near ptr L217
L220:           mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                call    far ptr InputQueueLine_
                mov     bx,word ptr -20H[bp]
                mov     dx,word ptr -1eH[bp]
                mov     ax,bx
                call    far ptr AsmFree_
                jmp     near ptr L217
L221:           lds     bx,dword ptr -28H[bp]
                mov     bx,word ptr +4H[bx]
                lds     si,dword ptr -28H[bp]
                mov     dx,word ptr +6H[si]
                mov     ax,bx
                call    far ptr PushMacro_
                lds     bx,dword ptr -1cH[bp]
                mov     bx,word ptr [bx]
                lds     si,dword ptr -1cH[bp]
                mov     dx,word ptr +2H[si]
                mov     ax,bx
                call    near ptr free_parmlist_
                mov     word ptr -16H[bp],0000H
L222:           mov     ax,word ptr -16H[bp]
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                dec     bp
                retf
MacroDef_:      inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0024H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0014H
                mov     word ptr -0cH[bp],ax
                cmp     byte ptr ss:_Parse_Pass,00H
                je      short L223
                jmp     near ptr L229
L223:           cmp     word ptr -0cH[bp],0000H
                jge     short L224
                mov     ax,041eH
                call    far ptr AsmError_
                mov     word ptr -0eH[bp],0ffffH
                jmp     near ptr L235
L224:           mov     si,word ptr -0cH[bp]
                shl     si,1
                shl     si,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     bx,dword ptr _AsmBuffer[si]
                mov     ax,word ptr +5H[bx]
                mov     word ptr -16H[bp],ax
                mov     ax,word ptr +7H[bx]
                mov     word ptr -14H[bp],ax
                mov     bx,word ptr -16H[bp]
                mov     dx,word ptr -14H[bp]
                mov     ax,bx
                call    far ptr AsmGetSymbol_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -12H[bp],bx
                mov     word ptr -10H[bp],ax
                mov     cx,word ptr -12H[bp]
                mov     bx,word ptr -10H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                je      short L225
                jmp     near ptr L227
L225:           cmp     cx,dx
                je      short L226
                jmp     near ptr L227
L226:           mov     cx,0007H
                mov     bx,word ptr -16H[bp]
                mov     dx,word ptr -14H[bp]
                mov     ax,bx
                mov     bx,cx
                call    far ptr dir_insert_
                mov     bx,dx
                mov     word ptr -1aH[bp],ax
                mov     word ptr -18H[bp],bx
                call    far ptr get_curr_filename_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -16H[bp],bx
                mov     word ptr -14H[bp],ax
                mov     bx,word ptr -16H[bp]
                mov     dx,word ptr -14H[bp]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                lds     si,dword ptr -1aH[bp]
                mov     si,word ptr +1aH[si]
                lds     di,dword ptr -1aH[bp]
                mov     ds,word ptr +1cH[di]
                mov     word ptr +8H[si],bx
                mov     word ptr +0aH[si],ax
                mov     si,word ptr -16H[bp]
                mov     cx,word ptr -14H[bp]
                lds     di,dword ptr -1aH[bp]
                mov     di,word ptr +1aH[di]
                lds     bx,dword ptr -1aH[bp]
                mov     ds,word ptr +1cH[bx]
                mov     bx,word ptr +8H[di]
                mov     dx,word ptr +0aH[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     bx,word ptr -1aH[bp]
                mov     word ptr -12H[bp],bx
                mov     bx,word ptr -18H[bp]
                mov     word ptr -10H[bp],bx
                jmp     short L228
L227:           mov     ax,041fH
                call    far ptr AsmError_
                mov     word ptr -0eH[bp],0ffffH
                jmp     near ptr L235
L228:           lds     bx,dword ptr -12H[bp]
                mov     byte ptr +15H[bx],07H
                mov     ax,word ptr -0cH[bp]
                call    near ptr macro_exam_
                mov     word ptr -0eH[bp],ax
                jmp     near ptr L235
L229:           cmp     word ptr -0cH[bp],0000H
                jl      short L230
                mov     word ptr -1cH[bp],0000H
                jmp     short L231
L230:           mov     cx,028eH
                mov     dx,cs
                mov     ax,offset L2
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                jmp     far ptr InternalError_
L231:           mov     bx,word ptr -0cH[bp]
                shl     bx,1
                shl     bx,1
                mov     ax,seg _AsmBuffer
                mov     ds,ax
                lds     si,dword ptr _AsmBuffer[bx]
                mov     bx,word ptr +5H[si]
                mov     dx,word ptr +7H[si]
                mov     ax,bx
                call    far ptr AsmGetSymbol_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -1aH[bp],bx
                mov     word ptr -18H[bp],ax
                mov     si,word ptr -1aH[bp]
                mov     bx,word ptr -18H[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L232
                cmp     si,dx
                je      short L233
L232:           mov     word ptr -1eH[bp],0000H
                jmp     short L234
L233:           mov     cx,0291H
                mov     dx,cs
                mov     ax,offset L2
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                jmp     far ptr InternalError_
L234:           mov     ax,word ptr -0cH[bp]
                inc     ax
                call    near ptr macro_exam_
                mov     word ptr -0eH[bp],ax
L235:           mov     ax,word ptr -0eH[bp]
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                dec     bp
                retf
macro_TEXT      ENDS

CONST           SEGMENT WORD PUBLIC  'DATA'
CONST           ENDS

CONST2          SEGMENT WORD PUBLIC  'DATA'
CONST2          ENDS

_DATA           SEGMENT WORD PUBLIC  'DATA'
_DATA           ENDS

_BSS            SEGMENT WORD PUBLIC  'BSS'
_BSS            ENDS

$$SYMBOLS       SEGMENT BYTE  'DEBSYM'
                DB      06H,31H
                DD      asmline_insert_
                DB      25H,21H,00H,00H,0e9H,00H,00H,00H
                DB      13H,07H,02H,00H,31H,31H,08H,0bH
                DB      02H,31H,08H,0bH,31H,09H,0aH,61H
                DB      73H,6dH,6cH,69H,6eH,65H,5fH,69H
                DB      6eH,73H,65H,72H,74H,0aH,11H,10H
                DB      0f8H,2eH,65H,6eH,74H,72H,79H,08H
                DB      11H,10H,0f4H,2fH,70H,74H,72H,09H
                DB      11H,10H,0ecH,30H,64H,61H,74H,61H
                DB      09H,11H,10H,0e8H,2fH,68H,65H,61H
                DB      64H,23H,21H,0e9H,00H,30H,02H,00H
                DB      00H,13H,09H,02H,00H,37H,31H,08H
                DB      0bH,02H,31H,08H,0bH,31H,09H,0aH
                DB      72H,65H,70H,6cH,61H,63H,65H,5fH
                DB      70H,61H,72H,6dH,0bH,11H,10H,0d6H
                DB      38H,62H,75H,66H,66H,65H,72H,06H
                DB      11H,10H,0f0H,36H,70H,0dH,11H,10H
                DB      0ecH,11H,6eH,65H,77H,5fH,6cH,69H
                DB      6eH,65H,0dH,11H,10H,0e8H,11H,6fH
                DB      6cH,64H,5fH,6cH,69H,6eH,65H,0bH
                DB      11H,10H,0faH,08H,62H,65H,66H,6fH
                DB      72H,65H,0aH,11H,10H,0f8H,08H,63H
                DB      6fH,75H,6eH,74H,0cH,11H,10H,06H
                DB      2eH,6cH,73H,74H,72H,75H,63H,74H
                DB      08H,11H,10H,04H,08H,6cH,65H,6eH
                DB      0aH,11H,10H,0e0H,11H,73H,74H,61H
                DB      72H,74H,0aH,11H,10H,0e4H,36H,70H
                DB      61H,72H,6dH,73H,32H,21H,19H,03H
                DB      33H,02H,00H,00H,13H,07H,02H,00H
                DB      39H,00H,02H,31H,08H,0bH,31H,09H
                DB      0aH,70H,75H,74H,5fH,70H,61H,72H
                DB      6dH,5fH,70H,6cH,61H,63H,65H,68H
                DB      6fH,6cH,64H,65H,72H,73H,5fH,69H
                DB      6eH,5fH,6cH,69H,6eH,65H,09H,11H
                DB      10H,0f0H,11H,6cH,69H,6eH,65H,08H
                DB      11H,10H,0ecH,11H,74H,6dH,70H,0aH
                DB      11H,10H,0f4H,11H,73H,74H,61H,72H
                DB      74H,0aH,11H,10H,0faH,08H,71H,75H
                DB      6fH,74H,65H,08H,11H,10H,0f8H,08H
                DB      6cH,65H,6eH,0aH,11H,10H,0e8H,36H
                DB      70H,61H,72H,6dH,73H,0fH,11H,10H
                DB      0e4H,2eH,6cH,69H,6eH,65H,73H,74H
                DB      72H,75H,63H,74H,1bH,21H,4cH,05H
                DB      94H,00H,00H,00H,13H,07H,02H,00H
                DB      3aH,40H,02H,31H,08H,0bH,31H,09H
                DB      0aH,6cH,69H,6eH,65H,69H,73H,08H
                DB      11H,10H,0f8H,06H,6cH,65H,6eH,0bH
                DB      11H,10H,0f4H,11H,73H,75H,62H,73H
                DB      74H,72H,08H,11H,10H,0f0H,11H,73H
                DB      74H,72H,1aH,21H,0e0H,05H,71H,01H
                DB      00H,00H,14H,0aH,02H,00H,3bH,48H
                DB      00H,6dH,61H,63H,72H,6fH,5fH,6cH
                DB      6fH,63H,61H,6cH,06H,11H,10H,0f4H
                DB      06H,69H,0cH,11H,11H,0f2H,0feH,3cH
                DB      62H,75H,66H,66H,65H,72H,08H,20H
                DB      3aH,0aH,65H,01H,0c5H,01H,09H,11H
                DB      11H,0ccH,0feH,11H,70H,74H,72H,1aH
                DB      21H,51H,07H,5eH,04H,00H,00H,16H
                DB      0aH,02H,00H,3dH,48H,01H,48H,6dH
                DB      61H,63H,72H,6fH,5fH,65H,78H,61H
                DB      6dH,09H,11H,10H,0e8H,40H,69H,6eH
                DB      66H,6fH,0bH,11H,10H,0e4H,11H,73H
                DB      74H,72H,69H,6eH,67H,0aH,11H,10H
                DB      0ecH,11H,74H,6fH,6bH,65H,6eH,09H
                DB      11H,10H,0d4H,11H,6eH,61H,6dH,65H
                DB      0dH,11H,10H,0d0H,36H,70H,61H,72H
                DB      61H,6eH,6fH,64H,65H,0dH,11H,10H
                DB      0dcH,36H,70H,61H,72H,61H,63H,75H
                DB      72H,72H,0fH,11H,10H,0e0H,2eH,6cH
                DB      69H,6eH,65H,73H,74H,72H,75H,63H
                DB      74H,0cH,11H,11H,0d0H,0feH,41H,62H
                DB      75H,66H,66H,65H,72H,09H,11H,10H
                DB      0d8H,80H,0baH,64H,69H,72H,12H,11H
                DB      10H,0f0H,17H,6eH,65H,73H,74H,69H
                DB      6eH,67H,5fH,64H,65H,70H,74H,68H
                DB      06H,11H,10H,0f2H,06H,69H,20H,21H
                DB      0afH,0bH,0cfH,01H,00H,00H,13H,09H
                DB      02H,00H,80H,0bcH,48H,02H,31H,08H
                DB      0bH,31H,09H,0aH,6dH,79H,5fH,73H
                DB      70H,72H,69H,6eH,74H,66H,0cH,11H
                DB      10H,0f4H,80H,0bdH,62H,75H,66H,66H
                DB      65H,72H,0aH,11H,10H,0f0H,11H,73H
                DB      74H,61H,72H,74H,08H,11H,10H,0ecH
                DB      11H,65H,6eH,64H,0bH,11H,10H,0faH
                DB      08H,70H,61H,72H,6dH,6eH,6fH,0aH
                DB      11H,10H,06H,80H,0bbH,61H,72H,67H
                DB      76H,09H,11H,10H,04H,06H,61H,72H
                DB      67H,63H,0bH,11H,10H,0e8H,11H,66H
                DB      6fH,72H,6dH,61H,74H,09H,11H,10H
                DB      0e4H,11H,64H,65H,73H,74H,25H,21H
                DB      7eH,0dH,88H,01H,00H,00H,13H,07H
                DB      02H,00H,80H,0beH,31H,08H,0bH,02H
                DB      31H,08H,0bH,31H,09H,0aH,66H,69H
                DB      6cH,6cH,5fH,69H,6eH,5fH,70H,61H
                DB      72H,6dH,73H,0dH,11H,11H,0e2H,0feH
                DB      80H,0bfH,62H,75H,66H,66H,65H,72H
                DB      09H,11H,10H,0f6H,36H,70H,61H,72H
                DB      6dH,0dH,11H,10H,0f2H,11H,6eH,65H
                DB      77H,5fH,6cH,69H,6eH,65H,10H,11H
                DB      10H,0eeH,80H,0bbH,70H,61H,72H,6dH
                DB      5fH,61H,72H,72H,61H,79H,0aH,11H
                DB      10H,0faH,08H,63H,6fH,75H,6eH,74H
                DB      0dH,11H,10H,0e2H,36H,70H,61H,72H
                DB      6dH,6cH,69H,73H,74H,0aH,11H,10H
                DB      0eaH,2eH,6cH,6eH,6fH,64H,65H,20H
                DB      21H,06H,0fH,7cH,00H,00H,00H,15H
                DB      09H,02H,00H,80H,0c0H,00H,01H,31H
                DB      08H,0bH,66H,72H,65H,65H,5fH,70H
                DB      61H,72H,6dH,6cH,69H,73H,74H,09H
                DB      11H,10H,0f4H,36H,70H,61H,72H,6dH
                DB      09H,11H,10H,0f0H,36H,68H,65H,61H
                DB      64H,1cH,22H,82H,0fH,0e2H,06H,00H
                DB      00H,17H,0bH,02H,00H,80H,0c1H,48H
                DB      01H,48H,45H,78H,70H,61H,6eH,64H
                DB      4dH,61H,63H,72H,6fH,0dH,11H,11H
                DB      0d0H,0feH,80H,0c2H,62H,75H,66H,66H
                DB      65H,72H,0fH,11H,10H,0eeH,80H,0c3H
                DB      6eH,65H,78H,74H,5fH,63H,68H,61H
                DB      72H,09H,11H,10H,0dcH,80H,0baH,64H
                DB      69H,72H,09H,11H,10H,0d8H,80H,0c5H
                DB      73H,79H,6dH,09H,11H,10H,0e4H,40H
                DB      69H,6eH,66H,6fH,09H,11H,10H,0d0H
                DB      36H,70H,61H,72H,6dH,0aH,11H,10H
                DB      0d4H,2eH,6cH,6eH,6fH,64H,65H,09H
                DB      11H,10H,0e0H,11H,6cH,69H,6eH,65H
                DB      0aH,11H,10H,0ecH,06H,63H,6fH,75H
                DB      6eH,74H,13H,11H,10H,0f0H,06H,6dH
                DB      61H,63H,72H,6fH,5fH,6eH,61H,6dH
                DB      65H,5fH,6cH,6fH,63H,13H,11H,10H
                DB      0f4H,08H,65H,78H,70H,61H,6eH,73H
                DB      69H,6fH,6eH,5fH,66H,6cH,61H,67H
                DB      0eH,11H,10H,0f2H,06H,65H,78H,70H
                DB      5fH,73H,74H,61H,72H,74H,0eH,11H
                DB      10H,0e8H,06H,74H,6fH,6bH,5fH,63H
                DB      6fH,75H,6eH,74H,19H,22H,64H,16H
                DB      0b5H,01H,00H,00H,17H,0bH,02H,00H
                DB      80H,0c1H,48H,01H,48H,4dH,61H,63H
                DB      72H,6fH,44H,65H,66H,08H,11H,10H
                DB      0eeH,43H,73H,79H,6dH,09H,11H,10H
                DB      0eaH,11H,6eH,61H,6dH,65H,0eH,11H
                DB      10H,0e6H,80H,0baH,63H,75H,72H,72H
                DB      70H,72H,6fH,63H,06H,11H,10H,0f4H
                DB      06H,69H
$$SYMBOLS       ENDS

$$TYPES         SEGMENT BYTE  'DEBTYP'
                DB      08H,11H,73H,74H,72H,75H,63H,74H
                DB      07H,11H,75H,6eH,69H,6fH,6eH,06H
                DB      11H,65H,6eH,75H,6dH,03H,10H,00H
                DB      03H,10H,10H,03H,10H,01H,03H,10H
                DB      11H,08H,12H,00H,05H,63H,68H,61H
                DB      72H,08H,12H,00H,05H,63H,68H,61H
                DB      72H,0aH,12H,00H,07H,73H,69H,7aH
                DB      65H,5fH,74H,0bH,12H,00H,07H,77H
                DB      63H,68H,61H,72H,5fH,74H,0bH,12H
                DB      00H,07H,77H,63H,68H,61H,72H,5fH
                DB      74H,03H,10H,30H,04H,71H,0dH,00H
                DB      03H,41H,0eH,0cH,12H,00H,0fH,6fH
                DB      6eH,65H,78H,69H,74H,5fH,74H,03H
                DB      41H,08H,04H,20H,00H,11H,0dH,12H
                DB      00H,12H,5fH,5fH,76H,61H,5fH,6cH
                DB      69H,73H,74H,03H,10H,03H,0aH,12H
                DB      00H,14H,66H,70H,6fH,73H,5fH,74H
                DB      0aH,12H,00H,14H,66H,70H,6fH,73H
                DB      5fH,74H,08H,12H,00H,07H,75H,69H
                DB      6eH,74H,0aH,12H,00H,05H,75H,69H
                DB      6eH,74H,5fH,38H,0bH,12H,00H,07H
                DB      75H,69H,6eH,74H,5fH,31H,36H,03H
                DB      10H,13H,0bH,12H,00H,1aH,75H,69H
                DB      6eH,74H,5fH,33H,32H,0eH,12H,00H
                DB      05H,75H,6eH,73H,69H,67H,6eH,65H
                DB      64H,5fH,38H,0fH,12H,00H,07H,75H
                DB      6eH,73H,69H,67H,6eH,65H,64H,5fH
                DB      31H,36H,0fH,12H,00H,1aH,75H,6eH
                DB      73H,69H,67H,6eH,65H,64H,5fH,33H
                DB      32H,09H,12H,00H,04H,69H,6eH,74H
                DB      5fH,38H,0aH,12H,00H,06H,69H,6eH
                DB      74H,5fH,31H,36H,0aH,12H,00H,14H
                DB      69H,6eH,74H,5fH,33H,32H,0cH,12H
                DB      00H,04H,73H,69H,67H,6eH,65H,64H
                DB      5fH,38H,0dH,12H,00H,06H,73H,69H
                DB      67H,6eH,65H,64H,5fH,31H,36H,0dH
                DB      12H,00H,14H,73H,69H,67H,6eH,65H
                DB      64H,5fH,33H,32H,08H,12H,00H,08H
                DB      62H,6fH,6fH,6cH,0dH,12H,00H,06H
                DB      70H,74H,72H,64H,69H,66H,66H,5fH
                DB      74H,0fH,12H,00H,19H,6eH,61H,6dH
                DB      65H,5fH,68H,61H,6eH,64H,6cH,65H
                DB      0aH,12H,00H,05H,75H,69H,6eH,74H
                DB      5fH,38H,0eH,12H,00H,06H,64H,69H
                DB      72H,65H,63H,74H,5fH,69H,64H,78H
                DB      0dH,12H,01H,80H,2cH,61H,73H,6dH
                DB      6cH,69H,6eH,65H,73H,03H,41H,2aH
                DB      08H,60H,03H,00H,09H,00H,00H,00H
                DB      0dH,61H,08H,08H,70H,61H,72H,6dH
                DB      63H,6fH,75H,6eH,74H,08H,61H,04H
                DB      11H,6cH,69H,6eH,65H,08H,61H,00H
                DB      2bH,6eH,65H,78H,74H,0cH,12H,00H
                DB      2aH,61H,73H,6dH,6cH,69H,6eH,65H
                DB      73H,03H,41H,2dH,03H,41H,2eH,03H
                DB      41H,0dH,06H,70H,2eH,02H,2fH,30H
                DB      0eH,12H,01H,80H,34H,70H,61H,72H
                DB      6dH,5fH,6cH,69H,73H,74H,03H,41H
                DB      32H,08H,60H,05H,00H,11H,00H,00H
                DB      00H,07H,61H,0dH,11H,64H,65H,66H
                DB      0cH,61H,0cH,08H,72H,65H,71H,75H
                DB      69H,72H,65H,64H,0bH,61H,08H,11H
                DB      72H,65H,70H,6cH,61H,63H,65H,09H
                DB      61H,04H,11H,6cH,61H,62H,65H,6cH
                DB      08H,61H,00H,33H,6eH,65H,78H,74H
                DB      0dH,12H,00H,32H,70H,61H,72H,6dH
                DB      5fH,6cH,69H,73H,74H,03H,41H,35H
                DB      08H,70H,11H,04H,36H,11H,08H,2eH
                DB      04H,20H,09H,08H,06H,70H,0dH,02H
                DB      2eH,36H,06H,70H,1fH,02H,11H,11H
                DB      05H,70H,06H,01H,0dH,05H,21H,0ffH
                DB      00H,08H,05H,70H,06H,01H,06H,08H
                DB      60H,04H,00H,0eH,00H,00H,00H,0eH
                DB      61H,0cH,07H,73H,74H,61H,72H,74H
                DB      5fH,6cH,69H,6eH,65H,0cH,61H,08H
                DB      11H,66H,69H,6cH,65H,6eH,61H,6dH
                DB      65H,08H,61H,04H,2eH,64H,61H,74H
                DB      61H,0cH,61H,00H,36H,70H,61H,72H
                DB      6dH,6cH,69H,73H,74H,0eH,12H,00H
                DB      3eH,6dH,61H,63H,72H,6fH,5fH,69H
                DB      6eH,66H,6fH,03H,41H,3fH,05H,21H
                DB      0ffH,00H,08H,0cH,12H,01H,80H,4cH
                DB      61H,73H,6dH,5fH,73H,79H,6dH,03H
                DB      41H,42H,05H,50H,0fH,00H,10H,0dH
                DB      51H,0eH,53H,59H,4dH,5fH,53H,54H
                DB      52H,55H,43H,54H,13H,51H,0dH,53H
                DB      59H,4dH,5fH,53H,54H,52H,55H,43H
                DB      54H,5fH,46H,49H,45H,4cH,44H,12H
                DB      51H,0cH,53H,59H,4dH,5fH,43H,4cH
                DB      41H,53H,53H,5fH,4cH,4eH,41H,4dH
                DB      45H,0cH,51H,0bH,53H,59H,4dH,5fH
                DB      4cH,4eH,41H,4dH,45H,0aH,51H,0aH
                DB      53H,59H,4dH,5fH,45H,58H,54H,0aH
                DB      51H,09H,53H,59H,4dH,5fH,4cH,49H
                DB      42H,0cH,51H,08H,53H,59H,4dH,5fH
                DB      43H,4fH,4eH,53H,54H,0cH,51H,07H
                DB      53H,59H,4dH,5fH,4dH,41H,43H,52H
                DB      4fH,0bH,51H,06H,53H,59H,4dH,5fH
                DB      50H,52H,4fH,43H,0aH,51H,05H,53H
                DB      59H,4dH,5fH,47H,52H,50H,0aH,51H
                DB      04H,53H,59H,4dH,5fH,53H,45H,47H
                DB      0cH,51H,03H,53H,59H,4dH,5fH,53H
                DB      54H,41H,43H,4bH,0fH,51H,02H,53H
                DB      59H,4dH,5fH,45H,58H,54H,45H,52H
                DB      4eH,41H,4cH,0fH,51H,01H,53H,59H
                DB      4dH,5fH,49H,4eH,54H,45H,52H,4eH
                DB      41H,4cH,10H,51H,00H,53H,59H,4dH
                DB      5fH,55H,4eH,44H,45H,46H,49H,4eH
                DB      45H,44H,0dH,12H,03H,44H,73H,79H
                DB      6dH,5fH,73H,74H,61H,74H,65H,0dH
                DB      12H,01H,80H,4aH,61H,73H,6dH,66H
                DB      69H,78H,75H,70H,03H,41H,46H,05H
                DB      50H,08H,00H,10H,0cH,51H,07H,46H
                DB      49H,58H,5fH,50H,54H,52H,33H,32H
                DB      0cH,51H,06H,46H,49H,58H,5fH,50H
                DB      54H,52H,31H,36H,0cH,51H,05H,46H
                DB      49H,58H,5fH,4fH,46H,46H,33H,32H
                DB      0cH,51H,04H,46H,49H,58H,5fH,4fH
                DB      46H,46H,31H,36H,0fH,51H,03H,46H
                DB      49H,58H,5fH,52H,45H,4cH,4fH,46H
                DB      46H,33H,32H,0fH,51H,02H,46H,49H
                DB      58H,5fH,52H,45H,4cH,4fH,46H,46H
                DB      31H,36H,0eH,51H,01H,46H,49H,58H
                DB      5fH,52H,45H,4cH,4fH,46H,46H,38H
                DB      0aH,51H,00H,46H,49H,58H,5fH,53H
                DB      45H,47H,0fH,12H,03H,48H,66H,69H
                DB      78H,75H,70H,5fH,74H,79H,70H,65H
                DB      73H,08H,60H,08H,00H,13H,00H,00H
                DB      00H,0fH,61H,11H,19H,66H,72H,61H
                DB      6dH,65H,5fH,64H,61H,74H,75H,6dH
                DB      09H,61H,10H,1fH,66H,72H,61H,6dH
                DB      65H,0cH,61H,0fH,08H,65H,78H,74H
                DB      65H,72H,6eH,61H,6cH,0cH,61H,0eH
                DB      48H,66H,69H,78H,5fH,74H,79H,70H
                DB      65H,0bH,61H,0cH,07H,66H,69H,78H
                DB      5fH,6cH,6fH,63H,0aH,61H,08H,1aH
                DB      6fH,66H,66H,73H,65H,74H,08H,61H
                DB      04H,11H,6eH,61H,6dH,65H,08H,61H
                DB      00H,47H,6eH,65H,78H,74H,03H,41H
                DB      46H,08H,60H,0dH,00H,1aH,00H,00H
                DB      00H,09H,61H,16H,4bH,66H,69H,78H
                DB      75H,70H,09H,61H,15H,44H,73H,74H
                DB      61H,74H,65H,0cH,61H,13H,06H,6dH
                DB      65H,6dH,5fH,74H,79H,70H,65H,10H
                DB      61H,12H,18H,74H,6fH,74H,61H,6cH
                DB      5fH,6cH,65H,6eH,67H,74H,68H,0eH
                DB      61H,11H,18H,74H,6fH,74H,61H,6cH
                DB      5fH,73H,69H,7aH,65H,10H,61H,10H
                DB      18H,66H,69H,72H,73H,74H,5fH,6cH
                DB      65H,6eH,67H,74H,68H,0eH,61H,0fH
                DB      18H,66H,69H,72H,73H,74H,5fH,73H
                DB      69H,7aH,65H,0aH,61H,0eH,18H,70H
                DB      75H,62H,6cH,69H,63H,0aH,61H,0cH
                DB      17H,6fH,66H,66H,73H,65H,74H,0aH
                DB      61H,0aH,17H,73H,65H,67H,69H,64H
                DB      78H,0aH,61H,08H,17H,67H,72H,70H
                DB      69H,64H,78H,08H,61H,04H,11H,6eH
                DB      61H,6dH,65H,08H,61H,00H,43H,6eH
                DB      65H,78H,74H,0cH,12H,00H,80H,7dH
                DB      6fH,62H,6aH,5fH,72H,65H,63H,03H
                DB      41H,4dH,03H,41H,18H,08H,60H,02H
                DB      00H,02H,00H,00H,00H,09H,61H,01H
                DB      18H,63H,6cH,61H,73H,73H,08H,61H
                DB      00H,18H,61H,74H,74H,72H,0fH,12H
                DB      01H,50H,63H,6fH,6dH,65H,6eH,74H
                DB      5fH,69H,6eH,66H,6fH,08H,60H,06H
                DB      00H,09H,00H,00H,00H,11H,61H,05H
                DB      21H,74H,61H,72H,67H,65H,74H,5fH
                DB      6fH,66H,66H,73H,65H,74H,10H,61H
                DB      03H,19H,74H,61H,72H,67H,65H,74H
                DB      5fH,64H,61H,74H,75H,6dH,0fH,61H
                DB      01H,19H,66H,72H,61H,6dH,65H,5fH
                DB      64H,61H,74H,75H,6dH,12H,64H,00H
                DB      06H,01H,05H,69H,73H,5fH,73H,65H
                DB      63H,6fH,6eH,64H,61H,72H,79H,0cH
                DB      64H,00H,03H,03H,05H,74H,61H,72H
                DB      67H,65H,74H,0bH,64H,00H,00H,03H
                DB      05H,66H,72H,61H,6dH,65H,0aH,12H
                DB      00H,52H,6cH,6fH,67H,72H,65H,66H
                DB      08H,60H,02H,00H,06H,00H,00H,00H
                DB      0aH,61H,02H,1bH,6fH,66H,66H,73H
                DB      65H,74H,09H,61H,00H,19H,66H,72H
                DB      61H,6dH,65H,0bH,12H,00H,54H,70H
                DB      68H,79H,73H,72H,65H,66H,08H,60H
                DB      02H,00H,09H,00H,00H,00H,08H,61H
                DB      00H,55H,70H,68H,79H,73H,07H,61H
                DB      00H,53H,6cH,6fH,67H,0bH,12H,00H
                DB      56H,6cH,6fH,67H,70H,68H,79H,73H
                DB      08H,60H,04H,00H,0aH,00H,00H,00H
                DB      07H,61H,01H,57H,72H,65H,66H,10H
                DB      64H,00H,02H,01H,05H,69H,73H,5fH
                DB      6cH,6fH,67H,69H,63H,61H,6cH,11H
                DB      64H,00H,01H,01H,05H,73H,74H,61H
                DB      72H,74H,5fH,61H,64H,64H,72H,73H
                DB      11H,64H,00H,00H,01H,05H,6dH,61H
                DB      69H,6eH,5fH,6dH,6fH,64H,75H,6cH
                DB      65H,0fH,12H,01H,58H,6dH,6fH,64H
                DB      65H,6eH,64H,5fH,69H,6eH,66H,6fH
                DB      08H,60H,02H,00H,04H,00H,00H,00H
                DB      0dH,61H,02H,19H,6eH,75H,6dH,5fH
                DB      6eH,61H,6dH,65H,73H,0dH,61H,00H
                DB      19H,66H,69H,72H,73H,74H,5fH,69H
                DB      64H,78H,0fH,12H,01H,5aH,6cH,6eH
                DB      61H,6dH,65H,73H,5fH,69H,6eH,66H
                DB      6fH,08H,60H,01H,00H,02H,00H,00H
                DB      00H,07H,61H,00H,19H,69H,64H,78H
                DB      0fH,12H,01H,5cH,67H,72H,70H,64H
                DB      65H,66H,5fH,69H,6eH,66H,6fH,08H
                DB      60H,0bH,00H,14H,00H,00H,00H,10H
                DB      61H,12H,19H,6fH,76H,6cH,5fH,6eH
                DB      61H,6dH,65H,5fH,69H,64H,78H,12H
                DB      61H,10H,19H,63H,6cH,61H,73H,73H
                DB      5fH,6eH,61H,6dH,65H,5fH,69H,64H
                DB      78H,10H,61H,0eH,19H,73H,65H,67H
                DB      5fH,6eH,61H,6dH,65H,5fH,69H,64H
                DB      78H,0eH,61H,0aH,1bH,73H,65H,67H
                DB      5fH,6cH,65H,6eH,67H,74H,68H,07H
                DB      61H,04H,55H,61H,62H,73H,11H,64H
                DB      03H,02H,02H,05H,61H,63H,63H,65H
                DB      73H,73H,5fH,61H,74H,74H,72H,12H
                DB      64H,03H,01H,01H,05H,61H,63H,63H
                DB      65H,73H,73H,5fH,76H,61H,6cH,69H
                DB      64H,0cH,64H,03H,00H,01H,05H,75H
                DB      73H,65H,5fH,33H,32H,0dH,64H,02H
                DB      04H,04H,05H,63H,6fH,6dH,62H,69H
                DB      6eH,65H,0bH,64H,02H,00H,04H,05H
                DB      61H,6cH,69H,67H,6eH,07H,61H,00H
                DB      19H,69H,64H,78H,0fH,12H,01H,5eH
                DB      73H,65H,67H,64H,65H,66H,5fH,69H
                DB      6eH,66H,6fH,08H,60H,02H,00H,06H
                DB      00H,00H,00H,0aH,61H,02H,1bH,6fH
                DB      66H,66H,73H,65H,74H,07H,61H,00H
                DB      19H,69H,64H,78H,0fH,12H,01H,60H
                DB      6cH,65H,64H,61H,74H,61H,5fH,69H
                DB      6eH,66H,6fH,08H,60H,03H,00H,06H
                DB      00H,00H,00H,09H,61H,04H,19H,66H
                DB      72H,61H,6dH,65H,0bH,61H,02H,19H
                DB      73H,65H,67H,5fH,69H,64H,78H,0bH
                DB      61H,00H,19H,67H,72H,70H,5fH,69H
                DB      64H,78H,0dH,12H,01H,62H,62H,61H
                DB      73H,65H,5fH,69H,6eH,66H,6fH,0aH
                DB      12H,00H,80H,67H,66H,69H,78H,75H
                DB      70H,03H,41H,64H,08H,60H,06H,00H
                DB      12H,00H,00H,00H,06H,61H,09H,53H
                DB      6cH,72H,0eH,61H,05H,1bH,6cH,6fH
                DB      63H,5fH,6fH,66H,66H,73H,65H,74H
                DB      15H,64H,04H,04H,01H,05H,6cH,6fH
                DB      61H,64H,65H,72H,5fH,72H,65H,73H
                DB      6fH,6cH,76H,65H,64H,13H,64H,04H
                DB      03H,01H,05H,73H,65H,6cH,66H,5fH
                DB      72H,65H,6cH,61H,74H,69H,76H,65H
                DB      10H,64H,04H,00H,03H,05H,6cH,6fH
                DB      63H,5fH,6dH,65H,74H,68H,6fH,64H
                DB      08H,61H,00H,65H,6eH,65H,78H,74H
                DB      09H,12H,01H,66H,66H,69H,78H,75H
                DB      70H,03H,41H,64H,08H,60H,02H,00H
                DB      08H,00H,00H,00H,09H,61H,04H,68H
                DB      66H,69H,78H,75H,70H,0cH,61H,00H
                DB      4eH,64H,61H,74H,61H,5fH,72H,65H
                DB      63H,0eH,12H,01H,69H,66H,69H,78H
                DB      75H,70H,5fH,69H,6eH,66H,6fH,08H
                DB      60H,02H,00H,03H,00H,00H,00H,13H
                DB      61H,01H,19H,70H,75H,62H,6cH,69H
                DB      63H,5fH,6eH,61H,6dH,65H,5fH,69H
                DB      64H,78H,09H,61H,00H,18H,66H,6cH
                DB      61H,67H,73H,08H,60H,02H,00H,06H
                DB      00H,00H,00H,0aH,61H,00H,6bH,6cH
                DB      69H,6eH,73H,79H,6dH,08H,61H,00H
                DB      63H,62H,61H,73H,65H,08H,60H,02H
                DB      00H,06H,00H,00H,00H,0aH,61H,02H
                DB      1bH,6fH,66H,66H,73H,65H,74H,0aH
                DB      61H,00H,19H,6eH,75H,6dH,62H,65H
                DB      72H,0fH,12H,01H,6dH,6cH,69H,6eH
                DB      6eH,75H,6dH,5fH,64H,61H,74H,61H
                DB      03H,41H,6eH,08H,60H,03H,00H,0cH
                DB      00H,00H,00H,09H,61H,08H,6fH,6cH
                DB      69H,6eH,65H,73H,0dH,61H,06H,19H
                DB      6eH,75H,6dH,5fH,6cH,69H,6eH,65H
                DB      73H,05H,61H,00H,6cH,64H,0fH,12H
                DB      01H,70H,6cH,69H,6eH,6eH,75H,6dH
                DB      5fH,69H,6eH,66H,6fH,08H,60H,01H
                DB      00H,02H,00H,00H,00H,07H,61H,00H
                DB      19H,69H,64H,78H,08H,60H,03H,00H
                DB      08H,00H,00H,00H,08H,61H,06H,72H
                DB      74H,79H,70H,65H,0aH,61H,02H,1bH
                DB      6fH,66H,66H,73H,65H,74H,08H,61H
                DB      00H,27H,6eH,61H,6dH,65H,0fH,12H
                DB      01H,73H,70H,75H,62H,64H,65H,66H
                DB      5fH,64H,61H,74H,61H,03H,41H,74H
                DB      08H,60H,05H,00H,0dH,00H,00H,00H
                DB      0fH,64H,0cH,01H,01H,05H,70H,72H
                DB      6fH,63H,65H,73H,73H,65H,64H,0fH
                DB      64H,0cH,00H,01H,05H,66H,72H,65H
                DB      65H,5fH,70H,75H,62H,73H,08H,61H
                DB      08H,75H,70H,75H,62H,73H,0cH,61H
                DB      06H,19H,6eH,75H,6dH,5fH,70H,75H
                DB      62H,73H,08H,61H,00H,63H,62H,61H
                DB      73H,65H,0fH,12H,01H,76H,70H,75H
                DB      62H,64H,65H,66H,5fH,69H,6eH,66H
                DB      6fH,08H,60H,07H,00H,11H,00H,00H
                DB      00H,13H,61H,0fH,19H,70H,75H,62H
                DB      6cH,69H,63H,5fH,6eH,61H,6dH,65H
                DB      5fH,69H,64H,78H,0cH,61H,0dH,19H
                DB      74H,79H,70H,65H,5fH,69H,64H,78H
                DB      0aH,61H,09H,1bH,6fH,66H,66H,73H
                DB      65H,74H,09H,61H,08H,18H,61H,6cH
                DB      69H,67H,6eH,0eH,61H,07H,18H,61H
                DB      74H,74H,72H,69H,62H,75H,74H,65H
                DB      73H,09H,61H,06H,18H,66H,6cH,61H
                DB      67H,73H,08H,61H,00H,63H,62H,61H
                DB      73H,65H,0fH,12H,01H,78H,63H,6fH
                DB      6dH,64H,61H,74H,5fH,69H,6eH,66H
                DB      6fH,08H,60H,11H,00H,14H,00H,00H
                DB      00H,0aH,61H,00H,79H,63H,6fH,6dH
                DB      64H,61H,74H,0aH,61H,00H,77H,70H
                DB      75H,62H,64H,65H,66H,0aH,61H,00H
                DB      71H,6cH,69H,6eH,73H,79H,6dH,0aH
                DB      61H,00H,71H,6cH,69H,6eH,6eH,75H
                DB      6dH,09H,61H,00H,6aH,66H,69H,78H
                DB      75H,70H,08H,61H,00H,63H,62H,61H
                DB      73H,65H,0aH,61H,00H,61H,6cH,69H
                DB      64H,61H,74H,61H,0aH,61H,00H,61H
                DB      6cH,65H,64H,61H,74H,61H,0aH,61H
                DB      00H,5fH,73H,65H,67H,64H,65H,66H
                DB      0aH,61H,00H,5dH,67H,72H,70H,64H
                DB      65H,66H,0aH,61H,00H,5bH,63H,65H
                DB      78H,74H,64H,66H,0aH,61H,00H,5bH
                DB      63H,6fH,6dH,64H,65H,66H,0aH,61H
                DB      00H,5bH,65H,78H,74H,64H,65H,66H
                DB      0bH,61H,00H,5bH,6cH,6cH,6eH,61H
                DB      6dH,65H,73H,0aH,61H,00H,5bH,6cH
                DB      6eH,61H,6dH,65H,73H,0aH,61H,00H
                DB      59H,6dH,6fH,64H,65H,6eH,64H,0aH
                DB      61H,00H,51H,63H,6fH,6dH,65H,6eH
                DB      74H,0fH,12H,02H,7aH,6fH,62H,6aH
                DB      72H,65H,63H,5fH,69H,6eH,66H,6fH
                DB      08H,60H,09H,00H,22H,00H,00H,00H
                DB      05H,61H,0eH,7bH,64H,0fH,64H,0dH
                DB      02H,01H,05H,66H,72H,65H,65H,5fH
                DB      64H,61H,74H,61H,0bH,64H,0dH,01H
                DB      01H,05H,69H,73H,5fH,33H,32H,0dH
                DB      64H,0dH,00H,01H,05H,69H,73H,5fH
                DB      70H,68H,61H,72H,0bH,61H,0cH,18H
                DB      63H,6fH,6dH,6dH,61H,6eH,64H,08H
                DB      61H,08H,4fH,64H,61H,74H,61H,0aH
                DB      61H,06H,19H,63H,75H,72H,6fH,66H
                DB      66H,0aH,61H,04H,19H,6cH,65H,6eH
                DB      67H,74H,68H,08H,61H,00H,4eH,6eH
                DB      65H,78H,74H,0bH,12H,01H,7cH,6fH
                DB      62H,6aH,5fH,72H,65H,63H,03H,41H
                DB      4dH,08H,60H,06H,00H,0bH,00H,00H
                DB      00H,0dH,61H,09H,29H,6cH,6eH,61H
                DB      6dH,65H,5fH,69H,64H,78H,0cH,64H
                DB      08H,01H,01H,04H,69H,67H,6eH,6fH
                DB      72H,65H,0eH,64H,08H,00H,01H,04H
                DB      72H,65H,61H,64H,6fH,6eH,6cH,79H
                DB      0dH,61H,06H,19H,73H,74H,61H,72H
                DB      74H,5fH,6cH,6fH,63H,07H,61H,04H
                DB      29H,69H,64H,78H,0aH,61H,00H,7eH
                DB      73H,65H,67H,72H,65H,63H,0cH,12H
                DB      00H,7fH,73H,65H,67H,5fH,69H,6eH
                DB      66H,6fH,04H,41H,80H,80H,0dH,12H
                DB      01H,80H,86H,73H,65H,67H,5fH,6cH
                DB      69H,73H,74H,04H,41H,80H,82H,0dH
                DB      12H,01H,80H,0b8H,64H,69H,72H,5fH
                DB      6eH,6fH,64H,65H,04H,41H,80H,84H
                DB      08H,60H,02H,00H,08H,00H,00H,00H
                DB      08H,61H,04H,80H,85H,73H,65H,67H
                DB      09H,61H,00H,80H,83H,6eH,65H,78H
                DB      74H,0dH,12H,00H,80H,82H,73H,65H
                DB      67H,5fH,6cH,69H,73H,74H,04H,41H
                DB      80H,87H,08H,60H,04H,00H,0aH,00H
                DB      00H,00H,0dH,61H,08H,29H,6cH,6eH
                DB      61H,6dH,65H,5fH,69H,64H,78H,0aH
                DB      61H,06H,17H,6eH,75H,6dH,73H,65H
                DB      67H,0cH,61H,02H,80H,88H,73H,65H
                DB      67H,6cH,69H,73H,74H,07H,61H,00H
                DB      29H,69H,64H,78H,0dH,12H,00H,80H
                DB      89H,67H,72H,70H,5fH,69H,6eH,66H
                DB      6fH,04H,41H,80H,8aH,08H,60H,01H
                DB      00H,02H,00H,00H,00H,07H,61H,00H
                DB      17H,69H,64H,78H,0dH,12H,00H,80H
                DB      8cH,65H,78H,74H,5fH,69H,6eH,66H
                DB      6fH,04H,41H,80H,8dH,05H,50H,22H
                DB      00H,10H,0cH,51H,25H,54H,5fH,50H
                DB      45H,52H,43H,45H,4eH,54H,12H,51H
                DB      3fH,54H,5fH,51H,55H,45H,53H,54H
                DB      49H,4fH,4eH,5fH,4dH,41H,52H,4bH
                DB      08H,51H,2eH,54H,5fH,44H,4fH,54H
                DB      0aH,51H,2dH,54H,5fH,4dH,49H,4eH
                DB      55H,53H,09H,51H,2bH,54H,5fH,50H
                DB      4cH,55H,53H,0aH,51H,2aH,54H,5fH
                DB      54H,49H,4dH,45H,53H,0fH,51H,3bH
                DB      54H,5fH,53H,45H,4dH,49H,5fH,43H
                DB      4fH,4cH,4fH,4eH,0aH,51H,3aH,54H
                DB      5fH,43H,4fH,4cH,4fH,4eH,0aH,51H
                DB      2cH,54H,5fH,43H,4fH,4dH,4dH,41H
                DB      12H,51H,5dH,54H,5fH,43H,4cH,5fH
                DB      53H,51H,5fH,42H,52H,41H,43H,4bH
                DB      45H,54H,0fH,51H,29H,54H,5fH,43H
                DB      4cH,5fH,42H,52H,41H,43H,4bH,45H
                DB      54H,12H,51H,5bH,54H,5fH,4fH,50H
                DB      5fH,53H,51H,5fH,42H,52H,41H,43H
                DB      4bH,45H,54H,0fH,51H,28H,54H,5fH
                DB      4fH,50H,5fH,42H,52H,41H,43H,4bH
                DB      45H,54H,13H,51H,14H,54H,5fH,55H
                DB      4eH,41H,52H,59H,5fH,4fH,50H,45H
                DB      52H,41H,54H,4fH,52H,09H,51H,13H
                DB      54H,5fH,50H,41H,54H,48H,0cH,51H
                DB      12H,54H,5fH,42H,49H,4eH,5fH,4eH
                DB      55H,4dH,15H,51H,11H,54H,5fH,49H
                DB      44H,5fH,49H,4eH,5fH,42H,41H,43H
                DB      4bH,51H,55H,4fH,54H,45H,53H,0dH
                DB      51H,10H,54H,5fH,4eH,45H,47H,41H
                DB      54H,49H,56H,45H,0dH,51H,0fH,54H
                DB      5fH,50H,4fH,53H,49H,54H,49H,56H
                DB      45H,09H,51H,0eH,54H,5fH,4eH,4fH
                DB      4fH,50H,0aH,51H,0dH,54H,5fH,46H
                DB      4cH,4fH,41H,54H,08H,51H,0cH,54H
                DB      5fH,4eH,55H,4dH,0cH,51H,0bH,54H
                DB      5fH,48H,45H,58H,5fH,4eH,55H,4dH
                DB      0eH,51H,0aH,54H,5fH,48H,45H,58H
                DB      5fH,4eH,55H,4dH,5fH,30H,0cH,51H
                DB      09H,54H,5fH,4fH,43H,54H,5fH,4eH
                DB      55H,4dH,0cH,51H,08H,54H,5fH,44H
                DB      45H,43H,5fH,4eH,55H,4dH,10H,51H
                DB      07H,54H,5fH,44H,49H,52H,45H,43H
                DB      54H,5fH,45H,58H,50H,52H,0eH,51H
                DB      06H,54H,5fH,44H,49H,52H,45H,43H
                DB      54H,49H,56H,45H,0bH,51H,05H,54H
                DB      5fH,53H,54H,52H,49H,4eH,47H,08H
                DB      51H,04H,54H,5fH,52H,45H,47H,07H
                DB      51H,03H,54H,5fH,49H,44H,0bH,51H
                DB      02H,54H,5fH,52H,45H,53H,5fH,49H
                DB      44H,08H,51H,01H,54H,5fH,49H,4eH
                DB      53H,0aH,51H,00H,54H,5fH,46H,49H
                DB      4eH,41H,4cH,0aH,12H,03H,80H,8fH
                DB      73H,74H,61H,74H,65H,08H,60H,03H
                DB      00H,09H,00H,00H,00H,0eH,61H,05H
                DB      11H,73H,74H,72H,69H,6eH,67H,5fH
                DB      70H,74H,72H,0aH,61H,04H,80H,8fH
                DB      74H,6fH,6bH,65H,6eH,09H,61H,00H
                DB      14H,76H,61H,6cH,75H,65H,0cH,12H
                DB      01H,80H,91H,61H,73H,6dH,5fH,74H
                DB      6fH,6bH,04H,41H,80H,92H,08H,60H
                DB      04H,00H,08H,00H,00H,00H,09H,61H
                DB      04H,80H,93H,64H,61H,74H,61H,09H
                DB      61H,02H,06H,63H,6fH,75H,6eH,74H
                DB      12H,64H,00H,01H,01H,06H,65H,78H
                DB      70H,61H,6eH,64H,5fH,65H,61H,72H
                DB      6cH,79H,0eH,64H,00H,00H,01H,06H
                DB      72H,65H,64H,65H,66H,69H,6eH,65H
                DB      0fH,12H,00H,80H,94H,63H,6fH,6eH
                DB      73H,74H,5fH,69H,6eH,66H,6fH,04H
                DB      41H,80H,95H,0eH,12H,01H,80H,99H
                DB      72H,65H,67H,73H,5fH,6cH,69H,73H
                DB      74H,04H,41H,80H,97H,08H,60H,02H
                DB      00H,08H,00H,00H,00H,07H,61H,04H
                DB      11H,72H,65H,67H,09H,61H,00H,80H
                DB      98H,6eH,65H,78H,74H,0eH,12H,00H
                DB      80H,97H,72H,65H,67H,73H,5fH,6cH
                DB      69H,73H,74H,04H,41H,80H,9aH,05H
                DB      50H,03H,00H,10H,0dH,51H,02H,56H
                DB      49H,53H,5fH,45H,58H,50H,4fH,52H
                DB      54H,0dH,51H,01H,56H,49H,53H,5fH
                DB      50H,55H,42H,4cH,49H,43H,0eH,51H
                DB      00H,56H,49H,53H,5fH,50H,52H,49H
                DB      56H,41H,54H,45H,0dH,12H,00H,80H
                DB      9cH,76H,69H,73H,5fH,74H,79H,70H
                DB      65H,05H,50H,08H,00H,10H,0fH,51H
                DB      07H,4cH,41H,4eH,47H,5fH,53H,59H
                DB      53H,43H,41H,4cH,4cH,0fH,51H,06H
                DB      4cH,41H,4eH,47H,5fH,53H,54H,44H
                DB      43H,41H,4cH,4cH,10H,51H,05H,4cH
                DB      41H,4eH,47H,5fH,57H,41H,54H,43H
                DB      4fH,4dH,5fH,43H,09H,51H,04H,4cH
                DB      41H,4eH,47H,5fH,43H,0eH,51H,03H
                DB      4cH,41H,4eH,47H,5fH,50H,41H,53H
                DB      43H,41H,4cH,0fH,51H,02H,4cH,41H
                DB      4eH,47H,5fH,46H,4fH,52H,54H,52H
                DB      41H,4eH,0dH,51H,01H,4cH,41H,4eH
                DB      47H,5fH,42H,41H,53H,49H,43H,0cH
                DB      51H,00H,4cH,41H,4eH,47H,5fH,4eH
                DB      4fH,4eH,45H,0eH,12H,00H,80H,9eH
                DB      6cH,61H,6eH,67H,5fH,74H,79H,70H
                DB      65H,0fH,12H,01H,80H,0a2H,6cH,61H
                DB      62H,65H,6cH,5fH,6cH,69H,73H,74H
                DB      04H,41H,80H,0a0H,08H,60H,07H,00H
                DB      14H,00H,00H,00H,09H,61H,12H,06H
                DB      63H,6fH,75H,6eH,74H,0fH,64H,10H
                DB      00H,01H,06H,69H,73H,5fH,76H,61H
                DB      72H,61H,72H,67H,0aH,61H,0eH,06H
                DB      66H,61H,63H,74H,6fH,72H,08H,61H
                DB      0cH,06H,73H,69H,7aH,65H,0bH,61H
                DB      08H,11H,72H,65H,70H,6cH,61H,63H
                DB      65H,09H,61H,04H,11H,6cH,61H,62H
                DB      65H,6cH,09H,61H,00H,80H,0a1H,6eH
                DB      65H,78H,74H,0fH,12H,00H,80H,0a0H
                DB      6cH,61H,62H,65H,6cH,5fH,6cH,69H
                DB      73H,74H,04H,41H,80H,0a3H,08H,60H
                DB      09H,00H,16H,00H,00H,00H,0fH,64H
                DB      14H,00H,01H,06H,69H,73H,5fH,76H
                DB      61H,72H,61H,72H,67H,0cH,61H,12H
                DB      06H,6dH,65H,6dH,5fH,74H,79H,70H
                DB      65H,0dH,61H,10H,06H,6cH,6fH,63H
                DB      61H,6cH,73H,69H,7aH,65H,0cH,61H
                DB      0eH,06H,70H,61H,72H,61H,73H,69H
                DB      7aH,65H,0eH,61H,0aH,80H,0a4H,6cH
                DB      6fH,63H,61H,6cH,6cH,69H,73H,74H
                DB      0dH,61H,06H,80H,0a4H,70H,61H,72H
                DB      61H,6cH,69H,73H,74H,0dH,61H,05H
                DB      80H,9fH,6cH,61H,6eH,67H,74H,79H
                DB      70H,65H,0fH,61H,04H,80H,9dH,76H
                DB      69H,73H,69H,62H,69H,6cH,69H,74H
                DB      79H,0dH,61H,00H,80H,9bH,72H,65H
                DB      67H,73H,6cH,69H,73H,74H,0eH,12H
                DB      00H,80H,0a5H,70H,72H,6fH,63H,5fH
                DB      69H,6eH,66H,6fH,04H,41H,80H,0a6H
                DB      08H,60H,01H,00H,02H,00H,00H,00H
                DB      07H,61H,00H,29H,69H,64H,78H,0fH
                DB      12H,00H,80H,0a8H,6cH,6eH,61H,6dH
                DB      65H,5fH,69H,6eH,66H,6fH,04H,41H
                DB      80H,0a9H,0fH,12H,01H,80H,0adH,66H
                DB      69H,65H,6cH,64H,5fH,6cH,69H,73H
                DB      74H,04H,41H,80H,0abH,08H,60H,03H
                DB      00H,0cH,00H,00H,00H,09H,61H,08H
                DB      11H,76H,61H,6cH,75H,65H,0fH,61H
                DB      04H,11H,69H,6eH,69H,74H,69H,61H
                DB      6cH,69H,7aH,65H,72H,09H,61H,00H
                DB      80H,0acH,6eH,65H,78H,74H,0fH,12H
                DB      00H,80H,0abH,66H,69H,65H,6cH,64H
                DB      5fH,6cH,69H,73H,74H,04H,41H,80H
                DB      0aeH,08H,60H,04H,00H,0cH,00H,00H
                DB      00H,09H,61H,08H,80H,0afH,74H,61H
                DB      69H,6cH,09H,61H,04H,80H,0afH,68H
                DB      65H,61H,64H,0dH,61H,02H,07H,61H
                DB      6cH,69H,67H,6eH,6dH,65H,6eH,74H
                DB      08H,61H,00H,07H,73H,69H,7aH,65H
                DB      10H,12H,00H,80H,0b0H,73H,74H,72H
                DB      75H,63H,74H,5fH,69H,6eH,66H,6fH
                DB      04H,41H,80H,0b1H,08H,60H,03H,00H
                DB      08H,00H,00H,00H,0cH,61H,06H,17H
                DB      64H,69H,73H,74H,61H,6eH,63H,65H
                DB      08H,61H,02H,1aH,73H,69H,7aH,65H
                DB      07H,61H,00H,17H,69H,64H,78H,0eH
                DB      12H,00H,80H,0b3H,63H,6fH,6dH,6dH
                DB      5fH,69H,6eH,66H,6fH,04H,41H,80H
                DB      0b4H,08H,60H,09H,00H,04H,00H,00H
                DB      00H,0dH,61H,00H,80H,0b5H,63H,6fH
                DB      6dH,6dH,69H,6eH,66H,6fH,0fH,61H
                DB      00H,80H,0b2H,73H,74H,72H,75H,63H
                DB      74H,69H,6eH,66H,6fH,0eH,61H,00H
                DB      80H,0aaH,6cH,6eH,61H,6dH,65H,69H
                DB      6eH,66H,6fH,0dH,61H,00H,40H,6dH
                DB      61H,63H,72H,6fH,69H,6eH,66H,6fH
                DB      0dH,61H,00H,80H,0a7H,70H,72H,6fH
                DB      63H,69H,6eH,66H,6fH,0eH,61H,00H
                DB      80H,96H,63H,6fH,6eH,73H,74H,69H
                DB      6eH,66H,6fH,0cH,61H,00H,80H,8eH
                DB      65H,78H,74H,69H,6eH,66H,6fH,0cH
                DB      61H,00H,80H,8bH,67H,72H,70H,69H
                DB      6eH,66H,6fH,0cH,61H,00H,80H,81H
                DB      73H,65H,67H,69H,6eH,66H,6fH,0aH
                DB      12H,02H,80H,0b6H,65H,6eH,74H,72H
                DB      79H,08H,60H,05H,00H,28H,00H,00H
                DB      00H,09H,61H,24H,80H,85H,70H,72H
                DB      65H,76H,09H,61H,20H,80H,85H,6eH
                DB      65H,78H,74H,08H,61H,1eH,19H,6cH
                DB      69H,6eH,65H,06H,61H,1aH,80H,0b7H
                DB      65H,07H,61H,00H,42H,73H,79H,6dH
                DB      0dH,12H,00H,80H,84H,64H,69H,72H
                DB      5fH,6eH,6fH,64H,65H,04H,41H,80H
                DB      0b9H,03H,41H,11H,09H,70H,06H,04H
                DB      11H,11H,06H,80H,0bbH,04H,20H,02H
                DB      08H,06H,70H,11H,02H,2eH,36H,05H
                DB      21H,0ffH,00H,08H,05H,70H,0dH,01H
                DB      36H,05H,71H,06H,01H,06H,05H,21H
                DB      0ffH,00H,08H,04H,20H,01H,08H,0bH
                DB      12H,00H,42H,61H,73H,6dH,5fH,73H
                DB      79H,6dH,04H,41H,80H,0c4H
$$TYPES         ENDS

                END
