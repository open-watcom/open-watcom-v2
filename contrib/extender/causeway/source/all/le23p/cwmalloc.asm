.386p
                NAME    mallocd
DGROUP          GROUP   _DATA
_TEXT           SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  _MallocDOS 
                PUBLIC  mallocdos 
mallocdos:
_MallocDOS:     push    ebx
                push    ecx
                push    esi
                push    edi
                xor     ecx,ecx
                xchg    ecx,dword ptr [esp+14H]
                add     ecx,0000000fH
                shr     ecx,04H
                sub     esp,00000032H
                mov     edi,esp
                mov     dword ptr [edi+1ch],00004800H
                mov     dword ptr [edi+10h],ecx
                xor     ecx,ecx

;                mov     word ptr +2eH[edi],0000H
;                mov     word ptr +30H[edi],0000H
				mov	[edi+2eh],ecx
				mov	[edi+30h],ecx
				mov	[edi+20h],cx
 
                mov     ebx,00000021H
                mov     ax,0300H
                int     31H
                rcr     word ptr [edi+20h],1
                lea     esp,[esp+32h]
                jb      short L1
                mov     eax,dword ptr [edi+1ch]
                xor     esi,esi
                mov     si,ax
                shl     esi,04H
                mov     dword ptr [esp+14h],esi
L1:             pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                mov     eax,dword ptr [esp+4]
                ret     
                nop     
_TEXT           ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

                END
