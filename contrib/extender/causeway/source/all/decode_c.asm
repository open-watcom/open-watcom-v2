decode_c_struc struc
DecC_ID db "CWC"
DecC_Bits       db 0
DecC_Len        dd ?
DecC_Size       dd ?
decode_c_struc  ends


RepMinSize      equ     2


;
;Get next input bit.
;
_DCD_ReadBit    macro
        local __0
        adc     ebp,ebp
        dec     dl
        jnz     __0
        mov     ebp,[esi]
        lea     esi,4[esi]
        mov     dl,dh
__0:    ;
        endm


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Return expanded length of a CWC'd file.
;
;On Entry:
;
;BX     - File handle.
;
;Carry set on error else,
;
;ECX    - Expanded data length.
;EDX    - Compressed data length.
;
GetCWCInfo      proc    near
        pushm   ebx,edx,esi,edi,ebp,ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        ;Get current file pointer position.
        ;
        mov     ax,4201h
        xor     cx,cx
        xor     dx,dx
        int     21h
        mov     w[decode_c_count],ax
        mov     w[decode_c_count+2],dx
        ;
        ;Read the header.
        ;
        mov     edx,offset decode_c_head

; MED 12/18/96, as per JW
        xor     ecx,ecx
        mov     cx,size decode_c_struc

        mov     ah,3fh
        int     21h
        jc      @@error
        cmp     ax,cx
        jnz     @@error
        ;
        ;Check for main ID string.
        ;
        mov     eax,d[decode_c_head+DecC_ID]
        and     eax,0FFFFFFh
        cmp     eax,"CWC"
        jnz     @@error
        mov     ecx,d[decode_c_head+DecC_Len]
        mov     eax,d[decode_c_head+DecC_Size]
        add     eax,size decode_c_struc
        clc
        jmp     @@exit
        ;
@@error:        stc
        ;
@@exit: pushf
        pushm   eax,ecx
        mov     ax,4200h
        mov     dx,w[decode_c_count]
        mov     cx,w[decode_c_count+2]
        int     21h
        popm    eax,ecx
        popf
        popm    ebx,edx,esi,edi,ebp,ds
        ret
GetCWCInfo      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Decompress CWC file. Source is a file on disk, destination is anywhere in
;memory.
;
;On Entry:-
;
;BX     - Source File Handle.
;ES:EDI - Destination for data.
;
;On Exit:-
;
;EAX    - Exit status.
;       0 = No problems
;       1 = Error during file access.
;       2 = Bad data.
;       3 = Not a CWC'd file.
;
;ECX    - Length of data produced.
;
DecodeCWC       proc    near
        cld
        pushm   ebx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     CWCStack,esp            ;Set for error exit.
        mov     Decode_c_Handle,bx      ;Store the handle.
        mov     d[OutAddr],edi  ;Store destination.
        mov     w[OutAddr+4],es ;/
;
;Setup disk buffer variables.
;
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        assume ds:nothing
        mov     ecx,ds:d[EPSP_TransSize]
        mov     ax,ds:w[EPSP_TransProt]
        mov     bx,ds:w[EPSP_TransReal]
        assume ds:_apiCode
        pop     ds
        mov     cDiskBufferMax,ecx
        mov     cDiskBufferSeg,ax
        mov     cDiskBufferReal,bx
;
;Get the header so we can check which type it is.
;
        mov     edx,offset decode_c_head
        mov     cx,size decode_c_struc
        mov     ah,3fh
        mov     bx,decode_c_handle
        int     21h
        jc      @@read_error
        cmp     ax,cx
        jnz     @@read_error
        mov     decode_c_count,0        ;size decode_c_struc
        ;
        ;Check for main ID string.
        ;
        mov     eax,d[decode_c_head+DecC_ID]
        and     eax,0FFFFFFh
        cmp     eax,"CWC"
        jnz     @@cwc_error
        movzx   ecx,b[decode_c_head+DecC_Bits]
        mov     b[@@Shifter+2],cl
        mov     eax,1
        shl     eax,cl
        dec     eax
        mov     d[@@Masker+2],eax
        mov     ebx,d[decode_c_head+DecC_Len]
;
;Get on with decodeing the data.
;
        les     edi,OutAddr
        mov     ds,cDiskBufferSeg
        xor     esi,esi
        call    @@FillBuffer
        mov     ebp,[esi]
        add     esi,4
        mov     dl,32
        mov     dh,dl
;
;The main decompresion loop.
;
        align 4
@@0:    _DCD_ReadBit
        jnc     @@1
        ;
        ;Read a raw byte.
        ;
        mov     al,[esi]
        mov     es:[edi],al
        inc     esi
        inc     edi
        dec     ebx
        js      @@4
        jmp     @@0
        ;
        align 4
@@1:    _DCD_ReadBit
        jnc     @@2
        ;
        ;Do a rep with 8 bit position, 2 bit length.
        ;
        xor     ecx,ecx
        _DCD_ReadBit
        adc     cl,cl
        _DCD_ReadBit
        adc     cl,cl
        add     cl,2
        xor     eax,eax
        mov     al,[esi]
        inc     esi
        dec     eax
        js      @@1_0
        add     eax,ecx
        pushm   esi,ds,es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      @@4
        rep     movsb
        popm    esi,ds
        jmp     @@0
        ;
        ;Do a run with 2 bit length.
        ;
        align 4
@@1_0:  mov     al,[esi]
        inc     esi
        inc     cl
        sub     ebx,ecx
        js      @@4
        rep     stosb
        jmp     @@0
        ;
        align 4
@@2:    _DCD_ReadBit
        jnc     @@3
        ;
        ;Do a rep with 12 bit position, 4 bit length.
        ;
        xor     eax,eax
        mov     ax,[esi]
        add     esi,2
        mov     cl,al
        and     ecx,15
        add     cl,2
        shr     eax,4
        dec     eax
        js      @@2_0
        add     eax,ecx
        pushm   esi,ds,es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      @@4
        rep     movsb
        popm    esi,ds
        jmp     @@0
        ;
        ;Do a run with 4 bit length.
        ;
        align 4
@@2_0:  mov     al,[esi]
        inc     esi
        inc     cl
        sub     ebx,ecx
        js      @@4
        rep     stosb
        jmp     @@0
        ;
        ;Do a rep with 12-bit position, 12-bit length.
        ;
        align 4
@@3:    mov     eax,[esi]
        add     esi,3
        and     eax,0FFFFFFh
        mov     ecx,eax
@@Masker:       and     ecx,4095
        add     ecx,2
@@Shifter:      shr     eax,12
        dec     eax
        js      @@3_0
        add     eax,ecx
        pushm   esi,ds,es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      @@4
        rep     movsb
        popm    esi,ds
        jmp     @@0
        ;
        ;Check for special codes of 0-15 (Would use 2 or 4 bit if really needed)
        ;
        align 4
@@3_0:  cmp     ecx,RepMinSize+15+1
        jnc     @@3_1
        cmp     cl,RepMinSize+2 ;Rationalise destination?
        jz      @@0             ;ignore destination stuff.
        cmp     cl,RepMinSize+1 ;Re-fill buffer?
        jz      @@3_2
        cmp     cl,RepMinSize+3 ;Literal string?
        jz      @@3_3
        cmp     cl,RepMinSize   ;Terminator?
        jz      @@4
        jmp     @@form_error
        ;
        ;Do a run with 12-bit length.
        ;
        align 4
@@3_1:  mov     al,[esi]
        inc     esi
        inc     ecx
        sub     ebx,ecx
        js      @@4
        rep     stosb
        jmp     @@0
        ;
        ;Buffer needs to be re-filled.
        ;
        align 4
@@3_2:  call    @@FillBuffer
        jmp     @@0
        ;
        ;Get literal string of bytes.
        ;
        align 4
@@3_3:  xor     ecx,ecx
        mov     cl,[esi]                ;get the length.
        inc     esi
        sub     ebx,ecx
        js      @@4
        rep     movsb           ;copy them.
        jmp     @@0
        ;
        ;We're all done so exit.
        ;
        align 4
@@4:    assume ds:nothing
        ;
        or      ebx,ebx
        jnz     @@form_error
        ;
        mov     ecx,cs:d[decode_c_head+DecC_Len]        ;return length.
        xor     eax,eax
        clc
        jmp     @@exit
        ;
@@read_error:   mov     eax,1
        stc
        jmp     @@exit
        ;
@@form_error:   mov     eax,2
        stc
        jmp     @@exit
        ;
@@cwc_error:    assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     bx,decode_c_handle
        mov     ax,4001h
        mov     dx,-(size decode_c_struc)
        or      cx,-1
        int     21h
        mov     eax,3
        stc
        assume ds:nothing
        ;
@@exit: mov     esp,cs:CWCStack
        popm    ebx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
;
;Re-fill the disk buffer.
;
@@FillBuffer:   pushad
        pushm   ds,es
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    ds
        pop     es
        ;
        mov     ecx,cDiskBufferMax      ;get block size.
        or      esi,esi
        jz      @@fb0
        ;
        sub     ecx,esi         ;get length still in the buffer.
        xor     edi,edi
        pushm   ecx,esi,ds,es
        mov     ds,cDiskBufferSeg
        push    ds
        pop     es
        rep     movsb           ;copy remaining data down.
        popm    ecx,esi,ds,es
        xchg    esi,ecx
        ;
@@fb0:  mov     edi,offset decode_c_buffer
        mov     eax,decode_c_count      ;get current count.
        add     eax,ecx
        cmp     eax,d[decode_c_head+DecC_Size]  ;check against total size.
        jc      @@FB_2
        sub     eax,d[decode_c_head+DecC_Size]
        sub     ecx,eax         ;reduce ECX as needed.
@@FB_2: add     decode_c_count,ecx      ;update total count.
        ;
        mov     Real_ECX[edi],ecx
        mov     ax,cDiskBufferReal
        mov     Real_DS[edi],ax
        mov     Real_EDX[edi],esi
        mov     Real_EAX[edi],3f00h
        mov     ax,decode_c_handle
        mov     Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX           ;fill the buffer again.
        test    Real_Flags[edi],1
        jnz     @@read_error
        mov     eax,Real_EAX[edi]
        mov     ecx,Real_ECX[edi]
        cmp     ax,cx
        jnz     @@read_error
        popm    ds,es
        popad
        xor     esi,esi
        ret
        assume ds:_apiCode
DecodeCWC       endp

;
        align 4
cDiskBufferMax  dd ?
cDiskBufferSeg  dw ?
cDiskBufferReal dw ?
;
decode_c_count  dd ?
CWCStack        dd ?
;
OutAddr df ?
decode_c_handle dw ?
;
        align 4
decode_c_Buffer db size v86CallStruc dup (?)
;
        align 4
decode_c_head   decode_c_struc <>
;
