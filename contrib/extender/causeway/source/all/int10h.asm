;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
        .386
_Int10h segment para public 'extension code' use32
        assume cs:_Int10h, ds:nothing, es:nothing
Int10hStart     label byte


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int10hOpen      proc    near
;
;Setup int 10h patch.
;
        assume ds:_Int10h
        assume es:_cwMain
        mov     Int10hDSeg,es   ;Store cwCode selector.
        mov     Int10hCSeg,cs   ;store this segment.
        mov     Int10hDDSeg,ds
        ;
        sys     GetSel          ;Get a selector
        jc      @@9
        mov     Int10hStaticSel,bx
        sys     GetSel
        jc      @@9
        mov     Int10hUltraFont,bx
        ;
        mov     bl,10h
        sys     GetVect
        test    es:SystemFlags,1
        jz      @@Use32
        mov     w[OldInt10h],dx
        mov     w[OldInt10h+2],cx
        jmp     @@Use0
@@Use32:        mov     d[OldInt10h],edx
        mov     w[OldInt10h+4],cx
@@Use0: mov     edx,offset Int10h
        mov     cx,cs
        mov     bl,10h
        sys     SetVect
        assume es:nothing
        assume ds:nothing
        clc
@@9:    ;
        db 66h
        retf
Int10hOpen      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int10hClose     proc    near
;
;Remove int 10h patch.
;
        push    ds
        mov     ds,cs:Int10hDDSeg
        assume ds:_Int10h
        cmp     d[OldInt10h],0
        jz      @@9
        mov     ds,Int10hDseg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int10hDDSeg
        assume ds:_Int10h
        jz      @@Use32
        movzx   edx,w[OldInt10h]
        mov     cx,w[OldInt10h+2]
        jmp     @@Use0
@@Use32:        mov     edx,d[OldInt10h]
        mov     cx,w[OldInt10h+4]
@@Use0: mov     bl,10h
        sys     SetVect
        assume ds:nothing
@@9:    pop     ds
        ;
        db 66h
        retf
Int10hClose     endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int10h  proc    far
        cmp     ax,1002h                ;set pen numbers?
        jz      @@SetPens
        cmp     ax,1009h                ;get pen numbers?
        jz      @@GetPens
        cmp     ax,1012h                ;set colour values?
        jz      @@SetColours
        cmp     ax,1017h                ;get colour values?
        jz      @@GetColours
        cmp     ax,1100h                ;Set font?
        jz      @@SetFont1
        cmp     ax,1110h                ;Set font and program CRTC?
        jz      @@SetFont1
        cmp     ax,1130h                ;Get font details?
        jz      @@GetFont
        cmp     ah,13h          ;write teletype string?
        jz      @@WriteString
        cmp     ah,1bh          ;state info?
        jz      @@VideoState
        cmp     ax,1c00h
        jz      @@StateSize
        cmp     ax,1c01h                ;state save?
        jz      @@StateSave
        cmp     ax,1c02h                ;state restore?
        jz      @@StateRestore
        cmp     ax,0cd00h               ;Ultra set palette?
        jz      @@UltraSetPal
        cmp     ax,0cd02h               ;Ultra get palette?
        jz      @@UltraGetPal
        cmp     ax,0cd10h               ;Ultra set font?
        jz      @@UltraSetFont
        jmp     @@NotOurs
        ;
@@GetFont:      ;Get font details?
        ;
        pushm   eax,ebx,esi,edi
        pushm   ds,fs
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        mov     bx,es:[edi].Real_ES
        mov     ax,0002h
        int     31h
        jc      @@9
        mov     ebp,es:[edi].Real_EBP
        movzx   ebp,bp
        mov     edx,es:[edi].Real_EDX
        mov     ecx,es:[edi].Real_ECX
        mov     es,ax
@@9:    popm    ds,fs
        popm    eax,ebx,esi,edi
        jmp     @@Done
        ;
@@UltraSetFont: ;UltraVision set font.
        ;
        pushm   ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit503
        movzx   ebp,bp
@@Use32Bit503:  xor     eax,eax
        mov     ax,ds
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit504
        movzx   esi,si
@@Use32Bit504:  pop     eax
        pushm   ax,bx,cx,dx
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        mov     ecx,8
        cld
        rep     movsb           ;copy into transfer buffer.
        popm    ax,bx,cx,dx
        pushm   ax,bx,cx,dx
        movzx   ax,bh
        mul     cx
        movzx   ecx,ax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        mov     edi,8
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        rep     movsb           ;copy into transfer buffer.
        popm    ax,bx,cx,dx
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EDX,edx
        mov     es:[edi].Real_ESI,0
        mov     es:[edi].Real_EBP,8
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_DS,ax
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        mov     eax,es:[edi].Real_EAX
        popm    ds,es,fs
        popm    ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@UltraGetPal:  ;UltraVision get palette.
        ;
        pushm   eax,ebx,ecx,edi,ebp
        pushm   es,fs
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        mov     es:[edi].Real_EAX,eax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        mov     ds,cs:Int10hDDseg
        assume ds:_Int10h
        mov     bx,Int10hUltraFont
        assume ds:nothing
        movzx   edx,es:[edi].Real_DS
        shl     edx,4
        mov     ecx,65535
        sys     SetSelDet32
        mov     ds,bx
        mov     edx,es:[edi].Real_EDX
        mov     esi,es:[edi].Real_ESI
        movzx   esi,si
        movzx   edx,dx
        popm    es,fs
        popm    eax,ebx,ecx,edi,ebp
        jmp     @@Done
        ;
@@UltraSetPal:  ;UltraVision set palette.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,ds
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit502
        movzx   edx,dx
@@Use32Bit502:  pop     eax
        mov     esi,edx         ;source buffer.
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        mov     ecx,16
        rep     movsb           ;copy into transfer buffer.
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_DS,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@VideoState:   ;Some sort of video state table function.
        ;
        or      bx,bx
        jnz     @@NotOurs
        pushm   ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit501
        movzx   edi,di
@@Use32Bit501:  pop     eax
        pushm   edi,es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        mov     eax,es:[edi].Real_EAX
        popm    edi,es
        cmp     al,1bh
        jz      @@ok100
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        xor     eax,eax
        jmp     @@Done
@@ok100:        push    eax
        push    edi
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,fs:w[EPSP_TransProt]
        assume ds:nothing
        mov     ecx,64
        cld
        rep     movsb           ;copy the results.
        pop     edi
        movzx   edx,es:w[edi+2]
        shl     edx,4
        mov     ecx,65535
        mov     ds,cs:Int10hDDseg
        assume ds:_Int10h
        mov     bx,Int10hStaticSel
        assume ds:nothing
        mov     es:w[edi+2],bx
        sys     SetSelDet32
        pop     eax
        popm    ds,es,fs
        popm    ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@SetFont1:     ;Load user font into character generator ram.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit500
        movzx   ebp,bp
@@Use32Bit500:  pop     eax
        pushm   ax,bx,cx,dx
        movzx   ax,bh
        mul     cx
        movzx   ecx,ax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        rep     movsb           ;copy into transfer buffer.
        popm    ax,bx,cx,dx
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EDX,edx
        mov     es:[edi].Real_EBP,0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@SetPens:      ;ES:DX - List of 17 bytes to send to video.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit50
        movzx   edx,dx
@@Use32Bit50:   pop     eax
        mov     esi,edx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        mov     ecx,17
        rep     movsb           ;copy into transfer buffer.
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].Real_EAX,1002h
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@GetPens:      ;ES:DX - Space for list of 17 pen numbers.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit51
        movzx   edx,dx
@@Use32Bit51:   pop     eax
        pushm   edx,es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].Real_EAX,1009h
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        popm    edi,es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,fs:w[EPSP_TransProt]
        assume ds:nothing
        mov     ecx,17
        cld
        rep     movsb           ;copy the results.
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@SetColours:   ;ES:DX - list of RGB values.
        ;CX    - number of values.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit52
        movzx   edx,dx
@@Use32Bit52:   pop     eax
        mov     esi,edx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        push    cx
        movzx   ecx,cx
        mov     eax,ecx
        add     ecx,ecx
        add     ecx,eax         ;3 bytes per entry.
        rep     movsb           ;copy into transfer buffer.
        pop     cx
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].Real_EAX,1012h
        mov     es:[edi].Real_EDX,edx
        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EBX,ebx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@GetColours:   ;ES:DX - Buffer for list of RGB values.
        ;CX    - Number of values.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit53
        movzx   edx,dx
@@Use32Bit53:   pop     eax
        pushm   ecx,edx,es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].Real_EAX,1017h
        mov     es:[edi].Real_EDX,edx
        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EBX,ebx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        popm    ecx,edi,es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,fs:w[EPSP_TransProt]
        assume ds:nothing
        movzx   ecx,cx
        mov     eax,ecx
        add     ecx,ecx
        add     ecx,eax         ;3 bytes per entry.
        cld
        rep     movsb           ;copy the results.
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@WriteString:  ;ES:BP - String to write.
        ;CX    - Number of characters to write.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit54
        movzx   ebp,bp
@@Use32Bit54:   pop     eax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        push    cx
        cmp     al,2
        jc      @@noatts
        add     cx,cx
@@noatts:       movzx   ecx,cx
        rep     movsb           ;copy into transfer buffer.
        pop     cx
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     ebp,ebp
        mov     es:[edi].Real_EAX,eax
;       mov     es:[edi].Real_EBP,edx
        mov     es:[edi].Real_EBP,ebp

        mov     es:[edi].Real_ECX,ecx
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@StateSize:    ;Return state size.
        ;
        pushm   eax,ecx,edi,es
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_ECX,ecx
        mov     bl,10h
        sys     IntXX
        mov     ebx,es:Real_EBX[edi]
        cmp     bx,2048/64
        jc      @@ss3
        ;
        xor     bx,bx
        test    cl,1
        jz      @@ss0
        add     bx,0c0h
@@ss0:  test    cl,2
        jz      @@ss1
        add     bx,0c0h
@@ss1:  test    cl,4
        jz      @@ss2
        add     bx,0380h
@@ss2:  add     bx,63
        shr     bx,6
@@ss3:  popm    eax,ecx,edi,es
        jmp     @@Done
        ;
@@StateSave:    ;Save state function.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit58
        movzx   ebx,bx
@@Use32Bit58:   pop     eax
        pushm   eax,ebx,ecx,es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     ebx,ebx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_ECX,ecx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        popm    eax,ebx,ecx,es
        ;
        pushm   eax,ebx,es
        mov     ax,1c00h
        int     10h             ;Get buffer size.
        movzx   ecx,bx
        popm    eax,ebx,es
        shl     ecx,6           ;*64
        mov     edi,ebx         ;destination buffer.
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,fs:w[EPSP_TransProt]
        assume ds:nothing
        cld
        rep     movsb
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@StateRestore: ;Restore state function.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit59
        movzx   ebx,bx
@@Use32Bit59:   pop     eax
        ;
        pushm   eax,ebx,ecx,es
        pushm   eax,ebx,es
        mov     ax,1c00h
        int     10h             ;Get buffer size.
        movzx   ecx,bx
        popm    eax,ebx,es
        shl     ecx,6           ;*64
        mov     esi,ebx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        assume ds:nothing
        pop     ds
        cld
        rep     movsb
        popm    eax,ebx,ecx,es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     ebx,ebx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_ECX,ecx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        ;
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@Done: ;Now update stacked flags.
        ;
        pushm   eax,ebx
        pushf
        pop     ax                      ;get new flags.
        push    ds
        mov     ds,cs:Int10hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Use32Bit8
        mov     bx,sp
        mov     bx,ss:[bx+(4+4)+(2+2)]          ;get origional flags.
        jmp     @@Use16Bit8
@@Use32Bit8:    mov     bx,ss:[esp+(4+4)+(4+4)]         ;get origional flags.
@@Use16Bit8:    and     bx,0000011000000000b            ;retain IF.
        and     ax,1111100111111111b            ;lose IF.
        or      ax,bx                   ;get old IF.
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Use32Bit9
        mov     bx,sp
        mov     ss:[bx+(4+4)+(2+2)],ax          ;modify stack flags.
        jmp     @@Use16Bit9
@@Use32Bit9:    mov     ss:[esp+(4+4)+(4+4)],ax         ;modify stack flags.
@@Use16Bit9:    popm    eax,ebx
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Use32Bit10
        iret
@@Use32Bit10:   ;
        iretd
        ;
@@NotOurs:      ;Not a function recognised by us so pass control to previous handler.
        ;
        push    ds
        mov     ds,cs:Int10hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Use32Bit11
        db 66h
        jmp     cs:f[OldInt10h]         ;pass it onto previous handler.
@@Use32Bit11:   jmp     cs:f[OldInt10h]         ;pass it onto previous handler.
Int10h  endp
;

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bord10  proc    near
        pushm   ax,dx
        mov     ah,al
        mov     dx,3dah
        in      al,dx
        mov     dl,0c0h
        mov     al,11h
        out     dx,al
        mov     al,ah
        out     dx,al
        mov     al,20h
        out     dx,al
        popm    ax,dx
        ret
Bord10  endp


OldInt10h       df 0
Int10hCSeg      dw ?
Int10hDSeg      dw ?
Int10hDDSeg     dw ?
;
Int10hStaticSel dw ?
Int10hUltraFont dw ?
;
        db 4 dup (0)
Int10hEnd       label byte
_Int10h ends
        .286



