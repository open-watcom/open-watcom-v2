.386p
.model small

        include cw.inc

.code

;****************************************************************************
;Load a module by file name. If the module already exists in memory a new
;version will still be loaded.
;
;Usage: GetModuleFileName(module_handle);
;
;Returns:
;
;   Module file name address.
;
;Module file name is a standard zero terminated string.
;
;****************************************************************************
        public GetModuleFileName
GetModuleFileName  proc    C USES ,handle:DWORD
        push    ebx
        push    ecx
        push    edx
;
;Get PSP linear address.
;
        mov     ebx,handle
        sys     GetSelDet32
        mov     eax,EPSP_Struc.EPSP_FileName
        add     eax,edx
        pop     edx
        pop     ecx
        pop     ebx
        ret
GetModuleFileName  endp

end
