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
        call    RAWCopyCheck
        ;
        pushm   ebx,ecx,edx,esi,edi,ds,es
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
        jnz     @@0
        ;
        ;Search for single selector.
        ;
        repne   scasb           ;find a zero.
        jnz     @@8
        dec     edi
        inc     al
        mov     es:[edi],al
        jmp     @@1
        ;
        ;search for multiple selectors.
        ;
@@0:    repne   scasb           ;find a zero.
        jnz     @@8
        cmp     edx,ecx
        jnc     @@8
        pushm   ecx,edi
        mov     ecx,edx
        repe    scasb           ;check for string of zero's
        popm    ecx,edi
        jnz     @@0
        dec     edi
        push    edi
        mov     ecx,edx
        inc     ecx
        inc     al
        rep     stosb           ;mark all entries as used.
        pop     edi
@@1:    sub     edi,MDTLinear+4
        shl     edi,3
        push    edi
        add     edi,MDTLinear
        xor     eax,eax
        mov     es:d[edi],eax
        mov     es:d[edi+4],(DescPresent+DescMemory+DescRWData+DescPL3) shl 8
        or      edx,edx
        jz      @@2
        mov     ecx,edx
        add     ecx,ecx
        mov     esi,edi
        add     edi,8
        pushm   ds,es
        pop     ds
        rep     movsd
        pop     ds
        ;
@@2:    pop     eax
        or      ax,3+4          ;RPL=3, TI=1
        clc
        jmp     @@9
        ;
@@8:    stc
        ;
@@9:    popm    ebx,ecx,edx,esi,edi,ds,es
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
        call    RAWCopyCheck
        ;
        ;Check segment registers for value we're releasing and clear
        ;if found.
        ;
        pushm   eax,ebx,ecx
        and     ebx,0ffffh-7
        xor     ecx,ecx
        xor     eax,eax
        mov     ax,ds
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@0
        mov     ds,cx
@@0:    mov     ax,es
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@1
        mov     es,cx
@@1:    mov     ax,fs
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@2
        mov     fs,cx
@@2:    mov     ax,gs
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@3
        mov     gs,cx
@@3:    popm    eax,ebx,ecx
        ;
        pushm   eax,esi,ds,es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get selector to use.
        shr     esi,3           ;/8 for descriptor number.
        add     esi,MDTLinear+4
        mov     es:b[esi],0             ;mark this entry as free.
        movzx   esi,bx
        and     esi,not 7
        add     esi,MDTLinear
        xor     eax,eax
        mov     es:d[esi],eax
        mov     es:d[esi+4],eax
        ;
        clc
        popm    eax,esi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   ebx,ecx,edx,esi,edi,ds
        mov     ax,DpmiEmuDS            ;make our data addresable.
        mov     ds,ax
        movzx   ebx,bx          ;Get the segment value.
        shl     ebx,4           ;convert to linear.
        mov     esi,offset Real2ProtList        ;point to list of selectors.
        mov     ecx,128
@@0:    cmp     w[esi],0                ;end of the list?
        jz      @@New
        pushm   ebx,ecx
        mov     bx,[esi]
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     edx,ecx
        popm    ebx,ecx
        cmp     edx,ebx         ;same base?
        jz      @@GotOne
        add     esi,2
        dec     ecx
        jnz     @@0
;
;Have to allocate a new descriptor.
;
@@New:  push    ecx
        mov     cx,1
        call    RawGetDescriptors       ;get a descriptor.
        pop     ecx
        jc      @@9
        pushm   eax,ebx,ecx
        mov     dx,bx
        shr     ebx,16
        mov     cx,bx
        mov     bx,ax
        call    RawSetSelBase
        popm    eax,ebx,ecx
        pushm   eax,ebx,ecx
        mov     dx,-1
        xor     cx,cx
        mov     bx,ax
        call    RawSetSelLimit
        popm    eax,ebx,ecx
        or      ecx,ecx         ;did we reach the end of the list?
        jz      @@Done
        mov     [esi],ax
        jmp     @@Done
;
;Found a previous selector with right base so use that.
;
@@GotOne:       mov     ax,[esi]
;
;Now exit with succesful value.
;
@@Done: clc
@@9:    popm    ebx,ecx,edx,esi,edi,ds
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
        call    RAWCopyCheck
        ;
        pushm   eax,esi,ds,es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make LDT addresable.
        mov     es,ax
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        mov     es:b[esi+5],cl
        and     ch,11110000b
        and     es:b[esi+6],00001111b
        or      es:b[esi+6],ch
        ;
        clc
        popm    eax,esi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,ecx,esi,ds,es
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
        jc      @@2
        shr     ecx,12          ; div by 4096
        or      al,80h          ; set g bit
@@2:    mov     es:[esi],cx             ;store low word of limit.
        shr     ecx,16
        or      cl,al
        and     es:b[esi+6],01110000b   ;lose limit & G bit.
        or      es:[esi+6],cl           ;store high bits of limit and G bit.
        ;
        clc
        popm    eax,ecx,esi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,esi,ds,es
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
        popm    eax,esi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,esi,ds,es
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
        popm    eax,esi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,ecx,esi,edi,ds,es
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        test    RawSystemFlags,1
        jz      @@Use32
        movzx   edi,di
@@Use32:        mov     ax,KernalZero   ;make LDT addresable.
        mov     ds,ax
        pushm   esi,ds,edi,es   ;swap them round.
        popm    edi,es,esi,ds
        cld
        movsd
        movsd
        ;
        clc
        popm    eax,ecx,esi,edi,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,ecx,esi,edi,ds
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        assume ds:_cwRaw
        movzx   esi,bx          ;Get the selector.
        and     si,0fff8h               ;lose RPL & TI.
        add     esi,MDTLinear   ;offset into descriptor table.
        test    RawSystemFlags,1
        jz      @@Use32
        movzx   edi,di
@@Use32:        mov     ax,KernalZero   ;make LDT addresable.
        mov     ds,ax
        cld
        movsd
        movsd
        ;
        clc
        popm    eax,ecx,esi,edi,ds
        ret
RawBGetDescriptor endp



