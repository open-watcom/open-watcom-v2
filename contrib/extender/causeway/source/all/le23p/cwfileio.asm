.386P
                NAME    fileio
                EXTRN   DummyRET :NEAR
                EXTRN   freedos :NEAR
                EXTRN   mallocdos :NEAR
                EXTRN   free :NEAR
                EXTRN   malloc :NEAR
DGROUP          GROUP   _DATA
_TEXT           SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  OpenFile 
                PUBLIC  _OpenFile 
                PUBLIC  CreateFile 
                PUBLIC  _CreateFile 
                PUBLIC  _CloseFile 
                PUBLIC  CloseFile 
                PUBLIC  SeekFile 
                PUBLIC  _SeekFile 
                PUBLIC  ReadFile 
                PUBLIC  _ReadFile 
                PUBLIC  _ReadLineFile 
                PUBLIC  ReadLineFile 
                PUBLIC  ReadByteFile 
                PUBLIC  _ReadByteFile 
                PUBLIC  _WriteFile 
                PUBLIC  WriteFile 
                PUBLIC  WriteByteFile 
                PUBLIC  _WriteByteFile 
                PUBLIC  _FlushFile 
                PUBLIC  FlushFile 
_OpenFile:
OpenFile:       sub     esp,00000004H
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                mov     edx,dword ptr [esp+1ch]
                mov     eax,dword ptr [esp+20h]
                mov     ah,3dH
                int     21H
                jae     short L1
                push    eax
                xor     eax,eax
                push    dword ptr [esp+24h]
                push    dword ptr [esp+24h]
                call    dword ptr REDOpenFile
                lea     esp,[esp+8]
                pop     ebx
                or      eax,eax
                jne     short L1
                mov     eax,ebx
                jmp     near ptr L5
L1:             mov     ebx,eax
                mov     ecx,dword ptr _FIOBufferSize
                add     ecx,00000020H
                push    ecx
                call    near ptr mallocdos
                lea     esp,[esp+4]
                mov     esi,eax
                or      eax,eax
                mov     eax,00000001H
                jne     short L2
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                push    ecx
                call    near ptr malloc
                lea     esp,[esp+4]
                pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                mov     esi,eax
                or      eax,eax
                mov     eax,00000000H
                jne     short L2
                mov     ah,3eH
                int     21H
                mov     eax,00000008H
                jmp     short L5
L2:             mov     edi,esi
                push    eax
                push    ecx
                mov     ecx,00000008H
                xor     eax,eax
                or      ecx,ecx
                je      short L4
L3:             mov     dword ptr [edi],eax
                add     edi,00000004H
                dec     ecx
                jne     short L3
L4:             pop     ecx
                pop     eax
                mov     dword ptr [esi],eax
                mov     dword ptr [esi+4],ebx
                sub     ecx,00000020H
                mov     dword ptr [esi+14h],ecx
                mov     dword ptr [esp+14h],esi
                mov     dword ptr _FIOError,00000000H
                jmp     short L6
L5:             mov     dword ptr _FIOError,eax
                mov     dword ptr [esp+14h],00000000H
L6:             mov     eax,dword ptr [esp+14h]
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                lea     esp,[esp+4]
                ret     
                nop     
_CreateFile:
CreateFile:     sub     esp,00000004H
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                mov     edx,dword ptr [esp+1ch]
                mov     ecx,dword ptr [esp+20h]
                mov     ax,3c00H
                int     21H
                jb      short L10
                mov     ebx,eax
                mov     ecx,dword ptr _FIOBufferSize
                add     ecx,00000020H
                push    ecx
                call    near ptr mallocdos
                lea     esp,[esp+4]
                mov     esi,eax
                or      eax,eax
                mov     eax,00000001H
                jne     short L7
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                push    ecx
                call    near ptr malloc
                lea     esp,[esp+4]
                pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                mov     esi,eax
                or      eax,eax
                mov     eax,00000000H
                jne     short L7
                mov     ah,3eH
                int     21H
                mov     eax,00000008H
                jmp     short L10
L7:             mov     edi,esi
                push    eax
                push    ecx
                mov     ecx,00000008H
                xor     eax,eax
                or      ecx,ecx
                je      short L9
L8:             mov     dword ptr [edi],eax
                add     edi,00000004H
                dec     ecx
                jne     short L8
L9:             pop     ecx
                pop     eax
                mov     dword ptr [esi],eax
                mov     dword ptr [esi+4],ebx
                sub     ecx,00000020H
                mov     dword ptr [esi+14h],ecx
                mov     dword ptr [esp+14h],esi
                jmp     short L11
L10:            mov     dword ptr _FIOError,eax
                mov     dword ptr [esp+14h],00000000H
L11:            mov     eax,dword ptr [esp+14h]
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                lea     esp,[esp+4]
                ret     
                nop     
CloseFile:
_CloseFile:     push    ebx
                push    esi
                mov     ebx,dword ptr [esp+0ch]
                push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     short L14
                push    ebx
                mov     ebx,dword ptr [ebx+4]
                mov     ah,3eH
                int     21H
                pop     ebx
                jb      short L14
                mov     esi,ebx
                test    dword ptr [ebx],00000001H
                je      short L12
                push    esi
                call    near ptr freedos
                lea     esp,[esp+4]
                jmp     short L13
L12:            push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                push    esi
                call    near ptr free
                lea     esp,[esp+4]
                pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
L13:            xor     eax,eax
                jmp     short L14
L14:            mov     dword ptr _FIOError,eax
                pop     esi
                pop     ebx
                ret     
                nop     
_SeekFile:
SeekFile:       push    ebx
                push    ecx
                push    edx
                mov     eax,dword ptr [esp+18h]
                mov     ebx,dword ptr [esp+10h]
                mov     ecx,dword ptr [esp+14h]
                mov     edx,eax
                push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     short L18
                mov     eax,edx
                or      al,al
                je      short L17
                dec     al
                je      short L15
                dec     al
                je      short L16
                mov     eax,00000001H
                jmp     short L18
L15:            add     ecx,dword ptr [ebx+8]
                jmp     short L17
L16:            push    ebx
                push    ecx
                xor     cx,cx
                xor     dx,dx
                mov     ax,4202H
                mov     ebx,dword ptr [ebx+4]
                int     21H
                shl     edx,10H
                mov     dx,ax
                pop     ecx
                pop     ebx
                add     ecx,edx
L17:            mov     dword ptr [ebx+8],ecx
                mov     dword ptr [ebx+0ch],ecx
                mov     dword ptr [ebx+10h],00000000H
                xor     eax,eax
                mov     dword ptr _FIOError,eax
                mov     eax,ecx
                jmp     short L19
L18:            mov     dword ptr _FIOError,eax
                or      eax,0ffffffffH
L19:            pop     edx
                pop     ecx
                pop     ebx
                ret     
                nop     
_ReadFile:
ReadFile:       sub     esp,00000004H
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                mov     dword ptr [esp+18h],00000000H
                mov     ebx,dword ptr [esp+20h]
                mov     edx,dword ptr [esp+24h]
                mov     ecx,dword ptr [esp+28h]
                mov     edi,edx
                mov     ebp,ecx
L20:            or      ebp,ebp
                je      near ptr L29
                mov     eax,dword ptr [ebx+8]
                sub     eax,dword ptr [ebx+0ch]
                sub     eax,dword ptr [ebx+10h]
                neg     eax
                mov     ecx,ebp
                or      eax,eax
                je      short L25
                cmp     eax,ebp
                jae     short L21
                mov     ecx,eax
L21:            lea     esi,[ebx+20h]
                add     esi,dword ptr [ebx+8]
                sub     esi,dword ptr [ebx+0ch]
                push    ecx
                push    eax
                mov     eax,esi
                and     eax,00000003H
                sub     eax,00000004H
                neg     eax
                and     eax,00000003H
                je      short L22
                cmp     ecx,eax
                jb      short L22
                push    ecx
                mov     ecx,eax
                rep     movsb    
                pop     ecx
                sub     ecx,eax
L22:            push    ecx
                shr     ecx,02H
                je      short L24
L23:            mov     eax,dword ptr [esi]
                mov     dword ptr [edi],eax
                add     esi,00000004H
                add     edi,00000004H
                dec     ecx
                jne     short L23
L24:            pop     ecx
                and     ecx,00000003H
                rep     movsb    
                pop     eax
                pop     ecx
                add     dword ptr [ebx+8],ecx
                add     dword ptr [esp+18h],ecx
                sub     ebp,ecx
                je      near ptr L29
L25:            push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     near ptr L30
                push    ebx
                push    ecx
                push    edx
                mov     ecx,dword ptr [ebx+8]
                mov     dword ptr [ebx+0ch],ecx
                mov     dx,cx
                shr     ecx,10H
                mov     ebx,dword ptr [ebx+4]
                mov     ax,4200H
                int     21H
                pop     edx
                pop     ecx
                pop     ebx
                mov     dword ptr [ebx+10h],00000000H
                cmp     dword ptr [ebx+14h],ebp
                jae     short L26
                mov     edx,edi
                mov     ecx,ebp
                push    ebx
                mov     ebx,dword ptr [ebx+4]
                mov     ah,3fH
                int     21H
                pop     ebx

;                jb      short L30
                jb      L30

                add     dword ptr [esp+18h],eax
                add     eax,dword ptr [ebx+8]
                mov     dword ptr [ebx+8],eax
                mov     dword ptr [ebx+0ch],eax
                jmp     short L29
L26:            mov     ecx,dword ptr [ebx+14h]
                lea     edx,[ebx+20h]
                test    dword ptr [ebx],00000001H
                push    ebx
                mov     ebx,dword ptr [ebx+4]
                je      short L27
                push    edi
                sub     esp,00000032H
                mov     edi,esp
                mov     dword ptr [edi+1ch],00003f00H
                mov     dword ptr [edi+10h],ebx
                mov     dword ptr [edi+18h],ecx

				mov	WORD PTR [edi+20h],0			

                mov     eax,edx
                and     eax,0000000fH
                mov     dword ptr [edi+14h],eax
                shr     edx,04H
                mov     word ptr [edi+24h],dx
                mov     bl,21H
                mov     ax,0ff01H
                int     31H
                mov     eax,dword ptr [edi+1ch]
                movzx   eax,ax
                rcr     word ptr [edi+20h],1
                lea     esp,[esp+32h]
                pop     edi
                jmp     short L28
L27:            mov     ah,3fH
                int     21H
L28:            pop     ebx
                jb      short L30
                mov     dword ptr [ebx+10h],eax
                or      eax,eax
                jne     near ptr L20
L29:            mov     dword ptr _FIOError,00000000H
                mov     eax,dword ptr [esp+18h]
                jmp     short L31
L30:            mov     dword ptr _FIOError,eax
                or      eax,0ffffffffH
L31:            pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                lea     esp,[esp+4]
                ret     
                nop     
ReadLineFile:
_ReadLineFile:  pushad  
                mov     edi,dword ptr [esp+28h]
                xor     edx,edx
                mov     ecx,00000001H
L32:            push    dword ptr [esp+24h]
                call    near ptr ReadByteFile
                lea     esp,[esp+4]
                cmp     eax,0ffffffffH
                je      short L34
                inc     edx
                cmp     al,0dH
                je      short L33
                cmp     al,1aH
                je      short L32
                cmp     ecx,dword ptr [esp+2ch]
                jae     short L32
                mov     byte ptr [edi],al
                inc     edi
                inc     ecx
                jmp     short L32
L33:            push    dword ptr [esp+24h]
                call    near ptr ReadByteFile
                lea     esp,[esp+4]
                cmp     eax,0ffffffffH
                je      short L35
                cmp     al,0aH
                mov     eax,00000004H
                jne     short L35
                xor     eax,eax
                jmp     short L35
L34:            mov     eax,00000003H
                cmp     dword ptr _FIOError,0fffffffeH
                jne     short L35
                mov     eax,00000001H
                or      edx,edx
                je      short L35
                xor     eax,eax
                jmp     short L35
L35:            mov     byte ptr [edi],00H
                mov     dword ptr [esp+24h],eax
                popad   
                mov     eax,dword ptr [esp+4]
                ret     
                nop     
_ReadByteFile:
ReadByteFile:   push    ebx
                mov     ebx,dword ptr [esp+8]
                mov     eax,dword ptr [ebx+8]
                sub     eax,dword ptr [ebx+0ch]
                cmp     eax,dword ptr [ebx+10h]
                jb      short L36
                push    ecx
                push    edx
                push    edx
                mov     edx,esp
                mov     ecx,00000001H
                push    ecx
                push    edx
                push    ebx
                call    near ptr ReadFile
                lea     esp,[esp+0ch]
                pop     edx
                pop     edx
                pop     ecx
                cmp     eax,0ffffffffH
                je      short L38
                or      eax,eax
                mov     eax,0fffffffeH
                je      short L37
                dec     dword ptr [ebx+8]
                xor     eax,eax
L36:            movzx   eax,byte ptr [eax+ebx+20h]
                inc     dword ptr [ebx+8]
                mov     dword ptr _FIOError,00000000H
                jmp     short L39
L37:            mov     dword ptr _FIOError,eax
L38:            or      eax,0ffffffffH
L39:            pop     ebx
                ret     
                nop     
WriteFile:
_WriteFile:     sub     esp,00000004H
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ebp
                mov     dword ptr [esp+18h],00000000H
                mov     ebx,dword ptr [esp+20h]
                mov     edx,dword ptr [esp+24h]
                mov     ecx,dword ptr [esp+28h]
                mov     esi,edx
                mov     ebp,ecx
                or      ebp,ebp
                jne     short L40
                push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     near ptr L47
                push    ebx
                push    ecx
                push    edx
                mov     ecx,dword ptr [ebx+8]
                mov     dword ptr [ebx+0ch],ecx
                mov     dx,cx
                shr     ecx,10H
                mov     ebx,dword ptr [ebx+4]
                mov     ax,4200H
                int     21H
                pop     edx
                pop     ecx
                pop     ebx
                push    ebx
                mov     ebx,dword ptr [ebx+4]
                mov     ah,40H
                xor     ecx,ecx
                int     21H
                pop     ebx
                jb      near ptr L47
                jmp     short L46
L40:            or      ebp,ebp
                je      short L46
                mov     eax,dword ptr [ebx+14h]
                sub     eax,dword ptr [ebx+10h]
                mov     ecx,ebp
                cmp     eax,ebp
                jae     short L41
                mov     ecx,eax
                or      eax,eax
                je      short L45
L41:            lea     edi,[ebx+20h]
                add     edi,dword ptr [ebx+8]
                sub     edi,dword ptr [ebx+0ch]
                push    ecx
                push    eax
                mov     eax,esi
                and     eax,00000003H
                sub     eax,00000004H
                neg     eax
                and     eax,00000003H
                je      short L42
                cmp     ecx,eax
                jb      short L42
                push    ecx
                mov     ecx,eax
                rep     movsb    
                pop     ecx
                sub     ecx,eax
L42:            push    ecx
                shr     ecx,02H
                je      short L44
L43:            mov     eax,dword ptr [esi]
                mov     dword ptr [edi],eax
                add     esi,00000004H
                add     edi,00000004H
                dec     ecx
                jne     short L43
L44:            pop     ecx
                and     ecx,00000003H
                rep     movsb    
                pop     eax
                pop     ecx
                or      dword ptr [ebx],00000002H
                add     dword ptr [ebx+8],ecx
                add     dword ptr [ebx+10h],ecx
                add     dword ptr [esp+18h],ecx
                sub     ebp,ecx
                je      short L46
L45:            push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     short L47
                jmp     short L40
L46:            mov     dword ptr _FIOError,00000000H
                mov     eax,dword ptr [esp+18h]
                jmp     short L48
L47:            mov     dword ptr _FIOError,eax
                or      eax,0ffffffffH
L48:            pop     ebp
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                lea     esp,[esp+4]
                ret     
                nop     
_WriteByteFile:
WriteByteFile:  push    ebx
                push    ecx
                mov     ebx,dword ptr [esp+0ch]
                mov     ecx,dword ptr [ebx+8]
                sub     ecx,dword ptr [ebx+0ch]
                cmp     ecx,dword ptr [ebx+14h]
                jb      short L49
                push    ebx
                call    near ptr _FlushFile
                lea     esp,[esp+4]
                or      eax,eax
                jne     short L50
                xor     ecx,ecx
L49:            mov     eax,dword ptr [esp+10h]
                mov     byte ptr [ecx+ebx+20h],al
                or      dword ptr [ebx],00000002H
                inc     dword ptr [ebx+10h]
                inc     dword ptr [ebx+8]
                mov     dword ptr _FIOError,00000000H
                jmp     short L51
L50:            mov     dword ptr _FIOError,eax
                or      eax,0ffffffffH
L51:            pop     ecx
                pop     ebx
                ret     
                nop     
FlushFile:
_FlushFile:     push    ebx
                mov     ebx,dword ptr [esp+8]
                xor     eax,eax
                test    dword ptr [ebx],00000002H
                clc     
                je      near ptr L56
                push    ecx
                push    edx
                push    ebx
                mov     ecx,dword ptr [ebx+0ch]
                mov     dx,cx
                shr     ecx,10H
                mov     ebx,dword ptr [ebx+4]
                mov     ax,4200H
                int     21H
                pop     ebx
                lea     edx,[ebx+20h]
                mov     ecx,dword ptr [ebx+10h]
                test    dword ptr [ebx],00000001H
                push    ebx
                mov     ebx,dword ptr [ebx+4]
                je      short L52
                push    edi
                sub     esp,00000032H
                mov     edi,esp
                mov     dword ptr [edi+1ch],00004000H
                mov     dword ptr [edi+10h],ebx
                mov     dword ptr [edi+18h],ecx

				mov	WORD PTR [edi+20h],0			

                mov     eax,edx
                and     eax,0000000fH
                mov     dword ptr [edi+14h],eax
                shr     edx,04H
                mov     word ptr [edi+24h],dx
                mov     bl,21H
                mov     ax,0ff01H
                int     31H
                mov     eax,dword ptr [edi+1ch]
                movzx   eax,ax
                rcr     word ptr [edi+20h],1
                lea     esp,[esp+32h]
                pop     edi
                jmp     short L53
L52:            mov     ah,40H
                int     21H
L53:            pop     ebx
                jb      short L54
                and     dword ptr [ebx],0fffffffdH
                mov     dword ptr [ebx+10h],00000000H
                mov     eax,dword ptr [ebx+8]
                mov     dword ptr [ebx+0ch],eax
                xor     eax,eax
                mov     dword ptr _FIOError,eax
                jmp     short L55
L54:            mov     dword ptr _FIOError,eax
L55:            pop     edx
                pop     ecx
L56:            pop     ebx
                ret     
                nop     
_TEXT           ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
                PUBLIC  FIOBufferSize 
                PUBLIC  _FIOBufferSize 
                PUBLIC  FIOError 
                PUBLIC  _FIOError 
                PUBLIC  REDOpenFile 
FIOBufferSize	LABEL	BYTE
_FIOBufferSize  DB      00H,08H,00H,00H
FIOError		LABEL BYTE
_FIOError       DB      00H,00H,00H,00H
REDOpenFile     DD      DummyRET
_DATA           ENDS

                END
