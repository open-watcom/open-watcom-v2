        .386

_Excep  segment para public 'code' use16
        assume cs:_Excep, ds:_Excep, es:nothing
ExcepStart      label byte


;-------------------------------------------------------------------------
ExcepOpen       proc    near
        assume es:_cwMain
        mov     ExcepDSeg,es            ;Store cwCode selector.
        mov     ExcepDDSeg,ds
        mov     eax,es:SystemFlags
        mov     ExcepSystemFlags,eax
        assume es:nothing
        ;
        mov     bl,0
        mov     ecx,1
        mov     edi,offset OldExc00
        mov     esi,offset DPMIExc00Patch
@@2:    pushm   ebx,ecx,esi,edi
        call    @@1
        popm    ebx,ecx,esi,edi
        add     esi,DPMIExc01Patch-DPMIExc00Patch
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@2
        ;
        mov     bl,4
        mov     ecx,6
        mov     edi,offset OldExc04
        mov     esi,offset DPMIExc04Patch
@@3:    pushm   ebx,ecx,esi,edi
        call    @@1
        popm    ebx,ecx,esi,edi
        add     esi,DPMIExc01Patch-DPMIExc00Patch
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@3
        ;
        mov     bl,13
        mov     ecx,3
        mov     edi,offset OldExc13
        mov     esi,offset DPMIExc13Patch
@@0:    pushm   ebx,ecx,esi,edi
        call    @@1
        popm    ebx,ecx,esi,edi
        add     esi,DPMIExc01Patch-DPMIExc00Patch
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@0
        ;
        mov     bl,0
        sys     GetVect
        test    ExcepSystemFlags,1
        jz      @@i00Use32
        mov     w[OldInt00],dx
        mov     w[OldInt00+2],cx
        jmp     @@i00Done3216
@@i00Use32:     mov     d[OldInt00],edx
        mov     w[OldInt00+4],cx
@@i00Done3216:  mov     edx,offset Int00Handler
        mov     cx,cs
        mov     bl,00
        sys     SetVect
        ;
        clc
        retf
        ;
@@1:    pushm   ebx,esi,edi
        mov     ax,0202h
        int     31h
        popm    ebx,esi,edi
        test    ExcepSystemFlags,1
        jz      @@Use32
        mov     w[edi],dx
        mov     w[edi+2],cx
        mov     w[edi+4],cx
        jmp     @@Use0
@@Use32:        mov     d[edi],edx
        mov     w[edi+4],cx
@@Use0: mov     edx,esi
        mov     cx,cs
        mov     ax,0203h
        int     31h
        ret
ExcepOpen       endp


;-------------------------------------------------------------------------
ExcepClose      proc    near
        push    ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        ;
        mov     bl,0
        mov     ecx,1
        mov     edi,offset OldExc00
@@2:    pushm   ebx,ecx,edi
        call    @@1
        popm    ebx,ecx,edi
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@2
        ;
        mov     bl,4
        mov     ecx,6
        mov     edi,offset OldExc04
@@3:    pushm   ebx,ecx,edi
        call    @@1
        popm    ebx,ecx,edi
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@3
        ;
        mov     bl,13
        mov     ecx,3
        mov     edi,offset OldExc13
@@0:    pushm   ebx,ecx,edi
        call    @@1
        popm    ebx,ecx,edi
        add     edi,6
        inc     bl
        dec     ecx
        jnz     @@0
        ;
        cmp     d[OldInt00],0
        jz      @@i2
        test    ExcepSystemFlags,1
        jz      @@i00Use32
        mov     dx,w[OldInt00]
        mov     cx,w[OldInt00+2]
        jmp     @@i00Done3216
@@i00Use32:     mov     edx,d[OldInt00]
        mov     cx,w[OldInt00+4]
@@i00Done3216:  mov     bl,00h
        sys     SetVect
        ;
@@i2:   pop     ds
        clc
        retf
        ;
@@1:    test    ExcepSystemFlags,1
        jz      @@Use32
        movzx   edx,w[edi]
        mov     cx,w[edi+2]
        jmp     @@Use0
@@Use32:        mov     edx,d[edi]
        mov     cx,w[edi+4]
@@Use0: mov     ax,0203h
        int     31h
        ret
        ;
OldExc00        df 0
OldExc01        df 0
OldExc02        df 0
OldExc03        df 0
OldExc04        df 0
OldExc05        df 0
OldExc06        df 0
OldExc07        df 0
OldExc08        df 0
OldExc09        df 0
OldExc10        df 0
OldExc11        df 0
OldExc12        df 0
OldExc13        df 0
OldExc14        df 0
OldExc15        df 0
ExcepClose      endp


;-------------------------------------------------------------------------
;
;Handle a divide by zero.
;
Int00Handler    proc    near
        push    ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugEAX,eax
        mov     DebugEBX,ebx
        mov     DebugECX,ecx
        mov     DebugEDX,edx
        mov     DebugESI,esi
        mov     DebugEDI,edi
        mov     DebugEBP,ebp
        pop     ax
        mov     DebugESP,esp
        mov     DebugDS,ax
        mov     DebugES,es
        mov     DebugFS,fs
        mov     DebugGS,gs
        ;
        test    ExcepSystemFlags,1
        jz      @@Use32_0
        add     DebugESP,2+2+2
        movzx   ebx,sp
        mov     ax,ss:[ebx+2+2]
        mov     w[DebugEFL],ax
        mov     ax,ss:[ebx+2]
        mov     DebugCS,ax
        movzx   eax,ss:w[ebx]
        mov     DebugEIP,eax
        jmp     @@Use0_0
        ;
@@Use32_0:      add     DebugESP,4+4+4
        mov     eax,[esp+4+4]
        mov     DebugEFL,eax
        mov     eax,[esp+4]
        mov     DebugCS,ax
        mov     eax,[esp]
        mov     DebugEIP,eax
        ;
@@Use0_0:       mov     DebugExceptionIndex,0
        mov     DebugExceptionCode,0
        ;
        mov     ds,ExcepDSeg
        assume ds:_cwMain
        mov     DebugDump,1
        pushm   ds,ds,ds
        popm    es,fs,gs
        sti
        jmp     f[TerminationHandler]
        assume ds:_Excep
OldInt00        df 0
Int00Handler    endp


;-------------------------------------------------------------------------
DPMIExc00Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,0
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc00Patch endp


;-------------------------------------------------------------------------
DPMIExc01Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,1
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc01Patch endp


;-------------------------------------------------------------------------
DPMIExc02Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,2
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc02Patch endp


;-------------------------------------------------------------------------
DPMIExc03Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,3
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc03Patch endp


;-------------------------------------------------------------------------
DPMIExc04Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,4
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc04Patch endp


;-------------------------------------------------------------------------
DPMIExc05Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,5
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc05Patch endp


;-------------------------------------------------------------------------
DPMIExc06Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,6
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc06Patch endp


;-------------------------------------------------------------------------
DPMIExc07Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,7
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc07Patch endp


;-------------------------------------------------------------------------
DPMIExc08Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,8
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc08Patch endp


;-------------------------------------------------------------------------
DPMIExc09Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,9
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc09Patch endp


;-------------------------------------------------------------------------
DPMIExc10Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,10
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc10Patch endp


;-------------------------------------------------------------------------
DPMIExc11Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,11
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc11Patch endp


;-------------------------------------------------------------------------
DPMIExc12Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,12
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc12Patch endp


;-------------------------------------------------------------------------
DPMIExc13Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,13
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc13Patch endp


;-------------------------------------------------------------------------
DPMIExc14Patch proc     far
        push    ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,14
        pop     ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc14Patch endp


;-------------------------------------------------------------------------
DPMIExc15Patch proc     far
        pushm   ds
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     DebugExceptionIndex,15
        popm    ds
        db 0e9h
        dw offset DPMIExcPatch-($+2)
DPMIExc15Patch endp



;-------------------------------------------------------------------------
DPMIExcPatch    proc    far
        pushm   ds,es,fs,gs
        pushm   eax,ebx,ecx,edx,esi,edi,ebp             ;save general registers.
        assume ds:nothing
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        ;
        cmp     DebugExceptionIndex,14
        jnz     @@Not14Special
        push    ds
        mov     ds,ExcepDSeg
        assume ds:_cwMain
        cmp     LinearAddressCheck,0
        assume ds:_Excep
        pop     ds
        jz      @@Not14Special
        push    ds
        mov     ds,ExcepDSeg
        assume ds:_cwMain
        mov     LinearAddressCheck,0
        test    cs:ExcepSystemFlags,1
        assume ds:_Excep
        pop     ds
        popm    eax,ebx,ecx,edx,esi,edi,ebp             ;save general registers.
        popm    ds,es,fs,gs
        mov     esi,0
        jz      @@S14_32
        retf
@@S14_32:       ;
        db 66h
        retf
        ;
@@Not14Special: ;Retrieve register values and get outa here.
        ;
        cmp     InExcep,0
        jz      @@ok
        movzx   esi,DebugExceptionIndex
        add     esi,esi
        mov     eax,esi
        add     esi,esi
        add     esi,eax
        add     esi,offset OldExc00
        mov     eax,[esi]
        mov     d[InExcepJMP],eax
        mov     ax,[esi+4]
        mov     w[InExcepJMP+4],ax
        popm    eax,ebx,ecx,edx,esi,edi,ebp             ;save general registers.
        popm    ds,es,fs,gs
        assume ds:nothing
        jmp     cs:f[InExcepJMP]
        assume ds:_Excep
        ;
@@ok:   or      InExcep,-1
        push    ds
        pop     es
        assume es:_Excep
        mov     ax,ss
        mov     ds,ax
        assume ds:nothing
        mov     esi,esp
        test    es:ExcepSystemFlags,1
        jz      @@SP320
        movzx   esi,si
@@SP320:        mov     edi,offset DebugBuffer
        mov     ecx,(4+4+4+4+4+4+4)+(2+2+2+2)
        cld
        db 67h
        rep     movsb           ;copy registers off the stack.
        ;
        test    es:ExcepSystemFlags,1
        jz      @@Use32Bit17
        movzx   ebp,sp
        movzx   eax,w[ebp+(4+4+4+4+4+4+4)+(2+2+2+2)+(2+2)]
        mov     es:DebugExceptionCode,eax
        add     esi,2+2+2               ;skip return address/flags.
        db 67h
        movsw
        add     edi,2
        db 67h
        movsw
        add     edi,2
        db 67h
        movsw
        add     edi,2
        jmp     @@Use16Bit17
        ;
@@Use32Bit17:   mov     eax,[esp+(4+4+4+4+4+4+4)+(2+2+2+2)+(4+4)]
        mov     es:DebugExceptionCode,eax
        add     esi,4+4+4               ;skip return address/flags.
        mov     ecx,4+4+4
        cld
        db 67h
        rep     movsb           ;get real return address.
@@Use16Bit17:   ;
        test    es:ExcepSystemFlags,1
        jz      @@Use32Bit678
        movzx   eax,w[esi]
        mov     es:DebugESP,eax
        mov     ax,[esi+2]
        mov     es:DebugSS,ax
        jmp     @@Use16Bit678
        ;
@@Use32Bit678:  mov     eax,[esi]
        mov     es:DebugESP,eax
        mov     ax,[esi+4]
        mov     es:DebugSS,ax
@@Use16Bit678:  ;
        push    es
        pop     ds
        assume es:nothing
        assume ds:_Excep
        ;
        mov     DebugTR,0
        mov     DebugCR0,0
        mov     DebugCR2,0
        mov     DebugCR3,0
        ;
        ;Process segment registers to produce application relative base
        ;addresses.
        ;
        mov     ax,DebugCS
        mov     edi,offset DebugCSApp
        call    DebugSegmentDPMI
        mov     ax,DebugDS
        mov     edi,offset DebugDSApp
        call    DebugSegmentDPMI
        mov     ax,DebugES
        mov     edi,offset DebugESApp
        call    DebugSegmentDPMI
        mov     ax,DebugFS
        mov     edi,offset DebugFSApp
        call    DebugSegmentDPMI
        mov     ax,DebugGS
        mov     edi,offset DebugGSApp
        call    DebugSegmentDPMI
        mov     ax,DebugSS
        mov     edi,offset DebugSSApp
        call    DebugSegmentDPMI
        ;
        popm    eax,ebx,ecx,edx,esi,edi,ebp             ;save general registers.
        popm    ds,es,fs,gs
        ;
        pushm   eax,ebp,ds
        assume ds:nothing
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain
        mov     DebugDump,1
        mov     ErrorNumber,9
        ;
        mov     ebp,esp         ;make stack addresable.
        ;
        ;Now modify origional CS:EIP,SS:ESP values and return control
        ;to this code via interupt structure to restore stacks.
        ;
        test    SystemFlags,1
        jz      @@Use32_2
        mov     eax,offset @@Use0_2
        mov     w[bp+(4+4+2)+(2+2+2)+(0)],ax
        mov     w[bp+(4+4+2)+(2+2+2)+(2)],cs
        popm    eax,ebp,ds
        retf
        ;
@@Use32_2:      mov     eax,offset @@Use0_2
        mov     d[ebp+(4+4+2)+(4+4+4)+(0)],eax
        mov     w[ebp+(4+4+2)+(4+4+4)+(4)],cs
        popm    eax,ebp,ds
        db 66h
        retf
        ;
@@Use0_2:       assume ds:nothing
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain
        mov     ss,StackSegment
        mov     esp,offset _cwStackEnd-256
        pushm   ds,ds,ds
        popm    es,fs,gs
        jmp     f[TerminationHandler]
        assume ds:_Excep
DPMIExcPatch    endp


;-------------------------------------------------------------------------
;
;Convert segment value into real & application relative.
;
;On Entry:-
;
;AX     - Selector.
;DS:EDI - Buffer address.
;
DebugSegmentDPMI proc near
        push    ds
        assume ds:nothing
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain
        push    fs
        mov     fs,PSPSegment
        mov     ebx,-1
        ;
        push    eax
        mov     ax,fs
        movzx   eax,ax
        lsl     eax,eax
        cmp     eax,(size PSP_Struc)+(size EPSP_Struc)
        pop     eax
        jc      @@9
        ;
        mov     dx,ax
        cmp     ax,fs:w[EPSP_SegBase]   ;inside application selector space?
        jc      @@9
        mov     cx,fs:w[EPSP_SegSize]
;       shl     cx,3            ;8 bytes per selector.
        add     cx,fs:w[EPSP_SegBase]   ;add in base selector.
        cmp     ax,cx
        jnc     @@9             ;outside application startup selectors.
        mov     bx,dx
        pushm   cx,dx
        mov     ax,0006h
        int     31h
        mov     ax,cx
        shl     eax,16
        mov     ax,dx
        popm    cx,dx
        sub     eax,fs:d[EPSP_MemBase]  ;get offset within application.
        mov     ebx,eax
@@9:    pop     fs
        pop     ds
        mov     [edi],ebx               ;store generated value.
        assume ds:_Excep
        ret
DebugSegmentDPMI endp


;-------------------------------------------------------------------------
;
;Do a debug dump. Very rough but it works for now....
;
DebugDisplay    proc    far

IFDEF DEBUG4
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        push    cs
        pop     ds
        mov     edx,OFFSET debuggtext1
debuggloop2:
        cmp     BYTE PTR ds:[edx],0
        je      debuggb
        mov     ecx,1
        mov     bx,1
        mov     ah,40h
        int     21h
        inc     edx
        jmp     debuggloop2
debuggb:
        mov     edx,OFFSET debuggtext2
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
        jmp     debuggout

debuggtext1     DB      'Entering DebugDisplay...',0
debuggtext2     DB      13,10

debuggout:
        push    ecx
        mov     ecx,100000h
debuggloop:
;       dec     ecx
;       jne     debugaloop
        pop     ecx
ENDIF

        push    ds
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain
        cmp     UserTermFlag,0  ; see if user termination routine
        je      dd2                             ; no

INFOLISTSIZE    EQU     104

        xor     ecx,ecx
        mov     esi,OFFSET cs:DebugBuffer
        mov     edi,DWORD PTR UserTermDump
        cmp     UserTermFlag,1  ; see if 16-bit termination routine
        mov     UserTermFlag,0  ; reset flag, KEEP CPU FLAG STATUS
        jne     ut32                    ; not
        mov     ax,WORD PTR UserTermDump+2
        mov     cx,ax
        lsl     ecx,ecx
        jnz     utsi                    ; invalid selector
        mov     es,ax
        mov     cx,INFOLISTSIZE

uttrans16:
        mov     al,cs:[si]
        mov     es:[di],al
        inc     si
        inc     di
        dec     cx
        jne     uttrans16

utsi:
        mov     si,sp
        call    DWORD PTR ds:[UserTermRoutine]
        mov     ds,cs:ExcepDSeg ; restore ds if used
        jmp     dd2

ut32:
        mov     ax,WORD PTR UserTermDump+4
        mov     cx,ax
        lsl     ecx,ecx
        jnz     utesi                   ; invalid selector
        mov     es,ax
        mov     cx,INFOLISTSIZE

uttrans32:
        mov     al,cs:[esi]
        mov     es:[edi],al
        inc     esi
        inc     edi
        dec     cx
        jne     uttrans32

utesi:
        mov     esi,esp
        call    FWORD PTR ds:[UserTermRoutine]
        mov     ds,cs:ExcepDSeg ; restore ds if used

dd2:
        cmp     DebugDump,0             ;register dump needed?
        jz      @@9
        mov     DebugDump,0

        cmp     EnableDebugDump,0       ; see if debug dumping enabled
        je      @@9

        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        ;
        mov     ah,0fh
        int     10h
        cmp     al,3
        jz      @@ModeOk
        cmp     al,2
        jz      @@ModeOk
        cmp     al,7
        jz      @@ModeOk
        mov     ax,3
        int     10h
@@ModeOk:       ;
        mov     d[@@Handle],0
        xor     cx,cx
        mov     ah,3ch

; MED 06/18/96
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain
        mov     edx,OFFSET NewCWErrName ; try new error file name
        cmp     ds:[edx],al
        je      oldcwerr                ; no new error file name
        int     21h
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        jnc     debhand                 ; successfully created new error file

oldcwerr:
        mov     ds,cs:ExcepDDSeg
        assume ds:_Excep
        mov     edx,offset DebugName
        xor     cx,cx
        mov     ah,3ch
        int     21h
        jc      @@NoFile

debhand:
        mov     d[@@Handle],eax

@@NoFile:       ;Display debug info.
        ;
        push    ds
        mov     ds,ExcepDseg
        assume ds:_cwMain
        mov     esi,offset Copyright
        xor     ecx,ecx
@@LookCEnd:     cmp     b[esi],0
        jz      @@AtCEnd
        cmp     b[si],"$"
        jz      @@AtCEnd
        inc     ecx
        inc     esi
        jmp     @@LookCEnd
@@AtCEnd:       mov     edx,offset Copyright
        mov     ebx,d[@@Handle]
        mov     ah,40h
        int     21h
        assume ds:_Excep
        pop     ds
        ;
        mov     ax,DebugExceptionIndex
        mov     cx,2
        mov     edi,offset DebugINum
        call    Bin2Hex
        mov     ax,w[DebugExceptionCode]        ;high word undefined upto 486.
        mov     cx,4
        mov     edi,offset DebugENum
        call    Bin2Hex
        ;
        mov     ax,DebugTR
        mov     cx,4
        mov     edi,offset DebugTRt
        call    Bin2Hex
        mov     eax,DebugCR0
        mov     cx,8
        mov     edi,offset DebugCR0t
        call    Bin2Hex
        mov     eax,DebugCR2
        mov     cx,8
        mov     edi,offset DebugCR2t
        call    Bin2Hex
        mov     eax,DebugCR3
        mov     cx,8
        mov     edi,offset DebugCR3t
        call    Bin2Hex
        ;
        mov     eax,DebugEAX
        mov     cx,8
        mov     edi,offset DebugEAXt
        call    Bin2Hex
        mov     eax,DebugEBX
        mov     cx,8
        mov     edi,offset DebugEBXt
        call    Bin2Hex
        mov     eax,DebugECX
        mov     cx,8
        mov     edi,offset DebugECXt
        call    Bin2Hex
        mov     eax,DebugEDX
        mov     cx,8
        mov     edi,offset DebugEDXt
        call    Bin2Hex
        mov     eax,DebugESI
        mov     cx,8
        mov     edi,offset DebugESIt
        call    Bin2Hex
        mov     eax,DebugEDI
        mov     cx,8
        mov     edi,offset DebugEDIt
        call    Bin2Hex
        mov     eax,DebugEBP
        mov     cx,8
        mov     edi,offset DebugEBPt
        call    Bin2Hex
        mov     eax,DebugESP
        mov     cx,8
        mov     edi,offset DebugESPt
        call    Bin2Hex
        mov     eax,DebugEIP
        mov     cx,8
        mov     edi,offset DebugEIPt
        call    Bin2Hex
        mov     eax,DebugEFL
        mov     cx,8
        mov     edi,offset DebugEFLt
        call    Bin2Hex
        ;
        mov     ax,DebugCS
        mov     cx,4
        mov     edi,offset DebugCSt
        call    Bin2Hex
        mov     eax,DebugCSApp
        mov     cx,8
        mov     edi,offset DebugCSAppt
        cmp     eax,-1
        jnz     @@0
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@1
@@0:    call    Bin2Hex
@@1:    mov     ax,DebugDS
        mov     cx,4
        mov     edi,offset DebugDSt
        call    Bin2Hex
        mov     eax,DebugDSApp
        mov     cx,8
        mov     edi,offset DebugDSAppt
        cmp     eax,-1
        jnz     @@2
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@3
@@2:    call    Bin2Hex
@@3:    mov     ax,DebugES
        mov     cx,4
        mov     edi,offset DebugESt
        call    Bin2Hex
        mov     eax,DebugESApp
        mov     cx,8
        mov     edi,offset DebugESAppt
        cmp     eax,-1
        jnz     @@4
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@5
@@4:    call    Bin2Hex
@@5:    mov     ax,DebugFS
        mov     cx,4
        mov     edi,offset DebugFSt
        call    Bin2Hex
        mov     eax,DebugFSApp
        mov     cx,8
        mov     edi,offset DebugFSAppt
        cmp     eax,-1
        jnz     @@6
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@7
@@6:    call    Bin2Hex
@@7:    mov     ax,DebugGS
        mov     cx,4
        mov     edi,offset DebugGSt
        call    Bin2Hex
        mov     eax,DebugGSApp
        mov     cx,8
        mov     edi,offset DebugGSAppt
        cmp     eax,-1
        jnz     @@10
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@11
@@10:   call    Bin2Hex
@@11:   mov     ax,DebugSS
        mov     cx,4
        mov     edi,offset DebugSSt
        call    Bin2Hex
        mov     eax,DebugSSApp
        mov     cx,8
        mov     edi,offset DebugSSAppt
        cmp     eax,-1
        jnz     @@12
        mov     d[edi],'xxxx'
        mov     d[edi+4],'xxxx'
        add     edi,8
        jmp     @@13
@@12:   call    Bin2Hex
@@13:   ;
        mov     edi,offset DebugSysFlags
        push    ds
        mov     ds,ExcepDSeg
        assume ds:_cwMain
        movzx   eax,w[SystemFlags]
        assume ds:_Excep
        pop     ds
        mov     cx,8
        call    Bin2Hex


        ;
        ;Now print the results.
        ;

        push    ds
        mov     ds,ExcepDseg
        assume ds:_cwMain
        mov     esi,offset Copyright
@@cp0:  cmp     b[esi],0
        jz      @@cp1
        mov     dl,[esi]
        mov     ah,2
        int     21h
        inc     esi
        jmp     @@cp0
@@cp1:  assume ds:_Excep
        pop     ds


        mov     edx,offset DebugHeader
        mov     ah,9
        int     21h
        mov     edx,offset DebugHeader
        mov     ecx,offset DebugHeaderEnd-(DebugHeader+1+768)
        mov     ebx,d[@@Handle]
        mov     ah,40h
        int     21h

        ;
        ;Print message about writing cw.err
        ;
        mov     edx,offset WritingCWERR
        mov     ah,9
        int     21h

CSEIPDEBUGDUMPCOUNT     EQU     256     ; should be multiple of 16
SSESPDEBUGDUMPCOUNT     EQU     256     ; should be multiple of 16
SSEBPDEBUGDUMPCOUNT     EQU     256     ; should be multiple of 16

        push    fs

        mov     fs,ds:ExcepDSeg
        assume fs:_cwMain
        mov     fs,fs:PSPSegment
        assume fs:nothing
        mov     edi,offset DebugHeader
        mov     edx,OFFSET LoadAddrDebugText
        call    DebugTextCopy
        xor     edx,edx
        mov     eax,fs:[EPSP_MemBase]
        mov     cx,8
        call    Bin2Hex
        mov     DWORD PTR ds:[edi],0a0d0a0dh
        add     edi,4

        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h

        ;
        ;Do a CS:EIP dump.
        ;
        mov     ecx,CSEIPDEBUGDUMPCOUNT
        mov     fs,DebugCS
        mov     esi,DebugEIP
        test    ExcepSystemFlags,1
        jz      @@cseip32
        movzx   esi,si

@@cseip32:
        xor     eax,eax
        mov     ax,fs
        lsl     eax,eax
        cmp     eax,-1
        jz      @@flat0
        inc     eax

@@flat0:
        mov     ebx,esi
        add     ebx,ecx
        cmp     ebx,eax
        jc      @@cseip2
        sub     ebx,eax
        sub     ecx,ebx
        jz      @@cseip3
        js      @@cseip3

@@cseip2:
        ;
        mov     edi,offset DebugHeader
        mov     edx,OFFSET CSEIPDebugText
        call    DebugTextCopy
        xor     edx,edx
@@cseip0:
        push    ecx
        mov     ecx,2
        mov     al,fs:[esi]
        call    Bin2Hex
        pop     ecx
        mov     b[edi]," "
        inc     edi
        inc     esi
        inc     edx
        and     dl,0fh
        jnz     @@cseip1
        mov     WORD PTR ds:[edi],0a0dh
        add     edi,2

        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi
        ;
@@cseip1:
        dec     ecx
        jnz     @@cseip0
        mov     DWORD PTR ds:[edi],0a0d0a0dh
        add     edi,4
;       mov     b[edi],"$"
        ;
        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h

@@cseip3:

; Do user-defined dump, if appropriate
        mov     ds,cs:ExcepDSeg
        assume ds:_cwMain       ; allow access to user input variables

        movzx   ecx,WORD PTR DebugUserCount
        test    ecx,ecx         ; no user dump defined
        je      userdone
        mov     ax,DebugUserSel
        verr    ax
        jnz     userdone
        mov     fs,ax
        mov     esi,DebugUserOff
        mov     al,DebugAsciiFlag

        mov     ds,cs:ExcepDDSeg        ; restore ds for normal variable access
        assume ds:_Excep
        mov     DumpAsciiFlag,al

        test    ExcepSystemFlags,1
        jz      user32
        movzx   esi,si

user32:
        xor     eax,eax
        mov     ax,fs
        lsl     eax,eax
        cmp     eax,-1
        jz      userflat0
        inc     eax

userflat0:
        mov     ebx,esi
        add     ebx,ecx
        cmp     ebx,eax
        jc      user2
        sub     ebx,eax
        sub     ecx,ebx
        jz      userdone
        js      userdone

user2:
        mov     edi,offset DebugHeader
        mov     edx,OFFSET UserDebugText
        call    DebugTextCopy
        xor     edx,edx
user0:
        push    ecx
        mov     ecx,2
        mov     al,fs:[esi]
        cmp     DumpAsciiFlag,0 ; show as ASCII if so flagged
        je      userbin                 ; binary
        cmp     al,' '                  ; ensure that space or greater
        jae     userascii
        mov     al,'.'                  ; show a period for control characters

userascii:
        mov     BYTE PTR ds:[edi],al
        inc     edi
        jmp     usershow

userbin:
        call    Bin2Hex

usershow:
        mov     b[edi]," "
        pop     ecx
        inc     edi
        inc     esi
        inc     edx
        and     dl,0fh
        jnz     user1
        mov     WORD PTR ds:[edi],0a0dh
        add     edi,2

        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi

user1:
        dec     ecx
        jnz     user0
        mov     DWORD PTR ds:[edi],0a0d0a0dh
        add     edi,4

        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h

userdone:
        mov     ds,cs:ExcepDDSeg        ; restore ds for normal variable access
        assume ds:_Excep

ssesp:
        ;Do a SS:ESP dump.
        ;
        cmp     DebugExceptionIndex,0ch ;stack problem?
        jz      @@ssesp3
        mov     ecx,SSESPDEBUGDUMPCOUNT
        mov     fs,DebugSS
        mov     esi,DebugESP
        test    ExcepSystemFlags,1
        jz      @@ssesp32
        movzx   esi,si

@@ssesp32:
        xor     eax,eax
        mov     ax,fs
        lsl     eax,eax
        cmp     eax,-1
        jz      @@flat1
        inc     eax

@@flat1:
        mov     ebx,esi
        add     ebx,ecx
        cmp     ebx,eax
        jc      @@ssesp2
        sub     ebx,eax
        sub     ecx,ebx
        jz      @@ssesp3
        js      @@ssesp3

@@ssesp2:
        ;
        mov     edi,offset DebugHeader
        mov     edx,OFFSET SSESPDebugText
        call    DebugTextCopy
        xor     edx,edx

@@ssesp0:
        push    ecx
        mov     ecx,2
        mov     al,fs:[esi]
        call    Bin2Hex
        pop     ecx
        mov     b[edi]," "
        inc     edi
        inc     esi
        inc     edx
        test    dl,0fh
        jnz     @@ssesp1
        mov     WORD PTR ds:[edi],0a0dh
        add     edi,2

        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi
        ;
@@ssesp1:
        dec     ecx
        jnz     @@ssesp0

        mov     DWORD PTR ds:[edi],0a0d0a0dh
        add     edi,4
;       mov     b[edi],"$"
        ;
        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h

@@ssesp3:
        ;
        ;Do a SS:EBP dump.
        ;
        cmp     DebugExceptionIndex,0ch ;stack problem?
        jz      @@ssebp3

; do a pre-EBP dump for stack frames
        mov     ecx,SSEBPDEBUGDUMPCOUNT
        mov     fs,DebugSS
        mov     esi,DebugEBP
        test    esi,esi         ; see if any previous bytes
        je      ebpdump                 ; no, nothing to display
        cmp     esi,ecx                 ; see if stack frame as large as display byte dump
        jae     med2                    ; yes
        mov     ecx,esi

med2:
        sub     esi,ecx

        test    ExcepSystemFlags,1
        jz      medssebp32
        movzx   esi,si

medssebp32:
        xor     eax,eax
        mov     ax,fs
        lsl     eax,eax
        cmp     eax,-1
        jz      medflat2
        inc     eax

medflat2:
        mov     ebx,esi
        add     ebx,ecx
        cmp     ebx,eax
        jc      medssebp2
        sub     ebx,eax
        sub     ecx,ebx
        jz      ebpdump
        js      ebpdump

medssebp2:
        ;
        xor     edx,edx

        mov     edi,offset DebugHeader
        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi

        push    ecx
        and     ecx,0fh
        je      med3
        mov     dl,16                   ; high bytes known zero
        sub     dl,cl                   ; save hex bytes left to display on row
        mov     cl,dl

medloop:
        mov     ds:[edi],ax             ; 3-byte pads until at proper display position
        mov     ds:[edi+2],al
        add     edi,3
        dec     ecx
        jne     medloop

med3:
        pop     ecx

medssebp0:
        push    ecx
        mov     al,fs:[esi]
        mov     ecx,2
        call    Bin2Hex
        pop     ecx

        mov     b[edi]," "
        inc     edi
        inc     esi
        inc     edx
        test    dl,0fh
        jnz     medssebp1

        mov     WORD PTR ds:[edi],0a0dh
        add     edi,2
        cmp     ecx,1
        je      medssebp1
        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi

        ;
medssebp1:
        dec     ecx
        jne     medssebp0

;       mov     WORD PTR ds:[edi],0a0dh
;       add     edi,2
;       mov     b[edi],"$"
        ;
        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        ;

ebpdump:
        mov     ecx,SSEBPDEBUGDUMPCOUNT
        mov     fs,DebugSS
        mov     esi,DebugEBP
        test    ExcepSystemFlags,1
        jz      @@ssebp32
        movzx   esi,si

@@ssebp32:
        xor     eax,eax
        mov     ax,fs
        lsl     eax,eax
        cmp     eax,-1
        jz      @@flat2
        inc     eax

@@flat2:
        mov     ebx,esi
        add     ebx,ecx
        cmp     ebx,eax
        jc      @@ssebp2
        sub     ebx,eax
        sub     ecx,ebx
        jz      @@ssebp3
        js      @@ssebp3

@@ssebp2:
        ;
        mov     edi,offset DebugHeader
        mov     edx,OFFSET SSEBPDebugText
        call    DebugTextCopy
        xor     edx,edx

@@ssebp0:
        push    ecx
        mov     ecx,2
        mov     al,fs:[esi]
        call    Bin2Hex
        pop     ecx
        mov     b[edi]," "
        inc     edi
        inc     esi
        inc     edx
        test    dl,0fh
        jnz     @@ssebp1
        mov     WORD PTR ds:[edi],0a0dh
        add     edi,2

        mov     eax,20202020h
        mov     DWORD PTR ds:[edi],eax
        mov     DWORD PTR ds:[edi+4],eax
        add     edi,8
        mov     BYTE PTR ds:[edi],al
        inc     edi
        ;
@@ssebp1:
        dec     ecx
        jnz     @@ssebp0
        mov     DWORD PTR ds:[edi],0a0d0a0dh
        add     edi,4
;       mov     b[edi],"$"
        ;
        mov     edx,offset DebugHeader
        sub     edi,edx
        mov     ecx,edi
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        ;

@@ssebp3:

        pop     fs

        ;
        ;Do resource output stuff.
        ;
        push    fs
        mov     fs,ExcepDSeg
        assume fs:_cwMain
        mov     fs,fs:PSPSegment
        assume fs:nothing
        push    es
        pushad
        cmp     fs:d[EPSP_Resource],0
        jz      @@r8

        mov     edx,offset ResHeader
        mov     ah,40h
        mov     ecx,ResHeaderLen
        mov     ebx,d[@@Handle]
        int     21h

        mov     edx,offset SelHeader
        mov     ah,40h
        mov     ecx,SelHeaderLen
        mov     ebx,d[@@Handle]
        int     21h

        mov     es,ExcepDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     esi,fs:d[EPSP_Resource]
@@LookLoop:     push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@r0:   cmp     es:b[edi],Res_PSP
        jz      @@r1_0
        cmp     es:b[edi],Res_SEL
        jnz     @@r1
@@r1_0: mov     ax,fs:w[EPSP_DPMIMem]
        cmp     es:w[esi],ax
        jz      @@r1

        ;
@@SEL:  pushad
        mov     edi,offset DebugHeader
        mov     eax,es:[esi]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     ebx,es:[esi]
        sys     GetSelDet32
        push    ecx
        mov     eax,edx
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        pop     eax
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi]
        lar     eax,eax
        test    eax,0000100000000000b
        mov     ebx,'EDOC'
        jnz     @@dc0
        mov     ebx,'ATAD'
@@dc0:  mov     [edi],ebx
        add     edi,4
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi]
        lar     eax,eax
        mov     bx,"61"
        test    eax,00400000h
        jz      @@Use32It
        mov     bx,"23"
@@Use32It:      mov     [edi],bx
        add     edi,2
        mov     b[edi],' '
        inc     edi

        inc     TotalSelectors


        ;
        ;See if there is a memory block that matches this selector.
        ;
        mov     ebx,es:[esi]
        sys     GetSelDet32
        mov     ebx,edx
        pushad

        mov     esi,fs:d[EPSP_Resource]
@@s2:   push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@s0:   cmp     es:b[edi],Res_MEM       ;Anything here?
        jnz     @@s1

        mov     eax,fs:d[EPSP_INTMem]
        cmp     es:[esi],eax
        jz      @@s1
        cmp     fs:w[EPSP_DPMIMem],0
        jz      @@nodpmi
        pushad
        mov     bx,fs:w[EPSP_DPMIMem]
        sys     GetSelDet32
        cmp     es:[esi],edx
        popad
        jz      @@s1

@@nodpmi:       ;
        cmp     ebx,es:[esi]            ;Fits this block?
        jnz     @@s1
        pop     esi
        clc
        jmp     @@s3
        ;
@@s1:   add     esi,4
        inc     edi
        dec     ebp
        jnz     @@s0
        pop     esi
        mov     esi,es:[esi+8]  ;link to next list.
        or      esi,esi
        jnz     @@s2


        ;
        ;Have a look in the MCB list.
        ;
        cmp     fs:d[EPSP_mcbHead],0
        jz      @@nomcbsel
        mov     esi,fs:d[EPSP_mcbHead]
@@mcbs0:        mov     edi,esi
        add     esi,mcbChunkLen
@@mcbs1:        mov     eax,esi
        add     eax,mcbLen
        cmp     ebx,eax
        jnz     @@mcbs2
        clc
        jmp     @@s3
@@mcbs2:        cmp     es:b[esi+mcbNext],"M"
        jz      @@mcbs3
        mov     esi,es:[edi+mcbChunkNext]
        or      esi,esi
        jz      @@nomcbsel
        jmp     @@mcbs0
@@mcbs3:        movzx   eax,es:w[esi+mcbNextSize]
        add     eax,mcbLen
        add     esi,eax
        jmp     @@mcbs1


@@nomcbsel:     stc


@@s3:   popad


        jc      @@r4
        ;
        mov     b[edi],'Y'
        jmp     @@r5
        ;
@@r4:   mov     d[edi],'N'
        ;
@@r5:   inc     edi
        mov     b[edi],' '
        inc     edi
        ;
        mov     eax,es:[esi]
        mov     bx,fs:w[EPSP_SegBase]
        cmp     ax,bx
        jc      @@r2
        mov     bx,fs:w[EPSP_SegSize]
        add     bx,fs:w[EPSP_SegBase]
        cmp     ax,bx
        jnc     @@r2
        mov     bx,fs:w[EPSP_SegBase]
        shr     bx,3
        shr     ax,3
        sub     ax,bx
        inc     ax
        mov     ecx,4
        call    Bin2Hex
        jmp     @@r20
        ;
@@r2:   mov     d[edi],'xxxx'
        add     edi,4
        ;
@@r20:  mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        popad
        ;
@@r1:   add     esi,4
        inc     edi
        dec     ebp
        jnz     @@r0
        pop     edi
        mov     esi,es:[edi+8]  ;link to next list.
        or      esi,esi
        jnz     @@LookLoop
        ;


        mov     edi,offset TotalSelsNum
        mov     eax,TotalSelectors
        mov     ecx,4
        call    Bin2Hex
        mov     ecx,TotalSelsTLen
        mov     edx,offset TotalSelsText
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h





        ;
        ;Now do memory blocks.
        ;
        mov     edx,offset MemHeader
        mov     ah,40h
        mov     ecx,MemHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     esi,fs:d[EPSP_Resource]
        mov     es,ExcepDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
@@mLookLoop:    push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@m0:   cmp     es:b[edi],Res_MEM
        jnz     @@m1

        mov     eax,fs:d[EPSP_INTMem]
        cmp     es:[esi],eax
        jz      @@m1
        cmp     fs:w[EPSP_DPMIMem],0
        jz      @@nodpmimem
        pushad
        mov     bx,fs:w[EPSP_DPMIMem]
        sys     GetSelDet32
        cmp     es:[esi],edx
        popad
        jz      @@m1

        ;
@@nodpmimem:    ;Check if this is an MCB block.
        ;
        push    edi
        cmp     fs:d[EPSP_mcbHead],0
        jz      @@MEM
        mov     ebx,es:[esi]
        mov     edi,fs:d[EPSP_mcbHead]
@@mcbmc0:       cmp     ebx,edi
        jz      @@mcbmc1
        mov     edi,es:[edi+mcbChunkNext]
        or      edi,edi
        jz      @@MEM
        jmp     @@mcbmc0

        ;
@@mcbmc1:       ;Update Total memory value and skip this block.
        ;
        mov     eax,es:[esi+8]
        add     eax,4095
        and     eax,0fffff000h
        add     TotalLinearMem+4,eax
        pop     edi
        jmp     @@m1

@@MEM:  pop     edi
        pushad
        mov     edi,offset DebugHeader
        mov     eax,es:[esi+4]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi+8]
        add     TotalLinearMem,eax
        push    eax
        add     eax,4095
        and     eax,0fffff000h
        add     TotalLinearMem+4,eax
        pop     eax
        inc     TotalLinearMem+8
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        push    es
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        pop     es
        popad

        ;
@@m1:   add     esi,4
        inc     edi
        dec     ebp
        jnz     @@m0
        pop     edi
        mov     esi,es:[edi+8]  ;link to next list.
        or      esi,esi
        jnz     @@mLookLoop




        ;
        ;Now print MCB controlled blocks.
        ;
        cmp     fs:d[EPSP_mcbHead],0
        jz      @@nomcbdis
        mov     esi,fs:d[EPSP_mcbHead]
@@mcb0: mov     edi,esi
        add     esi,mcbChunkLen
@@mcb1: cmp     es:b[esi+mcbFreeUsed],"J"       ;Free block?
        jz      @@mcb2

        mov     eax,fs:d[EPSP_INTMem]
        sub     eax,mcbLen
        cmp     esi,eax
        jz      @@mcb2
        cmp     fs:w[EPSP_DPMIMem],0
        jz      @@nodpmimemhere
        pushad
        mov     bx,fs:w[EPSP_DPMIMem]
        sys     GetSelDet32
        sub     edx,mcbLen
        cmp     esi,edx
        popad
        jz      @@mcb2

@@nodpmimemhere:
        pushad
        mov     edi,offset DebugHeader
        mov     eax,esi
        add     eax,mcbLen
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,esi
        add     eax,mcbLen
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        movzx   eax,es:w[esi+mcbNextSize]
        inc     TotalLinearMem+8
        mov     cx,8
        push    eax
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        pop     eax
        add     TotalLinearMem,eax
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        push    es
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        pop     es

        popad

@@mcb2: cmp     es:b[esi+mcbNext],"M"   ;end of the chain?
        jz      @@mcb3
        mov     esi,es:[edi+mcbChunkNext]
        or      esi,esi
        jnz     @@mcb0
        jmp     @@nomcbdis
        ;
@@mcb3: movzx   eax,es:w[esi+mcbNextSize]
        add     eax,mcbLen
        add     esi,eax
        jmp     @@mcb1



        ;
@@nomcbdis:     ;Display totals.
        ;
        mov     edi,offset TotalMemNum1
        mov     eax,TotalLinearMem
        mov     ecx,8
        call    Bin2Hex
        mov     edi,offset TotalMemNum2
        mov     eax,TotalLinearMem+4
        mov     ecx,8
        call    Bin2Hex
        mov     edi,offset TotalMemNum3
        mov     eax,TotalLinearMem+8
        mov     ecx,8
        call    Bin2Hex
        mov     ecx,TotalMemTLen
        mov     edx,offset TotalMemText
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h






        ;
        ;Now do lock details.
        ;
        mov     edx,offset LockHeader
        mov     ah,40h
        mov     ecx,LockHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     esi,fs:d[EPSP_Resource]
@@lLookLoop:    push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@l0:   cmp     es:b[edi],Res_LOCK
        jnz     @@l1
        ;
@@LOCK: pushad
        mov     edi,offset DebugHeader
        mov     eax,es:[esi]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi+4]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        popad
        ;
@@l1:   add     esi,4
        inc     edi
        dec     ebp
        jnz     @@l0
        pop     edi
        mov     esi,es:[edi+8]  ;link to next list.
        or      esi,esi
        jnz     @@lLookLoop


        ;
        ;Now do DOS memory details.
        ;
        mov     edx,offset DosMemHeader
        mov     ah,40h
        mov     ecx,DosMemHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     esi,fs:d[EPSP_Resource]
@@dmLookLoop:   push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@dm0:  cmp     es:b[edi],Res_DOSMEM
        jnz     @@dm1
        ;
        pushad
        mov     edi,offset DebugHeader
        mov     eax,es:[esi]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        pushm   ebx,ecx,edx
        mov     bx,es:[esi]
        sys     GetSelDet32
        mov     eax,edx
        popm    ebx,ecx,edx
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi+4]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        popad
        ;
@@dm1:  add     esi,4
        inc     edi
        dec     ebp
        jnz     @@dm0
        pop     edi
        mov     esi,es:[edi+8]  ;link to next list.
        or      esi,esi
        jnz     @@dmLookLoop




        cmp     fs:d[EPSP_INTMem],0
        jz      @@r8

        ;
        ;Now do protected mode int details.
        ;
        mov     edx,offset PIntHeader
        mov     ah,40h
        mov     ecx,PIntHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     ecx,256
        mov     esi,fs:d[EPSP_INTMem]
        mov     ebx,0
@@pv0:  pushm   ebx,ecx,esi
        cmp     es:w[esi+4],-1
        jz      @@pv1
        ;
        mov     edi,offset DebugHeader
        mov     eax,ebx
        mov     cx,2
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     ax,es:w[esi+4]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:d[esi]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        ;
@@pv1:  popm    ebx,ecx,esi
        add     esi,6
        inc     ebx
        loop    @@pv0



        ;
        ;Now do protected mode exception details.
        ;
        mov     edx,offset EIntHeader
        mov     ah,40h
        mov     ecx,EIntHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     ecx,32
        mov     esi,fs:d[EPSP_INTMem]
        add     esi,256*6
        mov     ebx,0
@@pe0:  pushm   ebx,ecx,esi
        cmp     es:w[esi+4],-1
        jz      @@pe1
        ;
        mov     edi,offset DebugHeader
        mov     eax,ebx
        mov     cx,2
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     ax,es:w[esi+4]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:d[esi]
        mov     cx,8
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        ;
@@pe1:  popm    ebx,ecx,esi
        add     esi,6
        inc     ebx
        loop    @@pe0



        ;
        ;Now do real mode int details.
        ;
        mov     edx,offset RIntHeader
        mov     ah,40h
        mov     ecx,RIntHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     ecx,256
        mov     esi,fs:d[EPSP_INTMem]
        add     esi,(256*6)+(32*6)
        mov     ebx,0
@@ri0:  pushm   ebx,ecx,esi
        cmp     es:w[esi+2],-1
        jz      @@ri1
        ;
        mov     edi,offset DebugHeader
        mov     eax,ebx
        mov     cx,2
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     ax,es:w[esi+2]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     ax,es:w[esi]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        ;
@@ri1:  popm    ebx,ecx,esi
        add     esi,4
        inc     ebx
        loop    @@ri0



        ;
        ;Now do call-back details.
        ;
        mov     edx,offset CallBackHeader
        mov     ah,40h
        mov     ecx,CallBackHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        ;
        mov     esi,fs:d[EPSP_Resource]
@@cbLookLoop:   push    esi
        mov     ebp,ResNum              ;get number of entries.
        mov     edi,esi
        add     esi,ResHead+ResNum
        add     edi,ResHead
@@cb0:  cmp     es:b[edi],Res_CALLBACK
        jnz     @@cb1
        ;
        pushad
        mov     edi,offset DebugHeader
        mov     eax,es:[esi]
        shr     eax,16
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],':'
        inc     edi
        mov     eax,es:[esi]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],' '
        inc     edi
        mov     eax,es:[esi+8]
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],':'
        inc     edi
        mov     eax,es:[esi+4]
        mov     cx,8
        call    Bin2Hex
        ;
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h
        popad
        ;
@@cb1:  add     esi,4
        inc     edi
        dec     ebp
        jnz     @@cb0
        pop     edi
        mov     esi,es:[edi+8]  ;link to next list.
        or      esi,esi
        jnz     @@cbLookLoop

        ;
        ;Print mouse event target details.
        ;
        push    ds
        mov     ds,ExcepDSeg
        assume ds:_cwMain
        mov     eax,MouseETarget
        mov     ebx,MouseETarget+4
        assume ds:_Excep
        pop     ds
        mov     ecx,eax
        or      ecx,ebx
        jz      @@r8

        pushm   eax,ebx
        mov     edx,offset MouseEHeader
        mov     ah,40h
        mov     ecx,MouseEHeaderLen
        mov     ebx,d[@@Handle]
        int     21h
        popm    ebx,eax
        mov     edi,offset DebugHeader
        mov     cx,4
        call    Bin2Hex
        mov     b[edi],':'
        inc     edi
        mov     eax,ebx
        mov     ecx,8
        call    Bin2Hex
        mov     b[edi],13
        inc     edi
        mov     b[edi],10
        inc     edi
        mov     b[edi],13
        mov     b[edi+1],10
        mov     b[edi+2],'$'
        add     edi,2
        push    edi
        pop     ecx
        sub     ecx,offset DebugHeader
        mov     edx,offset DebugHeader
        mov     ah,40h
        mov     ebx,d[@@Handle]
        int     21h

@@r8:   popad
        pop     es
        pop     fs

        ;
        mov     ebx,d[@@Handle]
        mov     ah,3eh
        int     21h
        ;

@@9:    pop     ds
        retf
@@Handle:       ;
        dd ?
DebugDisplay    endp


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
Bin2Hex proc    near
        pushm   eax,ebx,ecx,edx,edi
        movzx   ecx,cx
        mov     ebx,offset HexTable
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
Bin2Hex endp


;-------------------------------------------------------------------------
BordExcep       proc    near
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
BordExcep       endp

;-------------------------------------------------------------------------
; copy debug text into output buffer
; upon entry ds:edi -> out buffer, ds:edx -> in buffer
;
DebugTextCopy   PROC    NEAR

dbdloop:
        mov     al,ds:[edx]
        test    al,al
        je      dbdret          ; null terminator flags end of text, don't transfer
        mov     ds:[edi],al
        inc     edx
        inc     edi
        jmp     dbdloop

dbdret:
        ret
DebugTextCopy   ENDP

;-------------------------------------------------------------------------
ExcepDSeg       dw ?
ExcepDDSeg      dw ?
ExcepSystemFlags dd ?
;
DebugBuffer     label byte
DebugEBP        dd ?
DebugEDI        dd ?
DebugESI        dd ?
DebugEDX        dd ?
DebugECX        dd ?
DebugEBX        dd ?
DebugEAX        dd ?
DebugGS dw ?
DebugFS dw ?
DebugES dw ?
DebugDS dw ?
DebugEIP        dd ?
DebugCS dw ?,?
DebugEFL        dd ?
DebugESP        dd ?
DebugSS dw ?,?
DebugTR dw ?
DebugCR0        dd ?
DebugCR1        dd ?
DebugCR2        dd ?
DebugCR3        dd ?
;
DebugCSApp      dd ?
DebugDSApp      dd ?
DebugESApp      dd ?
DebugFSApp      dd ?
DebugGSApp      dd ?
DebugSSApp      dd ?
;
DebugExceptionIndex dw 0
DebugExceptionCode dd 0
;
DebugName       db 'cw.err',0
;
ResHeader       db 13,10
        db '   Application resource tracking details   ',13,10
        db '===========================================',13,10,'$'
ResHeaderLen    equ     ($-1)-ResHeader
;
SelHeader       db 13,10,13,10
        db 'Selectors',13,10
        db '=========',13,10,13,10
        db ' sel   base     limit  type D mem count',13,10
        db '----------------------------------------',13,10,'$'
SelHeaderLen    equ ($-1)-SelHeader
;
TotalSelsText   db 13,10,"Total selectors: "
TotalSelsNum    db "0000",13,10
TotalSelsTLen   equ     $-TotalSelsText
;
MemHeader       db 13,10,13,10
        db 'Linear memory blocks',13,10
        db '====================',13,10,13,10
        db ' handle    base    length ',13,10
        db '--------------------------',13,10,'$'
MemHeaderLen    equ ($-1)-MemHeader
;
TotalMemText    db 13,10,"Total Linear memory: "
TotalMemNum1    db "00000000 ("
TotalMemNum2    db "00000000) in "
TotalMemNum3    db "00000000 blocks",13,10
TotalMemTLen    equ     $-TotalMemText
;
MouseEHeader    db 13,10,13,10
        db "Mouse event target: ","$"
MouseEHeaderLen equ ($-1)-MouseEHeader
DosMemHeader    db 13,10,13,10
        db "DOS memory blocks",13,10
        db "=================",13,10,13,10
        db " sel   base    length ",13,10
        db "----------------------",13,10,"$"
DosMemHeaderLen equ ($-1)-DosMemHeader
CallBackHeader  db 13,10,13,10
        db "Call-Backs",13,10
        db "==========",13,10,13,10
        db "  real        target",13,10
        db "-----------------------",13,10,"$"
CallBackHeaderLen equ ($-1)-CallBackHeader
LockHeader      db 13,10,13,10
        db 'Linear memory locked',13,10
        db '====================',13,10,13,10
        db '  base    length ',13,10
        db '-----------------',13,10,'$'
LockHeaderLen   equ ($-1)-LockHeader
;
PIntHeader      db 13,10,13,10
        db 'Protected mode interrupt vectors',13,10
        db '================================',13,10,13,10
        db 'No sel   offset ',13,10
        db '----------------',13,10,'$'
PIntHeaderLen   equ ($-1)-PIntHeader
;
EIntHeader      db 13,10,13,10
        db 'Protected mode exception vectors',13,10
        db '================================',13,10,13,10
        db 'No sel   offset ',13,10
        db '----------------',13,10,'$'
EIntHeaderLen   equ ($-1)-EIntHeader
;
RIntHeader      db 13,10,13,10
        db 'Real mode interrupt vectors',13,10
        db '===========================',13,10,13,10
        db 'No seg offset',13,10
        db '-------------',13,10,'$'
RIntHeaderLen   equ ($-1)-RIntHeader
;
TotalLinearMem  dd 0,0,0
TotalSelectors  dd 0
;
DebugHeader     db 13,10,'Exception: '
DebugINum       db '00, Error code: '
DebugENum       db '0000',13,10,13,10
        db 'EAX='
DebugEAXt       db '00000000 '
        db 'EBX='
DebugEBXt       db '00000000 '
        db 'ECX='
DebugECXt       db '00000000 '
        db 'EDX='
DebugEDXt       db '00000000 '
        db 'ESI='
DebugESIt       db '00000000 '
        db 13,10
        db 'EDI='
DebugEDIt       db '00000000 '
        db 'EBP='
DebugEBPt       db '00000000 '
        db 'ESP='
DebugESPt       db '00000000 '
        db 'EIP='
DebugEIPt       db '00000000 '
        db 'EFL='
DebugEFLt       db '00000000 '
        db 13,10,13,10
        db 'CS='
DebugCSt        db '0000-'
DebugCSAppt     db '00000000 '
        db 'DS='
DebugDSt        db '0000-'
DebugDSAppt     db '00000000 '
        db 'ES='
DebugESt        db '0000-'
DebugESAppt     db '00000000 '
        db 13,10
        db 'FS='
DebugFSt        db '0000-'
DebugFSAppt     db '00000000 '
        db 'GS='
DebugGSt        db '0000-'
DebugGSAppt     db '00000000 '
        db 'SS='
DebugSSt        db '0000-'
DebugSSAppt     db '00000000 '
        db 13,10,13,10
        db 'CR0='
DebugCR0t       db '00000000 '
        db 'CR2='
DebugCR2t       db '00000000 '
        db 'CR3='
DebugCR3t       db '00000000 '
        db 'TR='
DebugTRt        db '0000'
        db 13,10,13,10
        db 'Info flags='
DebugSysFlags   db '00000000 '
        db 13,10,13,10,'$'
        db      768 DUP (0)
DebugHeaderEnd  label byte
;
HexTable        db '0123456789ABCDEF'
;
InExcep db 0
InExcepJMP      df 0
;
WritingCWERR    db "Writing CW.ERR file....",13,10,13,10,36
;

CSEIPDebugText  DB      "CS:EIP > ",0
SSESPDebugText  DB      "SS:ESP > ",0
SSEBPDebugText  DB      "SS:EBP > ",0
UserDebugText   DB      "User   > ",0
LoadAddrDebugText       DB      "Program Linear Load Address: ",0
DumpAsciiFlag   DB      ?

ExcepEnd        label byte
_Excep  ends

        .286
