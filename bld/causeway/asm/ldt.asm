;-------------------------------------------------------------------------------
;
;Allocate block of descriptor entries.
;
;On Entry:-
;
;CX     - Number of descriptors to allocate.
;
;On Exit:-
;
;AX     - Base selector.
;
RawGetDescriptors proc near
        ;
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT memory addresable.
        mov     es,ax
        mov     edi,MDTLinear+4 ;point to descriptor map.
        movzx   edx,cx
        mov     ecx,8192
        xor     al,al
        cld
        dec     edx
        jnz     ldt1_0
        ;
        ;Search for single selector.
        ;
        repne   scasb           ;find a zero.
        jnz     ldt1_8
        dec     edi
        inc     al
        mov     es:[edi],al
        jmp     ldt1_1
        ;
        ;search for multiple selectors.
        ;
ldt1_0: repne   scasb           ;find a zero.
        jnz     ldt1_8
        cmp     edx,ecx
        jnc     ldt1_8
        push    ecx
        push    edi
        mov     ecx,edx
        repe    scasb           ;check for string of zero's
        pop     edi
        pop     ecx
        jnz     ldt1_0
        dec     edi
        push    edi
        mov     ecx,edx
        inc     ecx
        inc     al
        rep     stosb           ;mark all entries as used.
        pop     edi
ldt1_1: sub     edi,MDTLinear+4
        shl     edi,3
        push    edi
        add     edi,MDTLinear
        xor     eax,eax
        mov     DWORD PTR es:[edi],eax
        mov     DWORD PTR es:[edi+4],(DescPresent+DescMemory+DescRWData+DescPL3) shl 8
        or      edx,edx
        jz      ldt1_2
        mov     ecx,edx
        add     ecx,ecx
        mov     esi,edi
        add     edi,8
        push    ds
        push    es
        pop     ds
        rep     movsd
        pop     ds
        ;
ldt1_2: pop     eax
        or      ax,3+4          ;RPL=3, TI=1
        clc
        jmp     ldt1_9
        ;
ldt1_8: stc
        ;
ldt1_9:
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        ret
        assume ds:_cwDPMIEMU
RawGetDescriptors endp


;-------------------------------------------------------------------------------
;
;Release a previously allocated descriptor entry.
;
;On Entry:-
;
;BX     - Selector for descriptor to release.
;
RawRelDescriptor proc near
        ;
        ;Check segment registers for value we're releasing and clear
        ;if found.
        ;
        push    eax
        push    ebx
        push    ecx
        and     ebx,0ffffh-7
        xor     ecx,ecx
        xor     eax,eax
        mov     ax,ds
        and     eax,not 7
        cmp     eax,ebx
        jnz     ldt2_0
        mov     ds,cx
ldt2_0: mov     ax,es
        and     eax,not 7
        cmp     eax,ebx
        jnz     ldt2_1
        mov     es,cx
ldt2_1: mov     ax,fs
        and     eax,not 7
        cmp     eax,ebx
        jnz     ldt2_2
        mov     fs,cx
ldt2_2: mov     ax,gs
        and     eax,not 7
        cmp     eax,ebx
        jnz     ldt2_3
        mov     gs,cx
ldt2_3:
        pop     ecx
        pop     ebx
        pop     eax
        ;
        push    eax
        push    esi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get selector to use.
        shr     esi,3           ;/8 for descriptor number.
        add     esi,MDTLinear+4
        mov     BYTE PTR es:[esi],0             ;mark this entry as free.
        movzx   esi,bx
        and     esi,not 7
        add     esi,MDTLinear
        xor     eax,eax
        mov     DWORD PTR es:[esi],eax
        mov     DWORD PTR es:[esi+4],eax
        ;
        clc
        pop     es
        pop     ds
        pop     esi
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawRelDescriptor endp


;-------------------------------------------------------------------------------
;
;Create/return a protected mode selector that maps a real mode segment.
;
;On Entry:
;
;BX     - Real mode segment address to map.
;
;On Exit:
;
;AX     - Protected mode selector.
;
;Note: Multiple calls with the same value will nearly always return the same
;selector depending on how many selectors have been allocated with this routine.
;
RawReal2ProtSel proc    near
        ;
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ds
        mov     ax,DpmiEmuDS            ;make our data addresable.
        mov     ds,ax
        movzx   ebx,bx          ;Get the segment value.
        shl     ebx,4           ;convert to linear.
        mov     esi,offset Real2ProtList        ;point to list of selectors.
        mov     ecx,128
ldt3_0: cmp     w[esi],0                ;end of the list?
        jz      ldt3_New
        push    ebx
        push    ecx
        mov     bx,[esi]
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     edx,ecx
        pop     ecx
        pop     ebx
        cmp     edx,ebx         ;same base?
        jz      ldt3_GotOne
        add     esi,2
        dec     ecx
        jnz     ldt3_0
;
;Have to allocate a new descriptor.
;
ldt3_New:
        push    ecx
        mov     cx,1
        call    RawGetDescriptors       ;get a descriptor.
        pop     ecx
        jc      ldt3_9
        push    eax
        push    ebx
        push    ecx
        mov     dx,bx
        shr     ebx,16
        mov     cx,bx
        mov     bx,ax
        call    RawSetSelBase
        pop     ecx
        pop     ebx
        pop     eax
        push    eax
        push    ebx
        push    ecx
        mov     dx,-1
        xor     cx,cx
        mov     bx,ax
        call    RawSetSelLimit
        pop     ecx
        pop     ebx
        pop     eax
        or      ecx,ecx         ;did we reach the end of the list?
        jz      ldt3_Done
        mov     [esi],ax
        jmp     ldt3_Done
;
;Found a previous selector with right base so use that.
;
ldt3_GotOne:
        mov     ax,[esi]
;
;Now exit with succesful value.
;
ldt3_Done:
        clc
ldt3_9:
        pop     ds
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        ret
Real2ProtList   dw 128 dup (0)
RawReal2ProtSel endp


;-------------------------------------------------------------------------------
;
;Set selector type.
;
;On Entry:-
;
;BX     - Selector.
;CL     - Type.
;CH     - Extended type.
;
RawSetSelType   proc    near
        ;
        push    eax
        push    esi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        mov     BYTE PTR es:[esi+5],cl
        and     ch,11110000b
        and     BYTE PTR es:[esi+6],00001111b
        or      BYTE PTR es:[esi+6],ch
        ;
        clc
        pop     es
        pop     ds
        pop     esi
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawSetSelType   endp


;-------------------------------------------------------------------------------
;
;Set selector limit.
;
;On Entry:-
;
;BX     - Selector.
;CX:DX  - Limit.
;
RawSetSelLimit  proc    near
        ;
        push    eax
        push    ecx
        push    esi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get the selector.
        and     si,0ffffh-7             ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        shl     ecx,16
        mov     cx,dx
        xor     al,al
        cmp     ecx,100000h     ; see if we need to set g bit
        jc      ldt5_2
        shr     ecx,12          ; div by 4096
        or      al,80h          ; set g bit
ldt5_2: mov     es:[esi],cx             ;store low word of limit.
        shr     ecx,16
        or      cl,al
        and     BYTE PTR es:[esi+6],01110000b   ;lose limit & G bit.
        or      es:[esi+6],cl           ;store high bits of limit and G bit.
        ;
        clc
        pop     es
        pop     ds
        pop     esi
        pop     ecx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawSetSelLimit  endp


;-------------------------------------------------------------------------------
;
;Set selector linear base address.
;
;On Entry:-
;
;BX     - Selector.
;CX:DX  - Linear base.
;
RawSetSelBase   proc    near
        ;
        push    eax
        push    esi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addressable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addressable.
        mov     es,ax
        movzx   esi,bx          ;Get the selector.
        and     si,0ffffh-7             ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        mov     es:[esi+4],cl           ;base mid.
        mov     es:[esi+7],ch           ;base high.
        mov     es:[esi+2],dx           ;base low.
        clc
        pop     es
        pop     ds
        pop     esi
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawSetSelBase   endp


;-------------------------------------------------------------------------------
;
;Get selector linear base address.
;
;On Entry:-
;
;BX     - Selector.
;
;On Exit:-
;
;CX:DX  - Linear base.
;
RawGetSelBase   proc    near
        ;
        push    eax
        push    esi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        mov     cl,es:[esi+4]           ;base mid.
        mov     ch,es:[esi+7]           ;base high.
        mov     dx,es:[esi+2]           ;base low.
        clc
        pop     es
        pop     ds
        pop     esi
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawGetSelBase   endp


;-------------------------------------------------------------------------------
;
;Copy buffer into descriptor.
;
;On Entry:-
;
;BX     - Selector.
;ES:[E]DI       - Buffer.
;
RawBPutDescriptor proc near
        ;
        push    eax
        push    ecx
        push    esi
        push    edi
        push    ds
        push    es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        test    BYTE PTR RawSystemFlags,1
        jz      ldt8_Use32
        movzx   edi,di
ldt8_Use32:
        mov     ax,KernalZero   ;make LDT addresable.
        mov     ds,ax
        push    esi
        push    ds
        push    edi
        push    es
        pop     ds
        pop     esi
        pop     es
        pop     edi
        cld
        movsd
        movsd
        ;
        clc
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ecx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RawBPutDescriptor endp


;-------------------------------------------------------------------------------
;
;Copy descriptor into buffer.
;
;On Entry:-
;
;BX     - Selector.
;ES:[E]DI       - Buffer.
;
RawBGetDescriptor proc near
        ;
        push    eax
        push    ecx
        push    esi
        push    edi
        push    ds
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        test    BYTE PTR RawSystemFlags,1
        jz      ldt9_Use32
        movzx   edi,di
ldt9_Use32:
        mov     ax,KernalZero   ;make LDT addresable.
        mov     ds,ax
        cld
        movsd
        movsd
        ;
        clc
        pop     ds
        pop     edi
        pop     esi
        pop     ecx
        pop     eax
        ret
RawBGetDescriptor endp
