.386p
.model small

        extrn __CWAPI_FINDMODULE:far
        extrn __CWAPI_UNFINDMODULE:far

.code

;****************************************************************************
;Load a module by module name. If the module is already in memory then just
;return the handle for the existing copy.
;
;Usage: LoadModule(module_name);
;
;Returns:
;
;   zero on error else module handle.
;
;module_name is a standard zero terminated string.
;
;Handles returned by this function should always be released via FreeModule()
;
;****************************************************************************
        public  LoadModule
LoadModule      proc    C USES ,mod_name:DWORD
        LOCAL   handle:DWORD,NameSpace[257]:BYTE
        pushad
;
;Build the module name string in the format CauseWay API likes.
;
        mov         esi,mod_name        ;Point to module name
        mov         edi,esi
        or          ecx,-1
        xor         al,al
        cld
        repnz       scasb               ;get the strings length.
        not         ecx
        dec         ecx                 ;Don't include terminator.
        lea         edi,NameSpace
        mov         [edi],cl
        inc         edi
        rep         movsb
;
;Call API code.
;
        lea         esi,NameSpace
        call        __CWAPI_FINDMODULE
        jnc         @@0
        xor         edi,edi             ;Zero the handle
;
;Return handle (or error) to caller.
;
@@0:    mov         handle,edi          ;Use calling parameter space.
        popad
        mov         eax,handle          ;Get return value.
        ret
LoadModule      endp


;****************************************************************************
;Releases a LoadModule() module handle back to the system.
;
;Usage: FreeModule(module_handle);
;
;Returns:
;
;nothing.
;
;module_handle is the value returned by LoadModule();
;
;****************************************************************************
        public FreeModule
FreeModule      proc    C USES ,handle:DWORD
        push        edi
        mov         edi,handle
        call        __CWAPI_UNFINDMODULE
        pop         edi
        ret
FreeModule      endp

end
