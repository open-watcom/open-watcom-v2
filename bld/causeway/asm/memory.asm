;-------------------------------------------------------------------------------
;
;Get memory using RAW/VCPI.
;
;On Entry:-
;
;BX:CX  - Size of block.
;
;On Exit:-
;
;Carry on error else,
;
;SI:DI  - Handle.
;BX:CX  - Logical address of block.
;
RAWGetMemory    proc    near
        ;
        push    eax
        push    edx
        push    ebp
        push    ds
        push    es
        mov     ax,KernalDS             ;make data addresable.
        mov     ds,ax
        assume ds:_cwRaw

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        mov     ax,KernalZero
        mov     es,ax
        shl     ebx,16          ;get block size as 32-bit.
        mov     bx,cx
        add     ebx,4095                ;round up to next 4k.
        shr     ebx,12
        or      ebx,ebx
        jz      mem1_error
mem1_start:
        mov     edi,LinearBase  ;Get starting point.
        mov     ecx,LinearLimit
        sub     ecx,edi         ;Get memory present.
        shr     ecx,12          ;Get pages.
        shr     edi,12          ;Get page number.
        shl     edi,2           ;dword per entry.
        add     edi,1024*4096*1022      ;point to page details.
        ;
        ;Look for a big enough block of free memory.
        ;
        xor     eax,eax         ;value to look for.
        cld

        ;
        ;EAX    - zero, what we're looking for.
        ;ECX    - entries to check.
        ;EDI    - address of entries to check.
        ;
        ;EBX    - number of entries we want.
        ;

mem1_l0:
        repne   scasd

        ;
        ;Nothing found means extend end of memory map.
        ;
        jnz     mem1_ExtendNew

        ;
        ;Point everything back to the matching value.
        ;
        sub     edi,4
        inc     ecx

        ;
        ;Save current position as start of last free block incase
        ;this block extends to the end of memory.
        ;
        mov     esi,edi         ;save last position.

        ;
        ;If we only wanted one page then we're done, this is it!
        ;
        cmp     ebx,1
        jz      mem1_l2         ;Speed up 4K allocations.

        ;
        ;Set the number of pages to scan. This should be the number of
        ;pages we want but has to be reduced to the number of pages
        ;actually available when the request would go off the end of
        ;memory.
        ;
        mov     edx,ebx
        cmp     ecx,ebx         ;Enough space to check?
        jnc     mem1_l1
        mov     edx,ecx

        ;
        ;Scan specified number of entries to see if their all free.
        ;
mem1_l1:
        push    ecx
        push    edi
        mov     ecx,edx
        repe    scasd           ;Scan for free pages.
        pop     edi
        pop     ecx
        jnz     mem1_l3         ;not enough free entries.

        ;
        ;All the entries were free, now check if we were truncating the
        ;length to stop us running off the end of memory.
        ;
        cmp     edx,ebx         ;Truncated length?
        jnz     mem1_ExtendEnd
        jmp     mem1_l2

        ;
        ;One way or another we didn't find the number of entries we were
        ;after so restart the search.
        ;
mem1_l3:
        add     edi,4
        dec     ecx
        jmp     mem1_l0

        ;
        ;We've found what we wanted without any extra messing around so
        ;hand this over to the marker.
        ;
mem1_l2:
        sub     esi,1024*4096*1022      ;point to page details.
        shr     esi,2
        jmp     mem1_MarkMemRet


        ;
        ;Last block is free so use it as the basis for extension.
        ;
mem1_ExtendEnd:
        mov     edi,ecx         ;remaining entries.
        sub     esi,(1024*4096*1022)
        shr     esi,2           ;Get start page number.
        mov     ecx,ebx         ;Total pages needed.
        sub     ecx,edi         ;Pages found so far.
        jmp     mem1_Extend
        ;
mem1_ExtendNew:
        ;Last block not free so new block will start at LinearLimit.
        ;
        mov     ecx,ebx         ;Set pages needed.
        mov     esi,LinearLimit ;New start address.
        shr     esi,12
        ;
mem1_Extend:
        ;Memory map needs extending so get on with it.
        ;
        call    ExtendLinearMemory      ;Try and extend memory map.
        jnc     mem1_MarkMemRet         ;Mark the block as used.
        ;
        ;Not enough memory so report failure.
        ;
        stc
        jmp     mem1_exit
        ;
mem1_MarkMemRet:
        ;Got the memory so mark it in use.
        ;
        sub     FreePages,ebx   ;reduce free page count.
        add     medAllocPages,ebx
        ;
        mov     ecx,ebx         ;Get number of pages again.
        mov     edx,esi
        mov     edi,esi         ;Get base page again.
        shl     edi,2
        add     edi,1024*4096*1022
        mov     eax,MEM_END
        rep     stosd
        mov     DWORD PTR es:[(1024*4096*1022)+edx*4],MEM_START
        ;
        ;Now return details to caller.
        ;
        shl     esi,12          ;Convert back to a real address.
        mov     di,si
        shr     esi,16
        mov     cx,di
        mov     bx,si
        clc
        jmp     mem1_exit
        ;
mem1_error:
        stc
        ;
mem1_exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     es
        pop     ds
        pop     ebp
        pop     edx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RAWGetMemory    endp


;-------------------------------------------------------------------------------
;
;Re-size a block of memory.
;
;On Entry:-
;
;SI:DI  - Handle.
;BX:CX  - New block size.
;
;On Exit:-
;
;SI:DI  - New handle.
;BX:CX  - New address.
;
RAWResMemory    proc    near
        ;
        push    eax
        push    edx
        push    ebp
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        mov     ax,KernalZero
        mov     es,ax
        shl     esi,16
        mov     si,di           ;Get real address.
        shl     ecx,16
        mov     cx,bx
        ror     ecx,16
        add     ecx,4095
        shr     ecx,12          ;Get new block size.
        or      ecx,ecx
        jz      mem2_error
        ;
        ;Check for a valid address.
        ;
        test    esi,4095                ;all memory on page boundaries.
        jnz     mem2_error
        cmp     esi,LinearBase
        jc      mem2_error
        cmp     esi,LinearLimit
        jnc     mem2_error
        shr     esi,12          ;Get page number.
        mov     eax,DWORD PTR es:[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_START
        jnz     mem2_error
        ;
        ;Work out the blocks current size.
        ;
        push    ecx
        mov     ebp,1
        mov     edx,esi
        inc     edx
        mov     ecx,LinearLimit
        shr     ecx,12
        sub     ecx,edx
        jz      mem2_l1
mem2_l0:
        mov     eax,DWORD PTR es:[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_END
        jnz     mem2_l1
        inc     ebp
        inc     edx
        dec     ecx
        jnz     mem2_l0
        ;
mem2_l1:
        ;Shrinking or expanding?
        ;
        pop     ecx
        cmp     ecx,ebp
        jz      mem2_RetNewAddr
        jnc     mem2_Bigger
        ;
mem2_Smaller:
        ;Shrinking the block so get on with it.
        ;
        ;ECX - New size in pages.
        ;EBP - Current size in pages.
        ;ESI - Current start page.
        ;
        mov     edx,esi
        add     edx,ecx         ;move to new block end.
        sub     ebp,ecx         ;get number of pages to lose.
        mov     ecx,ebp
        ;
        add     FreePages,ebp   ;update number of free pages.
        sub     medAllocPages,ebp
        ;
mem2_s0:
        and     DWORD PTR es:[(1024*4096*1022)+edx*4],not MEM_MASK
        or      DWORD PTR es:[(1024*4096*1022)+edx*4],MEM_FREE
        and     DWORD PTR es:[(1024*4096*1023)+edx*4],not (1 shl 6)
        inc     edx
        dec     ecx
        jnz     mem2_s0
        call    EMUCR3Flush
        jmp     mem2_RetNewAddr
        ;
mem2_Bigger:
        ;Want to expand the block so get on with it.
        ;
        ;ECX - New size in pages.
        ;EBP - Current size in pages.
        ;ESI - Current start page.
        ;
mem2_b0:
        mov     edx,esi
        add     edx,ebp         ;move to end of this block.
        mov     ebx,LinearLimit
        shr     ebx,12
        sub     ebx,edx         ;get pages to end of memory.
        or      ebx,ebx
        jz      mem2_Extend
        ;
        ;See if the next block is free.
        ;
        mov     eax,DWORD PTR es:[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE
        jnz     mem2_NewBlock           ;no hope so get a new block.
        ;
        ;Check how big this next block is.
        ;
        mov     edi,ebp         ;use current size as basis.
mem2_b1:
        mov     eax,DWORD PTR es:[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE
        jnz     mem2_NewBlock           ;No hope so get a new block.
        inc     edi
        cmp     edi,ecx         ;Got enough yet?
        jz      mem2_MarkAndRet
        inc     edx
        dec     ebx
        jnz     mem2_b1         ;keep trying.
        ;
        ;Reached the end of the memory map so try extending it.
        ;
        pushad
        sub     ecx,edi         ;pages still needed.
        call    ExtendLinearMemory
        popad
        jc      mem2_error
        jmp     mem2_MarkAndRet
        ;
mem2_Extend:
        ;Need to extend the memory map to provide a block of free memory
        ;after the current block.
        ;
        pushad
        sub     ecx,ebp         ;pages needed.
        call    ExtendLinearMemory
        popad
        jc      mem2_error
        ;
mem2_MarkAndRet:
        ;Mark the new memory as in use and exit.
        ;
        ;ECX - New size in pages.
        ;EBP - Current size in pages.
        ;ESI - Current start page.
        ;
        mov     edx,esi
        add     edx,ebp         ;move to start of new memory.
        sub     ecx,ebp         ;pages to mark.
        ;
        sub     FreePages,ecx   ;update number of free pages.
        add     medAllocPages,ecx
        ;
mem2_mr0:
        and     DWORD PTR es:[(1024*4096*1022)+edx*4],not MEM_MASK
        or      DWORD PTR es:[(1024*4096*1022)+edx*4],MEM_END
        inc     edx
        dec     ecx
        jnz     mem2_mr0
        jmp     mem2_RetNewAddr
        ;
mem2_NewBlock:
        ;Nothing for it but to try and allocate a new block of memory.
        ;
        push    ecx
        push    ebp
        push    esi
        shl     ecx,12
        mov     ebx,ecx
        shr     ebx,16
        call    RawGetMemory
        pushf
        shl     ebx,16
        mov     bx,cx
        popf
        pop     esi
        pop     ebp
        pop     ecx
        jc      mem2_error
        ;
        ;Copy current block to new block.
        ;
        pushad
        mov     ecx,ebp
        shl     ecx,12
        shl     esi,12
        mov     edi,ebx
        push    ds
        push    es
        pop     ds
        shr     ecx,2
        rep     movsd
        pop     ds
        popad
        ;
        ;Release current block.
        ;
        pushad
        shl     esi,12
        mov     di,si
        shr     esi,16
        call    RawRelMemory
        popad
        ;
        ;Use new block in place of original.
        ;
        mov     esi,ebx
        shr     esi,12
        ;
mem2_RetNewAddr:
        ;Return possibly new address/handle to caller.
        ;
        shl     esi,12          ;Get a real address again and
        mov     di,si           ;use it as both the memory
        mov     cx,si           ;address to return and the handle.
        shr     esi,16
        mov     bx,si
        clc
        jmp     mem2_exit
        ;
mem2_error:
        stc
mem2_exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     es
        pop     ds
        pop     ebp
        pop     edx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RAWResMemory    endp


;-------------------------------------------------------------------------------
;
;Release block of memory using RAW/VCPI.
;
;On Entry:-
;
;SI:DI  - handle (Base linear address).
;
RAWRelMemory    proc    near
        ;
        pushad
        push    ds
        push    es
        ;
        shl     esi,16          ;Get block base address.
        mov     si,di
        mov     ax,KernalDS             ;make data addresable.
        mov     ds,ax
        assume ds:_cwRaw

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        mov     ax,KernalZero
        mov     es,ax
        ;
        ;Check for a legal address.
        ;
        test    esi,4095
        jnz     mem3_error
        cmp     esi,LinearBase  ;inside logical memory map?
        jc      mem3_error
        cmp     esi,LinearLimit
        jnc     mem3_error
        shr     esi,12          ;Get page number.
        mov     eax,DWORD PTR es:[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_START
        jnz     mem3_error
        ;
        ;Mark all this blocks pages as free.
        ;
        inc     FreePages
        dec     medAllocPages
        ;
        and     DWORD PTR es:[1024*4096*1022+esi*4],not MEM_MASK
        or      DWORD PTR es:[1024*4096*1022+esi*4],MEM_FREE
        and     DWORD PTR es:[1024*4096*1023+esi*4],not (1 shl 6)
        inc     esi
        mov     ecx,LinearLimit
        shr     ecx,12
        sub     ecx,esi         ;Get pages remaining.
        jz      mem3_1
mem3_0: mov     eax,DWORD PTR es:[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_END
        jnz     mem3_1
        inc     FreePages
        dec     medAllocPages
        and     DWORD PTR es:[1024*4096*1022+esi*4],not MEM_MASK
        or      DWORD PTR es:[1024*4096*1022+esi*4],MEM_FREE
        and     DWORD PTR es:[1024*4096*1023+esi*4],not (1 shl 6)
        inc     esi
        dec     ecx
        jnz     mem3_0
        ;
mem3_1: call    EMUCR3Flush
        ;
        clc
        jmp     mem3_exit
        ;
mem3_error:
        stc
mem3_exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     es
        pop     ds
        popad
        ret
        assume ds:_cwDPMIEMU
RAWRelMemory    endp


;-------------------------------------------------------------------------------
;
;Reset dirty bit for specified memory.
;
;On Entry:
;
;BX:CX  Starting linear address of pages to discard
;SI:DI  Number of bytes to discard
;
;On Exit:
;
;Carry clear, all other registers preserved.
;
;-------------------------------------------------------------------------------
RawDiscardPages proc    near
        push    ds
        push    es
        pushad
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero
        mov     es,ax
;
;Get base address.
;
        shl     ebx,16
        mov     bx,cx
;
;Get length.
;
        shl     esi,16
        mov     si,di
;
;Round start up a page.
;
        mov     eax,ebx
        add     ebx,4095
        and     ebx,not 4095
        sub     eax,ebx
        neg     eax
        cmp     eax,esi
        jnc     mem4_8
        sub     esi,eax
;
;Round length down a page.
;
        and     esi,not 4095
        or      esi,esi
        jz      mem4_8
;
;Get page values.
;
        mov     edx,ebx
        shr     ebx,12
        shr     esi,12
        mov     ecx,esi
;
;Modify all page tables.
;
        mov     esi,4096*1024*1023      ;base of page alias memory.
mem4_0: cmp     edx,LinearBase
        jc      mem4_1
        cmp     edx,LinearLimit
        jnc     mem4_2
        and     DWORD PTR es:[esi+ebx*4],not ((1 shl 6)+(1 shl 11)) ;clear dirty & disk bits.
mem4_1: add     edx,4096
        inc     ebx
        dec     ecx
        jnz     mem4_0
;
mem4_2: call    EMUCR3Flush             ;update page cache.
;
mem4_8: clc
        popad
        assume ds:_cwDPMIEMU
        pop     es
        pop     ds
        ret
RawDiscardPages endp


;-------------------------------------------------------------------------------
;
;Map physical memory into address space and return linear access address.
;
;BX:CX  Physical address of memory
;SI:DI  Size of region to map in bytes
;
;Returns
;
;Carry set on error else,
;
;BX:CX  Linear address that can be used to access the physical memory.
;
RawMapPhys2Lin  proc    near
        push    ds
        push    es
        push    eax
        push    edx
        push    esi
        push    edi
        push    ebp
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero
        mov     es,ax
;
;Get physical address as 32-bit & check for none-page boundary.
;
        shl     ebx,16
        mov     bx,cx
        test    ebx,4095
        jnz     mem5_9
;
;Get length as number of pages.
;
        shl     esi,16
        mov     si,di
        add     esi,4095
        shr     esi,12
        test    esi,esi
        jz      mem5_9

; MED 06/13/96, remove 4M restriction
;; MED 04/18/96, don't allow more than a 4M mapping (not supported by CW)
;       cmp     esi,400h
;       ja      @@9

;
;Deal with addresses below 1 Meg.
;
        mov     edi,ebx
        mov     eax,esi
        shl     eax,12
        add     eax,edi
        dec     eax
        cmp     eax,100000h+10000h
        jc      mem5_8
;
;Find first un-used physical mapping space.
;
        mov     ebp,PageDirLinear
        mov     eax,1021
mem5_0:

; MED 04/18/96
        mov     ecx,es:[ebp+eax*4]
        and     ecx,(NOT 4095)  ; get linear address
        cmp     ecx,ebx                 ; see if matches desired linear address
        jne     med2c                   ; no, continue as before
        mov     edi,eax
        shl     edi,22                  ; convert index to 4M space
        jmp     mem5_8

med2c:
        cmp     DWORD PTR es:[ebp+eax*4],0
        jz      mem5_1

;       dec     eax
        sub     eax,1
        jc      mem5_9

        jmp     mem5_0
;
;Work out how many page tables we need and set first tables index.
;
mem5_1: mov     ecx,esi
        shr     ecx,10
        sub     eax,ecx
        inc     ecx
;
;Make sure we can get enough memory for physical page tables.
;
        call    PhysicalGetPages
        add     edx,NoneLockedPages
        cmp     edx,ecx
        jc      mem5_9
;
;Put all the page tables into place.
;
        lea     edi,[ebp+eax*4]
        push    edi
        mov     ebp,ecx
mem5_2: call    PhysicalGetPage
        jnc     mem5_3
        call    UnMapPhysical
        jc      mem5_10
mem5_3: and     ecx,1           ;put user bits in useful place.
        shl     ecx,10
        and     edx,0FFFFFFFFh-4095     ;lose user bits.
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     DWORD PTR es:[edi],edx  ;store this tables address.
        push    edi
        sub     edi,PageDIRLinear
        add     edi,PageAliasLinear     ;get alias table address.
        mov     DWORD PTR es:[edi],edx  ;setup in alias table as well.
        pop     edi
        ;
        ;Clear this page to 0.
        ;
        push    ecx
        push    edi
        sub     edi,PageDIRLinear
        shl     edi,10
        add     edi,1024*4096*1023      ;base of page alias's.
        mov     ecx,4096/4
        xor     eax,eax
        cld
        rep     stosd
        pop     edi
        pop     ecx
        ;
        add     edi,4
        dec     ebp
        jnz     mem5_2
        pop     edi
;
;Now map specified physical address range into place.
;
        sub     edi,PageDIRLinear
        shl     edi,10          ;start of first page table
        push    edi
        add     edi,1024*4096*1023
        or      ebx,111b
mem5_4: mov     es:[edi],ebx
        add     edi,4
        add     ebx,4096
        dec     esi
        jnz     mem5_4
        pop     edi
;
;Return linear address to caller.
;
        shl     edi,12-2
mem5_8: xor     ecx,ecx
        xor     ebx,ebx
        mov     cx,di
        shr     edi,16
        mov     bx,di
        clc
        jmp     mem5_11
;
mem5_10:
        pop     edi
mem5_9: stc
mem5_11:
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     eax
        assume ds:_cwDPMIEMU
        pop     es
        pop     ds
        ret
RawMapPhys2Lin  endp


;-------------------------------------------------------------------------------
;
;Un-Do a physical to linear address mapping.
;
RawUnMapPhys2Lin proc near
        clc
        ret
RawUnMapPhys2Lin endp


;-------------------------------------------------------------------------------
RAWLockMemory   proc    near
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
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        ;
        shl     ebx,16
        mov     bx,cx
        shl     esi,16
        mov     si,di
        add     esi,ebx
        and     ebx,not 4095            ;round down to nearest page.
        mov     d[_LM_BlockBase],ebx
        add     esi,4095
        and     esi,not 4095            ;round up to next page.
        dec     esi
        mov     d[_LM_BlockEnd],esi     ;store address of last page.
        ;
        ;Check if start above end now!
        ;
        mov     eax,d[_LM_BlockBase]
        cmp     eax,d[_LM_BlockEnd]
        jnc     mem7_10
        ;
        ;Count number of pages we need for this range.
        ;
        mov     d[_LM_Needed],0
        mov     eax,NoneLockedPages
        mov     d[_LM_Got],eax
        mov     eax,d[_LM_BlockBase]
mem7_04:
        cmp     eax,LinearBase  ;must be in our memory pool.
        jc      mem7_05
        cmp     eax,LinearLimit
        jnc     mem7_05
        push    eax
        call    GetPageStatus
        pop     eax
        jc      mem7_15         ;ignore not present tables.
        test    edx,1
        jz      mem7_005                ;already present.
        call    RawPageLocked   ;locked page?
        jnz     mem7_05
        dec     d[_LM_Got]              ;reduce available pages.
        jmp     mem7_05
mem7_005:
        inc     d[_LM_Needed]
mem7_05:
        add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      mem7_04
;
;Check if we actually need any more pages to lock this region.
;
        cmp     d[_LM_Needed],0
        jz      mem7_OK
;
;If VMM isn't active then pages can always be locked assumeing they exist.
;
        cmp     VMMHandle,0
        jnz     mem7_VMM
        mov     eax,d[_LM_Needed]
        cmp     eax,d[_LM_Got]
        jc      mem7_OK
        jz      mem7_OK
        jmp     mem7_15
;
;VMM is active and pages are required so we need to make sure enough pages are
;left for swapping.
;
mem7_VMM:
        mov     eax,d[_LM_Needed]
        add     eax,16          ;arbitrary safety buffer.
        cmp     eax,d[_LM_Got]
        jc      mem7_OK
        jz      mem7_OK
        jmp     mem7_15
;
;Enough free pages so lock the region.
;
mem7_OK:
        mov     eax,d[_LM_BlockBase]
mem7_4: cmp     eax,LinearBase  ;must be in our memory pool.
        jc      mem7_5
        cmp     eax,LinearLimit
        jnc     mem7_5
        push    eax
        call    GetPageStatus
        pop     eax
        jc      mem7_15         ;ignore not present tables.
        test    edx,1           ;is it present?
        jnz     mem7_6
        ;
mem7_11:
        ;Need to allocate a physical page first.
        ;
        push    eax
        call    UnMapPhysical
        pop     eax
        jc      mem7_15         ;this shouldn't happen.
        mov     LinearEntry,eax
        shr     LinearEntry,12  ;store page number to allocate at.
        push    eax
        call    MapPhysical             ;map this page in.
        pop     eax
        ;
mem7_6: ;Now mark this page as locked.
        ;
        call    RawLockPage
        ;
mem7_5: add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      mem7_4
        ;
mem7_10:
        clc
        jmp     mem7_1
        ;
mem7_15:
        stc
        ;
mem7_1:
        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     gs
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
        ret
        assume ds:_cwDPMIEMU
RAWLockMemory   endp


;-------------------------------------------------------------------------------
RAWUnLockMemory proc near
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
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        shl     ebx,16
        mov     bx,cx
        shl     esi,16
        mov     si,di
        add     esi,ebx
        and     ebx,0FFFFFFFFh-4095     ;round down to nearest page.
        mov     d[_LM_BlockBase],ebx
        add     esi,4095
        and     esi,0FFFFFFFFh-4095     ;round up to next page.
        dec     esi
        mov     d[_LM_BlockEnd],esi     ;store address of last page.
        ;
        ;Now run through all pages in this range un-locking them.
        ;
        mov     eax,d[_LM_BlockBase]
mem8_4: cmp     eax,LinearBase  ;must be in our memory pool.
        jc      mem8_5
        cmp     eax,LinearLimit
        jnc     mem8_5
        push    eax
        call    GetPageStatus
        pop     eax
        jc      mem8_5          ;ignore not present tables.
        test    edx,1           ;is it present?
        jz      mem8_5
        call    RawUnLockPage   ;unlock the page.
mem8_5: add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      mem8_4
        clc
mem8_1:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     gs
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
        ret
        assume ds:_cwDPMIEMU
RAWUnLockMemory endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
RAWGetMemoryMax proc near
;
;Work out biggest memory block remaining.
;
        ;
        push    eax
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        mov     ax,KernalDS             ;make data addresable.
        mov     ds,ax
        assume ds:_cwRaw

        push    ebx
        push    ecx
        push    edx
        xor     ebx,ebx
        mov     bx,ss
        mov     ecx,esp
        pushf
        cli
        mov     edx,d[RawStackPos]
        sub     d[RawStackPos],RawStackDif
        popf
        mov     ax,KernalSS
        mov     ss,ax
        mov     esp,edx
        add     ecx,4+4+4
        push    ebx
        push    ecx
        sub     ecx,4+4+4
        push    es
        mov     es,bx
        mov     edx,es:[ecx]
        mov     ebx,es:[ecx+4+4]
        mov     ecx,es:[ecx+4]
        pop     es

        mov     ax,KernalZero
        mov     es,ax
        ;
        ;Look in existing memory first.
        ;
        mov     edx,LinearBase  ;Get starting point.
        mov     ecx,LinearLimit
        sub     ecx,edx         ;Get memory present.
        shr     ecx,12          ;Get pages.
        shr     edx,12          ;Get page number.
        xor     edi,edi         ;Clear flag.
        xor     ebp,ebp         ;Clear biggest so far.
        ;
mem9_l0:
        ;Look for a bigest block of free memory.
        ;
        mov     eax,DWORD PTR es:[(1024*4096*1022)+edx*4] ;Get page details.
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE            ;Free block?
        jnz     mem9_l2
        or      edi,edi         ;Got any yet?
        jnz     mem9_l1
        mov     esi,edx         ;Get base page number.
mem9_l1:
        inc     edi
        cmp     edi,ebp         ;Biggest yet?
        jc      mem9_l3
        mov     ebx,esi         ;Get base.
        mov     ebp,edi         ;Get size.
        jmp     mem9_l3
mem9_l2:
        xor     edi,edi
mem9_l3:
        inc     edx             ;Next page.
        dec     ecx
        jnz     mem9_l0
        ;
        ;See if biggest block found is the last block in the map.
        ;
        xor     edx,edx         ;reset end of chain value.
        or      edi,edi         ;last block free?
        jz      mem9_l4
        cmp     ebx,esi         ;same base?
        jnz     mem9_l4
        mov     edx,ebp         ;setup extra block size.
        mov     ebp,0           ;reset normal block size.
        jmp     mem9_l5
        ;
mem9_l4:
        ;Get size of the last block in the memory map.
        ;
        mov     eax,LinearBase
        mov     esi,LinearLimit
        shr     esi,12
        dec     esi
        shr     eax,12
        mov     ecx,esi
        sub     ecx,eax
mem9_l6:
        jecxz   mem9_l5
        mov     eax,DWORD PTR es:[(1024*4096*1022)+esi*4] ;Get page details.
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE            ;Free block?
        jnz     mem9_l5
        dec     esi
        inc     edx
        dec     ecx
        jmp     mem9_l6
        ;
mem9_l5:
        ;See what extra memory we can get hold of.
        ;
        mov     ebx,edx
        call    PhysicalGetPages        ;Get extra memory available.
        mov     ecx,ebx         ;Save original value.
        add     ebx,edx         ;update extra memory value.
        ;
        ;See how many pages of real memory would be lost to page tables.
        ;
        mov     eax,LinearLimit
        shr     eax,12
        add     eax,edx
        shr     eax,10
        mov     edx,LinearLimit
        shr     edx,12
        dec     edx
        shr     edx,10
        sub     eax,edx
        add     eax,eax         ;Page + Det
        sub     ebx,eax
        ;
        ;See what extra memory the VMM can get hold of.
        ;
        cmp     VMMHandle,0
        jz      mem9_l8
        mov     ebx,ecx
        push    ebx
        push    ebp
        ;
        ;Get free disk space remaining.
        ;
        mov     dl,VMMName              ;get drive letter for this media.
        sub     dl,'A'          ;make it real.
        inc     dl              ;adjust for current type select.
        mov     ah,36h          ;get free space.
        int     21h             ;/
        xor     edx,edx
        cmp     ax,-1           ;invalid drive?
        jz      mem9_l7
        mul     cx              ;Get bytes per cluster.
        mul     bx              ;Get bytes available.
        shl     edx,16
        mov     dx,ax
mem9_l7:
        ;
        ;Get current swap file size.
        ;
        push    edx
        mov     bx,VMMHandle
        mov     ax,4202h
        xor     cx,cx
        mov     dx,cx
        int     21h
        shl     edx,16
        mov     dx,ax
        pop     eax
        add     edx,eax
        and     edx,not 65535
        shr     edx,12
        pop     ebp
        pop     ebx
        ;
        ;Work out how much of the VMM space is extra.
        ;
        mov     eax,LinearLimit
        sub     eax,LinearBase
        shr     eax,12
        sub     edx,eax
        add     ebx,edx

        ;
mem9_l8:
        ;Check which block is bigger and exit.
        ;
        push    ecx
        mov     eax,ebx
        shl     eax,12
        mov     ecx,LinearLimit
        sub     ecx,LinearBase
        sub     eax,ecx
        js      mem9_l89
        cmp     eax,MaxMemLin
        jc      mem9_l89
        mov     ebx,MaxMemLin
        sub     ebx,ecx
        shr     ebx,12
mem9_l89:
        pop     ecx

        cmp     ebx,ebp
        jnc     mem9_l9
        mov     ebx,ebp
mem9_l9:
        shl     ebx,12
        clc

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
RAWGetMemoryMax endp


;-------------------------------------------------------------------------------
;
;Extend the linear memory map by allocateing physical memory if available or
;virtual memory if not, or even a combination of the two.
;
;On Entry:
;
;ECX    - Pages to extend by.
;
;On Exit:
;
;Carry set on error else memory map extended.
;
ExtendLinearMemory proc near
        pushad
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero
        mov     es,ax
        mov     ebp,ecx

        mov     eax,ecx
        shl     eax,12
        dec     eax
        add     eax,LinearLimit
        dec     eax
        sub     eax,LinearBase
        cmp     eax,MaxMemLin
        jnc     mem10_error

        ;
        ;Try extending useing physical memory first.
        ;
mem10_f0:
        mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        shr     eax,10          ;/1024 for page dir entry.
        mov     edi,PageDirLinear       ;get page table address.
        mov     eax,DWORD PTR es:[edi+eax*4]    ;this page present?
        test    eax,1           ;do we have a page table?
        jnz     mem10_f1                ;keep going till we do.
;
;No page table so make sure we can get 2 pages (page and det)
;
        call    PhysicalGetPages
        cmp     edx,2
        jc      mem10_virtual
;
;both pages available so go to it.
;
mem10_f2:
        call    PhysicalGetPage ;get a page.
        jc      mem10_error             ;it lied.
        mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        push    LinearEntry
        call    MapPhysical             ;use this page for page table.
        call    PhysicalGetPage ;get a page.
        pop     LinearEntry
        jc      mem10_error             ;it lied.
        call    MapPhysical             ;use this page for page table.
        ;
mem10_f1:
        call    PhysicalGetPage ;get a page.
        jc      mem10_Virtual           ;use virtual memory.
        mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        call    MapPhysical             ;use this page for page table.
        ;
        ;Update details.
        ;
        mov     eax,LinearLimit
        shr     eax,12
        mov     DWORD PTR es:[(1024*4096*1022)+eax*4],0
        inc     FreePages
;       dec     medAllocPages
        inc     TotalPages
        add     LinearLimit,4096        ;bump up the end of the memory map.
        dec     ebp             ;update the counter.
        jnz     mem10_f0                ;keep looking.
        clc
        jmp     mem10_exit              ;All physical so exit.
        ;
mem10_Virtual:
        ;Virtual memory will be needed so see if we can provide any.
        ;
        cmp     VMMHandle,0             ;Virtual memory active?
        jz      mem10_error
        ;
        ;Find out how much disk space is left for swap file to grow into.
        ;
        mov     dl,VMMName              ;get drive letter for this media.
        sub     dl,'A'          ;make it real.
        inc     dl              ;adjust for current type select.
        mov     ah,36h          ;get free space.
        push    ebp
        int     21h             ;/
        pop     ebp
        cmp     ax,-1           ;invalid drive?
        jz      mem10_error
        mul     cx              ;Get bytes per cluster.
        mul     bx              ;Get bytes available.
        shl     edx,16
        mov     dx,ax           ;make 32 bit.
        and     edx,not 65535
        add     edx,SwapFileLength      ;add existing size.
        mov     eax,LinearLimit
        sub     eax,LinearBase  ;get current real memory.
        sub     edx,eax
        shr     edx,12          ;get it as pages.
        cmp     edx,ebp         ;Enough pages?
        jc      mem10_error
        ;
        ;Find out how many un-locked pages we currently have.
        ;
        mov     edx,NoneLockedPages
        ;
        ;Enough page's for minimum requirement?
        ;
        cmp     edx,16          ;un-locked pages < 16?
        jc      mem10_error             ;force minimum of 16.
        sub     edx,16
        ;
        ;Work out how many new page/det tables are required.
        ;
        mov     eax,LinearLimit
        shr     eax,12
        mov     ebx,eax
        dec     eax             ;Last definatly valid page table.
        add     ebx,ebp         ;New end page.
        shr     eax,10
        shr     ebx,10          ;Get page table units (4Meg)
        sub     ebx,eax         ;get the differance.
        jz      mem10_DoneTables
        add     ebx,ebx         ;Page + Det
        mov     ecx,ebp
        cmp     ebx,edx
        jc      mem10_OK
        jz      mem10_OK
        jmp     mem10_error
        ;
mem10_OK:
        ;Allocate new page tables.
        ;
        mov     esi,PageDirLinear       ;get page directory address.
        mov     edx,LinearLimit
        shr     edx,12
mem10_v2:
        mov     eax,edx         ;get new entry number.
        shr     eax,10          ;/1024 for page dir entry.
        test    DWORD PTR es:[esi+eax*4],1      ;this page present?
        jnz     mem10_v3
        ;
        ;get DET page.
        ;
        push    ecx
        push    edx
        call    UnMapPhysical   ;get a physical page for us to use.
        mov     eax,edx
        mov     ebx,ecx
        pop     edx
        pop     ecx
        jc      mem10_error             ;not enough physical memory to support virtual memory.
        push    ecx
        push    edx
        mov     LinearEntry,edx ;set logical page address.
        mov     edx,eax         ;get physical address again.
        mov     ecx,ebx
        call    MapPhysical             ;use this to add a new page table.
        pop     edx
        pop     ecx
        ;
        ;And again for page table.
        ;
        push    ecx
        push    edx
        call    UnMapPhysical   ;get a physical page for us to use.
        mov     eax,edx
        mov     ebx,ecx
        pop     edx
        pop     ecx
        jc      mem10_error             ;not enough physical memory to support virtual memory.
        push    ecx
        push    edx
        mov     LinearEntry,edx ;set logical page address.
        mov     edx,eax         ;get physical address again.
        mov     ecx,ebx
        call    MapPhysical             ;use this to add a new page table.
        pop     edx
        pop     ecx
mem10_v3:
        inc     edx
        dec     ecx
        jnz     mem10_v2
        ;
mem10_DoneTables:
        ;Now mark all the new pages as un-locked/free
        ;
        mov     eax,LinearLimit
        mov     ecx,ebp
mem10_v4:
        call    RawClearPageLock        ;clear page locking for this entry.
        push    eax
        shr     eax,12
        mov     DWORD PTR es:[1024*4096*1022+eax*4],0
        pop     eax
        add     eax,4096
        dec     ecx
        jnz     mem10_v4
        ;
        ;Extend the swap file.
        ;
        mov     ecx,LinearLimit ;current end position.
        sub     ecx,LinearBase  ;length.
        mov     eax,ebp         ;extension needed in pages.
        shl     eax,12
        add     ecx,eax         ;New extremity desired.
        cmp     ecx,SwapFileLength
        jc      mem10_Extended
        add     ecx,65535
        and     ecx,not 65535
        push    ecx
        mov     dx,cx
        shr     ecx,16
        mov     bx,VMMHandle            ;get swap file handle.
        mov     ax,4200h
        mov     edi,offset PageInt
        push    edi
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     RealRegsStruc.Real_EDX[edi],edx
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        mov     ah,40h
        mov     bx,VMMHandle            ;get swap file handle.
        xor     cx,cx
        push    edi
        mov     edi,offset PageInt
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     RealRegsStruc.Real_EDX[edi],edx
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        pop     ecx
        test    RealRegsStruc.Real_Flags[edi],1
        jnz     mem10_disk_error
        mov     SwapFileLength,ecx
        ;
        xor     cx,cx
        xor     dx,dx
        mov     ax,4201h
        mov     bx,VMMHandle
        push    edi
        mov     edi,offset PageInt
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     RealRegsStruc.Real_EDX[edi],edx
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        test    RealRegsStruc.Real_Flags[edi],1
        jnz     mem10_disk_error
        mov     edx,RealRegsStruc.Real_EDX[edi]
        mov     eax,RealRegsStruc.Real_EAX[edi]
        shl     edx,16
        mov     dx,ax
        cmp     edx,SwapFileLength
        jnz     mem10_disk_error
        ;
mem10_Extended:
        ;Update the end of the memory map.
        ;
        add     FreePages,ebp
;       sub     medAllocPages,ebp
        shl     ebp,12
        add     LinearLimit,ebp
        clc
        jmp     mem10_Exit
        ;
mem10_error:
        stc
mem10_Exit:
        pop     es
        pop     ds
        popad
        ret
        ;
mem10_Disk_Error:
        jmp     mem10_error
        assume ds:_cwDPMIEMU
ExtendLinearMemory endp


;-------------------------------------------------------------------------------
;
;Map physical page supplied into logical address space. If a new page table is needed, the page
;supplied will become a page table.
;
;On Entry:-
;
;CL     - Use flags 0-3. 1 being used for VCPI memory to allow release later.
;EDX    - Physical address to map.
;
MapPhysical     proc    near
        pushad
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     ax,KernalZero   ;make everything addresable.
        mov     es,ax
        ;
        and     ecx,1           ;put user bits in useful place.
        shl     ecx,10
        and     edx,0FFFFFFFFh-4095     ;lose user bits.
        mov     eax,LinearEntry ;get new entry number.
        shr     eax,10          ;/1024 for page dir entry.
        ;
        mov     esi,PageDirLinear       ;get page table address.
        test    DWORD PTR es:[esi+eax*4],1      ;this page present?
        jnz     mem11_AddPage
        ;
        cmp     PageDETLinear,0 ;DET in use yet?
        jz      mem11_AddTable
        mov     esi,PageDETLinear       ;get page table address.
        test    DWORD PTR es:[esi+eax*4],1      ;DET page present?
        jnz     mem11_AddTable
        ;
mem11_AddDET:
        ;Need a new DET page.
        ;
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     DWORD PTR es:[esi+eax*4],edx    ;store this tables address.
        ;
        ;Clear this page to locked.
        ;
        mov     edi,1024*4096*1022      ;base of page DET's.
        mov     eax,LinearEntry ;get the entry number again.
        shl     eax,2           ;4 bytes per entry.
        add     edi,eax
        mov     ecx,4096/4
        mov     eax,MEM_FILL
        cld
        rep     stosd
        ;
        jmp     mem11_Finished
        ;
mem11_AddTable:
        ;Need a new page table.
        ;
        mov     eax,LinearEntry ;get new entry number.
        shr     eax,10          ;/1024 for page dir entry.
        mov     esi,PageDirLinear       ;get page table address.
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     DWORD PTR es:[esi+eax*4],edx    ;store this tables address.
        mov     esi,PageAliasLinear     ;get alias table address.
        mov     DWORD PTR es:[esi+eax*4],edx    ;setup in alias table as well.
        ;
        ;Clear this page to 0.
        ;
        mov     edi,1024*4096*1023      ;base of page alias's.
        mov     eax,LinearEntry ;get the entry number again.
        shl     eax,2           ;4 bytes per entry.
        add     edi,eax
        mov     ecx,4096/4
        xor     eax,eax
        cld
        rep     stosd
        ;
        jmp     mem11_Finished
        ;
mem11_AddPage:
        ;Update recent page usage stack.
        ;
        push    ecx
        push    es
        push    ds
        pop     es
        mov     edi,offset RecentMapStack+(4*PageStackSize)-4
        mov     esi,offset RecentMapStack+(4*PageStackSize)-8
        mov     ecx,PageStackSize-1
        std
        rep     movsd
        pop     es
        pop     ecx
        cld
        mov     eax,LinearEntry
        shl     eax,12
        mov     RecentMapStack,eax
        ;
        ;Add this to the relavent page table.
        ;
        mov     eax,LinearEntry ;get the entry number again.
        mov     esi,1024*4096*1023      ;base of page alias's.
        mov     ebx,DWORD PTR es:[esi+eax*4]    ;get current details.
        and     ebx,1 shl 11
        or      edx,ebx
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     DWORD PTR es:[esi+eax*4],edx    ;set physical address.
        ;
        cmp     PageDETLinear,0
        jz      mem11_NoLocking
        mov     eax,LinearEntry
        shl     eax,12
        call    RawClearPageLock        ;clear page locking for this entry.
        ;
        ;Update number of un-locked physical pages present.
        ;
        inc     NoneLockedPages
        ;
mem11_NoLocking:
        ;Check if this page needs fetching from swap space.
        ;
        cmp     VMMHandle,0
        jz      mem11_NoRead
        ;
        test    ebx,1 shl 11
        jz      mem11_NoRead
        ;
        mov     esi,BreakAddress
        mov     al,es:[esi]
        mov     BYTE PTR es:[esi],0
        push    eax
        ;
        mov     ecx,LinearEntry ;get page number.
        shl     ecx,12          ;get linear address.
        sub     ecx,LinearBase
        mov     dx,cx
        shr     ecx,16
        mov     bx,VMMHandle
        mov     ax,4200h
        mov     edi,offset PageInt
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        push    es
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        mov     edi,offset PageInt
        mov     ax,VMMHandle
        mov     RealRegsStruc.Real_EBX[edi],eax
        mov     ax,PageBufferReal
        mov     RealRegsStruc.Real_DS[edi],ax
        mov     RealRegsStruc.Real_EAX[edi],3f00h
        mov     RealRegsStruc.Real_EDX[edi],0
        mov     RealRegsStruc.Real_ECX[edi],4096
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateInt       ;read it from disk.
        pop     es
        test    RealRegsStruc.Real_Flags[edi],1
        jz      mem11_ok
        mov     esi,BreakAddress
        pop     eax
        mov     es:[esi],al
        jmp     mem11_Finished2
        ;
mem11_ok:
        mov     esi,BreakAddress
        pop     eax
        mov     es:[esi],al
        ;
        mov     esi,PageBufferLinear
        mov     edi,LinearEntry
        shl     edi,12          ;get linear address again.
        mov     ecx,4096/4
        push    ds
        push    es
        pop     ds
        cld
        rep     movsd           ;copy back into place.
        pop     ds
        ;
        mov     eax,LinearEntry ;get new entry number.
        mov     esi,1024*4096*1023      ;base of page alias's.
        and     DWORD PTR es:[esi+eax*4],0FFFFFFFFh-(3 shl 5)   ;clear accesed & dirty bits.
        call    EMUCR3Flush
        ;
mem11_NoRead:
        inc     LinearEntry             ;update counter.
        ;
mem11_Finished:
        clc
        ;
mem11_Finished2:
        pop     es
        pop     ds
        popad
        ret
        assume ds:_cwDPMIEMU
MapPhysical     endp


;-------------------------------------------------------------------------------
;
;Retrieve the address of the most un-needed physical memory and mark its current page table entry
;as not present after writing to disk if needed.
;
;On Entry:-
;
;On Exit:-
;
;EDX    - Physical address of page.
;CL     - User flags.
;
UnMapPhysical   proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        ;
        mov     ax,KernalDS
        mov     ds,ax
        mov     es,ax
        assume ds:_cwRaw
        mov     ax,KernalZero
        mov     fs,ax
        ;
        mov     ProposedPresentFlag,0
        mov     ProposedRecentFlag,0
        mov     CompareCount,0
        ;
        mov     esi,PageDirLinear
        mov     edi,1024*4096*1023      ;base of page alias's.
        mov     ebp,1024*4096*1022
        mov     ecx,PageingPointer      ;get current position.
        mov     ebx,LinearLimit ;maximum size of scan we can do.
        sub     ebx,LinearBase
        shr     ebx,12
        inc     ebx
        cld
        ;
mem12_ScanLoop:
        dec     ebx
        jnz     mem12_80                ;shit, we've been all the way round.
        ;
        cmp     ProposedPresentFlag,0
        jnz     mem12_UseProposed
        jmp     mem12_8
        ;
mem12_80:
        cmp     ecx,LinearBase
        jnc     mem12_80_0
        mov     ecx,LinearBase
        sub     ecx,4096
mem12_80_0:
        add     ecx,4096
        cmp     ecx,LinearLimit ;End of memory map yet?
        jc      mem12_NoWrap
        mov     ecx,LinearBase
        ;
mem12_NoWrap:
        mov     eax,ecx
        shr     eax,12          ;get page number.
        test    DWORD PTR fs:[edi+eax*4],1      ;this page present?
        jz      mem12_ScanLoop
        test    DWORD PTR fs:[ebp+eax*4],MEM_LOCK_MASK shl MEM_LOCK_SHIFT
        jnz     mem12_ScanLoop
        ;
        inc     CompareCount
        ;
        ;Check against recent stack.
        ;
        push    ecx
        push    edi
        mov     eax,ecx
        mov     edi,offset RecentMapStack
        mov     ecx,PageStackSize
        repnz   scasd
        pop     edi
        pop     ecx
        jz      mem12_IsRecent
        shr     eax,12          ;get page number again.
        test    DWORD PTR fs:[edi+eax*4],1 shl 6
        jz      mem12_GotPage
        ;
        cmp     ProposedPresentFlag,0
        jz      mem12_SetProposed
        cmp     ProposedRecentFlag,0
        jz      mem12_UseProposed?
mem12_SetProposed:
        mov     ProposedPresentFlag,-1
        mov     ProposedPage,ecx
        mov     CompareCount,0
        mov     ProposedRecentFlag,0
        jmp     mem12_ScanLoop
        ;
mem12_UseProposed?:
        mov eax,NoneLockedPages
        shr     eax,2
        cmp     eax,4096
        jc      mem12_UP0
        mov     eax,4096
mem12_UP0:
        cmp     CompareCount,eax
        jc      mem12_UseProposed
        jmp     mem12_ScanLoop
        ;
mem12_IsRecent:
        cmp     ProposedPresentFlag,0
        jnz     mem12_ProposedRecent?
        mov     ProposedPresentFlag,-1
        mov     ProposedPage,ecx
        mov     ProposedRecentFlag,-1
        mov     CompareCount,0
        jmp     mem12_ScanLoop
        ;
mem12_ProposedRecent?:
        cmp     ProposedRecentFlag,0
        jnz     mem12_LookedEnough?
        mov     eax,NoneLockedPages
        shr     eax,2
        cmp     eax,4096
        jc      mem12_PR0
        mov     eax,4096
mem12_PR0:
        cmp     CompareCount,eax
        jnc     mem12_UseProposed
        jmp     mem12_ScanLoop
        ;
mem12_LookedEnough?:
        mov     eax,NoneLockedPages
        cmp     CompareCount,eax
        jnc     mem12_UseProposed
        jmp     mem12_ScanLoop
        ;
mem12_UseProposed:
        mov     ecx,ProposedPage
        ;
mem12_GotPage:
        mov     PageingPointer,ecx
        ;
        mov     eax,ecx
        shr     eax,12          ;get page number again.
        shl     eax,2
        add     edi,eax
        ;
        ;Check if it needs to go to the swap file.
        ;
        test    DWORD PTR fs:[edi],1 shl 6      ;is it dirty?
        jz      mem12_5         ;no need to write it if not.
        ;
        ;Flush this page to disk.
        ;
        push    edi
        mov     esi,BreakAddress
        mov     al,fs:[esi]
        mov     BYTE PTR fs:[esi],0
        push    eax
        ;
        sub     edi,1024*4096*1023      ;get page table entry number.
        shr     edi,2           ;page number.
        shl     edi,12          ;get linear address.
        push    edi
        mov     esi,edi
        push    ds
        push    es
        mov     edi,PageBufferLinear    ;copy it to somewhere we can deal with it.
        mov     ax,KernalZero
        mov     ds,ax
        mov     es,ax
        mov     ecx,4096/4
        rep     movsd
        pop     ds
        pop     es
        pop     edi
        ;
        sub     edi,LinearBase
        mov     ebp,edi
        mov     dx,di
        shr     edi,16
        mov     cx,di
        mov     ax,4200h
        mov     bx,VMMHandle
        mov     edi,offset PageInt
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        mov     RealRegsStruc.Real_ECX[edi],ecx
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        ;
        test    RealRegsStruc.Real_Flags[edi],1
        stc
        jnz     mem12_error_anyway
        mov     edx,RealRegsStruc.Real_EDX[edi]
        mov     eax,RealRegsStruc.Real_EAX[edi]
        shl     edx,16
        mov     dx,ax
        cmp     edx,ebp
        jnz     mem12_force_error
        ;
        mov     edi,offset PageInt
        mov     ax,VMMHandle
        mov     RealRegsStruc.Real_EBX[edi],eax
        mov     ax,PageBufferReal
        mov     RealRegsStruc.Real_DS[edi],ax
        mov     RealRegsStruc.Real_EAX[edi],4000h
        mov     RealRegsStruc.Real_EDX[edi],0
        mov     RealRegsStruc.Real_ECX[edi],4096
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateInt       ;write it to disk.
        test    RealRegsStruc.Real_Flags[edi],1
        stc
        jnz     mem12_error_anyway
        mov     eax,RealRegsStruc.Real_EAX[edi]
        cmp     ax,4096
        jz      mem12_error_anyway
mem12_force_error:
        stc
mem12_error_anyway:
        mov     esi,BreakAddress
        pop     eax
        mov     fs:[esi],al
        ;
        pop     edi
        jc      mem12_8
        or      DWORD PTR fs:[edi],1 shl 11     ;signal it living on disk.
        ;
mem12_5:
        ;Now remove it from the page table and exit.
        ;
        and     DWORD PTR fs:[edi],0FFFFFFFFh-1 ;mark as not present.
        mov     edx,fs:[edi]            ;get page entry.
        mov     ecx,edx
        and     edx,0FFFFFFFFh-4095     ;lose flag bits.
        shr     ecx,10
        and     ecx,1           ;preserve user flags.
        call    EMUCR3Flush
        ;
        ;Update number of un-locked physical pages present.
        ;
        dec     NoneLockedPages
        ;
        clc
        jmp     mem12_9
        ;
mem12_8:
        stc                     ;failed to find free page.
        ;
mem12_9:
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
UnMapPhysical   endp


;-------------------------------------------------------------------------------
;
;Lock page indicated.
;
;On Entry:-
;
;EAX    - Linear address to lock.
;
RawLockPage     proc    near
        push    eax
        push    ebx
        push    esi
        push    es
        shr     eax,12          ;get page number.
        mov     bx,KernalZero
        mov     es,bx
        mov     esi,1024*4096*1022      ;base of page DET's.
        mov     ebx,es:[esi+eax*4]
        shr     ebx,MEM_LOCK_SHIFT
        and     ebx,MEM_LOCK_MASK
        jnz     mem13_WasLocked
        ;
        ;Update number of un-locked physical pages present.
        ;
        push    ax
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        dec     NoneLockedPages
        assume ds:_cwDPMIEMU
        pop     ds
        pop     ax
        ;
mem13_WasLocked:
        cmp     ebx,MEM_LOCK_MASK
        jz      mem13_0
        add     DWORD PTR es:[esi+eax*4],1 shl MEM_LOCK_SHIFT   ;lock it.
mem13_0:
        pop     es
        pop     esi
        pop     ebx
        pop     eax
        ret
RawLockPage     endp


;-------------------------------------------------------------------------------
RawClearPageLock proc near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12
        mov     esi,1024*4096*1022      ;base of page alias's.
        mov     ebx,MEM_LOCK_MASK shl MEM_LOCK_SHIFT
        xor     ebx,-1
        and     DWORD PTR es:[esi+eax*4],ebx    ;un-lock it.
        pop     es
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        ret
RawClearPageLock endp


;-------------------------------------------------------------------------------
;
;Un-lock a linear page.
;
;On Entry:-
;
;EAX    - Linear address of page to unlock.
;
RawUnLockPage   proc    near
        call    RawPageLocked
        jz      mem15_9
        push    eax
        push    ebx
        push    esi
        push    es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12
        mov     esi,1024*4096*1022      ;base of page alias's.
        sub     DWORD PTR es:[esi+eax*4],1 shl MEM_LOCK_SHIFT   ;un-lock it.
        mov     eax,DWORD PTR es:[esi+eax*4]
        shr     eax,MEM_LOCK_SHIFT
        and     eax,MEM_LOCK_MASK
        jnz     mem15_NotUnLocked
        ;
        ;Update number of un-locked physical pages present.
        ;
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        inc     NoneLockedPages
        assume ds:_cwDPMIEMU
        pop     ds
        ;
mem15_NotUnLocked:
        pop     es
        pop     esi
        pop     ebx
        pop     eax
mem15_9:
        ret
RawUnLockPage   endp


;-------------------------------------------------------------------------------
;
;Check if page lock count is zero.
;
;On Entry:-
;
;EAX    - Linear page address.
;
;On Exit:-
;
;Zero set if page count=0.
;
RawPageLocked   proc    near
        push    eax
        push    ebx
        push    esi
        push    es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12          ;get page number.
        mov     esi,1024*4096*1022      ;base of page alias's.
        mov     ebx,DWORD PTR es:[esi+eax*4]
        shr     ebx,MEM_LOCK_SHIFT
        and     ebx,MEM_LOCK_MASK
        pop     es
        pop     esi
        pop     ebx
        pop     eax
        ret
RawPageLocked   endp


;-------------------------------------------------------------------------------
;
;Return page table entry for a particular linear address.
;
;On Entry:-
;
;EAX    - linear address.
;
;On Exit:-
;
;Carry set if page table not present else,
;
;EDX    - Page table entry.
;
GetPageStatus   proc    near
        push    eax
        push    ebx
        push    esi
        push    ds
        push    es
        push    ax
        mov     ax,KernalDS
        mov     ds,ax
        pop     ax
        assume ds:_cwRaw
        mov     bx,KernalZero
        mov     es,bx
        push    eax
        mov     esi,PageDirLinear
        shr     eax,12          ;get page number.
        shr     eax,10          ;get dir entry number.
        test    DWORD PTR es:[esi+eax*4],1      ;page table present?
        pop     eax
        jz      mem17_8
        mov     esi,4096*1024*1023      ;base of page alias memory.
        shr     eax,12          ;get page number.
        mov     edx,es:[esi+eax*4]      ;get page details.
        clc
        jmp     mem17_9
        ;
mem17_8:
        xor     edx,edx
        stc
mem17_9:
        pop     es
        pop     ds
        pop     esi
        pop     ebx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
GetPageStatus   endp


;-------------------------------------------------------------------------------
;
;The Exception 14 handler. This gets a shout whenever a not present page table is encountered by
;the processor. We check if the faulting address is within our range, passing to old handler if
;its not, re-aranging memory if it is.
;
VirtualFault    proc    far
        push    eax
        push    ecx
        push    edx
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     eax,ExceptionCR2
        mov     cx,KernalDS
        mov     ds,cx
        assume ds:_cwRaw
        cmp     eax,LinearBase  ;address below our memory space?
        jc      mem18_OldExit
        cmp     eax,LinearLimit ;address above our memory space?
        jnc     mem18_OldExit
        ;
        push    eax             ;save the linear address.
        call    UnMapPhysical   ;retrieve a physical page to use.
        pop     eax
        jc      mem18_Disk_Error                ;This should only happen on disk errors.
        shr     eax,12
        push    LinearEntry
        mov     LinearEntry,eax ;setup linear address we want to map.
        call    MapPhysical             ;map new page into faulting linear address space.
        pop     LinearEntry
        jc      mem18_Disk_Error                ;This should only happen on disk errors.
        ;
        pop     ds
        pop     edx
        pop     ecx
        pop     eax
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1        ;check exit size.
        assume ds:_cwRaw
        jz      mem18_Use32Bit3
        db 66h
        retf                    ;16 bit exit.
mem18_Use32Bit3:
        retf
        ;
mem18_disk_error:
        jmp     mem18_OldExit
        ;
        assume ds:nothing
mem18_OldExit:
        pop     ds
        pop     edx
        pop     ecx
        pop     eax
        jmp     FWORD PTR cs:[OldExcep14]       ;32 bit chaining.
OldExcep14      df ?
        assume ds:_cwDPMIEMU
VirtualFault    endp


;-------------------------------------------------------------------------------
;
;Allocate a block of DOS memory & provide a selector to access it with.
;
RawGetDOSMemory proc near
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        cmp     bx,-1           ;maximum?
        jz      mem19_0
        inc     ebx             ;para extra for us.
mem19_0:
        push    ebx
        mov     edi,offset MemIntBuffer
        push    ds
        pop     es
        mov     RealRegsStruc.Real_EAX[edi],4800h       ;get memory.
        mov     RealRegsStruc.Real_EBX[edi],ebx ;memory size.
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;allocate it.
        pop     ecx
        mov     eax,RealRegsStruc.Real_EAX[edi] ;get result.
        mov     ebx,RealRegsStruc.Real_EBX[edi]
        test    RealRegsStruc.Real_Flags[edi],1
        jz      mem19_1
        or      bx,bx           ;nothing available?
        jz      mem19_9
        dec     ebx             ;leave space for us!
        jmp     mem19_9
        ;
mem19_1:
        ;Now try and allocate enough selectors.
        ;
        push    eax
        push    ecx
        dec     cx              ;lose our para.
        shr     cx,12           ;get number of 64k chunks.
        inc     cx              ;+1 for base.
        call    RawGetDescriptors
        pop     ecx
        pop     ebx
        jc      mem19_8
        ;
        ;Store block size.
        ;
        push    ecx
        mov     dx,KernalZero
        mov     es,dx
        movzx   esi,bx          ;get segment address.
        shl     esi,4           ;get linear address.
        mov     WORD PTR es:[esi],cx            ;store block size.
        dec     WORD PTR es:[esi]               ;lose our para.
        pop     ecx
        ;
        ;Setup selectors.
        ;
        dec     cx              ;lose our para.
        inc     bx              ;/
        push    ds
        pop     es
        push    eax
        push    ebx
        ;
mem19_2:
        push    eax
        push    ebx
        push    ecx
        push    eax
        mov     edi,offset RawSelBuffer
        add     edi,7
        and     edi,not 7
        movzx   esi,bx
        shl     esi,4
        movzx   ecx,cx
        shl     ecx,4
        dec     ecx
        xor     al,al
        mov     ah,DescPresent+DescPL3+DescMemory+DescRWData
        call    EMUMakeDesc
        pop     ebx
        call    RawBPutDescriptor
        pop     ecx
        pop     ebx
        pop     eax
        add     eax,8           ;next selector.
        add     ebx,1000h               ;update segment base.
        movzx   ecx,cx
        sub     ecx,1000h               ;reduce segment size.
        jns     mem19_2         ;keep going till all done.
        ;
        pop     eax             ;Get base segment again.
        pop     edx             ;Get base selector again.
        clc
        jmp     mem19_10
        ;
mem19_8:
        ;Release memory we managed to allocate.
        ;
        mov     edi,offset MemIntBuffer
        mov     ax,KernalDS
        mov     es,ax
        mov     es:RealRegsStruc.Real_EAX[edi],4900h    ;release memory.
        mov     es:RealRegsStruc.Real_ES[edi],bx        ;segment.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;release it.
        ;
mem19_9:
        stc
mem19_10:
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        ret
        assume ds:_cwDPMIEMU
RawGetDOSMemory endp


;-------------------------------------------------------------------------------
;
;Re-size a block of DOS memory.
;
RawResDOSMemory proc near
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        push    bx
        push    dx
        mov     bx,dx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     esi,ecx
        sub     esi,16          ;back to our stuff.
        pop     dx
        pop     bx
        mov     ax,KernalZero
        mov     es,ax
        cmp     bx,WORD PTR es:[esi]            ;shrinking or expanding?
        jz      mem20_8
        jnc     mem20_Expand
        ;
mem20_Shrink:
        ;Attempt to shrink the memory block.
        ;
        push    ebx
        push    edx
        inc     ebx             ;include our para.
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     eax,esi
        shr     eax,4           ;get real mode segment.
        mov     RealRegsStruc.Real_EAX[edi],4a00h
        mov     RealRegsStruc.Real_EBX[edi],ebx
        mov     RealRegsStruc.Real_ES[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        mov     eax,RealRegsStruc.Real_EAX[edi]
        mov     ebx,RealRegsStruc.Real_EBX[edi]
        test    RealRegsStruc.Real_Flags[edi],1
        pop     edx
        pop     ecx
        jnz     mem20_9         ;DOS failed it!
        ;
        ;Lose any selectors that are no longer needed.
        ;
        push    ecx
        push    edx
        mov     ebx,edx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     esi,ecx
        sub     esi,16          ;back to our stuff.
        pop     edx
        pop     ecx
        push    ecx
        push    edx
        mov     ax,KernalZero
        mov     es,ax
        mov     ax,WORD PTR es:[esi]            ;get old size.
        mov     WORD PTR es:[esi],cx            ;store new size.
        shr     ax,12
        inc     eax
        mov     ebx,eax         ;need existing number.
        shr     cx,12           ;get 64k chunks.
        inc     ecx             ;+ base.
        sub     ax,cx           ;get number of selectors to lose.
        jz      mem20_0
        pop     edx
        pop     ecx
        push    ecx
        push    edx
        shl     bx,3
        add     dx,bx           ;move to end of descriptors.
        movzx   ecx,ax
        shl     ax,3
        sub     edx,eax         ;reduce by number to lose.
        mov     ebx,edx
mem20_1:
        push    ebx
        push    ecx
        call    RawRelDescriptor        ;release this selector.
        pop     ecx
        pop     ebx
        add     ebx,8
        dec     ecx
        jnz     mem20_1
mem20_0:
        pop     edx
        pop     ecx
        ;
        ;Re-set all selector base & limits to affect new last selector.
        ;
        push    ecx
        push    edx
        mov     ebx,edx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     ebx,ecx
        shr     ebx,4           ;get real mode segment.
        pop     edx
        pop     ecx
        mov     eax,edx
;       shr     ax,3            ;lose TI & RPL
;       shr     ax,3            ;get descriptor number.
        push    ds
        pop     es
        ;
mem20_2:
        push    eax
        push    ebx
        push    ecx
        push    eax
        mov     edi,offset RawSelBuffer
        add     edi,7
        and     edi,0fffffff8h
        movzx   esi,bx
        shl     esi,4
        movzx   ecx,cx
        shl     ecx,4
        dec     ecx
        xor     al,al
        mov     ah,DescPresent+DescPL3+DescMemory+DescRWData
        call    EMUMakeDesc
        pop     ebx
        call    RawBPutDescriptor
        pop     ecx
        pop     ebx
        pop     eax
        add     eax,8           ;next selector.
        add     ebx,1000h               ;update segment base.
        movzx   ecx,cx
        sub     ecx,1000h               ;reduce segment size.
        jns     mem20_2         ;keep going till all done.
        ;
mem20_8:
        clc
        jmp     mem20_10
        ;
mem20_Expand:
        ;Attempt to expand the memory block.
        ;
        mov     bx,WORD PTR es:[esi]            ;return current length as maximum.
        mov     ax,8
mem20_9:
        stc
mem20_10:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        ret
        assume ds:_cwDPMIEMU
RawResDOSMemory endp


;-------------------------------------------------------------------------------
;
;Release a block of DOS memory.
;
RawRelDOSMemory proc near

        push    eax
        push    ebx
        push    ecx
        mov     ebx,edx
        and     ebx,0ffffh-7
        xor     ecx,ecx
        xor     eax,eax
        mov     ax,ds
        and     eax,not 7
        cmp     eax,ebx
        jnz     mem21_z0
        mov     ds,cx
mem21_z0:
        mov     ax,es
        and     eax,not 7
        cmp     eax,ebx
        jnz     mem21_z1
        mov     es,cx
mem21_z1:
        mov     ax,fs
        and     eax,not 7
        cmp     eax,ebx
        jnz     mem21_z2
        mov     fs,cx
mem21_z2:
        mov     ax,gs
        and     eax,not 7
        cmp     eax,ebx
        jnz     mem21_z3
        mov     gs,cx
mem21_z3:
        pop     ecx
        pop     ebx
        pop     eax

        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        ;Get segment base address.
        ;
        push    edx
        mov     ebx,edx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     esi,ecx
        pop     ebx
        sub     esi,16          ;back to our stuff.
        mov     eax,esi
        shr     eax,4           ;real mode paragraph address.
        mov     dx,KernalZero
        mov     es,dx
        mov     cx,WORD PTR es:[esi]            ;get block size.
        ;
        ;Release selectors.
        ;
mem21_0:
        push    eax
        push    ebx
        push    ecx
        call    RawRelDescriptor
        pop     ecx
        pop     ebx
        pop     eax
        add     ebx,8           ;next descriptor.
        movzx   ecx,cx
        sub     ecx,1000h
        jns     mem21_0         ;release all selectors.
        ;
        ;Release DOS memory block.
        ;
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],4900h       ;release block.
        mov     RealRegsStruc.Real_ES[edi],ax   ;block to release.
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;release it.
        mov     eax,RealRegsStruc.Real_EAX[edi]
        test    RealRegsStruc.Real_Flags[edi],1
        clc
        jz      mem21_noc
        stc
mem21_noc:
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        ret
RawRelDOSMemory endp
