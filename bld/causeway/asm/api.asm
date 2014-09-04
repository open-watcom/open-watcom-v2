        .386P

_apiCode        segment para 'API code' use32
        assume cs:_apiCode, ds:_apiCode
_apiCodeStart   label byte



;-------------------------------------------------------------------------
;
apiSystemFlags  dw 0,0
DescriptorBuffer db 8 dup (?)
apiDSeg dw ?
apiDDSeg        dw ?
;
apiNewHeader    NewHeaderStruc <>       ;make space for a header.
;
LastResource    dd 0,0
;
PatchTable      dd 16 dup (0)

mcbLastChunk    dd 0
ExecMCount      dd 0

nullCmdl        db 0

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
        test    BYTE PTR cs:apiSystemFlags,1
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
        test    BYTE PTR cs:apiSystemFlags,1
        jz      __0
        pushf
        db 66h
        call    FWORD PTR cs:[OldIntSys]                ;pass it onto previous handler.
        jmp     __1
__0:    pushfd
        call    FWORD PTR cs:[OldIntSys]                ;pass it onto previous handler.
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
        push    ds
        push    es
        push    fs
        push    gs
        pushad
        mov     ebp,esp                         ;Make registers addressable.
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
        jnz     api1_Nope
;
;Check if this is a CauseWay function.
;
        cmp     ah,255
        jz      api1_Ours
;
;Scan the table of other relavent functions.
;
        mov     edi,offset apiExtraCallTable+2
api1_extra:
        cmp     WORD PTR cs:[edi-2],0
        jz      api1_Nope
        cmp     ax,WORD PTR cs:[edi-2]
        jz      api1_GotCall
        add     edi,4+2
        jmp     api1_extra
;
;An internal funtion? so point to table entry.
;
api1_Ours:
        movzx   edi,al
        shl     edi,2
        add     edi,offset apiCallTable
;
;Check if final call is still going to pass control to the old handler.
;
api1_GotCall:
        cmp     DWORD PTR cs:[edi],0
        jz      api1_Nope
;
;See about turning interrupts back on.
;
        mov     esi,Int_Flags32
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api1_32bit0
        movzx   ebp,bp
        mov     esi,Int_Flags16
api1_32Bit0:
        and     BYTE PTR ss:[ebp+esi],not 1     ;clear carry.
        cld                                     ;Default direction.
        test    WORD PTR ss:[ebp+esi],1 shl 9   ;Were interrupts enabled?
        jz      api1_NoInts
        sti                                     ;Turn interrupts back on.
;
;Call the function handler.
;
api1_NoInts:
        assume ds:nothing
        call    DWORD PTR cs:[edi]              ;Pass control to handler.
        assume ds:_apiCode
        popad
        pop     gs
        pop     fs
        pop     es
        pop     ds
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api1_32Bit1
        iret
api1_32Bit1:
        iretd                                   ;Return to caller.
;
;Not an internal function or anything we want to interfere with so pass control
;to original handler.
;
api1_Nope:
        popad                                   ;Restore registers.
        pop     gs
        pop     fs
        pop     es
        pop     ds
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        jz      api1_n32
        db 66h
api1_n32:
        jmp     FWORD PTR cs:[OldIntSys]        ;pass it onto previous handler.
        assume ds:_apiCode
;
OldIntSys       df 0
;
APICallTable    dd cwAPI_Info       ;00
        dd cwAPI_IntXX              ;01
        dd cwAPI_FarCallReal        ;02
        dd cwAPI_GetSel             ;03
        dd cwAPI_RelSel             ;04
        dd cwAPI_CodeSel            ;05
        dd cwAPI_AliasSel           ;06
        dd cwAPI_GetSelDet          ;07
        ;
        dd cwAPI_GetSelDet32        ;08
        dd cwAPI_SetSelDet          ;09
        dd cwAPI_SetSelDet32        ;0A
        dd cwAPI_GetMem             ;0B
        dd cwAPI_GetMem32           ;0C
        dd cwAPI_ResMem             ;0D
        dd cwAPI_ResMem32           ;0E
        dd cwAPI_RelMem             ;0F
        ;
        dd cwAPI_GetMemLinear       ;10
        dd cwAPI_GetMemLinear32     ;11
        dd cwAPI_ResMemLinear       ;12
        dd cwAPI_ResMemLinear32     ;13
        dd cwAPI_RelMemLinear       ;14
        dd cwAPI_RelMemLinear32     ;15
        dd cwAPI_GetMemNear         ;16
        dd cwAPI_ResMemNear         ;17
        ;
        dd cwAPI_RelMemNear         ;18
        dd cwAPI_Linear2Near        ;19
        dd cwAPI_Near2Linear        ;1A
        dd cwAPI_LockMem            ;1B
        dd cwAPI_LockMem32          ;1C
        dd cwAPI_UnLockMem          ;1D
        dd cwAPI_UnLockMem32        ;1E
        dd cwAPI_LockMemNear        ;1F
        ;
        dd cwAPI_UnLockMemNear      ;20
        dd cwAPI_GetMemDOS          ;21
        dd cwAPI_ResMemDOS          ;22
        dd cwAPI_RelMemDOS          ;23
        dd cwAPI_Exec               ;24
        dd cwAPI_GetDOSTrans        ;25
        dd cwAPI_SetDOSTrans        ;26
        dd cwAPI_GetMCBSize         ;27
        ;
        dd cwAPI_SetMCBSize         ;28
        dd cwAPI_GetSels            ;29
        dd cwAPI_cwLoad             ;2A
        dd cwAPI_cwcInfo            ;2B
        dd cwAPI_GetMemSO           ;2C
        dd cwAPI_ResMemSO           ;2D
        dd cwAPI_RelMemSO           ;2E
        dd cwAPI_UserDump           ;2F

;       dd 16 dup (0)               ;30-3F
        dd cwAPI_SetDump            ;30
        dd cwAPI_UserErrTerm        ;31
        dd cwAPI_CWErrName          ;32
        dd 13 dup (0)               ;33-3F

        dd 16 dup (0)               ;40-4F
        dd 16 dup (0)               ;50-5F
        dd 16 dup (0)               ;60-6F
        dd 16 dup (0)               ;70-7F
        ;
        dd 16*7 dup (0)             ;80-EF
        ;
        dd 7 dup (0)                ;F0-F6
        dd cwAPI_DbgNtfModLoad      ;F7
        ;
        dd cwAPI_DbgNtfModUnload    ;F8
        dd cwAPI_ID                 ;F9
        dd cwAPI_GetPatch           ;FA
        dd cwAPI_cwcLoad            ;FB
        dd cwAPI_LinearCheck        ;FC
        dd cwAPI_ExecDebug          ;FD
        dd cwAPI_Cleanup            ;FE
        dd 0                        ;FF
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
        jnc     api2_0
        mov     [ebp+Int_AX],ax
api2_0: ret
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
        jnc     api3_0
        mov     [ebp+Int_AX],ax
api3_0: ret
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
        jc      api4_9
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
api4_9: ret
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
        jc      api5_9
;
        shl     ecx,16
        mov     cx,dx
        mov     edx,ecx
        mov     ax,Res_CALLBACK
        call    ReleaseResource
;
api5_9: ret
dpmiAPI_RelCallBack endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Call user^Aermination routine on exception
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
        mov     UserTermFlag,0          ; initialize flag
        mov     dx,[ebp+Int_DS]
        lsl     ax,dx
        jnz     uetret                  ; invalid selector
        mov     al,[ebp+Int_CL]         ; get bitness flag
        test    al,al
        setne   al                      ; 0 if 0, 1 if nonzero
        inc     ax                      ; 1 if 16-bit, 2 if 32-bit
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
        test    BYTE PTR cs:apiSystemFlags,1
        jz      cen2
        movzx   edx,dx                  ; 16-bit, zero high word of edx

cen2:
        mov     ax,[ebp+Int_CX]
        mov     es,ax
        mov     ebx,OFFSET NewCWErrName ; ds:ebx -> destination, es:edx -> source
        mov     cx,80                   ; don't allow more than 80 chars in file name

cenloop:
        mov     al,es:[edx]
        test    al,al
        je      centerm                 ; at null terminator
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
        jnc     api9_0
        movzx   edi,di
api9_0:
        mov     DebugUserOff,edi
        mov     cx,[ebp+Int_CX]
        mov     DebugUserCount,cx

; check for out of bounds
        movzx   edx,cx
        add     edx,edi
        jc      udfail
        dec     edx                     ; make relative 0
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
        mov     al,[ebp+Int_BH]         ; check fill flag
        test    al,al                   ; no fill, carry flag reset by default
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
        clc                             ; flag success
        jmp     udret

udfail:
        mov     DebugUserCount,0
        stc                             ; flag failure

udret:
        cwAPI_C2C
        ret
cwAPI_UserDump  ENDP


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Notify Debugger of new module loaded.
;
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
cwAPI_DbgNtfModLoad proc    near
        ret
cwAPI_DbgNtfModLoad endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Notify Debugger of module unloading.
;
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
cwAPI_DbgNtfModUnload   proc    near
        ret
cwAPI_DbgNtfModUnload   endp


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
        mov     ax,WORD PTR es:[EPSP_Struc.EPSP_TransReal]
        mov     [ebp+Int_BX],ax
        mov     ax,WORD PTR es:[EPSP_Struc.EPSP_TransProt]
        mov     [ebp+Int_DX],ax
        mov     eax,DWORD PTR es:[EPSP_Struc.EPSP_TransSize]
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
        mov     WORD PTR es:[EPSP_Struc.EPSP_TransReal],bx
        mov     WORD PTR es:[EPSP_Struc.EPSP_TransProt],dx
        cmp     ecx,10000h
        jc      api13_0
        mov     ecx,65535
api13_0:
        mov     DWORD PTR es:[EPSP_Struc.EPSP_TransSize],ecx
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
        mov     dx,WORD PTR es:[EPSP_Struc.EPSP_TransReal]
        mov     ecx,DWORD PTR es:[EPSP_Struc.EPSP_TransSize]
        mov     es,WORD PTR es:[EPSP_Struc.EPSP_TransProt]
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
        jnc     api15_0
        movzx   edi,di
api15_0:
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     api15_NoStack0
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        sub     DPMIStackOff,RawStackDif
        mov     ax,DPMIStackOff
        add     ax,RawStackDif
        mov     es:RealRegsStruc.Real_SP[edi],ax
        mov     ax,DPMIStackSeg
        mov     es:RealRegsStruc.Real_SS[edi],ax

; MED 05/23/96
        test    BYTE PTR cs:apiSystemFlags,1
        jz      medUse32Bit8
        mov     ebx,Int_Flags16
        jmp     medUse16Bit8

medUse32Bit8:
        mov     ebx,Int_Flags32

medUse16Bit8:
        mov     ax,ss:[ebp+ebx]
        and     ax,1111110011111111b                ;clear Trap and INT flag.
        mov     es:RealRegsStruc.Real_Flags[edi],ax ; explicitly set flags on DPMI 300h call

        assume ds:_apiCode
        jmp     api15_DoneStack0
api15_NoStack0:
        mov     es:RealRegsStruc.Real_SP[edi],0     ;use DPMI provided stack.
        mov     es:RealRegsStruc.Real_SS[edi],0
api15_DoneStack0:
        xor     cx,cx                               ;No stack parameters.
        mov     bh,ch                               ;no flags.

        mov     bl,ss:[ebp+Int_BL]

        mov     ax,0300h
        cwAPI_CallOld
        cwAPI_C2C
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     api15_DoneStack1
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        add     DPMIStackOff,RawStackDif
        assume ds:_apiCode
api15_DoneStack1:
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api15_Use32Bit8
        mov     ebx,Int_Flags16
        jmp     api15_Use16Bit8
api15_Use32Bit8:
        mov     ebx,Int_Flags32
api15_Use16Bit8:
        mov     ax,ss:[ebp+ebx]
        and     ax,0000011000000000b                ;retain IF.
        and     es:RealRegsStruc.Real_Flags[edi],1111100111111111b  ;lose IF.
        or      es:RealRegsStruc.Real_Flags[edi],ax
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
        jnc     api16_0
        movzx   edi,di
api16_0:
        pushf
        pop     es:RealRegsStruc.Real_Flags[edi]
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     api16_NoStack1
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        sub     DPMIStackOff,RawStackDif
        mov     ax,DPMIStackOff
        add     ax,RawStackDif
        mov     es:RealRegsStruc.Real_SP[edi],ax
        mov     ax,DPMIStackSeg
        mov     es:RealRegsStruc.Real_SS[edi],ax
        assume ds:_apiCode
        jmp     api16_DoneStack2
api16_NoStack1:
        mov     es:RealRegsStruc.Real_SP[edi],0 ;use DPMI provided stack.
        mov     es:RealRegsStruc.Real_SS[edi],0
api16_DoneStack2:
        xor     cx,cx                           ;No stack parameters.
        xor     bx,bx                           ;no flags.
        mov     ax,0301h
        cwAPI_CallOld
        cwAPI_C2C
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        jnz     api16_DoneStack3
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        add     DPMIStackOff,RawStackDif
        assume ds:_apiCode
api16_DoneStack3:
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api16_Use32Bit8
        mov     ebx,Int_Flags16
        jmp     api16_Use16Bit8
api16_Use32Bit8:
        mov     ebx,Int_Flags32
api16_Use16Bit8:
        mov     ax,ss:[ebp+ebx]
        and     ax,0000011000000000b            ;retain IF.
        and     es:RealRegsStruc.Real_Flags[edi],1111100111111111b  ;lose IF.
        or      es:RealRegsStruc.Real_Flags[edi],ax
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
        jc      api17_9
        mov     [ebp+Int_BX],bx
api17_9:
        ret
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
        jc      api18_9
;
        mov     [ebp+Int_BX],ax
        movzx   edx,ax
        movzx   ecx,cx
        mov     ax,Res_SEL
api18_0:
        call    RegisterResource
        add     edx,8
        dec     ecx
        jnz     api18_0
;
api18_9:
        ret
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
        jc      api19_9
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
api19_9:
        ret
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
        jc      api21_9
;
        mov     [ebp+Int_AX],ax
        push    eax
        push    edx
        movzx   edx,ax
        mov     ax,Res_SEL
        call    RegisterResource
        pop     edx
        pop     eax
;
api21_9:
        ret
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
        jc      api22_9
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
api22_9:
        ret
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
        jc      api23_9
;
        mov     [ebp+Int_EDX],eax
        mov     [ebp+Int_ECX],ebx
;
api23_9:
        ret
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
        jc      api26_2
        sys     GetSel
        jc      api26_0
        mov     edx,esi
        jecxz   api26_3
        dec     ecx             ;limit=length-1
api26_3:
        sys     SetSelDet32
        mov     [ebp+Int_BX],bx
        clc
        jmp     api26_1
api26_0:
        call    mcbRelMemLinear32
        stc
        jmp     api26_1
        ;
api26_2:
        mov     dx,cx
        shr     ecx,16
        mov     ax,[ebp+Int_CX]
        shl     eax,16
        mov     ax,[ebp+Int_DX]
        cmp     eax,-2
        jz      api26_5
        cmp     eax,-1
        jnz     api26_4
api26_5:
        mov     [ebp+Int_CX],cx
        mov     [ebp+Int_DX],dx
api26_4:
        stc
        ;
api26_1:
        cwAPI_C2C
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
        jc      api27_9
        assume ds:nothing
        cmp     cs:mcbLastChunk,0       ;DPMI memory?
        assume ds:_apiCode
        jnz     api27_1
;
;Allocate a selector for this memory.
;
        sys     GetSel
        jc      api27_8
        mov     edx,esi
        jecxz   api27_0
        dec     ecx                     ;limit=length-1
api27_0:
        sys     SetSelDet32
        xor     dx,dx
        jmp     api27_7
;
;Get chunk's selector.
;
api27_1:
        push    es
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
        sub     edx,edi                 ;get blocks offset.
;
api27_7:
        mov     [ebp+Int_SI],bx
        mov     [ebp+Int_DI],dx
        clc
        jmp     api27_9
;
api27_8:
        call    mcbRelMemLinear32
        stc
;
api27_9:
        cwAPI_C2C
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
        jc      api28_2
        sys     GetSel
        jc      api28_0
        mov     edx,esi
        jecxz   api28_3
        dec     ecx                     ;limit=length-1
        sys     SetSelDet32
api28_3:
        mov     [ebp+Int_BX],bx
        clc
        jmp     api28_1
api28_0:
        call    mcbRelMemLinear32
        stc
        jmp     api28_1
        ;
api28_2:
        cmp     d[ebp+Int_ECX],-1
        jz      api28_5
        cmp     d[ebp+Int_ECX],-2
        jnz     api28_4
api28_5:
        mov     [ebp+Int_ECX],ecx
api28_4:
        stc
        ;
api28_1:
        cwAPI_C2C
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
        shl     ecx,16                  ;convert new size to 32-bit.
        mov     cx,dx
        push    ecx
        sys     GetSelDet32             ;Get selector base address.
        mov     esi,edx
        pop     ecx
        jc      api29_9
        call    mcbResMemLinear32       ;re-size the memory.
        jc      api29_9
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
api29_9:
        cwAPI_C2C
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
        jc      api30_9
        mov     esi,edx
        movzx   eax,w[ebp+Int_DI]
        add     esi,eax                 ;get memory blocks address.
        mov     cx,[ebp+Int_CX]
        mov     dx,[ebp+Int_DX]
        shl     ecx,16                  ;convert new size to 32-bit.
        mov     cx,dx
        call    mcbResMemLinear32       ;re-size the memory.
        jc      api30_9
;
;Check new block type.
;
        assume ds:nothing
        cmp     cs:mcbLastChunk,0       ;DPMI memory?
        assume ds:_apiCode
        jnz     api30_1
;
;Update selectors details.
;
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
        xor     dx,dx
        jmp     api30_7
;
;Get chunk's selector.
;
api30_1:
        push    es
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
        sub     edx,edi                 ;get blocks offset.
;
;Check if the old block had it's own selector.
;
        cmp     w[ebp+Int_DI],0         ;offset of zero?
        jnz     api30_7
        push    bx
        mov     bx,w[ebp+Int_SI]
        sys     RelSel                  ;release the selector.
        pop     bx
;
api30_7:
        mov     [ebp+Int_SI],bx
        mov     [ebp+Int_DI],dx
        clc
;
api30_9:
        cwAPI_C2C
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
        jc      api31_9
        call    mcbResMemLinear32       ;re-size the memory.
        jc      api31_9
        mov     edx,esi
        dec     ecx
        sys     SetSelDet32
api31_9:
        cwAPI_C2C
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
        jc      api32_9
        mov     esi,edx
        mov     ds,[ebp+Int_DS]
        mov     es,[ebp+Int_ES]
        mov     fs,[ebp+Int_FS]
        mov     gs,[ebp+Int_GS]
        sys     RelSel                  ;release the selector.
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
        call    mcbRelMemLinear32       ;release the memory.
api32_9:
        cwAPI_C2C
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
        jz      api33_bad               ; MED 11/18/96, screen out null pointer releases
        xor     cx,cx
        mov     ax,w[ebp+Int_DS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api33_0
        mov     w[ebp+Int_DS],cx
        mov     ds,cx
api33_0:
        mov     ax,w[ebp+Int_ES]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api33_1
        mov     w[ebp+Int_ES],cx
        mov     es,cx
api33_1:
        mov     ax,w[ebp+Int_FS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api33_2
        mov     w[ebp+Int_FS],cx
        mov     fs,cx
api33_2:
        mov     ax,w[ebp+Int_GS]
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api33_3
        mov     w[ebp+Int_GS],cx
        mov     gs,cx
api33_3:
        mov     bx,w[ebp+Int_SI]
        sys     GetSelDet32             ;get selectors details.
        jc      api33_9
        mov     esi,edx
        movzx   eax,w[ebp+Int_DI]
        add     esi,eax                 ;get memory blocks address.
        call    mcbRelMemLinear32       ;release the memory.
;
;Check if block had it's own selector.
;
        cmp     w[ebp+Int_DI],0         ;offset of zero?
        jnz     api33_4
        mov     bx,w[ebp+Int_SI]
        sys     RelSel                  ;release the selector.
;
api33_4:
        clc
;
api33_9:
        cwAPI_C2C
        ret

; MED 11/18/96
api33_bad:
        stc
        jmp     api33_9

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
        jc      api34_9
        mov     di,si
        shr     esi,16
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_DI],di
        clc
api34_9:
        cwAPI_C2C
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
        jc      api35_9
        mov     [ebp+Int_ESI],esi
api35_9:
        ret
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
        jc      api36_9
        mov     di,si
        shr     esi,16
        mov     [ebp+Int_SI],si
        mov     [ebp+Int_DI],di
        clc
api36_9:
        cwAPI_C2C
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
        jc      api37_9
        mov     [ebp+Int_ESI],esi
api37_9:
        cwAPI_C2C
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
        jc      api40_9
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        sub     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        clc
api40_9:
        cwAPI_C2C
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
        add     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
        assume ds:_apiCode
        call    mcbResMemLinear32
        jc      api41_9
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        sub     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
        assume ds:_apiCode
        mov     [ebp+Int_ESI],esi
        clc
api41_9:
        cwAPI_C2C
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
        add     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
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
        sub     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
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
        add     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
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
        jc      api45_9
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
api45_9:
        cwAPI_C2C
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
        jc      api46_9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    RegisterResource
        clc
api46_9:
        cwAPI_C2C
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
        jc      api47_9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
api47_9:
        cwAPI_C2C
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
        jc      api48_9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
api48_9:
        cwAPI_C2C
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
        add     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
        assume ds:_apiCode
        xchg    esi,ebx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16
        mov     ax,0600h
        cwAPI_CallOld
        jc      api49_9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    RegisterResource
        clc
api49_9:
        cwAPI_C2C
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
        add     esi,DWORD PTR ds:[EPSP_Struc.EPSP_NearBase]
        assume ds:_apiCode
        xchg    esi,ebx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16
        mov     ax,0601h
        cwAPI_CallOld
        jc      api50_9
        mov     dx,bx
        shl     edx,16
        mov     dx,cx
        mov     cx,si
        shl     ecx,16
        mov     cx,di
        mov     ax,Res_LOCK
        call    ReleaseResource
        clc
api50_9:
        cwAPI_C2C
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
        jnc     api51_0
        mov     [ebp+Int_BX],bx
        jmp     api51_9
;
api51_0:
        mov     [ebp+Int_DX],dx
        movzx   edx,dx
        movzx   ecx,bx
        shl     ecx,4
        mov     ax,Res_DOSMEM
        call    RegisterResource
;
api51_9:
        ret
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
        jnc     api52_0
        mov     [ebp+Int_AX],ax
        mov     [ebp+Int_BX],bx
        jmp     api52_9
;
api52_0:
        mov     ax,Res_DOSMEM
        movzx   edx,dx
        call    ReleaseResource
        movzx   ecx,bx
        shl     ecx,4
        call    RegisterResource
        clc
;
api52_9:
        cwAPI_C2C
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
        jnc     api53_0
        mov     [ebp+Int_AX],ax
        jmp     api53_9
;
api53_0:
        mov     ax,Res_DOSMEM
        movzx   edx,dx
        call    ReleaseResource
        clc
;
api53_9:
        cwAPI_C2C

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
        xor     ebx,ebx
        push    ebp
        call    _Exec
        pop     ebp
        jnc     api54_0
        mov     [ebp+Int_AX],ax
        jmp     api54_9
;
api54_0:
        mov     [ebp+Int_AL],al
;
api54_9:
        cwAPI_C2C
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
        jc      api56_9
        mov     [ebp+Int_ECX],ecx
api56_9:
        cwAPI_C2C
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
        jc      api57_3
        mov     LinearAddressCheck,1
        mov     al,es:[esi]
        cmp     LinearAddressCheck,0
        mov     LinearAddressCheck,0
        jnz     api57_2
api57_3:
        stc
        jmp     api57_1
api57_2:
        clc
api57_1:
        assume ds:_apiCode
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
        jnc     api58_0
        pop     ebp
        mov     [ebp+Int_AX],ax
        jmp     api58_9
;
api58_0:
        shl     esi,16
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
api58_9:
        cwAPI_C2C
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
        jnc     api59_0
        mov     [ebp+Int_AX],ax
        jmp     api59_9
;
api59_0:
        mov     [ebp+Int_CX],cx
        mov     [ebp+Int_EDX],edx
        mov     [ebp+Int_BX],bx
        mov     [ebp+Int_EAX],eax
        mov     [ebp+Int_SI],si
;
api59_9:
        cwAPI_C2C
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
        ;
        push    eax
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        cmp     ebx,100000h
        jc      api61_ok
        cmp     ebx,-1
        jz      api61_ok
        add     ebx,4095
        and     ebx,0FFFFFFFFh-4095
        dec     ebx
        ;
api61_ok:
        mov     d[api61_SelectorBase],eax
        mov     d[api61_SelectorSize],ebx
        ;
        ;Get a new selector from DPMI.
        ;
        mov     ax,0000h
        mov     cx,1
        cwAPI_CallOld
        jc      api61_9
        mov     bx,ax
api61_GotSel:
        push    bx
        mov     ecx,d[api61_SelectorSize]
        mov     esi,d[api61_SelectorBase]
        mov     ax,ds                       ;need RPL to base DPL on.
        lar     ax,ax
        and     ah,DescPL3
        or      ah,DescPresent+DescMemory+DescRWData
        ;
        mov     dx,apiSystemFlags           ;use default setting.
        shr     dx,14
        mov     al,dl
        xor     al,1
        or      al,b[apiSystemFlags+2]
        and     al,1
        shl     al,6
        ;
        cmp     ecx,0fffffh                 ; see if we need to set g bit
        jc      api61_3
        jz      api61_3
        shr     ecx,12                      ; div by 4096
        or      al,80h                      ; set g bit
api61_3:
        mov     w[DescriptorBuffer],cx      ;store low word of limit.
        shr     ecx,16
        or      cl,al
        mov     b[DescriptorBuffer+6],cl    ;store high bits of limit and gran/code size bits.
        mov     w[DescriptorBuffer+2],si    ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     b[DescriptorBuffer+4],bl    ;store mid byte of linear base.
        mov     b[DescriptorBuffer+7],bh    ;store high byte of linear base.
        mov     b[DescriptorBuffer+5],ah    ;store pp/dpl/dt/type bits.
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
        jc      api61_9
        ;
        movzx   edx,bx
        mov     ax,Res_SEL
        call    RegisterResource
        ;
api61_8:
        clc
        jmp     api61_10
        ;
api61_9:
        stc
api61_10:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        ret
api61_SelectorBase:
        dd ?
api61_SelectorSize:
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
        ;
        ;Check segment registers for value we're releasing and clear
        ;if found.
        ;
        push    ax
        push    bx
        push    cx
        and     bx,0ffffh-3
        xor     cx,cx
        mov     ax,ds
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api62_0
        mov     ds,cx
api62_0:
        mov     ax,es
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api62_1
        mov     es,cx
api62_1:
        mov     ax,fs
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api62_2
        mov     fs,cx
api62_2:
        mov     ax,gs
        and     ax,0ffffh-3
        cmp     ax,bx
        jnz     api62_3
        mov     gs,cx
api62_3:
        pop     cx
        pop     bx
        pop     ax
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
        jc      api62_9
        ;
        clc
        jmp     api62_10
        ;
api62_9:
        stc
api62_10:
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
api62_11:
        ret
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
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[api63_SelectorBase],eax
        mov     d[api63_SelectorSize],ebx
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
        xor     al,al
        mov     ecx,d[api63_SelectorSize]
        mov     esi,d[api63_SelectorBase]
        cmp     ecx,0fffffh                     ; see if we need to set g bit
        jc      api63_2
        jz      api63_2
        shr     ecx,12                          ; div by 4096
        or      al,80h                          ; set g bit
api63_2:
        mov     w[DescriptorBuffer+0],cx        ;store low word of limit.
        shr     ecx,16
        or      cl,al
        and     b[DescriptorBuffer+6],01110000b ;lose limit & G bit.
        or      b[DescriptorBuffer+6],cl        ;store high bits of limit and G bit.
        mov     w[DescriptorBuffer+2],si        ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     b[DescriptorBuffer+4],bl        ;store mid byte of linear base.
        mov     b[DescriptorBuffer+7],bh        ;store high byte of linear base.
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
        jc      api63_9
        ;
        clc
        jmp     api63_10
        ;
api63_9:
        stc
api63_10:
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
api63_SelectorBase:
        dd ?
api63_SelectorSize:
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
        ;
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
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
        jc      api64_9
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
        jz      api64_0
        shl     ebx,12
        or      ebx,0FFFh
api64_0:
        clc
        jmp     api64_10
api64_9:
        stc
api64_10:
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
_DetSelector    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_CodeSelector   proc near
        ;
        pushad
        call    _DSizeSelector
        popad
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
        and     DescriptorBuffer+6,255-(1 shl 6)    ;clear code size bit.
        and     cl,1
        shl     cl,6
        or      DescriptorBuffer+6,cl               ;code size bit.
        ;
        mov     ax,000ch
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        clc
        jmp     api65_10
        ;
api65_9:
        stc
api65_10:
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
api65_11:
        ret
_CodeSelector   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_DSizeSelector  proc near
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
        and     DescriptorBuffer+6,255-(1 shl 6)    ;clear code size bit.
        and     cl,1
        shl     cl,6
        or      DescriptorBuffer+6,cl               ;code size bit.
        ;
        mov     ax,000ch
        push    ds
        pop     es
        mov     edi,offset DescriptorBuffer
        cwAPI_CallOld
        ;
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
_DSizeSelector  endp


mcbChunkLast    equ     0       ;pointer to previous chunk in chain.
mcbChunkNext    equ     4       ;pointer to next chunk in chain.
mcbBiggest      equ     8       ;biggest free block in this chunk.
mcbChunkSize    equ     10      ;size of this chunk.
mcbChunkSel     equ     12      ;chunk's selector.
mcbChunkLen     equ     16      ;length of chunk control entry.

mcbID           equ     0       ;ID for corruption checking.
mcbLast         equ     1       ;previous MCB pointer status.
mcbLastSize     equ     2       ;pointer to previous MCB.
mcbFreeUsed     equ     4       ;free or used status.
mcbNext         equ     5       ;next MCB pointer status.
mcbNextSize     equ     6       ;pointer to next MCB.
mcbLen          equ     8       ;length of an MCB entry.


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
        mov     ecx,DWORD PTR ds:[EPSP_Struc.EPSP_mcbMaxAlloc]
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
        jc      api68_1
        stc
        jmp     api68_9
        ;
api68_1:
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,PSPSegment
        cmp     ecx,mcbChunkLen+mcbLen
        jc      api68_9
        sub     ecx,mcbChunkLen+mcbLen
api68_0:
        mov     DWORD PTR ds:[EPSP_Struc.EPSP_mcbMaxAlloc],ecx
        assume ds:_apiCode
        clc
;
api68_9:
        cwAPI_C2C
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
        jz      api69_GetMax
        cmp     ecx,-2
        jz      api69_GetMax
        add     ecx,3
        and     ecx,not 3
;
;Check MCB allocation system is enabled.
;
        cmp     mcbAllocations,0
        jz      api69_GetMax
;
;Check block size is small enough for these functions.
;
        cmp     ecx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        jc      mGML32_0
;
;Size is above threshold so use normal API service for this request.
;
api69_GetMax:
        mov     ebx,ecx
        call    _GetMemory
        jnc     mGML32_9
        cmp     ecx,-2
        jz      api69_RetMax
        cmp     ecx,-1
        stc
        jnz     mGML32_9
api69_RetMax:
        pop     ebp
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
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead],0
        jnz     mGML32_10
        mov     ebx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        add     ebx,mcbChunkLen+mcbLen              ;chunk size.
        call    _GetMemory                          ;allocate it.
        jc      mGML32_9                            ;oops, appear to be out of memory.
        mov     DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead],esi
;
;Initialise this chunk.
;
        mov     es,RealSegment
        sub     ebx,mcbChunkLen+mcbLen              ;chunk size.
        mov     WORD PTR es:[esi+mcbChunkSize],bx
        mov     DWORD PTR es:[esi+mcbChunkNext],0   ;set forward link address.
        mov     DWORD PTR es:[esi+mcbChunkLast],0   ;store back link address.
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        mov     WORD PTR es:[esi+mcbBiggest],ax     ;set biggest chunk size.
        sys     GetSel
        jc      mGML32_9
        mov     WORD PTR es:[esi+mcbChunkSel],bx
        push    edx
        push    ecx
        mov     edx,esi
        mov     ecx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        add     ecx,mcbChunkLen+mcbLen
        sys     SetSelDet32
        pop     ecx
        pop     edx
        add     esi,mcbChunkLen                     ;skip chunk link info.
        mov     BYTE PTR es:[esi+mcbID],"C"         ;set ID byte.
        mov     BYTE PTR es:[esi+mcbLast],"D"       ;mark it as last block in back link.
        mov     WORD PTR es:[esi+mcbLastSize],0     ;clear back link entry.
        mov     BYTE PTR es:[esi+mcbFreeUsed],"J"   ;mark it as a free block,
        mov     BYTE PTR es:[esi+mcbNext],"D"       ;last block in MCB chain,
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        mov     WORD PTR es:[esi+mcbNextSize],ax
;
;Scan through all mcb's in all chunks looking for a big enough block.
;
mGML32_10:
        mov     es,RealSegment                      ;0-4G selector.
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead]  ;start of local allocation trail.
mGML32_1:
        mov     edi,esi                             ;keep a copy for chunk chaining.
        mov     ebp,edi                             ;keep a copy for mcbBiggest
        ;
        cmp     WORD PTR es:[edi+mcbBiggest],cx     ;check if this chunk has a big
        jc      mGML32_6_0                          ;enough free block.
        add     esi,mcbChunkLen
;
;Find first free and big enough block.
;
mGML32_2:
        cmp     BYTE PTR es:[esi+mcbFreeUsed],"J"   ;Free block?
        jz      mGML32_5
mGML32_6:
        cmp     BYTE PTR es:[esi+mcbNext],"M"       ;Normal block (not end of chain)?
        jz      mGML32_4
;
;Reached the end of the chain for this chunk so we need to move onto the next
;chunk in the chain.
;
mGML32_6_0:
        cmp     DWORD PTR es:[edi+mcbChunkNext],0   ;already have a link?
        jnz     mGML32_3
;
;Need another chunk to put in the chain so try and allocate it via normal
;CauseWay API.
;
        mov     ebx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        add     ebx,mcbChunkLen+mcbLen              ;chunk size.
        call    _GetMemory
        jc      mGML32_9                            ;oops, appear to be out of memory.
;
;Update current chunk with address of new chunk and initialise new chunk.
;
        sub     ebx,mcbChunkLen+mcbLen              ;chunk size.
        mov     WORD PTR es:[esi+mcbChunkSize],bx
        mov     DWORD PTR es:[edi+mcbChunkNext],esi ;store forward link address.
        mov     DWORD PTR es:[esi+mcbChunkLast],edi ;store back link address.
        mov     DWORD PTR es:[esi+mcbChunkNext],0   ;clear new forward link address.
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        mov     WORD PTR es:[esi+mcbBiggest],ax     ;set biggest chunk size.
        sys     GetSel
        jc      mGML32_9
        mov     WORD PTR es:[esi+mcbChunkSel],bx
        push    edx
        push    ecx
        mov     edx,esi
        mov     ecx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        add     ecx,mcbChunkLen+mcbLen
        sys     SetSelDet32
        pop     ecx
        pop     edx
        add     esi,mcbChunkLen                     ;skip chunk link info.
        mov     BYTE PTR es:[esi+mcbID],"C"         ;set ID.
        mov     BYTE PTR es:[esi+mcbLast],"D"       ;mark it as last block in back link.
        mov     WORD PTR es:[esi+mcbLastSize],0     ;clear back link entry.
        mov     BYTE PTR es:[esi+mcbFreeUsed],"J"   ;mark it as a free block,
        mov     BYTE PTR es:[esi+mcbNext],"D"       ;last block in MCB chain,
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        mov     WORD PTR es:[esi+mcbNextSize],ax
;
;Chain to next chunk.
;
mGML32_3:
        mov     esi,DWORD PTR es:[edi+mcbChunkNext] ;pickup forward link address.
        jmp     mGML32_1                            ;scan this MCB chain.
;
;Move to next MCB.
;
mGML32_4:
        movzx   eax,WORD PTR es:[esi+mcbNextSize]   ;get block length.
        add     eax,mcbLen                          ;include size of an MCB.
        add     esi,eax
        jmp     mGML32_2
;
;Check if this block is big enough.
;
mGML32_5:
        cmp     WORD PTR es:[esi+mcbNextSize],cx    ;Big enough block?
        jc      mGML32_6
;
;Found a big enough free block so make use of it.
;
        mov     BYTE PTR es:[esi+mcbFreeUsed],"W"   ;mark it as used.
        movzx   ebx,WORD PTR es:[esi+mcbNextSize]
        sub     ebx,ecx                             ;get spare size.
        cmp     ebx,mcbLen+1                        ;smaller than an MCB?
        jc      mGML32_8
;
;Create a new MCB from whats left over.
;
        sub     bx,mcbLen                           ;MCB comes out of this size.
        mov     es:[esi+mcbNextSize],cx             ;set allocated block's size.
        mov     al,BYTE PTR es:[esi+mcbNext]        ;get next status.
        mov     BYTE PTR es:[esi+mcbNext],"M"       ;make sure its not end of chain now.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen                          ;move to where new MCB will be.
        mov     BYTE PTR es:[edi+mcbID],"C"
        mov     BYTE PTR es:[edi+mcbLast],"M"       ;not last in back link chain.
        mov     es:[edi+mcbLastSize],cx             ;set back link size.
        mov     BYTE PTR es:[edi+mcbFreeUsed],"J"   ;mark as free,
        mov     es:[edi+mcbNext],al                 ;set end of chain status.
        mov     es:[edi+mcbNextSize],bx             ;and new block size.
;
;Check if old block used to be end of chain.
;
        cmp     al,"D"                  ;end of chain?
        jz      mGML32_8
;
;Update back link size of next block.
;
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        add     eax,mcbLen              ;include MCB size.
        add     edi,eax
        mov     es:[edi+mcbLastSize],ax
        sub     WORD PTR es:[edi+mcbLastSize],mcbLen
;
;Time to exit.
;
mGML32_8:
        add     esi,mcbLen              ;skip the MCB.
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
mGML32_9:
        pop     ebp
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
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead]
        or      esi,esi                 ;check mcb's are active.
        jz      mRsML32_8
        ;
mRsML32_0:
        cmp     edi,esi
        jc      mRsML32_1
        movzx   eax,WORD PTR es:[esi+mcbChunkSize]
        add     eax,esi
        add     eax,mcbChunkLen+mcbLen  ;chunk size.
        cmp     edi,eax
        jc      mRsML32_2
mRsML32_1:
        cmp     DWORD PTR es:[esi+mcbChunkNext],0   ;Next link field set?
        jz      mRsML32_8
        mov     esi,DWORD PTR es:[esi+mcbChunkNext]
        jmp     mRsML32_0
;
;In range of a chunk so deal with it here.
;
mRsML32_2:
        xchg    edi,esi
        mov     ebp,edi
;
;Check block size is small enough for these functions.
;
        cmp     ecx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
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
        movzx   ecx,WORD PTR es:[esi+mcbNextSize]
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
;Release original block and return address of new block.
;
        call    mcbRelMemLinear32
        pop     esi
        pop     ecx
        jmp     mRsML32_9
;
;Get block's current size.
;
mRsML32_3:
        sub     esi,mcbLen              ;move back to MCB.
        cmp     cx,es:[esi+mcbNextSize]
        jz      mRsML32_7
        jnc     mRsML32_4               ;extending block.
;
;Block is shrinking so build another MCB at the end of this one.
;
        movzx   ebx,WORD PTR es:[esi+mcbNextSize]
        sub     ebx,ecx                 ;get size differance.
        cmp     ebx,mcbLen+1            ;enough for a new MCB?
        jnc     api70_0
;
;Before we abandon this block size change as too small we should check if the
;next block is free and join the new space onto that if it is.
;
        cmp     BYTE PTR es:[esi+mcbNext],"M"   ;end of the chain?
        jnz     mRsML32_7                       ;yep, can't be another block.
        mov     edi,esi
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        add     eax,mcbLen
        add     edi,eax                         ;point to the next block.
        cmp     BYTE PTR es:[edi+mcbFreeUsed],"J"   ;Free block?
        jnz     mRsML32_7                       ;no, so leave things alone.
        mov     es:[esi+mcbNextSize],cx         ;set new size.
        push    esi
        push    ecx
        mov     eax,ecx
        add     eax,mcbLen
        add     esi,eax
        xchg    esi,edi
        mov     ecx,mcbLen
        push    ds
        push    es
        pop     ds
        rep     movsb
        pop     ds
        sub     edi,mcbLen
        add     es:[edi+mcbNextSize],bx         ;update block size.
        sub     es:[edi+mcbLastSize],bx         ;update last size.
        pop     ecx
        pop     esi
        cmp     BYTE PTR es:[edi+mcbNext],"M"   ;end of chain?
        jnz     mRsML32_7
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        add     edi,eax
        add     edi,mcbLen
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7                       ;leave things as they are.
        ;
api70_0:
        sub     ebx,mcbLen                      ;need space for a new MCB.
        mov     es:[esi+mcbNextSize],cx         ;set new size.
        mov     al,es:[esi+mcbNext]             ;get next status.
        mov     BYTE PTR es:[esi+mcbNext],"M"   ;force not end of chain.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen                      ;move to new MCB.
        mov     BYTE PTR es:[edi+mcbID],"C"     ;set ID.
        mov     BYTE PTR es:[edi+mcbLast],"M"   ;not last in last chain.
        mov     es:[edi+mcbLastSize],cx         ;set last link size.
        mov     BYTE PTR es:[edi+mcbNext],al    ;set next status.
        mov     BYTE PTR es:[edi+mcbFreeUsed],"J"   ;mark it as free.
        mov     es:[edi+mcbNextSize],bx         ;set next link size.
        cmp     al,"D"                          ;end of chain?
        jz      mRsML32_7
;
;Update last link size of next MCB.
;
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        mov     ebx,edi
        add     ebx,eax
        add     ebx,mcbLen                      ;move to next block.
        mov     es:[ebx+mcbLastSize],ax
;
;Check if next block is free and join it to the newly created block if it is,
;
        cmp     BYTE PTR es:[ebx+mcbFreeUsed],"J"   ;Free block?
        jnz     mRsML32_7
;
;Join next block to this one.
;
        movzx   eax,WORD PTR es:[ebx+mcbNextSize]   ;get block's size.
        add     eax,mcbLen                          ;include size of an mcb.
        add     es:[edi+mcbNextSize],ax
        mov     al,es:[ebx+mcbNext]
        mov     es:[edi+mcbNext],al                 ;copy next status.
        cmp     al,"D"                              ;end of chain?
        jz      mRsML32_7
;
;Update next blocks last link size.
;
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        add     edi,eax
        add     edi,mcbLen                          ;move to next block.
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7
;
;Need to expand the block so check out the block above this one.
;
mRsML32_4:
        cmp     BYTE PTR es:[esi+mcbNext],"D"       ;end of the chain?
        jz      mRsML32_6
        mov     edi,esi
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        add     eax,mcbLen                          ;move to next block.
        add     edi,eax
        cmp     BYTE PTR es:[edi+mcbFreeUsed],"J"   ;This block free?
        jnz     mRsML32_6
        mov     ebx,ecx
        sub     bx,WORD PTR es:[esi+mcbNextSize]    ;Get size needed.
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        add     eax,mcbLen
        cmp     eax,ebx                             ;Big enough block?
        jc      mRsML32_6
;
;Next block is big enough, is it big enough to leave a free block behind still?
;
        sub     eax,ebx                             ;Get size differance.
        cmp     eax,mcbLen+1
        jnc     mRsML32_5
;
;Swollow new block whole and update next blocks last link entry.
;
        movzx   eax,WORD PTR es:[edi+mcbNextSize]
        add     eax,mcbLen
        add     es:[esi+mcbNextSize],ax             ;update block size.
        mov     al,es:[edi+mcbNext]                 ;get next status.
        mov     es:[esi+mcbNext],al
        cmp     al,"D"                              ;end of the chain?
        jz      mRsML32_7
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        mov     edi,esi
        add     edi,eax
        add     edi,mcbLen
        mov     es:[edi+mcbLastSize],ax
        jmp     mRsML32_7
;
;Create a new MCB in the right place.
;
mRsML32_5:
        mov     al,es:[edi+mcbNext]                 ;Get next status.
        movzx   ebx,WORD PTR es:[edi+mcbNextSize]   ;Get size of this block.
        add     bx,WORD PTR es:[esi+mcbNextSize]
        mov     es:[esi+mcbNextSize],cx             ;set new size of this block.
        sub     ebx,ecx                             ;get size remaining.
        mov     edi,esi
        add     edi,ecx
        add     edi,mcbLen
        mov     BYTE PTR es:[edi+mcbID],"C"         ;set ID.
        mov     BYTE PTR es:[edi+mcbLast],"M"       ;not last in last chain.
        mov     es:[edi+mcbLastSize],cx             ;last link size.
        mov     BYTE PTR es:[edi+mcbFreeUsed],"J"   ;mark it as free again.
        mov     es:[edi+mcbNext],al                 ;set next status.
        mov     es:[edi+mcbNextSize],bx             ;set next link size.
        cmp     al,"D"                              ;end of the chain?
        jz      mRsML32_7
        add     edi,ebx
        add     edi,mcbLen                          ;move to next block.
        mov     es:[edi+mcbLastSize],bx
        jmp     mRsML32_7
;
;Have to try and allocate another block and copy the current blocks contents.
;
mRsML32_6:
        add     esi,mcbLen
        mov     edi,esi
        call    mcbGetMemLinear32
        jc      mRsML32_9
        push    ecx
        push    esi
        push    edi
        xchg    esi,edi
        sub     esi,mcbLen
        movzx   ecx,WORD PTR es:[esi+mcbNextSize]
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
        call    mcbRelMemLinear32                   ;release original block.
        pop     esi
        pop     ecx
        clc
        jmp     mRsML32_9
;
;Setup block address to return and exit.
;
mRsML32_7:
        add     esi,mcbLen
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
mRsML32_8:
        cmp     ecx,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        jc      mRsML32_11
mRsML32_30:
        mov     esi,edi
        mov     ebx,ecx
        call    _ResMemory
        jmp     mRsML32_9
;
;Convert normal API block to local MCB block. This assumes that a none MCB block
;comeing through here is bigger than mcbMaxAlloc and therefore is shrinking.
;
mRsML32_11:
        call    mcbGetMemLinear32
        jc      mRsML32_9
;
;Copy original block contents.
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
        call    _RelMemory                  ;release original block.
        pop     esi
;
;Return to caller.
;
mRsML32_9:
        pop     ebp
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
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead]
        or      esi,esi                             ;check mcb's are active.
        jz      mRML32_8
        ;
mRML32_0:
        cmp     edi,esi
        jc      mRML32_1
        movzx   eax,WORD PTR es:[esi+mcbChunkSize]
        add     eax,esi
        add     eax,mcbChunkLen+mcbLen              ;chunk size.
        cmp     edi,eax
        jc      mRML32_2
mRML32_1:
        cmp     DWORD PTR es:[esi+mcbChunkNext],0   ;Next link field set?
        jz      mRML32_8
        mov     esi,DWORD PTR es:[esi+mcbChunkNext]
        jmp     mRML32_0
;
;In range of a chunk so deal with it here.
;
mRML32_2:
        xchg    edi,esi
        mov     ebp,edi
        sub     esi,mcbLen
        mov     BYTE PTR es:[esi+mcbFreeUsed],"J"   ;mark it as free.
;
;Check if next block is free and join it to this one if it is.
;
mRML32_11:
        cmp     BYTE PTR es:[esi+mcbNext],"D"       ;last block in chain?
        jz      mRML32_3
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        mov     ebx,esi
        add     eax,mcbLen
        add     ebx,eax
        cmp     BYTE PTR es:[ebx+mcbFreeUsed],"J"   ;free block?
        jnz     mRML32_3
        movzx   eax,WORD PTR es:[ebx+mcbNextSize]
        add     eax,mcbLen
        add     es:[esi+mcbNextSize],ax             ;update block size.
        mov     al,es:[ebx+mcbNext]
        mov     es:[esi+mcbNext],al                 ;copy next status.
        cmp     al,"D"                              ;last block in chain?
        jz      mRML32_3
;
;Update next blocks last block pointer.
;
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        add     eax,mcbLen
        add     esi,eax
        mov     es:[esi+mcbLastSize],ax
        sub     WORD PTR es:[esi+mcbLastSize],mcbLen
        sub     esi,eax
;
;Check if current block is end of next chain and end of last chain. If it
;is we can release this chunk because it isn't being used anymore.
;
mRML32_3:
        cmp     BYTE PTR es:[esi+mcbNext],"M"
        jz      mRML32_4
        cmp     BYTE PTR es:[esi+mcbLast],"M"
        jz      mRML32_4
;
;Un-link and release this chunk, its not being used.
;
        mov     esi,edi                             ;Get chunk address.
        mov     edi,es:[esi+mcbChunkLast]
        mov     eax,es:[esi+mcbChunkNext]
        or      edi,edi                             ;First chunk?
        jnz     mRML32_5
        mov     DWORD PTR fs:[EPSP_Struc.EPSP_mcbHead],eax  ;Set new head chunk, 0 is valid.
        jmp     mRML32_6
mRML32_5:
        mov     es:[edi+mcbChunkNext],eax           ;link to next chunk.
mRML32_6:
        xchg    eax,edi
        or      edi,edi                             ;is there a next chunk?
        jz      mRML32_7
        mov     es:[edi+mcbChunkLast],eax           ;link to last chunk.
mRML32_7:
        mov     bx,es:[esi+mcbChunkSel]
        sys     RelSel                              ;release this selector.
        call    _RelMemory                          ;release this memory for real.
        jmp     mRML32_9                            ;exit, we're all done.
;
;Check if previous block is free and join this one to it if it is.
;
mRML32_4:
        cmp     BYTE PTR es:[esi+mcbLast],"M"       ;last block in last chain?
        jnz     mRML32_10
;
;Move back to previous block, see if its free and let the next block join code
;deal with it if it is.
;
        movzx   eax,WORD PTR es:[esi+mcbLastSize]
        add     eax,mcbLen
        sub     esi,eax
        cmp     BYTE PTR es:[esi+mcbFreeUsed],"J"
        jz      mRML32_11
;
;Finished but we couldn't release this chunk.
;
mRML32_10:
        call    mcbSetBiggest
        clc
        jmp     mRML32_9
;
;Not in range of any local chunks so pass it to CauseWay API function.
;
mRML32_8:
        mov     esi,edi
        call    _RelMemory
        ;
mRML32_9:
        popad
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
        push    eax
        push    ecx
        push    esi
        mov     esi,ebp
        add     esi,mcbChunkLen
        xor     ecx,ecx
api72_0:
        cmp     BYTE PTR es:[esi+mcbFreeUsed],"J"
        jnz     api72_1
        cmp     cx,WORD PTR es:[esi+mcbNextSize]
        jnc     api72_1
        mov     cx,WORD PTR es:[esi+mcbNextSize]
api72_1:
        cmp     BYTE PTR es:[esi+mcbNext],"M"   ;last block in last chain?
        jnz     api72_2
        movzx   eax,WORD PTR es:[esi+mcbNextSize]
        add     esi,eax
        add     esi,mcbLen
        jmp     api72_0
api72_2:
        mov     WORD PTR es:[ebp+mcbBiggest],cx
        pop     esi
        pop     ecx
        pop     eax
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
        ;
        push    ecx
        push    edx
        push    edi
        push    ebp
        push    ds
        push    es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    d[api73_BlockSize]
        push    d[api73_BlockBase]
        push    d[api73_BlockHandle]
        ;
        cmp     ebx,-2
        jz      api73_Special
        cmp     ebx,-1                  ;special value to get memory free?
        jnz     api73_NotSpecial
api73_Special:
        call    _GetMemoryMax           ;call free memory code.
        stc
        pop     d[api73_BlockHandle]
        pop     d[api73_BlockBase]
        pop     d[api73_BlockSize]
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     edx
        pop     ecx
        ret
        ;
api73_NotSpecial:
        mov     d[api73_BlockSize],ebx  ;store real block size.
        ;
        mov     cx,bx
        shr     ebx,16
        mov     ax,0501h                ;Allocate memory block.
        cwAPI_CallOld
        jc      api73_9
        shl     ebx,16
        mov     bx,cx
        mov     d[api73_BlockBase],ebx  ;store linear base address.
        shl     esi,16
        mov     si,di
        mov     d[api73_BlockHandle],esi    ;store access handle.
        ;
        mov     ax,Res_MEM
        mov     edx,ebx
        mov     ecx,esi
        mov     ebx,d[api73_BlockSize]
        call    RegisterResource
        ;
        mov     esi,edx
        clc
        jmp     api73_10
        ;
api73_9:
        stc
api73_10:
        pop     d[api73_BlockHandle]
        pop     d[api73_BlockBase]
        pop     d[api73_BlockSize]
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     edx
        pop     ecx
        ret
api73_BlockBase:
        dd ?
api73_BlockHandle:
        dd ?
api73_BlockSize:
        dd ?
_GetMemory      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
_GetMemoryMax proc near
;
;Work out biggest memory block remaining.
;
        ;
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;

COMMENT ! MED 02/15/96
        cmp     ebx,-1                  ;normal max reporting?
        jz      api74_normal

        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        cmp     ProtectedType,2
        assume ds:_apiCode
        pop     ds
        jz      api74_normal

        ;
        ;Get free disk space remaining.
        ;
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw

        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        xor     edx,edx
        cmp     VMMHandle,0
        jz      api74_500_1
        mov     dl,VMMName              ;get drive letter for this media.
        sub     dl,'A'                  ;make it real.
        inc     dl                      ;adjust for current type select.
        mov     ah,36h                  ;get free space.
        int     21h
        xor     edx,edx
        cmp     ax,-1                   ;invalid drive?
        jz      api74_500_1
        mul     cx                      ;Get bytes per cluster.
        mul     bx                      ;Get bytes available.
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
        jmp     api74_500_2
        ;
api74_500_1:
        mov     edx,TotalPhysical
api74_500_2:
        add     edx,FreePages
        ;
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax

        assume ds:_apiCode
        pop     ds
        mov     ebx,edx
        shl     ebx,12
        jmp     api74_exit
END COMMENT !

api74_normal:
        push    es
        mov     edi,offset api74_dpmembuff
        push    ds
        pop     es
        push    ebx
        mov     ax,0500h
        cwAPI_CallOld
        pop     ebx
        pop     es

        cmp     ebx,-2
        jnz     api74_normal2

        mov     ebx,d[api74_dpmembuff+1Ch]
        shl     ebx,12
        jmp     api74_exit

api74_normal2:
        mov     ebx,d[api74_dpmembuff]
        ;
api74_exit:
        pop     ds
        ret
api74_dpmembuff:
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
        ;
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    ebp
        push    ds
        push    es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    d[api75_OldESI]
        mov     d[api75_OldESI],esi
        push    d[api75_OldEBX]
        mov     d[api75_OldEBX],ebx
        push    LastResource
        push    LastResource+4
        ;
        mov     edx,esi
        mov     ax,Res_MEM
        call    FindResource
        jc      api75_9
        ;
        mov     ebx,d[api75_OldEBX]
        mov     esi,ecx
        mov     di,si
        shr     esi,16
        mov     cx,bx
        shr     ebx,16                  ;set block size.
        mov     ax,0503h                ;release the block.
        cwAPI_CallOld
        jc      api75_9
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
        mov     ebx,d[api75_OldEBX]
        mov     es:[edx],ebx            ;update size.
        ;
        clc
        jmp     api75_10
        ;
api75_9:
        stc
api75_10:
        pop     LastResource+4
        pop     LastResource
        pop     d[api75_OldEBX]
        pop     d[api75_OldESI]
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret
api75_OldEBX:
        dd 0
api75_OldESI:
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
        ;
        push    ds
        push    es
        pushad
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    LastResource
        push    LastResource+4
        ;
        mov     edx,esi
        mov     ax,Res_MEM
        call    FindResource
        jc      api76_9
        ;
        mov     esi,ecx
        mov     di,si
        shr     esi,16
        mov     ax,0502h                ;release the block.
        cwAPI_CallOld
        jc      api76_9
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
api76_9:
        pop     LastResource+4
        pop     LastResource
        popad
        pop     es
        pop     ds
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
        push    ds
        push    es
        push    fs
        pushad
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     es,RealSegment
        mov     fs,PSPSegment
        ;
        cmp     ResourceTracking,0
        jz      api77_9
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_Resource],0   ;allocated memory for list yet?
        jnz     api77_0
        ;
        ;Allocate initial resource list space.
        ;
        pushad
        call    api77_GetAndInit
        jc      api77_1
        mov     DWORD PTR fs:[EPSP_Struc.EPSP_Resource],esi
api77_1:
        popad
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_Resource],0
        jz      api77_90
        ;
api77_0:
        ;Search list for free slot.
        ;
        cld
        mov     ebp,ecx                 ;Copy value 2.
        mov     ah,al                   ;Copy type.
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_Resource]
api77_FreeLoop:
        mov     edi,esi
        add     edi,size ResHead        ;point to types.
        mov     ecx,ResCount            ;get number of entries.
api77_2:
        or      ecx,ecx
        jz      api77_2_0
        js      api77_2_0
        xor     al,al
        repne   scasb                   ;Find NULL entry.
        jz      api77_3
api77_2_0:
        cmp     DWORD PTR es:[esi+ResHead_Next],0   ;link field setup?
        jz      api77_Extend
        mov     esi,es:[esi+ResHead_Next]   ;link to next block.
        jmp     api77_FreeLoop
        ;
api77_Extend:
        ;Extend the list.
        ;
        push    esi
        call    api77_GetAndInit
        pop     ecx
        jc      api77_90
        mov     es:[esi+ResHead_Prev],ecx   ;store back link address.
        mov     es:[ecx+ResHead_Next],esi   ;store forward link address.
        jmp     api77_FreeLoop
;
;Check if enough entries are free.
;
api77_3:
        cmp     ah,Res_SEL
        jz      api77_sel
        cmp     ah,Res_MEM
        jz      api77_mem
        cmp     ah,Res_LOCK
        jz      api77_lock
        cmp     ah,Res_DOSMEM
        jz      api77_dosmem
        cmp     ah,Res_CALLBACK
        jz      api77_callback
        cmp     ah,Res_PSP
        jz      api77_psp
        jmp     api77_sel
;
;Check for 2 free entries for DOS memory.
;
api77_dosmem:
        cmp     BYTE PTR es:[edi],0
        jnz     api77_2
        dec     edi
        mov     ecx,edi
        sub     ecx,size ResHead        ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,size ResHead + ResNum
        add     ecx,esi
        mov     BYTE PTR es:[edi],ah    ;Store type.
        mov     DWORD PTR es:[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebp
        jmp     api77_9
;
;Check for 3 free entries for callback.
;
api77_callback:
        cmp     WORD PTR es:[edi],0     ;2 more entries?
        jnz     api77_2
        dec     edi
        mov     ecx,edi
        sub     ecx,size ResHead        ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,size ResHead + ResNum
        add     ecx,esi
        mov     BYTE PTR es:[edi],ah    ;Store type.
        mov     DWORD PTR es:[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebp
        inc     edi
        add     ecx,4
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebx
        jmp     api77_9
;
;Check for 3 free entries for memory.
;
api77_mem:
        cmp     WORD PTR es:[edi],0     ;2 more entries?
        jnz     api77_2
        dec     edi
        mov     ecx,edi
        sub     ecx,size ResHead        ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,size ResHead + ResNum
        add     ecx,esi
        mov     BYTE PTR es:[edi],ah    ;Store type.
        mov     DWORD PTR es:[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebp
        inc     edi
        add     ecx,4
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebx
        jmp     api77_9
;
;Check for 2 free entries for lock's.
;
api77_lock:
        cmp     BYTE PTR es:[edi],0
        jnz     api77_2
        dec     edi
        mov     ecx,edi
        sub     ecx,size ResHead        ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,size ResHead + ResNum
        add     ecx,esi
        mov     BYTE PTR es:[edi],ah    ;Store type.
        mov     DWORD PTR es:[ecx],edx
        inc     edi
        add     ecx,4
        mov     ah,Res_CHAIN
        mov     BYTE PTR es:[edi],ah
        mov     DWORD PTR es:[ecx],ebp
        jmp     api77_9
;
;Only one free entry needed for selectors.
;
api77_psp:
api77_sel:
        dec     edi
        mov     ecx,edi
        sub     ecx,size ResHead        ;Get offset from list start.
        sub     ecx,esi
        shl     ecx,2
        add     ecx,size ResHead + ResNum
        add     ecx,esi
        mov     BYTE PTR es:[edi],ah    ;Store type.
        mov     DWORD PTR es:[ecx],edx
        jmp     api77_9
        ;
api77_90:
        stc
        jmp     api77_100
        ;
api77_9:
        clc
api77_100:
        popad
        pop     fs
        pop     es
        pop     ds
        ret
        ;
api77_GetAndInit:
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ebp
        mov     cx,ResSize
        xor     bx,bx
        mov     ax,0501h                ;Allocate memory block.
        cwAPI_CallOld
        jc      api77_GAIerror
        shl     esi,16
        mov     si,di
        shl     ebx,16
        mov     bx,cx
        xchg    esi,ebx
        mov     edi,esi
        xor     eax,eax
        mov     ecx,ResSize/4
        cld
        rep     stosd                       ;init memory.
        mov     DWORD PTR es:[esi+ResHead_Handle],ebx   ;store the handle.
        clc
api77_GAIerror:
        pop     ebp
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
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
        push    ds
        push    es
        push    fs
        pushad
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        cmp     ResourceTracking,0
        jz      api78_9
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_Resource],0
        jz      api78_9
        xchg    eax,edx
        mov     es,RealSegment
        cld
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_Resource]
api78_LookLoop:
        mov     edi,esi
        add     edi,size ResHead + ResNum
        mov     ecx,ResNum              ;get number of entries.
api78_2:
        repne   scasd
        jz      api78_0
        mov     esi,es:[esi+ResHead_Next]   ;link to next list.
        or      esi,esi
        jz      api78_9
        jmp     api78_LookLoop
        ;
api78_0:
        mov     ebx,edi
        sub     ebx,size ResHead + ResNum + 4
        sub     ebx,esi
        shr     ebx,2
        add     ebx,esi
        add     ebx,size ResHead
        cmp     BYTE PTR es:[ebx],dl    ;Right type?
        jz      api78_1
        cmp     dl,Res_SEL              ;Selector?
        jnz     api78_2
        cmp     BYTE PTR es:[ebx],Res_PSP
        jnz     api78_2
        ;
api78_1:
        ;At this point we have a match.
        ;
        xor     eax,eax
        sub     edi,4
        mov     dl,es:[ebx]
        xor     dh,dh
        cmp     dl,Res_SEL
        jz      api78_rel_1
        cmp     dl,Res_LOCK
        jz      api78_Rel_2
        cmp     dl,Res_MEM
        jz      api78_Rel_3
        cmp     dl,Res_DOSMEM
        jz      api78_Rel_2
        cmp     dl,Res_CALLBACK
        jz      api78_Rel_3
        cmp     dl,Res_PSP
        jz      api78_psp
        jmp     api78_9
;
api78_Rel_3:
        mov     BYTE PTR es:[ebx],dh
        mov     DWORD PTR es:[edi],eax
        inc     ebx
        add     edi,4
api78_Rel_2:
        mov     BYTE PTR es:[ebx],dh
        mov     DWORD PTR es:[edi],eax
        inc     ebx
        add     edi,4
api78_Rel_1:
        mov     BYTE PTR es:[ebx],dh
        mov     DWORD PTR es:[edi],eax
        jmp     api78_9
;
;Release a PSP. Assumes that the PSP memory will be released by the caller.
;
api78_psp:
        mov     es:[ebx],dh
        push    w[PSPSegment]
        mov     bx,es:[edi]
        mov     fs,bx
;
;Remove links to all other modules.
;
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_Imports],0
        jz      api78_no_imports
        push    ecx
        push    esi
        push    edi
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_Imports]
        mov     ecx,es:[esi]
        add     esi,4
api78_imp0:
        or      ecx,ecx
        jz      api78_imp1
        mov     edi,es:[esi]
        call    UnFindModule
        add     esi,4
        dec     ecx
        jmp     api78_imp0
api78_imp1:
        pop     edi
        pop     esi
        pop     ecx
api78_no_imports:

; MED, 02/07/2000, always explicitly release program selectors and memory
;       cmp     fs:[EPSP_Struc.EPSP_Resource],0
;       jnz     @@normal_res
        ;
        ;This must be a cwLoad PSP.
        ;
        ;release program selectors and memory manually.
        ;
        pushad
        movzx   ecx,WORD PTR fs:[EPSP_Struc.EPSP_SegSize]
        shr     ecx,3
        mov     bx,WORD PTR fs:[EPSP_Struc.EPSP_SegBase]
        or      ecx,ecx
        jz      api78_frelsel9
api78_frelsel:
        sys     RelSel
        add     bx,8
        dec     ecx
        jnz     api78_frelsel
api78_frelsel9:
        mov     esi,fs:[EPSP_Struc.EPSP_MemBase]
        sys     RelMemLinear32
        mov     esi,fs:[EPSP_Struc.EPSP_Exports]
        or      esi,esi
        jz      api78_frelsel0
        sys     RelMemLinear32
api78_frelsel0:
        popad
        ;
api78_normal_res:
        ;Take this PSP out of the linked list.
        ;
        pushad
        mov     bx,fs
        sys     GetSelDet32
        mov     esi,es:EPSP_Struc.EPSP_NextPSP[edx]
        mov     edi,es:EPSP_Struc.EPSP_LastPSP[edx]
        or      esi,esi
        jz      api78_ChainPSP0
        mov     es:EPSP_Struc.EPSP_LastPSP[esi],edi
api78_ChainPSP0:
        or      edi,edi
        jz      api78_ChainPSP1
        mov     es:EPSP_Struc.EPSP_NextPSP[edi],esi
api78_ChainPSP1:
        popad
        ;
        mov     ah,50h                  ;set PSP
        int     21h                     ;set new PSP.
        ;
        ;Search for PSP's and release them first.
        ;
        mov     esi,fs:[EPSP_Struc.EPSP_Resource]   ;Get resource pointer.
api78_fPSP0:
        or      esi,esi
        jz      api78_NoPSPSearch
        mov     ebp,ResNum
        mov     edi,esi
        add     edi,size ResHead
        mov     edx,esi
        add     edx,size ResHead + ResNum
api78_fPSP1:
        cmp     BYTE PTR es:[edi],Res_PSP
        jnz     api78_fPSP2
        push    ebx
        mov     ebx,es:[edx]            ;Get selector.
        sys     RelMem
        pop     ebx
api78_fPSP2:
        inc     edi
        add     edx,4
        dec     ebp
        jnz     api78_fPSP1
        mov     esi,es:[esi+ResHead_Next]   ;link to next list.
        jmp     api78_fPSP0
        ;
api78_NoPSPSearch:
        ;Now release all other types of resource.
        ;
        cmp     WORD PTR fs:[EPSP_Struc.EPSP_DPMIMem],0 ;Any DPMI save buffer?
        jz      api78_psp_0
        xor     bx,bx
        xchg    bx,WORD PTR fs:[EPSP_Struc.EPSP_DPMIMem]
        sys     RelMem
api78_psp_0:
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_INTMem],0 ;Any int/excep vector memory?
        jz      api78_psp_1
        xor     esi,esi
        xchg    esi,DWORD PTR fs:[EPSP_Struc.EPSP_INTMem]
        sys     RelMemLinear32
api78_psp_1:
        mov     esi,fs:[EPSP_Struc.EPSP_Resource]       ;Get resource pointer.
        or      esi,esi
        jz      api78_psp_9
api78_psp_2:
        cmp     DWORD PTR es:[esi+ResHead_Next],0   ;Found last entry in chain?
        jz      api78_psp_3
        mov     esi,DWORD PTR es:[esi+ResHead_Next]
        jmp     api78_psp_2
api78_psp_3:
        mov     ebp,ResNum
        mov     edi,esi
        add     edi,size ResHead
        mov     edx,esi
        add     edx,size ResHead + ResNum
api78_psp_4:
        cmp     BYTE PTR es:[edi],Res_NULL
        jz      api78_psp_5
        cmp     BYTE PTR es:[edi],Res_SEL
        jz      api78_sel
        cmp     BYTE PTR es:[edi],Res_MEM
        jz      api78_mem
        cmp     BYTE PTR es:[edi],Res_LOCK
        jz      api78_lock
        cmp     BYTE PTR es:[edi],Res_DOSMEM
        jz      api78_dosmem
        cmp     BYTE PTR es:[edi],Res_CALLBACK
        jz      api78_callback
        cmp     BYTE PTR es:[edi],Res_PSP
        jz      api78_rel_psp
        jmp     api78_psp_5
;
;Release a selector.
;
api78_sel:
        mov     BYTE PTR es:[edi],Res_NULL
        push    esi
        push    edi
        push    ebp
        push    edx
        mov     ebx,es:[edx]
        push    ResourceTracking
        mov     ResourceTracking,0
        call    _RelSelector
        pop     ResourceTracking
        pop     edx
        pop     ebp
        pop     edi
        pop     esi
        jmp     api78_psp_5
;
;Release some memory.
;
api78_mem:
        mov     ForcedFind,edx
        mov     ForcedFind+4,edi
        mov     BYTE PTR es:[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        push    esi
        push    edi
        push    ebp
        push    edx
        mov     esi,eax
        call    _RelMemory
        pop     edx
        pop     ebp
        pop     edi
        pop     esi
        sub     ebp,2
        jmp     api78_psp_5
;
;Release a lock.
;
api78_lock:
        mov     BYTE PTR es:[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        push    esi
        push    edi
        push    ebp
        push    edx
        mov     ecx,es:[edx]
        mov     esi,eax
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     UnLockMem32
        pop     ResourceTracking
        pop     edx
        pop     ebp
        pop     edi
        pop     esi
        dec     ebp
        jmp     api78_psp_5
;
;Release DOS memory.
;
api78_dosmem:
        mov     BYTE PTR es:[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        push    eax
        push    edx
        mov     edx,eax
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     RelMemDOS
        pop     ResourceTracking
        pop     edx
        pop     eax
        dec     ebp
        jmp     api78_psp_5
;
;Release a call-back.
;
api78_callback:
        mov     BYTE PTR es:[edi],Res_NULL
        mov     eax,es:[edx]
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        inc     edi
        add     edx,4
        mov     BYTE PTR es:[edi],Res_NULL
        push    eax
        push    ecx
        push    edx
        mov     ecx,eax
        mov     dx,cx
        shr     ecx,16
        mov     ax,0304h
        push    ResourceTracking
        mov     ResourceTracking,0
        cwAPI_CallOld
        assume ds:_cwMain
        pop     ResourceTracking
        pop     edx
        pop     ecx
        pop     eax
        sub     ebp,2
        jmp     api78_psp_5
;
;Release another PSP.
;
api78_rel_psp:
        push    esi
        push    edi
        push    ebp
        push    edx
        mov     ebx,es:[edx]
        sys     RelMem
        pop     edx
        pop     ebp
        pop     edi
        pop     esi
;
api78_psp_5:
        inc     edi
        add     edx,4
        dec     ebp
        jnz     api78_psp_4
        mov     eax,DWORD PTR es:[esi+ResHead_Prev]    ;get back link pointer.
        push    eax
        push    esi
        mov     esi,es:[esi+ResHead_Handle] ;get memory handle.
        mov     di,si
        shr     esi,16
        mov     ax,0502h                    ;release the block.
        cwAPI_CallOld
        pop     esi
        push    ebx
        push    ecx
        push    edi
        mov     cx,si
        shr     esi,16
        mov     bx,si
        xor     si,si
        mov     di,ResSize
        mov     ax,0703h                    ;discard the block.
        cwAPI_CallOld
        pop     edi
        pop     ecx
        pop     ebx
        pop     esi
        or      esi,esi
        jz      api78_psp_9
        mov     DWORD PTR es:[esi+ResHead_Next],0   ;make sure link pointer is clear
        jmp     api78_psp_3
api78_psp_9:
        ;
        ;Switch back to the old PSP
        ;
        pop     bx
        mov     ah,50h                      ;set PSP
        int     21h                         ;go back to old PSP.
        ;
api78_9:
        popad
        pop     fs
        pop     es
        pop     ds
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
        push    ds
        push    es
        push    fs
        push    eax
        push    edx
        push    esi
        push    edi
        push    ebp
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
        jnz     api79_Found
        cmp     ResourceTracking,0
        jz      api79_8
        cmp     DWORD PTR fs:[EPSP_Struc.EPSP_Resource],0
        jz      api79_8
        ;
        mov     esi,DWORD PTR fs:[EPSP_Struc.EPSP_Resource]
api79_LookLoop:
        mov     ecx,ResNum              ;get number of entries.
        mov     edi,esi
        add     edi,size ResHead + ResNum
        cld
api79_0:
        repne   scasd
        jz      api79_1
        mov     esi,es:[esi+ResHead_Next]   ;link to next list.
        or      esi,esi
        jnz     api79_LookLoop
        stc
        jmp     api79_9
api79_1:
        mov     ebp,edi
        sub     ebp,size ResHead + ResNum + 4
        sub     ebp,esi
        shr     ebp,2
        add     ebp,size ResHead
        add     ebp,esi
        cmp     es:[ebp],dl
        jnz     api79_0
        sub     edi,4
        ;
api79_Found:
        cmp     dl,Res_SEL
        jz      api79_8
        cmp     dl,Res_MEM
        jz      api79_2
        cmp     dl,Res_LOCK
        jz      api79_8
        cmp     dl,Res_DOSMEM
        jz      api79_8
        cmp     dl,Res_CALLBACK
        jz      api79_8
        cmp     dl,Res_PSP
        jz      api79_8
        stc
        jmp     api79_9
        ;
api79_2:
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     LastResource,edi
        mov     LastResource+4,ebp
        add     edi,4
        mov     ecx,es:[edi]
        add     edi,4
        mov     ebx,es:[edi]
        ;
api79_8:
        clc
        ;
api79_9:
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     eax
        pop     fs
        pop     es
        pop     ds
        ret
FindResource    endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
SaveExecState   proc    near    uses ds es eax ebx ecx edx edi esi
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
;
;Get memory for the interrupt/exception vector save buffer.
;
        mov     ecx,(256*6)+(256*4)+(32*6)
        sys     GetMemLinear32
        jc      api80_9
        mov     es,PSPSegment
        mov     DWORD PTR es:[EPSP_Struc.EPSP_INTMem],esi
        mov     edi,esi
        mov     es,RealSegment
;
;Get protected mode interrupt vectors.
;
        xor     ebx,ebx
api80_GetPVect:
        sys     GetVect
        mov     DWORD PTR es:[edi],edx
        mov     WORD PTR es:[edi+4],cx
        add     edi,6
        inc     ebx
        cmp     ebx,256
        jb      api80_GetPVect
;
;Get protected mode exception vectors.
;
        xor     ebx,ebx
api80_GetEVect:
        sys     GetEVect
        mov     DWORD PTR es:[edi],edx
        mov     WORD PTR es:[edi+4],cx
        add     edi,6
        inc     ebx
        cmp     ebx,32
        jb      api80_GetEVect
;
;Get real mode interrupt vectors.
;
        xor     ebx,ebx
api80_GetRVect:
        sys     GetRVect
        mov     WORD PTR es:[edi],dx
        mov     WORD PTR es:[edi+2],cx
        add     edi,4
        inc     ebx
        cmp     ebx,256
        jb      api80_GetRVect
;
;Get memory for DPMI state buffer.
;
        cmp     d[DPMIStateSize],0
        jz      api80_NoDPMISave
        mov     ecx,d[DPMIStateSize]
        sys     GetMem32
        jc      api80_9
;
;Save DPMI state.
;
        mov     es,PSPSegment
        mov     WORD PTR es:[EPSP_Struc.EPSP_DPMIMem],bx
        mov     es,bx
        xor     edi,edi
        xor     al,al
        test    BYTE PTR SystemFlags,1
        jz      api80_DPMISave32
        db 66h
        call    f[DPMIStateAddr]
        jmp     api80_NoDPMISave
api80_DPMISave32:
        call    f[DPMIStateAddr]
api80_NoDPMISave:
        clc
api80_9:
        assume ds:_apiCode
        ret
SaveExecState   endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LoadExecState   proc    near    uses ds es eax ebx ecx edx edi
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
;
;Set protected mode interrupt vectors.
;
        mov     es,PSPSegment
        mov     edi,DWORD PTR es:[EPSP_Struc.EPSP_INTMem]
        or      edi,edi
        jz      api81_NoIntRel
        mov     es,RealSegment
        xor     ebx,ebx
api81_SetPVect:
        sys     GetVect
        xchg    edx,DWORD PTR es:[edi]
        xchg    cx,WORD PTR es:[edi+4]
        sys     SetVect
        cmp     edx,DWORD PTR es:[edi]
        jnz     api81_Pdone
        cmp     cx,WORD PTR es:[edi+4]
        jnz     api81_Pdone
        mov     WORD PTR es:[edi+4],-1
api81_Pdone:
        add     edi,6
        inc     ebx
        cmp     ebx,256
        jb      api81_SetPVect
;
;Set protected mode exception vectors.
;
        xor     ebx,ebx
api81_SetEVect:
        sys     GetEVect
        xchg    edx,DWORD PTR es:[edi]
        xchg    cx,WORD PTR es:[edi+4]
        sys     SetEVect
        cmp     edx,es:[edi]
        jnz     api81_Edone
        cmp     cx,es:[edi+4]
        jnz     api81_Edone
        mov     WORD PTR es:[edi+4],-1
api81_Edone:
        add     edi,6
        inc     ebx
        cmp     ebx,32
        jb      api81_SetEVect
;
;Set real mode interrupt vectors.
;
        xor     ebx,ebx
api81_SetRVect:
        sys     GetRVect
        xchg    dx,WORD PTR es:[edi]
        xchg    cx,WORD PTR es:[edi+2]
        sys     SetRVect
        cmp     dx,es:[edi]
        jnz     api81_Rdone
        cmp     cx,es:[edi+2]
        jnz     api81_Rdone
        mov     WORD PTR es:[edi+2],-1
api81_Rdone:
        add     edi,4
        inc     ebx
        cmp     ebx,256
        jb      api81_SetRVect
;
;Restore DPMI stack state.
;
api81_NoIntRel:
        mov     es,PSPSegment
        mov     ax,WORD PTR es:[EPSP_Struc.EPSP_DPMIMem]
        or      ax,ax
        jz      api81_NoDPMIRel
        jmp     api81_NoDPMIRel
        mov     es,ax
        xor     edi,edi
        mov     al,1
        test    BYTE PTR SystemFlags,1
        jz      api81_SaveDPMI32
        db 66h
        call    f[DPMIStateAddr]
        jmp     api81_NoDPMIRel
api81_SaveDPMI32:
        call    f[DPMIStateAddr]
api81_NoDPMIRel:
        clc
        assume ds:_apiCode
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
        push    ds
        push    es
        push    fs
        push    gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[api82_Name],edx
        mov     w[api82_Name+4],ax
        mov     d[api82_Flags],ebx
        mov     d[api82_Command],esi
        mov     w[api82_Command+4],es
        mov     w[api82_Environment],cx
        ;
        cmp     w[api82_Command+4],0
        jnz     api82_CmdlOK
        mov     w[api82_Command+4],cs
        mov     d[api82_Command],offset nullCmdl
api82_CmdlOK:
        mov     w[api82_PSP],0
;
;Allocate PSP memory.
;
        mov     ecx,size EPSP_Struc
        sys     GetMemLinear32
        jc      api82_error             ;Not enough memory.
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    ResourceTracking
        mov     ResourceTracking,0
        sys     GetSel
        jnc     api82_memOK
        pop     ResourceTracking
        pop     ds
        sys     RelMemLinear32
        jmp     api82_error
api82_memOK:
        mov     edx,esi
        mov     ecx,size EPSP_Struc
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
        jnc     api82_memOK2
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
        jmp     api82_error
        ;
api82_memOK2:
        mov     w[api82_PSP],bx
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
        mov     ax,ds:[EPSP_Struc.EPSP_RealENV] ;copy real mode environment
        mov     es:[EPSP_Struc.EPSP_RealENV],ax ;segment.
        mov     eax,ds:[EPSP_Struc.EPSP_ExecCount]
        mov     es:[EPSP_Struc.EPSP_ExecCount],eax
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
        xor     edx,edx
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Resource],edx ;Clear memory fields.
        mov     DWORD PTR es:[EPSP_Struc.EPSP_INTMem],edx
        mov     DWORD PTR es:[EPSP_Struc.EPSP_DPMIMem],edx
        mov     WORD PTR es:[EPSP_Struc.EPSP_Parent],fs     ;set parent PSP.
        mov     DWORD PTR es:[EPSP_Struc.EPSP_DTA],80h      ;Use default PSP DTA.
        mov     WORD PTR es:[EPSP_Struc.EPSP_DTA+4],es
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransProt]  ;inherit current transfer buffer.
        mov     WORD PTR es:[EPSP_Struc.EPSP_TransProt],ax
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_TransReal]
        mov     WORD PTR es:[EPSP_Struc.EPSP_TransReal],ax
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_TransSize]
        mov     DWORD PTR es:[EPSP_Struc.EPSP_TransSize],eax
        mov     eax,DWORD PTR fs:[EPSP_Struc.EPSP_mcbMaxAlloc]
        mov     DWORD PTR es:[EPSP_Struc.EPSP_mcbMaxAlloc],eax
        mov     DWORD PTR es:[EPSP_Struc.EPSP_mcbHead],edx
        cmp     d[api82_Flags],2                            ;cwLoad?
        jz      api82_NoNext
        mov     WORD PTR fs:[EPSP_Struc.EPSP_Next],es
api82_NoNext:
        mov     WORD PTR es:[EPSP_Struc.EPSP_Next],dx
        mov     WORD PTR es:[PSP_Struc.PSP_Environment],dx
        mov     WORD PTR es:[EPSP_Struc.EPSP_SegBase],dx
        mov     WORD PTR es:[EPSP_Struc.EPSP_SegSize],dx
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Exports],edx
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Imports],edx
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Links],edx
        mov     WORD PTR es:[EPSP_Struc.EPSP_EntryCSEIP+4],dx
        mov     WORD PTR es:[EPSP_Struc.EPSP_PSPSel],es
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
api82_ChainPSP0:
        cmp     es:EPSP_Struc.EPSP_NextPSP[esi],0
        jz      api82_ChainPSP1
        mov     esi,es:EPSP_Struc.EPSP_NextPSP[esi]
        jmp     api82_ChainPSP0
api82_ChainPSP1:
        sys     GetSelDet32
        mov     es:EPSP_Struc.EPSP_NextPSP[esi],edx
        mov     es:EPSP_Struc.EPSP_LastPSP[edx],esi
        mov     es:EPSP_Struc.EPSP_NextPSP[edx],0
        pop     es
;
;Save full file name into EPSP.
;
        push    ds
        lds     esi,f[api82_Name]
        mov     edi,EPSP_Struc.EPSP_FileName
        mov     BYTE PTR es:[edi],0
        cmp     b[esi],0
        jz      api82_SkipCopy
        mov     al,b[esi+1]
        cmp     al,":"                  ;drive specification?
        jnz     api82_GetDrive
        lodsb
        stosb
        mov     dl,al                   ;dl - zero based drive number (0=A,...)
        or      dl,20h
        sub     dl,'a'
        movsb
        jmp     api82_CheckPath
api82_GetDrive:
        mov     ah,19h                  ;get current disc
        int     21h
        mov     dl,al                   ;dl - zero based drive number (0=A,...)
        add     al,'A'                  ;make it a character
        stosb
        mov     al,":"
        stosb
api82_CheckPath:
        mov     al,b[esi]
        cmp     al,"\"                  ;path specification?
        jz      api82_CopyName
        mov     al,"\"                  ;store leading \
        stosb
        mov     BYTE PTR es:[edi],0
        inc     dl                      ;dl - drive number is 1 based (1=A,...)
        xchg    esi,edi
        mov     ah,47h                  ;get current directory without
        push    ds                      ;leading and trailing \
        push    es
        pop     ds
        int     21h                     ;get text
api82_cp1:
        lodsb
        or      al,al
        jnz     api82_cp1
        dec     esi
        xchg    esi,edi
        pop     ds
        mov     al,"\"                  ;store trailing \
        stosb
api82_CopyName:
        lodsb
        stosb
        or      al,al
        jnz     api82_CopyName
api82_SkipCopy:
        pop     ds
;
;Switch to this PSP.
;
        cmp     d[api82_Flags],2        ;cwLoad?
        jz      api82_NoPSwitch0
        mov     ah,50h                  ;set PSP
        int     21h                     ;set new PSP.
;
;Set new DTA address.
;
        push    ds
        lds     edx,FWORD PTR es:[EPSP_Struc.EPSP_DTA]
        mov     ah,1ah
        int     21h
        pop     ds
;
;Preserve current state.
;
        call    SaveExecState           ;do old state save.
        jc      api82_error             ;Not enough memory.
api82_NoPSwitch0:
;
;Build command line.
;
        mov     es,w[api82_PSP]
        mov     edi,80h
        mov     DWORD PTR es:[edi],0
        push    ds
        lds     esi,f[api82_Command]
        movzx   ecx,b[esi]
        inc     ecx
        cld
        rep     movsb
        mov     BYTE PTR es:[edi],13
        pop     ds
;
;Check what's needed with the environment selector.
;
        cmp     d[api82_Flags],2        ;cwLoad?
        jz      api82_CopyEnv           ;NoEnv
        mov     ax,w[api82_Environment]
        or      ax,ax
        jz      api82_ParentEnv
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     WORD PTR es:[PSP_Struc.PSP_Environment],ax
        jmp     api82_GotEnv

api82_CopyEnv:
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     ax,WORD PTR es:[PSP_Struc.PSP_Environment]  ;Get parents environment.
        mov     es,w[api82_PSP]
        mov     WORD PTR es:[PSP_Struc.PSP_Environment],ax
        pop     es
        jmp     api82_NoEnv

;
;Need to make a copy of the parent environment string.
;
api82_ParentEnv:
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es,WORD PTR es:[EPSP_Struc.EPSP_Parent]     ;Get parent PSP.
        mov     es,WORD PTR es:[PSP_Struc.PSP_Environment]  ;Get parents environment.
;
;Find out how long current environment is.
;
        xor     esi,esi
api82_gp2:
        mov     al,es:[esi]             ;Get a byte.
        inc     esi
        or      al,al                   ;End of a string?
        jnz     api82_gp2               ;keep looking.
        mov     al,es:[esi]             ;Double zero?
        or      al,al
        jnz     api82_gp2               ;keep looking.
        add     esi,3                   ;Skip last 0 and word count.
        mov     ecx,esi
        add     ecx,256                 ;allow for exec name length.
        sys     GetMem32
        jc      api82_error
        mov     ax,es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     WORD PTR es:[PSP_Struc.PSP_Environment],bx
        mov     ecx,esi                 ;get length again.
        push    ds
        mov     ds,ax
        mov     es,bx
        xor     esi,esi
        xor     edi,edi
        rep     movsb                   ;copy current strings.
        pop     ds
;
;Add execution path and name to environment strings.
;
api82_GotEnv:
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     es,WORD PTR es:[PSP_Struc.PSP_Environment]
        xor     edi,edi
api82_gp0:
        mov     al,es:[edi]             ;Get a byte.
        inc     edi
        or      al,al                   ;End of a string?
        jnz     api82_gp0               ;keep looking.
        mov     al,es:[edi]             ;Double zero?
        or      al,al
        jnz     api82_gp0               ;keep looking.
        add     edi,3                   ;Skip last 0 and word count.
;
;Now copy the file name from EPSP to the environment.
;
        push    ds
        mov     ds,w[api82_PSP]
        mov     esi,EPSP_Struc.EPSP_FileName
api82_cp2:
        lodsb
        stosb
        or      al,al
        jnz     api82_cp2
        pop     ds
;
;Return to caller.
;
api82_NoEnv:
        clc
        jmp     api82_exit
;
;Not enough memory.
;
api82_error:
        stc
;
api82_exit:
        mov     bx,w[api82_PSP]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        ret
;
api82_PSP:
        dw 0
api82_Command:
        df 0
api82_Name:
        df 0
api82_Environment:
        dw 0
api82_Flags:
        dd 0
CreatePSP       endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
DeletePSP       proc    near
        push    ds
        push    es
        push    fs
        push    gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        mov     d[api83_Flags],eax
        mov     w[api83_PSP],bx
;
        cmp     d[api83_Flags],2
        jz      api83_NoIRel
        call    LoadExecState
;
;Close all open files.
;
        call    LoseFileHandles
;
;Restore resources.
;
api83_NoIRel:
        mov     es,apiDSeg
        assume es:_cwMain
        cmp     WORD PTR es:[TerminationHandler],offset cwClose
        jnz     api83_YesRelRes
        cmp     es:DebugDump,0
        jnz     api83_NoNRel
api83_YesRelRes:
        assume es:nothing
        mov     es,w[api83_PSP]
        mov     bx,WORD PTR es:[EPSP_Struc.EPSP_Parent]
        cmp     d[api83_Flags],2
        jz      api83_NoPRel
        mov     ah,50h                  ;set PSP
        int     21h                     ;restore old PSP.
api83_NoPRel:
        mov     bx,es
        sys     RelMem
        cmp     d[api83_Flags],2
        jz      api83_NoNRel
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:PSPSegment
        assume es:nothing
        mov     WORD PTR es:[EPSP_Struc.EPSP_Next],0
api83_NoNRel:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        ret
;
api83_PSP:
        dw ?
api83_Flags:
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
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    ebp
        push    ds
        push    es
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
        movzx   ecx,BYTE PTR es:[esi]
        inc     esi
        mov     ebx,esi
        mov     ebp,ecx
api84_5:
        inc     esi
        cmp     BYTE PTR es:[esi-1],"\"
        jnz     api84_6
        mov     ebx,esi
        mov     ebp,ecx
api84_6:
        dec     ecx
        jnz     api84_5
        mov     esi,ebx
        ;
api84_imp1:
        mov     ebx,esi
        mov     edx,es:EPSP_Struc.EPSP_Exports[edi] ;Point to export memory.
        or      edx,edx                 ;Any exports?
        jz      api84_imp3
        mov     edx,es:[edx+4]          ;Point to module name.
        mov     ecx,ebp
        cmp     cl,es:[edx]             ;Right name length?
        jnz     api84_imp3
        inc     edx
api84_imp2:
        mov     al,es:[ebx]
        cmp     al,es:[edx]             ;right char?
        jnz     api84_imp3
        inc     ebx
        inc     edx
        dec     ecx
        jnz     api84_imp2
        ;
        pop     esi
        jmp     api84_imp5              ;got it!
        ;
api84_imp3:
        mov     edi,es:EPSP_Struc.EPSP_NextPSP[edi]
        or      edi,edi                 ;check there is something else to look at.
        jnz     api84_imp1
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
        mov     bx,WORD PTR es:[PSP_Struc.PSP_Environment]
        pop     es
        push    edx
        push    ecx
        sys     GetSelDet32
        mov     esi,edx
        pop     ecx
        pop     edx
api84_imp3_0:
        mov     al,es:[esi]             ;Get a byte.
        inc     esi
        or      al,al                   ;End of a string?
        jnz     api84_imp3_0            ;keep looking.
        mov     al,es:[esi]             ;Double zero?
        or      al,al
        jnz     api84_imp3_0            ;keep looking.
        add     esi,3                   ;Skip last 0 and word count.
        ;
        ;Copy up to last "\"
        ;
        mov     edi,offset DLLNameSpace
        mov     ebp,edi
api84_imp3_1:
        mov     al,es:[esi]
        inc     esi
        mov     [edi],al
        inc     edi
        cmp     al,"\"
        jnz     api84_imp3_2
        mov     ebp,edi
api84_imp3_2:
        or      al,al
        jnz     api84_imp3_1
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
        movzx   ecx,BYTE PTR es:[esi]
        inc     esi
        mov     ebp,esi
api84_0:
        inc     esi
        cmp     BYTE PTR es:[esi-1],"\"
        jnz     api84_1
        mov     ebp,esi
api84_1:
        dec     ecx
        jnz     api84_0
        mov     ecx,esi
        sub     ecx,ebp
        pop     esi
        push    esi
        inc     esi
api84_2:
        cmp     esi,ebp
        jnc     api84_3
        mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        jmp     api84_2
api84_3:
        mov     b[edi],0
        ;
        ;Copy module name into work space.
        ;
        mov     edi,offset MODNameSpace
        mov     [edi],cl
        inc     edi
api84_4:
        mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        dec     ecx
        jnz     api84_4
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
        jnc     api84_8
;
;Couldn't find the module so return an error.
;
        mov     eax,1
        jmp     api84_error
;
;Try loading the module found.
;
api84_8:
        push    ExecMCount
        mov     ExecMCount,ecx
;
;Set master PSP.
;
        assume ds:nothing
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    PSPSegment
        mov     ah,51h                  ;get PSP
        int     21h
        push    bx
        mov     bx,BasePSP
        mov     ah,50h                  ;set PSP
        int     21h
;
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     edx,offset DLLNameSpace
        sys     cwLoad                  ;call ourselves.
;
;Go back to original PSP
;
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        pop     bx
        mov     ah,50h                  ;set PSP
        int     21h
        pop     PSPSegment
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        pop     ExecMCount
        jc      api84_error
;
        mov     bx,si                   ;get the PSP
        push    ecx
        push    edx
        sys     GetSelDet32             ;need a 32-bit address for it.
        mov     edi,edx
        pop     edx
        pop     ecx
        mov     WORD PTR es:[edi+EPSP_Struc.EPSP_EntryCSEIP+4],cx
        mov     DWORD PTR es:[edi+EPSP_Struc.EPSP_EntryCSEIP],edx
;
;Notify debugger of new module loaded.
;
        sys DbgNtfModLoad
;
;Call DLL's initialisation code.
;
        or      cx,cx
        jz      api84_imp5
        xor     eax,eax
        mov     ax,WORD PTR es:[edi+EPSP_Struc.EPSP_EntryCSEIP+4]
        lar     eax,eax
        test    eax,00400000h
        jnz     api84_imp6_0
        mov     WORD PTR es:[edi+EPSP_Struc.EPSP_EntryCSEIP+2],cx
api84_imp6_0:
        push    ds
        push    es
        push    fs
        push    gs
        pushad
        push    es
        pop     fs
        mov     ds,si
        mov     es,si
        test    eax,00400000h
        mov     eax,0
        jnz     api84_imp6
        db 66h
api84_imp6:
        call    FWORD PTR fs:[edi+EPSP_Struc.EPSP_EntryCSEIP]
        or      ax,ax
        popad
        pop     gs
        pop     fs
        pop     es
        pop     ds
        mov     eax,1
        jnz     api84_error
;
;Update modules referance count.
;
api84_imp5:
        inc     es:EPSP_Struc.EPSP_Links[edi]
;
;Return module PSP address to caller.
;
        xor     eax,eax
        clc
        jmp     api84_imp10
;
;Couldn't find name so return error to caller.
;
api84_error:
        stc
;
api84_imp10:
        pop     es
        pop     ds
        pop     ebp
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
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
        push    ds
        push    es
        push    fs
        push    gs
        pushad
;
        dec     es:EPSP_Struc.EPSP_Links[edi]
        jnz     api85_8
;
;Notify debugger of module unloading.
;
        sys DbgNtfModUnload
;
;Get this PSP's selector.
;
        mov     fs,es:EPSP_Struc.EPSP_PSPSel[edi]
;
;Call DLL's exit code.
;
        cmp     WORD PTR fs:[EPSP_Struc.EPSP_EntryCSEIP+4],0
        jz      api85_imp5
        xor     eax,eax
        mov     ax,WORD PTR fs:[EPSP_Struc.EPSP_EntryCSEIP+4]
        lar     eax,eax
        push    ds
        push    es
        push    fs
        push    gs
        pushad
        mov     ax,fs
        mov     ds,ax
        mov     es,ax
        test    eax,00400000h
        mov     eax,1
        jnz     api85_imp6
        db 66h
api85_imp6:
        call    FWORD PTR fs:[EPSP_Struc.EPSP_EntryCSEIP]
        or      ax,ax
        popad
        pop     gs
        pop     fs
        pop     es
        pop     ds
;
;Switch to the master PSP.
;
api85_imp5:
        assume ds:nothing
        mov     ds,apiDSeg
        assume ds:_cwMain
        push    PSPSegment
        mov     bx,BasePSP
        mov     ah,50h                  ;set PSP
        int     21h
;
;Release module.
;
        mov     bx,fs
        sys     RelSel
;
;Go back to original PSP
;
        pop     PSPSegment
        assume ds:_apiCode
;
api85_8:
        clc
        popad
        pop     gs
        pop     fs
        pop     es
        pop     ds
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
        push    ds
        push    es
        pushad
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        mov     d[api86_Path],edi
        mov     w[api86_Path+4],ax
        mov     d[api86_Name],esi
        mov     w[api86_Name+4],es

; MED 01/05/96
        push    eax
        push    es
        mov     ah,2fh                  ; get DTA in es:ebx
        int     21h
        mov     DWORD PTR EntryDTAAddress,ebx
        mov     WORD PTR EntryDTAAddress+4,es
        pop     es

        mov     edx,OFFSET TemporaryDTA
        mov     ah,1ah                  ; set DTA in ds:edx
        int     21h
        pop     eax

;
;Get path length so we don't have to keep scanning the string.
;
        push    ds
        mov     ds,ax
        xor     ecx,ecx
api86_0:
        cmp     b[edi],0
        jz      api86_1
        inc     edi
        inc     ecx
        jmp     api86_0
api86_1:
        pop     ds
        mov     d[api86_Length],ecx
;
;Setup initial mask pointer.
;
        mov     d[api86_Mask],offset api86_Masks
;
;Get DTA address.
;
        push    es
        mov     ah,2fh
        int     21h
        mov     d[api86_Dta],ebx
        mov     w[api86_Dta+4],es
        pop     es
;
;Work through all mask types.
;
api86_2:
        mov     esi,d[api86_Mask]
        cmp     b[esi],0                ;end of the list?
        jz      api86_9
;
;Add new mask to path string.
;
        add     d[api86_Mask],4         ;move to next mask.
        les     edi,f[api86_Path]
        add     edi,d[api86_Length]     ;point to end of path string.
        mov     BYTE PTR es:[edi],"*"
        inc     edi
        movsd                           ;copy extension.
        mov     BYTE PTR es:[edi],0
;
;Work through all files with the right extension.
;
        push    ds
        lds     edx,f[api86_Path]
        xor     cx,cx
        mov     ah,4eh                  ;find first file
        int     21h
        pop     ds
        jc      api86_2
        jmp     api86_4
;
api86_3:
        mov     ah,4fh
        int     21h
        jc      api86_2
;
;Add this name to the path string?
;
api86_4:
        les     edi,f[api86_DTA]
        test    BYTE PTR es:[edi+21],16 ;DIR?
        jnz     api86_3
        push    ds
        mov     eax,d[api86_Length]     ;point to end of path string.
        lds     esi,f[api86_Path]
        add     esi,eax
        lea     edi,[edi+1eh]           ;point to file name.
        mov     ecx,13
        cld
api86_5:
        mov     al,es:[edi]
        mov     [esi],al
        inc     esi
        inc     edi
        or      al,al
        jz      api86_6
        dec     ecx
        jnz     api86_5
api86_6:
        mov     b[esi],0                ;terminate the name.
        pop     ds
;
;Find out what the files "module" name is.
;
        ;
        ;Open the file.
        ;
        push    ds
        lds     edx,f[api86_Path]
        mov     ax,3d00h
        int     21h
        pop     ds
        jc      api86_7
        mov     w[api86_Handle],ax
        mov     bx,ax
        mov     d[api86_Count],0
        ;
api86_11:
        ;See what sort of file it is.
        ;
        mov     edx,offset api86_ID
        mov     ecx,2
        mov     ah,3fh
        int     21h
        jc      api86_7
        cmp     ax,cx
        jnz     api86_7
        cmp     w[api86_ID],"ZM"
        jz      api86_MZ
        cmp     w[api86_ID],"P3"
        jz      api86_3P
        jmp     api86_7
;
;Process an MZ section.
;
api86_MZ:
        mov     edx,offset api86_ID+2
        mov     ecx,1bh-2
        mov     ah,3fh
        int     21h
        jc      api86_7
        cmp     ax,cx
        jnz     api86_7
        cmp     w[api86_ID+18h],40h
        jz      api86_LE
        ;
        ;Find out how long the MZ bit is.
        ;
        mov     ax,w[api86_ID+2+2]      ;get length in 512 byte blocks

; MED 04/26/96
        cmp     WORD PTR [api86_ID+2],0
        je      medexe4                 ; not rounded if no modulo

        dec     ax                      ;lose 1 cos its rounded up

medexe4:
        add     ax,ax                   ;mult by 2
        xor     dh,dh
        mov     dl,ah
        mov     ah,al
        mov     al,dh                   ;mult by 256=*512
        add     ax,w[api86_ID+2]        ;add length mod 512
        adc     dx,0                    ;add any carry to dx
        mov     cx,ax
        sub     cx,1bh                  ;account for the header.
        sbb     dx,0
        xchg    cx,dx                   ;swap round for DOS.
        mov     ax,4201h                ;set new file offset.
        int     21h
        jmp     api86_11
;
;Process what should be an LE section.
;
api86_LE:
        mov     ecx,3ch
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h                     ;move to LE offset.
        mov     edx,offset api86_ID
        mov     ecx,4
        mov     ah,3fh
        int     21h                     ;read LE offset.
        jc      api86_7
        cmp     ax,4
        jnz     api86_7
        cmp     d[api86_ID],0
        jz      api86_7
        mov     ecx,d[api86_ID]
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h                     ;point to LE section.
        mov     edx,offset api86_ID+4
        mov     ah,3fh
        mov     ecx,2
        int     21h                     ;read ID string.
        jc      api86_7
        cmp     ax,2
        jnz     api86_7

        cmp     w[api86_ID+4],"EL"
        jz      medle1

IFDEF LXWORK
        mov     eax,DWORD PTR [api86_ID+4]
        mov     cs:[0],al
        cmp     w[api86_ID+4],"XL"      ; MED
        jnz     api86_7
        mov     cs:[0],bl
ENDIF
        jmp     api86_7

        ;
        ;Process an LE section.
        ;
medle1:
        mov     ecx,LE_Header.LE_ResidentNames-2
        mov     dx,cx
        shr     ecx,16
        mov     ax,4201h
        int     21h                     ;move to module name offset.
        mov     edx,offset api86_ID+4
        mov     ecx,4
        mov     ah,3fh
        int     21h                     ;read module name offset.
        jc      api86_7
        cmp     ax,4
        jnz     api86_7
        mov     ecx,d[api86_ID]
        add     ecx,d[api86_ID+4]
        mov     dx,cx
        shr     ecx,16
        mov     ax,4200h
        int     21h                     ;move to module name.
        mov     edx,offset api86_ID
        mov     ecx,256
        mov     ah,3fh
        int     21h                     ;read the module name.
        ;
        ;See if this module has right name.
        ;
        push    es
        les     edx,f[api86_Name]       ;point to name we're looking for.
        mov     edi,offset api86_ID
        movzx   ecx,b[edi]
        cmp     cl,es:[edx]             ;right length?
        jnz     api86_le7
api86_le5:
        inc     edx
        inc     edi
        mov     al,es:[edx]
        cmp     al,[edi]
        jnz     api86_le7
        dec     ecx
        jnz     api86_le5
        pop     es
        ;
        ;Close the file.
        ;
        xor     bx,bx
        xchg    bx,w[api86_Handle]
        mov     ah,3eh
        int     21h
        jmp     api86_8
        ;
api86_le7:
        pop     es
        jmp     api86_7
;
;Process a 3P section.
;
api86_3P:
        mov     edx,offset api86_ID+2
        mov     ecx,size NewHeaderStruc-2
        mov     ah,3fh
        int     21h
        jc      api86_7
        cmp     ax,cx
        jnz     api86_7
        sub     d[api86_ID+NewHeaderStruc.NewSize],size NewHeaderStruc
        ;
        ;Check this file has exports.
        ;
        cmp     d[api86_ID+NewHeaderStruc.NewExports],0
        jz      api86_3p6
        ;
        ;Skip segment definitions.
        ;
        movzx   edx,w[api86_ID+NewHeaderStruc.NewSegments]
        shl     edx,3
        sys     cwcInfo
        jc      api86_3p0
        mov     edx,eax
api86_3p0:
        sub     DWORD PTR [api86_ID+NewHeaderStruc.NewSize],edx
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        ;
        ;Skip relocations.
        ;
        mov     edx,d[api86_ID+NewHeaderStruc.NewRelocs]
        shl     edx,2
        or      edx,edx
        jz      api86_3p1
        sys     cwcInfo
        jc      api86_3p1
        mov     edx,eax
api86_3p1:
        sub     d[api86_ID+NewHeaderStruc.NewSize],edx
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        ;
        ;Load export details.
        ;
        mov     ecx,d[api86_ID+NewHeaderStruc.NewExports]
        sys     GetMemLinear32
        jc      api86_7                 ;treat memory error as no file.
        mov     edx,ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        assume ds:_apiCode
        sys     cwcInfo
        jc      api86_3p2

        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        sub     d[api86_ID+NewHeaderStruc.NewSize],ecx
        pop     ds

        push    es
        mov     ax,ds
        mov     es,ax
        mov     edi,esi
        sys     cwcLoad
        pop     es
        jc      api86_3p3
        jmp     api86_3p4

api86_3p2:
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        sub     d[api86_ID+NewHeaderStruc.NewSize],edx
        pop     ds

        mov     ecx,edx
        mov     edx,esi
        call    ReadFile
        jc      api86_3p3
        cmp     eax,ecx
        jz      api86_3p4
api86_3p3:
        pop     ds
        sys     RelMemLinear32
        jmp     api86_7
api86_3p4:
        mov     ax,ds
        mov     es,ax
        pop     ds
        ;
        ;See if this module has right name.
        ;
        push    ds
        lds     edx,f[api86_Name]       ;point to name we're looking for.
        mov     edi,esi
        add     edi,es:[edi+4]          ;get offset of module name.
        movzx   ecx,BYTE PTR es:[edi]
        cmp     cl,[edx]                ;right length?
        jnz     api86_3p7
api86_3p5:
        inc     edx
        inc     edi
        mov     al,[edx]
        cmp     al,es:[edi]
        jnz     api86_3p7
        dec     ecx
        jnz     api86_3p5
        pop     ds
        ;
        ;Release EXPORT record memory.
        ;
        sys     RelMemLinear32
        ;
        ;Close the file.
        ;
        xor     bx,bx
        xchg    bx,w[api86_Handle]
        mov     ah,3eh
        int     21h
        jmp     api86_8
;
;Not this one, move to next part of the file.
;
api86_3p7:
        pop     ds
        sys     RelMemLinear32
api86_3p6:
        mov     edx,d[api86_ID+NewHeaderStruc.NewSize]
        mov     cx,dx
        shr     edx,16
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        inc     d[api86_Count]
        jmp     api86_11
;
;Make sure file is closed.
;
api86_7:
        xor     bx,bx
        xchg    bx,w[api86_Handle]
        or      bx,bx
        jz      api86_3
        mov     ah,3eh
        int     21h
        jmp     api86_3
;
api86_8:
        clc
        jmp     api86_10
;
api86_9:
        stc
;
api86_10:

; MED 01/05/96
        pushf
        push    ds
        lds     edx,EntryDTAAddress
        mov     ah,1ah                  ; set DTA in ds:edx
        int     21h
        pop     ds
        popf

        popad
        mov     ecx,d[api86_Count]
        pop     es
        pop     ds
        ret
;
SearchModule    endp

api86_ID:
        db 256 dup (0)
api86_Handle:
        dw 0
api86_DTA:
        df 0
api86_Path:
        df 0
api86_Name:
        df 0
api86_Length:
        dd 0
api86_Count:
        dd 0
api86_Mask:
        dd 0
api86_Masks:
        db ".DLL"
        db ".EXE"
        db 0
; temporary DTA storage so PSP isn't munged, MED 01/03/96
TemporaryDTA    DB      80h DUP (?)
EntryDTAAddress DF      0               ; Entry DTA address

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

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    ebp
        ;
        mov     edx,es:[edi]            ;get number of exports.
        add     edi,4+4                 ;skip entry count and module name.
        ;
api87_imp11:
        push    edi
        mov     edi,es:[edi]            ;point to name string.
        mov     ebx,ebp
        xor     ecx,ecx
        mov     cl,es:[edi+6]
        cmp     cl,es:[ebx]
        jnz     api87_imp13
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
api87_imp12:
        mov     al,es:[edi]
        cmp     al,es:[ebx]
        jnz     api87_imp13
        inc     edi
        inc     ebx
        dec     ecx
        jnz     api87_imp12
        jmp     api87_imp14
        ;
api87_imp13:
        pop     edi
        add     edi,4
        dec     edx
        jnz     api87_imp11
        jmp     api87_error
        ;
api87_imp14:
        pop     edi
        mov     edi,es:[edi]
        clc
        jmp     api87_10
        ;
api87_error:
        stc
api87_10:
        pop     ebp
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
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
;
;Preserve details.
;
        mov     d[api88_Name],edx
        mov     w[api88_Name+4],ax
        mov     d[api88_Flags],ebx
        mov     d[api88_Command],esi
        mov     w[api88_Command+4],es
        mov     w[api88_Environment],cx
        mov     w[api88_Handle],0
;
;Try and open the file.
;
        push    ds
        lds     edx,f[api88_Name]
        mov     ax,3d00h                ;open, read only.
        int     21h
        pop     ds
        jc      api88_no_file_error
        mov     w[api88_Handle],ax      ;store the handle.
;
;Find out what format the current section is.
;
api88_0:
        mov     bx,w[api88_Handle]
        mov     edx,offset api88_Temp
        mov     ecx,2
        mov     ah,3fh
        int     21h
        jc      api88_file_error
        cmp     ax,cx
        jnz     api88_file_error
        ;
        cmp     w[api88_Temp],"ZM"      ;MZ EXE?
        jz      api88_MZ
        cmp     w[api88_Temp],"EL"      ;LE EXE?
        jz      api88_LE
        cmp     w[api88_Temp],"P3"      ;3P EXE?
        jz      api88_3P

IFDEF LXWORK
        cmp     w[api88_Temp],"XL"      ;LX EXE? -- MED
        jz      api88_LE
        mov     cs:[0],dl
ENDIF

        jmp     api88_file_error
        ;
api88_MZ:
        ;Look for an LE offset.
        ;
        mov     bx,w[api88_Handle]
        mov     dx,18h
        xor     cx,cx
        mov     ax,4200h
        int     21h
        mov     edx,offset api88_Temp
        mov     ecx,2
        mov     ah,3fh
        int     21h                     ;Fetch LE offset.
        jc      api88_file_error
        cmp     ax,cx
        jnz     api88_file_error
        cmp     w[api88_Temp],40h       ;LE offset present?
        jnz     api88_MZ2
        ;
        ;Fetch the NE/LE/LX offset.
        ;
        mov     bx,w[api88_Handle]
        mov     dx,3ch
        xor     cx,cx
        mov     ax,4200h
        int     21h
        mov     edx,offset api88_Temp
        mov     ecx,4
        mov     ah,3fh
        int     21h                     ;Fetch LE offset.
        jc      api88_file_error
        cmp     ax,cx
        jnz     api88_file_error
        cmp     d[api88_Temp],0         ;any offset?
        jz      api88_MZ2
        mov     eax,d[api88_Temp]
        mov     dx,ax
        shr     eax,16
        mov     cx,ax
        mov     bx,w[api88_Handle]
        mov     ax,4200h
        int     21h
        jmp     api88_0
        ;
api88_MZ2:
        ;Get MZ length and skip it.
        ;
        mov     dx,2
        xor     cx,cx
        mov     ax,4200h
        mov     bx,w[api88_Handle]
        int     21h
        mov     edx,offset api88_Temp
        mov     ecx,4
        mov     ah,3fh
        mov     bx,w[api88_Handle]
        int     21h
        jc      api88_file_error
        cmp     ax,cx
        jnz     api88_file_error
        mov     ax,w[api88_Temp+2]      ;get length in 512 byte blocks

; MED 04/26/96
        cmp     WORD PTR [api88_Temp],0
        je      medexe5                 ; not rounded if no modulo

        dec     ax                      ;lose 1 cos its rounded up

medexe5:
        add     ax,ax                   ;mult by 2
        xor     dh,dh
        mov     dl,ah
        mov     ah,al
        mov     al,dh                   ;mult by 256=*512
        add     ax,w[api88_Temp]        ;add length mod 512
        adc     dx,0                    ;add any carry to dx
        mov     cx,ax
        xchg    cx,dx                   ;swap round for DOS.
        mov     ax,4200h                ;set absolute position.
        mov     bx,w[api88_Handle]
        int     21h
        jmp     api88_0
;
;Run an LE format program.
;
api88_LE:
        mov     d[api88_ExecAdd],offset LoadLE
        jmp     api88_exec
;
;Run a 3P format program.
;
api88_3P:
        mov     d[api88_ExecAdd],offset Load3P
;
;Close the file.
;
api88_exec:
        mov     bx,w[api88_Handle]
        mov     w[api88_Handle],0
        mov     ah,3eh
        int     21h
;
;Fetch registers & call format specific loader.
;
        mov     edx,d[api88_Name]
        mov     ax,w[api88_Name+4]
        mov     ebx,d[api88_Flags]
        mov     esi,d[api88_Command]
        mov     es,w[api88_Command+4]
        mov     cx,w[api88_Environment]
        mov     ds,ax
        assume ds:nothing
        call    DWORD PTR cs:[api88_ExecAdd]

        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
;
api88_exit:
        pushf
        push    eax
        cmp     w[api88_Handle],0
        jz      api88_e0
        mov     bx,w[api88_Handle]
        mov     ah,3eh
        int     21h
api88_e0:
        pop     eax
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
api88_no_file_error:
        mov        eax,1
        stc
        jmp     api88_exit
api88_file_error:
        mov     eax,2
        stc
        jmp     api88_exit
api88_mem_error:
        mov     eax,3
        stc
        jmp     api88_exit
;
api88_Name:
        df 0
api88_Flags:
        dd 0
api88_Command:
        df 0
api88_Environment:
        dw 0
api88_Handle:
        dw 0
api88_Temp:
        dd 0
api88_ExecAdd:
        dd 0
_Exec   endp


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
        push    ds
        push    es
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    d[api90_Flags]
        push    d[api90_EntryEIP]
        push    w[api90_EntryCS]
        push    d[api90_EntryESP]
        push    w[api90_EntrySS]
        push    w[api90_EntryES]
        push    w[api90_EntryDS]
        push    w[api90_PSP]
        ;
        mov     d[api90_Flags],eax
        mov     d[api90_EntryEIP],ebx
        mov     w[api90_EntryCS],cx
        mov     d[api90_EntryESP],edx
        mov     w[api90_EntrySS],si
        mov     w[api90_EntryES],di
        mov     w[api90_EntryDS],bp
        mov     w[api90_PSP],di
;
        push    es
        mov     es,di
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Links],8000000h
        pop     es
;
;Patch Int 21h for function 4ch.
;
        push    w[api90_OldInt21h+6]
        mov     w[api90_OldInt21h+6],0
        mov     eax,d[api90_OldInt21h]
        or      ax,w[api90_OldInt21h+4]
        or      eax,eax                     ;Already patched?
        jnz     api90_NoPatch21h
        ;
        mov     w[api90_OldInt21h+6],-1
        mov     bl,21h
        sys     GetVect
        test    BYTE PTR apiSystemFlags,1   ;16/32 bit?
        jz      api90_Use32Bit100
        mov     w[api90_OldInt21h+2],cx
        mov     w[api90_OldInt21h],dx
        jmp     api90_Use16Bit100
api90_Use32Bit100:
        mov     w[api90_OldInt21h+4],cx
        mov     d[api90_OldInt21h],edx
api90_Use16Bit100:
        mov     bl,21h
        mov     cx,cs
        mov     edx,offset api90_Int21Patch
        sys     SetVect                     ;put us in the running.
;
;Patch exception termination handler address.
;
api90_NoPatch21h:
        mov     es,apiDSeg
        assume es:_cwMain
        push    DWORD PTR es:[TerminationHandler]
        push    DWORD PTR es:[TerminationHandler+4]
        mov     WORD PTR es:[TerminationHandler+4],cs
        mov     DWORD PTR es:[TerminationHandler],offset api90_KillIt
        assume es:nothing
        mov     es,w[api90_PSP]
        mov     DWORD PTR es:[EPSP_Struc.EPSP_SSESP],esp
        mov     WORD PTR es:[EPSP_Struc.EPSP_SSESP+4],ss
        lss     esp,f[api90_EntryESP]       ;fetch new stack address.
        xor     eax,eax
        mov     ebx,eax
        mov     ecx,eax
        mov     edx,eax
        mov     esi,eax
        mov     edi,eax
        mov     ebp,eax
        mov     fs,ax
        mov     gs,ax
        mov     ds,WORD PTR cs:[api90_EntryDS]
        sti
        assume ds:nothing
        jmp     FWORD PTR cs:[api90_EntryEIP]   ;pass control to program.
        ;
api90_Int21Patch:
        cmp     ah,4ch                      ;terminate?
        jz      api90_CheckKillIt
        cmp     ah,31h                      ;TSR?
        jnz     api90_OldVect
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
        mov     RealRegsStruc.Real_EAX[edi],3100h
        mov     RealRegsStruc.Real_EDX[edi],eax
        mov     bl,21h
        sys     IntXX
        jmp     api90_KillIt                ;just incase!
        ;
api90_OldVect:
        test    BYTE PTR cs:apiSystemFlags,1    ;16/32?
        jz      api90_Use32Bit101
        db 66h
        jmp     FWORD PTR cs:[api90_OldInt21h]  ;pass control to old handler.
api90_Use32Bit101:
        jmp     FWORD PTR cs:[api90_OldInt21h]
        ;
api90_CheckKillIt:
        push    ds
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        cmp     Int21hExecCount,0
        assume ds:nothing
        pop     ds
        jnz     api90_OldVect
;
;Clean things up and return to caller.
;
api90_KillIt:
        mov     ds,cs:apiDDSeg              ;make our data addressable.
        assume ds:_apiCode
        mov     w[api90_errorcode],ax
        mov     es,w[api90_PSP]
        lss     esp,FWORD PTR es:[EPSP_Struc.EPSP_SSESP]
        mov     es,apiDSeg
        assume es:_cwMain
        pop     DWORD PTR es:[TerminationHandler+4]
        pop     DWORD PTR es:[TerminationHandler]
        assume es:nothing
        push    ds
        push    ds
        push    ds
        pop     gs
        pop     fs
        pop     es
        sti                                 ;turn interupts back on.
        ;
        cmp     w[api90_OldInt21h+6],0
        jz      api90_NoRel21h
        test    BYTE PTR apiSystemFlags,1   ;16/32?
        jz      api90_Use32Bit102
        mov     cx,w[api90_OldInt21h+2]
        movzx   edx,w[api90_OldInt21h]
        jmp     api90_Use16Bit102
api90_Use32Bit102:
        mov     cx,w[api90_OldInt21h+4]
        mov     edx,d[api90_OldInt21h]
api90_Use16Bit102:
        mov     bl,21h
        sys     SetVect                     ;restore old handler.
api90_NoRel21h:
        pop     w[api90_OldInt21h+6]
;
;Get mouse event target state.
;
        push    ds
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     ds,w[Int33hExtension+20]    ;Get mouse data segment.
        assume ds:_Int33h
        xor     eax,eax
        xor     ebx,ebx
        cmp     WORD PTR Int33hUserOK,0
        jz      api90_meventnodum
        test    BYTE PTR cs:apiSystemFlags,1
        jz      api90_mevent32
        movzx   eax,w[Int33hUserCode]
        movzx   ebx,w[Int33hUserCode+2]
        jmp     api90_mevent16
api90_mevent32:
        mov     eax,d[Int33hUserCode]
        movzx   ebx,w[Int33hUserCode+4]
api90_mevent16:
        cmp     bx,Int33hCSeg
        jnz     api90_meventnodum
        cmp     eax,offset Int33hDummy
        jnz     api90_meventnodum
        xor     eax,eax
        xor     ebx,ebx
api90_meventnodum:
        mov     ds,Int33hDSeg
        assume ds:_cwMain
        mov     MouseETarget,eax
        mov     MouseETarget+4,ebx
        pop     ds
        assume ds:_apiCode
        ;
;       mov     ax,0                        ;reset mouse if present.
;       int     33h
        mov     ax,w[api90_errorcode]
        ;
        pop     w[api90_PSP]
        pop     w[api90_EntryDS]
        pop     w[api90_EntryES]
        pop     w[api90_EntrySS]
        pop     d[api90_EntryESP]
        pop     w[api90_EntryCS]
        pop     d[api90_EntryEIP]
        pop     d[api90_Flags]
        pop     es
        pop     ds
        ret
;
api90_OldInt21h:
        df 0,0
api90_errorcode:
        dw 0
api90_Flags:
        dd 0
api90_EntryEIP:
        dd 0
api90_EntryCS:
        dw 0
api90_EntryESP:
        dd 0
api90_EntrySS:
        dw 0
api90_EntryES:
        dw 0
api90_EntryDS:
        dw 0
api90_PSP:
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
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        movzx   ecx,cx
        mov     ebx,offset HexTableA
        add     edi,ecx
        dec     edi
        mov     edx,eax
api91_0:
        mov     al,dl
        shr     edx,4
        and     al,15
        xlat    cs:[ebx]
        mov     [edi],al
        dec     edi
        dec     ecx
        jnz     api91_0
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        push    ecx
        movzx   ecx,cx
        add     edi,ecx
        pop     ecx
        ret
HexTableA       db '0123456789ABCDEF'
Bin2HexA        endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LosefileHandles proc near

if 0
        push    ds
        push    es
        push    fs
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        mov     fs,PSPSegment
        cmp     WORD PTR fs:[PSP_HandlePtr+2],0
        jz      api92_donehandles
        les     di,DWORD PTR fs:[PSP_HandlePtr]
        mov     cx,WORD PTR fs:[PSP_Handles]

  if 0
api92_RelHandles:
        cmp     BYTE PTR es:[di],255
        jz      api92_NoRelHandle
        movzx   bx,BYTE PTR es:[di]
        mov     ah,3eh
        int     21h
api92_NoRelHandle:
        inc     di
        dec     cx
        jnz     api92_RelHandles
  endif

        mov     ax,fs
        mov     bx,es
        cmp     ax,bx
        jz      api92_donehandles
        sys     RelMem

  if 0
;
;Reduce real mode handle count.
;
        mov     edi,offset Int21Buffer
        mov     ax,TotalHandles         ;get current count.
        sub     ax,WORD PTR fs:[PSP_Handles]
        or      ax,1                    ;force bit 0 for DOS bug.
        mov     [edi].RealRegsStruc.Real_EAX,6700h
        mov     [edi].RealRegsStruc.Real_EBX,eax
        mov     bl,21h
        push    ds
        pop     es
        sys     IntXX
        sub     TotalHandles,20         ;update total handle count.
  endif

api92_donehandles:

        pop     fs
        pop     es
        pop     ds
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
        push    ecx
        push    edx
        push    esi
        xor     esi,esi                 ;reset length read.
api93_0:
        push    ebx
        push    ecx
        push    edx
        push    esi
        cmp     ecx,65535               ;size of chunks to load.
        jc      api93_1
        mov     ecx,65535               ;as close to 64k as can get.
api93_1:
        mov     ah,3fh
        int     21h                     ;read from the file.
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jc      api93_2                 ;DOS error so exit NOW.
        movzx   eax,ax                  ;get length read.
        add     esi,eax                 ;update length read counter.
        add     edx,eax                 ;move memory pointer.
        sub     ecx,eax                 ;update length counter.
        jz      api93_2                 ;read as much as was wanted.
        or      eax,eax                 ;did we read anything?
        jnz     api93_0
api93_2:
        mov     eax,esi
        pop     esi
        pop     edx
        pop     ecx
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
        test    BYTE PTR apiSystemFlags,1
        jz      api94_iUse32
        mov     w[OldInt23],dx
        mov     w[OldInt23+2],cx
        jmp     api94_iDone3216
api94_iUse32:
        mov     d[OldInt23],edx
        mov     w[OldInt23+4],cx
api94_iDone3216:
        mov     edx,offset Int23Handler
        mov     cx,cs
        mov     bl,23h
        sys     SetVect
        ;
        mov     bl,24h
        sys     GetVect
        test    BYTE PTR apiSystemFlags,1
        jz      api94_i24Use32
        mov     w[OldInt24],dx
        mov     w[OldInt24+2],cx
        jmp     api94_i24Done3216
api94_i24Use32:
        mov     d[OldInt24],edx
        mov     w[OldInt24+4],cx
api94_i24Done3216:
        mov     edx,offset Int24Handler
        mov     cx,cs
        mov     bl,24h
        sys     SetVect
;
;Patch 1Bh vector.
;
        ;
        ;Get a call back.
        ;
        push    ds
        push    es
        push    ds
        pop     es
        mov     esi,offset CtrlBrkEvent
        mov     edi,offset CtrlBrkEventTab
        push    cs
        pop     ds
        sys     GetCallBack
        pop     es
        pop     ds
        jc      api94_0

        ;
        ;Get current protected mode 1Bh vector.
        ;
        push    cx
        push    dx
        mov     bl,1bh
        sys     GetVect
        test    BYTE PTR apiSystemFlags,1
        jz      api94_1
        movzx   edx,dx
api94_1:
        mov     d[OldInt1BhP],edx
        mov     w[OldInt1BhP+4],cx
        ;
        ;Install our own handler.
        ;
        mov     edx,offset Int1bHandler
        mov     cx,cs
        mov     bl,1bh
        sys     SetVect
        pop     dx
        pop     cx

        ;
        ;Get current real mode 1Bh vector.
        ;
        push    cx
        push    dx
        mov     al,1bh
        sys     GetRVect
        mov     w[OldInt1bh],dx
        mov     w[OldInt1bh+2],cx
        pop     dx
        pop     cx
        mov     w[Int1bhcall],dx
        mov     w[Int1bhcall+2],cx
        ;
        ;Install call-back.
        ;
        mov     bl,1bh
        sys     SetRVect


api94_0:
        mov     ds,apiDSeg
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
        jz      api95_no1b
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
api95_no1b:
        cmp     d[OldInt23],0
        jz      api95_i0
        test    BYTE PTR apiSystemFlags,1
        jz      api95_iUse32
        mov     dx,w[OldInt23]
        mov     cx,w[OldInt23+2]
        jmp     api95_iDone3216
api95_iUse32:
        mov     edx,d[OldInt23]
        mov     cx,w[OldInt23+4]
api95_iDone3216:
        mov     bl,23h
        sys     SetVect
api95_i0:
        cmp     d[OldInt24],0
        jz      api95_i1
        test    BYTE PTR apiSystemFlags,1
        jz      api95_i24Use32
        mov     dx,w[OldInt24]
        mov     cx,w[OldInt24+2]
        jmp     api95_i24Done3216
api95_i24Use32:
        mov     edx,d[OldInt24]
        mov     cx,w[OldInt24+4]
api95_i24Done3216:
        mov     bl,24h
        sys     SetVect
api95_i1:
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
        test    BYTE PTR SystemFlags,1
        assume ds:_apiCode
        pop     ds
        jz      api96_start32
        ;
        mov     ax,[si]                 ;get stacked offset.
        mov     es:RealRegsStruc.Real_IP[di],ax
        mov     ax,2[si]
        mov     es:RealRegsStruc.Real_CS[di],ax
        mov     ax,4[si]
        mov     es:RealRegsStruc.Real_Flags[di],ax
        add     es:RealRegsStruc.Real_SP[di],6
        jmp     api96_start0
        ;
api96_start32:
;       mov     ax,[esi]                ;get stacked offset.
        mov     ax,[si]                 ;get stacked offset. MED 01/24/96

        mov     es:RealRegsStruc.Real_IP[edi],ax
;       mov     ax,2[esi]
        mov     ax,2[si]

        mov     es:RealRegsStruc.Real_CS[edi],ax
;       mov     ax,4[esi]
        mov     ax,4[si]

        mov     es:RealRegsStruc.Real_Flags[edi],ax
        add     es:RealRegsStruc.Real_SP[edi],6
        ;
api96_start0:
        pop     ax
        ;
        ;Give protected mode handler a shout.
        ;
        int     1bh
        ;
        ;Go back to caller.
        ;
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api96_Use32Bit2
        iret
api96_Use32Bit2:
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
        ;Need to chain to original real mode handler.
        ;
        mov     es,cs:apiDDSeg
        mov     edi,offset Int1bRegs
        mov     eax,DWORD PTR cs:[OldInt1bh]
        mov     es:RealRegsStruc.Real_IP[edi],ax
        shr     eax,16
        mov     es:RealRegsStruc.Real_CS[edi],ax
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     ax,0302h
        xor     ecx,ecx
        xor     ebx,ebx
        int     31h
        ;
        ;Return to caller.
        ;
        popad
        pop     es
        test    BYTE PTR cs:apiSystemFlags,1
        jz      api97_0
        iret
api97_0:
        iretd
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
        push    ds
        push    ds
        push    ds
        pop     gs
        pop     fs
        pop     es
        sti
        jmp     f[TerminationHandler]
        assume ds:_apiCode
OldInt23        df 0
Int23Handler    endp


;-------------------------------------------------------------------------
Int24Handler    proc    near
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
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        push    ds
        push    ds
        push    ds
        pop     gs
        pop     fs
        pop     es
        ;
api99_0:
        mov     edx,offset CriticalPrompt
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
        jnz     api99_0
        movzx   eax,b[edi+7]
        cmp     eax,2
        jz      api99_Terminate
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
        assume ds:nothing
        test    BYTE PTR cs:apiSystemFlags,1
        assume ds:_apiCode
        jz      api99_Use32_2
        iret
api99_Use32_2:
        iretd
        ;
api99_Terminate:
        assume ds:nothing
        mov     ds,cs:apiDSeg
        assume ds:_cwMain
        push    ds
        push    ds
        push    ds
        pop     gs
        pop     fs
        pop     es
        sti
        jmp     f[TerminationHandler]
        assume ds:_apiCode
;
OldInt24        df 0
CriticalPrompt  db 13,10,'Critical Error: Abort, Retry, Ignore, Fail? $'
CriticalKeys    db 'aArRiIfF'
CriticalCodes   db 2,2,1,1,0,0,3,3
Int24Handler    endp

        include load3p.asm
        include loadle.asm
        include decode_c.asm

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

