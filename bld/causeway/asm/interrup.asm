;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Protected mode interrupt/exception handlers.
;

;-------------------------------------------------------------------------------

;
DpmiEmuSystemFlags dd 0
ExceptionType   dd 0
;
; do NOT change order of these variables (you can add at the end), MED 01/08/96
ExceptionCode   dd 0
ExceptionFlags  dd 0
ExceptionIndex  dd 0
;
ExceptionEBP    dd ?
ExceptionEDI    dd ?
ExceptionESI    dd ?
ExceptionEDX    dd ?
ExceptionECX    dd ?
ExceptionEBX    dd ?
ExceptionEAX    dd ?
ExceptionGS     dw ?,?
ExceptionFS     dw ?,?
ExceptionES     dw ?,?
ExceptionDS     dw ?,?
ExceptionEIP    dd ?
ExceptionCS     dw ?,?
ExceptionEFL    dd ?
ExceptionESP    dd ?
ExceptionSS     dw ?,?
ExceptionTR     dw ?
ExceptionCR0    dd ?
ExceptionCR2    dd ?
ExceptionCR3    dd ?
;
ExceptionHeader label byte
        db 13,10,"CauseWay Error 09 : Unrecoverable internal exception, program terminated.",13,10
        db 13,10,'Exception: '
ExceptionINum   db '00, Error code: '
ExceptionENum   db '0000',13,10,13,10
        db 'EAX='
ExceptionEAXt   db '00000000 '
        db 'EBX='
ExceptionEBXt   db '00000000 '
        db 'ECX='
ExceptionECXt   db '00000000 '
        db 'EDX='
ExceptionEDXt   db '00000000 '
        db 'ESI='
ExceptionESIt   db '00000000 '
        db 13,10
        db 'EDI='
ExceptionEDIt   db '00000000 '
        db 'EBP='
ExceptionEBPt   db '00000000 '
        db 'ESP='
ExceptionESPt   db '00000000 '
        db 'EIP='
ExceptionEIPt   db '00000000 '
        db 'EFL='
ExceptionEFLt   db '00000000 '
        db 13,10,13,10
        db 'CS='
ExceptionCSt    db '0000 '
        db 'DS='
ExceptionDSt    db '0000 '
        db 'ES='
ExceptionESt    db '0000 '
        db 'FS='
ExceptionFSt    db '0000 '
        db 'GS='
ExceptionGSt    db '0000 '
        db 'SS='
ExceptionSSt    db '0000 '
        db 13,10,13,10
        db 'CR0='
ExceptionCR0t   db '00000000 '
        db 'CR2='
ExceptionCR2t   db '00000000 '
        db 'CR3='
ExceptionCR3t   db '00000000 '
        db 'TR='
ExceptionTRt    db '0000'
        db 13,10,13,10
        db 'SystemFlags='
ExceptionSysFlags db '00000000 '
        db 13,10,13,10,'$'
ExceptionHeaderEnd      label byte
;
ExceptionIntBuffer db size RealRegsStruc dup (?)
;

;
;Generate the initial entry points for the interupt handlers.
;
InterruptHandler proc near
        rept 256
        db 0e8h
        dd offset IntHandler-($+4)
        db 3 dup (-1)
        endm
InterruptHandler endp

;-------------------------------------------------------------------------------
;
;Interupt handler entry points (Int nn and IntR).
;
InterruptTable  proc    near
IntNum  = 0
        rept 2fh
        dd offset IntNN386Catch+IntNum
        dw DpmiEmuCS
IntNum  = IntNum+8
        endm
        dd offset Raw2FPatch
        dw DpmiEmuCS
IntNum  = IntNum+8
        rept 30h-2fh
        dd offset IntNN386Catch+IntNum
        dw DpmiEmuCS
IntNum  = IntNum+8
        endm
        dd offset RawDPMIPatch
        dw DpmiEmuCS
IntNum  = IntNum+8
        rept 256-32h
        dd offset IntNN386Catch+IntNum
        dw DpmiEmuCS
IntNum  = IntNum+8
        endm
InterruptTable  endp

;-------------------------------------------------------------------------------
IntNN386Catch   proc    near
        rept 256
        db 0e8h
        dd offset IntNN386-($+4)
        db 3 dup (-1)
        endm
IntNN386Catch   endp

;-------------------------------------------------------------------------------
;
;Exception handler entry points (Processor exceptions).
;
ExceptionTable  proc    near
IntNum  = 0
        rept 32
        dd offset ExcepNN386Catch+IntNum
        dw DpmiEmuCS
IntNum  = IntNum+8
        endm
ExceptionTable  endp

;-------------------------------------------------------------------------------
ExcepNN386Catch proc near
        rept 32
        db 0e8h
        dd offset ExcepNN386-($+4)
        db 3 dup (-1)
        endm
ExcepNN386Catch endp

;-------------------------------------------------------------------------------
;
;Get protected mode interupt handler address.
;
;On Entry:-
;
;BL     - Interupt vector number.
;
;On Exit:-
;
;CF set on error. (no errors looked for).
;
;CX:EDX - selector:offset of current handler.
;
RawGetVector    proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,DpmiEmuDS
        mov     ds,ax
        movzx   eax,bl
        mov     ebx,eax
        shl     ebx,1           ;*2
        mov     eax,ebx
        shl     ebx,1           ;*4
        add     ebx,eax         ;*6
        add     ebx,offset InterruptTable
        mov     edx,[ebx]               ;get offset.
        mov     cx,[ebx+4]              ;get segment selector.
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        ret
RawGetVector    endp

;-------------------------------------------------------------------------------
;
;Get real mode interupt handler address.
;
;On Entry:-
;
;BL     - Interupt vector number.
;
;On Exit:-
;
;CF set on error. (no errors looked for).
;
;CX:DX  - selector:offset of current handler.
;
RawGetRVector   proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalZero
        mov     es,ax
        movzx   ebx,bl
        shl     ebx,2
        mov     dx,es:[ebx]
        mov     cx,es:[ebx+2]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        ret
RawGetRVector   endp

;-------------------------------------------------------------------------------
;
;Get protected mode exception handler address.
;
;On Entry:-
;
;BL     - Exception vector number.
;
;On Exit:-
;
;CF set on error.
;
;CX:EDX - selector:offset of current handler.
;
RawGetEVector   proc    near
        ;pushm   eax,ebx,esi,edi,ebp,ds,es,fs,gs
        push    ds
        push    es
        push    fs
        push    gs
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    eax                     ; push last so is easily accessible for changing
        mov     ax,DpmiEmuDS
        mov     ds,ax
        movzx   eax,bl
        mov     edx,eax
        mov     ebx,eax
        shl     ebx,1           ;*2
        mov     eax,ebx
        shl     ebx,1           ;*4
        add     ebx,eax         ;*6
        add     ebx,offset ExceptionTable
        ;
        cmp     dl,14           ;Special case for 14
        ;jnz     @@Normal
        je      Special14       ; MED 01/17/96
        cmp     dl,20h
        jc      inter8_Normal
        stc                             ; flag error
        pop     eax
        mov     ax,8021h        ; flag invalid value
        push    eax
        jmp     inter8_GotVect  ; don't get vector
        ;
Special14:
        cmp     w[OldExcep14+4],0       ;Virtual memory active?
        jz      inter8_Normal
        ;
        ;Vector 14 and VMM is installed.
        ;
        mov     edx,d[OldExcep14]
        mov     cx,w[OldExcep14+4]
        jmp     inter8_GotVect
        ;
inter8_Normal:
        clc             ; MED 01/17/96, flag no error
        mov     edx,[ebx]               ;get offset.
        mov     cx,[ebx+4]              ;get segment selector.
inter8_GotVect:
        ;popm    eax,ebx,esi,edi,ebp,ds,es,fs,gs
        pop     eax
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     gs
        pop     fs
        pop     es
        pop     ds
        ret
RawGetEVector   endp

;-------------------------------------------------------------------------------
;
;Set protected mode interupt handler address.
;
;On Entry:-
;
;BL     - Interupt vector number.
;CX:EDX - selector:offset of new handler.
;
;On Exit:-
;
;CF set on error.
;
RawSetVector    proc    near
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
        mov     ax,DpmiEmuDS
        mov     ds,ax
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter9_use32_add
        movzx   edx,dx
inter9_use32_add:
        ;
        ;Check if its a hardware interrupt.
        ;
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        movzx   ebx,bl
        mov     al,[ebx+Int2CallCheck]
        or      al,al
        jz      inter9_NotHardware
        ;
        ;*** MED 11/30/95
        ;
        cmp     bl,23h                  ; always allow 23h callback
        je      med2a
        cmp     bl,24h                  ; always allow 24h callback
        je      med2a
        test    BYTE PTR NoPassFlag,0ffh        ; see if not passing hardware interrupts from real to protected mode
        jne     inter9_NotHardware
med2a:
        cmp     cx,DpmiEmuCS            ;restoreing previous vector?
        jnz     inter9_Setting
inter9_Restoreing:
        pushad
        movzx   cx,bl
        sub     bl,al
        movzx   bx,bl
        mov     ax,size CallBackStruc
        mul     bx
        mov     bx,ax
        add     bx,offset CallBackTable
        test    CallBackStruc.CallBackFlags[bx],CBFLAG_INUSE    ;this one in use?
        jz      inter9_DoneHardware     ;not likely.
        pushf
        cli
        push    es
        mov     ax,KernalZero
        mov     es,ax
        mov     CallBackStruc.CallBackFlags[bx],0       ;Mark this one as un-used.
        push    cx
        mov     cx,w[CallBackStruc.CallBackReal+2+bx]   ;get original real mode vector.
        mov     dx,w[CallBackStruc.CallBackReal+bx]
        pop     bx
        shl     bx,2
        mov     es:[bx],dx
        mov     es:[bx+2],cx
        pop     es
        popf
        jmp     inter9_DoneHardware
        ;
inter9_Setting:
        pushad
        movzx   cx,bl
        sub     bl,al
        movzx   bx,bl
        mov     ax,size CallBackStruc
        mul     bx
        mov     dx,bx
        mov     bx,ax
        add     bx,offset CallBackTable
        test    CallBackStruc.CallBackFlags[bx],CBFLAG_INUSE    ;this one in use?
        jnz     inter9_DoneHardware
        pushf
        cli
        push    es
        mov     ax,KernalZero
        mov     es,ax
        mov     CallBackStruc.CallBackNum[bx],cl        ;set interupt number.
        mov     CallBackStruc.CallBackFlags[bx],CBFLAG_INUSE or CBFLAG_INT  ;mark call back as used interupt.
        mov     ax,CallBackSize
        mul     dx
        mov     si,offset ICallBackList
        add     si,ax           ;index list of calls.
        push    bx
        mov     bx,cx
        shl     bx,2
        mov     dx,es:[bx]
        mov     cx,es:[bx+2]
        mov     es:[bx],si
        mov     WORD PTR es:[bx+2],seg _cwRaw
        pop     bx
        mov     w[CallBackStruc.CallBackReal+2+bx],cx   ;store original real mode vector.
        mov     w[CallBackStruc.CallBackReal+bx],dx
        pop     es
        popf
inter9_DoneHardware:
        popad
inter9_NotHardware:
        mov     ax,DpmiEmuDS
        mov     ds,ax
        ;
        assume ds:_cwDPMIEMU
        movzx   eax,bl
        mov     ebx,eax
        shl     ebx,1           ;*2
        mov     eax,ebx
        shl     ebx,1           ;*4
        add     ebx,eax         ;*6
        add     ebx,offset InterruptTable
        pushf
        cli
        mov     [ebx],edx               ;set offset.
        mov     [ebx+4],cx              ;set segment selector.
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
RawSetVector    endp

;-------------------------------------------------------------------------------
;
;Set real mode interupt handler address.
;
;On Entry:-
;
;BL     - Interupt vector number.
;CX:DX  - selector:offset of new handler.
;
;On Exit:-
;
;CF set on error.
;
RawSetRVector   proc    near
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
        mov     ax,KernalZero
        mov     es,ax
        movzx   ebx,bl
        shl     ebx,2
        pushf
        cli
        mov     es:[ebx],dx
        mov     es:[ebx+2],cx
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
RawSetRVector   endp

;-------------------------------------------------------------------------------
;
;Set protected mode exception handler address.
;
;On Entry:-
;
;BL     - Exception vector number.
;CX:EDX - selector:offset of new handler.
;
;On Exit:-
;
;CF set on error.
;
RawSetEVector   proc    near
        ;pushm   eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        push    ds
        push    es
        push    fs
        push    gs
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    eax                     ; push last so is easily accessible for changing
        mov     ax,DpmiEmuDS
        mov     ds,ax
        movzx   eax,bl
        push    eax
        mov     ebx,eax
        shl     ebx,1           ;*2
        mov     eax,ebx
        shl     ebx,1           ;*4
        add     ebx,eax         ;*6
        pop     eax
        add     ebx,offset ExceptionTable
        cmp     al,14           ;Special case for 14
        ;jnz     @@Normal
        je      Special14x      ; MED 01/17/96
        cmp     al,20h
        jc      inter11_Normal
        stc                             ; flag error
        pop     eax
        mov     ax,8021h        ; flag invalid value
        push    eax
        jmp     inter11_GotVect ; don't set vector
        ;
Special14x:
        cmp     w[OldExcep14+4],0       ;Virtual memory active?
        jz      inter11_Normal
        ;
        ;Vector 14 and VMM is still installed.
        ;
        mov     d[OldExcep14],edx
        mov     w[OldExcep14+4],cx
        jmp     inter11_GotVect
        ;
inter11_Normal:
        clc             ; MED 01/17/96, flag no error
        mov     [ebx],edx               ;set offset.
        mov     [ebx+4],cx              ;set segment selector.
inter11_GotVect:
        ;popm    eax,ebx,ecx,edx,esi,edi,ebp,ds,es,fs,gs
        pop     eax
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     gs
        pop     fs
        pop     es
        pop     ds
        ret
RawSetEVector   endp

;-------------------------------------------------------------------------------
;
;Allocate a real mode call back address.
;
;On Entry:-
;
;DS:ESI - Protected mode code.
;ES:EDI - Real mode register structure.
;
;On Exit:-
;
;Carry set on error, else,
;
;CX:DX  - Real mode address allocated.
;
RawGetCallBack  proc near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        mov     ax,ds
        push    ax
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        pop     ax
        mov     ebx,offset CallBackTable+((size CallBackStruc)*AutoCallBacks)
        mov     ecx,MaxCallBacks-AutoCallBacks
        mov     edx,AutoCallBacks
inter12_0:
        test    CallBackStruc.CallBackFlags[ebx],CBFLAG_INUSE   ;this one in use?
        jz      inter12_1
        add     ebx,size CallBackStruc
        inc     edx
        dec     ecx
        jnz     inter12_0
        jmp     inter12_9
        ;
inter12_1:
        pushad
        xor     eax,eax
        mov     cx,1
        int     31h
        mov     w[CallBackStruc.CallBackStackSel+ebx],ax
        popad
        jc      inter12_9
        pushad
        mov     bx,w[CallBackStruc.CallBackStackSel+ebx]
        mov     eax,8
        xor     ecx,ecx
        or      edx,-1
        int     31h
        popad
        mov     w[CallBackStruc.CallBackProt+4+ebx],ax  ;store protected mode code address.
        mov     d[CallBackStruc.CallBackProt+0+ebx],esi ;/
        mov     w[CallBackStruc.CallBackRegs+4+ebx],es  ;store register table address.
        mov     d[CallBackStruc.CallBackRegs+0+ebx],edi ;/
        mov     ax,CallBackSize
        mul     dx
        mov     esi,offset CallBackList
        movzx   eax,ax
        add     esi,eax         ;index list of calls.
        mov     CallBackStruc.CallBackOff[ebx],si               ;store call back address.
        mov     CallBackStruc.CallBackFlags[ebx],CBFLAG_INUSE   ;flag this entry in use.
        mov     ax,_cwRaw
        mov     cx,ax           ;get real mode code seg.
        mov     dx,si           ;get real mode offset.
        clc
        jmp     inter12_10
        ;
inter12_9:
        stc
inter12_10:
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        ret
        ;
        assume ds:_cwDPMIEMU
RawGetCallBack endp

;-------------------------------------------------------------------------------
;
;Release a previously allocated real mode call back address.
;
;On Entry:-
;
;CX:DX  - Real mode call back address.
;
RawRelCallBack proc near
        pushad
        push    ds
        push    es
        push    fs
        push    gs
        push    ax
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        pop     ax
        mov     esi,offset CallBackTable
        mov     ebx,MaxCallBacks
inter13_0:
        test    CallBackStruc.CallBackFlags[esi],CBFLAG_INUSE
        jz      inter13_1
        cmp     dx,CallBackStruc.CallBackOff[esi]
        jnz     inter13_1
        mov     CallBackStruc.CallBackFlags[esi],0
        mov     bx,CallBackStruc.CallBackStackSel[esi]
        mov     ax,1
        int     31h
        clc
        jmp     inter13_2
        ;
inter13_1:
        add     esi,size CallBackStruc
        dec     ebx
        jnz     inter13_0
        stc
inter13_2:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        ret
        ;
        assume ds:_cwDPMIEMU
RawRelCallBack endp

;-------------------------------------------------------------------------------
;
;Need to retrieve the interupt number.
;
IntHandler      proc    near
        push    ds
        push    eax
        ;
;
; Handler stack frame structure
;
; EAX:DS
;
; Return address
;
; Error code        only some exceptions
;
;        --
; EIP:CS   |
; Eflags   |
;         IFrame
; ESP:SS   |
;        --
SFrameA1 struc
sa1_eax      dd ?
sa1_ds       dd ?
sa1_retaddr  dd ?
sa1_errcode  dd ?
sa1_iret     IFrame <?>
SFrameA1 ends
        ;
        mov     ax,DpmiEmuDS            ;make our data addresable.
        mov     ds,ax           ;/
        movzx   esp,sp          ;our stack never >64k.
        mov     eax,[esp+SFrameA1.sa1_retaddr] ;get return address.
        sub     eax,offset InterruptHandler
        shr     eax,3           ;convert it to an interrupt number.
        mov     ExceptionIndex,eax      ;/
        ;
        ;Check if this is an exception or interrupt (any error code)
        ;
        cmp     esp,tPL0StackSize-4-SIZE SFrameA1
        jnz     inter14_NoCode
        ;clear NT
        and     w[esp+SFrameA1.sa1_iret.i_eflags],NOT EFLAG_NT
        ;
        ; MED 12/02/95
        ; check if Exception Index is 0dh
        ;  if so and instruction at CS:EIP is:
        ;   mov eax,cr0 [0f 20 c0]  or
        ;   mov cr0,eax [0f 22 c0] or
        ;   mov eax,cr3 [0f 20 d8] or
        ;   mov cr3,eax [0f 22 d8]
        ;  then emulate it here and return
        ; MED 11/12/98, emulate RDMSR [0f 32]
        ; MED 04/05/99, emulate WBINVD [0f 09]
        ;   WRMSR [0f 30]
        ;   mov eax,cr4 [0f 20 e0]
        ;   mov cr4,eax [0f 22 e0]
        ; MED 05/02/2000, mov ebx,cr4 [0f 20 e3]
        ;   mov eax,cr2 [0f 20 d0]
        ;
        push    ds
        cmp     eax,0dh
        jne     mednoem                         ; not a GPF
        mov     ax,w ss:[esp+4+SFrameA1.sa1_iret.i_cs]     ; ax==original CS
        ;verr    ax                              ; check for looping lockup invalid value
        ;jnz     mednoem
        mov     ds,ax
        mov     eax,ss:[esp+4+SFrameA1.sa1_iret.i_eip]   ; eax==original EIP
        cmp     BYTE PTR ds:[eax],0fh           ; first opcode byte
        jne     mednoem                         ; no match
        cmp     BYTE PTR ds:[eax+1],20h         ; mov reg,crx
        jne     med6b
        cmp     BYTE PTR ds:[eax+2],0c0h        ; mov eax,cr0
        jne     med2b
.586
        mov     eax,cr0
        jmp     medemu3eax                      ; update original eax with cr0 value
        ;
med2b:
        cmp     BYTE PTR ds:[eax+2],0d0h        ; mov eax,cr2
        jne     med3b
        mov     eax,cr2
        jmp     medemu3eax                      ; update original eax with cr2 value
        ;
med3b:
        cmp     BYTE PTR ds:[eax+2],0d8h        ; mov eax,cr3
        jne     med4b
        mov     eax,cr3
        jmp     medemu3eax                      ; update original eax with cr3 value
        ;
med4b:
        cmp     BYTE PTR ds:[eax+2],0e0h        ; mov eax,cr4
        jne     med5b
        mov     eax,cr4
medemu3eax:
        mov     ss:[esp+4+SFrameA1.sa1_eax],eax   ; update original eax with cr4 value
        jmp     medemu3
        ;
med5b:
        cmp     BYTE PTR ds:[eax+2],0e3h        ; mov ebx,cr4
        jne     mednoem
        mov     ebx,cr4
        jmp     medemu3
        ;
med6b:
        cmp     BYTE PTR ds:[eax+1],22h         ; mov crx,reg
        jne     med9b                           ; no match
        cmp     BYTE PTR ds:[eax+2],0c0h        ; move cr0,eax
        jne     med7b                           ; no match
        mov     eax,ss:[esp+4+SFrameA1.sa1_eax]   ; get original eax value
        mov     cr0,eax                         ; update cr0 value with original eax
        jmp     medemu3
        ;
med7b:
        cmp     BYTE PTR ds:[eax+2],0d8h        ; move cr3,eax
        jne     med8b                           ; no match
        mov     eax,ss:[esp+4+SFrameA1.sa1_eax]   ; get original eax value
        mov     cr3,eax                         ; update cr3 value with original eax
        jmp     medemu3
        ;
med8b:
        cmp     BYTE PTR ds:[eax+2],0e0h        ; move cr4,eax
        jne     mednoem                         ; no match
        mov     eax,ss:[esp+4+SFrameA1.sa1_eax]   ; get original eax value
        mov     cr4,eax                         ; update cr4 value with original eax
medemu3:
        mov     eax,3
        jmp     medemu
        ;
med9b:
        cmp     BYTE PTR ds:[eax+1],9           ; WBINVD
        jne     med10b
        wbinvd
        jmp     medemu2
        ;
med10b:
        cmp     BYTE PTR ds:[eax+1],30h         ; WRMSR
        jne     med11b
        mov     eax,ss:[esp+4+SFrameA1.sa1_eax]   ; get original eax value
        wrmsr
        jmp     medemu2
        ;
med11b:
        cmp     BYTE PTR ds:[eax+1],32h         ; RDMSR
        jne     mednoem
        rdmsr
        mov     ss:[esp+4+SFrameA1.sa1_eax],eax   ; update original eax value
        ;
.386p
        ;
medemu2:
        mov     eax,2
medemu:
        add     ss:[esp+4+SFrameA1.sa1_iret.i_eip],eax   ; adjust EIP past emulated instruction
        pop     ds
        pop     eax
        pop     ds
        add     esp,8                           ; flush return address and error code off stack
        iretd
        ;
mednoem:
        pop     ds
        mov     eax,[esp+SFrameA1.sa1_errcode]    ;get error code.
        mov     ExceptionCode,eax               ;/
        mov     eax,[esp+SFrameA1.sa1_iret.i_eflags] ;Get flags.
        mov     ExceptionFlags,eax              ;/
        mov     b ExceptionType,1               ;Let dispatch know its an exception.
        mov     eax,cr2                         ;Grab this now to save more PL
        mov     ExceptionCR2,eax                ;switches for page faults.
        pop     eax
        pop     ds
        add     esp,4                           ;skip error code.
        jmp     inter14_SortedCode2
        ;
SFrameA2 struc
sa2_eax      dd ?
sa2_ds       dd ?
sa2_retaddr  dd ?
sa2_iret     IFrame <?>
SFrameA2 ends
        ;
inter14_NoCode:
        ;clear NT.
        and     w[esp+SFrameA2.sa2_iret.i_eflags],NOT EFLAG_NT
        mov     eax,[esp+SFrameA2.sa2_iret.i_eflags] ;Get flags.
        mov     ExceptionFlags,eax              ;/
        mov     b ExceptionType,0               ;unset exception
        cmp     ExceptionIndex,0        ;int 0
        jz inter14_ForceException
        cmp     ExceptionIndex,1        ;int 1
        jz inter14_ForceException
        cmp     ExceptionIndex,6        ;int 6
        jnz     inter14_SortedCode
inter14_ForceException:
        mov     b ExceptionType,1               ;set exception
inter14_SortedCode:
        pop     eax
        pop     ds
inter14_SortedCode2:
        add     esp,4           ;skip return address.
        ;
        ;Check which stack we should switch back to.
        ;
        push    ds
        push    eax
        ;
SFrameA3 struc
sa3_eax      dd ?
sa3_ds       dd ?
sa3_iret     IFrame <?>
SFrameA3 ends
        ;
        cmp     w[esp+SFrameA3.sa3_iret.i_ss],KernalSS
        jz      KernalStack             ;Already on system stack?
        mov     ax,DpmiEmuDS
        mov     ds,ax
        test    b ExceptionType,1       ;exception?
        jnz     KernalStack
        push    ebx
        mov     ebx,ExceptionIndex
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        ;
        assume ds:_cwRaw
        cmp     b[ebx+Int2CallCheck],0  ;Hardware INT?
        pop     ebx
        ;
        assume ds:_cwDPMIEMU
        jnz     KernalStack
        jmp     IntStack
        ;
IntHandler      endp

;-------------------------------------------------------------------------------
;
;It's a user stack and its not an exception or hardware interupt so switch back
;to the original stack via a system stack to allow re-entrancy if original
;stack needs to be fetched from disk.
;
IntStack        proc    near
        pop     eax
        pop     ds
        ;
        ;Get new stack address.
        ;
        push    eax
        push    ebx
        push    ds
        ;
SFrameA4 struc
sa4_ds       dd ?
sa4_ebx      dd ?
sa4_eax      dd ?
sa4_iret     IFrame <?>
SFrameA4 ends
        ;
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        ;
        assume ds:_cwRaw
        mov     ebx,RawStackPos ;get next stack address.
        sub     RawStackPos,RawStackDif
        mov     ax,KernalSS
        mov     ds,ax
        ;
        assume ds:nothing
        ;
        ;Put old details onto new stack.
        ;
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter15_iUse32
        mov     eax,[esp+SFrameA4.sa4_iret.i_ss]
        sub     ebx,2
        mov     [ebx],ax                ;SS
        mov     eax,[esp+SFrameA4.sa4_iret.i_esp]
        sub     ebx,2
        mov     [ebx],ax                ;ESP
        mov     eax,[esp+SFrameA4.sa4_iret.i_eflags]
        sub     ebx,2
        mov     [ebx],ax                ;EFlags
        mov     eax,[esp+SFrameA4.sa4_iret.i_cs]
        sub     ebx,2
        mov     [ebx],ax                ;CS
        mov     eax,[esp+SFrameA4.sa4_iret.i_eip]
        sub     ebx,2
        mov     [ebx],ax                ;EIP
        jmp     inter15_iUse0
        ;
inter15_iUse32:
        mov     eax,[esp+SFrameA4.sa4_iret.i_ss]
        sub     ebx,4
        mov     [ebx],eax               ;SS
        mov     eax,[esp+SFrameA4.sa4_iret.i_esp]
        sub     ebx,4
        mov     [ebx],eax               ;ESP
        mov     eax,[esp+SFrameA4.sa4_iret.i_eflags]
        sub     ebx,4
        mov     [ebx],eax               ;EFlags
        mov     eax,[esp+SFrameA4.sa4_iret.i_cs]
        sub     ebx,4
        mov     [ebx],eax               ;CS
        mov     eax,[esp+SFrameA4.sa4_iret.i_eip]
        sub     ebx,4
        mov     [ebx],eax               ;EIP
inter15_iUse0:
        ;
        ;Put new details into current stack.
        ;
        mov     eax,offset inter15_Int
        mov     [esp+SFrameA4.sa4_iret.i_eip],eax      ;EIP
        xor     eax,eax
        mov     ax,DpmiEmuCS
        mov     [esp+SFrameA4.sa4_iret.i_cs],eax       ;CS
        pushfd
        pop     eax
        mov     [esp+SFrameA4.sa4_iret.i_eflags],eax   ;EFlags
        mov     [esp+SFrameA4.sa4_iret.i_esp],ebx      ;ESP
        xor     eax,eax
        mov     ax,KernalSS
        mov     [esp+SFrameA4.sa4_iret.i_ss],eax       ;SS
        pop     ds
        pop     ebx
        pop     eax
        ;
        assume ds:_cwDPMIEMU
        iretd
        ;
inter15_Int:
        ;
        ;Now switch back to original stack.
        ;
        assume ds:nothing
        push    cs:ExceptionIndex       ;need to know the INT number.
        push    eax
        push    ebx
        push    esi
        push    ds
        ;
SFrameA5 struc
sa5_ds       dd ?
sa5_esi      dd ?
sa5_ebx      dd ?
sa5_eax      dd ?
sa5_index    dd ?
sa5_iret     IFrame <?>
SFrameA5 ends
        ;
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter15_i2Use32
        xor     ebx,ebx
        mov     ax,ss
        mov     ds,ax
        mov     bx,sp
        lss     sp,[ebx+SFrameA5.sa5_iret.i16_sp]    ;get original stack again.
        mov     ax,[ebx+SFrameA5.sa5_iret.i16_flags] ;get flags.
        push    ax
        mov     ax,[ebx+SFrameA5.sa5_iret.i16_cs]    ;get CS
        push    ax
        mov     ax,[ebx+SFrameA5.sa5_iret.i16_ip]    ;get IP
        push    ax
        xor     eax,eax
        mov     ax,[ebx+SFrameA5.sa5_iret.i16_flags] ;get flags again.
        ;clear IF & TF
        and     ax,NOT (EFLAG_IF or EFLAG_TF)
        push    eax             ;int handler flags.
        jmp     inter15_i2Use0
        ;
inter15_i2Use32:
        mov     ax,ss
        mov     ds,ax
        mov     ebx,esp
        lss     esp,[ebx+SFrameA5.sa5_iret.i_esp]      ;get original stack again.
        mov     eax,[ebx+SFrameA5.sa5_iret.i_eflags]   ;get flags.
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_iret.i_cs]       ;get CS
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_iret.i_eip]      ;get IP
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_iret.i_eflags]   ;get flags again.
        ;clear IF & TF
        and     ax,NOT (EFLAG_IF or EFLAG_TF)
        push    eax             ;int handler flags.
inter15_i2Use0:
        mov     eax,[ebx+SFrameA5.sa5_index]     ;get INT index.
        shl     eax,1
        mov     esi,eax
        shl     eax,1
        add     esi,eax         ;*6 for index into table.
        add     esi,offset InterruptTable
        xor     eax,eax
        mov     ax,cs:[esi+4]   ;get int handler CS
        push    eax
        mov     eax,cs:[esi]            ;get INT handler EIP.
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_eax]       ;EAX
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_ebx] ;EBX
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_esi]   ;ESI
        push    eax
        mov     eax,[ebx+SFrameA5.sa5_ds]   ;DS
        push    eax
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        add     RawStackPos,RawStackDif
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        pop     esi
        pop     ebx
        pop     eax
        iretd                   ;pass control to INT handler.
IntStack        endp

;-------------------------------------------------------------------------------
;
;Either we were already on the system stack or we have an exception or hardware
;interupt on our hands. Either way we need to switch to another piece of the
;system stack to make sure we're using a legitimate one.
;
KernalStack     proc    near
        pop     eax
        pop     ds
        ;
        ;Get new stack address.
        ;
        push    eax
        push    ebx
        push    ds
        ;
SFrameA6 struc
sa6_ds       dd ?
sa6_ebx      dd ?
sa6_eax      dd ?
sa6_iret     IFrame <?>
SFrameA6 ends
        ;
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        ;
        assume ds:_cwRaw
        mov     ebx,RawStackPos ;get next stack address.
        sub     RawStackPos,RawStackDif
        mov     ax,KernalSS
        mov     ds,ax
        ;
        assume ds:nothing
inter16_Update:
        ;
        ;Put old details onto new stack.
        ;
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter16_Use32
        mov     eax,[esp+SFrameA6.sa6_iret.i_ss]
        sub     ebx,2
        mov     [ebx],ax                ;SS
        mov     eax,[esp+SFrameA6.sa6_iret.i_esp]
        sub     ebx,2
        mov     [ebx],ax                ;ESP
        mov     eax,[esp+SFrameA6.sa6_iret.i_eflags]
        sub     ebx,2
        mov     [ebx],ax                ;EFlags
        mov     eax,[esp+SFrameA6.sa6_iret.i_cs]
        sub     ebx,2
        mov     [ebx],ax                ;CS
        mov     eax,[esp+SFrameA6.sa6_iret.i_eip]
        sub     ebx,2
        mov     [ebx],ax                ;EIP
        jmp     inter16_Use0
        ;
inter16_Use32:
        mov     eax,[esp+SFrameA6.sa6_iret.i_ss]
        sub     ebx,4
        mov     [ebx],eax               ;SS
        mov     eax,[esp+SFrameA6.sa6_iret.i_esp]
        sub     ebx,4
        mov     [ebx],eax               ;ESP
        mov     eax,[esp+SFrameA6.sa6_iret.i_eflags]
        sub     ebx,4
        mov     [ebx],eax               ;EFlags
        mov     eax,[esp+SFrameA6.sa6_iret.i_cs]
        sub     ebx,4
        mov     [ebx],eax               ;CS
        mov     eax,[esp+SFrameA6.sa6_iret.i_eip]
        sub     ebx,4
        mov     [ebx],eax               ;EIP
inter16_Use0:
        ;
        ;Put new details into current stack.
        ;
        mov     eax,offset IntDispatch
        mov     [esp+SFrameA6.sa6_iret.i_eip],eax      ;EIP
        xor     eax,eax
        mov     ax,DpmiEmuCS
        mov     [esp+SFrameA6.sa6_iret.i_cs],eax       ;CS
        pushfd
        pop     eax
        mov     [esp+SFrameA6.sa6_iret.i_eflags],eax   ;EFlags
        mov     [esp+SFrameA6.sa6_iret.i_esp],ebx      ;ESP
        xor     eax,eax
        mov     ax,KernalSS
        mov     [esp+SFrameA6.sa6_iret.i_ss],eax       ;SS
        pop     ds
        pop     ebx
        pop     eax
        ;
        assume ds:_cwDPMIEMU
        ;
        ;Pass control to interupt dispatcher.
        ;
        iretd
KernalStack     endp

;-------------------------------------------------------------------------------
;
;Direct control to exception or interupt handler.
;
IntDispatch     proc    near
        push    eax
        push    esi
        push    edi
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        ;clear IF & TF.
        and     w[ExceptionFlags],NOT (EFLAG_IF or EFLAG_TF)
        mov     esi,ExceptionIndex      ;Get the exception number.
        add     esi,esi         ;*2
        mov     eax,esi
        add     esi,esi         ;*4
        add     esi,eax         ;*6
        test    b ExceptionType,1       ;exception?
        jnz     inter17_Excep
        ;
        ;Dispatch normal interrupt.
        ;
        add     esi,offset InterruptTable
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter17_Use32Bit14
        mov     eax,[esi]
        mov     d[inter17_Call2],eax
        mov     ax,[esi+4]
        mov     w[inter17_Call2+4],ax
        mov     eax,offset inter17_Resume
        mov     w[inter17_Call20],ax
        mov     w[inter17_Call20+2],cs
        pop     ds
        pop     edi
        pop     esi
        pop     eax
        ;
        assume ds:nothing
        push    WORD PTR cs:[ExceptionFlags]    ;EFlags before entry.
        push    WORD PTR cs:[inter17_Call20+2]  ;CS
        push    WORD PTR cs:[inter17_Call20]    ;EIP
        jmp     FWORD PTR cs:[inter17_Call2]
        ;
        assume ds:_cwDPMIEMU
inter17_Use32Bit14:
        mov     eax,[esi]
        mov     d[inter17_Call2],eax
        mov     ax,[esi+4]
        mov     w[inter17_Call2+4],ax
        mov     d[inter17_Call20],offset inter17_Resume
        mov     w[inter17_Call20+4],cs
        pop     ds
        pop     edi
        pop     esi
        pop     eax
        ;
        assume ds:nothing
        push    DWORD PTR cs:[ExceptionFlags]   ;EFlags before entry.
        push    DWORD PTR cs:[inter17_Call20+4] ;CS
        push    DWORD PTR cs:[inter17_Call20]   ;EIP
        jmp     FWORD PTR cs:[inter17_Call2]
        ;
        assume ds:_cwDPMIEMU
        ;
        ; MED 08/13/96
        ;nop             ; make debugger EIP adjustment from debug int benign?
inter17_Resume:
        ;
        ;Return from normal int.
        ;
        pushfd
        cli                     ;stop interupts interfering.
        push    eax
        push    ebx
        push    ds
        ;
SFrameA7 struc
sa7_ds       dd ?
sa7_ebx      dd ?
sa7_eax      dd ?
sa7_eflags   dd ?
sa7_iret     IFrame <?>
SFrameA7 ends
        ;
        mov     ax,ss
        mov     ds,ax
        mov     ebx,esp
        ;
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        ;
        assume ds:_cwDPMIEMU
        jz      inter17_Use32
        movzx   ebx,bx
        lss     sp,d[ebx+SFrameA7.sa7_iret.i16_sp] ;get old stack address.
        ;retain IF & TF & DF.
        and     w[ebx+SFrameA7.sa7_iret.i16_flags],EFLAG_IF or EFLAG_TF or EFLAG_DF
        ;clear IF & TF & DF.
        and     w[ebx+SFrameA7.sa7_eflags],NOT (EFLAG_IF or EFLAG_TF or EFLAG_DF)
        mov     ax,w[ebx+SFrameA7.sa7_eflags]
        or      ax,w[ebx+SFrameA7.sa7_iret.i16_flags]
        push    ax              ;EFlags.
        mov     ax,[ebx+SFrameA7.sa7_iret.i16_cs]
        push    ax              ;CS
        mov     ax,[ebx+SFrameA7.sa7_iret.i16_ip]
        push    ax              ;EIP
        mov     eax,[ebx+SFrameA7.sa7_eax]
        push    eax             ;EAX
        mov     eax,[ebx+SFrameA7.sa7_ebx]
        push    eax             ;EBX
        mov     eax,[ebx+SFrameA7.sa7_ds]
        push    eax             ;DS
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        add     RawStackPos,RawStackDif ;update next stack.
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        pop     ebx
        pop     eax
        iret
        ;
inter17_Use32:
        lss     esp,f[ebx+SFrameA7.sa7_iret.i_esp] ;get old stack address.
        ;retain IF & TF & DF.
        and     w[ebx+SFrameA7.sa7_iret.i_eflags],EFLAG_IF or EFLAG_TF or EFLAG_DF
        ;clear IF & TF & DF.
        and     w[ebx+SFrameA7.sa7_eflags],NOT (EFLAG_IF or EFLAG_TF or EFLAG_DF)
        mov     eax,[ebx+SFrameA7.sa7_eflags]
        or      eax,[ebx+SFrameA7.sa7_iret.i_eflags]
        push    eax             ;EFlags.
        mov     eax,[ebx+SFrameA7.sa7_iret.i_cs]
        push    eax             ;CS
        mov     eax,[ebx+SFrameA7.sa7_iret.i_eip]
        push    eax             ;EIP
        mov     eax,[ebx+SFrameA7.sa7_eax]
        push    eax             ;EAX
        mov     eax,[ebx+SFrameA7.sa7_ebx]
        push    eax             ;EBX
        mov     eax,[ebx+SFrameA7.sa7_ds]
        push    eax             ;DS
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        add     RawStackPos,RawStackDif ;update next stack.
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        pop     ebx
        pop     eax
        iretd
        ;
inter17_Excep:
        ;
        ;Dispatch exception.
        ;
        add     esi,offset ExceptionTable
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter17_eUse32Bit14
        mov     eax,[esi]
        mov     d[inter17_Call2],eax
        mov     ax,[esi+4]
        mov     w[inter17_Call2+4],ax
        mov     eax,offset inter17_ResumeExp
        mov     w[inter17_Call20],ax
        mov     w[inter17_Call20+2],cs
        pop     ds
        pop     edi
        pop     esi
        pop     eax
        ;
        assume ds:nothing
        push    WORD PTR cs:[ExceptionCode]     ;EFlags before entry.
        push    WORD PTR cs:[inter17_Call20+2]  ;CS
        push    WORD PTR cs:[inter17_Call20]    ;EIP
        jmp     FWORD PTR cs:[inter17_Call2]
        ;
        assume ds:_cwDPMIEMU
inter17_eUse32Bit14:
        mov     eax,[esi]
        mov     d[inter17_Call2],eax
        mov     ax,[esi+4]
        mov     w[inter17_Call2+4],ax
        mov     d[inter17_Call20],offset inter17_ResumeExp
        mov     w[inter17_Call20+4],cs
        pop     ds
        pop     edi
        pop     esi
        pop     eax
        ;
        assume ds:nothing
        push    DWORD PTR cs:[ExceptionCode]    ;EFlags before entry.
        push    DWORD PTR cs:[inter17_Call20+4] ;CS
        push    DWORD PTR cs:[inter17_Call20]   ;EIP
        jmp     FWORD PTR cs:[inter17_Call2]
        ;
        assume ds:_cwDPMIEMU
inter17_ResumeExp:
        ;
        ;Return from exception.
        ;
        cli
        ;
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        ;
        assume ds:_cwDPMIEMU
        jz      inter17_ExpUse32
        add     sp,2    ;correct stack position
        pushfd
        push    eax
        push    ebx
        push    ds
        ;
SFrameA8 struc
sa8_ds       dd ?
sa8_ebx      dd ?
sa8_eax      dd ?
sa8_eflags   dd ?
sa8_iret     IFrame <?>
SFrameA8 ends
        ;
        mov     ax,ss
        mov     ds,ax
        mov     ebx,esp
        movzx   ebx,bx
        lss     sp,d[ebx+SFrameA8.sa8_iret.i16_sp] ;get old stack address.
        mov     ax,[ebx+SFrameA8.sa8_iret.i16_flags]
        push    ax              ;EFlags.
        mov     ax,[ebx+SFrameA8.sa8_iret.i16_cs]
        push    ax              ;CS
        mov     ax,[ebx+SFrameA8.sa8_iret.i16_ip]
        push    ax              ;EIP
        mov     eax,[ebx+SFrameA8.sa8_eax]
        push    eax             ;EAX
        mov     eax,[ebx+SFrameA8.sa8_ebx]
        push    eax             ;EBX
        mov     eax,[ebx+SFrameA8.sa8_ds]
        push    eax             ;DS
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        add     RawStackPos,RawStackDif ;update next stack.
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        pop     ebx
        pop     eax
        iret
        ;
inter17_ExpUse32:
        add     esp,4
        pushfd
        push    eax
        push    ebx
        push    ds
        ;
SFrameA9 struc
sa9_ds       dd ?
sa9_ebx      dd ?
sa9_eax      dd ?
sa9_eflags   dd ?
sa9_iret     IFrame <?>
SFrameA9 ends
        ;
        mov     ax,ss
        mov     ds,ax
        mov     ebx,esp
        lss     esp,f[ebx+SFrameA9.sa9_iret.i_esp] ;get old stack address.
        mov     eax,[ebx+SFrameA9.sa9_iret.i_eflags]
        push    eax             ;EFlags.
        mov     eax,[ebx+SFrameA9.sa9_iret.i_cs]
        push    eax             ;CS
        mov     eax,[ebx+SFrameA9.sa9_iret.i_eip]
        push    eax             ;EIP
        mov     eax,[ebx+SFrameA9.sa9_eax]
        push    eax             ;EAX
        mov     eax,[ebx+SFrameA9.sa9_ebx]
        push    eax             ;EBX
        mov     eax,[ebx+SFrameA9.sa9_ds]
        push    eax             ;DS
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        add     RawStackPos,RawStackDif ;update next stack.
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        pop     ebx
        pop     eax
        iretd
        ;
inter17_Call2   df 0,0
inter17_Call20  df 0,0
        ;
IntDispatch     endp

;-------------------------------------------------------------------------------
;
;Handle an INT nn instruction by retrieving registers from the stack and
;reflect to real mode.
;
        ;
SFrameA10 struc
sa10_edi     dd ?
sa10_esi     dd ?
sa10_ebp     dd ?
sa10_esp     dd ?
sa10_ebx     dd ?
sa10_edx     dd ?
sa10_ecx     dd ?
sa10_eax     dd ?
sa10_buff    dw 10 dup (?)
sa10_gs      dd ?
sa10_fs      dd ?
sa10_es      dd ?
sa10_ds      dd ?
sa10_eflags  dd ?
sa10_callbck dd ?
sa10_retaddr dd ?
sa10_iret    IFrame <?>
SFrameA10 ends
        ;
IntNN386        proc    far
        sub     esp,sa10_retaddr-sa10_eflags
        push    ds
        push    es
        push    fs
        push    gs
        sub     esp,SIZE SFrameA10.sa10_buff
        pushad
        mov     ax,DpmiEmuDS            ;make our data addresable.
        mov     ds,ax           ;/
        mov     ebp,esp
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter18_Use32Bit19
        movzx   ebp,bp
        movzx   eax,w[ebp+SFrameA10.sa10_iret.i16_flags]
        jmp     inter18_Use16Bit19
        ;
inter18_Use32Bit19:
        mov     eax,[ebp+SFrameA10.sa10_iret.i_eflags]
inter18_Use16Bit19:
        mov     [ebp+SFrameA10.sa10_eflags],eax
        mov     edx,[ebp+SFrameA10.sa10_retaddr]
        sub     edx,offset IntNN386Catch
        shr     edx,3
        mov     d[ebp+SFrameA10.sa10_callbck],0
        mov     edi,ebp
        push    ss
        pop     es
        ;
        ;If this is a "hardware" interrupt we should mark it as
        ;busy if it isn't already marked.
        ;
        mov     ebx,edx
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        mov     al,[ebx+Int2CallCheck]
        or      al,al           ;Hardware?
        jz      inter18_c2
        sub     bl,al
        shl     ebx,3           ;*8
        mov     eax,ebx
        shl     ebx,1           ;*16
        add     ebx,eax         ;*24
        add     ebx,offset CallBackTable
        test    CallBackStruc.CallBackFlags[ebx],CBFLAG_INUSE
        jz      inter18_c2
        test    CallBackStruc.CallBackFlags[ebx],CBFLAG_BUSY    ;already busy?
        jnz     inter18_c2
        or      CallBackStruc.CallBackFlags[ebx],CBFLAG_BUSY    ;mark it as busy.
        mov     d[ebp+SFrameA10.sa10_callbck],ebx
        ;
        assume ds:_cwDPMIEMU
inter18_c2:
        pop     ds
        ;
        ;Now pass control to the INT simulator.
        ;
        mov     eax,[ebp+SFrameA10.sa10_eflags]
        ;clear IF & TF & DF.
        and     ax,NOT (EFLAG_IF or EFLAG_TF or EFLAG_DF)
        push    eax
        popfd
        mov     ebx,edx
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     es:RealRegsStruc.Real_SS[edi],0
        call    EmuRawSimulateInt
        cmp     d[ebp+SFrameA10.sa10_callbck],0
        jz      inter18_NoCall
        push    ds
        mov     esi,d[ebp+SFrameA10.sa10_callbck]
        mov     ax,KernalDS
        mov     ds,ax
        ;
        assume ds:_cwRaw
        and     CallBackStruc.CallBackFlags[esi],0FFh AND NOT CBFLAG_BUSY
        ;
        assume ds:_cwDPMIEMU
        pop     ds
inter18_NoCall:
        ;
        ;Update the flags.
        ;
        mov     ax,es:RealRegsStruc.Real_Flags[edi]
        ;clear IF & TF & DF & NT & IOPL.
        and     ax,NOT (EFLAG_NT or EFLAG_IOPL or EFLAG_IF or EFLAG_TF or EFLAG_DF)
        ;
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter18_Use32Bit
        mov     bx,[ebp+SFrameA10.sa10_iret.i16_flags]
        ;retain IF & TF & DF & NT & IOPL.
        and     bx,EFLAG_NT or EFLAG_IOPL or EFLAG_IF or EFLAG_TF or EFLAG_DF
        or      ax,bx
        mov     [ebp+SFrameA10.sa10_iret.i16_flags],ax
        popad
        add     sp,SIZE SFrameA10.sa10_buff
        pop     gs
        pop     fs
        pop     es
        pop     ds
        add     sp,sa10_iret-sa10_eflags
        iret                    ;Switch back to calling program.
        ;
inter18_Use32Bit:
        mov     bx,w[ebp+SFrameA10.sa10_iret.i_eflags]
        ;retain IF & TF & DF & NT & IOPL.
        and     bx,EFLAG_NT or EFLAG_IOPL or EFLAG_IF or EFLAG_TF or EFLAG_DF
        or      ax,bx
        mov     w[ebp+SFrameA10.sa10_iret.i_eflags],ax
        popad
        add     esp,SIZE SFrameA10.sa10_buff
        pop     gs
        pop     fs
        pop     es
        pop     ds
        add     esp,sa10_iret-sa10_eflags
        iretd                   ;Switch back to calling program.
IntNN386        endp

;-------------------------------------------------------------------------------
;
;An unhandled low level exception has occured so terminate the program.
;
ExcepNN386      proc    far
        push    eax
        push    ds
        ;
SFrameA11 struc
sa11_ds     dd ?
sa11_eax    dd ?
sa11_retaddr dd ?
SFrameA11 ends
        ;
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     eax,[esp+SFrameA11.sa11_retaddr]
        sub     eax,offset ExcepNN386Catch
        shr     eax,3
        mov     ExceptionIndex,eax
        pop     ds
        pop     eax
        add     esp,4           ;skip return address
        push    ds
        push    es
        push    fs
        push    gs
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        ;
SFrameA12 struc
sa12_ebp     dd ?
sa12_edi     dd ?
sa12_esi     dd ?
sa12_edx     dd ?
sa12_ecx     dd ?
sa12_ebx     dd ?
sa12_eax     dd ?
sa12_gs      dd ?
sa12_fs      dd ?
sa12_es      dd ?
sa12_ds      dd ?
sa12_iret    IFrame <?>
SFrameA12 ends
        ;
        mov     ax,DpmiEmuDS            ;make our data addresable.
        mov     ds,ax           ;/
        test    BYTE PTR DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter19_Use32Bit16
        movzx   eax,w[esp+SFrameA12.sa12_iret.i16_flags]
        mov     ExceptionEFL,eax
        jmp     inter19_Use16Bit16
        ;
inter19_Use32Bit16:
        mov     eax,[esp+SFrameA12.sa12_iret.i_eflags]
        mov     ExceptionEFL,eax
inter19_Use16Bit16:
        ;
        ;Retrieve register values and get outa here.
        ;
        mov     ax,DpmiEmuDS            ;make sure our data is addresable.
        mov     es,ax           ;/
        ;
        assume es:_cwDPMIEMU
        mov     ax,ss
        mov     ds,ax
        ;
        assume ds:nothing
        mov     esi,esp
        mov     edi,offset ExceptionEBP
        mov     ecx,SFrameA12.sa12_iret
        cld
        rep     movs b[edi],[esi]       ;copy registers off the stack.
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter19_Use32Bit17
        add     esi,2+2+2               ;skip return address/flags.
        xor     eax,eax
        lodsw
        stosd
        lodsw
        stosd
        lodsw
        stosd
        jmp     inter19_Use16Bit17
        ;
inter19_Use32Bit17:
        add     esi,4+4+4               ;skip return address/flags.
        mov     ecx,4+4+4
        rep     movs b[edi],[esi]       ;get real return address.
inter19_Use16Bit17:
        test    BYTE PTR cs:DpmiEmuSystemFlags,SYSFLAG_16B
        jz      inter19_Use32Bit678
        movzx   eax,w[esi]
        mov     es:ExceptionESP,eax
        mov     ax,[esi+2]
        mov     es:ExceptionSS,ax
        jmp     inter19_Use16Bit678
        ;
inter19_Use32Bit678:
        mov     eax,[esi]
        mov     es:ExceptionESP,eax
        mov     ax,[esi+4]
        mov     es:ExceptionSS,ax
inter19_Use16Bit678:
        push    es
        pop     ds
        ;
        assume es:nothing
        assume ds:_cwDPMIEMU
        ;
        ;Now switch to PL0 to get CR0-3 values.
        ;
        call    EmuRawPL3toPL0
.386P
        str     ax              ;get TR
        mov     ExceptionTR,ax
        mov     eax,cr0
        mov     ExceptionCR0,eax
        mov     eax,cr2
        mov     ExceptionCR2,eax
        mov     eax,cr3
        mov     ExceptionCR3,eax
        mov     cx,MainSS
        mov     edx,offset _cwStackEnd-256
        call    EmuRawPL0toPL3
        ;
        ;Convert register values into ASCII ready for printing.
        ;
        mov     eax,ExceptionIndex
        mov     ecx,2
        mov     edi,offset ExceptionINum
        call    Bin2HexE
        mov     eax,ExceptionCode       ;high word undefined upto 486.
        mov     ecx,4
        mov     edi,offset ExceptionENum
        call    Bin2HexE
        movzx   eax,ExceptionTR
        mov     ecx,4
        mov     edi,offset ExceptionTRt
        call    Bin2HexE
        mov     eax,ExceptionCR0
        mov     ecx,8
        mov     edi,offset ExceptionCR0t
        call    Bin2HexE
        mov     eax,ExceptionCR2
        mov     ecx,8
        mov     edi,offset ExceptionCR2t
        call    Bin2HexE
        mov     eax,ExceptionCR3
        mov     ecx,8
        mov     edi,offset ExceptionCR3t
        call    Bin2HexE
        mov     eax,ExceptionEAX
        mov     ecx,8
        mov     edi,offset ExceptionEAXt
        call    Bin2HexE
        mov     eax,ExceptionEBX
        mov     ecx,8
        mov     edi,offset ExceptionEBXt
        call    Bin2HexE
        mov     eax,ExceptionECX
        mov     ecx,8
        mov     edi,offset ExceptionECXt
        call    Bin2HexE
        mov     eax,ExceptionEDX
        mov     ecx,8
        mov     edi,offset ExceptionEDXt
        call    Bin2HexE
        mov     eax,ExceptionESI
        mov     ecx,8
        mov     edi,offset ExceptionESIt
        call    Bin2HexE
        mov     eax,ExceptionEDI
        mov     ecx,8
        mov     edi,offset ExceptionEDIt
        call    Bin2HexE
        mov     eax,ExceptionEBP
        mov     ecx,8
        mov     edi,offset ExceptionEBPt
        call    Bin2HexE
        mov     eax,ExceptionESP
        mov     ecx,8
        mov     edi,offset ExceptionESPt
        call    Bin2HexE
        mov     eax,ExceptionEIP
        mov     ecx,8
        mov     edi,offset ExceptionEIPt
        call    Bin2HexE
        mov     eax,ExceptionEFL
        mov     ecx,8
        mov     edi,offset ExceptionEFLt
        call    Bin2HexE
        movzx   eax,ExceptionCS
        mov     ecx,4
        mov     edi,offset ExceptionCSt
        call    Bin2HexE
        movzx   eax,ExceptionDS
        mov     ecx,4
        mov     edi,offset ExceptionDSt
        call    Bin2HexE
        movzx   eax,ExceptionES
        mov     ecx,4
        mov     edi,offset ExceptionESt
        call    Bin2HexE
        movzx   eax,ExceptionFS
        mov     ecx,4
        mov     edi,offset ExceptionFSt
        call    Bin2HexE
        movzx   eax,ExceptionGS
        mov     ecx,4
        mov     edi,offset ExceptionGSt
        call    Bin2HexE
        movzx   eax,ExceptionSS
        mov     ecx,4
        mov     edi,offset ExceptionSSt
        call    Bin2HexE
        mov     edi,offset ExceptionSysFlags
        mov     eax,DpmiEmuSystemFlags
        mov     ecx,8
        call    Bin2HexE
        ;
        ;Copy register details into the transfer buffer.
        ;
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        ;
        assume ds:_cwMain
        movzx   edi,TransferReal
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        shl     edi,4
        mov     esi,offset ExceptionHeader
        mov     ecx,offset ExceptionHeaderEnd-ExceptionHeader
        mov     ax,KernalZero
        mov     es,ax
        rep     movsb
        push    ds
        pop     es
        ;
        ;Print the message.
        ;
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        ;
        assume ds:_cwMain
        mov     ax,TransferReal
        ;
        assume ds:_cwDPMIEMU
        pop     ds
        mov     edi,offset ExceptionIntBuffer
        mov     RealRegsStruc.Real_DS[edi],ax
        mov     RealRegsStruc.Real_EDX[edi],0
        mov     RealRegsStruc.Real_EAX[edi],0900h
        mov     RealRegsStruc.Real_SP[edi],0
        mov     RealRegsStruc.Real_SS[edi],0
        mov     bl,21h
        call    EmuRawSimulateInt
        ;
        ;Now switch back to exit code.
        ;
if 0
        mov     ax,InitDS
        mov     ds,ax
        ;
        assume ds:_cwInit
        mov     WORD PTR IErrorNumber,0
        mov     ax,InitCS
        push    ax
        mov     ax,offset InitError
        push    ax
        db 66h
        retf
else
        mov     ax,MainDS
        mov     ds,ax
        ;
        assume ds:_cwMain
        jmp     f[TerminationHandler]
        ;
endif
ExcepNN386      endp
