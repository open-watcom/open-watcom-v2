;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
        .386
_Int21h segment para public 'extension code' use32
        assume cs:_Int21h, ds:nothing, es:nothing
Int21hStart     label byte


;------------------------------------------------------------------------------
;
;DTA size for find first/next seems to be a little volatile so I'll use an EQU
;here incase it needs to change again.
;
DTASize equ     44-1


;------------------------------------------------------------------------------
;
;Some equates to make stacked register access simpler.
;
Int_EDI equ     0
Int_DI  equ     0
Int_ESI equ     4
Int_SI  equ     4
Int_EBP equ     8
Int_BP  equ     8
Int_ESP equ     12
Int_SP  equ     12
Int_EBX equ     16
Int_BX  equ     16
Int_BL  equ     16
Int_BH  equ     17
Int_EDX equ     20
Int_DX  equ     20
Int_DL  equ     20
Int_DH  equ     21
Int_ECX equ     24
Int_CX  equ     24
Int_CL  equ     24
Int_CH  equ     25
Int_EAX equ     28
Int_AX  equ     28
Int_AL  equ     28
Int_AH  equ     29
Int_GS  equ     32
Int_FS  equ     36
Int_ES  equ     40
Int_DS  equ     44
Int_Off equ     48
Int_Seg16       equ     50
Int_Seg32       equ     52
Int_Flags16     equ     52
Int_Flags32     equ     56


Int21h_repmovs  macro
        push    ecx
        shr     ecx,2
        rep     movsd           ;Copy the data.
        pop     ecx
        and     ecx,3
        rep     movsb
        endm


DOS4GExtend     macro p1
        local __0
        cmp     cs:Int21hDOS4GFlag,0
        jz      __0
        mov     p1,0
__0:    ;
        endm


;------------------------------------------------------------------------------
;
;The DOS function control code.
;
;All registers are stacked and EBP is setup to give access to the stacked
;values. Carry is cleared, interrupts are enabled if they we're enabled at
;entry. FS is set to the current PSP. Then the function specific handler is
;called.
;
;All this default setup means some handlers have unecesary processing overhead
;but the majority need it and it saves space.
;
;NOTES:
;
;None of the FCB related functions are modified.
;Media ID byte pointer of Get drive default data not yet implemented.
;
Int21h  proc    near
        pushm   ds,es,fs,gs             ;Preserve all registers.
        pushad          ;/
        push    eax
        movzx   eax,ah          ;Need extended register version.
        cmp     cs:d[Int21hTable+eax*4],offset Int21hNotOurs
        jz      Int21hNotOurs
        pop     eax
        mov     ebp,esp         ;Make registers addressable.
        mov     esi,Int_Flags32
        test    cs:Int21hSystemFlags,1  ;/
        jz      @@32bit0                ;/
        movzx   ebp,bp          ;/
        mov     esi,Int_Flags16
@@32Bit0:       add     esi,ebp
        and     ss:b[esi],not 1
        cld                     ;Default direction.
        test    ss:w[esi],1 shl 9       ;Were interrupts enabled?
        jz      @@NoInts
        sti                     ;Turn interrupts back on.
@@NoInts:       mov     fs,cs:Int21hDSeg
        assume fs:_cwMain
        mov     fs,fs:PSPSegment        ;Point to PSP.
        assume fs:nothing
        movzx   eax,ah          ;Need extended register version.
        call    cs:d[Int21hTable+eax*4] ;Pass control to handler.
        popad                   ;\
        popm    ds,es,fs,gs             ;Restore all registers.
        test    cs:Int21hSystemFlags,1
        jz      @@32Bit1
        iret
@@32Bit1:       iretd                   ;Return to caller.
Int21h  endp


;------------------------------------------------------------------------------
;
;Handler for functions that need to be passed to old DOS interrupt.
;
Int21hNotOurs   proc    near
        pop     eax             ;Lose return address.
        popad                   ;Restore registers.
        popm    ds,es,fs,gs
;       cli
        jmp     cs:f[OldInt21h] ;pass it onto previous handler.
Int21hNotOurs   endp


;------------------------------------------------------------------------------
Int21hInvalid   proc    near
        jmp     Int21hNotOurs
Int21hInvalid   endp


;------------------------------------------------------------------------------
;
;Function 09h emulation.
;
Int21hPrintString proc near
        mov     esi,[ebp+Int_EDX]       ;Point to source data.
        mov     ds,[ebp+Int_DS]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        mov     al,"$"          ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        mov     edx,ecx
;
;Now copy EPSP_TransSize-1 sized chunks into the transfer buffer
;and pass to the real mode handler.
;
@@0:    mov     ecx,edx         ;Get current count.
        cmp     ecx,fs:[EPSP_TransSize]
        jc      @@1
        mov     ecx,fs:[EPSP_TransSize] ;Use transfer buffer size -1.
        dec     ecx
@@1:    mov     es,fs:[EPSP_TransProt]  ;Point to transfer buffer.
        xor     edi,edi
        sub     edx,ecx         ;Update total counter.
        Int21h_repmovs          ;Copy this data.
        mov     es:b[edi],"$"   ;Terminate this string.
        ;
        ;Call the real mode handler to deal with this chunk.
        ;
        mov     es,cs:Int21hDSeg
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],0900h
        mov     es:Real_EDX[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        ;
        ;Keep looping till all data is done.
        ;
        or      edx,edx
        jnz     @@0
        ;
        ;All done so return to caller.
        ;
        ret
Int21hPrintString endp


;------------------------------------------------------------------------------
;
;Function 0A emulation.
;
Int21hGetString proc near
        mov     esi,[ebp+Int_EDX]       ;Point to source data.
        mov     ds,[ebp+Int_DS]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        mov     al,[esi]                ;Get length byte.
        mov     es,fs:[EPSP_TransProt]  ;Point to transfer buffer.
        mov     es:b[0],al              ;Store length byte.
        ;
        ;Call the real mode handler to read the string.
        ;
        mov     es,cs:Int21hDSeg
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],0A00h
        mov     es:Real_EDX[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        ;
        ;Copy returned string back into callers buffer.
        ;
        mov     edi,[ebp+Int_EDX]       ;Point to destination buffer.
        mov     es,[ebp+Int_DS]
        call    Int21hExtend_ES_EDI     ;Extend [E]DI.
        mov     ds,fs:[EPSP_TransProt]
        xor     esi,esi         ;Point to source data.
        movzx   ecx,b[esi+1]            ;get length of string read.
        add     ecx,1+1+1               ;max+len+eol.
        Int21h_repmovs          ;Copy this data.
        ;
        ;All done so return to caller.
        ;
        ret
Int21hGetString endp


;------------------------------------------------------------------------------
;
;Function 1A emulation.
;
Int21hSetDTA    proc    near
        mov     esi,[ebp+Int_EDX]       ;Point to source data.
        mov     ds,[ebp+Int_DS]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        mov     fs:d[EPSP_Dta],esi
        mov     fs:w[EPSP_Dta+4],ds     ;Store new DTA address.
        ret
Int21hSetDTA    endp


;------------------------------------------------------------------------------
;
;Function 25 emulation.
;
Int21hSetVect   proc    near
        mov     edx,[ebp+Int_EDX]
        mov     cx,[ebp+Int_DS]
        mov     bl,[ebp+Int_AL]
        sys     SetVect
        ret
Int21hSetVect   endp


;------------------------------------------------------------------------------
;
;Function 2F emulation.
;
Int21hGetDTA    proc    near
        mov     eax,fs:d[EPSP_DTA]      ;Copy current DTA address into
        mov     [ebp+Int_EBX],eax       ;return register storage.
        mov     ax,fs:w[EPSP_DTA+4]
        mov     [ebp+Int_ES],ax
        ret
Int21hGetDTA    endp


;------------------------------------------------------------------------------
;
;Function 35 emulation.
;
Int21hGetVect   proc    near
        mov     bl,[ebp+Int_AL]
        xor     edx,edx
        sys     GetVect
        mov     [ebp+Int_ES],cx
        mov     [ebp+Int_EBX],edx
        ret
Int21hGetVect   endp


;------------------------------------------------------------------------------
;
;Function 38 emulation.
;
Int21hGetSetCountry proc near
        test    cs:Int21hSystemFlags,1  ;16 or 32-bit -1 check?
        jz      @@32Bit0
        cmp     dx,-1           ;Setting code?
        jmp     @@ExitCheck             ;pass to old handler?
@@32Bit0:       cmp     edx,-1
@@ExitCheck:    jz      Int21hNotOurs   ;Go through normal stuff for SET.
        ;
        ;Call the real mode handler to take care of things.
        ;
        mov     es,cs:Int21hDseg
        mov     edi,offset Int21Buffer
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_EBX]
        mov     es:Real_EBX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     eax,es:Real_EBX[edi]
        mov     [ebp+Int_BX],ax
        DOS4GExtend w[ebp+Int_EBX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al
        jnz     @@9
        ;
        ;Copy returned info to callers buffer.
        ;
        mov     edi,[ebp+Int_EDX]       ;Point to source data.
        mov     es,[ebp+Int_DS]
        call    Int21hExtend_ES_EDI     ;Extend [E]DI.
        mov     ds,fs:[EPSP_TransProt]
        xor     esi,esi
        mov     ecx,22h
        rep     movsb
        ;
        ;Get re-map function address.
        ;
        mov     es,cs:Int21hDDSeg
        mov     edi,offset Int21hCountryTab
        xor     esi,esi
        mov     ax,w[esi+12h]   ;Fetch offset.
        mov     es:Real_IP[edi],ax
        mov     ax,w[esi+14h]   ;Fetch selector.
        mov     es:Real_CS[edi],ax
        ;
        ;Set new re-map function address.
        ;
        mov     eax,offset @@RemapCall
        mov     w[esi+12h],ax
        mov     w[esi+14h],cs
        ;
@@9:    ret
;
;The remap handler.
;
@@RemapCall:    pushm   edi,es
        mov     es,cs:Int21hDDSeg
        mov     edi,offset Int21hCountryTab
        mov     es:Real_EAX[edi],eax
        push    es:Real_CS[edi]
        push    es:Real_IP[edi]
        sys     FarCallReal
        pop     es:Real_IP[edi]
        pop     es:Real_CS[edi]
        mov     eax,es:Real_EAX[edi]
        popm    edi,es
        db 66h
        retf
Int21hGetSetCountry endp


;------------------------------------------------------------------------------
;
;Function 39 emulation.
;
Int21hCreateDIR proc near
        ;
        ;Copy string into transfer buffer.
        ;
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        inc     ecx             ;Include terminator.
        rep     movsb
        ;
        ;Pass control to real mode handler.
        ;
        mov     es,cs:Int21hDseg
        mov     edi,offset Int21Buffer
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        mov     eax,es:Real_EAX[edi]    ;Get return code.
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        ret
Int21hCreateDIR endp


;------------------------------------------------------------------------------
;
;Function 3C emulation.
;
Int21hCreateFile proc near
        ;
        ;Copy string into transfer buffer.
        ;
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        inc     ecx             ;Include terminator.
        rep     movsb
        ;
        ;Pass control to real mode handler.
        ;
        mov     es,cs:Int21hDseg
        mov     edi,offset Int21Buffer
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_ECX]
        mov     es:Real_ECX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        mov     eax,es:Real_EAX[edi]    ;Get return code.
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     eax,es:Real_ECX[edi]
        mov     [ebp+Int_CX],ax
        DOS4GExtend w[ebp+Int_ECX+2]
        ret
Int21hCreateFile endp


;------------------------------------------------------------------------------
;
;Function 3D emulation.
;
Int21hOpenFile  proc    near
        ;
        ;Copy string into transfer buffer.
        ;
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        inc     ecx             ;Include terminator.
        rep     movsb
        ;
        ;Pass control to real mode handler.
        ;
        mov     es,cs:Int21hDseg
        mov     edi,offset Int21Buffer
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        mov     eax,es:Real_EAX[edi]    ;Get return code.
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        ret
Int21hOpenFile  endp


;------------------------------------------------------------------------------
;
;Function 3F emulation.
;
Int21hReadFile  proc    near
        mov     ds,[ebp+Int_DS] ;Point to source.
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        mov     es,cs:Int21hDSeg
        mov     edi,offset Int21Buffer
        movzx   edx,w[ebp+Int_CX]       ;Get length.
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@4
        mov     edx,[ebp+Int_ECX]       ;Get length.
@@4:    xor     ebx,ebx         ;Reset length read.
@@0:    mov     ecx,edx
        cmp     ecx,fs:[EPSP_TransSize]
        jc      @@1
        mov     ecx,fs:[EPSP_TransSize]
@@1:    mov     es:Real_ECX[edi],ecx    ;Store length.
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     ax,[ebp+Int_AX]
        mov     es:Real_EAX[edi],eax
        mov     ax,[ebp+Int_BX] ;Set handle.
        mov     es:Real_EBX[edi],eax
        push    ebx
        mov     bl,21h
        sys     IntXX
        pop     ebx
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al           ;Carry set?
        jz      @@2
        mov     ebx,es:Real_EAX[edi]    ;get return code.
        jmp     @@3
@@2:    mov     eax,es:Real_EAX[edi]    ;get bytes read.
        movzx   eax,ax
        sub     edx,eax         ;Update count remaining.
        add     ebx,eax         ;Update count so far.
        push    ecx
        mov     ecx,eax
        pushm   edi,es
        pushm   esi,ds
        popm    edi,es
        mov     ds,fs:[EPSP_TransProt]
        xor     esi,esi
        Int21h_repmovs          ;Copy this data.
        pushm   edi,es
        popm    esi,ds
        popm    edi,es
        pop     ecx
        cmp     eax,ecx
        jnz     @@3
        or      edx,edx
        jnz     @@0
@@3:    mov     [ebp+Int_AX],bx ;store length or return code.
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@5
        mov     [ebp+Int_EAX],ebx
@@5:    ret
Int21hReadFile  endp


;------------------------------------------------------------------------------
;
;Function 40 emulation.
;
Int21hWriteFile proc near
        mov     ds,[ebp+Int_DS] ;Point to source.
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        mov     es,cs:Int21hDSeg
        mov     edi,offset Int21Buffer
        movzx   edx,w[ebp+Int_CX]       ;Get length.
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@4
        mov     edx,[ebp+Int_ECX]       ;Get length.
@@4:    xor     ebx,ebx         ;Reset length read.
@@0:    mov     ecx,edx
        cmp     ecx,fs:[EPSP_TransSize]
        jc      @@1
        mov     ecx,fs:[EPSP_TransSize]
@@1:    mov     es:Real_ECX[edi],ecx    ;Store length.
        mov     es:Real_EDX[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     ax,[ebp+Int_AX]
        mov     es:Real_EAX[edi],eax
        mov     ax,[ebp+Int_BX] ;Set handle.
        mov     es:Real_EBX[edi],eax
        pushm   ecx,edi,es
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        Int21h_repmovs          ;Copy this data.
        popm    ecx,edi,es
        push    ebx
        mov     bl,21h
        sys     IntXX           ;Do the write.
        pop     ebx
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al           ;Carry set?
        jz      @@2
        mov     ebx,es:Real_EAX[edi]    ;get return code.
        jmp     @@3
@@2:    mov     eax,es:Real_EAX[edi]    ;get bytes read.
        movzx   eax,ax
        sub     edx,eax         ;Update count remaining.
        add     ebx,eax         ;Update count so far.
        cmp     eax,ecx         ;Get anything?
        jnz     @@3
        or      edx,edx
        jnz     @@0
@@3:    mov     [ebp+Int_AX],bx ;store length or return code.
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@5
        mov     [ebp+Int_EAX],ebx
@@5:    ret
Int21hWriteFile endp


;------------------------------------------------------------------------------
;
;Function 44 emulation.
;
Int21hIOCTLDispatch proc near
        mov     al,[ebp+Int_AL]
        cmp     al,02h          ;IOCTL?
        jz      Int21hReadFile
        cmp     al,03h
        jz      Int21hWriteFile
        cmp     al,04h
        jz      Int21hReadFile
        cmp     al,05h
        jz      Int21hWriteFile
        jmp     Int21hNotOurs
Int21hIOCTLDispatch endp


;------------------------------------------------------------------------------
;
;Function 47 emulation.
;
Int21hGetCurDir proc near
        ;
        ;Call the real mode handler to take care of things.
        ;
        mov     es,cs:Int21hDseg
        mov     edi,offset Int21Buffer
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_EDX]
        mov     es:Real_EDX[edi],eax
        mov     es:Real_ESI[edi],0
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al
        jnz     @@9
        ;
        ;Copy returned info to callers buffer.
        ;
        mov     ds,fs:[EPSP_TransProt]  ;Point to source data.
        xor     esi,esi
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,[ebp+Int_DS]
        mov     edi,[ebp+Int_ESI]
        call    Int21hExtend_ES_EDI     ;Extend [E]SI.
        rep     movsb
@@9:    ret
Int21hGetCurDir endp


;------------------------------------------------------------------------------
;
;Function 48 emulation.
;
Int21hAllocMem  proc    near
        cmp     bx,-1           ;maximum free check?
        jz      @@AllocMuch
        movzx   ebx,bx
        shl     ebx,4           ;convert paragraphs to bytes.
        mov     dx,bx
        shr     ebx,16
        mov     cx,bx
        sys     GetMem          ;try to allocate memory.
        jc      @@AllocMuch             ;report how much free then.
        mov     [ebp+Int_AX],bx ;get the selector allocated.
        DOS4GExtend w[ebp+Int_EAX+2]
        ret
        ;
@@AllocMuch:    mov     cx,-1
        mov     dx,-1
        sys     GetMem          ;get free memory size.
        mov     bx,cx
        shl     ebx,16
        mov     bx,dx
        cmp     ebx,0fffeh*16   ;can't report too much.
        jc      @@AllocOK
        mov     ebx,0fffeh*16
@@AllocOK:      shr     ebx,4           ;convert to paragraphs free.
        mov     [ebp+Int_BX],ax
        DOS4GExtend w[ebp+Int_EBX+2]
        mov     w[ebp+Int_AX],1
        DOS4GExtend w[ebp+Int_EAX+2]
        call    Int21hAL2Carry  ;Set carry.
        ret
Int21hAllocMem  endp


;------------------------------------------------------------------------------
;
;Function 49 emulation.
;
Int21hRelMem    proc    near
        mov     bx,[ebp+Int_ES]
        mov     ds,[ebp+Int_DS]
        mov     es,[ebp+Int_ES]
        mov     fs,[ebp+Int_FS]
        mov     gs,[ebp+Int_GS]
        sys     RelMem
        mov     [ebp+Int_DS],ds
        mov     [ebp+Int_ES],es
        mov     [ebp+Int_FS],fs
        mov     [ebp+Int_GS],gs
        ret
Int21hRelMem    endp


;------------------------------------------------------------------------------
;
;Function 4A emulation.
;
Int21hResMem    proc    near
        movzx   ebx,w[ebp+Int_BX]
        shl     ebx,4           ;convert paragraphs to bytes.
        mov     dx,bx
        shr     ebx,16
        mov     cx,bx
        mov     bx,[ebp+Int_ES]
        sys     ResMem
        pushf
        pop     ax
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        ret
Int21hResMem    endp


;------------------------------------------------------------------------------
;
;Function 4B emulation.
;
Int21hExecFile  proc    near
        cmp     b[ebp+Int_AL],0 ;We only support sub-function 0.
        jz      @@OK
@@Done: call    Int21hAL2Carry  ;Set carry.
        ret
        ;
@@OK:   ;Copy the file name into transfer buffer.
        ;
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        rep     movsb           ;Copy the file name.
        ;
        ;Copy the command line into transfer buffer.
        ;
        mov     ebx,[ebp+Int_EBX]
        mov     ds,[ebp+Int_ES]
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@0
        lds     esi,f[ebx+4+2]
        jmp     @@Ef4
@@0:    test    cs:Int21hSystemFlags,1
        jz      @@Ef3
        movzx   ebx,bx
        movzx   esi,w[ebx+2]            ;Get command line offset.
        mov     ds,[ebx+2+2]            ;& segment.
        jmp     @@Ef4
@@Ef3:  mov     esi,[ebx+2]
        mov     ds,[ebx+2+4]
@@Ef4:  mov     es,fs:[EPSP_TransProt]
        mov     edi,256
        movzx   ecx,b[esi]              ;get command line length.
        inc     ecx             ;include length byte.
        rep     movsb
        mov al,0dh
        stosb
        mov al,0
        stosb
        ;
        ;Copy the FCB's
        ;
        if      0
        pushm   ecx,esi,edi,ds,es
        push    ds
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Ef6
        movzx   ebx,bx
        movzx   edi,es:w[ebx+(2)+(2+2)] ;Get FCB 1 offset.
        mov     es,es:[ebx+(2)+(2+2)+2] ;& segment.
        jmp     @@Ef7
@@Ef6:  mov     edi,es:[ebx+(2)+(4+2)]
        mov     es,es:[ebx+(2)+(4+2)+4]
@@Ef7:  mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        lds     esi,TransferBuffer
        assume ds:nothing
        add     esi,512
        pushm   esi,edi,ds,es
        popm    edi,esi,es,ds
        mov     ecx,10h
        cld
        rep     movsb
        popm    ecx,esi,edi,ds,es
        ;
        pushm   ecx,esi,edi,ds,es
        push    ds
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        test    SystemFlags,1
        assume ds:nothing
        pop     ds
        jz      @@Ef8
        movzx   ebx,bx
        movzx   edi,es:w[ebx+(2)+(2+2)+(2+2)] ;Get FCB 2 offset.
        mov     es,es:[ebx+(2)+(2+2)+(2+2)+2] ;& segment.
        jmp     @@Ef9
@@Ef8:  mov     edi,es:[ebx+(2)+(4+2)+(4+2)]
        mov     es,es:[ebx+(2)+(4+2)+(4+2)+4]
@@Ef9:  mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        lds     esi,TransferBuffer
        assume ds:nothing
        add     esi,512+16
        pushm   esi,edi,ds,es
        popm    edi,esi,es,ds
        mov     ecx,10h
        cld
        rep     movsb
        popm    ecx,esi,edi,ds,es
        endif

        ;
        ;Sort out the environment.
        ;
        mov     edi,[ebp+Int_EBX]
        mov     es,[ebp+Int_ES]
        call    Int21hExtend_ES_EDI     ;Extend [E]SI.
        mov     ebx,edi
        xor     esi,esi
        cmp     cs:Int21hDOS4GFlag,0
        jz      @@1
        cmp     es:w[ebx+4],0
        jz      @@2
        lds     esi,es:f[ebx]
        jmp     @@ef12
@@1:    cmp     es:w[ebx],0             ;got an environment?
        jnz     @@Ef10
@@2:    mov     ds,fs:w[PSP_Environment]        ;Get current environment.
        jmp     @@Ef12
@@Ef10: mov     ds,es:[ebx]             ;get environment segment.
@@Ef12: push    esi
@@3:    lodsb
        or      al,al
        jnz     @@3
        cmp     b[esi],0                ;double zero?
        jnz     @@3
        inc     esi
        inc     esi
        pop     eax
        push    eax
        sub     esi,eax
        mov     ecx,esi
        add     esi,256         ;make space for execution path.
        add     esi,15
        shr     esi,4           ;get paragraphs needed.
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     es:Real_EBX[edi],esi
        mov     es:Real_EAX[edi],4800h
        pop     esi
        mov     bl,21h
        sys     IntXX           ;allocate this memory.
        mov     eax,es:Real_EAX[edi]    ;get segment address.
        test    es:Real_Flags[edi],1
        mov di,ax
        mov al,1
        jnz     @@Ef13
        movzx   edi,di
        push    edi
        shl     edi,4
        mov     es,cs:Int21hDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        rep     movsb           ;Copy environment.
        pop     edx


;
;Patch INT 21h exec function to preserve the stack.
;
        pushad
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        cmp     NoEXECPatchFlag,0
        jne     medex1
        mov     bl,21h
        mov     ax,200h
        int     31h
        mov     w[OldInt21hExec],dx
        mov     w[OldInt21hExec+2],cx
        mov     dx,offset Int21hExecPatch
        mov     cx,_cwMain
        mov     bl,21h
        mov     ax,201h

IFNDEF DEBUG5
        int     31h
ENDIF

medex1:
        assume ds:nothing
        popad

        ;
        ;Setup real mode parameter block.
        ;
        mov     ax,fs:w[EPSP_TransReal]
        mov     ds,fs:w[EPSP_TransProt]
        mov     esi,512+32
        mov     w[esi],dx               ;set environment segment.
        mov     w[esi+2],256            ;command tail offset.
        mov     w[esi+4],ax             ;command tail segment.
        mov     w[esi+6],512            ;FCB 1 offset.
        mov     w[esi+8],ax             ;FCB 1 segment.
        mov     w[esi+10],512+16        ;FCB 2 offset.
        mov     w[esi+12],ax            ;FCB 2 segment.
        ;
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     es:Real_EAX[edi],4b00h
        mov     es:Real_EBX[edi],512+32
        mov     es:Real_EDX[edi],0
        mov     es:Real_DS[edi],ax
        mov     es:Real_ES[edi],ax
        mov     bl,21h
        sys     IntXX
;
;Restore INT 21h patch.
;
        pushad
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        cmp     NoEXECPatchFlag,0
        jne     medex2
        mov     bl,21h
        mov     dx,w[OldInt21hExec]
        mov     cx,w[OldInt21hExec+2]
        mov     d[OldInt21hExec],0
        mov     ax,201h

IFNDEF DEBUG5
        int     31h
ENDIF

medex2:
        assume ds:nothing
        popad

        push es:Real_Flags[edi] ;save flags.
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax ;store errorlevel.
        DOS4GExtend w[ebp+Int_EAX+2]
        ;
        or      dx,dx
        jz      @@Ef15          ;No environment to release.
        mov     es:Real_ES[edi],dx
        mov     es:Real_EAX[edi],4900h  ;release dos memory.
        mov     bl,21h
        sys     IntXX
        ;
@@Ef15: ;Put the DTA back where it's supposed to be.
        ;
        mov     es:Real_EDX[edi],offset DTABuffer
        mov     es:Real_DS[edi],_cwMain
        mov     es:Real_EAX[edi],1a00h
        mov     bl,21h
        sys     IntXX
        ;
        pop     ax
@@Ef13:
        and     ax,1
        call    Int21hAL2Carry
        ret
Int21hExecFile  endp


;------------------------------------------------------------------------------
;
;Function 4E emulation.
;
Int21hFindFirstFile proc near

        mov     es,cs:Int21hDseg        ;point to new source.
        mov     edi,offset DtaBuffer    ;use the real DTA buffer.
        lds     esi,fs:f[EPSP_DTA]      ;point to user buffer.
        mov     ecx,DTASize             ;length to copy.
        rep     movsb           ;copy data returned.

        mov     esi,[ebp+Int_EDX]       ;Point to source data.
        mov     ds,[ebp+Int_DS]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx             ;Include terminator.
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        rep     movsb
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_ECX]
        mov     es:Real_ECX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.

;       or      al,al
;       jnz     @@9

        mov     ds,cs:Int21hDseg        ;point to new source.
        mov     esi,offset DtaBuffer    ;use the real DTA buffer.
        les     edi,fs:f[EPSP_DTA]      ;point to user buffer.
        mov     ecx,DTASize             ;length to copy.
        rep     movsb           ;copy data returned.
@@9:    ret
Int21hFindFirstFile endp


;------------------------------------------------------------------------------
;
;Function 4F emulation.
;
Int21hFindNextFile proc near
        mov     es,cs:Int21hDseg        ;point to new source.
        mov     edi,offset DtaBuffer    ;use the real DTA buffer.
        lds     esi,fs:f[EPSP_DTA]      ;point to user buffer.
        mov     ecx,DTASize             ;length to copy.
        rep     movsb           ;copy data returned.
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDseg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
;       or      al,al
;       jnz     @@9
        mov     ds,cs:Int21hDseg        ;point to new source.
        mov     esi,offset DtaBuffer    ;use the real DTA buffer.
        les     edi,fs:f[EPSP_DTA]      ;point to user buffer.
        mov     ecx,DTASize             ;length to copy.
        rep     movsb           ;copy data returned.
@@9:    ret
Int21hFindNextFile endp


;------------------------------------------------------------------------------
;
;Function 50 emulation.
;
Int21hSetPSP    proc    near
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        mov     ax,[ebp+Int_BX]
        mov     PSPSegment,ax   ;Set current PSP.
        assume ds:nothing
        ret
Int21hSetPSP    endp


;------------------------------------------------------------------------------
;
;Function 51 emulation.
;
Int21hGetPSP    proc    near
        mov     [ebp+Int_BX],fs
        ret
Int21hGetPSP    endp


;------------------------------------------------------------------------------
;
;Function 56 emulation.
;
Int21hRenameFile proc near
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        push    ecx
        rep     movsb
        pop     edi
        ;
        mov     ds,[ebp+Int_ES]
        mov     esi,[ebp+Int_EDI]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,fs:[EPSP_TransProt]
        push    edi
        rep     movsb
        pop     edx
        ;
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     ax,fs:[EPSP_TransReal]
        mov     es:Real_EDX[edi],0
        mov     es:Real_EDI[edi],edx
        mov     es:Real_DS[edi],ax
        mov     es:Real_ES[edi],ax
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     bl,21h
        sys     IntXX

; return error status, MED 03/13/96
        mov     ax,WORD PTR es:Real_EAX[edi]
        mov     w[ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]

        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.

        ret
Int21hRenameFile endp


;------------------------------------------------------------------------------
;
;Function 5A emulation.
;
Int21hCreateTemp proc near
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_EDX]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,fs:[EPSP_TransProt]
        xor     edi,edi
        rep     movsb
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_ECX]
        mov     es:Real_ECX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al
        jnz     @@9
        mov     ds,fs:[EPSP_TransProt]
        xor     esi,esi
        xor     al,al           ;Character to look for.
        call    Int21hStringLen ;Get length of this string.
        inc     ecx
        mov     es,[ebp+Int_DS]
        mov     edi,[ebp+Int_EDX]
        call    Int21hExtend_ES_EDI     ;Extend [E]SI.
        rep     movsb
@@9:    ret
Int21hCreateTemp endp


;------------------------------------------------------------------------------
;
;Function 5E emulation.
;
Int21hMSNet     proc    near
        cmp     ax,5e00h
        jnz     Int21hNotOurs
        ;
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDseg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     es:Real_EDX[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX           ;read pen values.
        mov     eax,es:Real_ECX[edi]
        mov     [ebp+Int_CX],ax
        DOS4GExtend w[ebp+Int_ECX+2]
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        or      al,al
        jnz     @@9
        mov     ds,fs:w[EPSP_TransProt]
        xor     esi,esi
        mov     es,[ebp+Int_ES]
        mov     edi,[ebp+Int_EDX]
        call    Int21hExtend_ES_EDI     ;Extend [E]SI.
        mov     ecx,16/4
        rep     movsd           ;copy the results.
@@9:    ret
Int21hMSNet     endp


;------------------------------------------------------------------------------
;
;Function 67 emulation.
;
Int21hSetHandles proc near
;
;Set handle count with DOS.
;
        mov     es,cs:Int21hDSeg
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],6700h
        mov     ax,[ebp+Int_BX]
        mov     es:Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX
        test    es:Real_Flags[edi],1
        jz      @@0
        mov     al,1
        call    Int21hAL2Carry  ;Set carry.
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        ret
;
;Change succeded so modify handle table selector base.
;
@@0:    mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        movzx   eax,RealPSPSegment
        shl     eax,4
        mov     ds,RealSegment
        assume ds:nothing
        movzx   edx,ds:w[PSP_HandlePtr+2+eax]
        shl     edx,4
        movzx   ebx,ds:w[PSP_HandlePtr+eax]
        add     edx,ebx
        movzx   ecx,ds:w[PSP_Handles+eax]
        mov     bx,fs:w[PSP_HandlePtr+2]
        sys     SetSelDet32
;
;Now modify all PSP handle counts.
;
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        mov     esi,BasePSPAddress
        mov     ds,RealSegment
        assume ds:nothing
        mov     bx,[ebp+Int_BX]
@@1:    mov     PSP_Handles[esi],bx
        mov     esi,EPSP_NextPSP[esi]
        or      esi,esi
        jnz     @@1
        ret




        if      0
        mov     bx,[ebp+Int_BX]
        cmp     bx,fs:w[PSP_Handles]    ;more or less?
        jz      @@sh0
        jnc     @@shb0
        ;
        ;Want less handles!
        ;
        mov     ax,fs
        cmp     ax,fs:w[PSP_HandlePtr+2]        ;table still in PSP?
        jz      @@sh0
        ;
        ;Current table not in PSP.
        ;
        cmp     bx,20+1         ;20 is the lower limit.
        jnc     @@shs0
        ;
        ;Reduceing to 20 handles so move the table back into the PSP.
        ;
        push    fs
        pop     es
        mov     edi,offset PSP_HandleList
        mov     ecx,20
        mov     al,-1
        rep     stosb           ;initialise the table first.
        mov     edi,offset PSP_HandleList
        lds     si,fs:d[PSP_HandlePtr]  ;point to current list.
        movzx   esi,si
        mov     ecx,20
        rep     movsb           ;copy 1st 20 entries.
        jmp     @@shs1
        ;
@@shs0: ;Still more than 20 handles so just re-size the memory.
        ;
        push    bx
        movzx   ecx,bx          ;1 byte per entry.
        mov     bx,fs:w[PSP_HandlePtr+2]
        sys     ResMem32                ;re-size the memory.
        pop     bx
        jc      @@sh0           ;not sure if this can happen.
        ;
@@shs1: ;Now update the the DOS side of things and the PSP.
        ;
        mov     ax,bx
        sub     ax,fs:w[PSP_Handles]    ;find out how many less than before.
        mov     fs:w[PSP_Handles],bx    ;set new number of handles.
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        push    ds
        pop     es
        sub     TotalHandles,ax ;update total count needed.
        mov     ax,TotalHandles
        assume ds:nothing
        or      ax,1
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],6700h
        mov     es:Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX           ;set new value with DOS.
        jmp     @@sh0
        ;
@@shb0: ;Want more handles.
        ;
        mov     ax,bx
        sub     ax,fs:w[PSP_Handles]    ;find out how many more.
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        push    ds
        pop     es
        add     ax,TotalHandles ;get total count needed.
        assume ds:nothing
        or      ax,1
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],6700h
        mov     es:Real_EBX[edi],eax
        mov     ecx,ebx
        mov     bl,21h
        sys     IntXX
        mov     ebx,ecx
        mov     eax,es:Real_EAX[edi]
        test    es:Real_Flags[edi],1
        jnz     @@sh0           ;no can do!
        ;
        ;We managed to set the new number with DOS, now try and get the
        ;memory needed to take care of things in protected mode.
        ;
        mov     ax,fs
        cmp     ax,fs:w[PSP_HandlePtr+2]        ;table still in PSP?
        jnz     @@shb1
        ;
        ;Current table is in the PSP still so we want a fresh block
        ;of memory and a new selector.
        ;
        movzx   ecx,bx          ;1 byte per entry.
        sys     GetMem32                ;allocate the memory.
        jc      @@shb2
        push    es
        mov     es,bx           ;point to new memory.
        xor     edi,edi
        mov     al,-1
        push    ecx
        movzx   ecx,cx
        rep     stosb           ;initialise table to nothing.
        xor     edi,edi
        push    ds
        lds     si,fs:d[PSP_HandlePtr]
        movzx   esi,si
        mov     cx,fs:w[PSP_Handles]
        movzx   ecx,cx
        rep     movsb           ;copy existing entries.
        pop     ds
        pop     ecx
        mov     fs:w[PSP_HandlePtr],0
        mov     fs:w[PSP_HandlePtr+2],es
        pop     es
        mov     ax,cx
        sub     ax,fs:w[PSP_Handles]    ;find out how many more.
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        add     TotalHandles,ax
        assume ds:nothing
        mov     fs:w[PSP_Handles],cx    ;set new number.
        xor     ax,ax
        clc
        jmp     @@sh0
        ;
@@shb1: ;Current table is NOT in the PSP so just do a re-size on the
        ;memory.
        ;
        movzx   ecx,bx          ;1 byte per handle.
        mov     bx,fs:w[PSP_HandlePtr+2]
        sys     ResMem32                ;try and get adjusted memory.
        jc      @@shb2
        sub     cx,fs:w[PSP_Handles]    ;get number differance.
        mov     di,fs:w[PSP_Handles]    ;get origional value.
        mov     al,-1
        push    ecx
        push    es
        movzx   edi,di
        movzx   ecx,cx
        mov     es,bx
        rep     stosb           ;initialise new entries.
        pop     es
        pop     ecx
        sub     ax,fs:w[PSP_Handles]    ;find out how many more.
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        add     TotalHandles,ax
        assume ds:nothing
        mov     fs:w[PSP_Handles],cx    ;set new number.
        xor     ax,ax
        clc
        jmp     @@sh0
        ;
@@shb2: ;Couldn't get the memory at the protected mode end of things
        ;so we need to put the DOS version back to its origional size.
        ;
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        mov     ax,TotalHandles ;get total count needed.
        assume ds:nothing
        push    ds
        pop     es
        or      ax,1
        mov     edi,offset Int21Buffer
        mov     es:Real_EAX[edi],6700h
        mov     es:Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX
        stc
        ;
@@sh0:  ;Finaly we pass our results back to the caller.
        ;
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        pushf
        pop     ax
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        ret
        endif
Int21hSetHandles endp


;------------------------------------------------------------------------------
;
;Function 6C emulation.
;
Int21hExtendOpen proc near
        mov     ds,[ebp+Int_DS]
        mov     esi,[ebp+Int_ESI]
        call    Int21hExtend_DS_ESI     ;Extend [E]SI.
        xor     al,al
        call    Int21hStringLen
        inc     ecx
        mov     es,fs:w[EPSP_TransProt]
        xor     edi,edi
        rep     movsb
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_EBX]
        mov     es:Real_EBX[edi],eax
        mov     eax,[ebp+Int_ECX]
        mov     es:Real_ECX[edi],eax
        mov     eax,[ebp+Int_EDX]
        mov     es:Real_EDX[edi],edx
        mov     es:Real_ESI[edi],0
        mov     ax,fs:w[EPSP_TransReal]
        mov     es:Real_DS[edi],ax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     eax,es:Real_ECX[edi]
        mov     [ebp+Int_CX],ax
        DOS4GExtend w[ebp+Int_ECX+2]
        mov     ax,es:Real_Flags[edi]
        and     al,1
        call    Int21hAL2Carry  ;Set carry.
        ret
Int21hExtendOpen endp


;------------------------------------------------------------------------------
;
;Function FF, DOS4G detection.
;
Int21hDOS4GTest proc near
;
;This check only allowed for 32-bit programs which is what DOS4G uses.
;
        test    cs:Int21hSystemFlags,1
        jnz     Int21hNotOurs
;
;Check register setup.
;
        cmp     b[ebp+Int_AL],0 ;AX=0FF00h
        jnz     Int21hNotOurs
        cmp     w[ebp+Int_DX],78h       ;DX=0078h
        jnz     Int21hNotOurs

; changed for dummy segment modification
;       mov     w[ebp+Int_GS],ds

        mov     d[ebp+Int_EAX],4734ffffh

;
;Flag DOS4G mode for INT handling.
;
        push    ds
        push    eax
        push    ebx
        push    ecx
        mov     ecx,ds                  ; save application DS value
        assume ds:nothing
        mov     ds,cs:Int21hDDSeg
        assume ds:_Int21h

; give Watcom a dummy segment for it to stuff silly __D16SegInfo
; values in without trashing things, MED 12/12/95
        mov     ax,DOS16DummySegment
        test    ax,ax
        jne     setgs
        push    ecx                     ; save application DS value

; MED 07/15/97
DUMMYSEGMENTSIZE        EQU     48h     ; must be multiple of four
;       mov     ecx,44h                 ; dummy segment size
        mov     ecx,DUMMYSEGMENTSIZE

        sys     GetMem32                ; allocate the memory
        jnc     zerodummy               ; no error allocating memory
        pop     ebx
        push    ebx                     ; use application DS value if can't create dummy value
        jmp     savedummy

; MED 07/15/97, zero out dummy segment
zerodummy:
        push    edx
        push    ds
        mov     ds,bx
        mov     ecx,DUMMYSEGMENTSIZE-4

dumloop:
        mov     DWORD PTR ds:[ecx],0
        sub     ecx,4
        jns     dumloop
        pop     ds
        pop     edx

savedummy:
        add     esp,4                   ; throw away application DS value
        mov     DOS16DummySegment,bx
        mov     eax,ebx                 ; allocated memory or application DS selector to put in GS

setgs:
        mov     WORD PTR [ebp+Int_GS],ax

        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        or      DOS4GFlag,-1
        assume ds:nothing
        mov     ds,cs:Int21hDDSeg
        assume ds:_Int21h
        or      Int21hDOS4GFlag,-1
        assume ds:nothing
        pop     ecx
        pop     ebx
        pop     eax
        pop     ds
;
        ret
Int21hDOS4GTest endp


;------------------------------------------------------------------------------
;
;Get extended error information.
;
Int21hGetErrorInfo proc near
        mov     edi,offset Int21Buffer
        mov     es,cs:Int21hDSeg
        mov     eax,[ebp+Int_EAX]
        mov     es:Real_EAX[edi],eax
        mov     eax,[ebp+Int_EBX]
        mov     es:Real_EBX[edi],eax
        mov     bl,21h
        sys     IntXX
        mov     eax,es:Real_EAX[edi]
        mov     [ebp+Int_AX],ax
        DOS4GExtend w[ebp+Int_EAX+2]
        mov     eax,es:Real_EBX[edi]
        mov     [ebp+Int_BX],ax
        DOS4GExtend w[ebp+Int_EBX+2]
        mov     eax,es:Real_ECX[edi]
        mov     [ebp+Int_CX],ax
        DOS4GExtend w[ebp+Int_ECX+2]
;
;LABEL string?
;
        cmp     w[ebp+Int_AX],22h
        jnz     @@done
        mov     ds,cs:Int21hDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        assume ds:nothing
        mov     eax,es:Real_EDI[edi]
        movzx   eax,ax
        movzx   esi,es:Real_ES[edi]
        shl     esi,4
        add     esi,eax
        mov     es,fs:[EPSP_TransProt]
        mov     [ebp+Int_ES],es
        mov     w[ebp+Int_DI],0
        DOS4GExtend w[ebp+Int_EDI+2]
        xor     edi,edi
        mov     ecx,11
        rep     movsb
;
@@done: ret
Int21hGetErrorInfo endp


;------------------------------------------------------------------------------
;
;Extend SI into ESI if DS's limit is < 64K
;
;On Entry:
;
;DS:[E]SI       - Pointer to convert.
;
;On Exit:
;
;DS:ESI - Valid [extended] pointer.
;
;All other registers preserved.
;
Int21hExtend_DS_ESI proc near
        push    eax
        xor     eax,eax
        mov     ax,ds
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@32Bit
        movzx   esi,si
@@32Bit:        pop     eax
        ret
Int21hExtend_DS_ESI endp


;------------------------------------------------------------------------------
;
;Extend DI into EDI if ES's limit is < 64K
;
;On Entry:
;
;ES:[E]DI       - Pointer to convert.
;
;On Exit:
;
;ES:EDI - Valid [extended] pointer.
;
;All other registers preserved.
;
Int21hExtend_ES_EDI proc near
        push    eax
        xor     eax,eax
        mov     ax,es
        lsl     eax,eax
        cmp     eax,10000h
        jnc     @@32Bit
        movzx   edi,di
@@32Bit:        pop     eax
        ret
Int21hExtend_ES_EDI endp


;------------------------------------------------------------------------------
;
;Fing length of a string.
;
;On Entry:
;
;AL     - Termination value.
;DS:ESI - String to find length of.
;
;On Exit:
;
;ECX    - Length of string EXCLUDEING terminator.
;
;All other registers preserved.
;
Int21hStringLen proc near
        pushm   edi,es
        push    ds
        pop     es
        mov     edi,esi
        or      ecx,-1
        repne   scasb
        mov     ecx,edi
        sub     ecx,esi
        dec     ecx
        popm    edi,es
        ret
Int21hStringLen endp


;------------------------------------------------------------------------------
Int21hAL2Carry  proc    near
        test    cs:Int21hSystemFlags,1
        jz      @@32Bit
        or      b[ebp+Int_Flags16],al
        ret
@@32Bit:        or      b[ebp+Int_Flags32],al
        ret
Int21hAL2Carry  endp


;------------------------------------------------------------------------------
;
;The jump table used to pass control to the right routine.
;
Int21hTable     label dword
        dd 8 dup (Int21hNotOurs)        ;00-07
        ;
        dd Int21hNotOurs
        dd Int21hPrintString    ;09
        dd Int21hGetString              ;0A
        dd 4 dup (Int21hNotOurs)        ;0B-0E
        dd Int21hInvalid                ;0F
        ;
        dd 8 dup (Int21hInvalid)        ;10-17
        ;
        dd 2 dup (Int21hNotOurs)        ;18-19
        dd Int21hSetDTA         ;1A
        dd 5 dup (Int21hNotOurs)        ;1B-1F
        ;
        dd Int21hNotOurs                ;20
        dd 4 dup (Int21hInvalid)        ;21-24
        dd Int21hSetVect                ;25
        dd 2 dup (Int21hInvalid)        ;26-27
        ;
        dd 2 dup (Int21hInvalid)        ;29
        dd 5 dup (Int21hNotOurs)        ;2A-2E
        dd Int21hGetDTA         ;2F
        ;
        dd 5 dup (Int21hNotOurs)        ;30-34
        dd Int21hGetVect                ;35
        dd 2 dup (Int21hNotOurs)        ;36-37
        ;
        dd Int21hGetSetCountry  ;38
        dd Int21hCreateDIR              ;39
        dd Int21hCreateDIR              ;3A
        dd Int21hCreateDIR              ;3B
        dd Int21hCreateFile     ;3C
        dd Int21hOpenFile               ;3D
        dd Int21hNotOurs                ;3E
        dd Int21hReadFile               ;3F
        ;
        dd Int21hWriteFile              ;40
        dd Int21hOpenFile               ;41
        dd Int21hNotOurs                ;42
        dd Int21hCreateFile     ;43
        dd Int21hIOCTLDispatch  ;44
        dd 2 dup (Int21hNotOurs)        ;45-46
        dd Int21hGetCurDir              ;47
        ;
        dd Int21hAllocMem               ;48
        dd Int21hRelMem         ;49
        dd Int21hResMem         ;4A
        dd Int21hExecFile               ;4B
        dd 2 dup (Int21hNotOurs)        ;4C-4D
        dd Int21hFindFirstFile  ;4E
        dd Int21hFindNextFile   ;4F
        ;
        dd Int21hSetPSP         ;50
        dd Int21hGetPSP         ;51
        dd 4 dup (Int21hNotOurs)        ;52-55
        dd Int21hRenameFile     ;56
        dd Int21hNotOurs                ;57
        ;
        dd Int21hNotOurs                ;58
        dd Int21hGetErrorInfo   ;59
        dd Int21hCreateTemp     ;5A
        dd Int21hCreateFile     ;5B
        dd 2 dup (Int21hNotOurs)        ;5C-5D
        dd Int21hMSNet          ;5E
        dd Int21hNotOurs                ;5F
        ;
        dd 2 dup (Int21hNotOurs)        ;60-61
        dd Int21hGetPSP         ;62
        dd 4 dup (Int21hNotOurs)        ;63-66
        dd Int21hSetHandles     ;67
        ;
        dd 4 dup (Int21hNotOurs)        ;68-6B
        dd Int21hExtendOpen     ;6C
        dd 3 dup (Int21hNotOurs)        ;6D-6F
        ;
        dd (16*9)-1 dup (Int21hNotOurs) ;70-FE
        ;
        dd Int21hDOS4GTest              ;FF


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Setup int 21h patch.
;
Int21hOpen      proc    near
        assume ds:_Int21h
        assume es:_cwMain
        mov     Int21hDSeg,es   ;Store cwCode selector.
        mov     Int21hCSeg,cs   ;store this segment.
        mov     Int21hDDSeg,ds
        ;
        mov     bl,21h
        sys     GetVect
        test    es:SystemFlags,1
        jz      @@Use32
        movzx   edx,dx
@@Use32:        mov     d[OldInt21h],edx
        mov     w[OldInt21h+4],cx
@@Use0: mov     edx,offset Int21h
        mov     cx,cs
        mov     bl,21h
        sys     SetVect
        ;
        mov     eax,es:SystemFlags
        mov     Int21hSystemFlags,eax
        ;
        assume es:nothing
        assume ds:nothing
        clc
@@9:    ;
        db 66h
        retf
Int21hOpen      endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Remove int 21h patch.
;
Int21hClose     proc    near
        push    ds
        mov     ds,cs:Int21hDDSeg
        assume ds:_Int21h
        cmp     d[OldInt21h+2],0
        jz      @@9
        mov     edx,d[OldInt21h]
        mov     cx,w[OldInt21h+4]
        mov     bl,21h
        sys     SetVect
        assume ds:nothing
@@9:    pop     ds
        ;
        db 66h
        retf
Int21hClose     endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bord21  proc    near
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
Bord21  endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
OldInt21h       df 0
Int21hCSeg      dw ?
Int21hDSeg      dw ?
Int21hDDSeg     dw ?
Int21hSystemFlags dd ?
;
Int21hCountryTab db size RealRegsStruc dup (?)
;
Int21hDOS4GFlag db 0
;
DOS16DummySegment       DW      0
Int21hEnd       label byte
_Int21h ends

        .286
