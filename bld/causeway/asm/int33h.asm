;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
        .386P
_Int33h segment para private 'extension code' use32
        assume cs:_Int33h, ds:nothing, es:nothing
Int33hStart     label byte

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
        test    BYTE PTR es:SystemFlags,1
        jz      int331_32bit
        mov     eax,offset Int33hDummy
        mov     w[Int33hUserCode],ax
        mov     w[Int33hUserCode+2],cs
        jmp     int331_0bit
int331_32bit:
        mov     d[Int33hUserCode],offset Int33hDummy
        mov     w[Int33hUserCode+4],cs
int331_0bit:
        ;
        ;Check mouse driver present.
        ;
;       mov     ax,0
;       int     33h
;       cmp     ax,0            ;mouse driver present?
;       jz      @@9
        ;
        ;Get call back.
        ;
        push    ds
        push    es
        push    ds
        pop     es
        mov     esi,offset MouseEvent
        mov     edi,offset MouseEventTab
        push    cs
        pop     ds
        sys     GetCallBack
        pop     es
        pop     ds
        jc      int331_9
        mov     w[Int33hCallBack],dx
        mov     w[Int33hCallBack+2],cx
        ;
        ;Set patch vector.
        ;
        mov     bl,33h
        sys     GetVect
        test    BYTE PTR es:SystemFlags,1
        jz      int331_Use32
        mov     w[OldInt33h],dx
        mov     w[OldInt33h+2],cx
        jmp     int331_Use0
int331_Use32:
        mov     d[OldInt33h],edx
        mov     w[OldInt33h+4],cx
int331_Use0:
        mov     edx,offset Int33h
        mov     cx,cs
        mov     bl,33h
        sys     SetVect
        ;
        assume es:nothing
        assume ds:nothing
int331_9:
        clc
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
        jz      int332_8
;       mov     ax,0
;       int     33h
        mov     dx,w[Int33hCallBack]
        mov     cx,w[Int33hCallBack+2]
        sys     RelCallBack
        ;
int332_8:
        ;Release interupt vector.
        ;
        cmp     d[OldInt33h],0
        jz      int332_9
        mov     ds,Int33hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        jz      int332_Use32
        movzx   edx,w[OldInt33h]
        mov     cx,w[OldInt33h+2]
        jmp     int332_Use0
int332_Use32:
        mov     edx,d[OldInt33h]
        mov     cx,w[OldInt33h+4]
int332_Use0:
        mov     bl,33h
        sys     SetVect
        assume ds:nothing
int332_9:
        pop     ds
        ;
        db 66h
        retf
Int33hClose     endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int33h  proc    far
        cmp     ax,0            ;Reset?
        jz      int333_Reset
        cmp     ax,9            ;Set pointer shape?
        jz      int333_SetCursor
        cmp     ax,0ch          ;Set event handler?
        jz      int333_SetEvent
        cmp     ax,14h          ;Swap event handlers?
        jz      int333_SwapEvent
        cmp     ax,15h          ;Get state buffer size?
        jz      int333_GetStateSize
        cmp     ax,16h          ;Save state?
        jz      int333_SaveState
        cmp     ax,17h          ;Restore state?
        jz      int333_ResState
        jmp     int333_NotOurs
        ;
int333_SwapEvent:
        ;Swap event handlers.
        ;
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        cmp     Int33hUserOK,0  ;handler installed?
        jnz     int333_se0
        ;
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      int333_se1
        xchg    dx,w[Int33hUserCode]
        mov     ax,w[Int33hUserCode+2]
        mov     w[Int33hUserCode+2],es
        mov     es,ax
        jmp     int333_se2
int333_se1:
        xchg    edx,d[Int33hUserCode]
        mov     ax,w[Int33hUserCode+4]
        mov     w[Int33hUserCode+4],es
        mov     es,ax
int333_se2:
        xchg    cx,w[Int33hUserMask]
        push    ecx
        push    edx
        push    es
        mov     cx,w[Int33hUserMask]
        mov     es,Int33hDSeg
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:RealRegsStruc.Real_EAX[edi],0ch
        mov     es:RealRegsStruc.Real_ECX[edi],ecx
        mov     dx,w[Int33hCallBack]
        mov     es:RealRegsStruc.Real_EDX[edi],edx
        mov     cx,w[Int33hCallBack+2]
        mov     es:RealRegsStruc.Real_ES[edi],cx
        mov     bl,33h
        sys     IntXX
        mov     Int33hUserOK,1  ;enable user routine.
        assume es:nothing
        pop     es
        pop     edx
        pop     ecx
        jmp     int333_se9
        ;
int333_se0:
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      int333_se3
        xchg    dx,w[Int33hUserCode]
        mov     ax,w[Int33hUserCode+2]
        mov     w[Int33hUserCode+2],es
        mov     bx,es
        mov     es,ax
        mov     ax,cs
        cmp     ax,bx
        jnz     int333_se4
        mov     Int33hUserOK,0
        push    eax
        mov     eax,offset Int33hDummy
        mov     w[Int33hUserCode],ax
        pop     eax
        mov     w[Int33hUserCode+2],cs
        jmp     int333_se5
int333_se3:
        xchg    edx,d[Int33hUserCode]
        mov     ax,w[Int33hUserCode+4]
        mov     w[Int33hUserCode+4],es
        mov     bx,es
        mov     es,ax
        mov     ax,cs
        cmp     ax,bx
        jnz     int333_se4
        mov     Int33hUserOK,0
        mov     d[Int33hUserCode],offset Int33hDummy
        mov     w[Int33hUserCode+4],cs
int333_se5:
        push    ecx
        push    edx
        push    es
        mov     es,Int33hDSeg
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:RealRegsStruc.Real_EAX[edi],0
        mov     bl,33h
        sys     IntXX
        assume es:nothing
        pop     es
        pop     edx
        pop     ecx
        ;
int333_se4:
        xchg    cx,w[Int33hUserMask]
        ;
int333_se9:
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        assume ds:nothing
        jmp     int333_Done
        ;
int333_Reset:
        ;Remove's event handler among other things.
        ;
        push    ds
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     Int33hUserOK,0
        mov     d[Int33hUserCode],offset Int33hDummy
        push    ds
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      int333_r32
        mov     w[Int33hUserCode+2],cs
        jmp     int333_r0
int333_r32:
        mov     w[Int33hUserCode+4],cs
int333_r0:
        mov     w[Int33hUserMask],0
        assume ds:nothing
        pop     ds
        jmp     int333_NotOurs          ;carry onto real mode handler.
        ;
int333_SetCursor:
        ;Set graphics cursor shape.
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
        jnc     int333_Use32Bit50
        movzx   edx,dx
int333_Use32Bit50:
        pop     eax
        mov     esi,edx         ;source buffer.
        push    es
        mov     ds,cs:Int33hDseg
        assume ds:_cwMain
        mov     fs,PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
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
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        mov     es:[edi].RealRegsStruc.Real_EBX,ebx
        mov     es:[edi].RealRegsStruc.Real_ECX,ecx
        assume es:nothing
        mov     bl,33h
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
        jmp     int333_Done
        ;
int333_SetEvent:
        ;Set event handler.
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
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     Int33hUserOK,0          ;disable old handler.
        jz      int333_Use32Bit51
        mov     w[Int33hUserCode],dx
        mov     w[Int33hUserCode+2],es
        jmp     int333_Use16Bit51
int333_Use32Bit51:
        mov     d[Int33hUserCode],edx
        mov     w[Int33hUserCode+4],es
int333_Use16Bit51:
        mov     w[Int33hUserMask],cx
        assume ds:nothing
        mov     ds,cs:Int33hDDSeg
        mov     es,cs:Int33hDSeg
        assume ds:_Int33h
        assume es:_cwMain
        mov     edi,offset Int33Buffer
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     es:RealRegsStruc.Real_ECX[edi],ecx
        mov     dx,w[Int33hCallBack]
        mov     es:RealRegsStruc.Real_EDX[edi],edx
        mov     cx,w[Int33hCallBack+2]
        mov     es:RealRegsStruc.Real_ES[edi],cx
        mov     bl,33h
        sys     IntXX
        mov     Int33hUserOK,1  ;enable user routine.
        assume ds:nothing
        assume es:nothing
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     int333_Done
        ;
int333_GetStateSize:
        ; Get state buffer size.
        ;
        push    eax
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        mov     es,cs:Int33hDSeg
        mov     edi,offset Int33Buffer
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     bl,33h
        sys     IntXX
        mov     ebx,es:RealRegsStruc.Real_EBX[edi]
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        add     ebx,8+2         ;leave space for our stuff.
        jmp     int333_Done
        ;
int333_SaveState:
        ;Preserve mouse driver state.
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
        ;
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     int333_Use32Bit58
        movzx   edx,dx
int333_Use32Bit58:
        pop     eax
        ;
        push    edx
        push    es
        mov     edi,offset Int33Buffer
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,33h
        sys     IntXX
        mov     es:RealRegsStruc.Real_EAX[edi],15h
        mov     bl,33h
        sys     IntXX
        mov     ecx,es:RealRegsStruc.Real_EBX[edi]
        movzx   ecx,cx
        pop     es
        pop     edi
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
        jmp     int333_Done
        ;
int333_ResState:
        ;Restore mouse driver state.
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
        jnc     int333_Use32Bit59
        movzx   edx,dx
int333_Use32Bit59:
        pop     eax
        ;
        push    eax
        push    edx
        push    es
        push    eax
        push    edx
        push    es
        mov     es,cs:Int33hDseg
        mov     edi,offset Int33Buffer
        mov     es:RealRegsStruc.Real_EAX[edi],15h
        mov     bl,33h
        sys     IntXX
        mov     ecx,es:RealRegsStruc.Real_EBX[edi]
        movzx   ecx,cx
        pop     ds
        pop     esi
        pop     eax
        ;
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        mov     fs,es:PSPSegment
        xor     edi,edi
        mov     es,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]
        assume es:nothing
        ;
        push    ds
        push    es
        mov     es,cs:Int33hDDSeg
        assume es:_Int33h
        mov     eax,[esi]
        mov     DWORD PTR es:[Int33hUserCode],eax
        add     esi,4
        mov     ax,[esi]
        mov     WORD PTR es:[Int33hUserCode+4],ax
        add     esi,2
        mov     ax,[esi]
        mov     WORD PTR es:[Int33hUserOK],ax
        add     esi,2
        mov     ax,[esi]
        mov     WORD PTR es:[Int33hUserMask],ax
        add     esi,2
        assume es:nothing
        pop     es
        pop     ds
        cld
        rep     movsb
        ;
        pop     es
        pop     edx
        pop     eax
        mov     edi,offset Int33Buffer
        mov     es,cs:Int33hDseg
        assume es:_cwMain
        xor     edx,edx
        mov     es:[edi].RealRegsStruc.Real_EAX,eax
        mov     es:[edi].RealRegsStruc.Real_EDX,edx
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     es:[edi].RealRegsStruc.Real_ES,ax
        assume es:nothing
        mov     bl,33h
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
        jmp     int333_Done
        ;
int333_Done:
        ;Now update stacked flags.
        ;
        push    eax
        push    ebx
        pushf
        pop     ax                      ;get new flags.
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int333_Use32Bit8
        mov     bx,sp
        mov     bx,ss:[bx+(4+4)+(2+2)]          ;get original flags.
        jmp     int333_Use16Bit8
int333_Use32Bit8:
        mov     bx,ss:[esp+(4+4)+(4+4)]         ;get original flags.
int333_Use16Bit8:
        and     bx,0000011000000000b            ;retain IF.
        and     ax,1111100111111111b            ;lose IF.
        or      ax,bx                   ;get old IF.
        push    ds
        mov     ds,cs:Int33hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int333_Use32Bit9
        mov     bx,sp
        mov     ss:[bx+(4+4)+(2+2)],ax          ;modify stack flags.
        jmp     int333_Use16Bit9
int333_Use32Bit9:
        mov     ss:[esp+(4+4)+(4+4)],ax         ;modify stack flags.
int333_Use16Bit9:
        pop     ebx
        pop     eax
        push    ds
        mov     ds,cs:Int33hDseg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int333_Use32Bit10
        iret
int333_Use32Bit10:
        iretd
        ;
int333_NotOurs:
        ;Not a function recognised by us so pass control to previous handler.
        ;
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int333_Use32Bit11
        db 66h
        jmp     FWORD PTR cs:[OldInt33h]                ;pass it onto previous handler.
int333_Use32Bit11:
        jmp     FWORD PTR cs:[OldInt33h]                ;pass it onto previous handler.
Int33h  endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MouseEvent      proc    far
        push    ax
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int334_start32
        ;
        mov     ax,[si]         ;get stacked offset.
        mov     es:RealRegsStruc.Real_IP[di],ax
        mov     ax,2[si]
        mov     es:RealRegsStruc.Real_CS[di],ax
        add     es:RealRegsStruc.Real_SP[di],4
        jmp     int334_start0
        ;
int334_start32:
;       mov     ax,[esi]                ;get stacked offset.
        mov     ax,[si]         ;get stacked offset., MED 01/24/95

        mov     es:RealRegsStruc.Real_IP[edi],ax
;       mov     ax,2[esi]
        mov     ax,2[si]        ; MED 01/24/95

        mov     es:RealRegsStruc.Real_CS[edi],ax
        add     es:RealRegsStruc.Real_SP[edi],4
        ;
int334_start0:
        pop     ax
        push    ds
        push    es
        push    fs
        push    gs
        pushad
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        ;
        cmp     w[MouseEventStack+4],0
        jz      int334_ok
        mov     al,1
        call    bord33
int334_shit:
        jmp     int334_shit
        mov     ax,-1
        mov     es,ax
        jmp     int334_exit
        ;
int334_ok:
        cmp     w[Int33hUserOK],0
        jz      int334_exit
        ;
        mov     d[MouseEventStack],esp
        mov     w[MouseEventStack+4],ss
        mov     ax,ds
        mov     ss,ax
        mov     esp,offset MouseEventStack
        push    ds
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:_Int33h
        pop     ds
        jz      int334_its32
        movzx   edi,di
int334_its32:
        mov     eax,es:RealRegsStruc.Real_EAX[edi]
        movsx   eax,ax
        mov     ebx,es:RealRegsStruc.Real_EBX[edi]
        movsx   ebx,bx
        mov     ecx,es:RealRegsStruc.Real_ECX[edi]
        movsx   ecx,cx
        mov     edx,es:RealRegsStruc.Real_EDX[edi]
        movsx   edx,dx
        mov     esi,es:RealRegsStruc.Real_ESI[edi]
        movsx   esi,si
        mov     edi,es:RealRegsStruc.Real_EDI[edi]
        movsx   edi,di
        ;
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        jz      int334_Use32Bit
        db 66h
        call    FWORD PTR cs:[Int33hUserCode]
        jmp     int334_OldStack
int334_Use32Bit:
        call    FWORD PTR cs:[Int33hUserCode]
int334_OldStack:
        lss     esp,FWORD PTR cs:[MouseEventStack]
        push    ds
        mov     ds,cs:Int33hDDSeg
        assume ds:_Int33h
        mov     w[MouseEventStack+4],0
        assume ds:nothing
        pop     ds
int334_Exit:
        popad
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     gs
        pop     fs
        pop     es
        pop     ds
        jz      int334_Use32Bit2
        iret
int334_Use32Bit2:
        iretd
MouseEvent      endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int33hDummy     proc    near
        push    ds
        mov     ds,cs:Int33hDSeg
        assume ds:_cwMain
        test    BYTE PTR SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      int335_32bit
        db 66h
        retf
int335_32Bit:
        retf
Int33hDummy     endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bord33  proc    near
        push    ax
        push    dx
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
        pop     dx
        pop     ax
        ret
Bord33  endp


Int33hEnd       label byte
_Int33h ends
        .286
