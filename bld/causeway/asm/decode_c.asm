decode_c_struc struc
DecC_ID db "CWC"
DecC_Bits       db 0
DecC_Len        dd ?
DecC_Size       dd ?
decode_c_struc  ends


RepMinSize      equ     2

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
__0:
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
        push    ebx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
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
        jc      dec1_error
        cmp     ax,cx
        jnz     dec1_error
        ;
        ;Check for main ID string.
        ;
        mov     eax,d[decode_c_head.DecC_ID]
        and     eax,0FFFFFFh
        cmp     eax,"CWC"
        jnz     dec1_error
        mov     ecx,d[decode_c_head.DecC_Len]
        mov     eax,d[decode_c_head.DecC_Size]
        add     eax,size decode_c_struc
        clc
        jmp     dec1_exit
        ;
dec1_error:
        stc
        ;
dec1_exit:
        pushf
        push    eax
        push    ecx
        mov     ax,4200h
        mov     dx,w[decode_c_count]
        mov     cx,w[decode_c_count+2]
        int     21h
        pop     ecx
        pop     eax
        popf
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ebx
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
        push    ebx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
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
        mov     ecx,DWORD PTR ds:[EPSP_Struc.EPSP_TransSize]
        mov     ax,WORD PTR ds:[EPSP_Struc.EPSP_TransProt]
        mov     bx,WORD PTR ds:[EPSP_Struc.EPSP_TransReal]
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
        jc      dec2_read_error
        cmp     ax,cx
        jnz     dec2_read_error
        mov     decode_c_count,0        ;size decode_c_struc
        ;
        ;Check for main ID string.
        ;
        mov     eax,d[decode_c_head.DecC_ID]
        and     eax,0FFFFFFh
        cmp     eax,"CWC"
        jnz     dec2_cwc_error
        movzx   ecx,b[decode_c_head.DecC_Bits]
        mov     b[dec2_Shifter+2],cl
        mov     eax,1
        shl     eax,cl
        dec     eax
        mov     d[dec2_Masker+2],eax
        mov     ebx,d[decode_c_head.DecC_Len]
;
;Get on with decodeing the data.
;
        les     edi,OutAddr
        mov     ds,cDiskBufferSeg
        xor     esi,esi
        call    dec2_FillBuffer
        mov     ebp,[esi]
        add     esi,4
        mov     dl,32
        mov     dh,dl
;
;The main decompresion loop.
;
        align 4
dec2_0: _DCD_ReadBit
        jnc     dec2_1
        ;
        ;Read a raw byte.
        ;
        mov     al,[esi]
        mov     es:[edi],al
        inc     esi
        inc     edi
        dec     ebx
        js      dec2_4
        jmp     dec2_0
        ;
        align 4
dec2_1: _DCD_ReadBit
        jnc     dec2_2
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
        js      dec2_1_0
        add     eax,ecx
        push    esi
        push    ds
        push    es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      dec2_4
        rep     movsb
        pop     ds
        pop     esi
        jmp     dec2_0
        ;
        ;Do a run with 2 bit length.
        ;
        align 4
dec2_1_0:
        mov     al,[esi]
        inc     esi
        inc     cl
        sub     ebx,ecx
        js      dec2_4
        rep     stosb
        jmp     dec2_0
        ;
        align 4
dec2_2: _DCD_ReadBit
        jnc     dec2_3
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
        js      dec2_2_0
        add     eax,ecx
        push    esi
        push    ds
        push    es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      dec2_4
        rep     movsb
        pop     ds
        pop     esi
        jmp     dec2_0
        ;
        ;Do a run with 4 bit length.
        ;
        align 4
dec2_2_0:
        mov     al,[esi]
        inc     esi
        inc     cl
        sub     ebx,ecx
        js      dec2_4
        rep     stosb
        jmp     dec2_0
        ;
        ;Do a rep with 12-bit position, 12-bit length.
        ;
        align 4
dec2_3: mov     eax,[esi]
        add     esi,3
        and     eax,0FFFFFFh
        mov     ecx,eax
dec2_Masker:
        and     ecx,4095
        add     ecx,2
dec2_Shifter:
        shr     eax,12
        dec     eax
        js      dec2_3_0
        add     eax,ecx
        push    esi
        push    ds
        push    es
        pop     ds
        mov     esi,edi
        sub     esi,eax
        sub     ebx,ecx
        js      dec2_4
        rep     movsb
        pop     ds
        pop     esi
        jmp     dec2_0
        ;
        ;Check for special codes of 0-15 (Would use 2 or 4 bit if really needed)
        ;
        align 4
dec2_3_0:
        cmp     ecx,RepMinSize+15+1
        jnc     dec2_3_1
        cmp     cl,RepMinSize+2 ;Rationalise destination?
        jz      dec2_0          ;ignore destination stuff.
        cmp     cl,RepMinSize+1 ;Re-fill buffer?
        jz      dec2_3_2
        cmp     cl,RepMinSize+3 ;Literal string?
        jz      dec2_3_3
        cmp     cl,RepMinSize   ;Terminator?
        jz      dec2_4
        jmp     dec2_form_error
        ;
        ;Do a run with 12-bit length.
        ;
        align 4
dec2_3_1:
        mov     al,[esi]
        inc     esi
        inc     ecx
        sub     ebx,ecx
        js      dec2_4
        rep     stosb
        jmp     dec2_0
        ;
        ;Buffer needs to be re-filled.
        ;
        align 4
dec2_3_2:
        call    dec2_FillBuffer
        jmp     dec2_0
        ;
        ;Get literal string of bytes.
        ;
        align 4
dec2_3_3:
        xor     ecx,ecx
        mov     cl,[esi]                ;get the length.
        inc     esi
        sub     ebx,ecx
        js      dec2_4
        rep     movsb           ;copy them.
        jmp     dec2_0
        ;
        ;We're all done so exit.
        ;
        align 4
dec2_4: assume ds:nothing
        ;
        or      ebx,ebx
        jnz     dec2_form_error
        ;
        mov     ecx,DWORD PTR cs:[decode_c_head.DecC_Len]       ;return length.
        xor     eax,eax
        clc
        jmp     dec2_exit
        ;
dec2_read_error:
        mov     eax,1
        stc
        jmp     dec2_exit
        ;
dec2_form_error:
        mov     eax,2
        stc
        jmp     dec2_exit
        ;
dec2_cwc_error:
        assume ds:nothing
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
dec2_exit:
        mov     esp,cs:CWCStack
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ebx
        ret
;
;Re-fill the disk buffer.
;
dec2_FillBuffer:
        pushad
        push    ds
        push    es
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    ds
        pop     es
        ;
        mov     ecx,cDiskBufferMax      ;get block size.
        or      esi,esi
        jz      dec2_fb0
        ;
        sub     ecx,esi         ;get length still in the buffer.
        xor     edi,edi
        push    ecx
        push    esi
        push    ds
        push    es
        mov     ds,cDiskBufferSeg
        push    ds
        pop     es
        rep     movsb           ;copy remaining data down.
        pop     es
        pop     ds
        pop     esi
        pop     ecx
        xchg    esi,ecx
        ;
dec2_fb0:
        mov     edi,offset decode_c_buffer
        mov     eax,decode_c_count      ;get current count.
        add     eax,ecx
        cmp     eax,d[decode_c_head.DecC_Size]  ;check against total size.
        jc      dec2_FB_2
        sub     eax,d[decode_c_head.DecC_Size]
        sub     ecx,eax         ;reduce ECX as needed.
dec2_FB_2:
        add     decode_c_count,ecx      ;update total count.
        ;
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     ax,cDiskBufferReal
        mov     RealRegsStruc.Real_DS[edi],ax
        mov     RealRegsStruc.Real_EDX[edi],esi
        mov     RealRegsStruc.Real_EAX[edi],3f00h
        mov     ax,decode_c_handle
        mov     RealRegsStruc.Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX           ;fill the buffer again.
        test    RealRegsStruc.Real_Flags[edi],1
        jnz     dec2_read_error
        mov     eax,RealRegsStruc.Real_EAX[edi]
        mov     ecx,RealRegsStruc.Real_ECX[edi]
        cmp     ax,cx
        jnz     dec2_read_error
        pop     es
        pop     ds
        popad
        xor     esi,esi
        ret
        assume ds:_apiCode
DecodeCWC       endp
