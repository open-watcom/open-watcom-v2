;
; Only used to produce the data-array in misc.c; not put in a library
; NB! Requires TASM
;
.486
.model flat,c
.code

intel_bswap:
      mov eax, [esp+4]
      bswap eax
      ret

intel16_bswap:
      mov eax, [esp+4]
      bswap eax
      shr eax, 16
      ret

.model large,c
.code

intel_bswap_small:
      enter 0,0
      mov ax, [bp+8]
      mov dx, [bp+12]
      bswap ax
      bswap dx
      leave
      ret

intel16_bswap_small:
      enter 0, 0
      mov ax, [bp+8]
      bswap ax
      leave
      ret


intel_bswap_large:
      enter 0, 0
      mov ax, [bp+12]
      mov dx, [bp+16]
      bswap ax
      bswap dx
      leave
      retf

intel16_bswap_large:
      enter 0, 0
      mov ax, [bp+12]
      bswap ax
      leave
      retf  


.model flat,c

.data
         dd ?
   where dd ?

.code
   jmp dword ptr [where]
   

ffs_386:
    bsf eax, [esp+4]
    jnz short @1
    mov eax, -1
@1: inc eax
    ret


end
