     .386

externdef ?PUSHVALUE:ABS

_TEXT16 segment 'CODE' USE16

proc16 proc
     mov eax,ds
     mov ax,ds
     mov [bx],ds
     mov [ebx],ds
     mov word ptr [bx],ds
     mov word ptr [ebx],ds
     mov dword ptr [bx],ds
     mov dword ptr [ebx],ds
     mov ds,eax
     mov ds,ax
     mov ds,[bx]
     mov ds,[ebx]
     mov ds,word ptr [bx]
     mov ds,word ptr [ebx]
     mov ds,dword ptr [bx]
     mov ds,dword ptr [ebx]
     ret
proc16 endp

_TEXT16 ends

_TEXT32 segment 'CODE' USE32

proc32 proc
     mov eax,ds
     mov ax,ds
     mov [bx],ds
     mov [ebx],ds
     mov word ptr [bx],ds
     mov word ptr [ebx],ds
     mov dword ptr [bx],ds
     mov dword ptr [ebx],ds
     mov ds,eax
     mov ds,ax
     mov ds,[bx]
     mov ds,[ebx]
     mov ds,word ptr [bx]
     mov ds,word ptr [ebx]
     mov ds,dword ptr [bx]
     mov ds,dword ptr [ebx]
     ret
proc32 endp

_TEXT32 ends

     end
