.386p
.model small

        include cw.inc

        extrn   __CWAPI_FINDFUNCTION:far

.code


;****************************************************************************
;Returns the address of a symbol in a module.
;
;Usage: GetProcAddress(module_handle,function_name);
;
;Returns:
;
;zero on error else function address. (EDX:EAX, use just EAX for FLAT)
;
;module_handle is the value returned by LoadModule() or LoadLibrary()
;module_name is a standard zero terminated string.
;
;****************************************************************************
        public  GetProcAddress
GetProcAddress  proc    C USES ,handle:DWORD, fname:DWORD
        LOCAL       addr_off:DWORD,addr_seg:DWORD,NameSpace[257]:BYTE
        pushad
;
;Build the function name string in the format CauseWay API likes.
;
        mov         esi,fname
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
        mov         edi,handle
        mov         edi,EPSP_Struc.EPSP_Exports[edi]
        push        ebp
        lea         ebp,NameSpace
        call        __CWAPI_FINDFUNCTION
        pop         ebp
        jnc         @@2
        xor         ecx,ecx             ;Zero the address
        xor         edx,edx
        jmp         @@3
;
;Fetch function address.
;
@@2:    mov         edx,[edi]
        xor         ecx,ecx
        mov         cx,[edi+4]
;
;Return function (or error) to caller.
;
@@3:    mov         addr_off,edx        ;Use calling parameter space.
        mov         addr_seg,ecx
        popad
        mov         eax,addr_off        ;Get return value.
        mov         edx,addr_seg
        ret
GetProcAddress  endp

end
