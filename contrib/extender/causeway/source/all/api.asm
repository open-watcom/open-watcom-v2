        .386

_apiCode        segment para 'API CODE' use32
        assume cs:_apiCode, ds:_apiCode
_apiCodeStart   label byte


;*******************************************************************************
;Put carry into return carry.
;*******************************************************************************
cwAPI_C2C       macro
        push    eax
        pushfd
        pushfd
        pop     eax
        and     al,1
        cwAPI_AL2C
        popfd
        pop     eax
        endm


;*******************************************************************************
;Put carry into return carry.
;*******************************************************************************
cwAPI_AL2C      macro
        local __0, __1
        pushfd
        assume ds:nothing
        test    cs:apiSystemFlags,1
        jz      __0
        or      b[ebp+Int_Flags16],al
        jmp     __1
__0:    or      b[ebp+Int_Flags32],al
__1:    assume ds:_apiCode
        popfd
        endm


;*******************************************************************************
;Call old int 31h handler.
;*******************************************************************************
cwAPI_CallOld   macro
        local __0, __1
        assume ds:nothing
        test    cs:apiSystemFlags,1
        jz      __0
        pushf
        db 66h
        call    cs:f[OldIntSys]         ;pass it onto previous handler.
        jmp     __1
__0:    pushfd
        call    cs:f[OldIntSys]         ;pass it onto previous handler.
__1:    assume ds:_apiCode
        endm


;*******************************************************************************
;Convert character in AL to upper case.
;*******************************************************************************
UpperChar       macro
        local __0
        cmp     al,61h          ; 'a'
        jb      __0
        cmp     al,7Ah          ; 'z'
        ja      __0
        and     al,5Fh          ;convert to upper case.
__0:    ;
        endm


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;This must be just before cwAPIpatch
;
cwIdentity      db "CAUSEWAY"
cwMajorVersion  db 0
cwMinorVersion  db 0
;
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;CW API int 31h patch.
;
cwAPIpatch      proc    near
        pushm   ds,es,fs,gs             ;Preserve all registers.
        pushad          ;/
        mov     ebp,esp         ;Make registers addressable.
;
;Check if we're allowed to interfere.
;
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     Int21hExecCount,0
        assume ds:_apiCode
        pop     ds
        jnz     @@Nope
;
;Check if this is a CauseWay function.
;
        cmp     ah,255
        jz      @@Ours
;
;Scan the table of other relavent functions.
;
        mov     edi,offset apiExtraCallTable+2
@@extra:        cmp     cs:w[edi-2],0
        jz      @@Nope
        cmp     ax,cs:w[edi-2]
        jz      @@GotCall
        add     edi,4+2
        jmp     @@extra
;
;An internal funtion? so point to table entry.
;
@@Ours: movzx   edi,al
        shl     edi,2
        add     edi,offset apiCallTable
;
;Check if final call is still going to pass control to the old handler.
;
@@GotCall:      cmp     cs:d[edi],0
        jz      @@Nope
;
;See about turning interrupts back on.
;
        mov     esi,Int_Flags32
        assume ds:nothing
        test    cs:apiSystemFlags,1     ;/
        assume ds:_apiCode
        jz      @@32bit0                ;/
        movzx   ebp,bp          ;/
        mov     esi,Int_Flags16
@@32Bit0:       and     ss:b[ebp+esi],not 1     ;clear carry.
        cld                     ;Default direction.
        test    ss:w[ebp+esi],1 shl 9   ;Were interrupts enabled?
        jz      @@NoInts
        sti                     ;Turn interrupts back on.
;
;Call the function handler.
;
@@NoInts:       assume ds:nothing
        call    cs:d[edi]               ;Pass control to handler.
        assume ds:_apiCode
        popad                   ;\
        popm    ds,es,fs,gs             ;Restore all registers.
        assume ds:nothing
        test    cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      @@32Bit1
        iret
@@32Bit1:       iretd                   ;Return to caller.
;
;Not an internal function or anything we want to interfere with so pass control
;to origional handler.
;
@@Nope: popad                   ;Restore registers.
        popm    ds,es,fs,gs
        assume ds:nothing
        test    cs:apiSystemFlags,1
        jz      @@n32
        db 66h
@@n32:  jmp     cs:f[OldIntSys] ;pass it onto previous handler.
        assume ds:_apiCode
;
OldIntSys       df 0
;
APICallTable    dd cwAPI_Info           ;00
        dd cwAPI_IntXX          ;01
        dd cwAPI_FarCallReal    ;02
        dd cwAPI_GetSel         ;03
        dd cwAPI_RelSel         ;04
        dd cwAPI_CodeSel                ;05
        dd cwAPI_AliasSel               ;06
        dd cwAPI_GetSelDet              ;07
        ;
        dd cwAPI_GetSelDet32    ;08
        dd cwAPI_SetSelDet              ;09
        dd cwAPI_SetSelDet32    ;0A
        dd cwAPI_GetMem         ;0B
        dd cwAPI_GetMem32               ;0C
        dd cwAPI_ResMem         ;0D
        dd cwAPI_ResMem32               ;0E
        dd cwAPI_RelMem         ;0F
        ;
        dd cwAPI_GetMemLinear   ;10
        dd cwAPI_GetMemLinear32 ;11
        dd cwAPI_ResMemLinear   ;12
        dd cwAPI_ResMemLinear32 ;13
        dd cwAPI_RelMemLinear   ;14
        dd cwAPI_RelMemLinear32 ;15
        dd cwAPI_GetMemNear     ;16
        dd cwAPI_ResMemNear     ;17
        ;
        dd cwAPI_RelMemNear     ;18
        dd cwAPI_Linear2Near    ;19
        dd cwAPI_Near2Linear    ;1A
        dd cwAPI_LockMem                ;1B
        dd cwAPI_LockMem32              ;1C
        dd cwAPI_UnLockMem              ;1D
        dd cwAPI_UnLockMem32    ;1E
        dd cwAPI_LockMemNear    ;1F
        ;
        dd cwAPI_UnLockMemNear  ;20
        dd cwAPI_GetMemDOS              ;21
        dd cwAPI_ResMemDOS              ;22
        dd cwAPI_RelMemDOS              ;23
        dd cwAPI_Exec           ;24
        dd cwAPI_GetDOSTrans    ;25
        dd cwAPI_SetDOSTrans    ;26
        dd cwAPI_GetMCBSize     ;27
        ;
        dd cwAPI_SetMCBSize     ;28
        dd cwAPI_GetSels                ;29
        dd cwAPI_cwLoad         ;2A
        dd cwAPI_cwcInfo                ;2B
        dd cwAPI_GetMemSO               ;2C
        dd cwAPI_ResMemSO               ;2D
        dd cwAPI_RelMemSO               ;2E
        dd cwAPI_UserDump               ;2F

;       dd 16 dup (0)           ;30-3F
        dd      cwAPI_SetDump   ; 30
        dd      cwAPI_UserErrTerm       ; 31
        dd      cwAPI_CWErrName ; 32
        dd 13 dup (0)           ;33-3F

        dd 16 dup (0)           ;40-4F
        dd 16 dup (0)           ;50-5F
        dd 16 dup (0)           ;60-6F
        dd 16 dup (0)           ;70-7F
        ;
        dd 16*7 dup (0)         ;80-EF
        ;
        dd 8 dup (0)            ;F0-F7
        ;
        dd 0                    ;F8
        dd cwAPI_ID                             ;F9
        dd cwAPI_GetPatch               ;FA
        dd cwAPI_cwcLoad                ;FB
        dd cwAPI_LinearCheck    ;FC
        dd cwAPI_ExecDebug              ;FD
        dd cwAPI_Cleanup                ;FE
        dd 0                    ;FF
;
apiExtraCallTable label byte
        dw 0600h
        dd dpmiAPI_Lock
        dw 0601h
        dd dpmiAPI_UnLock
        dw 0303h
        dd dpmiAPI_GetCallBack
        dw 0304h
        dd dpmiAPI_RelCallBack
        dw 0
cwAPIpatch      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;AX     = 0600h
;BX:CX  = Starting linear address of memory to lock
;SI:DI  = Size of region to lock in bytes
;
dpmiAPI_Lock    proc    near
        mov     ax,[ebp+Int_AX]
        mov     bx,[ebp+Int_BX]
        mov     cx,[ebp+Int_CX]
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        pushad
        shl     ebx,16
        mov     bx,cx
        shl     esi,16
        mov     si,di
        mov     edx,ebx
        mov     ecx,esi
        mov     ax,Res_LOCK
        call    RegisterResource
        popad
        cwAPI_CallOld
        cwAPI_C2C
        jnc     @@0
        mov     [ebp+Int_AX],ax
@@0:    ret
dpmiAPI_Lock    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;AX     = 0601h
;BX:CX  = Starting linear address of memory to unlock
;SI:DI  = Size of region to unlock in bytes
;
dpmiAPI_UnLock  proc    near
        mov     ax,[ebp+Int_AX]
        mov     bx,[ebp+Int_BX]
        mov     cx,[ebp+Int_CX]
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        pushad
        shl     ebx,16
        mov     bx,cx
        shl     esi,16
        mov     si,di
        mov     edx,ebx
        mov     ecx,esi
        mov     ax,Res_LOCK
        call    ReleaseResource
        popad
        cwAPI_CallOld
        cwAPI_C2C
        jnc     @@0
        mov     [ebp+Int_AX],ax
@@0:    ret
dpmiAPI_UnLock  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;AX = 0303h
;DS:(E)SI = Selector:Offset of procedure to call
;ES:(E)DI = Selector:Offset of real mode call structure
;
;Returns
;
;If function was successful:
;Carry flag is clear.
;CX:DX = Segment:Offset of real mode call address
;
dpmiAPI_GetCallBack proc near
        mov     ax,[ebp+Int_AX]
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_ESI]
        mov     es,[ebp+Int_ES]
        mov     edi,[ebp+Int_EDI]
        cwAPI_CallOld
        cwAPI_C2C
        jc      @@9
;
        mov     [ebp+Int_CX],cx
        mov     [ebp+Int_DX],dx
;
        shl     ecx,16
        mov     cx,dx
        mov     edx,ecx
        mov     ecx,esi
        xor     ebx,ebx
        mov     bx,ds
        mov     ax,Res_CALLBACK
        call    RegisterResource
;
@@9:    ret
dpmiAPI_GetCallBack endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;AX = 0304h
;CX:DX = Real mode call-back address to free
;
dpmiAPI_RelCallBack proc near
        mov     ax,[ebp+Int_AX]
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        cwAPI_CallOld
        cwAPI_C2C
        jc      @@9
;
        shl     ecx,16
        mov     cx,dx
        mov     edx,ecx
        mov     ax,Res_CALLBACK
        call    ReleaseResource
;
@@9:    ret
dpmiAPI_RelCallBack endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Call user termination routine on exception
;
;On Entry:
;
; AX = 0ff31h
; CL = 0 if 16-bit termination routine
; CL = nonzero if 32-bit termination routine
; DS:[E]SI = user termination routine address, if ES is zero or an invalid
;  selector value, then the user termination routine call is removed
; ES:[E]DI = user information dump area
;
;Returns:
;
; None
;
cwAPI_UserErrTerm       PROC    NEAR
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     UserTermFlag,0  ; initialize flag
        mov     dx,[ebp+Int_DS]
        lsl     ax,dx
        jnz     uetret                  ; invalid selector
        mov     al,[ebp+Int_CL] ; get bitness flag
        test    al,al
        setne   al                      ; 0 if 0, 1 if nonzero
        inc     ax                              ; 1 if 16-bit, 2 if 32-bit
        mov     UserTermFlag,al
        cmp     al,1                    ; see if 16-bit
        jne     uet32                   ; no

        mov     ax,[ebp+Int_SI]
        mov     WORD PTR UserTermRoutine,ax
        mov     WORD PTR UserTermRoutine+2,dx
        mov     ax,[ebp+Int_DI]
        mov     WORD PTR UserTermDump,ax
        mov     ax,[ebp+Int_ES]
        mov     WORD PTR UserTermDump+2,ax
        jmp     uetret

uet32:
        mov     eax,[ebp+Int_ESI]
        mov     DWORD PTR UserTermRoutine,eax
        mov     WORD PTR UserTermRoutine+4,dx
        mov     eax,[ebp+Int_EDI]
        mov     DWORD PTR UserTermDump,eax
        mov     ax,[ebp+Int_ES]
        mov     WORD PTR UserTermDump+4,ax

uetret:
        ret
cwAPI_UserErrTerm       ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Disable/enable error display and CW.ERR creation
;
;On Entry:
;
; AX = 0ff30h
; CL = 0, disable error display and CW.ERR file creation
; CL = nonzero, enable error display and CW.ERR file creation
;
;Returns:
;
; None
;
cwAPI_SetDump   PROC    NEAR
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     al,[ebp+Int_CL]
        mov     EnableDebugDump,al
        ret
cwAPI_SetDump   ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
; change CW.ERR file name, with optional path/drivespec
;
;On Entry:
;
; AX = 0ff32h
; CX:[E]DX = new CW.ERR ASCIIZ file name
;
;Returns:
;
; None
;
cwAPI_CWErrName PROC    NEAR
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     edx,[ebp+Int_EDX]
        test    cs:apiSystemFlags,1
        jz      cen2
        movzx   edx,dx  ; 16-bit, zero high word of edx

cen2:
        mov     ax,[ebp+Int_CX]
        mov     es,ax
        mov     ebx,OFFSET NewCWErrName ; ds:ebx -> destination, es:edx -> source
        mov     cx,80           ; don't allow more than 80 chars in file name

cenloop:
        mov     al,es:[edx]
        test    al,al
        je      centerm         ; at null terminator
        mov     ds:[ebx],al
        inc     edx
        inc     ebx
        dec     cx
        jne     cenloop

centerm:
        mov     BYTE PTR ds:[ebx],0
        ret
cwAPI_CWErrName ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Setup user buffer display in CW.ERR file
;
;On Entry:
;
; AX = 0ff2fh
; ES:[E]DI -> user buffer to display in CW.ERR
; CX = count of bytes to display from buffer in CW.ERR
; BL = 'A' if ASCII dump (non-binary display of bytes, control characters
;      display as periods)
; BH = nonzero if preset ASCII buffer to word value, ignored for non-ASCII
; DX = word value to fill ASCII dump buffer if BH is nonzero, ignored
;      for non-ASCII
;
;Returns:
;
; Carry set on ASCII dump invalid buffer address
;
cwAPI_UserDump  PROC    NEAR
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ax,[ebp+Int_ES]
        verr    ax
        jnz     udfail                  ; can't read to user selector
        mov     DebugUserSel,ax
        mov     es,ax

        mov     edi,[ebp+Int_EDI]
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@0
        movzx   edi,di
@@0:
        mov     DebugUserOff,edi
        mov     cx,[ebp+Int_CX]
        mov     DebugUserCount,cx

; check for out of bounds
        movzx   edx,cx
        add     edx,edi
        jc      udfail
        dec     edx                             ; make relative 0
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,edx
        jb      udfail

        mov     al,[ebp+Int_BL]
        cmp     al,'A'
        je      aflag
        xor     al,al

aflag:
        mov     DebugAsciiFlag,al
        test    al,al
        je      udret                   ; if not ASCII then no fill
        mov     al,[ebp+Int_BH] ; check fill flag
        test    al,al           ; no fill, carry flag reset by default
        je      udret

; fill the allocation with value in dx to count specified in cx
        mov     ax,es
        verw    ax
        jnz     udfail                  ; can't write to user selector
        mov     ax,[ebp+Int_DX]
        push    ax
        push    ax
        pop     eax
        push    ecx
        shr     ecx,1
        rep     stosd
        pop     ecx
        and     cx,3
        je      udsuccess
        mov     es:[edi],al             ; finish off remainder bytes
        dec     cx
        je      udsuccess
        inc     edi
        mov     es:[edi],ah
        dec     cx
        je      udsuccess
        inc     edi
        mov     es:[edi],al

udsuccess:
        clc                                     ; flag success
        jmp     udret

udfail:
        mov     DebugUserCount,0
        stc                                     ; flag failure

udret:
        cwAPI_C2C
        ret
cwAPI_UserDump  ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get CauseWay identifier, return PageDIRLinear and Page1stLinear info
;
;Returns:
;
;EDX    - Linear address of patch table.
;
cwAPI_ID        proc    near
        push    ds
        assume ds:nothing
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     esi,PageDIRLinear
        mov     edi,Page1stLinear
        pop     ds
        mov     ecx,"CAUS"
        mov     edx,"EWAY"
        mov     [ebp+Int_ESI],esi
        mov     [ebp+Int_EDI],edi
        mov     [ebp+Int_EDX],edx
        mov     [ebp+Int_ECX],ecx
        xor     al,al
        cwAPI_AL2C
        ret
cwAPI_ID        endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get patch table address.
;
;Returns:
;
;EDX    - Linear address of patch table.
;
cwAPI_GetPatch  proc    near
        assume ds:nothing
        mov     bx,cs:apiDDSeg
        sys     GetSelDet32
        add     edx,offset PatchTable
        assume ds:_apiCode
        mov     [ebp+Int_EDX],edx
        xor     al,al
        cwAPI_AL2C
        ret
cwAPI_GetPatch  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get address of transfer buffer.
;
;Returns:
;
;BX     - Real mode segment
;DX     - Protected mode selector.
;ECX    - Buffer size.
;
cwAPI_GetDOSTrans proc near
        assume ds:nothing
        mov     es,cs:apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        assume ds:_apiCode
        mov     ax,es:w[EPSP_TransReal]
        mov     [ebp+Int_BX],ax
        mov     ax,es:w[EPSP_TransProt]
        mov     [ebp+Int_DX],ax
        mov     eax,es:d[EPSP_TransSize]
        mov     [ebp+Int_ECX],eax
        ret
cwAPI_GetDOSTrans endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set real mode transfer buffer address.
;
;On Entry:
;
;BX     - Real mode segment.
;DX     - Protected mode selector.
;ECX    - Buffer size.
;
cwAPI_SetDOSTrans proc near
        mov     bx,[ebp+Int_BX]
        mov     dx,[ebp+Int_DX]
        mov     ecx,[ebp+Int_ECX]
        assume ds:nothing
        mov     es,cs:apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        assume ds:_apiCode
        mov     es:w[EPSP_TransReal],bx
        mov     es:w[EPSP_TransProt],dx
        cmp     ecx,10000h
        jc      @@0
        mov     ecx,65535
@@0:    mov     es:d[EPSP_TransSize],ecx
        ret
cwAPI_SetDOSTrans endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get various useful variable contents.
;
;Returns:
;
;AX     - 0-4G selector.
;BX     - Current PSP selector.
;ECX    - Transfer buffer size.
;DX     - Transfer buffer real mode segment.
;ESI    - Transfer buffer offset.
;DI     - System flags.
;ES     - Transfer buffer protected mode selector.
;
cwAPI_Info      proc    near
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     di,w[SystemFlags]
        mov     ax,RealSegment
        mov     bx,PspSegment
        mov     es,bx
        mov     dx,es:w[EPSP_TransReal]
        mov     ecx,es:d[EPSP_TransSize]
        mov     es,es:w[EPSP_TransProt]
        xor     esi,esi
        assume ds:_apiCode
        mov     [ebp+Int_AX],ax
        mov     [ebp+Int_BX],bx
        mov     [ebp+Int_ECX],ecx
        mov     [ebp+Int_DX],dx
        mov     [ebp+Int_ESI],esi
        mov     [ebp+Int_DI],di
        mov     [ebp+Int_ES],es
        ret
cwAPI_Info      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Pass control to a real mode interrupt handler.
;
;On Entry:
;
;BL     - INT number.
;ES:[E]DI       - Real mode register structure.
;
cwAPI_IntXX     proc    near
        mov     edi,[ebp+Int_EDI]
        mov     es,[ebp+Int_ES]
;       mov     bl,[ebp+Int_BL]
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@0
        movzx   edi,di
@@0:    assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     @@NoStack0
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        sub     DPMIStackOff,RawStackDif
        mov     ax,DPMIStackOff
        add     ax,RawStackDif
        mov     es:Real_SP[edi],ax
        mov     ax,DPMIStackSeg
        mov     es:Real_SS[edi],ax

; MED 05/23/96
        test    cs:apiSystemFlags,1
        jz      medUse32Bit8
        mov     ebx,Int_Flags16
        jmp     medUse16Bit8

medUse32Bit8:
        mov     ebx,Int_Flags32

medUse16Bit8:
        mov     ax,ss:[ebp+ebx]
        and     ax,1111110011111111b    ;clear Trap and INT flag.
        mov     es:Real_Flags[edi],ax   ; explicitly set flags on DPMI 300h call

        assume ds:_apiCode
        jmp     @@DoneStack0
@@NoStack0:
        mov     es:Real_SP[edi],0       ;use DPMI provided stack.
        mov     es:Real_SS[edi],0
@@DoneStack0:
        xor     cx,cx           ;No stack parameters.
        mov     bh,ch           ;no flags.

        mov     bl,ss:[ebp+Int_BL]      ; MED 05/23/96

        mov     ax,0300h
        cwAPI_CallOld
        cwAPI_C2C
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     @@DoneStack1
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        add     DPMIStackOff,RawStackDif
        assume ds:_apiCode
@@DoneStack1:   assume ds:nothing
        test    cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      @@Use32Bit8
        mov     ebx,Int_Flags16
        jmp     @@Use16Bit8
@@Use32Bit8:    mov     ebx,Int_Flags32
@@Use16Bit8:    mov     ax,ss:[ebp+ebx]
        and     ax,0000011000000000b            ;retain IF.
        and     es:Real_Flags[edi],1111100111111111b    ;lose IF.
        or      es:Real_Flags[edi],ax
        ret
cwAPI_IntXX     endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Pass control to a real mode far routine.
;
;On Entry:
;
;ES:[E]DI       - Real mode register structure.
;
cwAPI_FarCallReal proc near
        mov     edi,[ebp+Int_EDI]
        mov     es,[ebp+Int_ES]
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@0
        movzx   edi,di
@@0:    pushf
        pop     es:Real_Flags[edi]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     @@NoStack1
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        sub     DPMIStackOff,RawStackDif
        mov     ax,DPMIStackOff
        add     ax,RawStackDif
        mov     es:Real_SP[edi],ax
        mov     ax,DPMIStackSeg
        mov     es:Real_SS[edi],ax
        assume ds:_apiCode
        jmp     @@DoneStack2
@@NoStack1:     mov     es:Real_SP[edi],0       ;use DPMI provided stack.
        mov     es:Real_SS[edi],0
@@DoneStack2:   xor     cx,cx           ;No stack parameters.
        xor     bx,bx           ;no flags.
        mov     ax,0301h
        cwAPI_CallOld
        cwAPI_C2C
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     @@DoneStack3
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        add     DPMIStackOff,RawStackDif
        assume ds:_apiCode
@@DoneStack3:   ;
        assume ds:nothing
        test    cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      @@Use32Bit8
        mov     ebx,Int_Flags16
        jmp     @@Use16Bit8
@@Use32Bit8:    mov     ebx,Int_Flags32
@@Use16Bit8:    mov     ax,ss:[ebp+ebx]
        and     ax,0000011000000000b            ;retain IF.
        and     es:Real_Flags[edi],1111100111111111b    ;lose IF.
        or      es:Real_Flags[edi],ax
        ret
cwAPI_FarCallReal endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate a selector.
;
;Returns:
;
;Carry set on error (no more selectors) else,
;
;BX     - new selector.
;
cwAPI_GetSel    proc    near
        xor     eax,eax
        xor     ebx,ebx
        call    _SetSelector
        cwAPI_C2C
        jc      @@9
        mov     [ebp+Int_BX],bx
@@9:    ret
cwAPI_GetSel    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate multiple selectors.
;
;On Entry:
;
;CX     - Number of selectors.
;
;Returns:
;
;Carry set on error (not enough selectors) else,
;
;BX     - Base selector.
;
cwAPI_GetSels   proc    near
;
;Get selectors from DPMI.
;
        mov     cx,[ebp+Int_CX]
        mov     ax,0000h
        cwAPI_CallOld
        cwAPI_C2C
        jc      @@9
;
        mov     [ebp+Int_BX],ax
        movzx   edx,ax
        movzx   ecx,cx
        mov     ax,Res_SEL
@@0:    call    RegisterResource
        add     edx,8
        dec     ecx
        jnz     @@0
;
@@9:    ret
cwAPI_GetSels   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release a selector.
;
;On Entry:
;
;BX     - Selector to release.
;
cwAPI_RelSel    proc    near
        mov     bx,[ebp+Int_BX]
        mov     ds,[ebp+Int_DS]
        mov     es,[ebp+Int_ES]
        mov     fs,[ebp+Int_FS]
        mov     gs,[ebp+Int_GS]
        call    _RelSelector
        cwAPI_C2C
        jc      @@9
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
@@9:    ret
cwAPI_RelSel    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Force a selector to be type CODE.
;
;On Entry:
;
;BX     - Selector.
;
cwAPI_CodeSel   proc    near
        mov     bx,[ebp+Int_BX]
        call    _CodeSelector
        cwAPI_C2C
        ret
cwAPI_CodeSel   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate a selector with type DATA that maps same memory as another selector.
;
;On Entry:
;
;BX     - selector.
;
;Returns:
;
;AX     - New selector.
;
cwAPI_AliasSel  proc    near
        mov     ax,000ah
        mov     bx,[ebp+Int_BX]
        cwAPI_CallOld
        cwAPI_C2C
        jc      @@9
;
        mov     [ebp+Int_AX],ax
        pushm   eax,edx
        movzx   edx,ax
        mov     ax,Res_SEL
        call    RegisterResource
        popm    eax,edx
;
@@9:    ret
cwAPI_AliasSel  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get selector base & limit.
;
;On Entry:
;
;BX     - selector.
;
;Returns:
;
;CX:DX  - Linear base.
;SI:DI  - Byte granular limit.
;
cwAPI_GetSelDet proc near
        mov     bx,[ebp+Int_BX]
        call    _DetSelector
        cwAPI_C2C
        jc      @@9
;
        mov     ecx,eax         ;get base.
        mov     dx,cx
        shr     ecx,16
        mov     esi,ebx         ;get limit.
        mov     di,si
        shr     esi,16
        mov     [ebp+Int_CX],cx
        mov     [ebp+Int_DX],dx
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_DI],di
;
@@9:    ret
cwAPI_GetSelDet endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get selector base & limit.
;
;On Entry:
;
;BX     - selector.
;
;Returns:
;
;EDX    - base
;ECX    - limit
;
cwAPI_GetSelDet32 proc near
        mov     bx,[ebp+Int_BX]
        call    _DetSelector
        cwAPI_C2C
        jc      @@9
;
        mov     [ebp+Int_EDX],eax
        mov     [ebp+Int_ECX],ebx
;
@@9:    ret
cwAPI_GetSelDet32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set selectors base & limit.
;
;On Entry:
;
;BX     - selector
;CX:DX  - Linear base.
;SI:DI  - Byte granular limit.
;
cwAPI_SetSelDet proc near
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        shl     ecx,16          ;Get base to somewhere useful.
        mov     cx,dx
        mov     eax,ecx
        shl     esi,16          ;get limit to somewhere useful.
        mov     si,di
        mov     ebx,esi
        mov     cx,[ebp+Int_BX]
        call    _SizeSelector
        cwAPI_C2C
        ret
cwAPI_SetSelDet endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set a selectors details.
;
;On Entry:
;
;BX     - selector
;EDX    - base
;ECX    - limit.
;
cwAPI_SetSelDet32 proc near
        mov     cx,[ebp+Int_BX]
        mov     eax,[ebp+Int_EDX]
        mov     ebx,[ebp+Int_ECX]
        call    _SizeSelector
        cwAPI_C2C
        ret
cwAPI_SetSelDet32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate some memory with a selector attatched.
;
;On Entry:
;
;CX:DX  - Size of block required in bytes. (-1:-1 to get maximum memory size)
;
;On Exit:
;
;Carry clear if OK &
;
;BX     - Selector to access the block with.
;
;Else if CX:DX was -1, CX:DX is size of largest block available.
;
cwAPI_GetMem    proc    near
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16
        mov     cx,dx

; MED 06/25/97
        push    ds              ; test padding flag
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        test    Pad1Flag,-1
        pop     ds
        assume ds:nothing
        je      gm2             ; padding flag not turned on
        test    ecx,0ffff0000h
        jne     gm2             ; don't pad >64K allocation
        add     ecx,1023
        and     ecx,NOT 1023    ; pad to 1K-boundary allocation
gm2:

        call    mcbGetMemLinear32
        jc      @@2
        sys     GetSel
        jc      @@0
        mov     edx,esi
        jecxz   @@3
        dec     ecx             ;limit=length-1
@@3:    sys     SetSelDet32
        mov     [ebp+Int_BX],bx
        clc
        jmp     @@1
@@0:    call    mcbRelMemLinear32
        stc
        jmp     @@1
        ;
@@2:    mov     dx,cx
        shr     ecx,16
        mov     ax,[ebp+Int_CX]
        shl     eax,16
        mov     ax,[ebp+Int_DX]
        cmp     eax,-2
        jz      @@5
        cmp     eax,-1
        jnz     @@4
@@5:    mov     [ebp+Int_CX],cx
        mov     [ebp+Int_DX],dx
@@4:    stc
        ;
@@1:    cwAPI_C2C
        ret
cwAPI_GetMem    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate some memory and return selector:offset (16-bit).
;
;On Entry:
;
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry set on error else,
;
;SI:DI  - selector:offset of allocated memory.
;
cwAPI_GetMemSO  proc    near
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16
        mov     cx,dx
        call    mcbGetMemLinear32       ;allocate some memory.
        jc      @@9
        assume ds:nothing
        cmp     cs:mcbLastChunk,0       ;DPMI memory?
        assume ds:_apiCode
        jnz     @@1
;
;Allocate a selector for this memory.
;
        sys     GetSel
        jc      @@8
        mov     edx,esi
        jecxz   @@0
        dec     ecx             ;limit=length-1
@@0:    sys     SetSelDet32
        xor     dx,dx
        jmp     @@7
;
;Get chunk's selector.
;
@@1:    push    es
        assume ds:nothing
        mov     es,cs:apiDSeg
        assume es:_cwMain
        mov     es,RealSegment
        assume es:nothing
        mov     edi,cs:mcbLastChunk
        assume ds:_apiCode
        mov     bx,es:[edi+mcbChunkSel] ;get chunk selector.
        pop     es
        mov     edx,esi
        sub     edx,edi         ;get blocks offset.
;
@@7:    mov     [ebp+Int_SI],bx
        mov     [ebp+Int_DI],dx
        clc
        jmp     @@9
;
@@8:    call    mcbRelMemLinear32
        stc
;
@@9:    cwAPI_C2C
        ret
cwAPI_GetMemSO  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;On Entry:
;
;ECX    - Size of block required in bytes. (-1 to get maximum memory size)
;
;On Exit:
;
;Carry clear if OK &
;
;BX     - Selector to access the block with.
;
;Else if ECX was -1, ECX is size of largest block available.
;
cwAPI_GetMem32  proc near
        mov     ecx,[ebp+Int_ECX]
        call    mcbGetMemLinear32
        jc      @@2
        sys     GetSel
        jc      @@0
        mov     edx,esi
        jecxz   @@3
        dec     ecx             ;limit=length-1
        sys     SetSelDet32
@@3:    mov     [ebp+Int_BX],bx
        clc
        jmp     @@1
@@0:    call    mcbRelMemLinear32
        stc
        jmp     @@1
        ;
@@2:    cmp     d[ebp+Int_ECX],-1
        jz      @@5
        cmp     d[ebp+Int_ECX],-2
        jnz     @@4
@@5:    mov     [ebp+Int_ECX],ecx
@@4:    stc
        ;
@@1:    cwAPI_C2C
        ret
cwAPI_GetMem32  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated block of memory.
;
;On Entry:
;
;BX     - Selector for block.
;CX:DX  - New size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK.
;
cwAPI_ResMem    proc    near
        mov     bx,[ebp+Int_BX]
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16          ;convert new size to 32-bit.
        mov     cx,dx
        push    ecx
        sys     GetSelDet32             ;Get selector base address.
        mov     esi,edx
        pop     ecx
        jc      @@9
        call    mcbResMemLinear32       ;re-size the memory.
        jc      @@9
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
@@9:    cwAPI_C2C
        ret
cwAPI_ResMem    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated block of memory.
;
;On Entry:
;
;SI:DI  - Selector:offset for block.
;CX:DX  - New size of block required in bytes.
;
;On Exit:
;
;Carry set on error else,
;
;SI:DI  - selector:offset new block address.
;
cwAPI_ResMemSO  proc    near
        mov     ds,[ebp+Int_DS]
        mov     es,[ebp+Int_ES]
        mov     fs,[ebp+Int_FS]
        mov     gs,[ebp+Int_GS]
;
        mov     bx,w[ebp+Int_SI]
        sys     GetSelDet32             ;get selectors details.
        jc      @@9
        mov     esi,edx
        movzx   eax,w[ebp+Int_DI]
        add     esi,eax         ;get memory blocks address.
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16          ;convert new size to 32-bit.
        mov     cx,dx
        call    mcbResMemLinear32       ;re-size the memory.
        jc      @@9
;
;Check new block type.
;
        assume ds:nothing
        cmp     cs:mcbLastChunk,0       ;DPMI memory?
        assume ds:_apiCode
        jnz     @@1
;
;Update selectors details.
;
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
        xor     dx,dx
        jmp     @@7
;
;Get chunk's selector.
;
@@1:    push    es
        assume ds:nothing
        mov     es,cs:apiDSeg
        assume es:_cwMain
        mov     es,RealSegment
        assume es:nothing
        mov     edi,cs:mcbLastChunk
        assume ds:_apiCode
        mov     bx,es:[edi+mcbChunkSel] ;get chunk selector.
        pop     es
        mov     edx,esi
        sub     edx,edi         ;get blocks offset.
;
;Check if the old block had it's own selector.
;
        cmp     w[ebp+Int_DI],0 ;offset of zero?
        jnz     @@7
        push    bx
        mov     bx,w[ebp+Int_SI]
        sys     RelSel          ;release the selector.
        pop     bx
;
@@7:    mov     [ebp+Int_SI],bx
        mov     [ebp+Int_DI],dx
        clc
;
@@9:    cwAPI_C2C
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
        ret
cwAPI_ResMemSO  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated block of memory.
;
;On Entry:
;
;BX     - Selector for block.
;ECX    - New size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK.
;
cwAPI_ResMem32  proc    near
        mov     bx,[ebp+Int_BX]
        mov     ecx,[ebp+Int_ECX]
        push    ecx
        sys     GetSelDet32             ;Get selector base address.
        mov     esi,edx
        pop     ecx
        jc      @@9
        call    mcbResMemLinear32       ;re-size the memory.
        jc      @@9
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
@@9:    cwAPI_C2C
        ret
cwAPI_ResMem32  endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release previously allocated block of memory. Is be used for memory allocated
;by either GetMem or GetMem32.
;
;
;On Entry:
;
;BX     - Selector for block to release.
;
cwAPI_RelMem    proc    near
        mov     bx,[ebp+Int_BX]
        sys     GetSelDet32             ;Get selector base address.
        jc      @@9
        mov     esi,edx
        mov     ds,[ebp+Int_DS]
        mov     es,[ebp+Int_ES]
        mov     fs,[ebp+Int_FS]
        mov     gs,[ebp+Int_GS]
        sys     RelSel          ;release the selector.
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
        call    mcbRelMemLinear32       ;release the memory.
@@9:    cwAPI_C2C
        ret
cwAPI_RelMem    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release block of memory allocated via GetMemSO.
;
;On Entry:
;
;SI:DI  - Selector:offset for block to release.
;
cwAPI_RelMemSO  proc    near
;
;Zero any segment registers using this selector.
;
        mov     bx,w[ebp+Int_SI]
        and     bx,0ffffh-3
        jz      @@bad           ; MED 11/18/96, screen out null pointer releases
        xor     cx,cx
        mov     ax,w[ebp+Int_DS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@0
        mov     w[ebp+Int_DS],cx
        mov     ds,cx
@@0:    mov     ax,w[ebp+Int_ES]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@1
        mov     w[ebp+Int_ES],cx
        mov     es,cx
@@1:    mov     ax,w[ebp+Int_FS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@2
        mov     w[ebp+Int_FS],cx
        mov     fs,cx
@@2:    mov     ax,w[ebp+Int_GS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@3
        mov     w[ebp+Int_GS],cx
        mov     gs,cx
@@3:    ;
        mov     bx,w[ebp+Int_SI]
        sys     GetSelDet32             ;get selectors details.
        jc      @@9
        mov     esi,edx
        movzx   eax,w[ebp+Int_DI]
        add     esi,eax         ;get memory blocks address.
        call    mcbRelMemLinear32       ;release the memory.
;
;Check if block had it's own selector.
;
        cmp     w[ebp+Int_DI],0 ;offset of zero?
        jnz     @@4
        mov     bx,w[ebp+Int_SI]
        sys     RelSel          ;release the selector.
;
@@4:    clc
;
@@9:    cwAPI_C2C
        ret

; MED 11/18/96
@@bad:
        stc
        jmp     @@9

cwAPI_RelMemSO  endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate a block of memory without a selector.
;
;On Entry:
;
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;SI:DI  - Linear address of block allocated.
;
cwAPI_GetMemLinear proc near
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16
        mov     cx,dx
        call    mcbGetMemLinear32
        jc      @@9
        mov     di,si
        shr     esi,16
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_DI],di
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_GetMemLinear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate a block of memory without a selector.
;
;On Entry:
;
;ECX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - Linear address of block allocated.
;
cwAPI_GetMemLinear32 proc near
        mov     ecx,[ebp+Int_ECX]
        call    mcbGetMemLinear32
        cwAPI_C2C
        jc      @@9
        mov     [ebp+Int_ESI],esi
@@9:    ret
cwAPI_GetMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated block of memory without a selector.
;
;On Entry:
;
;SI:DI  - Linear address of block to re-size.
;CX:DX  - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;SI:DI  - New linear address of block.
;
cwAPI_ResMemLinear proc near
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16
        mov     cx,dx
        shl     esi,16
        mov     si,di
        call    mcbResMemLinear32
        jc      @@9
        mov     di,si
        shr     esi,16
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_DI],di
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_ResMemLinear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated block of memory without a selector.
;
;On Entry:
;
;ESI    - Linear address of block to re-size.
;ECX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - New linear address of block.
;
cwAPI_ResMemLinear32 proc near
        mov     esi,[ebp+Int_ESI]
        mov     ecx,[ebp+Int_ECX]
        call    mcbResMemLinear32
        jc      @@9
        mov     [ebp+Int_ESI],esi
@@9:    cwAPI_C2C
        ret
cwAPI_ResMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release previously allocated block of memory (linear address).
;
;On Entry:
;
;SI:DI  - Linear address of block to release.
;
;On Exit:
;
cwAPI_RelMemLinear proc near
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        shl     esi,16
        mov     si,di
        call    mcbRelMemLinear32
        cwAPI_C2C
        ret
cwAPI_RelMemLinear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release previously allocated block of memory (linear address).
;
;On Entry:
;
;ESI    - Linear address of block to release.
;
;On Exit:
;
cwAPI_RelMemLinear32 proc near
        mov     esi,[ebp+Int_ESI]
        call    mcbRelMemLinear32
        cwAPI_C2C
        ret
cwAPI_RelMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate an application relative block of memory.
;
;On Entry:
;
;EBX    - Size of block required in bytes.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - Application relative linear address of block allocated.
;
cwAPI_GetMemNear proc near
        mov     ecx,[ebp+Int_EBX]
        call    mcbGetMemLinear32
        jc      @@9
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        sub     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_GetMemNear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a previously allocated application relative block of memory.
;
;On Entry:
;
;EBX    - Size of block required in bytes.
;ESI    - application relative linear address of block to re-size.
;
;On Exit:
;
;Carry clear if OK &,
;
;ESI    - New application relative linear address of block.
;
cwAPI_ResMemNear proc near
        mov     ecx,[ebp+Int_EBX]
        mov     esi,[ebp+Int_ESI]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        add     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        call    mcbResMemLinear32
        jc      @@9
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        sub     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_ResMemNear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release previously allocated application relative block of memory.
;
;On Entry:
;
;ESI    - Application relative linear address of block to release.
;
cwAPI_RelMemNear proc near
        mov     esi,[ebp+Int_ESI]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        add     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        call    mcbRelMemLinear32
        cwAPI_C2C
        ret
cwAPI_RelMemNear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Convert linear address to application relative address.
;
;On Entry:
;
;ESI    - Linear address to convert.
;
;On Exit:
;
;ESI    - Application relative linear address.
;
cwAPI_Linear2Near proc near
        mov     esi,[ebp+Int_ESI]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        sub     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        ret
cwAPI_Linear2Near endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Convert application relative address to linear address.
;
;On Entry:
;
;ESI    - Application relative linear address.
;
;On Exit:
;
;ESI    - Linear address to convert.
;
cwAPI_Near2Linear proc near
        mov     esi,[ebp+Int_ESI]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        add     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        ret
cwAPI_Near2Linear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Lock a region of memory.
;
;On Entry:
;
;BX:CX  - Starting linear address of memory to lock.
;SI:DI  - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
cwAPI_LockMem   proc    near
        mov     bx,[ebp+Int_BX]
        mov     cx,[ebp+Int_CX]
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        mov     ax,0600h
        cwAPI_CallOld
        jc      @@9
;
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    RegisterResource
        clc
;
@@9:    cwAPI_C2C
        ret
cwAPI_LockMem   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Lock a region of memory.
;
;On Entry:
;
;ESI    - Starting linear address of memory to lock.
;ECX    - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
cwAPI_LockMem32 proc near
        mov     esi,[ebp+Int_ESI]
        mov     ecx,[ebp+Int_ECX]
        xchg    esi,ecx
        mov     ebx,ecx
        shr     ebx,16
        mov     di,si
        shr     esi,16
        mov     ax,0600h
        cwAPI_CallOld
        jc      @@9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    RegisterResource
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_LockMem32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Un-lock a region of memory.
;
;On Entry:
;
;BX:CX  - Starting linear address of memory to unlock
;SI:DI  - Size of region to unlock in bytes
;
cwAPI_UnLockMem proc near
        mov     bx,[ebp+Int_BX]
        mov     cx,[ebp+Int_CX]
        mov     si,[ebp+Int_SI]
        mov     di,[ebp+Int_DI]
        mov     ax,0601h
        cwAPI_CallOld
        jc      @@9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_UnLockMem endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Un-lock a region of memory.
;
;On Entry:
;
;ESI    - Starting linear address of memory to unlock
;ECX    - Size of region to unlock in bytes
;
;NOTES:
;
;This will allow the memory to be swapped to disk by the VMM if neccessary.
;Areas below and above the specified memory will also be un-locked if the
;specified region is not page aligned.
;
cwAPI_UnLockMem32 proc near
        mov     esi,[ebp+Int_ESI]
        mov     ecx,[ebp+Int_ECX]
        xchg    esi,ecx
        mov     ebx,ecx
        shr     ebx,16
        mov     di,si
        shr     esi,16
        mov     ax,0601h
        cwAPI_CallOld
        jc      @@9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_UnLockMem32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Lock a region of memory using application relative address.
;
;On Entry:
;
;ESI    - Starting linear address of memory to lock.
;EBX    - Size of region to lock in bytes.
;
;On Exit:
;
;Carry set on error, none of the memory locked, else memory is locked.
;
cwAPI_LockMemNear proc near
        mov     esi,[ebp+Int_ESI]
        mov     ebx,[ebp+Int_EBX]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        add     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        xchg    esi,ebx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16
        mov     ax,0600h
        cwAPI_CallOld
        jc      @@9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    RegisterResource
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_LockMemNear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Un-lock a region of memory using application relative address.
;
;On Entry:
;
;ESI    - Starting linear address of memory to unlock
;EBX    - Size of region to unlock in bytes
;
cwAPI_UnLockMemNear proc near
        mov     esi,[ebp+Int_ESI]
        mov     ecx,[ebp+Int_ECX]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        add     esi,ds:d[EPSP_NearBase]
        assume ds:_apiCode
        xchg    esi,ebx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16
        mov     ax,0601h
        cwAPI_CallOld
        jc      @@9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
@@9:    cwAPI_C2C
        ret
cwAPI_UnLockMemNear endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Allocate a region of DOS (conventional) memory.
;
;On Entry:
;
;BX     - Number of paragraphs (16 byte blocks) required.
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;AX     - Initial real mode segment of allocated block
;DX     - Initial selector for allocated block
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code.
;BX     - Size of largest available block in paragraphs.
;
cwAPI_GetMemDOS proc near
        mov     bx,[ebp+Int_BX]
        mov     ax,0100h
        cwAPI_CallOld
        mov     [ebp+Int_AX],ax
        cwAPI_C2C
        jnc     @@0
        mov     [ebp+Int_BX],bx
        jmp     @@9
;
@@0:    mov     [ebp+Int_DX],dx
        movzx   edx,dx
        movzx   ecx,bx
        shl     ecx,4
        mov     ax,Res_DOSMEM
        call    RegisterResource
;
@@9:    ret
cwAPI_GetMemDOS endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Re-size a block of DOS (conventional) memory previously allocated with
;GetMemDOS.
;
;On Entry:
;
;BX     - New block size in paragraphs
;DX     - Selector of block to modify
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code:
;BX     - Maximum block size possible in paragraphs
;
cwAPI_ResMemDOS proc near
        mov     bx,[ebp+Int_BX]
        mov     dx,[ebp+Int_DX]
        mov     ax,0102h
        cwAPI_CallOld
        jnc     @@0
        mov     [ebp+Int_AX],ax
        mov     [ebp+Int_BX],bx
        jmp     @@9
;
@@0:    mov     ax,Res_DOSMEM
        movzx   edx,dx
        call    ReleaseResource
        movzx   ecx,bx
        shl     ecx,4
        call    RegisterResource
        clc
;
@@9:    cwAPI_C2C
        ret
cwAPI_ResMemDOS endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Release a block of DOS (conventional) memory previously allocated with
;GetMemDOS.
;
;On Entry:
;
;DX     - Selector of block to free.
;
;On Exit:
;
;If function was successful:
;Carry flag is clear.
;
;If function was not successful:
;Carry flag is set.
;
;AX     - DOS error code.
;
cwAPI_RelMemDOS proc near

        mov     ds,w[ebp+INT_DS]
        mov     es,w[ebp+INT_ES]
        mov     fs,w[ebp+INT_FS]
        mov     gs,w[ebp+INT_GS]

        mov     dx,[ebp+Int_DX]
        mov     ax,0101h
        cwAPI_CallOld
        jnc     @@0
        mov     [ebp+Int_AX],ax
        jmp     @@9
;
@@0:    mov     ax,Res_DOSMEM
        movzx   edx,dx
        call    ReleaseResource
        clc
;
@@9:    cwAPI_C2C

        mov     w[ebp+INT_DS],ds
        mov     w[ebp+INT_ES],es
        mov     w[ebp+INT_FS],fs
        mov     w[ebp+INT_GS],gs

        ret
cwAPI_RelMemDOS endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Run another CauseWay program directly.
;
;On Entry:
;
;DS:EDX - File name.
;ES:ESI - Command line. First byte is length, then real data.
;CX     - Environment selector, 0 to use existing copy.
;
;On Exit:
;
;Carry set on error and AX = error code else AL=ErrorLevel
;
cwAPI_Exec      proc    near
        mov     ds,[ebp+Int_DS]
        mov     edx,[ebp+Int_EDX]
        mov     es,[ebp+Int_ES]
        mov     esi,[ebp+Int_ESI]
        mov     cx,[ebp+Int_CX]
        mov     ebx,0
        push    ebp
        call    _Exec
        pop     ebp
        jnc     @@0
        mov     [ebp+Int_AX],ax
        jmp     @@9
;
@@0:    mov     [ebp+Int_AL],al
;
@@9:    cwAPI_C2C
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugagtext1
debugagloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugagb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugagloop2
debugagb:
        mov     edx,OFFSET debugagtext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugagout

debugagtext1    DB      'Done with cwAPI_Exec...',0
debugagtext2    DB      13,10

debugagout:
ENDIF

        ret
cwAPI_Exec      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Validate and get expanded length of a CWC'd file.
;
;On Entry:
;
;BX     - File handle.
;
;On Exit:
;
;Carry set if not a CWC'd file else,
;
;ECX    - Expanded data size.
;
cwAPI_cwcInfo   proc    near
        mov     bx,[ebp+Int_BX]
        call    GetCWCInfo
        cwAPI_C2C
        mov     [ebp+Int_ECX],ecx
        mov     [ebp+Int_EAX],eax
        ret
cwAPI_cwcInfo   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Load/Expand a CWC'd data file into memory.
;
;On Entry:
;
;BX     - Source file handle.
;ES:EDI - Destination memory.
;
;On Exit:
;
;Carry set on error and EAX is error code else,
;
;ECX    - Expanded data length.
;
cwAPI_cwcLoad proc near
        mov     bx,[ebp+Int_BX]
        mov     es,[ebp+Int_ES]
        mov     edi,[ebp+Int_EDI]
        call    DecodeCWC
        mov     [ebp+Int_EAX],eax
        jc      @@9
        mov     [ebp+Int_ECX],ecx
@@9:    cwAPI_C2C
        ret
cwAPI_cwcLoad endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
cwAPI_LinearCheck proc near
        mov     esi,[ebp+Int_ESI]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        ;
        mov     es,RealSegment
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,esi
        jc      @@3
        mov     LinearAddressCheck,1
        mov     al,es:[esi]
        cmp     LinearAddressCheck,0
        mov     LinearAddressCheck,0
        jnz     @@2
@@3:    stc
        jmp     @@1
@@2:    clc
@@1:    assume ds:_apiCode
        ;
        cwAPI_C2C
        ret
cwAPI_LinearCheck endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
cwAPI_ExecDebug proc near
        mov     ds,[ebp+Int_DS]
        mov     edx,[ebp+Int_EDX]
        mov     es,[ebp+Int_ES]
        mov     esi,[ebp+Int_ESI]
        mov     cx,[ebp+Int_CX]
        mov     ebx,1
        push    ebp
        call    _Exec
        jnc     @@0
        pop     ebp
        mov     [ebp+Int_AX],ax
        jmp     @@9
;
@@0:    shl     esi,16
        mov     si,di
        mov     edi,ebp
        pop     ebp
        mov     [ebp+Int_EAX],eax
        mov     [ebp+Int_BX],bx
        mov     [ebp+Int_CX],cx
        mov     [ebp+Int_EDX],edx
        mov     [ebp+Int_DI],si
        shr     esi,16
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_EBP],edi
        clc
;
@@9:    cwAPI_C2C
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugaitext1
debugailoop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugaib
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugailoop2
debugaib:
        mov     edx,OFFSET debugaitext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugaiout

debugaitext1    DB      'Done with cwAPI_ExecDebug...',0
debugaitext2    DB      13,10

debugaiout:
ENDIF

        ret
cwAPI_ExecDebug endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Load another CauseWay program as an overlay, ie, do relocations etc but don't
;actually execute it.
;
;On Entry:
;
;DS:EDX - File name.
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;
cwAPI_cwLoad    proc near
        mov     ds,[ebp+Int_DS]
        mov     edx,[ebp+Int_EDX]
        mov     ebx,2
        xor     ax,ax
        mov     es,ax
        mov     fs,ax
        push    ebp
        call    _Exec
        pop     ebp
        jnc     @@0
        mov     [ebp+Int_AX],ax
        jmp     @@9
;
@@0:    mov     [ebp+Int_CX],cx
        mov     [ebp+Int_EDX],edx
        mov     [ebp+Int_BX],bx
        mov     [ebp+Int_EAX],eax
        mov     [ebp+Int_SI],si
;
@@9:    cwAPI_C2C
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugahtext1
debugahloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugahb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugahloop2
debugahb:
        mov     edx,OFFSET debugahtext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugahout

debugahtext1    DB      'Done with cwAPI_cwLoad...',0
debugahtext2    DB      13,10

debugahout:
ENDIF

        ret
cwAPI_cwLoad    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
cwAPI_Cleanup   proc    near
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        call    LoseFileHandles
        clc
        assume ds:_apiCode
        cwAPI_C2C
        ret
cwAPI_Cleanup   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Use DPMI services to allocate a single selector and initialise base & limit
;as specified with type=data/read/write.
;
;On Entry:-
;
;EAX    - Linear base.
;EBX    - limit.
;
;On Exit:-
;
;BX     - Segment selector.
;
_SetSelector    proc near
        call    DPMICopyCheck
        ;
        pushm   eax,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        cmp     ebx,100000h
        jc      @@ok
        cmp     ebx,-1
        jz      @@ok
        add     ebx,4095
        and     ebx,0FFFFFFFFh-4095
        dec     ebx
        ;
@@ok:   mov     d[@@SelectorBase],eax
        mov     d[@@SelectorSize],ebx
        ;
        ;Get a new selector from DPMI.
        ;
        mov     ax,0000h
        mov     cx,1
        cwAPI_CallOld
        jc      @@9
        mov     bx,ax
@@GotSel:       ;
        push    bx
        mov     ecx,d[@@SelectorSize]
        mov     esi,d[@@SelectorBase]
        mov     ax,ds           ;need RPL to base DPL on.
        lar     ax,ax
        and     ah,DescPL3
        or      ah,DescPresent+DescMemory+DescRWData
        ;
        mov     dx,apiSystemFlags       ;use default setting.
        shr     dx,14
        mov     al,dl
        xor     al,1
        or      al,b[apiSystemFlags+2]
        and     al,1
        shl     al,6
        ;
        cmp     ecx,0fffffh     ; see if we need to set g bit
        jc      @@3
        jz      @@3
        shr     ecx,12          ; div by 4096
        or      al,80h          ; set g bit
@@3:    mov     w[DescriptorBuffer],cx  ;store low word of limit.
        shr     ecx,16
        or      cl,al
        mov     b[DescriptorBuffer+6],cl        ;store high bits of limit and gran/code size bits.
        mov     w[DescriptorBuffer+2],si        ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     b[DescriptorBuffer+4],bl        ;store mid byte of linear base.
        mov     b[DescriptorBuffer+7],bh        ;store high byte of linear base.
        mov     b[DescriptorBuffer+5],ah        ;store pp/dpl/dt/type bits.
        pop     bx
        ;
        ;Write new descriptor table entry.
        ;
        push    bx
        mov     ax,000ch
        mov     edi,offset DescriptorBuffer
        push    ds
        pop     es
        cwAPI_CallOld
        pop     bx
        jc      @@9
        ;
        movzx   edx,bx
        mov     ax,Res_SEL
        call    RegisterResource
        ;
@@8:    clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    eax,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
@@SelectorBase: ;
        dd ?
@@SelectorSize: ;
        dd ?
_SetSelector    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_RelSelector    proc    near
;
;Release a segment selector table entry.
;
;On Entry:-
;
;BX     - Segment selector.
;
        call    DPMICopyCheck
        ;
        ;Check segment registers for value we're releasing and clear
        ;if found.
        ;
        pushm   ax,bx,cx
        and     bx,0ffffh-3
        xor     cx,cx
        mov     ax,ds
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@0
        mov     ds,cx
@@0:    mov     ax,es
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@1
        mov     es,cx
@@1:    mov     ax,fs
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@2
        mov     fs,cx
@@2:    mov     ax,gs
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     @@3
        mov     gs,cx
@@3:    popm    ax,bx,cx
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        pushad
        movzx   edx,bx
        mov     ax,Res_SEL
        call    ReleaseResource
        popad
        ;
        push    bx
        mov     ax,0001h
        cwAPI_CallOld
        pop     bx
        jc      @@9
        ;
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
@@11:   ret
_RelSelector    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_SizeSelector   proc near
;
;Set a segment selector table entry and return a suitable segment value. Will
;use whatever method is apropriate to the system environment.
;
;On Entry:-
;
;EAX    - Linear base address.
;EBX    - size in bytes.
;CX     - Selector.
;
;On Exit:-
;
        call    DPMICopyCheck
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[@@SelectorBase],eax
        mov     d[@@SelectorSize],ebx
        mov     bx,cx
        ;
        push    bx
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        mov     ax,000bh
        cwAPI_CallOld
        pop     bx
        ;
        push    bx
        mov     al,0
        mov     ecx,d[@@SelectorSize]
        mov     esi,d[@@SelectorBase]
        cmp     ecx,0fffffh     ; see if we need to set g bit
        jc      @@2
        jz      @@2
        shr     ecx,12          ; div by 4096
        or      al,80h          ; set g bit
@@2:    mov     w[DescriptorBuffer+0],cx                ;store low word of limit.
        shr     ecx,16
        or      cl,al
        and     b[DescriptorBuffer+6],01110000b ;lose limit & G bit.
        or      b[DescriptorBuffer+6],cl                ;store high bits of limit and G bit.
        mov     w[DescriptorBuffer+2],si                ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     b[DescriptorBuffer+4],bl                ;store mid byte of linear base.
        mov     b[DescriptorBuffer+7],bh                ;store high byte of linear base.
        pop     bx
        ;
        ;Write new descriptor table entry.
        ;
        push    bx
        mov     ax,000ch
        mov     edi,offset DescriptorBuffer
        push    ds
        pop     es
        cwAPI_CallOld
        pop     bx
        jc      @@9
        ;
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
@@SelectorBase: ;
        dd ?
@@SelectorSize: ;
        dd ?
_SizeSelector   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_DetSelector    proc    near
;
;Fetch selector base & limit values.
;
;On Entry:-
;
;BX     - Selector.
;
;On Exit:-
;
;EAX    - Linear base address.
;EBX    - size in bytes.
;
        call    DPMICopyCheck
        ;
        pushm   ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        push    bx
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        mov     ax,000bh
        cwAPI_CallOld
        pop     bx
        jc      @@9
        ;
        xor     eax,eax
        mov     al,DescriptorBuffer+7   ;base 31-24
        shl     eax,8
        mov     al,DescriptorBuffer+4   ;base 23-16
        shl     eax,8
        mov     al,DescriptorBuffer+3   ;base 15-8
        shl     eax,8
        mov     al,DescriptorBuffer+2   ;base 7-0
        ;
        xor     ebx,ebx
        mov     bl,DescriptorBuffer+6   ;limit 19-16
        and     bl,0fh
        shl     ebx,8
        mov     bl,DescriptorBuffer+1   ;limit 15-8
        shl     ebx,8
        mov     bl,DescriptorBuffer+0   ;limit 7-0
        test    DescriptorBuffer+6,128  ;granularity
        jz      @@0
        shl     ebx,12
        or      ebx,0FFFh
@@0:    clc
        jmp     @@10
@@9:    stc
@@10:   popm    ecx,edx,esi,edi,ebp,ds,es,fs,gs
        ret
_DetSelector    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_CodeSelector   proc near
        call    DPMICopyCheck
        ;
        pushad
        call    _DSizeSelector
        popad
        ;
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        push    cx
        push    bx
        mov     ax,000bh
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        pop     bx
        pop     cx
        ;
        mov     ax,cs
        and     al,11b
        shl     al,5
        or      al,DescERCode
        and     DescriptorBuffer+5,10010000b
        or      DescriptorBuffer+5,al   ;
        and     DescriptorBuffer+6,255-(1 shl 6) ;clear code size bit.
        and     cl,1
        shl     cl,6
        or      DescriptorBuffer+6,cl   ;code size bit.
        ;
        mov     ax,000ch
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    ds,es,fs,gs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
@@11:   ret
_CodeSelector   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_DSizeSelector  proc near
        pushm   eax,ebx,ecx,edx,esi,edi,ebp
        pushm   ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        push    cx
        push    bx
        mov     ax,000bh
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        pop     bx
        pop     cx
        ;
        and     DescriptorBuffer+6,255-(1 shl 6) ;clear code size bit.
        and     cl,1
        shl     cl,6
        or      DescriptorBuffer+6,cl   ;code size bit.
        ;
        mov     ax,000ch
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        ;
        popm    ds,es,fs,gs
        popm    eax,ebx,ecx,edx,esi,edi,ebp
        ret
_DSizeSelector  endp


mcbChunkLast    equ     0       ;pointer to previous chunk in chain.
mcbChunkNext    equ     4       ;pointer to next chunk in chain.
mcbBiggest      equ     8       ;biggest free block in this chunk.
mcbChunkSize    equ     10      ;size of this chunk.
mcbChunkSel     equ     12      ;chunk's selector.
mcbChunkLen     equ     16      ;length of chunk control entry.

mcbID   equ     0       ;ID for corruption checking.
mcbLast equ     1       ;previous MCB pointer status.
mcbLastSize     equ     2       ;pointer to previous MCB.
mcbFreeUsed     equ     4       ;free or used status.
mcbNext equ     5       ;next MCB pointer status.
mcbNextSize     equ     6       ;pointer to next MCB.
mcbLen  equ     8       ;length of an MCB entry.


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get current MCB memory allocation block size.
;
;On Exit:
;
;ECX    - Current threshold.
;
cwAPI_GetMCBSize proc near
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        mov     ecx,ds:d[EPSP_mcbMaxAlloc]
        add     ecx,mcbChunkLen+mcbLen
        assume ds:_apiCode
        mov     [ebp+Int_ECX],ecx
        ret
cwAPI_GetMCBSize endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set new MCB memory allocation block size.
;
;On Entry:
;
;ECX    - New value to set.
;
;On Exit:
;
;Carry set on error else new value will be used.
;
cwAPI_SetMCBSize proc near
        mov     ecx,[ebp+Int_ECX]
        add     ecx,4095
        and     ecx,not 4095
        cmp     ecx,65536+1
        jc      @@1
        stc
        jmp     @@9
        ;
@@1:    assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        cmp     ecx,mcbChunkLen+mcbLen
        jc      @@9
        sub     ecx,mcbChunkLen+mcbLen
@@0:    mov     ds:d[EPSP_mcbMaxAlloc],ecx
        assume ds:_apiCode
        clc
;
@@9:    cwAPI_C2C
        ret
cwAPI_SetMCBSize endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Replacement for CauseWay API GetMemLinear32 function.
;
mcbGetMemLinear32 proc near
        push    ds
        push    es
        push    fs
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    ebp
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSEg
        assume ds:_apiCode
        mov     mcbLastChunk,0
        assume ds:_cwMain
        pop     ds
;
;Round size up to next dword to keep things aligned.
;
        cmp     ecx,-1
        jz      @@GetMax
        cmp     ecx,-2
        jz      @@GetMax
        add     ecx,3
        and     ecx,not 3
;
;Check MCB allocation system is enabled.
;
        cmp     mcbAllocations,0
        jz      @@GetMax
;
;Check block size is small enough for these functions.
;
        cmp     ecx,fs:d[EPSP_mcbMaxAlloc]
        jc      mGML32_0
;
;Size is above threshold so use normal API service for this request.
;
@@GetMax:       mov     ebx,ecx
        call    _GetMemory
        jnc     mGML32_9
        cmp     ecx,-2
        jz      @@RetMax
        cmp     ecx,-1
        stc
        jnz     mGML32_9
@@RetMax:       pop     ebp
        pop     edi
        pop     edx
        pop     ecx
        mov     ecx,ebx
        push    ecx
        push    edx
        push    edi
        push    ebp
        stc
        jmp     mGML32_9
;
;Size is below threshold so use local allocation scheme.
;
mGML32_0:
;
;Check if initial block has been claimed yet, allocate and initialise one if not.
;
        cmp     fs:d[EPSP_mcbHead],0
        jnz     mGML32_10
        mov     ebx,fs:d[EPSP_mcbMaxAlloc]
        add     ebx,mcbChunkLen+mcbLen  ;chunk size.
        call    _GetMemory              ;allocate it.
        jc      mGML32_9                ;oops, appear to be out of memory.
        mov     fs:d[EPSP_mcbHead],esi
;
;Initialise this chunk.
;
        mov     es,RealSegment
        sub     ebx,mcbChunkLen+mcbLen  ;chunk size.
        mov     es:w[esi+mcbChunkSize],bx
        mov     es:d[esi+mcbChunkNext],0        ;set forward link address.
        mov     es:d[esi+mcbChunkLast],0        ;store back link address.
        mov     eax,fs:d[EPSP_mcbMaxAlloc]
        mov     es:w[esi+mcbBiggest],ax ;set biggest chunk size.
        sys     GetSel
        jc      mGML32_9
        mov     es:w[esi+mcbChunkSel],bx
        pushm   edx,ecx
        mov     edx,esi
        mov     ecx,fs:d[EPSP_mcbMaxAlloc]
        add     ecx,mcbChunkLen+mcbLen
        sys     SetSelDet32
        popm    edx,ecx
        add     esi,mcbChunkLen ;skip chunk link info.
        mov     es:b[esi+mcbID],"C"     ;set ID byte.
        mov     es:b[esi+mcbLast],"D"   ;mark it as last block in back link.
        mov     es:w[esi+mcbLastSize],0 ;clear back link entry.
        mov     es:b[esi+mcbFreeUsed],"J"       ;mark it as a free block,
        mov     es:b[esi+mcbNext],"D"   ;last block in MCB chain,
        mov     eax,fs:d[EPSP_mcbMaxAlloc]
        mov     es:w[esi+mcbNextSize],ax
;
;Scan through all mcb's in all chunks looking for a big enough block.
;
mGML32_10:      mov     es,RealSegment  ;0-4G selector.
        mov     esi,fs:d[EPSP_mcbHead]  ;start of local allocation trail.
mGML32_1:       mov     edi,esi         ;keep a copy for chunk chaining.
        mov     ebp,edi         ;keep a copy for mcbBiggest
        ;
        cmp     es:w[edi+mcbBiggest],cx ;check if this chunk has a big
        jc      mGML32_6_0              ;enough free block.
        add     esi,mcbChunkLen
;
;Find first free and big enough block.
;
mGML32_2:       cmp     es:b[esi+mcbFreeUsed],"J"       ;Free block?
        jz      mGML32_5
mGML32_6:       cmp     es:b[esi+mcbNext],"M"   ;Normal block (not end of chain)?
        jz      mGML32_4
;
;Reached the end of the chain for this chunk so we need to move onto the next
;chunk in the chain.
;
mGML32_6_0:     cmp     es:d[edi+mcbChunkNext],0        ;already have a link?
        jnz     mGML32_3
;
;Need another chunk to put in the chain so try and allocate it via normal
;CauseWay API.
;
        mov     ebx,fs:d[EPSP_mcbMaxAlloc]
        add     ebx,mcbChunkLen+mcbLen  ;chunk size.
        call    _GetMemory
        jc      mGML32_9                ;oops, appear to be out of memory.
;
;Update current chunk with address of new chunk and initialise new chunk.
;
        sub     ebx,mcbChunkLen+mcbLen  ;chunk size.
        mov     es:w[esi+mcbChunkSize],bx
        mov     es:d[edi+mcbChunkNext],esi ;store forward link address.
        mov     es:d[esi+mcbChunkLast],edi ;store back link address.
        mov     es:d[esi+mcbChunkNext],0        ;clear new forward link address.
        mov     eax,fs:d[EPSP_mcbMaxAlloc]
        mov     es:w[esi+mcbBiggest],ax ;set biggest chunk size.
        sys     GetSel
        jc      mGML32_9
        mov     es:w[esi+mcbChunkSel],bx
        pushm   edx,ecx
        mov     edx,esi
        mov     ecx,fs:d[EPSP_mcbMaxAlloc]
        add     ecx,mcbChunkLen+mcbLen
        sys     SetSelDet32
        popm    edx,ecx
        add     esi,mcbChunkLen ;skip chunk link info.
        mov     es:b[esi+mcbID],"C"     ;set ID.
        mov     es:b[esi+mcbLast],"D"   ;mark it as last block in back link.
        mov     es:w[esi+mcbLastSize],0 ;clear back link entry.
        mov     es:b[esi+mcbFreeUsed],"J"       ;mark it as a free block,
        mov     es:b[esi+mcbNext],"D"   ;last block in MCB chain,
        mov     eax,fs:d[EPSP_mcbMaxAlloc]
        mov     es:w[esi+mcbNextSize],ax
;
;Chain to next chunk.
;
mGML32_3:       mov     esi,es:d[edi+mcbChunkNext] ;pickup forward link address.
        jmp     mGML32_1                ;scan this MCB chain.
;
;Move to next MCB.
;
mGML32_4:       movzx   eax,es:w[esi+mcbNextSize]       ;get block length.
        add     eax,mcbLen              ;include size of an MCB.
        add     esi,eax
        jmp     mGML32_2
;
;Check if this block is big enough.
;
mGML32_5:       cmp     es:w[esi+mcbNextSize],cx        ;Big enough block?
        jc      mGML32_6
;
;Found a big enough free block so make use of it.
;
        mov     es:b[esi+mcbFreeUsed],"W"       ;mark it as used.
        movzx   ebx,es:w[esi+mcbNextSize]
        sub     ebx,ecx         ;get spare size.
        cmp     ebx,mcbLen+1            ;smaller than an MCB?
        jc      mGML32_8
;
;Create a new MCB from whats left over.
;
        sub     bx,mcbLen               ;MCB comes out of this size.
        mov     es:[esi+mcbNextSize],cx ;set allocated block's size.
        mov     al,es:b[esi+mcbNext]    ;get next status.
        mov     es:b[esi+mcbNext],"M"   ;make sure its not end of chain now.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen              ;move to where new MCB will be.
        mov     es:b[edi+mcbID],"C"
        mov     es:b[edi+mcbLast],"M"   ;not last in back link chain.
        mov     es:[edi+mcbLastSize],cx ;set back link size.
        mov     es:b[edi+mcbFreeUsed],"J"       ;mark as free,
        mov     es:[edi+mcbNext],al     ;set end of chain status.
        mov     es:[edi+mcbNextSize],bx ;and new block size.
;
;Check if old block used to be end of chain.
;
        cmp     al,"D"          ;end of chain?
        jz      mGML32_8
;
;Update back link size of next block.
;
        movzx   eax,es:w[edi+mcbNextSize]
        add     eax,mcbLen              ;include MCB size.
        add     edi,eax
        mov     es:[edi+mcbLastSize],ax
        sub     es:w[edi+mcbLastSize],mcbLen
;
;Time to exit.
;
mGML32_8:       add     esi,mcbLen              ;skip the MCB.
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     mcbLastChunk,ebp
        assume ds:_cwMain
        pop     ds
        call    mcbSetBiggest
        clc
;
;Restore stacked registers.
;
mGML32_9:       pop     ebp
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        pop     fs
        pop     es
        pop     ds
        ret
        assume ds:_apiCode
mcbGetMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Replacement for CauseWay API ResMemLinear32 function.
;
mcbResMemLinear32 proc near
        push    ds
        push    es
        push    fs
        push    eax
        push    ebx
        push    ecx
        push    edi
        push    ebp
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     mcbLastChunk,0
        assume ds:_cwMain
        pop     ds
        ;
        mov     edi,esi
        add     ecx,3
        and     ecx,not 3
;
;See if MCB allocations are enabled.
;
        cmp     mcbAllocations,0
        jz      mRsML32_30
;
;See if EDI is within the range of any of the chunks in the list.
;
        mov     es,RealSegment
        mov     esi,fs:d[EPSP_mcbHead]
        or      esi,esi         ;check mcb's are active.
        jz      mRsML32_8
        ;
mRsML32_0:      cmp     edi,esi
        jc      mRsML32_1
        movzx   eax,es:w[esi+mcbChunkSize]
        add     eax,esi
        add     eax,mcbChunkLen+mcbLen  ;chunk size.
        cmp     edi,eax
        jc      mRsML32_2
mRsML32_1:      cmp     es:d[esi+mcbChunkNext],0        ;Next link field set?
        jz      mRsML32_8
        mov     esi,es:d[esi+mcbChunkNext]
        jmp     mRsML32_0
;
;In range of a chunk so deal with it here.
;
mRsML32_2:      xchg    edi,esi
        mov     ebp,edi
;
;Check block size is small enough for these functions.
;
        cmp     ecx,fs:d[EPSP_mcbMaxAlloc]
        jc      mRsML32_3
;
;Can't deal with a block this big so convert it to a normal API block.
;
        mov     edi,esi
        mov     ebx,ecx
        call    _GetMemory              ;try and allocate a normal block.
        jc      mRsML32_9
;
;Copy existing block to new block.
;
        push    ecx
        push    esi
        push    edi
        xchg    esi,edi
        sub     esi,mcbLen
        movzx   ecx,es:w[esi+mcbNextSize]
        add     esi,mcbLen
        push    ds
        mov     ds,RealSegment
        ;
        push    ecx
        shr     ecx,2
        rep     movsd
        pop     ecx
        and     ecx,3
        rep     movsb
        pop     ds
        pop     esi
;
;Release origional block and return address of new block.
;
        call    mcbRelMemLinear32
        pop     esi
        pop     ecx
        jmp     mRsML32_9
;
;Get block's current size.
;
mRsML32_3:      sub     esi,mcbLen              ;move back to MCB.
        cmp     cx,es:[esi+mcbNextSize]
        jz      mRsML32_7
        jnc     mRsML32_4               ;extending block.
;
;Block is shrinking so build another MCB at the end of this one.
;
        movzx   ebx,es:w[esi+mcbNextSize]
        sub     ebx,ecx         ;get size differance.
        cmp     ebx,mcbLen+1            ;enough for a new MCB?
        jnc     @@0
;
;Before we abandon this block size change as too small we should check if the
;next block is free and join the new space onto that if it is.
;
        cmp     es:b[esi+mcbNext],"M"   ;end of the chain?
        jnz     mRsML32_7               ;yep, can't be another block.
        mov     edi,esi
        movzx   eax,es:w[esi+mcbNextSize]
        add     eax,mcbLen
        add     edi,eax         ;point to the next block.
        cmp     es:b[edi+mcbFreeUsed],"J"       ;Free block?
        jnz     mRsML32_7               ;no, so leave things alone.
        mov     es:[esi+mcbNextSize],cx ;set new size.
        push    esi
        push    ecx
        mov     eax,ecx
        add     eax,mcbLen
        add     esi,eax
        xchg    esi,edi
        mov     ecx,mcbLen
        pushm   ds,es
        pop     ds
        rep     movsb
        pop     ds
        sub     edi,mcbLen
        add     es:[edi+mcbNextSize],bx ;update block size.
        sub     es:[edi+mcbLastSize],bx ;update last size.
        pop     ecx
        pop     esi
        cmp     es:b[edi+mcbNext],"M"   ;end of chain?
        jnz     mRsML32_7
        movzx   eax,es:w[edi+mcbNextSize]
        add     edi,eax
        add     edi,mcbLen
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7               ;leave things as they are.
        ;
@@0:    sub     ebx,mcbLen              ;need space for a new MCB.
        mov     es:[esi+mcbNextSize],cx ;set new size.
        mov     al,es:[esi+mcbNext]     ;get next status.
        mov     es:b[esi+mcbNext],"M"   ;force not end of chain.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen              ;move to new MCB.
        mov     es:b[edi+mcbID],"C"     ;set ID.
        mov     es:b[edi+mcbLast],"M"   ;not last in last chain.
        mov     es:[edi+mcbLastSize],cx ;set last link size.
        mov     es:b[edi+mcbNext],al    ;set next status.
        mov     es:b[edi+mcbFreeUsed],"J"       ;mark it as free.
        mov     es:[edi+mcbNextSize],bx ;set next link size.
        cmp     al,"D"          ;end of chain?
        jz      mRsML32_7
;
;Update last link size of next MCB.
;
        movzx   eax,es:w[edi+mcbNextSize]
        mov     ebx,edi
        add     ebx,eax
        add     ebx,mcbLen              ;move to next block.
        mov     es:[ebx+mcbLastSize],ax
;
;Check if next block is free and join it to the newly created block if it is,
;
        cmp     es:b[ebx+mcbFreeUsed],"J"       ;Free block?
        jnz     mRsML32_7
;
;Join next block to this one.
;
        movzx   eax,es:w[ebx+mcbNextSize]       ;get block's size.
        add     eax,mcbLen              ;include size of an mcb.
        add     es:[edi+mcbNextSize],ax
        mov     al,es:[ebx+mcbNext]
        mov     es:[edi+mcbNext],al     ;copy next status.
        cmp     al,"D"          ;end of chain?
        jz      mRsML32_7
;
;Update next blocks last link size.
;
        movzx   eax,es:w[edi+mcbNextSize]
        add     edi,eax
        add     edi,mcbLen              ;move to next block.
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7
;
;Need to expand the block so check out the block above this one.
;
mRsML32_4:      cmp     es:b[esi+mcbNext],"D"   ;end of the chain?
        jz      mRsML32_6
        mov     edi,esi
        movzx   eax,es:w[esi+mcbNextSize]
        add     eax,mcbLen              ;move to next block.
        add     edi,eax
        cmp     es:b[edi+mcbFreeUsed],"J"       ;This block free?
        jnz     mRsML32_6
        mov     ebx,ecx
        sub     bx,es:w[esi+mcbNextSize]        ;Get size needed.
        movzx   eax,es:w[edi+mcbNextSize]
        add     eax,mcbLen
        cmp     eax,ebx         ;Big enough block?
        jc      mRsML32_6
;
;Next block is big enough, is it big enough to leave a free block behind still?
;
        sub     eax,ebx         ;Get size differance.
        cmp     eax,mcbLen+1
        jnc     mRsML32_5
;
;Swollow new block whole and update next blocks last link entry.
;
        movzx   eax,es:w[edi+mcbNextSize]
        add     eax,mcbLen
        add     es:[esi+mcbNextSize],ax ;update block size.
        mov     al,es:[edi+mcbNext]     ;get next status.
        mov     es:[esi+mcbNext],al
        cmp     al,"D"          ;end of the chain?
        jz      mRsML32_7
        movzx   eax,es:w[esi+mcbNextSize]
        mov     edi,esi
        add     edi,eax
        add     edi,mcbLen
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7
;
;Create a new MCB in the right place.
;
mRsML32_5:      mov     al,es:[edi+mcbNext]     ;Get next status.
        movzx   ebx,es:w[edi+mcbNextSize]       ;Get size of this block.
        add     bx,es:w[esi+mcbNextSize]
        mov     es:[esi+mcbNextSize],cx ;set new size of this block.
        sub     ebx,ecx         ;get size remaining.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen
        mov     es:b[edi+mcbID],"C"     ;set ID.
        mov     es:b[edi+mcbLast],"M"   ;not last in last chain.
        mov     es:[edi+mcbLastSize],cx ;last link size.
        mov     es:b[edi+mcbFreeUsed],"J"       ;mark it as free again.
        mov     es:[edi+mcbNext],al     ;set next status.
        mov     es:[edi+mcbNextSize],bx ;set next link size.
        cmp     al,"D"          ;end of the chain?
        jz      mRsML32_7
        add     edi,ebx
        add     edi,mcbLen              ;move to next block.
        mov     es:[edi+mcbLastSize],bx
        jmp     mRsML32_7
;
;Have to try and allocate another block and copy the current blocks contents.
;
mRsML32_6:      add     esi,mcbLen
        mov     edi,esi
        call    mcbGetMemLinear32
        jc      mRsML32_9
        push    ecx
        push    esi
        push    edi
        xchg    esi,edi
        sub     esi,mcbLen
        movzx   ecx,es:w[esi+mcbNextSize]
        add     esi,mcbLen
        push    ds
        mov     ds,RealSegment
        push    ecx
        shr     ecx,2
        rep     movsd
        pop     ecx
        and     ecx,3
        rep     movsb
        pop     ds
        pop     esi
        call    mcbRelMemLinear32       ;release origional block.
        pop     esi
        pop     ecx
        clc
        jmp     mRsML32_9
;
;Setup block address to return and exit.
;
mRsML32_7:      add     esi,mcbLen
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     mcbLastChunk,ebp
        assume ds:_cwMain
        pop     ds
        call    mcbSetBiggest
        clc
        jmp     mRsML32_9
;
;Not in range of any local chunks so pass it to CauseWay API function.
;
mRsML32_8:      cmp     ecx,fs:d[EPSP_mcbMaxAlloc]
        jc      mRsML32_11
mRsML32_30:     mov     esi,edi
        mov     ebx,ecx
        call    _ResMemory
        jmp     mRsML32_9
;
;Convert normal API block to local MCB block. This assumes that a none MCB block
;comeing through here is bigger than mcbMaxAlloc and therefore is shrinking.
;
mRsML32_11:     call    mcbGetMemLinear32
        jc      mRsML32_9
;
;Copy origional block contents.
;
        push    esi
        push    edi
        xchg    esi,edi
        push    ds
        mov     ds,RealSegment
        push    ecx
        shr     ecx,2
        rep     movsd
        pop     ecx
        and     ecx,3
        rep     movsb
        pop     ds
        pop     esi
        call    _RelMemory              ;release origional block.
        pop     esi
;
;Return to caller.
;
mRsML32_9:      pop     ebp
        pop     edi
        pop     ecx
        pop     ebx
        pop     eax
        pop     fs
        pop     es
        pop     ds
        ret
        assume ds:_apiCode
mcbResMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Replacement for CauseWay API RelMemLinear32 function.
;
mcbRelMemLinear32 proc near
        push    ds
        push    es
        push    fs
        pushad
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        mov     edi,esi
;
;See if MCB blocks are enabled.
;
        cmp     mcbAllocations,0
        jz      mRML32_8
;
;See if EDI is within the range of any of the chunks in the list.
;
        mov     es,RealSegment
        mov     esi,fs:d[EPSP_mcbHead]
        or      esi,esi         ;check mcb's are active.
        jz      mRML32_8
        ;
mRML32_0:       cmp     edi,esi
        jc      mRML32_1
        movzx   eax,es:w[esi+mcbChunkSize]
        add     eax,esi
        add     eax,mcbChunkLen+mcbLen  ;chunk size.
        cmp     edi,eax
        jc      mRML32_2
mRML32_1:       cmp     es:d[esi+mcbChunkNext],0        ;Next link field set?
        jz      mRML32_8
        mov     esi,es:d[esi+mcbChunkNext]
        jmp     mRML32_0
;
;In range of a chunk so deal with it here.
;
mRML32_2:       xchg    edi,esi
        mov     ebp,edi
        sub     esi,mcbLen
        mov     es:b[esi+mcbFreeUsed],"J"       ;mark it as free.
;
;Check if next block is free and join it to this one if it is.
;
mRML32_11:      cmp     es:b[esi+mcbNext],"D"   ;last block in chain?
        jz      mRML32_3
        movzx   eax,es:w[esi+mcbNextSize]
        mov     ebx,esi
        add     eax,mcbLen
        add     ebx,eax
        cmp     es:b[ebx+mcbFreeUsed],"J"       ;free block?
        jnz     mRML32_3
        movzx   eax,es:w[ebx+mcbNextSize]
        add     eax,mcbLen
        add     es:[esi+mcbNextSize],ax ;update block size.
        mov     al,es:[ebx+mcbNext]
        mov     es:[esi+mcbNext],al     ;copy next status.
        cmp     al,"D"          ;last block in chain?
        jz      mRML32_3
;
;Update next blocks last block pointer.
;
        movzx   eax,es:w[esi+mcbNextSize]
        add     eax,mcbLen
        add     esi,eax
        mov     es:[esi+mcbLastSize],ax
        sub     es:w[esi+mcbLastSize],mcbLen
        sub     esi,eax
;
;Check if current block is end of next chain and end of last chain. If it
;is we can release this chunk because it isn't being used anymore.
;
mRML32_3:       cmp     es:b[esi+mcbNext],"M"
        jz      mRML32_4
        cmp     es:b[esi+mcbLast],"M"
        jz      mRML32_4
;
;Un-link and release this chunk, its not being used.
;
        mov     esi,edi         ;Get chunk address.
        mov     edi,es:[esi+mcbChunkLast]
        mov     eax,es:[esi+mcbChunkNext]
        or      edi,edi         ;First chunk?
        jnz     mRML32_5
        mov     fs:d[EPSP_mcbHead],eax  ;Set new head chunk, 0 is valid.
        jmp     mRML32_6
mRML32_5:       mov     es:[edi+mcbChunkNext],eax       ;link to next chunk.
mRML32_6:       xchg    eax,edi
        or      edi,edi         ;is there a next chunk?
        jz      mRML32_7
        mov     es:[edi+mcbChunkLast],eax       ;link to last chunk.
mRML32_7:       mov     bx,es:[esi+mcbChunkSel]
        sys     RelSel          ;release this selector.
        call    _RelMemory              ;release this memory for real.
        jmp     mRML32_9                ;exit, we're all done.
;
;Check if previous block is free and join this one to it if it is.
;
mRML32_4:       cmp     es:b[esi+mcbLast],"M"   ;last block in last chain?
        jnz     mRML32_10
;
;Move back to previous block, see if its free and let the next block join code
;deal with it if it is.
;
        movzx   eax,es:w[esi+mcbLastSize]
        add     eax,mcbLen
        sub     esi,eax
        cmp     es:b[esi+mcbFreeUsed],"J"
        jz      mRML32_11
;
;Finished but we couldn't release this chunk.
;
mRML32_10:      call    mcbSetBiggest
        clc
        jmp     mRML32_9
;
;Not in range of any local chunks so pass it to CauseWay API function.
;
mRML32_8:       mov     esi,edi
        call    _RelMemory
        ;
mRML32_9:       popad
        pop     fs
        pop     es
        pop     ds
        ret
        assume ds:_apiCode
mcbRelMemLinear32 endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set mcbBiggest entry in chunk header.
;
;On Entry:
;
;ES:EBP - Chunk header.
;
mcbSetBiggest   proc    near
        pushm   eax,ecx,esi
        mov     esi,ebp
        add     esi,mcbChunkLen
        xor     ecx,ecx
@@0:    cmp     es:b[esi+mcbFreeUsed],"J"
        jnz     @@1
        cmp     cx,es:w[esi+mcbNextSize]
        jnc     @@1
        mov     cx,es:w[esi+mcbNextSize]
@@1:    cmp     es:b[esi+mcbNext],"M"   ;last block in last chain?
        jnz     @@2
        movzx   eax,es:w[esi+mcbNextSize]
        add     esi,eax
        add     esi,mcbLen
        jmp     @@0
@@2:    mov     es:w[ebp+mcbBiggest],cx
        popm    eax,ecx,esi
        ret
mcbSetBiggest   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_GetMemory      proc    near
;
;Attempt to allocate a block of memory.
;
;On Entry:-
;
;EBX    - Size of block required. (-1 to get size of free memory).
;
;On Exit:-
;
;Carry set on error, else:-
;
;ESI    - linear address of memory.
;
;ALL registers may be corrupted.
;
        call    DPMICopyCheck
        ;
        pushm   ecx,edx,edi,ebp,ds,es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pushm   d[@@BlockSize],d[@@BlockBase],d[@@BlockHandle]
        ;
        cmp     ebx,-2
        jz      @@Special
        cmp     ebx,-1          ;special value to get memory free?
        jnz     @@NotSpecial
@@Special:      call    _GetMemoryMax   ;call free memory code.
        stc
        popm    d[@@BlockSize],d[@@BlockBase],d[@@BlockHandle]
        popm    ecx,edx,edi,ebp,ds,es
        ret
        ;
@@NotSpecial:   mov     d[@@BlockSize],ebx      ;store real block size.
        ;
        mov     cx,bx
        shr     ebx,16
        mov     ax,0501h                ;Allocate memory block.
        cwAPI_CallOld
        jc      @@9
        shl     ebx,16
        mov     bx,cx
        mov     d[@@BlockBase],ebx      ;store linear base address.
        shl     esi,16
        mov     si,di
        mov     d[@@BlockHandle],esi    ;store access handle.
        ;
        mov     ax,Res_MEM
        mov     edx,ebx
        mov     ecx,esi
        mov     ebx,d[@@BlockSize]
        call    RegisterResource
        ;
        mov     esi,edx
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    d[@@BlockSize],d[@@BlockBase],d[@@BlockHandle]
        popm    ecx,edx,edi,ebp,ds,es
        ret
@@BlockBase:    ;
        dd ?
@@BlockHandle:  ;
        dd ?
@@BlockSize:    ;
        dd ?
_GetMemory      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_GetMemoryMax proc near
;
;Work out biggest memory block remaining.
;
        call    DPMICopyCheck
        ;
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;

COMMENT ! MED 02/15/96
        cmp     ebx,-1          ;normal max reporting?
        jz      @@normal

        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        pop     ds
        jz      @@normal

        ;
        ;Get free disk space remaining.
        ;
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw

        pushm   eax,ebx,ecx,esi,edi
        xor     edx,edx
        cmp     VMMHandle,0
        jz      @@500_1
        mov     dl,VMMName              ;get drive letter for this media.
        sub     dl,'A'          ;make it real.
        inc     dl              ;adjust for current type select.
        mov     ah,36h          ;get free space.
        int     21h             ;/
        xor     edx,edx
        cmp     ax,-1           ;invalid drive?
        jz      @@500_1
        mul     cx              ;Get bytes per cluster.
        mul     bx              ;Get bytes available.
        shl     edx,16
        mov     dx,ax
        add     edx,SwapFileLength      ;include current size.
        shr     edx,12
        ;
        ;Work out how much of the VMM space is extra.
        ;
        mov     eax,LinearLimit
        sub     eax,LinearBase
        shr     eax,12
        sub     edx,eax
        jmp     @@500_2
        ;
@@500_1:
        mov     edx,TotalPhysical
@@500_2:        add     edx,FreePages
        ;
        popm    eax,ebx,ecx,esi,edi

        assume ds:_apiCode
        pop     ds
        mov     ebx,edx
        shl     ebx,12
        jmp     @@exit
END COMMENT !

@@normal:       push    es
        mov     edi,offset @@dpmembuff
        push    ds
        pop     es
        push    ebx
        mov     ax,0500h
        cwAPI_CallOld
        pop     ebx
        pop     es

        cmp     ebx,-2
        jnz     @@normal2

        mov     ebx,d[@@dpmembuff+1Ch]
        shl     ebx,12
        jmp     @@exit

@@normal2:      mov     ebx,d[@@dpmembuff]
        ;
@@exit: pop     ds
        ret
@@dpmembuff:    ;
        db 30h dup (0)
_GetMemoryMax   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_ResMemory      proc    near
;
;Re-size a block of memory.
;
;On Entry:-
;
;ESI    - Linear address of memory.
;EBX    - New size.
;
;On Exit:-
;
;ESI    - New linear address of memory.
;
        call    DPMICopyCheck
        ;
        pushm   eax,ebx,ecx,edx,edi,ebp,ds,es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    d[@@OldESI]
        mov     d[@@OldESI],esi
        push    d[@@OldEBX]
        mov     d[@@OldEBX],ebx
        pushm   LastResource,LastResource+4
        ;
        mov     edx,esi
        mov     ax,Res_MEM
        call    FindResource
        jc      @@9
        ;
        mov     ebx,d[@@OldEBX]
        mov     esi,ecx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16          ;set block size.
        mov     ax,0503h                ;release the block.
        cwAPI_CallOld
        jc      @@9
        shl     ebx,16
        mov     bx,cx
        shl     esi,16
        mov     si,di
        xchg    esi,ebx
        ;
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edx,LastResource
        mov     es:[edx],esi            ;update base.
        add     edx,4
        mov     es:[edx],ebx            ;update handle.
        add     edx,4
        mov     ebx,d[@@OldEBX]
        mov     es:[edx],ebx            ;update size.
        ;
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popm    LastResource,LastResource+4
        pop     d[@@OldEBX]
        pop     d[@@OldESI]
        popm    eax,ebx,ecx,edx,edi,ebp,ds,es
        ret
@@OldEBX:       ;
        dd 0
@@OldESI:       ;
        dd 0
_ResMemory      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_RelMemory      proc    near
;
;Release a previously allocated block of memory.
;
;On Entry:-
;
;ESI    - Near DS pointer.
;
        call    DPMICopyCheck
        ;
        pushm   ds,es
        pushad
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pushm   LastResource,LastResource+4
        ;
        mov     edx,esi
        mov     ax,Res_MEM
        call    FindResource
        jc      @@9
        ;
        mov     esi,ecx
        mov     di,si
        shr     esi,16
        mov     ax,0502h                ;release the block.
        cwAPI_CallOld
        jc      @@9
        ;
        mov     di,bx
        shr     ebx,16
        mov     si,bx
        mov     cx,dx
        shr     edx,16
        mov     bx,dx
        mov     ax,0703h                ;discard the block.
        cwAPI_CallOld
        ;
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     esi,LastResource
        mov     edi,LastResource+4
        xor     eax,eax
        mov     es:[edi],ax
        mov     es:[edi+2],al
        mov     es:[esi],eax
        mov     es:[esi+4],eax
        mov     es:[esi+8],eax
        ;
        clc
        ;
@@9:    popm    LastResource,LastResource+4
        popad
        popm    ds,es
        ret
_RelMemory      endp


;-------------------------------------------------------------------------
;
;Register a new resource.
;
;On Entry:
;
;AL     - Resource type to register.
;EDX    - Key value.
;ECX    - optional second value.
;EBX    - optional third value.
;
RegisterResource proc near
        pushm   ds,es,fs
        pushad
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     es,RealSegment
        mov     fs,PSPSegment
        ;
        cmp     ResourceTracking,0
        jz      @@9
        cmp     fs:d[EPSP_Resource],0   ;allocated memory for list yet?
        jnz     @@0
        ;
        ;Allocate initial resource list space.
        ;
        pushad
        call    @@GetAndInit
        jc      @@1
        mov     fs:d[EPSP_Resource],esi
@@1:    popad
        cmp     fs:d[EPSP_Resource],0
        jz      @@90
        ;
@@0:    ;Search list for free slot.
        ;
        cld
        mov     ebp,ecx         ;Copy value 2.
        mov     ah,al           ;Copy type.
        mov     esi,fs:d[EPSP_Resource]
@@FreeLoop:     mov     edi,esi
        add     edi,ResHead             ;point to types.
        mov     ecx,ResCount            ;get number of entries.
@@2:    or      ecx,ecx
        jz      @@2_0
        js      @@2_0
        xor     al,al
        repne   scasb           ;Find NULL entry.
        jz      @@3
@@2_0:  cmp     es:d[esi+8],0   ;link field setup?
        jz      @@Extend
        mov     esi,es:[esi+8]  ;link to next block.
        jmp     @@FreeLoop
        ;
@@Extend:       ;Extend the list.
        ;
        push    esi
        call    @@GetAndInit
        pop     ecx
        jc      @@90
        mov     es:[esi+4],ecx  ;store back link address.
        mov     es:[ecx+8],esi  ;store forward link address.
        jmp     @@FreeLoop
;
;Check if enough entries are free.
;
@@3:    cmp     ah,Res_SEL
        jz      @@sel
        cmp     ah,Res_MEM
        jz      @@mem
        cmp     ah,Res_LOCK
        jz      @@lock
        cmp     ah,Res_DOSMEM
        jz      @@dosmem
        cmp     ah,Res_CALLBACK
        jz      @@callback
        cmp     ah,Res_PSP
        jz      @@psp
        jmp     @@sel
;
;Check for 2 free entries for DOS memory.
;
@@dosmem:       cmp     es:b[edi],0
        jnz     @@2
        dec     edi
        mov     ecx,edi
        sub     ecx,ResHead             ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,ResNum+ResHead
        add     ecx,esi
        mov     es:b[edi],ah            ;Store type.
        mov     es:d[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     es:b[edi],ah
        mov     es:d[ecx],ebp
        jmp     @@9
;
;Check for 3 free entries for callback.
;
@@callback:     cmp     es:w[edi],0             ;2 more entries?
        jnz     @@2
        dec     edi
        mov     ecx,edi
        sub     ecx,ResHead             ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,ResNum+ResHead
        add     ecx,esi
        mov     es:b[edi],ah            ;Store type.
        mov     es:d[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     es:b[edi],ah
        mov     es:d[ecx],ebp
        inc     edi
        add     ecx,4
        mov     es:b[edi],ah
        mov     es:d[ecx],ebx
        jmp     @@9
;
;Check for 3 free entries for memory.
;
@@mem:  cmp     es:w[edi],0             ;2 more entries?
        jnz     @@2
        dec     edi
        mov     ecx,edi
        sub     ecx,ResHead             ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,ResNum+ResHead
        add     ecx,esi
        mov     es:b[edi],ah            ;Store type.
        mov     es:d[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     es:b[edi],ah
        mov     es:d[ecx],ebp
        inc     edi
        add     ecx,4
        mov     es:b[edi],ah
        mov     es:d[ecx],ebx
        jmp     @@9
;
;Check for 2 free entries for lock's.
;
@@lock: cmp     es:b[edi],0
        jnz     @@2
        dec     edi
        mov     ecx,edi
        sub     ecx,ResHead             ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,ResNum+ResHead
        add     ecx,esi
        mov     es:b[edi],ah            ;Store type.
        mov     es:d[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     es:b[edi],ah
        mov     es:d[ecx],ebp
        jmp     @@9
;
;Only one free entry needed for selectors.
;
@@psp:  ;
@@sel:  dec     edi
        mov     ecx,edi
        sub     ecx,ResHead             ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,ResNum+ResHead
        add     ecx,esi
        mov     es:b[edi],ah            ;Store type.
        mov     es:d[ecx],edx
        jmp     @@9
        ;
@@90:   stc
        jmp     @@100
        ;
@@9:    clc
@@100:  popad
        popm    ds,es,fs
        ret
        ;
@@GetAndInit:   pushm   eax,ebx,ecx,edx,ebp
        mov     cx,4096
        xor     bx,bx
        mov     ax,0501h                ;Allocate memory block.
        cwAPI_CallOld
        jc      @@GAIerror
        shl     esi,16
        mov     si,di
        shl     ebx,16
        mov     bx,cx
        xchg    esi,ebx
        mov     edi,esi
        xor     eax,eax
        mov     ecx,ResSize/4
        cld
        rep     stosd           ;init memory.
        mov     es:d[esi+12],ebx        ;store the handle.
        clc
@@GAIerror:     popm    eax,ebx,ecx,edx,ebp
        ret
        assume ds:_apiCode
RegisterResource endp


;-------------------------------------------------------------------------
;
;Release a resource entry. If it's a PSP then release its resources.
;
;On Entry:
;
;AL     - Resource type.
;EDX    - Key value.
;
ReleaseResource proc near
        pushm   ds,es,fs
        pushad
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        cmp     ResourceTracking,0
        jz      @@9
        cmp     fs:d[EPSP_Resource],0
        jz      @@9
        xchg    eax,edx
        mov     es,RealSegment
        cld
        mov     esi,fs:d[EPSP_Resource]
@@LookLoop:     mov     edi,esi
        add     edi,ResHead+ResNum
        mov     ecx,ResNum              ;get number of entries.
@@2:    repne   scasd
        jz      @@0
        mov     esi,es:[esi+8]  ;link to next list.
        or      esi,esi
        jz      @@9
        jmp     @@LookLoop
        ;
@@0:    mov     ebx,edi
        sub     ebx,4+ResHead+ResNum
        sub     ebx,esi
        shr     ebx,2
        add     ebx,esi
        add     ebx,ResHead
        cmp     es:b[ebx],dl            ;Right type?
        jz      @@1
        cmp     dl,Res_SEL              ;Selector?
        jnz     @@2
        cmp     es:b[ebx],Res_PSP
        jnz     @@2
        ;
@@1:    ;At this point we have a match.
        ;
        xor     eax,eax
        sub     edi,4
        mov     dl,es:[ebx]
        xor     dh,dh
        cmp     dl,Res_SEL
        jz      @@rel_1
        cmp     dl,Res_LOCK
        jz      @@Rel_2
        cmp     dl,Res_MEM
        jz      @@Rel_3
        cmp     dl,Res_DOSMEM
        jz      @@Rel_2
        cmp     dl,Res_CALLBACK
        jz      @@Rel_3
        cmp     dl,Res_PSP
        jz      @@psp
        jmp     @@9
;
@@Rel_3:        mov     es:b[ebx],dh
        mov     es:d[edi],eax
        inc     ebx
        add     edi,4
@@Rel_2:        mov     es:b[ebx],dh
        mov     es:d[edi],eax
        inc     ebx
        add     edi,4
@@Rel_1:        mov     es:b[ebx],dh
        mov     es:d[edi],eax
        jmp     @@9
;
;Release a PSP. Assumes that the PSP memory will be released by the caller.
;
@@psp:  mov     es:[ebx],dh
        push    w[PSPSegment]
        mov     bx,es:[edi]
        mov     fs,bx
;
;Remove links to all other modules.
;
        cmp     fs:d[EPSP_Imports],0
        jz      @@no_imports
        pushm   ecx,esi,edi
        mov     esi,fs:d[EPSP_Imports]
        mov     ecx,es:[esi]
        add     esi,4
@@imp0: or      ecx,ecx
        jz      @@imp1
        mov     edi,es:[esi]
        call    UnFindModule
        add     esi,4
        dec     ecx
        jmp     @@imp0
@@imp1: popm    ecx,esi,edi
@@no_imports:

; MED, 02/07/2000, always explicitly release program selectors and memory
;       cmp     fs:[EPSP_Resource],0
;       jnz     @@normal_res
        ;
        ;This must be a cwLoad PSP.
        ;
        ;release program selectors and memory manually.
        ;
        pushad
        movzx   ecx,fs:w[EPSP_SegSize]
        shr     ecx,3
        mov     bx,fs:w[EPSP_SegBase]
        or      ecx,ecx
        jz      @@frelsel9
@@frelsel:      sys     RelSel
        add     bx,8
        dec     ecx
        jnz     @@frelsel
@@frelsel9:     mov     esi,fs:[EPSP_MemBase]
        sys     RelMemLinear32
        mov     esi,fs:[EPSP_Exports]
        or      esi,esi
        jz      @@frelsel0
        sys     RelMemLinear32
@@frelsel0:     popad
        ;
@@normal_res:   ;Take this PSP out of the linked list.
        ;
        pushad
        mov     bx,fs
        sys     GetSelDet32
        mov     esi,es:EPSP_NextPSP[edx]
        mov     edi,es:EPSP_LastPSP[edx]
        or      esi,esi
        jz      @@ChainPSP0
        mov     es:EPSP_LastPSP[esi],edi
@@ChainPSP0:    or      edi,edi
        jz      @@ChainPSP1
        mov     es:EPSP_NextPSP[edi],esi
@@ChainPSP1:    popad
        ;
        mov     ah,50h
        int     21h             ;set new PSP.
        ;
        ;Search for PSP's and release them first.
        ;
        mov     esi,fs:[EPSP_Resource]  ;Get resource pointer.
@@fPSP0:        or      esi,esi
        jz      @@NoPSPSearch
        mov     ebp,ResNum
        mov     edi,esi
        add     edi,16
        mov     edx,esi
        add     edx,ResHead+ResNum
@@fPSP1:        cmp     es:b[edi],Res_PSP
        jnz     @@fPSP2
        push    ebx
        mov     ebx,es:[edx]            ;Get selector.
        sys     RelMem
        pop     ebx
@@fPSP2:        inc     edi
        add     edx,4
        dec     ebp
        jnz     @@fPSP1
        mov     esi,es:[esi+8]  ;link to next list.
        jmp     @@fPSP0
        ;
@@NoPSPSearch:  ;Now release all other types of resource.
        ;
        cmp     fs:w[EPSP_DPMIMem],0    ;Any DPMI save buffer?
        jz      @@psp_0
        xor     bx,bx
        xchg    bx,fs:w[EPSP_DPMIMem]
        sys     RelMem
@@psp_0:        ;
        cmp     fs:d[EPSP_INTMem],0     ;Any int/excep vector memory?
        jz      @@psp_1
        xor     esi,esi
        xchg    esi,fs:d[EPSP_INTMem]
        sys     RelMemLinear32
@@psp_1:        ;
        mov     esi,fs:[EPSP_Resource]  ;Get resource pointer.
        or      esi,esi
        jz      @@psp_9
@@psp_2:        cmp     es:d[esi+8],0   ;Found last entry in chain?
        jz      @@psp_3
        mov     esi,es:d[esi+8]
        jmp     @@psp_2
@@psp_3:        ;
        mov     ebp,ResNum
        mov     edi,esi
        add     edi,16
        mov     edx,esi
        add     edx,ResHead+ResNum
@@psp_4:        cmp     es:b[edi],Res_NULL
        jz      @@psp_5
        cmp     es:b[edi],Res_SEL
        jz      @@sel
        cmp     es:b[edi],Res_MEM
        jz      @@mem
        cmp     es:b[edi],Res_LOCK
        jz      @@lock
        cmp     es:b[edi],Res_DOSMEM
        jz      @@dosmem
        cmp     es:b[edi],Res_CALLBACK
        jz      @@callback
        cmp     es:b[edi],Res_PSP
        jz      @@rel_psp
        jmp     @@psp_5
;
;Release a selector.
;
@@sel:  mov     es:b[edi],Res_NULL
        pushm   esi,edi,ebp,edx
        mov     ebx,es:[edx]
        push    ResourceTracking
        mov     ResourceTracking,0
        call    _RelSelector
        pop     ResourceTracking
        popm    esi,edi,ebp,edx
        jmp     @@psp_5
;
;Release some memory.
;
@@mem:  mov     ForcedFind,edx
        mov     ForcedFind+4,edi
        mov     es:b[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        pushm   esi,edi,ebp,edx
        mov     esi,eax
        call    _RelMemory
        popm    esi,edi,ebp,edx
        sub     ebp,2
        jmp     @@psp_5
;
;Release a lock.
;
@@lock: mov     es:b[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        pushm   esi,edi,ebp,edx
        mov     ecx,es:[edx]
        mov     esi,eax
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     UnLockMem32
        pop     ResourceTracking
        popm    esi,edi,ebp,edx
        dec     ebp
        jmp     @@psp_5
;
;Release DOS memory.
;
@@dosmem:       mov     es:b[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        pushm   eax,edx
        mov     edx,eax
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     RelMemDOS
        pop     ResourceTracking
        popm    eax,edx
        dec     ebp
        jmp     @@psp_5
;
;Release a call-back.
;
@@callback:     mov     es:b[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        inc     edi
        add     edx,4
        mov     es:b[edi],Res_NULL
        pushm   eax,ecx,edx
        mov     ecx,eax
        mov     dx,cx
        shr     ecx,16
        mov     ax,0304h
        push    ResourceTracking
        mov     ResourceTracking,0
        cwAPI_CallOld
        assume ds:_cwMain
        pop     ResourceTracking
        popm    eax,ecx,edx
        sub     ebp,2
        jmp     @@psp_5
;
;Release another PSP.
;
@@rel_psp:      pushm   esi,edi,ebp,edx
        mov     ebx,es:[edx]
        sys     RelMem
        popm    esi,edi,ebp,edx
;
@@psp_5:        inc     edi
        add     edx,4
        dec     ebp
        jnz     @@psp_4
        mov     eax,es:d[esi+4] ;get back link pointer.
        push    eax
        push    esi
        mov     esi,es:[esi+12] ;get memory handle.
        mov     di,si
        shr     esi,16
        mov     ax,0502h                ;release the block.
        cwAPI_CallOld
        pop     esi
        pushm   ebx,ecx,edi
        mov     cx,si
        shr     esi,16
        mov     bx,si
        xor     si,si
        mov     di,4096
        mov     ax,0703h                ;discard the block.
        cwAPI_CallOld
        popm    ebx,ecx,edi
        pop     esi
        or      esi,esi
        jz      @@psp_9
        mov     es:d[esi+8],0   ;make sure link pointer is clear
        jmp     @@psp_3
@@psp_9:        ;
        ;Switch back to the old PSP
        ;
        pop     bx
        mov     ah,50h
        int     21h             ;go back to old PSP.
        ;
@@9:    popad
        popm    ds,es,fs
        ret
        assume ds:_apiCode
ReleaseResource endp


;-------------------------------------------------------------------------
;
;Check if a resource exists.
;
;On Entry:
;
;AL     - Resource type.
;EDX    - Key value.
;
;On Exit:
;
;Carry clear if it does and,
;
;ECX    - Optional second value.
;EBX    - Optional third value.
;
FindResource    proc    near
        pushm   ds,es,fs
        pushm   eax,edx,esi,edi,ebp
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     es,RealSegment
        mov     fs,PSPSegment
        xchg    eax,edx
        xor     edi,edi
        xchg    edi,ForcedFind
        mov     ebp,ForcedFind+4
        or      edi,edi
        jnz     @@Found
        cmp     ResourceTracking,0
        jz      @@8
        cmp     fs:d[EPSP_Resource],0
        jz      @@8
        ;
        mov     esi,fs:d[EPSP_Resource]
@@LookLoop:     mov     ecx,ResNum              ;get number of entries.
        mov     edi,esi
        add     edi,ResHead+ResNum
        cld
@@0:    repne   scasd
        jz      @@1
        mov     esi,es:[esi+8]  ;link to next list.
        or      esi,esi
        jnz     @@LookLoop
        stc
        jmp     @@9
@@1:    mov     ebp,edi
        sub     ebp,4+ResHead+ResNum
        sub     ebp,esi
        shr     ebp,2
        add     ebp,ResHead
        add     ebp,esi
        cmp     es:[ebp],dl
        jnz     @@0
        sub     edi,4
        ;
@@Found:        cmp     dl,Res_SEL
        jz      @@8
        cmp     dl,Res_MEM
        jz      @@2
        cmp     dl,Res_LOCK
        jz      @@8
        cmp     dl,Res_DOSMEM
        jz      @@8
        cmp     dl,Res_CALLBACK
        jz      @@8
        cmp     dl,Res_PSP
        jz      @@8
        stc
        jmp     @@9
        ;
@@2:    assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     LastResource,edi
        mov     LastResource+4,ebp
        add     edi,4
        mov     ecx,es:[edi]
        add     edi,4
        mov     ebx,es:[edi]
        ;
@@8:    clc
        ;
@@9:    popm    eax,edx,esi,edi,ebp
        popm    ds,es,fs
        ret
FindResource    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
SaveExecState   proc    near
        push    es
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
;
;Get memory for the interrupt/exception vector save buffer.
;
        mov     ecx,(256*6)+(256*4)+(32*6)
        sys     GetMemLinear32
        jc      @@9
        mov     es,PSPSegment
        mov     es:d[EPSP_INTMem],esi
        mov     edi,esi
        mov     es,RealSegment
;
;Get protected mode interrupt vectors.
;
        mov     bl,0
        mov     bp,256
@@GetPVect:     pushm   bx,edi,bp,es
        sys     GetVect
        popm    bx,edi,bp,es
        mov     es:d[edi],edx
        mov     es:w[edi+4],cx
        add     edi,6
        inc     bl
        dec     bp
        jnz     @@GetPVect
;
;Get protected mode exception vectors.
;
        mov     bp,32
        mov     bl,0
@@GetEVect:     pushm   bx,edi,bp,es
        sys     GetEVect
        popm    bx,edi,bp,es
        mov     es:d[edi],edx
        mov     es:w[edi+4],cx
        add     edi,6
        inc     bl
        dec     bp
        jnz     @@GetEVect
;
;Get real mode interrupt vectors.
;
        mov     bp,256
        mov     bl,0
@@GetRVect:     pushm   bx,edi,bp,es
        sys     GetRVect
        popm    bx,edi,bp,es
        mov     es:w[edi],dx
        mov     es:w[edi+2],cx
        add     edi,4
        inc     bl
        dec     bp
        jnz     @@GetRVect
;
;Get memory for DPMI state buffer.
;
        cmp     d[DPMIStateSize],0
        jz      @@NoDPMISave
        mov     ecx,d[DPMIStateSize]
        sys     GetMem32
        jc      @@9
;
;Save DPMI state.
;
        mov     es,PSPSegment
        mov     es:w[EPSP_DPMIMem],bx
        mov     es,bx
        xor     edi,edi
        mov     al,0
        test    SystemFlags,1
        jz      @@DPMISave32
        db 66h
        call    f[DPMIStateAddr]
        jmp     @@NoDPMISave
@@DPMISave32:   call    f[DPMIStateAddr]
@@NoDPMISave:   ;
        clc
@@9:    pop     ds
        assume ds:_apiCode
        pop     es
        ret
SaveExecState   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LoadExecState   proc    near
        push    es
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
;
;Set protected mode interrupt vectors.
;
        mov     es,PSPSegment
        mov     edi,es:d[EPSP_INTMem]
        or      edi,edi
        jz      @@NoIntRel
        mov     es,RealSegment
        mov     bl,0
        mov     bp,256
@@SetPVect:     pushm   bx,edi,bp,es
        sys     GetVect
        pushm   ecx,edx
        mov     edx,es:d[edi]
        mov     cx,es:w[edi+4]
        sys     SetVect
        popm    ecx,edx
        cmp     edx,es:d[edi]
        jnz     @@Pdiff
        cmp     cx,es:w[edi+4]
        jnz     @@Pdiff
        mov     es:w[edi+4],-1
        jmp     @@Pdone
@@Pdiff:        mov     es:d[edi],edx
        mov     es:w[edi+4],cx
@@Pdone:        popm    bx,edi,bp,es
        add     edi,6
        inc     bl
        dec     bp
        jnz     @@SetPVect
;
;Set protected mode exception vectors.
;
        mov     bp,32
        mov     bl,0
@@SetEVect:     pushm   bx,edi,bp,es
        sys     GetEVect
        pushm   ecx,edx
        mov     edx,es:d[edi]
        mov     cx,es:w[edi+4]
        sys     SetEVect
        popm    ecx,edx
        cmp     edx,es:[edi]
        jnz     @@Ediff
        cmp     cx,es:[edi+4]
        jnz     @@Ediff
        mov     es:w[edi+4],-1
        jmp     @@Edone
@@Ediff:        mov     es:[edi],edx
        mov     es:[edi+4],cx
@@Edone:        popm    bx,edi,bp,es
        add     edi,6
        inc     bl
        dec     bp
        jnz     @@SetEVect
;
;Set real mode interrupt vectors.
;
        mov     bp,256
        mov     bl,0
@@SetRVect:     pushm   bx,edi,bp,es
        sys     GetRVect
        pushm   cx,dx
        mov     dx,es:w[edi]
        mov     cx,es:w[edi+2]
        sys     SetRVect
        popm    cx,dx
        cmp     dx,es:[edi]
        jnz     @@Rdiff
        cmp     cx,es:[edi+2]
        jnz     @@Rdiff
        mov     es:w[edi+2],-1
        jmp     @@Rdone
@@Rdiff:        mov     es:[edi],dx
        mov     es:[edi+2],cx
@@Rdone:        popm    bx,edi,bp,es
        add     edi,4
        inc     bl
        dec     bp
        jnz     @@SetRVect
;
;Restore DPMI stack state.
;
@@NoIntRel:     mov     es,PSPSegment
        mov     ax,es:w[EPSP_DPMIMem]
        or      ax,ax
        jz      @@NoDPMIRel
        jmp     @@NoDPMIRel
        mov     es,ax
        xor     edi,edi
        mov     al,1
        test    SystemFlags,1
        jz      @@SaveDPMI32
        db 66h
        call    f[DPMIStateAddr]
        jmp     @@NoDPMIRel
@@SaveDPMI32:   call    f[DPMIStateAddr]
@@NoDPMIRel:    ;
        pop     ds
        assume ds:_apiCode
        pop     es
        ret
LoadExecState   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Create a new PSP complete with command line, environment & saved state.
;
;On Entry:
;
;EBX    - flags, as passed to _Exec
;DS:EDX - program name
;ES:ESI - command line.
;CX     - environment.
;
;On Exit:
;
;Carry set on error &,
;
;BX     - new PSP (zero if error).
;
CreatePSP       proc    near
        mov     ax,ds
        pushm   ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[@@Name],edx
        mov     w[@@Name+4],ax
        mov     d[@@Flags],ebx
        mov     d[@@Command],esi
        mov     w[@@Command+4],es
        mov     w[@@Environment],cx
        ;
        mov     w[@@PSP],0
;
;Allocate PSP memory.
;
        mov     ecx,(size PSP_Struc)+(size EPSP_Struc)
        sys     GetMemLinear32
        jc      @@error         ;Not enough memory.
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     GetSel
        jnc     @@memOK
        pop     ResourceTracking
        pop     ds
        sys     RelMemLinear32
        jmp     @@error
@@memOK:        mov     edx,esi
        mov     ecx,(size PSP_Struc)+(size EPSP_Struc)
        sys     SetSelDet32
        pop     ResourceTracking
        assume ds:_apiCode
        pop     ds
;
;Register this selector as a PSP.
;
        mov     ax,Res_PSP
        movzx   edx,bx
        call    RegisterResource        ;register this PSP.
        jnc     @@memOK2
        ;
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     RelSel
        sys     RelMemLinear32
        pop     ResourceTracking
        assume ds:_apiCode
        pop     ds
        jmp     @@error
        ;
@@memOK2:       mov     w[@@PSP],bx
;
;Copy parent PSP to this PSP.
;
        push    ds
        mov     es,bx
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        assume ds:_apiCode
        xor     esi,esi
        xor     edi,edi
        mov     ecx,256/4
        cld
        rep     movsd
        mov     ax,ds:[EPSP_RealENV]    ;copy real mode environment
        mov     es:[EPSP_RealENV],ax    ;segment.
        mov     eax,ds:[EPSP_ExecCount]
        mov     es:[EPSP_ExecCount],eax
        pop     ds
;
;Initialise PSP fields.
;
        push    fs
        mov     fs,apiDSeg
        assume fs:_cwMain
        mov     fs,fs:PSPSegment
        assume fs:nothing
        mov     es,bx
        mov     es:d[EPSP_Resource],0   ;Clear memory fields.
        mov     es:d[EPSP_INTMem],0
        mov     es:d[EPSP_DPMIMem],0
        mov     es:w[EPSP_Parent],fs    ;set parent PSP.
        mov     es:d[EPSP_DTA],80h      ;Use default PSP DTA.
        mov     es:w[EPSP_DTA+4],es
        mov     ax,fs:w[EPSP_TransProt] ;inherit current transfer buffer.
        mov     es:w[EPSP_TransProt],ax
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:w[EPSP_TransReal],ax
        mov     eax,fs:d[EPSP_TransSize]
        mov     es:d[EPSP_TransSize],eax
        mov     eax,fs:d[EPSP_mcbMaxAlloc]
        mov     es:d[EPSP_mcbMaxAlloc],eax
        mov     es:d[EPSP_mcbHead],0
        cmp     d[@@Flags],2            ;cwLoad?
        jz      @@NoNext
        mov     fs:w[EPSP_Next],es
@@NoNext:       mov     es:w[EPSP_Next],0
        mov     es:w[PSP_Environment],0
        mov     es:w[EPSP_SegBase],0
        mov     es:w[EPSP_SegSize],0
        mov     es:d[EPSP_Exports],0
        mov     es:d[EPSP_Imports],0
        mov     es:d[EPSP_Links],0
        mov     es:w[EPSP_EntryCSEIP+4],0
        mov     es:w[EPSP_PSPSel],es
        pop     fs
;
;Update PSP linked list.
;
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     esi,es:BasePSPAddress
        mov     es,es:RealSegment
        assume es:nothing
@@ChainPSP0:    cmp     es:EPSP_NextPSP[esi],0
        jz      @@ChainPSP1
        mov     esi,es:EPSP_NextPSP[esi]
        jmp     @@ChainPSP0
@@ChainPSP1:    sys     GetSelDet32
        mov     es:EPSP_NextPSP[esi],edx
        mov     es:EPSP_LastPSP[edx],esi
        mov     es:EPSP_NextPSP[edx],0
        pop     es
;
;Switch to this PSP.
;
        cmp     d[@@Flags],2            ;cwLoad?
        jz      @@NoPSwitch0
        mov     ah,50h
        int     21h             ;set new PSP.
;
;Set new DTA address.
;
@@NoPSwitch0:   cmp     d[@@Flags],2
        jz      @@NoDTA
        push    ds
        lds     edx,es:f[EPSP_DTA]
        mov     ah,1ah
        int     21h
        pop     ds
;
;Preserve current state.
;
@@NoDTA:        cmp     d[@@Flags],2
        jz      @@NoSave
        call    SaveExecState   ;do old state save.
        jc      @@error         ;Not enough memory.
;
;Build command line.
;
@@NoSave:       cmp     w[@@Command+4],0
        jz      @@NoCommand
        mov     es,w[@@PSP]
        mov     edi,80h
        mov     es:d[edi],0
        push    ds
        lds     esi,f[@@Command]
        movzx   ecx,b[esi]
        inc     ecx
        cld
        rep     movsb
        mov     es:b[edi],13
        pop     ds
;
;Check what's needed with the environment selector.
;
@@NoCommand:    cmp     d[@@Flags],2            ;cwLoad?
        jz      @@CopyEnv       ;NoEnv
        mov     ax,w[@@Environment]
        or      ax,ax
        jz      @@ParentEnv
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es:w[PSP_Environment],ax
        jmp     @@GotEnv

@@CopyEnv:      push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     ax,es:PSPSegment
        assume es:nothing
        mov     es,w[@@PSP]
        mov     es:w[PSP_Environment],ax
        pop     es
        jmp     @@NoEnv

;
;Need to make a copy of the parent environment string.
;
@@ParentEnv:    mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es,es:w[EPSP_Parent]    ;Get parent PSP.
        mov     es,es:w[PSP_Environment]        ;Get parents environment.
;
;Find out how long current environment is.
;
        xor     esi,esi
@@gp2:  mov     al,es:[esi]             ;Get a byte.
        inc     esi             ;/
        or      al,al           ;End of a string?
        jnz     @@gp2           ;keep looking.
        mov     al,es:[esi]             ;Double zero?
        or      al,al           ;/
        jnz     @@gp2           ;keep looking.
        add     esi,3           ;Skip last 0 and word count.
        mov     ecx,esi
        add     ecx,256         ;allow for exec name length.
        sys     GetMem32
        jc      @@error
        mov     ax,es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es:w[PSP_Environment],bx
        mov     ecx,esi         ;get length again.
        push    ds
        mov     ds,ax
        mov     es,bx
        xor     esi,esi
        xor     edi,edi
        rep     movsb           ;copy current strings.
        pop     ds
;
;Add execution path and name to environment strings.
;
@@GotEnv:       mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es,es:w[PSP_Environment]
        xor     esi,esi
@@gp0:  mov     al,es:[esi]             ;Get a byte.
        inc     esi             ;/
        or      al,al           ;End of a string?
        jnz     @@gp0           ;keep looking.
        mov     al,es:[esi]             ;Double zero?
        or      al,al           ;/
        jnz     @@gp0           ;keep looking.
        add     esi,3           ;Skip last 0 and word count.
        mov     ebp,esi
;
;Now build the file name.
;
        push    ds
        lds     esi,f[@@Name]
        mov     al,b[esi+1]
        pop     ds
        mov     edi,ebp
        cmp     al,":"          ;drive specification?
        jz      @@NoGetPath
        push    edi
        mov     ah,19h  ;get current disc
        int     21h
        mov     dl,al
        add     al,'A'  ;make it a character
        pop     esi
        mov     es:[esi],al
        inc     esi
        mov     es:b[esi],":"
        inc     esi
        ;
        push    ds
        push    esi
        lds     esi,f[@@Name]
        mov     al,b[esi]
        pop     esi
        pop     ds
        mov     edi,esi
        cmp     al,"\"
        jz      @@NoGetPath
        ;
        mov     es:b[esi],"\"
        inc     esi
        mov     es:b[esi],0
        mov     ah,47h  ;get current directory
        xor     dl,dl   ;default drive
        push    ds
        push    es
        pop     ds
        int     21h     ;get text
        pop     ds
        mov     esi,ebp
        push    ds
        push    es
        pop     ds
@@gp1:  lodsb
        or      al,al
        jnz     @@gp1
        pop     ds
        dec     esi
        mov     edi,esi
        mov     al,"\"
        stosb
;
;Append EXE file name to EDI.
;
@@NoGetPath:    push    ds
        lds     esi,f[@@Name]
@@gp3:  lodsb
        stosb
        or      al,al
        jnz     @@gp3
        pop     ds
;
;Return to caller.
;
@@NoEnv:        clc
        jmp     @@exit
;
;Not enough memory.
;
@@error:        stc
;
@@exit: mov     bx,w[@@PSP]
        popm    ds,es,fs,gs
        ret
;
@@PSP:  ;
        dw 0
@@Command:      ;
        df 0
@@Name: ;
        df 0
@@Environment:  ;
        dw 0
@@Flags:        ;
        dd 0
CreatePSP       endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
DeletePSP       proc    near
        pushm   ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[@@Flags],eax
        mov     w[@@PSP],bx
;
        cmp     d[@@Flags],2
        jz      @@NoIRel
        call    LoadExecState
;
;Close all open files.
;
        call    LoseFileHandles
;
;Restore resources.
;
@@NoIRel:       mov     es,apiDSeg
        assume es:_cwMain
        cmp     es:w[TerminationHandler],offset cwClose
        jnz     @@YesRelRes
        cmp     es:DebugDump,0
        jnz     @@NoNRel
@@YesRelRes:    assume es:nothing
        mov     es,w[@@PSP]
        mov     bx,es:w[EPSP_Parent]
        cmp     d[@@Flags],2
        jz      @@NoPRel
        mov     ah,50h
        int     21h             ;restore old PSP.
@@NoPRel:       mov     bx,es
        sys     RelMem
        cmp     d[@@Flags],2
        jz      @@NoNRel
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es:w[EPSP_Next],0
@@NoNRel:       ;
        popm    ds,es,fs,gs
        ret
;
@@PSP:  ;
        dw ?
@@Flags:        ;
        dd ?
DeletePSP       endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Find a module with the right main EXPORT name. If it's not already resident
;search for it on disk and load it if found.
;
;On Entry:
;
;ES:ESI - Module name linear address (ES must address 0-4G)
;
;On Exit:
;
;Carry set on error & EAX is error code else,
;
;EDI    - Linear address of modules PSP.
;
;Error codes match cwLoad values.
;
;All other registers preserved.
;
FindModule      proc    near
        pushm   ebx,ecx,edx,esi,ebp,ds,es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode

;
;Search resident modules first.
;
        push    fs
        mov     fs,apiDSeg
        assume fs:_cwMain
        mov     edi,fs:BasePSPAddress   ;Point to start of PSP chain.
        assume fs:nothing
        pop     fs
        ;
        ;Skip partial path if there is one.
        ;
        push    esi
        movzx   ecx,es:b[esi]
        inc     esi
        mov     ebx,esi
        mov     ebp,ecx
@@5:    inc     esi
        cmp     es:b[esi-1],"\"
        jnz     @@6
        mov     ebx,esi
        mov     ebp,ecx
@@6:    dec     ecx
        jnz     @@5
        mov     esi,ebx
        ;
@@imp1: mov     ebx,esi
        mov     edx,es:EPSP_Exports[edi]        ;Point to export memory.
        or      edx,edx         ;Any exports?
        jz      @@imp3
        mov     edx,es:[edx+4]  ;Point to module name.
        mov     ecx,ebp
        cmp     cl,es:[edx]             ;Right name length?
        jnz     @@imp3
        inc     edx
@@imp2: mov     al,es:[ebx]
        cmp     al,es:[edx]             ;right char?
        jnz     @@imp3
        inc     ebx
        inc     edx
        dec     ecx
        jnz     @@imp2
        ;
        pop     esi
        jmp     @@imp5          ;got it!
        ;
@@imp3: mov     edi,es:EPSP_NextPSP[edi]
        or      edi,edi         ;check there is something else to look at.
        jnz     @@imp1
        pop     esi
;
;Shit, not a resident module so look for it on disk.
;

;
;Try current PSP's execution path.
;
        push    esi
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     bx,es:w[PSP_Environment]
        pop     es
        pushm   edx,ecx
        sys     GetSelDet32
        mov     esi,edx
        popm    edx,ecx
@@imp3_0:
        mov     al,es:[esi]             ;Get a byte.
        inc     esi             ;/
        or      al,al           ;End of a string?
        jnz     @@imp3_0                ;keep looking.
        mov     al,es:[esi]             ;Double zero?
        or      al,al           ;/
        jnz     @@imp3_0                ;keep looking.
        add     esi,3           ;Skip last 0 and word count.
        ;
        ;Copy up to last "\"
        ;
        mov     edi,offset DLLNameSpace
        mov     ebp,edi
@@imp3_1:       mov     al,es:[esi]
        inc     esi
        mov     [edi],al
        inc     edi
        cmp     al,"\"
        jnz     @@imp3_2
        mov     ebp,edi
@@imp3_2:       or      al,al
        jnz     @@imp3_1
        mov     edi,ebp
        pop     esi

        cmp     BYTE PTR es:[esi+2],':' ; see if module name has absolute path
        jne     chkmodname
        cmp     BYTE PTR es:[esi],4
        jb      chkmodname              ; name not long enough for absolute path
        mov     edi,offset DLLNameSpace ; reset DLL path to receive module name path only

        ;
        ;Check if module name has any path in it.
        ;
chkmodname:
        push    esi
        movzx   ecx,es:b[esi]
        inc     esi
        mov     ebp,esi
@@0:
        inc     esi
        cmp     es:b[esi-1],"\"
        jnz     @@1
        mov     ebp,esi
@@1:
        dec     ecx
        jnz     @@0
        mov     ecx,esi
        sub     ecx,ebp
        pop     esi
        push    esi
        inc     esi
@@2:
        cmp     esi,ebp
        jnc     @@3
        mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        jmp     @@2
@@3:
        mov     b[edi],0
        ;
        ;Copy module name into work space.
        ;
        mov     edi,offset MODNameSpace
        mov     [edi],cl
        inc     edi
@@4:
        mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        dec     ecx
        jnz     @@4
        pop     esi
        ;
        ;Try and find the module.
        ;
        push    es
        push    ds
        pop     es
        mov     esi,offset MODNameSpace
        mov     edi,offset DLLNameSpace
        call    SearchModule            ;look for module with right name.
        pop     es
        jnc     @@8
;
;Couldn't find the module so return an error.
;
        mov     eax,1
        jmp     @@error
;
;Try loading the module found.
;
@@8:
        push    ExecMCount
        mov     ExecMCount,ecx
;
;Set master PSP.
;
        assume ds:nothing
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    PSPSegment
        mov     ah,51h
        int     21h
        push    bx
        mov     bx,BasePSP
        mov     ah,50h
        int     21h
;
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     edx,offset DLLNameSpace
        sys     cwLoad          ;call ourselves.
;
;Go back to origional PSP
;
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        pop     bx
        mov     ah,50h
        int     21h
        pop     PSPSegment
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pop     ExecMCount
        jc      @@error
;
        mov     bx,si           ;get the PSP
        pushm   ecx,edx
        sys     GetSelDet32             ;need a 32-bit address for it.
        mov     edi,edx
        popm    ecx,edx
;
;Call DLL's initialisation code.
;
        mov     es:w[edi+EPSP_EntryCSEIP+4],cx
        mov     es:d[edi+EPSP_EntryCSEIP],edx
        or      cx,cx
        jz      @@imp5
        xor     eax,eax
        mov     ax,es:w[edi+EPSP_EntryCSEIP+4]
        lar     eax,eax
        test    eax,00400000h
        jnz     @@imp6_0
        mov     es:w[edi+EPSP_EntryCSEIP+2],cx
@@imp6_0:       pushm   ds,es,fs,gs
        pushad
        push    es
        pop     fs
        mov     ds,si
        mov     es,si
        test    eax,00400000h
        mov     eax,0
        jnz     @@imp6
        db 66h
@@imp6: call    fs:f[edi+EPSP_EntryCSEIP]
        or      ax,ax
        popad
        popm    ds,es,fs,gs
        mov     eax,1
        jnz     @@error
;
;Update modules referance count.
;
@@imp5: inc     es:EPSP_Links[edi]
;
;Return module PSP address to caller.
;
        xor     eax,eax
        clc
        jmp     @@imp10
;
;Couldn't find name so return error to caller.
;
@@error:        stc
;
@@imp10:        popm    ebx,ecx,edx,esi,ebp,ds,es
        ret
;
DLLNameSpace    db 256 dup (0)
MODNameSpace    db 256 dup (0)
FindModule      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Reverse the FindModule process.
;
;On Entry:
;
;ES:EDI - module PSP (ES must address 0-4G)
;
;This basicly DEC's the modules referance count and if it becomes zero it
;removes the module from memory.
;
UnFindModule    proc    near
        pushm   ds,es,fs,gs
        pushad
;
        dec     es:EPSP_Links[edi]
        jnz     @@8
;
;Get this PSP's selector.
;
        mov     fs,es:EPSP_PSPSel[edi]
;
;Call DLL's exit code.
;
        cmp     fs:w[EPSP_EntryCSEIP+4],0
        jz      @@imp5
        xor     eax,eax
        mov     ax,fs:w[EPSP_EntryCSEIP+4]
        lar     eax,eax
        pushm   ds,es,fs,gs
        pushad
        mov     ax,fs
        mov     ds,ax
        mov     es,ax
        test    eax,00400000h
        mov     eax,1
        jnz     @@imp6
        db 66h
@@imp6: call    fs:f[EPSP_EntryCSEIP]
        or      ax,ax
        popad
        popm    ds,es,fs,gs
;
;Switch to the master PSP.
;
@@imp5: assume ds:nothing
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    PSPSegment
        mov     bx,BasePSP
        mov     ah,50h
        int     21h
;
;Release module.
;
        mov     bx,fs
        sys     RelSel
;
;Go back to origional PSP
;
        pop     PSPSegment
        assume ds:_apiCode
;
@@8:    clc
        popad
        popm    ds,es,fs,gs
        ret
UnFindModule    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Search specified path for a file with the right module name.
;
;On Entry:
;
;DS:EDI - path string.
;ES:ESI - module name.
;
;On Exit:
;
;Carry set on error else,
;
;Path string specified in DS:EDI has file name appended to it.
;
;ECX    - module number within file.
;
SearchModule    proc    near
        mov     ax,ds
        pushm   ds,es
        pushad
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     d[@@Path],edi
        mov     w[@@Path+4],ax
        mov     d[@@Name],esi
        mov     w[@@Name+4],es

; MED 01/05/96
        push    eax
        push    es
        mov     ah,2fh  ; get DTA in es:ebx
        int     21h
        mov     DWORD PTR EntryDTAAddress,ebx
        mov     WORD PTR EntryDTAAddress+4,es
        pop     es

        mov     edx,OFFSET TemporaryDTA
        mov     ah,1ah  ; set DTA in ds:edx
        int     21h
        pop     eax

;
;Get path length so we don't have to keep scanning the string.
;
        push    ds
        mov     ds,ax
        xor     ecx,ecx
@@0:    cmp     b[edi],0
        jz      @@1
        inc     edi
        inc     ecx
        jmp     @@0
@@1:    pop     ds
        mov     d[@@Length],ecx
;
;Setup initial mask pointer.
;
        mov     d[@@Mask],offset @@Masks
;
;Get DTA address.
;
        push    es
        mov     ah,2fh
        int     21h
        mov     d[@@Dta],ebx
        mov     w[@@Dta+4],es
        pop     es
;
;Work through all mask types.
;
@@2:    mov     esi,d[@@Mask]
        cmp     b[esi],0                ;end of the list?
        jz      @@9
;
;Add new mask to path string.
;
        add     d[@@Mask],4             ;move to next mask.
        les     edi,f[@@Path]
        add     edi,d[@@Length] ;point to end of path string.
        mov     es:b[edi],"*"
        inc     edi
        movsd                   ;copy extension.
        mov     es:b[edi],0
;
;Work through all files with the right extension.
;
        push    ds
        lds     edx,f[@@Path]
        xor     cx,cx
        mov     ah,4eh          ;find first file
        int     21h
        pop     ds
        jc      @@2
        jmp     @@4
;
@@3:    mov     ah,4fh
        int     21h
        jc      @@2
;
;Add this name to the path string?
;
@@4:    les     edi,f[@@DTA]
        test    es:b[edi+21],16 ;DIR?
        jnz     @@3
        push    ds
        mov     eax,d[@@Length] ;point to end of path string.
        lds     esi,f[@@Path]
        add     esi,eax
        lea     edi,[edi+1eh]   ;point to file name.
        mov     ecx,13
        cld
@@5:    mov     al,es:[edi]
        mov     [esi],al
        inc     esi
        inc     edi
        or      al,al
        jz      @@6
        dec     ecx
        jnz     @@5
@@6:    mov     b[esi],0                ;terminate the name.
        pop     ds
;
;Find out what the files "module" name is.
;
        ;
        ;Open the file.
        ;
        push    ds
        lds     edx,f[@@Path]
        mov     ax,3d00h
        int     21h
        pop     ds
        jc      @@7
        mov     w[@@Handle],ax
        mov     bx,ax
        mov     d[@@Count],0
        ;
@@11:   ;See what sort of file it is.
        ;
        mov     edx,offset @@ID
        mov     ecx,2
        mov     ah,3fh
        int     21h
        jc      @@7
        cmp     ax,cx
        jnz     @@7
        cmp     w[@@ID],"ZM"
        jz      @@MZ
        cmp     w[@@ID],"P3"
        jz      @@3P
        jmp     @@7
;
;Process an MZ section.
;
@@MZ:   mov     edx,offset @@ID+2
        mov     ecx,1bh-2
        mov     ah,3fh
        int     21h
        jc      @@7
        cmp     ax,cx
        jnz     @@7
        cmp     w[@@ID+18h],40h
        jz      @@LE
        ;
        ;Find out how long the MZ bit is.
        ;
        mov     ax,w[@@ID+2+2]  ;get length in 512 byte blocks

; MED 04/26/96
        cmp     WORD PTR [@@ID+2],0
        je      medexe4 ; not rounded if no modulo

        dec     ax              ;lose 1 cos its rounded up

medexe4:
        add     ax,ax           ;mult by 2
        mov     dh,0
        mov     dl,ah
        mov     ah,al
        mov     al,dh           ;mult by 256=*512
        add     ax,w[@@ID+2]            ;add length mod 512
        adc     dx,0            ;add any carry to dx
        mov     cx,ax
        sub     cx,1bh          ;account for the header.
        sbb     dx,0
        xchg    cx,dx           ;swap round for DOS.
        mov     ax,4201h                ;set new file offset.
        int     21h
        jmp     @@11
;
;Process what should be an LE section.
;
@@LE:   mov     ecx,3ch
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h             ;move to LE offset.
        mov     edx,offset @@ID
        mov     ecx,4
        mov     ah,3fh
        int     21h             ;read LE offset.
        jc      @@7
        cmp     ax,4
        jnz     @@7
        cmp     d[@@ID],0
        jz      @@7
        mov     ecx,d[@@ID]
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h             ;point to LE section.
        mov     edx,offset @@ID+4
        mov     ah,3fh
        mov     ecx,2
        int     21h             ;read ID string.
        jc      @@7
        cmp     ax,2
        jnz     @@7

        cmp     w[@@ID+4],"EL"
        jz      medle1

IFDEF LXWORK
        mov     eax,DWORD PTR [@@ID+4]
        mov     cs:[0],al
        cmp     w[@@ID+4],"XL"  ; MED
        jnz     @@7
        mov     cs:[0],bl
ENDIF
        jmp     @@7

        ;
        ;Process an LE section.
        ;
medle1:
        mov     ecx,LE_ResidentNames-2
        mov     dx,cx
        shr     ecx,16
        mov     ax,4201h
        int     21h             ;move to module name offset.
        mov     edx,offset @@ID+4
        mov     ecx,4
        mov     ah,3fh
        int     21h             ;read module name offset.
        jc      @@7
        cmp     ax,4
        jnz     @@7
        mov     ecx,d[@@ID]
        add     ecx,d[@@ID+4]
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h             ;move to module name.
        mov     edx,offset @@ID
        mov     ecx,256
        mov     ah,3fh
        int     21h             ;read the module name.
        ;
        ;See if this module has right name.
        ;
        push    es
        les     edx,f[@@Name]   ;point to name we're looking for.
        mov     edi,offset @@ID
        movzx   ecx,b[edi]
        cmp     cl,es:[edx]             ;right length?
        jnz     @@le7
@@le5:  inc     edx
        inc     edi
        mov     al,es:[edx]
        cmp     al,[edi]
        jnz     @@le7
        dec     ecx
        jnz     @@le5
        pop     es
        ;
        ;Close the file.
        ;
        xor     bx,bx
        xchg    bx,w[@@Handle]
        mov     ah,3eh
        int     21h
        jmp     @@8
        ;
@@le7:  pop     es
        jmp     @@7
;
;Process a 3P section.
;
@@3P:   mov     edx,offset @@ID+2
        mov     ecx,size NewHeaderStruc-2
        mov     ah,3fh
        int     21h
        jc      @@7
        cmp     ax,cx
        jnz     @@7
        sub     d[NewSize+@@ID],size NewHeaderStruc
        ;
        ;Check this file has exports.
        ;
        cmp     d[NewExports+@@ID],0
        jz      @@3p6
        ;
        ;Skip segment definitions.
        ;
        movzx   edx,w[NewSegments+@@ID]
        shl     edx,3
        sys     cwcInfo
        jc      @@3p0
        mov     edx,eax
@@3p0:  sub     d[NewSize+@@ID],edx
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        ;
        ;Skip relocations.
        ;
        mov     edx,d[NewRelocs+@@ID]
        shl     edx,2
        or      edx,edx
        jz      @@3p1
        sys     cwcInfo
        jc      @@3p1
        mov     edx,eax
@@3p1:  sub     d[NewSize+@@ID],edx
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        ;
        ;Load export details.
        ;
        mov     ecx,d[NewExports+@@ID]
        sys     GetMemLinear32
        jc      @@7             ;treat memory error as no file.
        mov     edx,ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        assume ds:_apiCode
        sys     cwcInfo
        jc      @@3p2

        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        sub     d[NewSize+@@ID],ecx
        pop     ds

        push    es
        mov     ax,ds
        mov     es,ax
        mov     edi,esi
        sys     cwcLoad
        pop     es
        jc      @@3p3
        jmp     @@3p4

@@3p2:  push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        sub     d[NewSize+@@ID],edx
        pop     ds

        mov     ecx,edx
        mov     edx,esi
        call    ReadFile
        jc      @@3p3
        cmp     eax,ecx
        jz      @@3p4
@@3p3:  pop     ds
        sys     RelMemLinear32
        jmp     @@7
@@3p4:  mov     ax,ds
        mov     es,ax
        pop     ds
        ;
        ;See if this module has right name.
        ;
        push    ds
        lds     edx,f[@@Name]   ;point to name we're looking for.
        mov     edi,esi
        add     edi,es:[edi+4]  ;get offset of module name.
        movzx   ecx,es:b[edi]
        cmp     cl,[edx]                ;right length?
        jnz     @@3p7
@@3p5:  inc     edx
        inc     edi
        mov     al,[edx]
        cmp     al,es:[edi]
        jnz     @@3p7
        dec     ecx
        jnz     @@3p5
        pop     ds
        ;
        ;Release EXPORT record memory.
        ;
        sys     RelMemLinear32
        ;
        ;Close the file.
        ;
        xor     bx,bx
        xchg    bx,w[@@Handle]
        mov     ah,3eh
        int     21h
        jmp     @@8
;
;Not this one, move to next part of the file.
;
@@3p7:  pop     ds
        sys     RelMemLinear32
@@3p6:  mov     edx,d[NewSize+@@ID]
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        inc     d[@@Count]
        jmp     @@11
;
;Make sure file is closed.
;
@@7:    xor     bx,bx
        xchg    bx,w[@@Handle]
        or      bx,bx
        jz      @@3
        mov     ah,3eh
        int     21h
        jmp     @@3
;
@@8:    clc
        jmp     @@10
;
@@9:    stc
;
@@10:

; MED 01/05/96
        pushf
        push    ds
        lds     edx,EntryDTAAddress
        mov     ah,1ah  ; set DTA in ds:edx
        int     21h
        pop     ds
        popf

        popad
        mov     ecx,d[@@Count]
        popm    ds,es
        ret
;
@@ID:   ;
        db 256 dup (0)
@@Handle:       ;
        dw 0
@@DTA:  ;
        df 0
@@Path: ;
        df 0
@@Name: ;
        df 0
@@Length:       ;
        dd 0
@@Count:        ;
        dd 0
@@Mask: ;
        dd 0
@@Masks:        ;
        db ".DLL"
        db ".EXE"
        db 0
; temporary DTA storage so PSP isn't munged, MED 01/03/96
TemporaryDTA    DB      80h DUP (?)
EntryDTAAddress DF      0       ; Entry DTA address
SearchModule    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Search a modules export table for name specified.
;
;On Entry:
;
;ES:EDI - Export table.
;ES:EBP - function name.
;
;On Exit:
;
;Carry set on error else,
;
;ES:EDI - matched export entry.
;
;All other registers preserved.
;
FindFunction    proc    near
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugamtext1
debugamloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugamb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugamloop2
debugamb:
        mov     edx,OFFSET debugamtext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugamout

debugamtext1    DB      'Entering FindFunction...',0
debugamtext2    DB      13,10

debugamout:
ENDIF

        pushm   eax,ebx,ecx,edx,esi,ebp
        ;
        mov     edx,es:[edi]            ;get number of exports.
        add     edi,4+4         ;skip entry count and module name.
        ;
@@imp11:        push    edi
        mov     edi,es:[edi]            ;point to name string.
        mov     ebx,ebp
        xor     ecx,ecx
        mov     cl,es:[edi+6]
        cmp     cl,es:[ebx]
        jnz     @@imp13
        add     edi,6+1

COMMENT !
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugaktext1
        push    cx
debugakloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugakb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugakloop2
debugakb:
        pop     cx
        push    es
        pop     ds
debugakloop1:
        mov     edx,edi
        mov     bx,1
        mov     ah,40h
        int     21h
debugaka:
        mov     edx,OFFSET debugaktext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugakout

debugaktext1    DB      'Checking against function: ',0
debugaktext2    DB      13,10

debugakout:
ENDIF
END COMMENT !

        inc     ebx
        ;
@@imp12:        mov     al,es:[edi]
        cmp     al,es:[ebx]
        jnz     @@imp13
        inc     edi
        inc     ebx
        dec     ecx
        jnz     @@imp12
        jmp     @@imp14
        ;
@@imp13:        pop     edi
        add     edi,4
        dec     edx
        jnz     @@imp11
        jmp     @@error
        ;
@@imp14:        pop     edi
        mov     edi,es:[edi]
        clc
        jmp     @@10
        ;
@@error:        stc
@@10:   popm    eax,ebx,ecx,edx,esi,ebp
IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugaltext1
debugalloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugalb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugalloop2
debugalb:
        mov     edx,OFFSET debugaltext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugalout

debugaltext1    DB      'Done with FindFunction...',0
debugaltext2    DB      13,10

debugalout:
ENDIF

        ret
FindFunction    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Load, relocate and execute the application code. All format loader.
;
;On Entry:
;
;EBX    - Mode.
;       0 - Normal EXEC.
;       1 - Load for debug.
;       2 - Load for overlay.
;DS:EDX - File name.
;ES:ESI - Command line.
;CX     - Environment selector, 0 to use existing copy.
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;If Mode=0
;
;AL = ErrorLevel (returned by child's terminate)
;
;If Mode=1
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;DI     - Auto DS.
;EBP    - Segment definition memory.
;
;If Mode=2
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;
;Error codes:
;
;1      - DOS file access error.
;2      - Not a recognisable format.
;3      - Not enough memory.
;
_Exec   proc    near
        mov     ax,ds
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        call    DPMICopyCheck
;
;Preserve details.
;
        mov     d[@@Name],edx
        mov     w[@@Name+4],ax
        mov     d[@@Flags],ebx
        mov     d[@@Command],esi
        mov     w[@@Command+4],es
        mov     w[@@Environment],cx
        mov     w[@@Handle],0
;
;Try and open the file.
;
        push    ds
        lds     edx,f[@@Name]
        mov     ax,3d00h                ;open, read only.
        int     21h
        pop     ds
        jc      @@no_file_error
        mov     w[@@Handle],ax  ;store the handle.
;
;Find out what format the current section is.
;
@@0:    mov     bx,w[@@Handle]
        mov     edx,offset @@Temp
        mov     ecx,2
        mov     ah,3fh
        int     21h
        jc      @@file_error
        cmp     ax,cx
        jnz     @@file_error
        ;
        cmp     w[@@Temp],"ZM"  ;MZ EXE?
        jz      @@MZ
        cmp     w[@@Temp],"EL"  ;LE EXE?
        jz      @@LE
        cmp     w[@@Temp],"P3"  ;3P EXE?
        jz      @@3P

IFDEF LXWORK
        cmp     w[@@Temp],"XL"  ;LX EXE? -- MED
        jz      @@LE
        mov     cs:[0],dl
ENDIF

        jmp     @@file_error
        ;
@@MZ:   ;Look for an LE offset.
        ;
        mov     bx,w[@@Handle]
        mov     dx,18h
        xor     cx,cx
        mov     ax,4200h
        int     21h
        mov     edx,offset @@Temp
        mov     ecx,2
        mov     ah,3fh
        int     21h             ;Fetch LE offset.
        jc      @@file_error
        cmp     ax,cx
        jnz     @@file_error
        cmp     w[@@Temp],40h   ;LE offset present?
        jnz     @@MZ2
        ;
        ;Fetch the NE/LE/LX offset.
        ;
        mov     bx,w[@@Handle]
        mov     dx,3ch
        xor     cx,cx
        mov     ax,4200h
        int     21h
        mov     edx,offset @@Temp
        mov     ecx,4
        mov     ah,3fh
        int     21h             ;Fetch LE offset.
        jc      @@file_error
        cmp     ax,cx
        jnz     @@file_error
        cmp     d[@@Temp],0             ;any offset?
        jz      @@MZ2
        mov     eax,d[@@Temp]
        mov     dx,ax
        shr     eax,16
        mov     cx,ax
        mov     bx,w[@@Handle]
        mov     ax,4200h
        int     21h
        jmp     @@0
        ;
@@MZ2:  ;Get MZ length and skip it.
        ;
        mov     dx,2
        xor     cx,cx
        mov     ax,4200h
        mov     bx,w[@@Handle]
        int     21h
        mov     edx,offset @@Temp
        mov     ecx,4
        mov     ah,3fh
        mov     bx,w[@@Handle]
        int     21h
        jc      @@file_error
        cmp     ax,cx
        jnz     @@file_error
        mov     ax,w[@@Temp+2]  ;get length in 512 byte blocks

; MED 04/26/96
        cmp     WORD PTR [@@Temp],0
        je      medexe5 ; not rounded if no modulo

        dec     ax              ;lose 1 cos its rounded up

medexe5:
        add     ax,ax           ;mult by 2
        mov     dh,0
        mov     dl,ah
        mov     ah,al
        mov     al,dh           ;mult by 256=*512
        add     ax,w[@@Temp]            ;add length mod 512
        adc     dx,0            ;add any carry to dx
        mov     cx,ax
        xchg    cx,dx           ;swap round for DOS.
        mov     ax,4200h                ;set absolute position.
        mov     bx,w[@@Handle]
        int     21h
        jmp     @@0
;
;Run an LE format program.
;
@@LE:   mov     d[@@ExecAdd],offset LoadLE
        jmp     @@exec
;
;Run a 3P format program.
;
@@3P:   mov     d[@@ExecAdd],offset Load3P
;
;Close the file.
;
@@exec: mov     bx,w[@@Handle]
        mov     w[@@Handle],0
        mov     ah,3eh
        int     21h
;
;Fetch registers & call format specific loader.
;
        mov     edx,d[@@Name]
        mov     ax,w[@@Name+4]
        mov     ebx,d[@@Flags]
        mov     esi,d[@@Command]
        mov     es,w[@@Command+4]
        mov     cx,w[@@Environment]
        mov     ds,ax
        assume ds:nothing
        call    cs:d[@@ExecAdd]

        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
;
@@exit: pushf
        push    eax
        cmp     w[@@Handle],0
        jz      @@e0
        mov     bx,w[@@Handle]
        mov     ah,3eh
        int     21h
@@e0:   pop     eax
        popf
;
        pop     ds

IFDEF DEBUG2
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debugajtext1
debugajloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debugajb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debugajloop2
debugajb:
        mov     edx,OFFSET debugajtext2
        push    cs
        pop     ds
        mov     ecx,2
        mov     bx,1
        mov     ah,40h
        int     21h
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     debugajout

debugajtext1    DB      'Done with _Exec...',0
debugajtext2    DB      13,10

debugajout:
ENDIF

        ret
;
@@no_file_error: mov    eax,1
        stc
        jmp     @@exit
@@file_error:   mov     eax,2
        stc
        jmp     @@exit
@@mem_error:    mov     eax,3
        stc
        jmp     @@exit
;
@@Name: ;
        df 0
@@Flags:        ;
        dd 0
@@Command:      ;
        df 0
@@Environment:  ;
        dw 0
@@Handle:       ;
        dw 0
@@Temp: ;
        dd 0
@@ExecAdd:      ;
        dd 0
_Exec   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Load, relocate and execute the application code. 3P format loader.
;
;On Entry:
;
;EBX    - Mode.
;       0 - Normal EXEC.
;       1 - Load for debug.
;       2 - Load for overlay.
;DS:EDX - File name.
;ES:ESI - Command line.
;CX     - Environment selector, 0 to use existing copy.
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;If Mode=0
;
;AL = ErrorLevel (returned by child's terminate)
;
;If Mode=1
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;DI     - Auto DS.
;EBP    - Segment definition memory.
;
;If Mode=2
;
;CX:EDX - Entry CS:EIP
;BX:EAX - Entry SS:ESP
;SI     - PSP.
;
;Error codes:
;
;1      - DOS file access error.
;2      - Not a 3P file.
;3      - Not enough memory.
;
Load3P  proc    near
        mov     ax,ds
        pushm   ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        pushm   w[@@SegSS],d[@@SegMem],w[@@EntryDS]
        pushm   d[@@ExpMem],d[@@EntryEIP],w[@@SegCS],d[@@EntryESP]
        pushm   d[@@SegMem],d[@@RelocMem],d[@@ProgBase],d[@@Segs],w[@@PSP]
        pushm   d[@@AutoOffset],d[@@Command],w[@@Command+4],d[@@Flags]
        pushm   d[@@Name],w[@@Name+4],w[@@Environment],d[@@SystemFlags]
        pushm   d[@@ModLink],d[@@ImpMem],d[@@ImpCnt],d[@@3PStart]
        ;
        mov     d[@@Name],edx
        mov     w[@@Name+4],ax
        mov     d[@@Flags],ebx
        mov     d[@@Command],esi
        mov     w[@@Command+4],es
        mov     w[@@Environment],cx
        ;
        mov     w[@@PSP],0
        mov     w[@@Handle],0
        mov     d[@@SegMem],0
        mov     d[@@RelocMem],0
        mov     d[@@AutoOffset],0
        mov     d[@@ExpMem],0
        mov     d[@@ModLink],0
        mov     d[@@ImpMem],0
;
;Try and open the file.
;
        push    ds
        lds     edx,f[@@Name]
        mov     ax,3d00h                ;open, read only.
        int     21h
        pop     ds
        jc      @@no_file_error
        mov     w[@@Handle],ax  ;store the handle.
;
;Check for just a 3P file with no extender.
;
@@Look3P:       mov     bx,w[@@Handle]
        mov     edx,offset apiNewHeader ;somewhere to put the info.
        mov     ecx,size NewHeaderStruc ;size of it.
        mov     ah,3fh
        int     21h
        jc      @@file_error
        cmp     ax,size NewHeaderStruc  ;did we read right amount?
        jnz     @@file_error
        ;
        cmp     w[apiNewHeader],'ZM'
        jnz     @@CheckNew
        ;
        ;Move back to EXE details.
        ;
        mov     bx,w[@@Handle]
        mov     dx,-(size NewHeaderStruc)
        mov     cx,-1
        mov     ax,4201h
        int     21h             ;move back to start of the header.
        ;
        mov     bx,w[@@Handle]
        mov     edx,offset apiExeSignature ;somewhere to put the info.
        mov     ecx,1bh         ;size of it.
        mov     ah,3fh
        int     21h
        jc      @@file_error
        cmp     ax,1bh          ;did we read right amount?
        jnz     @@file_error
        ;
        mov     ax,w[apiExeLength+2]    ;get length in 512 byte blocks

; MED 01/17/96
        cmp     WORD PTR [apiExeLength],0
        je      medexe3         ; not rounded if no modulo

        dec     ax              ;lose 1 cos its rounded up

medexe3:
        add     ax,ax           ;mult by 2
        mov     dh,0
        mov     dl,ah
        mov     ah,al
        mov     al,dh           ;mult by 256=*512
        add     ax,w[apiExeLength]      ;add length mod 512
        adc     dx,0            ;add any carry to dx
        mov     cx,ax
        xchg    cx,dx           ;swap round for DOS.
        mov     ax,4200h                ;set absolute position.
        int     21h
        jmp     @@Look3P
        ;
@@CheckNew:     cmp     w[apiNewHeader],'P3'    ;ID ok?
        jnz     @@file_error
;
;Check if this is the right module.
;
        cmp     ExecMCount,0
        jz      @@emc0
        dec     ExecMCount
        mov     edx,d[apiNewHeader+NewSize]
        sub     edx,size NewHeaderStruc
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        jmp     @@Look3P
;
;Get file offset and store it.
;
@@emc0: mov     bx,w[@@Handle]
        xor     cx,cx
        xor     dx,dx
        mov     ax,4201h
        int     21h
        shl     edx,16
        mov     dx,ax
        mov     d[@@3PStart],edx
;
;Close the file again.
;
        mov     bx,w[@@Handle]
        mov     ah,3eh
        int     21h
        mov     w[@@Handle],0
;
;Create a new PSP.
;
        push    ds
        mov     ebx,d[@@Flags]
        mov     cx,w[@@Environment]
        les     esi,f[@@command]
        lds     edx,f[@@name]
        call    CreatePSP
        pop     ds
        mov     w[@@PSP],bx
        jc      @@mem_error
;
;Open the input file again.
;
        push    ds
        lds     edx,f[@@Name]
        mov     ax,3d00h                ;open, read only.
        int     21h
        pop     ds
        jc      @@file_error
        mov     w[@@Handle],ax  ;store the handle.
;
;Move past 3P header again.
;
        mov     edx,d[@@3PStart]
        mov     cx,dx
        shr     edx,16
        xchg    dx,cx
        mov     ax,4200h
        mov     bx,w[@@Handle]
        int     21h
;
;Grab flags.
;
        mov     esi,offset apiNewHeader
        mov     eax,NewFlags[esi]
        mov     d[@@SystemFlags],eax
        test    b[@@SystemFlags+3],128  ;compressed?
        jz      @@noret
        cmp     d[@@Flags],1            ;debug load?
        jnz     @@noret
        jmp     @@file_error
;
;Setup auto stack stuff if its needed.
;
@@noret:        mov     esi,offset apiNewHeader
        cmp     NewEntryESP[esi],0      ;Need an automatic stack?
        jnz     @@NotAutoESP
        mov     eax,NewAutoStack[esi]   ;Get auto stack size.
        or      eax,eax
        jnz     @@GotAutoSize
        mov     eax,1024
@@GotAutoSize:  mov     NewEntryESP[esi],eax    ;Setup ESP value.
        mov     ebx,NewAlloc[esi]
        add     NewAlloc[esi],eax       ;update memory size needed.
        mov     d[@@AutoOffset],ebx     ;store it for later.
;
;Get EXPORT memory.
;
@@NotAutoESP:   mov     esi,offset apiNewHeader
        mov     ecx,NewExports[esi]
        or      ecx,ecx
        jz      @@NoExports0
        sys     GetMemLinear32
        jc      @@mem_error
        mov     d[@@ExpMem],esi
;
;Get IMPORT module link table memory.
;
@@NoExports0:   mov     esi,offset apiNewHeader
        mov     ecx,NewImportModCnt[esi]
        or      ecx,ecx
        jz      @@NoImports0
        shl     ecx,2           ;dword per entry.
        add     ecx,4           ;allow for count dword.
        sys     GetMemLinear32
        jc      @@mem_error
        mov     d[@@ModLink],esi
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,RealSegment
        assume es:nothing
        mov     es:d[esi],0             ;clear entry count for now.
        mov     es,w[@@PSP]
        mov     es:d[EPSP_Imports],esi
        pop     es
;
;Get program main memory.
;
@@NoImports0:   mov     esi,offset apiNewHeader
        mov     ecx,NewAlloc[esi]       ;get memory size required.
        sys     GetMemLinear32  ;Get segment/selector.
        jc      @@mem_error             ;Not enough memory.
        mov     d[@@ProgBase],esi
;
;Get segment definition memory, selectors and details.
;
        mov     esi,offset apiNewHeader
        movzx   ecx,NewSegments[esi]    ;get number of segments.
        cmp     d[@@AutoOffset],0       ;need auto SS?
        jz      @@NoAutoSeg
        inc     cx              ;one more for the stack.
@@NoAutoSeg:    push    cx
        shl     ecx,3           ;8 bytes per entry.
        sys     GetMemLinear32
        pop     cx
        jc      @@mem_error
        mov     d[@@SegMem],esi
        sys     GetSels
        jc      @@mem_error
        mov     w[@@Segs],bx            ;store base selector.
        mov     w[@@Segs+2],cx  ;store number of selectors.
;
;Update programs memory and selector details in PSP and variables.
;
        push    es
        mov     es,w[@@PSP]
        mov     ax,w[@@Segs]            ;get base selector.
        mov     es:w[EPSP_SegBase],ax
        mov     ax,w[@@Segs+2]  ;get number of selectors.
        shl     ax,3
        mov     es:w[EPSP_SegSize],ax
        mov     eax,d[@@ProgBase]       ;get memory address.
        mov     es:d[EPSP_MemBase],eax
        mov     es:d[EPSP_NearBase],eax
        mov     edi,offset apiNewHeader
        mov     eax,NewAlloc[edi]       ;get memory size.
        mov     es:d[EPSP_MemSize],eax
        pop     es
;
;Read segment definitions.
;
        test    b[@@SystemFlags+3],128  ;compressed?
        jz      @@ncp0
        mov     bx,w[@@Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[@@SegMem]
        sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     @@file_error
        jmp     @@ncp1
        ;
@@ncp0: mov     esi,offset apiNewHeader
        movzx   ecx,NewSegments[esi]    ;get number of segments.
        shl     ecx,3           ;8 bytes per entry.
        mov     bx,w[@@Handle]
        mov     edx,d[@@SegMem]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      @@file_error
        cmp     eax,ecx
        jnz     @@file_error
;
;Setup a segment definition for auto-stack if needed.
;
@@ncp1: cmp     d[@@AutoOffset],0
        jz      @@NoAutoMake
        mov     esi,offset apiNewHeader
        movzx   edi,NewSegments[esi]    ;get number of segments.
        mov     NewEntrySS[esi],di      ;store SS number.
        shl     edi,3           ;8 bytes per entry.
        add     edi,d[@@SegMem] ;offset to space for new entry.
        inc     NewSegments[esi]        ;update number of segments.
        mov     eax,d[@@AutoOffset]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     es:[edi],eax            ;setup base.
        mov     eax,NewEntryESP[esi]    ;get limit.
        cmp     eax,100000h
        jc      @@NoAutoGBit
        shr     eax,12
        or      eax,1 shl 20            ;set G bit.
@@NoAutoGBit:   or      eax,2 shl 21            ;set class 'stack'
        mov     es:[edi+4],eax  ;setup limit entry.
;
;Get relocation table memory.
;
@@NoAutoMake:   mov     esi,offset apiNewHeader
        mov     ecx,NewRelocs[esi]      ;get number of relocation entries.
        or      ecx,ecx
        jz      @@NoRelocsMem
        shl     ecx,2           ;4 bytes per entry.
        sys     GetMemLinear32
        jc      @@mem_error
        mov     d[@@RelocMem],esi
;
;Read relocation entries.
;
        test    b[@@SystemFlags+3],128  ;compressed?
        jz      @@ncp2
        mov     bx,w[@@Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[@@RelocMem]
        sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     @@file_error
        jmp     @@NoRelocsMem
        ;
@@ncp2: mov     esi,offset apiNewHeader
        mov     ecx,NewRelocs[esi]      ;get number of relocation entries.
        shl     ecx,2           ;4 bytes per entry.
        mov     bx,w[@@Handle]
        mov     edx,d[@@RelocMem]
        pushm   ecx,ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        popm    ecx,ds
        jc      @@file_error
        cmp     eax,ecx         ;did we get enough?
        jnz     @@file_error
;
;Read export table.
;
@@NoRelocsMem:  mov     esi,offset apiNewHeader
        mov     ecx,NewExports[esi]     ;get length of export table.
        or      ecx,ecx
        jz      @@NoExpMem
        ;
        test    b[@@SystemFlags+3],128  ;compressed?
        jz      @@ncp3
        mov     bx,w[@@Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[@@ExpMem]
        sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     @@file_error
        jmp     @@NoExpMem
        ;
@@ncp3: mov     bx,w[@@Handle]
        mov     edx,d[@@ExpMem]
        pushm   ecx,ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        popm    ecx,ds
        jc      @@file_error
        cmp     eax,ecx         ;did we get enough?
        jnz     @@file_error
;
;Get IMPORT memory.
;
@@NoExpMem:     mov     esi,offset apiNewHeader
        mov     ecx,NewImports[esi]     ;get size of IMPORT table.
        or      ecx,ecx
        jz      @@NoImpMem1
        shl     ecx,2           ;4 bytes per entry.
        sys     GetMemLinear32
        jc      @@mem_error
        mov     d[@@ImpMem],esi
;
;Read IMPORT entries.
;
        test    b[@@SystemFlags+3],128  ;compressed?
        jz      @@ncp20
        mov     bx,w[@@Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[@@ImpMem]
        sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     @@file_error
        jmp     @@NoImpMem1
        ;
@@ncp20:        mov     esi,offset apiNewHeader
        mov     ecx,NewImports[esi]     ;get size of IMPORT table.
        mov     bx,w[@@Handle]
        mov     edx,d[@@ImpMem]
        pushm   ecx,ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        popm    ecx,ds
        jc      @@file_error
        cmp     eax,ecx         ;did we get enough?
        jnz     @@file_error
;
;Read exe image.
;
@@NoImpMem1:    mov     esi,offset apiNewHeader
        test    NewFlags[esi],1 shl 31  ;compressed?
        jz      @@NotComp
        mov     bx,w[@@Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[@@ProgBase]
        sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     @@file_error
        jmp     @@ImageLoaded
        ;
@@NotComp:      test    NewFlags[esi],1 shl 30
        jz      @@LoadAll

        jmp     @@LoadVMM

@@LoadAll:      mov     edx,d[@@ProgBase]
        mov     ecx,NewLength[esi]      ;get image length.
        mov     bx,w[@@Handle]
        pushm   ecx,ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile                ;read the file.
        assume ds:_apiCode
        popm    ecx,ds
        jc      @@file_error            ;problems problems.
        cmp     eax,ecx         ;did we get right amount?
        jnz     @@file_error
        jmp     @@ImageLoaded
;
;Break the file read up into 8K chunks that suit the relocations.
;
@@LoadVMM:      mov     eax,offset apiNewHeader
        mov     eax,NewRelocs[eax]      ;get number of relocation entries.
        mov     d[@@SL_RelocLeft],eax
        mov     eax,d[@@RelocMem]
        mov     d[@@SL_RelocPnt],eax
        mov     eax,offset apiNewHeader
        mov     eax,NewLength[eax]
        mov     d[@@SL_LoadLeft],eax
        mov     eax,d[@@ProgBase]
        mov     d[@@SL_LoadPnt],eax
        mov     d[@@EntryEIP],0
        ;
@@sl0:  ;Anything left to load?
        ;
        cmp     d[@@SL_LoadLeft],0
        jz      @@ImageLoaded
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        ;
        ;Work out how much we should load.
        ;
        mov     ecx,d[@@SL_LoadLeft]
        cmp     d[@@SL_RelocLeft],0
        jz      @@sl3
        mov     esi,d[@@SL_RelocPnt]
        mov     eax,es:[esi]            ;get fixup address.
        and     eax,0FFFFFFFh
        add     eax,4           ;allow for cross page fixup.
        add     eax,8191
        and     eax,not 8191            ;get terminal offset.
        mov     ebx,d[@@SL_LoadPnt]
        sub     ebx,d[@@ProgBase]
        sub     eax,ebx         ;get relative offset.
        cmp     eax,ecx
        jnc     @@sl1
        mov     ecx,eax
        ;
        ;Load next program chunk.
        ;
@@sl1:
        or      ecx,ecx
        jnz     @@sl3

        mov     eax,d[@@EntryEIP]
        or      bx,-1
        mov     es,bx

@@sl3:

        mov     edx,d[@@SL_LoadPnt]
        mov     bx,w[@@Handle]
        pushm   ecx,ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile                ;read the file.
        assume ds:_apiCode
        popm    ecx,ds
        jc      @@file_error            ;problems problems.
        cmp     eax,ecx         ;did we get right amount?
        jnz     @@file_error
        ;
        ;Update variables.
        ;
        add     d[@@SL_LoadPnt],ecx
        sub     d[@@SL_LoadLeft],ecx
        ;
        ;Process all fixups in range of what we loaded.
        ;
        mov     esi,d[@@SL_RelocPnt]
        mov     ecx,d[@@SL_RelocLeft]
        mov     fs,w[@@PSP]
        ;
        mov     bx,w[@@Segs]
        mov     edi,fs:d[EPSP_MemBase]
        add     edi,fs:d[EPSP_MemSize]
        mov     dx,fs:w[EPSP_SegSize]
        shr     dx,3
@@SL_RelocLoop: or ecx,ecx
        jz      @@SL_DoneRelocs
        mov     ebp,es:[esi]            ;get linear offset.
        mov     eax,ebp
        and     ebp,0FFFFFFFh   ;Lose relocation type.
        shr     eax,28          ;Get relocation type.
        add     ebp,d[@@ProgBase]       ;get linear address.
        cmp     ebp,edi
        jnc     @@file_error2
        or      al,al
        jz      @@SL_Seg16
        dec     al
        jz      @@SL_Offset32
        jmp     @@file_error2
        ;
@@SL_Offset32:  mov     eax,ebp
        add     eax,4-1
        cmp     eax,d[@@SL_LoadPnt]     ;beyond what we loaded?
        jnc     @@SL_DoneRelocs
        mov     eax,es:[ebp]            ;Get offset.
        add     eax,d[@@ProgBase]
        cmp     eax,edi         ;Beyond program?
        jnc     @@file_error2
        mov     eax,d[@@ProgBase]
        add     es:d[ebp],eax   ;Do the relocation.
        jmp     @@SL_NextReloc
        ;
@@SL_Seg16:     mov     eax,ebp
        add     eax,2-1
        cmp     eax,d[@@SL_LoadPnt]     ;beyond what we loaded?
        jnc     @@SL_DoneRelocs
        mov     ax,es:[ebp]             ;get selector number.
        cmp     ax,dx
        jnc     @@file_error2
        shl     ax,3            ;*8 for selector value.
        add     ax,bx           ;add base selector.
        mov     es:[ebp],ax             ;set segment selector.
        ;
@@SL_NextReloc:
        add     esi,4           ;next relocation entry.
        inc     d[@@EntryEIP]
        dec     ecx
        jnz     @@SL_RelocLoop
        ;
@@SL_DoneRelocs:
        mov     d[@@SL_RelocPnt],esi
        mov     d[@@SL_RelocLeft],ecx
        mov     esi,offset apiNewHeader
        mov     NewRelocs[esi],ecx
        jmp     @@sl0
;
;Close the file now we're done with it.
;
@@ImageLoaded:  pushm   ds,ds
        popm    es,fs
        ;
        mov     bx,w[@@Handle]  ;grab handle again,
        mov     w[@@Handle],0
        mov     ah,3eh          ;and close it.
        int     21h
;
;Process relocations.
;
        mov     esi,offset apiNewHeader
        mov     ecx,NewRelocs[esi]      ;get number of relocation entries.
        or      ecx,ecx
        jz      @@DoneReloc
        mov     esi,d[@@RelocMem]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     fs,w[@@PSP]
        ;
        mov     d[@@EntryEIP],0
        mov     bx,w[@@Segs]
        mov     edi,fs:d[EPSP_MemBase]
        add     edi,fs:d[EPSP_MemSize]
        mov     dx,fs:w[EPSP_SegSize]
        shr     dx,3
@@RelocLoop:    mov     ebp,es:[esi]            ;get linear offset.
        mov     eax,ebp
        and     ebp,0FFFFFFFh   ;Lose relocation type.
        shr     eax,28          ;Get relocation type.
        add     ebp,d[@@ProgBase]       ;get linear address.
        cmp     ebp,edi
        jnc     @@file_error2
        or      al,al
        jz      @@Seg16
        dec     al
        jz      @@Offset32
        jmp     @@file_error2
        ;
@@Offset32:     mov     eax,es:[ebp]            ;Get offset.
        add     eax,d[@@ProgBase]
        cmp     eax,edi         ;Beyond program?
        jnc     @@file_error2
        mov     eax,d[@@ProgBase]
        add     es:d[ebp],eax   ;Do the relocation.
        jmp     @@NextReloc
        ;
@@Seg16:        mov     ax,es:[ebp]             ;get selector number.
        cmp     ax,dx
        jnc     @@file_error2
        shl     ax,3            ;*8 for selector value.
        add     ax,bx           ;add base selector.
        mov     es:[ebp],ax             ;set segment selector.
        ;
@@NextReloc:    add     esi,4           ;next relocation entry.
        inc     d[@@EntryEIP]
        dec     ecx
        jnz     @@RelocLoop
        pushm   ds,ds
        popm    es,fs
;
;Set selector details.
;
@@DoneReloc:    mov     esi,offset apiNewHeader
        movzx   ecx,NewSegments[esi]    ;get number of segments.
        mov     esi,d[@@SegMem]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     bx,w[@@Segs]            ;base selector.
@@SegLoop:      pushm   ebx,ecx,esi
        ;
        mov     eax,es:[esi+4]  ;Get limit.
        mov     ecx,eax
        and     ecx,0fffffh             ;mask to 20 bits.
        test    eax,1 shl 20            ;G bit set?
        jz      @@NoGBit
        shl     ecx,12
        or      ecx,4095
@@NoGBit:       or      ecx,ecx
        jz      @@NoDecLim
        cmp     ecx,-1
        jz      @@NoDecLim
        dec     ecx
@@NoDecLim:     mov     edx,es:[esi]            ;get base.
        ;
        test    eax,1 shl 27            ;FLAT segment?
        jz      @@NotFLATSeg
        ;
        push    fs
        mov     fs,w[@@PSP]
        mov     fs:d[EPSP_NearBase],0   ;Make sure NEAR functions work.
        pop     fs
        ;
        add     edx,d[@@ProgBase]
        or      ecx,-1          ;Update the limit.
        xor     edx,edx
        jmp     @@DoSegSet
        ;
@@NotFLATSeg:   add     edx,d[@@ProgBase]       ;offset within real memory.
        ;
@@DoSegSet:     sys     SetSelDet32
        mov     cx,w[@@SystemFlags]     ;use default setting.
        shr     cx,14
        xor     cl,1
        or      cl,b[@@SystemFlags+2]
        and     cl,1
        call    _DSizeSelector
        ;
        mov     eax,es:[esi+4]  ;Get class.
        shr     eax,21          ;move type into useful place.
        and     eax,0fh         ;isolate type.
        or      eax,eax
        jz      @@CodeSeg
        jmp     @@SegDone
        ;
@@CodeSeg:      mov     eax,es:[esi+4]  ;Get type bits.
        mov     cx,0            ;Set 16 bit seg.
        test    eax,1 shl 25
        jnz     @@Default
        mov     cx,1
        test    eax,1 shl 26            ;32 bit seg?
        jnz     @@Default
        mov     cx,w[@@SystemFlags]     ;use default setting.
        shr     cx,14
        and     cx,1            ;get code size
        xor     cx,1            ;flip it for selector function.
@@Default:      sys     CodeSel
        ;
@@SegDone:      popm    ebx,ecx,esi
        add     esi,8           ;next definition.
        add     ebx,8           ;next selector.
        dec     ecx
        jnz     @@SegLoop
;
;Update export table values.
;
        cmp     d[@@ExpMem],0
        jz      @@exp9
        push    es
        mov     es,w[@@PSP]
        mov     esi,d[@@ExpMem]
        mov     edi,d[@@SegMem]
        mov     es:d[EPSP_Exports],esi
        mov     dx,es:[EPSP_SegBase]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     ecx,es:[esi]
        mov     ebp,esi
        add     esi,4
        ;
        ;Do module name entry.
        ;
        add     es:[esi],ebp            ;turn offset into an address.
        add     esi,4
        ;
        ;Do all other entries.
        ;
@@exp0: add     es:[esi],ebp            ;turn offset into an address.
        mov     ebx,es:[esi]            ;get address of export entry.
        movzx   eax,es:w[ebx+4] ;get segment def.
        add     es:w[ebx+4],dx  ;update seg
        shl     eax,3
        add     eax,edi         ;point into seg defs.
        test    es:d[eax+4],1 shl 27    ;FLAT seg?
        jz      @@exp1
        mov     eax,d[@@ProgBase]
        add     es:d[ebx],eax   ;update to reflect FLAT
@@exp1: add     esi,4
        dec     ecx
        jnz     @@exp0
        pop     es
@@exp9: ;
;
;Setup entry regs.
;
        mov     esi,offset apiNewHeader
        ;
        mov     ebx,NewEntryEIP[esi]    ;get entry address.
        movzx   eax,NewEntryCS[esi]
        shl     eax,3
        add     ax,w[@@Segs]
        mov     w[@@SegCS],ax
        sub     ax,w[@@Segs]
        add     eax,d[@@SegMem] ;point to this segments entry.
        test    es:d[eax+4],1 shl 27    ;FLAT segment?
        jz      @@NotFLATEIP
        add     ebx,es:[eax]            ;include segments offset within image.
        add     ebx,d[@@ProgBase]
@@NotFLATEIP:   mov     d[@@EntryEIP],ebx
        ;
        mov     ebx,NewEntryESP[esi]
        movzx   eax,NewEntrySS[esi]
        shl     eax,3
        add     ax,w[@@Segs]
        mov     w[@@SegSS],ax
        sub     ax,w[@@Segs]
        add     eax,d[@@SegMem] ;point to this segments entry.
        test    es:d[eax+4],1 shl 27    ;FLAT segment?
        jz      @@NotFLATESP
        add     ebx,es:[eax]            ;include segments offset within image.
        add     ebx,d[@@ProgBase]
@@NotFLATESP:   mov     d[@@EntryESP],ebx
        ;
        push    es
        mov     es,w[@@PSP]
        mov     ax,es
        pop     es
        cmp     NewAutoDS[esi],0
        jz      @@NotAUTODS
        mov     ax,NewAutoDS[esi]
        dec     ax
        shl     ax,3
        add     ax,w[@@Segs]
@@NotAUTODS:    mov     w[@@EntryDS],ax
;
;Convert import module names to pointers to export tables.
;
        cmp     d[@@ImpMem],0   ;any imports?
        jz      @@imp9
        mov     d[@@ImpCnt],0   ;Clear the counter.
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
@@imp0: mov     esi,d[@@ImpMem]
        add     esi,es:d[esi]   ;move to module name list.
        mov     eax,d[@@ImpCnt]
        cmp     eax,es:d[esi]   ;Done all modules?
        jnc     @@imp6
        add     esi,es:[esi+4+eax*4]    ;Point to this module name.
        ;
        ;Try and find the module.
        ;
        call    FindModule
        jnc     @@got_module
        ;
        ;Put module's name into command line error message.
        ;
        ;ES:ESI is the name string.
        ;
        push    ds
        push    ax
        mov     ds,apiDSeg
        assume ds:_cwMain
        cmp     ax,1
        jnz     @@egm1
        mov     edi,offset ErrorM10_T
        movzx   ecx,es:b[esi]
        inc     esi
        mov     b[edi],'"'
        inc     edi
@@egm0: mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        dec     ecx
        jnz     @@egm0
        mov     b[edi+0],'"'
        mov     b[edi+1],13
        mov     b[edi+2],10
        mov     b[edi+3],"$"
@@egm1: pop     ax
        assume ds:_apiCode
        pop     ds
        stc
        jmp     @@error
        ;
@@got_module:   ;Update the module link table and usage count.
        ;
        mov     esi,d[@@ModLink]
        inc     es:d[esi]               ;update count.
        mov     eax,d[@@ImpCnt]
        mov     es:[esi+4+eax*4],edi    ;store EPSP link pointer.
        ;
        ;Move onto next module name.
        ;
        inc     d[@@ImpCnt]
        jmp     @@imp0
;
;Now it's time to work through the relocations performing the fixups.
;
@@imp6: mov     esi,d[@@ImpMem]
        add     esi,es:d[esi+8] ;move to fixup list.
        mov     eax,es:[esi]            ;get number of entries.
        or      eax,eax
        jz      @@imp9
        mov     d[@@ImpCnt],eax
        add     esi,4
@@imp7: xor     eax,eax
        mov     al,es:[esi]
        mov     d[@@ImpType],eax
        mov     eax,es:[esi+1]
        mov     d[@@ImpFlags],eax
        add     esi,1+4
        ;
        ;Point to the right modules export list.
        ;
        mov     ecx,d[@@ImpFlags]       ;get flags
        shr     ecx,30
        xor     eax,eax
@@imp8: shl     eax,8
        mov     al,es:[esi]
        inc     esi
        dec     ecx
        jnz     @@imp8
        mov     edi,d[@@ModLink]
        mov     edi,es:[edi+4+eax*4]
        mov     edi,es:EPSP_Exports[edi]
        ;
        ;Point to the import name string.
        ;
        mov     ecx,d[@@ImpFlags]
        shr     ecx,28
        and     ecx,3
        xor     eax,eax
@@imp10:        shl     eax,8
        mov     al,es:[esi]
        inc     esi
        dec     ecx
        jnz     @@imp10
        ;
        ;Ordinal or string import?
        ;
        test    d[@@ImpType],128        ;ordinal?
        jz      @@imp14
        cmp     es:[edi],eax            ;check number of entries.
        jc      @@file_error
        mov     edi,es:[edi+4+eax*4]
        jmp     @@imp11
        ;
@@imp14:        mov     ebp,d[@@ImpMem]
        add     ebp,es:[ebp+4]  ;point to name list.
        add     ebp,es:[ebp+4+eax*4]    ;point to name string.
        ;
        ;Try and find name string in export list.
        ;
        call    FindFunction
        jc      @@file_error
        ;
@@imp11:        ;Found the right entry, now do the fixup.
        ;
        mov     ebx,d[@@ImpFlags]
        and     ebx,0FFFFFFFh
        add     ebx,d[@@ProgBase]
        mov     edx,es:[edi]
        xor     ecx,ecx
        mov     cx,es:[edi+4]
        mov     eax,d[@@ImpType]
        ;
        ;Check if this is self relative.
        ;
        test    eax,64
        jz      @@imp15
        and     eax,63
        mov     edx,ebx         ;must be flat.
        cmp     eax,1
        jz      @@imp18
        mov     edx,d[@@ImpFlags]
        and     edx,0FFFFFFFh
@@imp18:        add     edx,2
        test    eax,1
        jz      @@imp16
        add     edx,2
@@imp16:        sub     edx,es:[edi]
        neg     edx
@@imp15:        and     eax,63
        or      eax,eax
        jz      @@imp_offset16
        dec     eax
        jz      @@imp_offset32
        dec     eax
        jz      @@imp_pointer16
        dec     eax
        jz      @@imp_pointer32
        dec     eax
        jz      @@imp_base
        jmp     @@file_error
        ;
        ;Do a 16-bit offset fixup.
        ;
@@imp_offset16: mov     es:[ebx],dx
        jmp     @@imp17
        ;
        ;Do a 32-bit offset fixup.
        ;
@@imp_offset32: mov     es:[ebx],edx
        jmp     @@imp17
        ;
        ;Do a 16-bit pointer fixup.
        ;
@@imp_pointer16: mov    es:[ebx],dx
        mov     es:[ebx+2],cx
        jmp     @@imp17
        ;
        ;Do a 32-bit pointer fixup.
        ;
@@imp_pointer32: mov    es:[ebx],edx
        mov     es:[ebx+4],cx
        jmp     @@imp17
        ;
        ;Do a base fixup.
        ;
@@imp_base:     mov     es:[ebx],cx
        ;
@@imp17:        dec     d[@@ImpCnt]
        jnz     @@imp7
@@imp9: ;
;
;Lose relocation memory.
;
        mov     esi,d[@@RelocMem]
        sys     RelMemLinear32  ;release relocation list memory.
        mov     d[@@RelocMem],0
;
;Lose import memory.
;
        mov     esi,d[@@ImpMem] ;release IMPORT memory.
        or      esi,esi
        jz      @@NoRelImp9
        sys     RelMemLinear32
        mov     d[@@ImpMem],0
;
;Lose segment definitions.
;
@@NoRelImp9:    cmp     d[@@Flags],1
        jz      @@NoSegMemRel
        mov     esi,d[@@SegMem]
        sys     RelMemLinear32  ;release segment memory.
        mov     d[@@SegMem],0
;
;Check if this is an exec or just a load.
;
@@NoSegMemRel:  cmp     d[@@Flags],0
        jz      @@Exec
;
;Switch back to parents PSP if this is a debug load.
;
        cmp     d[@@Flags],2
        jz      @@NoPSwitch2
        push    fs
        mov     fs,w[@@PSP]
        mov     bx,fs:w[EPSP_Parent]
        pop     fs
        mov     ah,50h
        int     21h
        mov     ebp,d[@@SegMem]
;
;Return program details to caller.
;
@@NoPSwitch2:   mov     edx,d[@@EntryEIP]
        mov     cx,w[@@SegCS]
        mov     eax,d[@@EntryESP]
        mov     bx,w[@@SegSS]
        mov     si,w[@@PSP]
        mov     di,w[@@EntryDS]
        clc
        jmp     @@exit
;
;Run it.
;
@@Exec: mov     eax,d[@@Flags]
        mov     ebx,d[@@EntryEIP]
        mov     cx,w[@@SegCS]
        mov     edx,d[@@EntryESP]
        mov     si,w[@@SegSS]
        mov     di,w[@@PSP]
        mov     bp,w[@@EntryDS]
        call    ExecModule
        clc
;
;Shut down anything still hanging around.
;
@@error:        pushf
        push    ax
;
;Make sure file is closed.
;
        pushf
        xor     bx,bx
        xchg    bx,w[@@Handle]
        or      bx,bx
        jz      @@NoClose
        mov     ah,3eh
        int     21h
;
;Make sure all work spaces are released.
;
@@NoClose:      mov     esi,d[@@RelocMem]
        or      esi,esi
        jz      @@NoRelRel
        sys     RelMemLinear32  ;release relocation list memory.
        mov     d[@@RelocMem],0
@@NoRelRel:     mov     esi,d[@@SegMem]
        or      esi,esi
        jz      @@NoSegRel
        sys     RelMemLinear32  ;release segment memory.
        mov     d[@@SegMem],0
@@NoSegRel:     mov     esi,d[@@ImpMem]
        or      esi,esi
        jz      @@NoImpRel
        sys     RelMemLinear32
;
;Restore previous state.
;
@@NoImpRel:     popf
        jnc     @@RelPSP
        cmp     w[@@PSP],0
        jz      @@NoRelRes
;
;Restore vectors & DPMI state.
;
@@RelPSP:       pushm   ds,ds,ds
        popm    es,fs,gs
        mov     eax,d[@@Flags]
        mov     bx,w[@@PSP]
        call    DeletePSP
;
;Return to caller.
;
@@NoRelRes:     pop     ax
        popf
        ;
@@exit: popm    d[@@ModLink],d[@@ImpMem],d[@@ImpCnt],d[@@3PStart]
        popm    d[@@Name],w[@@Name+4],w[@@Environment],d[@@SystemFlags]
        popm    d[@@AutoOffset],d[@@Command],w[@@Command+4],d[@@Flags]
        popm    d[@@SegMem],d[@@RelocMem],d[@@ProgBase],d[@@Segs],w[@@PSP]
        popm    d[@@ExpMem],d[@@EntryEIP],w[@@SegCS],d[@@EntryESP]
        popm    w[@@SegSS],d[@@SegMem],w[@@EntryDS]
        ;
        popm    ds,es,fs,gs
        ret
;
;Not enough memory error.
;
@@mem_error:    mov     ax,3
        stc
        jmp     @@error
;
;Couldn't find the file.
;
@@no_file_error: mov    ax,1
        stc
        jmp     @@error
;
;Not a 3P file.
;
@@file_error:   mov     ax,2
        stc
        jmp     @@error
;
;Corrupt file.
;
@@file_error2:  mov     eax,d[@@EntryEIP]       ;Get the relocation number.
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     b[ErrorM11_0+0]," "
        mov     b[ErrorM11_0+1]," "
        mov     b[ErrorM11_0+2]," "
        mov     ecx,8
        mov     edi,offset ErrorM11_1
        call    Bin2HexA
        assume ds:_apiCode
        pop     ds
        mov     ax,2
        stc
        jmp     @@error
;
@@Name: ;
        df 0,0
@@Environment:  ;
        dw 0
@@SystemFlags:  ;
        dd ?
@@AutoOffset:   ;
        dd ?
@@Command:      ;
        df ?,0
@@Flags:        ;
        dd 0
@@ErrorCode:    ;
        dw 0            ;error number.
@@Handle:       ;
        dw 0            ;file handle.
@@EntryEIP:     ;
        dd 0
@@SegCS:        ;
        dw 0
@@EntryESP:     ;
        dd 0
@@SegSS:        ;
        dw 0
@@SegMem:       ;
        dd 0
@@RelocMem:     ;
        dd 0
@@ProgBase:     ;
        dd 0
@@Segs: ;
        dd 0
@@PSP:  ;
        dw 0
@@EntryDS:      ;
        dw 0
@@ExpMem:       ;
        dd 0
@@ModLink:      ;
        dd 0
@@ImpMem:       ;
        dd 0
@@ImpCnt:       ;
        dd 0
@@ImpFlags:     ;
        dd 0
@@ImpType:      ;
        dd 0
@@3PStart:      ;
        dd 0
@@SL_RelocPnt:  ;
        dd 0
@@SL_RelocLeft: ;
        dd 0
@@SL_LoadPnt:   ;
        dd 0
@@SL_LoadLeft:  ;
        dd 0
Load3P  endp


;-------------------------------------------------------------------------
;
;Execute module.
;
;On Entry:
;
;EAX    - Flags.
;EBX    - Entry EIP
;CX     - Entry CS
;EDX    - Entry ESP
;SI     - Entry SS
;DI     - Entry ES
;BP     - Entry DS
;
ExecModule      proc    near
        pushm   ds,es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pushm   d[@@Flags],d[@@EntryEIP],w[@@EntryCS],d[@@EntryESP]
        pushm   w[@@EntrySS],w[@@EntryES],w[@@EntryDS],w[@@PSP]
        ;
        mov     d[@@Flags],eax
        mov     d[@@EntryEIP],ebx
        mov     w[@@EntryCS],cx
        mov     d[@@EntryESP],edx
        mov     w[@@EntrySS],si
        mov     w[@@EntryES],di
        mov     w[@@EntryDS],bp
        mov     w[@@PSP],di
;
        push    es
        mov     es,di
        mov     es:d[EPSP_Links],8000000h
        pop     es
;
;Patch Int 21h for function 4ch.
;
        push    w[@@OldInt21h+6]
        mov     w[@@OldInt21h+6],0
        mov     eax,d[@@OldInt21h]
        or      ax,w[@@OldInt21h+4]
        or      eax,eax         ;Already patched?
        jnz     @@NoPatch21h
        ;
        mov     w[@@OldInt21h+6],-1
        mov     bl,21h
        sys     GetVect
        test    apiSystemFlags,1        ;16/32 bit?
        jz      @@Use32Bit100
        mov     w[@@OldInt21h+2],cx
        mov     w[@@OldInt21h],dx
        jmp     @@Use16Bit100
@@Use32Bit100:  mov     w[@@OldInt21h+4],cx
        mov     d[@@OldInt21h],edx
@@Use16Bit100:  mov     bl,21h
        mov     cx,cs
        mov     edx,offset @@Int21Patch
        sys     SetVect         ;put us in the running.
;
;Patch exception termination handler address.
;
@@NoPatch21h:   mov     es,apiDSeg
        assume es:_cwMain
        push    es:d[TerminationHandler]
        push    es:d[TerminationHandler+4]
        mov     es:w[TerminationHandler+4],cs
        mov     es:d[TerminationHandler],offset @@KillIt
        assume es:nothing
        mov     es,w[@@PSP]
        mov     es:d[EPSP_SSESP],esp
        mov     es:w[EPSP_SSESP+4],ss
        lss     esp,f[@@EntryESP]       ;fetch new stack address.
        xor     eax,eax
        mov     ebx,eax
        mov     ecx,eax
        mov     edx,eax
        mov     esi,eax
        mov     edi,eax
        mov     ebp,eax
        mov     fs,ax
        mov     gs,ax
        mov     ds,cs:w[@@EntryDS]
        sti
        assume ds:nothing
        jmp     cs:f[@@EntryEIP]        ;pass control to program.
        ;
@@Int21Patch:   cmp     ah,4ch          ;terminate?
        jz      @@CheckKillIt
        cmp     ah,31h          ;TSR?
        jnz     @@OldVect
;
;Clean things up and exit.
;
        mov     ax,cs:apiDSeg
        mov     ds,ax
        assume ds:_cwMain
        mov     ax,TSRSize
        assume ds:nothing
        mov     edi,offset Int21Buffer
        push    ds
        pop     es
        mov     Real_EAX[edi],3100h
        mov     Real_EDX[edi],eax
        mov     bl,21h
        sys     IntXX
        jmp     @@KillIt                ;just incase!
        ;
@@OldVect:      test    cs:apiSystemFlags,1     ;16/32?
        jz      @@Use32Bit101
        db 66h
        jmp     cs:f[@@OldInt21h]       ;pass control to old handler.
@@Use32Bit101:  jmp     cs:f[@@OldInt21h]
        ;
@@CheckKillIt:  push    ds
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     Int21hExecCount,0
        assume ds:nothing
        pop     ds
        jnz     @@OldVect
;
;Clean things up and return to caller.
;
@@KillIt:       mov     ds,cs:apiDDSeg  ;make our data addressable.
        assume ds:_apiCode
        mov     w[@@errorcode],ax
        mov     es,w[@@PSP]
        lss     esp,es:f[EPSP_SSESP]
        mov     es,apiDSeg
        assume es:_cwMain
        pop     es:d[TerminationHandler+4]
        pop     es:d[TerminationHandler]
        assume es:nothing
        pushm   ds,ds,ds
        popm    es,fs,gs
        sti                     ;turn interupts back on.
        ;
        cmp     w[@@OldInt21h+6],0
        jz      @@NoRel21h
        test    apiSystemFlags,1        ;16/32?
        jz      @@Use32Bit102
        mov     cx,w[@@OldInt21h+2]
        movzx   edx,w[@@OldInt21h]
        jmp     @@Use16Bit102
@@Use32Bit102:  mov     cx,w[@@OldInt21h+4]
        mov     edx,d[@@OldInt21h]
@@Use16Bit102:  mov     bl,21h
        sys     SetVect         ;restore old handler.
@@NoRel21h:     pop     w[@@OldInt21h+6]
;
;Get mouse event target state.
;
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,w[Int33hExtension+20]        ;Get mouse data segment.
        assume ds:_Int33h
        xor     eax,eax
        xor     ebx,ebx
        cmp     Int33hUserOK,0
        jz      @@meventnodum
        test    cs:apiSystemFlags,1
        jz      @@mevent32
        movzx   eax,w[Int33hUserCode]
        movzx   ebx,w[Int33hUserCode+2]
        jmp     @@mevent16
@@mevent32:     mov     eax,d[Int33hUserCode]
        movzx   ebx,w[Int33hUserCode+4]
@@mevent16:     cmp     bx,Int33hCSeg
        jnz     @@meventnodum
        cmp     eax,offset Int33hDummy
        jnz     @@meventnodum
        xor     eax,eax
        xor     ebx,ebx
@@meventnodum:  mov     ds,Int33hDSeg
        assume ds:_cwMain
        mov     MouseETarget,eax
        mov     MouseETarget+4,ebx
        pop     ds
        assume ds:_apiCode
        ;
;       mov     ax,0            ;reset mouse if present.
;       int     33h
        mov     ax,w[@@errorcode]
        ;
        popm    w[@@EntrySS],w[@@EntryES],w[@@EntryDS],w[@@PSP]
        popm    d[@@Flags],d[@@EntryEIP],w[@@EntryCS],d[@@EntryESP]
        popm    ds,es
        ret
;
@@OldInt21h:    ;
        df 0,0
@@errorcode:    ;
        dw 0
@@Flags:        ;
        dd 0
@@EntryEIP:     ;
        dd 0
@@EntryCS:      ;
        dw 0
@@EntryESP:     ;
        dd 0
@@EntrySS:      ;
        dw 0
@@EntryES:      ;
        dw 0
@@EntryDS:      ;
        dw 0
@@PSP:  ;
        dw 0
ExecModule      endp


;-------------------------------------------------------------------------
;
;Convert number into ASCII Hex version.
;
;On Entry:-
;
;EAX    - Number to convert.
;CX     - Digits to do.
;DS:EDI - Buffer to put string in.
;
Bin2HexA        proc    near
        pushm   eax,ebx,ecx,edx,edi
        movzx   ecx,cx
        mov     ebx,offset HexTableA
        add     edi,ecx
        dec     edi
        mov     edx,eax
@@0:    mov     al,dl
        shr     edx,4
        and     al,15
        xlat    cs:[ebx]
        mov     [edi],al
        dec     edi
        dec     ecx
        jnz     @@0
        popm    eax,ebx,ecx,edx,edi
        push    ecx
        movzx   ecx,cx
        add     edi,ecx
        pop     ecx
        ret
HexTableA       db '0123456789ABCDEF'
Bin2HexA        endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LosefileHandles proc near

        if      0
        pushm   ds,es,fs
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        cmp     fs:w[PSP_HandlePtr+2],0
        jz      @@donehandles
        les     di,fs:d[PSP_HandlePtr]
        mov     cx,fs:w[PSP_Handles]

        if      0
@@RelHandles:   cmp     es:b[di],255
        jz      @@NoRelHandle
        movzx   bx,es:b[di]
        mov     ah,3eh
        int     21h
@@NoRelHandle:  inc     di
        dec     cx
        jnz     @@RelHandles
        endif

        mov     ax,fs
        mov     bx,es
        cmp     ax,bx
        jz      @@donehandles
        sys     RelMem

        if      0
;
;Reduce real mode handle count.
;
        mov     edi,offset Int21Buffer
        mov     ax,TotalHandles ;get current count.
        sub     ax,fs:w[PSP_Handles]
        or      ax,1            ;force bit 0 for DOS bug.
        mov     [edi].Real_EAX,6700h
        mov     [edi].Real_EBX,eax
        mov     bl,21h
        push    ds
        pop     es
        sys     IntXX
        sub     TotalHandles,20 ;update total handle count.
        endif

@@donehandles:

        popm    ds,es,fs
        endif

        ret
        assume ds:_apiCode
LosefileHandles endp


;-------------------------------------------------------------------------
;
;Read some data from a file.
;
;On Entry:
;
;DS:EDX - Address to read to.
;ECX    - length to read.
;EBX    - file handle.
;
;On Exit:
;
;EAX    - bytes read.
;
ReadFile        proc    near
        call    DPMICopyCheck
        pushm   ecx,edx,esi
        xor     esi,esi         ;reset length read.
@@0:    pushm   ebx,ecx,edx,esi
        cmp     ecx,65535               ;size of chunks to load.
        jc      @@1
        mov     ecx,65535               ;as close to 64k as can get.
@@1:    mov     ah,3fh
        int     21h             ;read from the file.
        popm    ebx,ecx,edx,esi
        jc      @@2             ;DOS error so exit NOW.
        movzx   eax,ax          ;get length read.
        add     esi,eax         ;update length read counter.
        add     edx,eax         ;move memory pointer.
        sub     ecx,eax         ;update length counter.
        jz      @@2             ;read as much as was wanted.
        or      eax,eax         ;did we read anything?
        jnz     @@0
@@2:    mov     eax,esi
        popm    ecx,edx,esi
        ret
ReadFile        endp


;-------------------------------------------------------------------------
;
;Patch exceptions to do register dump and terminate.
;
PatchExc        proc    near
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
;
;Patch some interrupts.
;
        mov     bl,23h
        sys     GetVect
        test    apiSystemFlags,1
        jz      @@iUse32
        mov     w[OldInt23],dx
        mov     w[OldInt23+2],cx
        jmp     @@iDone3216
@@iUse32:       mov     d[OldInt23],edx
        mov     w[OldInt23+4],cx
@@iDone3216:    mov     edx,offset Int23Handler
        mov     cx,cs
        mov     bl,23h
        sys     SetVect
        ;
        mov     bl,24h
        sys     GetVect
        test    apiSystemFlags,1
        jz      @@i24Use32
        mov     w[OldInt24],dx
        mov     w[OldInt24+2],cx
        jmp     @@i24Done3216
@@i24Use32:     mov     d[OldInt24],edx
        mov     w[OldInt24+4],cx
@@i24Done3216:  mov     edx,offset Int24Handler
        mov     cx,cs
        mov     bl,24h
        sys     SetVect
;
;Patch 1Bh vector.
;
        ;
        ;Get a call back.
        ;
        pushm   ds,es,ds
        pop     es
        mov     esi,offset CtrlBrkEvent
        mov     edi,offset CtrlBrkEventTab
        push    cs
        pop     ds
        sys     GetCallBack
        popm    ds,es
        jc      @@0

        ;
        ;Get current protected mode 1Bh vector.
        ;
        pushm   cx,dx
        mov     bl,1bh
        sys     GetVect
        test    apiSystemFlags,1
        jz      @@1
        movzx   edx,dx
@@1:    mov     d[OldInt1BhP],edx
        mov     w[OldInt1BhP+4],cx
        ;
        ;Install our own handler.
        ;
        mov     edx,offset Int1bHandler
        mov     cx,cs
        mov     bl,1bh
        sys     SetVect
        popm    cx,dx

        ;
        ;Get current real mode 1Bh vector.
        ;
        pushm   cx,dx
        mov     al,1bh
        sys     GetRVect
        mov     w[OldInt1bh],dx
        mov     w[OldInt1bh+2],cx
        popm    cx,dx
        mov     w[Int1bhcall],dx
        mov     w[Int1bhcall+2],cx
        ;
        ;Install call-back.
        ;
        mov     bl,1bh
        sys     SetRVect


@@0:    mov     ds,apiDSeg
        assume ds:_cwMain
        or      apiExcepPatched,-1
        assume ds:_apiCode
        ;
        pop     ds
        db 66h
        retf
PatchExc        endp


;-------------------------------------------------------------------------
;
;Remove exception patchs.
;
UnPatchExc      proc    near
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
;
;Release real mode hardware interupt vectors and call back entries.
;
        cmp     d[OldInt1bh],0
        jz      @@no1b
        mov     dx,w[OldInt1bh]
        mov     cx,w[OldInt1bh+2]
        mov     bl,1bh
        sys     SetRVect
        mov     cx,w[Int1bhcall+2]
        mov     dx,w[Int1bhcall]
        sys     RelCallBack
        mov     edx,d[OldInt1bhP]
        mov     cx,w[OldInt1bhP+4]
        mov     bl,1bh
        sys     SetVect
;
@@no1b: cmp     d[OldInt23],0
        jz      @@i0
        test    apiSystemFlags,1
        jz      @@iUse32
        mov     dx,w[OldInt23]
        mov     cx,w[OldInt23+2]
        jmp     @@iDone3216
@@iUse32:       mov     edx,d[OldInt23]
        mov     cx,w[OldInt23+4]
@@iDone3216:    mov     bl,23h
        sys     SetVect
@@i0:   ;
        cmp     d[OldInt24],0
        jz      @@i1
        test    apiSystemFlags,1
        jz      @@i24Use32
        mov     dx,w[OldInt24]
        mov     cx,w[OldInt24+2]
        jmp     @@i24Done3216
@@i24Use32:     mov     edx,d[OldInt24]
        mov     cx,w[OldInt24+4]
@@i24Done3216:  mov     bl,24h
        sys     SetVect
@@i1:   ;
        pop     ds
        db 66h
        retf
UnPatchExc      endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CtrlBrkEvent    proc    far
        push    ax
        push    ds
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:_apiCode
        pop     ds
        jz      @@start32
        ;
        mov     ax,[si]         ;get stacked offset.
        mov     es:Real_IP[di],ax
        mov     ax,2[si]
        mov     es:Real_CS[di],ax
        mov     ax,4[si]
        mov     es:Real_Flags[di],ax
        add     es:Real_SP[di],6
        jmp     @@start0
        ;
@@start32:
;       mov     ax,[esi]                ;get stacked offset.
        mov     ax,[si]         ;get stacked offset. MED 01/24/96

        mov     es:Real_IP[edi],ax
;       mov     ax,2[esi]
        mov     ax,2[si]         ; MED 01/24/96

        mov     es:Real_CS[edi],ax
;       mov     ax,4[esi]
        mov     ax,4[si]        ; MED 01/24/96

        mov     es:Real_Flags[edi],ax
        add     es:Real_SP[edi],6
        ;
@@start0:       pop     ax
        ;
        ;Give protected mode handler a shout.
        ;
        int     1bh
        ;
        ;Go back to caller.
        ;
        assume ds:nothing
        test    cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      @@Use32Bit2
        iret
@@Use32Bit2:    ;
        iretd
;
OldInt1bh       dd 0
Int1bhcall      dd 0
CtrlBrkEventTab db size RealRegsStruc dup (0)
CtrlBrkEvent    endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Base handler for protected mode int 1bh's.
;
Int1bHandler    proc    far
        push    es
        pushad
        assume ds:nothing
        ;
        ;Need to chain to origional real mode handler.
        ;
        mov     es,cs:apiDDSeg
        mov     edi,offset Int1bRegs
        mov     eax,cs:d[OldInt1bh]
        mov     es:Real_IP[edi],ax
        shr     eax,16
        mov     es:Real_CS[edi],ax
        mov     es:Real_SS[edi],0
        mov     es:Real_SP[edi],0
        mov     ax,0302h
        xor     ecx,ecx
        xor     ebx,ebx
        int     31h
        ;
        ;Return to caller.
        ;
        popad
        pop     es
        test    cs:apiSystemFlags,1
        jz      @@0
        iret
@@0:    iretd
        assume ds:_apiCode
;
OldInt1BhP      df 0
Int1bRegs       db size RealRegsStruc dup (0)
Int1bHandler    endp


;-------------------------------------------------------------------------
;
;Handle a ctrl-break key press by terminating this application cleanly.
;
Int23Handler    proc    near
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        pushm   ds,ds,ds
        popm    es,fs,gs
        sti
        jmp     f[TerminationHandler]
        assume ds:_apiCode
OldInt23        df 0
Int23Handler    endp


;-------------------------------------------------------------------------
Int24Handler    proc    near
        pushm   ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pushm   ds,ds,ds
        popm    es,fs,gs
        ;
@@0:    mov     edx,offset CriticalPrompt
        mov     ah,9
        int     21h
        mov     ah,1
        int     21h
        mov     edi,offset CriticalKeys
        mov     ecx,8
        cld
        push    ds
        pop     es
        repne   scasb
        jnz     @@0
        movzx   eax,b[edi+7]
        cmp     eax,2
        jz      @@Terminate
        popm    ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        assume ds:nothing
        test    cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      @@Use32_2
        iret
@@Use32_2:      ;
        iretd
        ;
@@Terminate:    assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        pushm   ds,ds,ds
        popm    es,fs,gs
        sti
        jmp     f[TerminationHandler]
        assume ds:_apiCode
;
OldInt24        df 0
CriticalPrompt  db 13,10,'Critical Error: Abort, Retry, Ignore, Fail? $'
CriticalKeys    db 'aArRiIfF'
CriticalCodes   db 2,2,1,1,0,0,3,3
Int24Handler    endp


; remembered to defang this ancient artifact, MED, 11/01/99
;-------------------------------------------------------------------------
DPMICopyCheck   proc    near
COMMENT !
        pushf
        pushm   ax,ds,si
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        lea     si,Copyright
        add     si,CopyCount
        cmp     b[si],0
        jnz     @@0
        sub     si,CopyCount
        mov     CopyCount,0
@@0:    inc     CopyCount
        mov     al,b[CopyCheck+si]
        cmp     al,255
        jz      @@1
        add     al,44
        cmp     al,b[si]
        jz      @@1
        sub     DPMIStackOff,32
        ;
@@1:    popm    ax,ds,si
        popf
        assume ds:_apiCode
END COMMENT !
        ret
DPMICopyCheck   endp


;-------------------------------------------------------------------------
Borda   proc    near
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
Borda   endp


;-------------------------------------------------------------------------
;
apiSystemFlags  dw 0,0
DescriptorBuffer db 8 dup (?)
apiDSeg dw ?
apiDDSeg        dw ?
;
apiNewHeader    NewHeaderStruc <>       ;make space for a header.
;
apiExeSignature db ?    ;00 Identifier text.
        db ?    ;01 /
apiExeLength    dw ?    ;02 Length of file MOD 512
        dw ?    ;04 Length of file in 512 byte blocks.
apiExeRelocNum  dw ?    ;06 Number of relocation items.
apiExeHeaderSize        dw ?    ;08 Length of header in 16 byte paragraphs.
apiExeMinAlloc  dw ?    ;0A Minimum number of para's needed above program.
apiExeMaxAlloc  dw ?    ;0C Maximum number of para's needed above program.
apiExeStackSeg  dw ?    ;0E Segment displacement of stack module.
apiExeEntrySP   dw ?    ;10 value for SP at entry.
apiExeCheckSum  dw ?    ;12 Check sum...
apiExeEntryIP   dw ?    ;14 Contents of IP at entry.
apiExeEntryCS   dw ?    ;16 Segment displacement of CS at entry.
apiExeRelocFirst        dw ?    ;18 First relocation item offset.
apiExeOverlayNum        db ?    ;1A Overlay number.
;
LastResource    dd 0,0
;
PatchTable      dd 16 dup (0)
;
        include loadle\loadle.asm
        include decode_c.asm

mcbLastChunk    dd 0

ExecMCount      dd 0

;
;Export table for base PSP. EXPORT's internal functions.
;
apiEXPORT_cwMainCode equ 0
apiEXPORT_cwMainData equ 1
apiEXPORT_apiCode equ 2
apiEXPORT_apiData equ 3


apiExports      dd 10
;
        dd apiExportName
;
        dd apiExport01
        dd apiExport02
        dd apiExport03
        dd apiExport04
        dd apiExport05
        dd apiExport06
        dd apiExport07
        dd apiExport08
        dd apiExport09
        dd apiExport10
;
apiExportName   db 15,"CAUSEWAY_KERNAL"
;
apiExport01     dd __cwAPI_CreatePSP
        dw apiEXPORT_apiCode
        db 17,"__CWAPI_CREATEPSP"
;
apiExport02     dd __cwAPI_DeletePSP
        dw apiEXPORT_apiCode
        db 17,"__CWAPI_DELETEPSP"
;
apiExport03     dd __cwAPI_ExecModule
        dw apiEXPORT_apiCode
        db 18,"__CWAPI_EXECMODULE"
;
apiExport04     dd __cwAPI_DSizeSelector
        dw apiEXPORT_apiCode
        db 21,"__CWAPI_DSIZESELECTOR"
;
apiExport05     dd __cwAPI_FindModule
        dw apiEXPORT_apiCode
        db 18,"__CWAPI_FINDMODULE"
;
apiExport06     dd __cwAPI_UnFindModule
        dw apiEXPORT_apiCode
        db 20,"__CWAPI_UNFINDMODULE"
;
apiExport07     dd __cwAPI_FindFunction
        dw apiEXPORT_apiCode
        db 20,"__CWAPI_FINDFUNCTION"
;
apiExport08     dd __cwAPI_GetMemLinear32
        dw apiEXPORT_apiCode
        db 22,"__CWAPI_GETMEMLINEAR32"
;
apiExport09     dd __cwAPI_ResMemLinear32
        dw apiEXPORT_apiCode
        db 22,"__CWAPI_RESMEMLINEAR32"
;
apiExport10     dd __cwAPI_RelMemLinear32
        dw apiEXPORT_apiCode
        db 22,"__CWAPI_RELMEMLINEAR32"

__cwAPI_CreatePSP proc far
        call    CreatePSP
        ret
__cwAPI_CreatePSP endp

__cwAPI_DeletePSP proc far
        call    DeletePSP
        ret
__cwAPI_DeletePSP endp

__cwAPI_ExecModule proc far
        call    ExecModule
        ret
__cwAPI_ExecModule endp

__cwAPI_DSizeSelector proc far
        call    _DSizeSelector
        ret
__cwAPI_DSizeSelector endp

__cwAPI_FindModule proc far
        call    FindModule
        ret
__cwAPI_FindModule endp

__cwAPI_UnFindModule proc far
        call    UnFindModule
        ret
__cwAPI_UnFindModule endp

__cwAPI_FindFunction proc far
        call    FindFunction
        ret
__cwAPI_FindFunction endp

__cwAPI_GetMemLinear32 proc far
        call    mcbGetMemLinear32
        ret
__cwAPI_GetMemLinear32 endp

__cwAPI_ResMemLinear32 proc far
        call    mcbResMemLinear32
        ret
__cwAPI_ResMemLinear32 endp

__cwAPI_RelMemLinear32 proc far
        call    mcbRelMemLinear32
        ret
__cwAPI_RelMemLinear32 endp


        db 4 dup (?)
_apiCodeEnd     label byte
_apiCode        ends

        .286

