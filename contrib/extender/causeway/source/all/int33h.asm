;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
        .386
_Int33h segment para public 'extension code' use32
        assume cs:_Int33h, ds:nothing, es:nothing
Int33hStart     label byte


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int33hOpen      proc    near
;
;Setup int 33h patch.
;
        assume ds:_Int33h
        assume es:_cwMain
        mov     Int33hDSeg,es   ;Store cwCode selector.
        mov     Int33hCSeg,cs   ;store this segment.
        mov     Int33hDDSeg,ds
        test    es:SystemFlags,1
        jz      @@32bit
        mov     eax,offset Int33hDummy
        mov     w[Int33hUserCode],ax
        mov     w[Int33hUserCode+2],cs
        jmp     @@0bit
@@32bit:        mov     d[Int33hUserCode],offset Int33hDummy
        mov     w[Int33hUserCode+4],cs
@@0bit: ;
        ;Check mouse driver present.
        ;
;       mov     ax,0
;       int     33h
;       cmp     ax,0            ;mouse driver present?
;       jz      @@9
        ;
        ;Get call back.
        ;
        pushm   ds,es,ds
        pop     es
        mov     esi,offset MouseEvent
        mov     edi,offset MouseEventTab
        push    cs
        pop     ds
        sys     GetCallBack
        popm    ds,es
        jc      @@9
        mov     w[Int33hCallBack],dx
        mov     w[Int33hCallBack+2],cx
        ;
        ;Set patch vector.
        ;
        mov     bl,33h
        sys     GetVect
        test    es:SystemFlags,1
        jz      @@Use32
        mov     w[OldInt33h],dx
        mov     w[OldInt33h+2],cx
        jmp     @@Use0
@@Use32:        mov     d[OldInt33h],edx
        mov     w[OldInt33h+4],cx
@@Use0: mov     edx,offset Int33h
        mov     cx,cs
        mov     bl,33h
        sys     SetVect
        ;
        assume es:nothing
        assume ds:nothing
@@9:    clc
        ;
        db 66h
        retf
Int33hOpen      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int33hClose     proc    near
;
;Remove int 33h patch.
;
        push    ds
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        ;
        ;Release CallBack.
        ;
        cmp     d[Int33hCallBack],0
        jz      @@8
;       mov     ax,0
;       int     33h
        mov     dx,w[Int33hCallBack]
        mov     cx,w[Int33hCallBack+2]
        sys     RelCallBack
        ;
@@8:    ;Release interupt vector.
        ;
        cmp     d[OldInt33h],0
        jz      @@9
        mov     ds,Int33hDseg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        jz      @@Use32
        movzx   edx,w[OldInt33h]
        mov     cx,w[OldInt33h+2]
        jmp     @@Use0
@@Use32:        mov     edx,d[OldInt33h]
        mov     cx,w[OldInt33h+4]
@@Use0: mov     bl,33h
        sys     SetVect
        assume ds:nothing
@@9:    pop     ds
        ;
        db 66h
        retf
Int33hClose     endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int33h  proc    far
        cmp     ax,0            ;Reset?
        jz      @@Reset
        cmp     ax,9            ;Set pointer shape?
        jz      @@SetCursor
        cmp     ax,0ch          ;Set event handler?
        jz      @@SetEvent
        cmp     ax,14h          ;Swap event handlers?
        jz      @@SwapEvent
        cmp     ax,15h          ;Get state buffer size?
        jz      @@GetStateSize
        cmp     ax,16h          ;Save state?
        jz      @@SaveState
        cmp     ax,17h          ;Restore state?
        jz      @@ResState
        jmp     @@NotOurs
        ;
@@SwapEvent:    ;Swap event handlers.
        ;
        pushm   eax,ebx,esi,edi,ebp,ds
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        cmp     Int33hUserOK,0  ;handler installed?
        jnz     @@se0
        ;
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      @@se1
        xchg    dx,w[Int33hUserCode]
        mov     ax,w[Int33hUserCode+2]
        mov     w[Int33hUserCode+2],es
        mov     es,ax
        jmp     @@se2
@@se1:  xchg    edx,d[Int33hUserCode]
        mov     ax,w[Int33hUserCode+4]
        mov     w[Int33hUserCode+4],es
        mov     es,ax
@@se2:  xchg    cx,w[Int33hUserMask]
        pushm   ecx,edx,es
        mov     cx,w[Int33hUserMask]
        mov     es,Int33hDSeg
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:Real_EAX[edi],0ch
        mov     es:Real_ECX[edi],ecx
        mov     dx,w[Int33hCallBack]
        mov     es:Real_EDX[edi],edx
        mov     cx,w[Int33hCallBack+2]
        mov     es:Real_ES[edi],cx
        mov     bl,33h
        sys     IntXX
        mov     Int33hUserOK,1  ;enable user routine.
        assume es:nothing
        popm    ecx,edx,es
        jmp     @@se9
        ;
@@se0:  push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      @@se3
        xchg    dx,w[Int33hUserCode]
        mov     ax,w[Int33hUserCode+2]
        mov     w[Int33hUserCode+2],es
        mov     bx,es
        mov     es,ax
        mov     ax,cs
        cmp     ax,bx
        jnz     @@se4
        mov     Int33hUserOK,0
        push    eax
        mov     eax,offset Int33hDummy
        mov     w[Int33hUserCode],ax
        pop     eax
        mov     w[Int33hUserCode+2],cs
        jmp     @@se5
@@se3:  xchg    edx,d[Int33hUserCode]
        mov     ax,w[Int33hUserCode+4]
        mov     w[Int33hUserCode+4],es
        mov     bx,es
        mov     es,ax
        mov     ax,cs
        cmp     ax,bx
        jnz     @@se4
        mov     Int33hUserOK,0
        mov     d[Int33hUserCode],offset Int33hDummy
        mov     w[Int33hUserCode+4],cs
@@se5:  pushm   ecx,edx,es
        mov     es,Int33hDSeg
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:Real_EAX[edi],0
        mov     bl,33h
        sys     IntXX
        assume es:nothing
        popm    ecx,edx,es
        ;
@@se4:  xchg    cx,w[Int33hUserMask]
        ;
@@se9:  popm    eax,ebx,esi,edi,ebp,ds
        assume ds:nothing
        jmp     @@Done
        ;
@@Reset:        ;Remove's event handler among other things.
        ;
        push    ds
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     Int33hUserOK,0
        mov     d[Int33hUserCode],offset Int33hDummy
        push    ds
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      @@r32
        mov     w[Int33hUserCode+2],cs
        jmp     @@r0
@@r32:  mov     w[Int33hUserCode+4],cs
@@r0:   mov     w[Int33hUserMask],0
        assume ds:nothing
        pop     ds
        jmp     @@NotOurs               ;carry onto real mode handler.
        ;
@@SetCursor:    ;Set graphics cursor shape.
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
        mov     ds,cs:Int33hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        pop     ds
        assume ds:nothing
        cld
        push    ecx
        mov     ecx,64
        rep     movsb           ;copy into transfer buffer.
        pop     ecx
        mov     edi,offset Int33Buffer
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        mov     es:[edi].Real_EBX,ebx
        mov     es:[edi].Real_ECX,ecx
        assume es:nothing
        mov     bl,33h
        sys     IntXX
        popm    ds,es,fs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@SetEvent:     ;Set event handler.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     Int33hUserOK,0          ;disable old handler.
        jz      @@Use32Bit51
        mov     w[Int33hUserCode],dx
        mov     w[Int33hUserCode+2],es
        jmp     @@Use16Bit51
@@Use32Bit51:   mov     d[Int33hUserCode],edx
        mov     w[Int33hUserCode+4],es
@@Use16Bit51:   mov     w[Int33hUserMask],cx
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        mov     es,cs:Int33hDSeg
        assume ds:_Int33h
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:Real_EAX[edi],eax
        mov     es:Real_ECX[edi],ecx
        mov     dx,w[Int33hCallBack]
        mov     es:Real_EDX[edi],edx
        mov     cx,w[Int33hCallBack+2]
        mov     es:Real_ES[edi],cx
        mov     bl,33h
        sys     IntXX
        mov     Int33hUserOK,1  ;enable user routine.
        assume ds:nothing
        assume es:nothing
        popm    ds,es
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        jmp     @@Done
        ;
@@GetStateSize: ; Get state buffer size.
        ;
        pushm   eax,ecx,edx,esi,edi,ebp,ds,es
        mov     es,cs:Int33hDSeg
        mov     edi,offset Int33Buffer
        mov     es:Real_EAX[edi],eax
        mov     bl,33h
        sys     IntXX
        mov     ebx,es:Real_EBX[edi]
        popm    eax,ecx,edx,esi,edi,ebp,ds,es
        add     ebx,8+2         ;leave space for our stuff.
        jmp     @@Done
        ;
@@SaveState:    ;Preserve mouse driver state.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        ;
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit58
        movzx   edx,dx
@@Use32Bit58:   pop     eax
        ;
        pushm   edx,es
        mov     edi,offset Int33Buffer
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,33h
        sys     IntXX
        mov     es:Real_EAX[edi],15h
        mov     bl,33h
        sys     IntXX
        mov     ecx,es:Real_EBX[edi]
        movzx   ecx,cx
        popm    edi,es
        ;
        mov     ds,cs:Int33hDDseg
        assume ds:_Int33h
        mov     eax,d[Int33hUserCode]
        mov     es:[edi],eax
        add     edi,4
        mov     ax,w[Int33hUserCode+4]
        mov     es:[edi],ax
        add     edi,2
        mov     ax,w[Int33hUserOK]
        mov     es:[edi],ax
        add     edi,2
        mov     ax,w[Int33hUserMask]
        mov     es:[edi],ax
        add     edi,2
        assume ds:nothing
        ;
        mov     ds,cs:Int33hDseg
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
@@ResState:     ;Restore mouse driver state.
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@Use32Bit59
        movzx   edx,dx
@@Use32Bit59:   pop     eax
        ;
        pushm   eax,edx,es
        pushm   eax,edx,es
        mov     es,cs:Int33hDseg
        mov     edi,offset Int33Buffer
        mov     es:Real_EAX[edi],15h
        mov     bl,33h
        sys     IntXX
        mov     ecx,es:Real_EBX[edi]
        movzx   ecx,cx
        popm    eax,esi,ds
        ;
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edi,edi
        mov     es,fs:w[EPSP_TransProt]
        assume es:nothing
        ;
        pushm   ds,es
        mov     es,cs:Int33hDDSeg
        assume es:_Int33h
        mov     eax,[esi]
        mov     es:d[Int33hUserCode],eax
        add     esi,4
        mov     ax,[esi]
        mov     es:w[Int33hUserCode+4],ax
        add     esi,2
        mov     ax,[esi]
        mov     es:w[Int33hUserOK],ax
        add     esi,2
        mov     ax,[esi]
        mov     es:w[Int33hUserMask],ax
        add     esi,2
        assume es:nothing
        popm    ds,es
        cld
        rep     movsb
        ;
        popm    eax,edx,es
        mov     edi,offset Int33Buffer
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].Real_EAX,eax
        mov     es:[edi].Real_EDX,edx
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:[edi].Real_ES,ax
        assume es:nothing
        mov     bl,33h
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
        mov     ds,cs:Int33hDSeg
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
        mov     ds,cs:Int33hDseg
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
        mov     ds,cs:Int33hDseg
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
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Use32Bit11
        db 66h
        jmp     cs:f[OldInt33h]         ;pass it onto previous handler.
@@Use32Bit11:   jmp     cs:f[OldInt33h]         ;pass it onto previous handler.
Int33h  endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MouseEvent      proc    far
        push    ax
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@start32
        ;
        mov     ax,[si]         ;get stacked offset.
        mov     es:Real_IP[di],ax
        mov     ax,2[si]
        mov     es:Real_CS[di],ax
        add     es:Real_SP[di],4
        jmp     @@start0
        ;
@@start32:
;       mov     ax,[esi]                ;get stacked offset.
        mov     ax,[si]         ;get stacked offset., MED 01/24/95

        mov     es:Real_IP[edi],ax
;       mov     ax,2[esi]
        mov     ax,2[si]        ; MED 01/24/95

        mov     es:Real_CS[edi],ax
        add     es:Real_SP[edi],4
        ;
@@start0:       pop     ax
        pushm   ds,es,fs,gs
        pushad
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        ;
        cmp     w[MouseEventStack+4],0
        jz      @@ok
        mov     al,1
        call    bord33
@@shit: jmp     @@shit
        mov     ax,-1
        mov     es,ax
        jmp     @@exit
        ;
@@ok:   cmp     w[Int33hUserOK],0
        jz      @@exit
        ;
        mov     d[MouseEventStack],esp
        mov     w[MouseEventStack+4],ss
        mov     ax,ds
        mov     ss,ax
        mov     esp,offset MouseEventStack
        push    ds
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      @@its32
        movzx   edi,di
@@its32:        mov     eax,es:Real_EAX[edi]
        movsx   eax,ax
        mov     ebx,es:Real_EBX[edi]
        movsx   ebx,bx
        mov     ecx,es:Real_ECX[edi]
        movsx   ecx,cx
        mov     edx,es:Real_EDX[edi]
        movsx   edx,dx
        mov     esi,es:Real_ESI[edi]
        movsx   esi,si
        mov     edi,es:Real_EDI[edi]
        movsx   edi,di
        ;
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        jz      @@Use32Bit
        db 66h
        call    cs:f[Int33hUserCode]
        jmp     @@OldStack
@@Use32Bit:     call    cs:f[Int33hUserCode]
@@OldStack:     ;
        lss     esp,cs:f[MouseEventStack]
        push    ds
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     w[MouseEventStack+4],0
        assume ds:nothing
        pop     ds
@@Exit: ;
        popad
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        popm    ds,es,fs,gs
        jz      @@Use32Bit2
        iret
@@Use32Bit2:    ;
        iretd
MouseEvent      endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int33hDummy     proc    near
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@32bit
        db 66h
        retf
@@32Bit:        ;
        retf
Int33hDummy     endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bord33  proc    near
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
Bord33  endp


MouseEventTab   db size RealRegsStruc dup (0)
        db 1024 dup (0)
MouseEventStack df 0
;
OldInt33h       df 0
Int33hCSeg      dw ?
Int33hDSeg      dw ?
Int33hDDSeg     dw ?
;
Int33hCallBack  dd 0
Int33hUserCode  df 0
Int33hUserMask  dw 0
Int33hUserOK    dw 0
;
        db 4 dup (0)
Int33hEnd       label byte
_Int33h ends
        .286




