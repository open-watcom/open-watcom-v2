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
        call    RAWCopyCheck
        ;
        pushm   eax,edx,ebp,ds,es
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
        pushm   ebx,ecx
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
        jz      @@error
@@start:        mov     edi,LinearBase  ;Get starting point.
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

@@l0:   repne   scasd

        ;
        ;Nothing found means extend end of memory map.
        ;
        jnz     @@ExtendNew

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
        jz      @@l2            ;Speed up 4K allocations.

        ;
        ;Set the number of pages to scan. This should be the number of
        ;pages we want but has to be reduced to the number of pages
        ;actually available when the request would go off the end of
        ;memory.
        ;
        mov     edx,ebx
        cmp     ecx,ebx         ;Enough space to check?
        jnc     @@l1
        mov     edx,ecx

        ;
        ;Scan specified number of entries to see if their all free.
        ;
@@l1:   pushm   ecx,edi
        mov     ecx,edx
        repe    scasd           ;Scan for free pages.
        popm    ecx,edi
        jnz     @@l3            ;not enough free entries.

        ;
        ;All the entries were free, now check if we were truncating the
        ;length to stop us running off the end of memory.
        ;
        cmp     edx,ebx         ;Truncated length?
        jnz     @@ExtendEnd
        jmp     @@l2

        ;
        ;One way or another we didn't find the number of entries we were
        ;after so restart the search.
        ;
@@l3:   add     edi,4
        dec     ecx
        jmp     @@l0

        ;
        ;We've found what we wanted without any extra messing around so
        ;hand this over to the marker.
        ;
@@l2:   sub     esi,1024*4096*1022      ;point to page details.
        shr     esi,2
        jmp     @@MarkMemRet


        ;
        ;Last block is free so use it as the basis for extension.
        ;
@@ExtendEnd:    mov     edi,ecx         ;remaining entries.
        sub     esi,(1024*4096*1022)
        shr     esi,2           ;Get start page number.
        mov     ecx,ebx         ;Total pages needed.
        sub     ecx,edi         ;Pages found so far.
        jmp     @@Extend
        ;
@@ExtendNew:    ;Last block not free so new block will start at LinearLimit.
        ;
        mov     ecx,ebx         ;Set pages needed.
        mov     esi,LinearLimit ;New start address.
        shr     esi,12
        ;
@@Extend:       ;Memory map needs extending so get on with it.
        ;
        call    ExtendLinearMemory      ;Try and extend memory map.
        jnc     @@MarkMemRet            ;Mark the block as used.
        ;
        ;Not enough memory so report failure.
        ;
        stc
        jmp     @@exit
        ;
@@MarkMemRet:   ;Got the memory so mark it in use.
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
        mov     es:d[(1024*4096*1022)+edx*4],MEM_START
        ;
        ;Now return details to caller.
        ;
        shl     esi,12          ;Convert back to a real address.
        mov     di,si
        shr     esi,16
        mov     cx,di
        mov     bx,si
        clc
        jmp     @@exit
        ;
@@error:        stc
        ;
@@exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    eax,edx,ebp,ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,edx,ebp,ds,es
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
        pushm   ebx,ecx
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
        jz      @@error
        ;
        ;Check for a valid address.
        ;
        test    esi,4095                ;all memory on page boundaries.
        jnz     @@error
        cmp     esi,LinearBase
        jc      @@error
        cmp     esi,LinearLimit
        jnc     @@error
        shr     esi,12          ;Get page number.
        mov     eax,es:d[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_START
        jnz     @@error
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
        jz      @@l1
@@l0:   mov     eax,es:d[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_END
        jnz     @@l1
        inc     ebp
        inc     edx
        dec     ecx
        jnz     @@l0
        ;
@@l1:   ;Shrinking or expanding?
        ;
        pop     ecx
        cmp     ecx,ebp
        jz      @@RetNewAddr
        jnc     @@Bigger
        ;
@@Smaller:      ;Shrinking the block so get on with it.
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
@@s0:   and     es:d[(1024*4096*1022)+edx*4],not MEM_MASK
        or      es:d[(1024*4096*1022)+edx*4],MEM_FREE
        and     es:d[(1024*4096*1023)+edx*4],not (1 shl 6)
        inc     edx
        dec     ecx
        jnz     @@s0
        call    EMUCR3Flush
        jmp     @@RetNewAddr
        ;
@@Bigger:       ;Want to expand the block so get on with it.
        ;
        ;ECX - New size in pages.
        ;EBP - Current size in pages.
        ;ESI - Current start page.
        ;
@@b0:   mov     edx,esi
        add     edx,ebp         ;move to end of this block.
        mov     ebx,LinearLimit
        shr     ebx,12
        sub     ebx,edx         ;get pages to end of memory.
        or      ebx,ebx
        jz      @@Extend
        ;
        ;See if the next block is free.
        ;
        mov     eax,es:d[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE
        jnz     @@NewBlock              ;no hope so get a new block.
        ;
        ;Check how big this next block is.
        ;
        mov     edi,ebp         ;use current size as basis.
@@b1:   mov     eax,es:d[(1024*4096*1022)+edx*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE
        jnz     @@NewBlock              ;No hope so get a new block.
        inc     edi
        cmp     edi,ecx         ;Got enough yet?
        jz      @@MarkAndRet
        inc     edx
        dec     ebx
        jnz     @@b1            ;keep trying.
        ;
        ;Reached the end of the memory map so try extending it.
        ;
        pushad
        sub     ecx,edi         ;pages still needed.
        call    ExtendLinearMemory
        popad
        jc      @@error
        jmp     @@MarkAndRet
        ;
@@Extend:       ;Need to extend the memory map to provide a block of free memory
        ;after the current block.
        ;
        pushad
        sub     ecx,ebp         ;pages needed.
        call    ExtendLinearMemory
        popad
        jc      @@error
        ;
@@MarkAndRet:   ;Mark the new memory as in use and exit.
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
@@mr0:  and     es:d[(1024*4096*1022)+edx*4],not MEM_MASK
        or      es:d[(1024*4096*1022)+edx*4],MEM_END
        inc     edx
        dec     ecx
        jnz     @@mr0
        jmp     @@RetNewAddr
        ;
@@NewBlock:     ;Nothing for it but to try and allocate a new block of memory.
        ;
        pushm   ecx,ebp,esi
        shl     ecx,12
        mov     ebx,ecx
        shr     ebx,16
        call    RawGetMemory
        pushf
        shl     ebx,16
        mov     bx,cx
        popf
        popm    ecx,ebp,esi
        jc      @@error
        ;
        ;Copy current block to new block.
        ;
        pushad
        mov     ecx,ebp
        shl     ecx,12
        shl     esi,12
        mov     edi,ebx
        pushm   ds,es
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
        ;Use new block in place of origional.
        ;
        mov     esi,ebx
        shr     esi,12
        ;
@@RetNewAddr:   ;Return possibly new address/handle to caller.
        ;
        shl     esi,12          ;Get a real address again and
        mov     di,si           ;use it as both the memory
        mov     cx,si           ;address to return and the handle.
        shr     esi,16
        mov     bx,si
        clc
        jmp     @@exit
        ;
@@error:        stc
@@exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    eax,edx,ebp,ds,es
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
        call    RAWCopyCheck
        ;
        pushad
        pushm   ds,es
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
        pushm   ebx,ecx
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
        jnz     @@error
        cmp     esi,LinearBase  ;inside logical memory map?
        jc      @@error
        cmp     esi,LinearLimit
        jnc     @@error
        shr     esi,12          ;Get page number.
        mov     eax,es:d[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_START
        jnz     @@error
        ;
        ;Mark all this blocks pages as free.
        ;
        inc     FreePages
        dec     medAllocPages
        ;
        and     es:d[1024*4096*1022+esi*4],not MEM_MASK
        or      es:d[1024*4096*1022+esi*4],MEM_FREE
        and     es:d[1024*4096*1023+esi*4],not (1 shl 6)
        inc     esi
        mov     ecx,LinearLimit
        shr     ecx,12
        sub     ecx,esi         ;Get pages remaining.
        jz      @@1
@@0:    mov     eax,es:d[1024*4096*1022+esi*4]
        and     eax,MEM_MASK
        cmp     eax,MEM_END
        jnz     @@1
        inc     FreePages
        dec     medAllocPages
        and     es:d[1024*4096*1022+esi*4],not MEM_MASK
        or      es:d[1024*4096*1022+esi*4],MEM_FREE
        and     es:d[1024*4096*1023+esi*4],not (1 shl 6)
        inc     esi
        dec     ecx
        jnz     @@0
        ;
@@1:    call    EMUCR3Flush
        ;
        clc
        jmp     @@exit
        ;
@@error:        stc
@@exit:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    ds,es
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
        pushm   ds,es
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
        jnc     @@8
        sub     esi,eax
;
;Round length down a page.
;
        and     esi,not 4095
        or      esi,esi
        jz      @@8
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
@@0:    cmp     edx,LinearBase
        jc      @@1
        cmp     edx,LinearLimit
        jnc     @@2
        and     es:d[esi+ebx*4],not ((1 shl 6)+(1 shl 11)) ;clear dirty & disk bits.
@@1:    add     edx,4096
        inc     ebx
        dec     ecx
        jnz     @@0
;
@@2:    call    EMUCR3Flush             ;update page cache.
;
@@8:    clc
        popad
        assume ds:_cwDPMIEMU
        popm    ds,es
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
        pushm   ds,es
        pushm   eax,edx,esi,edi,ebp
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
        jnz     @@9
;
;Get length as number of pages.
;
        shl     esi,16
        mov     si,di
        add     esi,4095
        shr     esi,12
        test    esi,esi
        jz      @@9

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
        jc      @@8
;
;Find first un-used physical mapping space.
;
        mov     ebp,PageDirLinear
        mov     eax,1021
@@0:

; MED 04/18/96
        mov     ecx,es:[ebp+eax*4]
        and     ecx,(NOT 4095)  ; get linear address
        cmp     ecx,ebx                 ; see if matches desired linear address
        jne     med2                    ; no, continue as before
        mov     edi,eax
        shl     edi,22                  ; convert index to 4M space
        jmp     @@8

med2:
        cmp     es:d[ebp+eax*4],0
        jz      @@1

;       dec     eax
        sub     eax,1
        jc      @@9

        jmp     @@0
;
;Work out how many page tables we need and set first tables index.
;
@@1:    mov     ecx,esi
        shr     ecx,10
        sub     eax,ecx
        inc     ecx
;
;Make sure we can get enough memory for physical page tables.
;
        call    PhysicalGetPages
        add     edx,NoneLockedPages
        cmp     edx,ecx
        jc      @@9
;
;Put all the page tables into place.
;
        lea     edi,[ebp+eax*4]
        push    edi
        mov     ebp,ecx
@@2:    call    PhysicalGetPage
        jnc     @@3
        call    UnMapPhysical
        jc      @@10
@@3:    and     ecx,1           ;put user bits in useful place.
        shl     ecx,10
        and     edx,0FFFFFFFFh-4095     ;lose user bits.
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     es:d[edi],edx   ;store this tables address.
        push    edi
        sub     edi,PageDIRLinear
        add     edi,PageAliasLinear     ;get alias table address.
        mov     es:d[edi],edx   ;setup in alias table as well.
        pop     edi
        ;
        ;Clear this page to 0.
        ;
        pushm   ecx,edi
        sub     edi,PageDIRLinear
        shl     edi,10
        add     edi,1024*4096*1023      ;base of page alias's.
        mov     ecx,4096/4
        xor     eax,eax
        cld
        rep     stosd
        popm    ecx,edi
        ;
        add     edi,4
        dec     ebp
        jnz     @@2
        pop     edi
;
;Now map specified physical address range into place.
;
        sub     edi,PageDIRLinear
        shl     edi,10          ;start of first page table
        push    edi
        add     edi,1024*4096*1023
        or      ebx,111b
@@4:    mov     es:[edi],ebx
        add     edi,4
        add     ebx,4096
        dec     esi
        jnz     @@4
        pop     edi
;
;Return linear address to caller.
;
        shl     edi,12-2
@@8:    xor     ecx,ecx
        xor     ebx,ebx
        mov     cx,di
        shr     edi,16
        mov     bx,di
        clc
        jmp     @@11
;
@@10:   pop     edi
@@9:    stc
@@11:   popm    eax,edx,esi,edi,ebp
        assume ds:_cwDPMIEMU
        popm    ds,es
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
        call    RAWCopyCheck
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
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
        pushm   ebx,ecx
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
        jnc     @@10
        ;
        ;Count number of pages we need for this range.
        ;
        mov     d[_LM_Needed],0
        mov     eax,NoneLockedPages
        mov     d[_LM_Got],eax
        mov     eax,d[_LM_BlockBase]
@@04:   cmp     eax,LinearBase  ;must be in our memory pool.
        jc      @@05
        cmp     eax,LinearLimit
        jnc     @@05
        push    eax
        call    GetPageStatus
        pop     eax
        jc      @@15            ;ignore not present tables.
        test    edx,1
        jz      @@005           ;already present.
        call    RawPageLocked   ;locked page?
        jnz     @@05
        dec     d[_LM_Got]              ;reduce available pages.
        jmp     @@05
@@005:  inc     d[_LM_Needed]
@@05:   add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      @@04
;
;Check if we actually need any more pages to lock this region.
;
        cmp     d[_LM_Needed],0
        jz      @@OK
;
;If VMM isn't active then pages can always be locked assumeing they exist.
;
        cmp     VMMHandle,0
        jnz     @@VMM
        mov     eax,d[_LM_Needed]
        cmp     eax,d[_LM_Got]
        jc      @@OK
        jz      @@OK
        jmp     @@15
;
;VMM is active and pages are required so we need to make sure enough pages are
;left for swapping.
;
@@VMM:  mov     eax,d[_LM_Needed]
        add     eax,16          ;arbitrary safety buffer.
        cmp     eax,d[_LM_Got]
        jc      @@OK
        jz      @@OK
        jmp     @@15
;
;Enough free pages so lock the region.
;
@@OK:   mov     eax,d[_LM_BlockBase]
@@4:    cmp     eax,LinearBase  ;must be in our memory pool.
        jc      @@5
        cmp     eax,LinearLimit
        jnc     @@5
        push    eax
        call    GetPageStatus
        pop     eax
        jc      @@15            ;ignore not present tables.
        test    edx,1           ;is it present?
        jnz     @@6
        ;
@@11:   ;Need to allocate a physical page first.
        ;
        push    eax
        call    UnMapPhysical
        pop     eax
        jc      @@15            ;this shouldn't happen.
        mov     LinearEntry,eax
        shr     LinearEntry,12  ;store page number to allocate at.
        push    eax
        call    MapPhysical             ;map this page in.
        pop     eax
        ;
@@6:    ;Now mark this page as locked.
        ;
        call    RawLockPage
        ;
@@5:    add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      @@4
        ;
@@10:   clc
        jmp     @@1
        ;
@@15:   stc
        ;
@@1:
        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
        assume ds:_cwDPMIEMU
RAWLockMemory   endp


;-------------------------------------------------------------------------------
RAWUnLockMemory proc near
        call    RAWCopyCheck
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
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
        pushm   ebx,ecx
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
@@4:    cmp     eax,LinearBase  ;must be in our memory pool.
        jc      @@5
        cmp     eax,LinearLimit
        jnc     @@5
        push    eax
        call    GetPageStatus
        pop     eax
        jc      @@5             ;ignore not present tables.
        test    edx,1           ;is it present?
        jz      @@5
        call    RawUnLockPage   ;unlock the page.
@@5:    add     eax,4096
        cmp     eax,d[_LM_BlockEnd]     ;done them all yet?
        jc      @@4
        clc
@@1:

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
        assume ds:_cwDPMIEMU
RAWUnLockMemory endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
RAWGetMemoryMax proc near
;
;Work out biggest memory block remaining.
;
        call    RAWCopyCheck
        ;
        pushm   eax,ecx,edx,esi,edi,ebp,ds,es
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
        pushm   ebx,ecx
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
@@l0:   ;Look for a bigest block of free memory.
        ;
        mov     eax,es:d[(1024*4096*1022)+edx*4] ;Get page details.
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE            ;Free block?
        jnz     @@l2
        or      edi,edi         ;Got any yet?
        jnz     @@l1
        mov     esi,edx         ;Get base page number.
@@l1:   inc     edi
        cmp     edi,ebp         ;Biggest yet?
        jc      @@l3
        mov     ebx,esi         ;Get base.
        mov     ebp,edi         ;Get size.
        jmp     @@l3
@@l2:   xor     edi,edi
@@l3:   inc     edx             ;Next page.
        dec     ecx
        jnz     @@l0
        ;
        ;See if biggest block found is the last block in the map.
        ;
        xor     edx,edx         ;reset end of chain value.
        or      edi,edi         ;last block free?
        jz      @@l4
        cmp     ebx,esi         ;same base?
        jnz     @@l4
        mov     edx,ebp         ;setup extra block size.
        mov     ebp,0           ;reset normal block size.
        jmp     @@l5
        ;
@@l4:   ;Get size of the last block in the memory map.
        ;
        mov     eax,LinearBase
        mov     esi,LinearLimit
        shr     esi,12
        dec     esi
        shr     eax,12
        mov     ecx,esi
        sub     ecx,eax
@@l6:   jecxz   @@l5
        mov     eax,es:d[(1024*4096*1022)+esi*4] ;Get page details.
        and     eax,MEM_MASK
        cmp     eax,MEM_FREE            ;Free block?
        jnz     @@l5
        dec     esi
        inc     edx
        dec     ecx
        jmp     @@l6
        ;
@@l5:   ;See what extra memory we can get hold of.
        ;
        mov     ebx,edx
        call    PhysicalGetPages        ;Get extra memory available.
        mov     ecx,ebx         ;Save origional value.
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
        jz      @@l8
        mov     ebx,ecx
        pushm   ebx,ebp
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
        jz      @@l7
        mul     cx              ;Get bytes per cluster.
        mul     bx              ;Get bytes available.
        shl     edx,16
        mov     dx,ax
@@l7:   ;
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
        popm    ebx,ebp
        ;
        ;Work out how much of the VMM space is extra.
        ;
        mov     eax,LinearLimit
        sub     eax,LinearBase
        shr     eax,12
        sub     edx,eax
        add     ebx,edx

        ;
@@l8:   ;Check which block is bigger and exit.
        ;
        push    ecx
        mov     eax,ebx
        shl     eax,12
        mov     ecx,LinearLimit
        sub     ecx,LinearBase
        sub     eax,ecx
        js      @@l89
        cmp     eax,MaxMemLin
        jc      @@l89
        mov     ebx,MaxMemLin
        sub     ebx,ecx
        shr     ebx,12
@@l89:  pop     ecx

        cmp     ebx,ebp
        jnc     @@l9
        mov     ebx,ebp
@@l9:   shl     ebx,12
        clc

        lss     esp,f[esp]
        pushf
        add     d[RawStackPos],RawStackDif
        popf

        popm    eax,ecx,edx,esi,edi,ebp,ds,es
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
        pushm   ds,es
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
        jnc     @@error

        ;
        ;Try extending useing physical memory first.
        ;
@@f0:   mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        shr     eax,10          ;/1024 for page dir entry.
        mov     edi,PageDirLinear       ;get page table address.
        mov     eax,es:d[edi+eax*4]     ;this page present?
        test    eax,1           ;do we have a page table?
        jnz     @@f1            ;keep going till we do.
;
;No page table so make sure we can get 2 pages (page and det)
;
        call    PhysicalGetPages
        cmp     edx,2
        jc      @@virtual
;
;both pages available so go to it.
;
@@f2:   call    PhysicalGetPage ;get a page.
        jc      @@error         ;it lied.
        mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        push    LinearEntry
        call    MapPhysical             ;use this page for page table.
        call    PhysicalGetPage ;get a page.
        pop     LinearEntry
        jc      @@error         ;it lied.
        call    MapPhysical             ;use this page for page table.
        ;
@@f1:   call    PhysicalGetPage ;get a page.
        jc      @@Virtual               ;use virtual memory.
        mov     eax,LinearLimit ;get new entry number.
        shr     eax,12          ;page number.
        mov     LinearEntry,eax
        call    MapPhysical             ;use this page for page table.
        ;
        ;Update details.
        ;
        mov     eax,LinearLimit
        shr     eax,12
        mov     es:d[(1024*4096*1022)+eax*4],0
        inc     FreePages
;       dec     medAllocPages
        inc     TotalPages
        add     LinearLimit,4096        ;bump up the end of the memory map.
        dec     ebp             ;update the counter.
        jnz     @@f0            ;keep looking.
        clc
        jmp     @@exit          ;All physical so exit.
        ;
@@Virtual:      ;Virtual memory will be needed so see if we can provide any.
        ;
        cmp     VMMHandle,0             ;Virtual memory active?
        jz      @@error
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
        jz      @@error
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
        jc      @@error
        ;
        ;Find out how many un-locked pages we currently have.
        ;
        mov     edx,NoneLockedPages
        ;
        ;Enough page's for minimum requirement?
        ;
        cmp     edx,16          ;un-locked pages < 16?
        jc      @@error         ;force minimum of 16.
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
        jz      @@DoneTables
        add     ebx,ebx         ;Page + Det
        mov     ecx,ebp
        cmp     ebx,edx
        jc      @@OK
        jz      @@OK
        jmp     @@error
        ;
@@OK:   ;Allocate new page tables.
        ;
        mov     esi,PageDirLinear       ;get page directory address.
        mov     edx,LinearLimit
        shr     edx,12
@@v2:   mov     eax,edx         ;get new entry number.
        shr     eax,10          ;/1024 for page dir entry.
        test    es:d[esi+eax*4],1       ;this page present?
        jnz     @@v3
        ;
        ;get DET page.
        ;
        pushm   ecx,edx
        call    UnMapPhysical   ;get a physical page for us to use.
        mov     eax,edx
        mov     ebx,ecx
        popm    ecx,edx
        jc      @@error         ;not enough physical memory to support virtual memory.
        pushm   ecx,edx
        mov     LinearEntry,edx ;set logical page address.
        mov     edx,eax         ;get physical address again.
        mov     ecx,ebx
        call    MapPhysical             ;use this to add a new page table.
        popm    ecx,edx
        ;
        ;And again for page table.
        ;
        pushm   ecx,edx
        call    UnMapPhysical   ;get a physical page for us to use.
        mov     eax,edx
        mov     ebx,ecx
        popm    ecx,edx
        jc      @@error         ;not enough physical memory to support virtual memory.
        pushm   ecx,edx
        mov     LinearEntry,edx ;set logical page address.
        mov     edx,eax         ;get physical address again.
        mov     ecx,ebx
        call    MapPhysical             ;use this to add a new page table.
        popm    ecx,edx
@@v3:   inc     edx
        dec     ecx
        jnz     @@v2
        ;
@@DoneTables:   ;Now mark all the new pages as un-locked/free
        ;
        mov     eax,LinearLimit
        mov     ecx,ebp
@@v4:   call    RawClearPageLock        ;clear page locking for this entry.
        push    eax
        shr     eax,12
        mov     es:d[1024*4096*1022+eax*4],0
        pop     eax
        add     eax,4096
        dec     ecx
        jnz     @@v4
        ;
        ;Extend the swap file.
        ;
        mov     ecx,LinearLimit ;current end position.
        sub     ecx,LinearBase  ;length.
        mov     eax,ebp         ;extension needed in pages.
        shl     eax,12
        add     ecx,eax         ;New extremity desired.
        cmp     ecx,SwapFileLength
        jc      @@Extended
        add     ecx,65535
        and     ecx,not 65535
        push    ecx
        mov     dx,cx
        shr     ecx,16
        mov     bx,VMMHandle            ;get swap file handle.
        mov     ax,4200h
        mov     edi,offset PageInt
        push    edi
        mov     Real_EAX[edi],eax
        mov     Real_EBX[edi],ebx
        mov     Real_ECX[edi],ecx
        mov     Real_EDX[edi],edx
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        mov     ah,40h
        mov     bx,VMMHandle            ;get swap file handle.
        mov     cx,0
        push    edi
        mov     edi,offset PageInt
        mov     Real_EAX[edi],eax
        mov     Real_EBX[edi],ebx
        mov     Real_ECX[edi],ecx
        mov     Real_EDX[edi],edx
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        pop     ecx
        test    Real_Flags[edi],1
        jnz     @@disk_error
        mov     SwapFileLength,ecx
        ;
        xor     cx,cx
        xor     dx,dx
        mov     ax,4201h
        mov     bx,VMMHandle
        push    edi
        mov     edi,offset PageInt
        mov     Real_EAX[edi],eax
        mov     Real_EBX[edi],ebx
        mov     Real_ECX[edi],ecx
        mov     Real_EDX[edi],edx
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        pop     edi
        test    Real_Flags[edi],1
        jnz     @@disk_error
        mov     edx,Real_EDX[edi]
        mov     eax,Real_EAX[edi]
        shl     edx,16
        mov     dx,ax
        cmp     edx,SwapFileLength
        jnz     @@disk_error
        ;
@@Extended:     ;Update the end of the memory map.
        ;
        add     FreePages,ebp
;       sub     medAllocPages,ebp
        shl     ebp,12
        add     LinearLimit,ebp
        clc
        jmp     @@Exit
        ;
@@error:        stc
@@Exit: ;
        popm    ds,es
        popad
        ret
        ;
@@Disk_Error:   jmp     @@error
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
        pushm   ds,es
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
        test    es:d[esi+eax*4],1       ;this page present?
        jnz     @@AddPage
        ;
        cmp     PageDETLinear,0 ;DET in use yet?
        jz      @@AddTable
        mov     esi,PageDETLinear       ;get page table address.
        test    es:d[esi+eax*4],1       ;DET page present?
        jnz     @@AddTable
        ;
@@AddDET:       ;Need a new DET page.
        ;
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     es:d[esi+eax*4],edx     ;store this tables address.
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
        jmp     @@Finished
        ;
@@AddTable:     ;Need a new page table.
        ;
        mov     eax,LinearEntry ;get new entry number.
        shr     eax,10          ;/1024 for page dir entry.
        mov     esi,PageDirLinear       ;get page table address.
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     es:d[esi+eax*4],edx     ;store this tables address.
        mov     esi,PageAliasLinear     ;get alias table address.
        mov     es:d[esi+eax*4],edx     ;setup in alias table as well.
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
        jmp     @@Finished
        ;
@@AddPage:      ;Update recent page usage stack.
        ;
        pushm   ecx,es,ds
        pop     es
        mov     edi,offset RecentMapStack+(4*PageStackSize)-4
        mov     esi,offset RecentMapStack+(4*PageStackSize)-8
        mov     ecx,PageStackSize-1
        std
        rep     movsd
        popm    ecx,es
        cld
        mov     eax,LinearEntry
        shl     eax,12
        mov     RecentMapStack,eax
        ;
        ;Add this to the relavent page table.
        ;
        mov     eax,LinearEntry ;get the entry number again.
        mov     esi,1024*4096*1023      ;base of page alias's.
        mov     ebx,es:d[esi+eax*4]     ;get current details.
        and     ebx,1 shl 11
        or      edx,ebx
        or      edx,111b                ;present+user+write.
        or      edx,ecx         ;set use flags.
        mov     es:d[esi+eax*4],edx     ;set physical address.
        ;
        cmp     PageDETLinear,0
        jz      @@NoLocking
        mov     eax,LinearEntry
        shl     eax,12
        call    RawClearPageLock        ;clear page locking for this entry.
        ;
        ;Update number of un-locked physical pages present.
        ;
        inc     NoneLockedPages
        ;
@@NoLocking:    ;Check if this page needs fetching from swap space.
        ;
        cmp     VMMHandle,0
        jz      @@NoRead
        ;
        test    ebx,1 shl 11
        jz      @@NoRead
        ;
        mov     esi,BreakAddress
        mov     al,es:[esi]
        mov     es:b[esi],0
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
        mov     Real_EBX[edi],ebx
        mov     Real_EAX[edi],eax
        mov     Real_EDX[edi],edx
        mov     Real_ECX[edi],ecx
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        pushm   es,ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        mov     edi,offset PageInt
        mov     ax,VMMHandle
        mov     Real_EBX[edi],eax
        mov     ax,PageBufferReal
        mov     Real_DS[edi],ax
        mov     Real_EAX[edi],3f00h
        mov     Real_EDX[edi],0
        mov     Real_ECX[edi],4096
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateInt       ;read it from disk.
        pop     es
        test    Real_Flags[edi],1
        jz      @@ok
        mov     esi,BreakAddress
        pop     eax
        mov     es:[esi],al
        jmp     @@Finished2
        ;
@@ok:   mov     esi,BreakAddress
        pop     eax
        mov     es:[esi],al
        ;
        mov     esi,PageBufferLinear
        mov     edi,LinearEntry
        shl     edi,12          ;get linear address again.
        mov     ecx,4096/4
        pushm   ds,es
        pop     ds
        cld
        rep     movsd           ;copy back into place.
        pop     ds
        ;
        mov     eax,LinearEntry ;get new entry number.
        mov     esi,1024*4096*1023      ;base of page alias's.
        and     es:d[esi+eax*4],0FFFFFFFFh-(3 shl 5)    ;clear accesed & dirty bits.
        call    EMUCR3Flush
        ;
@@NoRead:       inc     LinearEntry             ;update counter.
        ;
@@Finished:     clc
        ;
@@Finished2:    popm    ds,es
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
        pushm   eax,ebx,esi,edi,ebp,ds,es,fs
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
@@ScanLoop:     dec     ebx
        jnz     @@80            ;shit, we've been all the way round.
        ;
        cmp     ProposedPresentFlag,0
        jnz     @@UseProposed
        jmp     @@8
        ;
@@80:   cmp     ecx,LinearBase
        jnc     @@80_0
        mov     ecx,LinearBase
        sub     ecx,4096
@@80_0: add     ecx,4096
        cmp     ecx,LinearLimit ;End of memory map yet?
        jc      @@NoWrap
        mov     ecx,LinearBase
        ;
@@NoWrap:       mov     eax,ecx
        shr     eax,12          ;get page number.
        test    fs:d[edi+eax*4],1       ;this page present?
        jz      @@ScanLoop
        test    fs:d[ebp+eax*4],MEM_LOCK_MASK shl MEM_LOCK_SHIFT
        jnz     @@ScanLoop
        ;
        inc     CompareCount
        ;
        ;Check against recent stack.
        ;
        pushm   ecx,edi
        mov     eax,ecx
        mov     edi,offset RecentMapStack
        mov     ecx,PageStackSize
        repnz   scasd
        popm    ecx,edi
        jz      @@IsRecent
        shr     eax,12          ;get page number again.
        test    fs:d[edi+eax*4],1 shl 6
        jz      @@GotPage
        ;
        cmp     ProposedPresentFlag,0
        jz      @@SetProposed
        cmp     ProposedRecentFlag,0
        jz      @@UseProposed?
@@SetProposed:  mov     ProposedPresentFlag,-1
        mov     ProposedPage,ecx
        mov     CompareCount,0
        mov     ProposedRecentFlag,0
        jmp     @@ScanLoop
        ;
@@UseProposed?: mov     eax,NoneLockedPages
        shr     eax,2
        cmp     eax,4096
        jc      @@UP0
        mov     eax,4096
@@UP0:  cmp     CompareCount,eax
        jc      @@UseProposed
        jmp     @@ScanLoop
        ;
@@IsRecent:     cmp     ProposedPresentFlag,0
        jnz     @@ProposedRecent?
        mov     ProposedPresentFlag,-1
        mov     ProposedPage,ecx
        mov     ProposedRecentFlag,-1
        mov     CompareCount,0
        jmp     @@ScanLoop
        ;
@@ProposedRecent?:
        cmp     ProposedRecentFlag,0
        jnz     @@LookedEnough?
        mov     eax,NoneLockedPages
        shr     eax,2
        cmp     eax,4096
        jc      @@PR0
        mov     eax,4096
@@PR0:  cmp     CompareCount,eax
        jnc     @@UseProposed
        jmp     @@ScanLoop
        ;
@@LookedEnough?:
        mov     eax,NoneLockedPages
        cmp     CompareCount,eax
        jnc     @@UseProposed
        jmp     @@ScanLoop
        ;
@@UseProposed:  mov     ecx,ProposedPage
        ;
@@GotPage:      mov     PageingPointer,ecx
        ;
        mov     eax,ecx
        shr     eax,12          ;get page number again.
        shl     eax,2
        add     edi,eax
        ;
        ;Check if it needs to go to the swap file.
        ;
        test    fs:d[edi],1 shl 6       ;is it dirty?
        jz      @@5             ;no need to write it if not.
        ;
        ;Flush this page to disk.
        ;
        push    edi
        mov     esi,BreakAddress
        mov     al,fs:[esi]
        mov     fs:b[esi],0
        push    eax
        ;
        sub     edi,1024*4096*1023      ;get page table entry number.
        shr     edi,2           ;page number.
        shl     edi,12          ;get linear address.
        push    edi
        mov     esi,edi
        pushm   ds,es
        mov     edi,PageBufferLinear    ;copy it to somewhere we can deal with it.
        mov     ax,KernalZero
        mov     ds,ax
        mov     es,ax
        mov     ecx,4096/4
        rep     movsd
        popm    es,ds
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
        mov     Real_EBX[edi],ebx
        mov     Real_EAX[edi],eax
        mov     Real_EDX[edi],edx
        mov     Real_ECX[edi],ecx
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        push    ds
        pop     es
        call    EMURawSimulateInt       ;move to right place.
        ;
        test    Real_Flags[edi],1
        stc
        jnz     @@error_anyway
        mov     edx,Real_EDX[edi]
        mov     eax,Real_EAX[edi]
        shl     edx,16
        mov     dx,ax
        cmp     edx,ebp
        jnz     @@force_error
        ;
        mov     edi,offset PageInt
        mov     ax,VMMHandle
        mov     Real_EBX[edi],eax
        mov     ax,PageBufferReal
        mov     Real_DS[edi],ax
        mov     Real_EAX[edi],4000h
        mov     Real_EDX[edi],0
        mov     Real_ECX[edi],4096
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateInt       ;write it to disk.
        test    Real_Flags[edi],1
        stc
        jnz     @@error_anyway
        mov     eax,Real_EAX[edi]
        cmp     ax,4096
        jz      @@error_anyway
@@force_error:  stc
@@error_anyway: ;
        mov     esi,BreakAddress
        pop     eax
        mov     fs:[esi],al
        ;
        pop     edi
        jc      @@8
        or      fs:d[edi],1 shl 11      ;signal it living on disk.
        ;
@@5:    ;Now remove it from the page table and exit.
        ;
        and     fs:d[edi],0FFFFFFFFh-1  ;mark as not present.
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
        jmp     @@9
        ;
@@8:    stc                     ;failed to find free page.
        ;
@@9:    popm    eax,ebx,esi,edi,ebp,ds,es,fs
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
        pushm   eax,ebx,esi,es
        shr     eax,12          ;get page number.
        mov     bx,KernalZero
        mov     es,bx
        mov     esi,1024*4096*1022      ;base of page DET's.
        mov     ebx,es:[esi+eax*4]
        shr     ebx,MEM_LOCK_SHIFT
        and     ebx,MEM_LOCK_MASK
        jnz     @@WasLocked
        ;
        ;Update number of un-locked physical pages present.
        ;
        pushm   ax,ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        dec     NoneLockedPages
        assume ds:_cwDPMIEMU
        popm    ax,ds
        ;
@@WasLocked:    cmp     ebx,MEM_LOCK_MASK
        jz      @@0
        add     es:d[esi+eax*4],1 shl MEM_LOCK_SHIFT    ;lock it.
@@0:    popm    eax,ebx,esi,es
        ret
RawLockPage     endp


;-------------------------------------------------------------------------------
RawClearPageLock proc near
        pushm   eax,ebx,ecx,esi,es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12
        mov     esi,1024*4096*1022      ;base of page alias's.
        mov     ebx,MEM_LOCK_MASK shl MEM_LOCK_SHIFT
        xor     ebx,-1
        and     es:d[esi+eax*4],ebx     ;un-lock it.
        popm    eax,ebx,ecx,esi,es
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
        jz      @@9
        pushm   eax,ebx,esi,es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12
        mov     esi,1024*4096*1022      ;base of page alias's.
        sub     es:d[esi+eax*4],1 shl MEM_LOCK_SHIFT    ;un-lock it.
        mov     eax,es:d[esi+eax*4]
        shr     eax,MEM_LOCK_SHIFT
        and     eax,MEM_LOCK_MASK
        jnz     @@NotUnLocked
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
@@NotUnLocked:  popm    eax,ebx,esi,es
@@9:    ret
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
        pushm   eax,ebx,esi,es
        mov     bx,KernalZero
        mov     es,bx
        shr     eax,12          ;get page number.
        mov     esi,1024*4096*1022      ;base of page alias's.
        mov     ebx,es:d[esi+eax*4]
        shr     ebx,MEM_LOCK_SHIFT
        and     ebx,MEM_LOCK_MASK
        popm    eax,ebx,esi,es
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
        pushm   eax,ebx,esi,ds,es
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
        test    es:d[esi+eax*4],1       ;page table present?
        pop     eax
        jz      @@8
        mov     esi,4096*1024*1023      ;base of page alias memory.
        shr     eax,12          ;get page number.
        mov     edx,es:[esi+eax*4]      ;get page details.
        clc
        jmp     @@9
        ;
@@8:    xor     edx,edx
        stc
@@9:    popm    eax,ebx,esi,ds,es
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
        pushm   eax,ecx,edx,ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     eax,ExceptionCR2
        mov     cx,KernalDS
        mov     ds,cx
        assume ds:_cwRaw
        cmp     eax,LinearBase  ;address below our memory space?
        jc      @@OldExit
        cmp     eax,LinearLimit ;address above our memory space?
        jnc     @@OldExit
        ;
        push    eax             ;save the linear address.
        call    UnMapPhysical   ;retrieve a physical page to use.
        pop     eax
        jc      @@Disk_Error            ;This should only happen on disk errors.
        shr     eax,12
        push    LinearEntry
        mov     LinearEntry,eax ;setup linear address we want to map.
        call    MapPhysical             ;map new page into faulting linear address space.
        pop     LinearEntry
        jc      @@Disk_Error            ;This should only happen on disk errors.
        ;
        popm    eax,ecx,edx,ds
        assume ds:nothing
        test    cs:DpmiEmuSystemFlags,1 ;check exit size.
        assume ds:_cwRaw
        jz      @@Use32Bit3
        db 66h
        retf                    ;16 bit exit.
@@Use32Bit3:    ;
        retf
        ;
@@disk_error:   jmp     @@OldExit
        ;
        assume ds:nothing
@@OldExit:      popm    eax,ecx,edx,ds
        jmp     cs:f[OldExcep14]        ;32 bit chaining.
OldExcep14      df ?
        assume ds:_cwDPMIEMU
VirtualFault    endp


;-------------------------------------------------------------------------------
;
;Allocate a block of DOS memory & provide a selector to access it with.
;
RawGetDOSMemory proc near
        pushm   ecx,esi,edi,ebp,ds,es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        cmp     bx,-1           ;maximum?
        jz      @@0
        inc     ebx             ;para extra for us.
@@0:    ;
        push    ebx
        mov     edi,offset MemIntBuffer
        push    ds
        pop     es
        mov     Real_EAX[edi],4800h     ;get memory.
        mov     Real_EBX[edi],ebx       ;memory size.
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;allocate it.
        pop     ecx
        mov     eax,Real_EAX[edi]       ;get result.
        mov     ebx,Real_EBX[edi]
        test    Real_Flags[edi],1
        jz      @@1
        or      bx,bx           ;nothing available?
        jz      @@9
        dec     ebx             ;leave space for us!
        jmp     @@9
        ;
@@1:    ;Now try and allocate enough selectors.
        ;
        pushm   eax,ecx
        dec     cx              ;lose our para.
        shr     cx,12           ;get number of 64k chunks.
        inc     cx              ;+1 for base.
        call    RawGetDescriptors
        popm    ebx,ecx
        jc      @@8
        ;
        ;Store block size.
        ;
        push    ecx
        mov     dx,KernalZero
        mov     es,dx
        movzx   esi,bx          ;get segment address.
        shl     esi,4           ;get linear address.
        mov     es:w[esi],cx            ;store block size.
        dec     es:w[esi]               ;lose our para.
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
@@2:    pushm   eax,ebx,ecx,eax
        mov     edi,offset RawSelBuffer
        add     edi,7
        and     edi,not 7
        movzx   esi,bx
        shl     esi,4
        movzx   ecx,cx
        shl     ecx,4
        dec     ecx
        mov     al,0
        mov     ah,DescPresent+DescPL3+DescMemory+DescRWData
        call    EMUMakeDesc
        pop     ebx
        call    RawBPutDescriptor
        popm    eax,ebx,ecx
        add     eax,8           ;next selector.
        add     ebx,1000h               ;update segment base.
        movzx   ecx,cx
        sub     ecx,1000h               ;reduce segment size.
        jns     @@2             ;keep going till all done.
        ;
        pop     eax             ;Get base segment again.
        pop     edx             ;Get base selector again.
        clc
        jmp     @@10
        ;
@@8:    ;Release memory we managed to allocate.
        ;
        mov     edi,offset MemIntBuffer
        mov     ax,KernalDS
        mov     es,ax
        mov     es:Real_EAX[edi],4900h  ;release memory.
        mov     es:Real_ES[edi],bx      ;segment.
        mov     es:Real_SS[edi],0
        mov     es:Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;release it.
        ;
@@9:    stc
@@10:   popm    ecx,esi,edi,ebp,ds,es
        ret
        assume ds:_cwDPMIEMU
RawGetDOSMemory endp


;-------------------------------------------------------------------------------
;
;Re-size a block of DOS memory.
;
RawResDOSMemory proc near
        pushm   ecx,edx,esi,edi,ebp,ds,es,fs,gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        pushm   bx,dx
        mov     bx,dx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     esi,ecx
        sub     esi,16          ;back to our stuff.
        popm    bx,dx
        mov     ax,KernalZero
        mov     es,ax
        cmp     bx,es:w[esi]            ;shrinking or expanding?
        jz      @@8
        jnc     @@Expand
        ;
@@Shrink:       ;Attempt to shrink the memory block.
        ;
        pushm   ebx,edx
        inc     ebx             ;include our para.
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     eax,esi
        shr     eax,4           ;get real mode segment.
        mov     Real_EAX[edi],4a00h
        mov     Real_EBX[edi],ebx
        mov     Real_ES[edi],ax
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        mov     eax,Real_EAX[edi]
        mov     ebx,Real_EBX[edi]
        test    Real_Flags[edi],1
        popm    ecx,edx
        jnz     @@9             ;DOS failed it!
        ;
        ;Lose any selectors that are no longer needed.
        ;
        pushm   ecx,edx
        mov     ebx,edx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     esi,ecx
        sub     esi,16          ;back to our stuff.
        popm    ecx,edx
        pushm   ecx,edx
        mov     ax,KernalZero
        mov     es,ax
        mov     ax,es:w[esi]            ;get old size.
        mov     es:w[esi],cx            ;store new size.
        shr     ax,12
        inc     eax
        mov     ebx,eax         ;need existing number.
        shr     cx,12           ;get 64k chunks.
        inc     ecx             ;+ base.
        sub     ax,cx           ;get number of selectors to lose.
        jz      @@0
        popm    ecx,edx         ;need base selector again.
        pushm   ecx,edx
        shl     bx,3
        add     dx,bx           ;move to end of descriptors.
        movzx   ecx,ax
        shl     ax,3
        sub     edx,eax         ;reduce by number to lose.
        mov     ebx,edx
@@1:    pushm   ebx,ecx
        call    RawRelDescriptor        ;release this selector.
        popm    ebx,ecx
        add     ebx,8
        dec     ecx
        jnz     @@1
@@0:    popm    ecx,edx
        ;
        ;Re-set all selector base & limits to affect new last selector.
        ;
        pushm   ecx,edx
        mov     ebx,edx
        call    RawGetSelBase
        shl     ecx,16
        mov     cx,dx
        mov     ebx,ecx
        shr     ebx,4           ;get real mode segment.
        popm    ecx,edx
        mov     eax,edx
;       shr     ax,3            ;lose TI & RPL
;       shr     ax,3            ;get descriptor number.
        push    ds
        pop     es
        ;
@@2:    pushm   eax,ebx,ecx,eax
        mov     edi,offset RawSelBuffer
        add     edi,7
        and     edi,0fffffff8h
        movzx   esi,bx
        shl     esi,4
        movzx   ecx,cx
        shl     ecx,4
        dec     ecx
        mov     al,0
        mov     ah,DescPresent+DescPL3+DescMemory+DescRWData
        call    EMUMakeDesc
        pop     ebx
        call    RawBPutDescriptor
        popm    eax,ebx,ecx
        add     eax,8           ;next selector.
        add     ebx,1000h               ;update segment base.
        movzx   ecx,cx
        sub     ecx,1000h               ;reduce segment size.
        jns     @@2             ;keep going till all done.
        ;
@@8:    clc
        jmp     @@10
        ;
@@Expand:       ;Attempt to expand the memory block.
        ;
        mov     bx,es:w[esi]            ;return current length as maximum.
        mov     ax,8
@@9:    stc
@@10:   popm    ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
        assume ds:_cwDPMIEMU
RawResDOSMemory endp


;-------------------------------------------------------------------------------
;
;Release a block of DOS memory.
;
RawRelDOSMemory proc near

        pushm   eax,ebx,ecx
        mov     ebx,edx
        and     ebx,0ffffh-7
        xor     ecx,ecx
        xor     eax,eax
        mov     ax,ds
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@z0
        mov     ds,cx
@@z0:   mov     ax,es
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@z1
        mov     es,cx
@@z1:   mov     ax,fs
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@z2
        mov     fs,cx
@@z2:   mov     ax,gs
        and     eax,not 7
        cmp     eax,ebx
        jnz     @@z3
        mov     gs,cx
@@z3:   popm    eax,ebx,ecx

        pushm   ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es
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
        mov     cx,es:w[esi]            ;get block size.
        ;
        ;Release selectors.
        ;
@@0:    pushm   eax,ebx,ecx
        call    RawRelDescriptor
        popm    eax,ebx,ecx
        add     ebx,8           ;next descriptor.
        movzx   ecx,cx
        sub     ecx,1000h
        jns     @@0             ;release all selectors.
        ;
        ;Release DOS memory block.
        ;
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     Real_EAX[edi],4900h     ;release block.
        mov     Real_ES[edi],ax ;block to release.
        mov     Real_SS[edi],0
        mov     Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT       ;release it.
        mov     eax,Real_EAX[edi]
        test    Real_Flags[edi],1
        clc
        jz      @@noc
        stc
@@noc:  ;
        popm    ds,es
        popm    ebx,ecx,edx,esi,edi,ebp
        ret
RawRelDOSMemory endp
