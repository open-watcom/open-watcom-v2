;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
        .386P
_Int10h segment para private 'extension code' use32
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
        jc      int101_9
        mov     Int10hStaticSel,bx
        sys     GetSel
        jc      int101_9
        mov     Int10hUltraFont,bx
        ;
        mov     bl,10h
        sys     GetVect
        test    BYTE PTR es:SystemFlags,1
        jz      int101_Use32
        mov     w[OldInt10h],dx
        mov     w[OldInt10h+2],cx
        jmp     int101_Use0
int101_Use32:
        mov     d[OldInt10h],edx
        mov     w[OldInt10h+4],cx
int101_Use0:
        mov     edx,offset Int10h
        mov     cx,cs
        mov     bl,10h
        sys     SetVect
        assume es:nothing
        assume ds:nothing
        clc
int101_9:
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
        jz      int102_9
        mov     ds,Int10hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int10hDDSeg
        assume ds:_Int10h
        jz      int102_Use32
        movzx   edx,w[OldInt10h]
        mov     cx,w[OldInt10h+2]
        jmp     int102_Use0
int102_Use32:
        mov     edx,d[OldInt10h]
        mov     cx,w[OldInt10h+4]
int102_Use0:
        mov     bl,10h
        sys     SetVect
        assume ds:nothing
int102_9:
        pop     ds
        ;
        db 66h
        retf
Int10hClose     endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int10h  proc    far
        cmp     ax,1002h                ;set pen numbers?
        jz      int103_SetPens
        cmp     ax,1009h                ;get pen numbers?
        jz      int103_GetPens
        cmp     ax,1012h                ;set colour values?
        jz      int103_SetColours
        cmp     ax,1017h                ;get colour values?
        jz      int103_GetColours
        cmp     ax,1100h                ;Set font?
        jz      int103_SetFont1
        cmp     ax,1110h                ;Set font and program CRTC?
        jz      int103_SetFont1
        cmp     ax,1130h                ;Get font details?
        jz      int103_GetFont
        cmp     ah,13h          ;write teletype string?
        jz      int103_WriteString
        cmp     ah,1bh          ;state info?
        jz      int103_VideoState
        cmp     ax,1c00h
        jz      int103_StateSize
        cmp     ax,1c01h                ;state save?
        jz      int103_StateSave
        cmp     ax,1c02h                ;state restore?
        jz      int103_StateRestore
        cmp     ax,0cd00h               ;Ultra set palette?
        jz      int103_UltraSetPal
        cmp     ax,0cd02h               ;Ultra get palette?
        jz      int103_UltraGetPal
        cmp     ax,0cd10h               ;Ultra set font?
        jz      int103_UltraSetFont
        jmp     int103_NotOurs
        ;
int103_GetFont:
        ;Get font details?
        ;
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ds
        push    fs
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        mov     bx,es:[edi].RealRegsStruc.Real_ES
        mov     ax,0002h
        int     31h
        jc      int103_9
        mov     ebp,es:[edi].RealRegsStruc.Real_EBP
        movzx   ebp,bp
        mov     edx,es:[edi].RealRegsStruc.Real_EDX
        mov     ecx,es:[edi].RealRegsStruc.Real_ECX
        mov     es,ax
int103_9:
        pop     fs
        pop     ds
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_UltraSetFont:
        ;UltraVision set font.
        ;
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit503
        movzx   ebp,bp
int103_Use32Bit503:
        xor     eax,eax
        mov     ax,ds
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit504
        movzx   esi,si
int103_Use32Bit504:
        pop     eax
        push    ax
        push    bx
        push    cx
        push    dx
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        mov     ecx,8
        cld
        rep     movsb           ;copy into transfer buffer.
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        push    ax
        push    bx
        push    cx
        push    dx
        movzx   ax,bh
        mul     cx
        movzx   ecx,ax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        mov     edi,8
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        rep     movsb           ;copy into transfer buffer.
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     es:[edi].RealRegsStruc.Real_ESI,0
        mov     es:[edi].RealRegsStruc.Real_EBP,8
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_DS,ax
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        mov     eax,es:[edi].RealRegsStruc.Real_EAX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jmp     int103_Done
        ;
int103_UltraGetPal:
        ;UltraVision get palette.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edi
        push    ebp
        push    es
        push    fs
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        mov     ds,cs:Int10hDDseg
        assume ds:_Int10h
        mov     bx,Int10hUltraFont
        assume ds:nothing
        movzx   edx,es:[edi].RealRegsStruc.Real_DS
        shl     edx,4
        mov     ecx,65535
        sys     SetSelDet32
        mov     ds,bx
        mov     edx,es:[edi].RealRegsStruc.Real_EDX
        mov     esi,es:[edi].RealRegsStruc.Real_ESI
        movzx   esi,si
        movzx   edx,dx
        pop     fs
        pop     es
        pop     ebp
        pop     edi
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_UltraSetPal:
        ;UltraVision set palette.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,ds
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit502
        movzx   edx,dx
int103_Use32Bit502:
        pop     eax
        mov     esi,edx         ;source buffer.
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        mov     ecx,16
        rep     movsb           ;copy into transfer buffer.
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_DS,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_VideoState:
        ;Some sort of video state table function.
        ;
        or      bx,bx
        jnz     int103_NotOurs
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit501
        movzx   edi,di
int103_Use32Bit501:
        pop     eax
        push    edi
        push    es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        mov     eax,es:[edi].RealRegsStruc.Real_EAX
        pop     es
        pop     edi
        cmp     al,1bh
        jz      int103_ok100
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        xor     eax,eax
        jmp     int103_Done
int103_ok100:
        push    eax
        push    edi
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume ds:nothing
        mov     ecx,64
        cld
        rep     movsb           ;copy the results.
        pop     edi
        movzx   edx,WORD PTR es:[edi+2]
        shl     edx,4
        mov     ecx,65535
        mov     ds,cs:Int10hDDseg
        assume ds:_Int10h
        mov     bx,Int10hStaticSel
        assume ds:nothing
        mov     WORD PTR es:[edi+2],bx
        sys     SetSelDet32
        pop     eax
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jmp     int103_Done
        ;
int103_SetFont1:
        ;Load user font into character generator ram.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit500
        movzx   ebp,bp
int103_Use32Bit500:
        pop     eax
        push    ax
        push    bx
        push    cx
        push    dx
        movzx   ax,bh
        mul     cx
        movzx   ecx,ax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        rep     movsb           ;copy into transfer buffer.
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     es:[edi].RealRegsStruc.Real_EBP,0
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_SetPens:
        ;ES:DX - List of 17 bytes to send to video.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit50
        movzx   edx,dx
int103_Use32Bit50:
        pop     eax
        mov     esi,edx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        mov     ecx,17
        rep     movsb           ;copy into transfer buffer.
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,1002h
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_GetPens:
        ;ES:DX - Space for list of 17 pen numbers.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit51
        movzx   edx,dx
int103_Use32Bit51:
        pop     eax
        push    edx
        push    es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,1009h
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        pop     es
        pop     edi
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume ds:nothing
        mov     ecx,17
        cld
        rep     movsb           ;copy the results.
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_SetColours:
        ;ES:DX - list of RGB values.
        ;CX    - number of values.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit52
        movzx   edx,dx
int103_Use32Bit52:
        pop     eax
        mov     esi,edx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
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
        mov     es:[edi].RealRegsStruc.Real_EAX,1012h
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_GetColours:
        ;ES:DX - Buffer for list of RGB values.
        ;CX    - Number of values.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit53
        movzx   edx,dx
int103_Use32Bit53:
        pop     eax
        push    ecx
        push    edx
        push    es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,1017h
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX           ;read pen values.
        pop     es
        pop     edi
        pop     ecx
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume ds:nothing
        movzx   ecx,cx
        mov     eax,ecx
        add     ecx,ecx
        add     ecx,eax         ;3 bytes per entry.
        cld
        rep     movsb           ;copy the results.
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_WriteString:
        ;ES:BP - String to write.
        ;CX    - Number of characters to write.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit54
        movzx   ebp,bp
int103_Use32Bit54:
        pop     eax
        mov     esi,ebp         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        push    cx
        cmp     al,2
        jc      int103_noatts
        add     cx,cx
int103_noatts:
        movzx   ecx,cx
        rep     movsb           ;copy into transfer buffer.
        pop     cx
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     ebp,ebp
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
;       mov     es:[edi].RealRegsStruc.Real_EBP,edx
        mov     es:[edi].RealRegsStruc.Real_EBP,ebp

        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_StateSize:
        ;Return state size.
        ;
        push    eax
        push    ecx
        push    edi
        push    es
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     bl,10h
        sys     IntXX
        mov     ebx,es:RealRegsStruc.Real_EBX[edi]
        cmp     bx,2048/64
        jc      int103_ss3
        ;
        xor     bx,bx
        test    cl,1
        jz      int103_ss0
        add     bx,0c0h
int103_ss0:
        test    cl,2
        jz      int103_ss1
        add     bx,0c0h
int103_ss1:
        test    cl,4
        jz      int103_ss2
        add     bx,0380h
int103_ss2:
        add     bx,63
        shr     bx,6
int103_ss3:
        pop     es
        pop     edi
        pop     ecx
        pop     eax
        jmp     int103_Done
        ;
int103_StateSave:
        ;Save state function.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit58
        movzx   ebx,bx
int103_Use32Bit58:
        pop     eax
        push    eax
        push    ebx
        push    ecx
        push    es
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     ebx,ebx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        pop     es
        pop     ecx
        pop     ebx
        pop     eax
        ;
        push    eax
        push    ebx
        push    es
        mov     ax,1c00h
        int     10h             ;Get buffer size.
        movzx   ecx,bx
        pop     es
        pop     ebx
        pop     eax
        shl     ecx,6           ;*64
        mov     edi,ebx         ;destination buffer.
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        xor     esi,esi
        mov     ds,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume ds:nothing
        cld
        rep     movsb
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_StateRestore:
        ;Restore state function.
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int103_Use32Bit59
        movzx   ebx,bx
int103_Use32Bit59:
        pop     eax
        ;
        push    eax
        push    ebx
        push    ecx
        push    es
        push    eax
        push    ebx
        push    es
        mov     ax,1c00h
        int     10h             ;Get buffer size.
        movzx   ecx,bx
        pop     es
        pop     ebx
        pop     eax
        shl     ecx,6           ;*64
        mov     esi,ebx         ;source buffer.
        push    es
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume ds:nothing
        pop     ds
        cld
        rep     movsb
        pop     es
        pop     ecx
        pop     ebx
        pop     eax
        ;
        mov     edi,offset Int10Buffer
        mov     es,cs:Int10hDseg
        assume es:_cwMain
        xor     ebx,ebx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,10h
        sys     IntXX
        ;
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int103_Done
        ;
int103_Done:
        ;Now update stacked flags.
        ;
        push    eax
        push    ebx
        pushf
        pop     ax                      ;get new flags.
        push    ds
        mov     ds,cs:Int10hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int103_Use32Bit8
        mov     bx,sp
        mov     bx,ss:[bx+(4+4)+(2+2)]          ;get original flags.
        jmp     int103_Use16Bit8
int103_Use32Bit8:
        mov     bx,ss:[esp+(4+4)+(4+4)]         ;get original flags.
int103_Use16Bit8:
        and     bx,0000011000000000b            ;retain IF.
        and     ax,1111100111111111b            ;lose IF.
        or      ax,bx                   ;get old IF.
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int103_Use32Bit9
        mov     bx,sp
        mov     ss:[bx+(4+4)+(2+2)],ax          ;modify stack flags.
        jmp     int103_Use16Bit9
int103_Use32Bit9:
        mov     ss:[esp+(4+4)+(4+4)],ax         ;modify stack flags.
int103_Use16Bit9:
        pop     ebx
        pop     eax
        push    ds
        mov     ds,cs:Int10hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int103_Use32Bit10
        iret
        ;
int103_Use32Bit10:
        iretd
        ;
int103_NotOurs:
        ;Not a function recognised by us so pass control to previous handler.
        ;
        push    ds
        mov     ds,cs:Int10hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int103_Use32Bit11
        db 66h
        jmp     FWORD PTR cs:[OldInt10h]                ;pass it onto previous handler.
int103_Use32Bit11:
        jmp     FWORD PTR cs:[OldInt10h]                ;pass it onto previous handler.
Int10h  endp
;


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



