*** warning: Unknown Record type 80H at record 9 ***
.386p
                NAME    stk2reg
                EXTRN   kbhit_ :BYTE
                EXTRN   write_ :BYTE
                EXTRN   read_ :BYTE
                EXTRN   lseek_ :BYTE
                EXTRN   close_ :BYTE
                EXTRN   open_ :BYTE
                EXTRN   strlen_ :BYTE
                EXTRN   strcat_ :BYTE
                EXTRN   strcpy_ :BYTE
                EXTRN   free_ :BYTE
                EXTRN   realloc_ :BYTE
                EXTRN   malloc_ :BYTE
                EXTRN   __argv :BYTE
                EXTRN   __psp :BYTE
DGROUP          GROUP   _DATA
_TEXT           SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  strcpy 
                PUBLIC  strcat 
                PUBLIC  strlen 
                PUBLIC  open 
                PUBLIC  close 
                PUBLIC  lseek 
                PUBLIC  read 
                PUBLIC  write 
                PUBLIC  kbhit 
                PUBLIC  free 
                PUBLIC  malloc 
                PUBLIC  realloc 
strcpy:         push    edx
                mov     eax,dword ptr +8H[esp]
                mov     edx,dword ptr +0cH[esp]
                call    near ptr strcpy_
                pop     edx
                ret     
                nop     
strcat:         push    edx
                mov     eax,dword ptr +8H[esp]
                mov     edx,dword ptr +0cH[esp]
                call    near ptr strcat_
                pop     edx
                ret     
                nop     
strlen:         mov     eax,dword ptr +4H[esp]
                call    near ptr strlen_
                ret     
                nop     
open:           push    edx
                push    ebx
                mov     eax,dword ptr +0cH[esp]
                mov     edx,dword ptr +10H[esp]
                mov     ebx,dword ptr +14H[esp]
                call    near ptr open_
                pop     ebx
                pop     edx
                ret     
                nop     
close:          mov     eax,dword ptr +4H[esp]
                call    near ptr close_
                ret     
                nop     
lseek:          push    edx
                push    ebx
                mov     eax,dword ptr +0cH[esp]
                mov     edx,dword ptr +10H[esp]
                mov     ebx,dword ptr +14H[esp]
                call    near ptr lseek_
                pop     ebx
                pop     edx
                ret     
                nop     
read:           push    edx
                push    ebx
                mov     eax,dword ptr +0cH[esp]
                mov     edx,dword ptr +10H[esp]
                mov     ebx,dword ptr +14H[esp]
                call    near ptr read_
                pop     ebx
                pop     edx
                ret     
                nop     
write:          push    edx
                push    ebx
                mov     eax,dword ptr +0cH[esp]
                mov     edx,dword ptr +10H[esp]
                mov     ebx,dword ptr +14H[esp]
                call    near ptr write_
                pop     ebx
                pop     edx
                ret     
                nop     
kbhit:          jmp     near ptr kbhit_
free:           mov     eax,dword ptr +4H[esp]
                call    near ptr free_
                ret     
                nop     
malloc:         mov     eax,dword ptr +4H[esp]
                call    near ptr malloc_
                ret     
                nop     
realloc:        push    edx
                mov     eax,dword ptr +8H[esp]
                mov     edx,dword ptr +0cH[esp]
                call    near ptr realloc_
                pop     edx
                ret     
                nop     
_TEXT           ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

                END
