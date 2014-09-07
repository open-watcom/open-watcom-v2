.386p
.model small

        include cw.inc

.code


;****************************************************************************
;Load a module by file name. If the module already exists in memory a new
;version will still be loaded.
;
;Usage: LoadLibrary(file_name);
;
;Returns:
;
;   zero on error else module handle.
;
;file_name is a standard zero terminated string.
;
;Handles returned by this function should always be released via FreeLibrary()
;
;****************************************************************************
        public LoadLibrary
LoadLibrary    proc C USES ,fname:DWORD
        LOCAL   handle:DWORD
        pushad
;
;Let cwLoad have a go at loading it.
;
        mov     edx,fname
        sys     cwLoad
        jnc     @@4
        xor     edx,edx
        jmp     @@5
;
;Store entry/exit address in EPSP.
;
@@4:    push    es
        mov     es,si
        mov     edi,offset EPSP_Struc.EPSP_EntryCSEIP
        mov     es:dword ptr [edi],edx
        mov     es:word ptr [edi+4],cx
        pop     es
;
;Get PSP linear address.
;
        mov     bx,si
        sys     GetSelDet32
;
;Call entry code.
;
        push    ds
        push    es
        push    fs
        push    gs
        pushad
        push    ebx
        pop     fs
        mov     ds,bx
        mov     es,bx
        xor     eax,eax
        mov     ax,es:word ptr[edi+4]
        lar     eax,eax
        test    eax,00400000h
        xor     eax,eax
        jnz     @@6
        db 66h
@@6:    call    fs:fword ptr[edi]
        or      ax,ax
        popad
        pop     gs
        pop     fs
        pop     ds
        pop     es
        jz      @@5
;
;Initialisation failed so release this PSP.
;
        mov     bx,si
        sys     RelMem
        xor     edx,edx
;
;Return handle (or error) to caller.
;
@@5:    mov     handle,edx          ;Use calling parameter space.
        popad
        mov     eax,handle          ;Get return value.
        ret
LoadLibrary    endp


;****************************************************************************
;Releases a LoadLibrary module handle back to the system.
;
;Usage: FreeLibrary(module_handle);
;
;Returns:
;
;   nothing.
;
;module_handle is the value returned by LoadLibrary();
;
;****************************************************************************
        public FreeLibrary
FreeLibrary     proc C USES ,handle:DWORD
        pushad
;
;Call terminate code.
;
        mov         edx,handle
        xor         ebx,ebx
        mov         bx,EPSP_Struc.EPSP_PSPSel[edx]
        push        ds
        push        es
        push        fs
        push        gs
        pushad
        push        ebx
        pop         fs
        mov         ds,bx
        mov         es,bx
        xor         eax,eax
        mov         edi,offset EPSP_Struc.EPSP_EntryCSEIP
        mov         ax,es:word ptr[edi+4]
        lar         eax,eax
        test        eax,00400000h
        mov         eax,1
        jnz         @@7
        db 66h
@@7:    call        fs:fword ptr[edi]
        popad
        pop         gs
        pop         fs
        pop         ds
        pop         es
;
;Release the module.
;
        mov         bx,EPSP_Struc.EPSP_PSPSel[edx]
        sys         RelSel
;
        popad
        ret
FreeLibrary     endp

end
