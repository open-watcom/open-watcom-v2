        include system.inc
        include cw.inc
        include strucs.inc


_Seg_   struc
Seg_BaseAddress dd ?
Seg_Type        dd ?
Seg_Length      dd ?
Seg_Memory      dd ?
_Seg_   ends


        .code


;------------------------------------------------------------------------------
_ASMMain        proc    near
        public _ASMMain
        pushs   esi,edi,ebp
        mov     StackStore,esp
        ;
        mov     esi,offset Copyright    ;Anounce what we are.
        calls   PrintString,esi
        ;
        call    ReadCommand             ;Get command line and options.
        mov     ErrorNumber,1
        cmp     eax,1           ;Got a file name?
        jc      ASMExit
        ;
        cmp     OptionPointers+4,0
        jnz     @@gotoutname
        mov     eax,OptionPointers
        mov     OptionPointers+4,eax
@@gotoutname:   ;
        ;Add .EXE to input name if needed.
        ;
IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug6
        mov     ecx,SIZEOF debug6
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med6
debug6  DB      'Before strcpy 1',13,10
med6:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        callc   strcpy,offset InNameSpace,OptionPointers

IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug7
        mov     ecx,SIZEOF debug7
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med7
debug7  DB      'After strcpy 1',13,10
med7:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        mov     OptionPointers,offset InNameSpace
        mov     esi,OptionPointers
        xor     edi,edi
@@e0:   inc     esi
        cmp     b[esi-1],0
        jz      @@e2
        cmp     b[esi-1],"."
        jz      @@e1
        cmp     b[esi-1],"\"
        jnz     @@e0
        xor     edi,edi
        jmp     @@e0
@@e1:   mov     edi,esi
        jmp     @@e0
@@e2:   or      edi,edi
        jnz     @@e3
        mov     b[esi-1],"."
        mov     b[esi],"E"
        mov     b[esi+1],"X"
        mov     b[esi+2],"E"
        mov     b[esi+3],0
@@e3:

        ;Add .EXE to output name if needed.
        ;
IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug4
        mov     ecx,SIZEOF debug4
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med4
debug4  DB      'Before strcpy 2',13,10
med4:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        callc   strcpy,offset OutNameSpace,OptionPointers+4

IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug5
        mov     ecx,SIZEOF debug5
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med5
debug5  DB      'After strcpy 2',13,10
med5:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        mov     OptionPointers+4,offset OutNameSpace
        mov     esi,OptionPointers+4
        xor     edi,edi
@@e20:  inc     esi
        cmp     b[esi-1],0
        jz      @@e22
        cmp     b[esi-1],"."
        jz      @@e21
        cmp     b[esi-1],"\"
        jnz     @@e20
        xor     edi,edi
        jmp     @@e20
@@e21:  mov     edi,esi
        jmp     @@e20
@@e22:  or      edi,edi
        jnz     @@e23
        mov     b[esi-1],"."
        mov     b[esi],"E"
        mov     b[esi+1],"X"
        mov     b[esi+2],"E"
        mov     b[esi+3],0
@@e23:

IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug8
        mov     ecx,SIZEOF debug8
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med8
debug8  DB      'Calling Fetch...',13,10
med8:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        call    FetchLEFile             ;Read the LE file.
        jc      ASMExit
        ;
        call    Create3PFile            ;Create the 3P image.
        jc      ASMExit
        ;
        call    CreateRelocations       ;Build relocation list.
        jc      ASMExit
        ;
        call    Create3PHeader  ;Build the header.
        jc      ASMExit
        ;
        call    Write3PFile             ;Now write the 3P file.
        jc      ASMExit
        ;
        mov     ErrorNumber,0
        jmp     ASMExit
_ASMMain        endp


;------------------------------------------------------------------------------
ASMExit proc    near
        mov     eax,ErrorNumber
        mov     esi,[ErrorList+eax*4]
        calls   PrintString,esi
;
        mov     esp,StackStore
        pops    esi,edi,ebp
        ret
ASMExit endp


;------------------------------------------------------------------------------
;
;Create a 3P format relocation table from the LE fixup tables.
;
CreateRelocations proc near
        mov     esi,offset BuildRelocsText
        calls   PrintString,esi
IFDEF DEBUG2
;       int     3
ENDIF
        ;
        ;Setup a pointer to the object definitions.
        ;
        mov     RelocationCount,0
        ;
        mov     esi,LEAddress
        mov     ecx,[esi+44h]   ;Get number of Objects.
        add     esi,[esi+40h]   ;Point to object table.
        mov     ObjectCount,ecx
        mov     ObjectBase,esi
        mov     eax,ObjectList
        add     eax,4
        mov     SegmentList,eax
        ;
@@0:    mov     esi,ObjectBase
        mov     ecx,[esi+10h]   ;Get number of pages.

; MED 09/06/97
        test ecx,ecx
        je      mednextobj2

        mov     PageCount,ecx
        mov     PageCount+4,0
        mov     edx,[esi+0Ch]   ;Get page table index.
        dec     edx
        mov     ebp,edx         ;Set base page map entry.
@@1:    ;
        mov     edx,ebp
        mov     esi,LEAddress
        add     esi,[esi+68h]   ;Point to fixup page maps.
        mov     ecx,[esi+4+edx*4]       ;Get next offset.
        mov     edx,[esi+edx*4] ;Get start offset.
        sub     ecx,edx         ;Get number of bytes
        jz      @@4
        ;
        mov     esi,LEAddress
        add     esi,[esi+6Ch]   ;Point to fixup data.
        add     esi,edx         ;Move to start of this pages fixups.
        ;
@@2:    mov     al,[esi]                ;Get type byte.
        mov     bl,al
        shr     bl,4            ;Get single/multiple flag.
        mov     bh,al
        and     bh,15           ;Get type.
        inc     esi
        dec     ecx
        mov     al,[esi]                ;Get second type byte.
        mov     dl,al
        and     dl,3            ;Get internal/external specifier.
        mov     dh,al
        shr     dh,2            ;Get destination type.
        inc     esi
        dec     ecx
        ;
        or      bl,bl           ;check for single entry
        jz      @@multok
        cmp     bl,1
        jz      @@multok
        mov     ErrorNumber,9
        jmp     @@9
@@multok:       ;
        mov     ErrorNumber,10
        or      dl,dl           ;Check it's an internal target.
        jnz     @@9

        mov     ErrorNumber,13
        test    dh,111011b              ;Check for unknown bits.

IFNDEF DEBUG2
        jnz     @@9
ELSE
        jz      med2
        int     3
med2:
ENDIF

        cmp     bh,0010b                ;Word segment?
        jz      @@Seg16
        cmp     bh,0111b                ;32-bit offset?
        jz      @@32BitOff
        cmp     bh,0110b                ;Seg:32-bit offset?
        jz      @@Seg1632BitOff
        cmp     bh,1000b                ;32-bit self relative?
        jz      @@Self32Off
        cmp     bh,0101b                ;16-bit offset?
        jz      @@16BitOff
        cmp bh,0011b            ;16-bit dword?
        jz @@16bitdword

; MED 12/23/96
        cmp     bh,1                    ; ignore fixup ???
        je      @@3

        mov     ErrorNumber,11
        jmp     @@9

@@16bitdword:
        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1.
        ;w[esi+3] - target offset
        ;
        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg160
        add     edi,eax         ;Point to the right offset.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        mov     [edi+2],ax              ;Store target.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_BaseAddress[edi]        ;Get offset from image start.
        mov     eax,PageCount+4 ;Get page number within segment.
        shl     eax,12
        add     edi,eax         ;Include page offset.
        movzx   eax,w[esi]
        add     edi,eax         ;Include byte offset.
        add edi,2
        mov     eax,edi
        call    AddRelocationEntry      ;Add it to the list.
        mov     ErrorNumber,6
        jc      @@9
        ;
        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg160
        add     edi,eax         ;Point to the right offset.
        movzx   eax,w[esi+3]            ;Get target offset.
        mov     [edi],ax
@@Neg160:

        add     esi,2+1+2
        sub     ecx,2+1+2
        jmp     @@3

@@16BitOff:

        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1.
        ;w[esi+3]

        mov     ErrorNumber,13

; MED 12/27/96
;       test    dh,4
;       jnz     @@9

        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg3
        add     edi,eax         ;Point to the right offset.
        movzx   eax,w[esi+3]            ;Get target offset.
        test    dh,4                    ;Target offset may be 32-bit!
        jz      @@Big3
        mov     ebx,[esi+3]             ;Get target offset.
@@Big3: add     eax,ebx
        mov     [edi],ax
@@Neg3: ;
        add     esi,2+1+2
        sub     ecx,2+1+2
        test    dh,4
        jz      @@3
        add     esi,2
        sub     ecx,2
        jmp     @@3

@@Seg16:        ;Deal with a 16-bit segment.
        ;
        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1.
        ;
        mov     ErrorNumber,13
        test    dh,4

IFNDEF DEBUG2
        jnz     @@9
ELSE
        jz      med3
        int     3
med3:
ENDIF

COMMENT !
        jz      med2
        mov     cx,8
bfloop:
        rol     dh,1
        mov     bl,dh
        and     dh,1
        mov     dl,dh
        add     dl,30h
        mov     ah,2
        int     21h
        mov     dh,bl
        dec     cx
        jne     bfloop
        mov     ax,4c00h
        int     21h
med2:
END COMMENT !

        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg0
        add     edi,eax         ;Point to the right offset.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        mov     [edi],ax                ;Store target.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_BaseAddress[edi]        ;Get offset from image start.
        mov     eax,PageCount+4 ;Get page number within segment.
        shl     eax,12
        add     edi,eax         ;Include page offset.
        movzx   eax,w[esi]
        add     edi,eax         ;Include byte offset.
        mov     eax,edi
        call    AddRelocationEntry      ;Add it to the list.
        mov     ErrorNumber,6
        jc      @@9
        ;
@@Neg0: add     esi,2+1
        sub     ecx,2+1
        jmp     @@3
        ;
@@32BitOff:     ;Deal with a 32-bit offset.
        ;
        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1
        ;w[esi+3] - target offset.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg1
        add     edi,eax         ;Point to the right offset.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        push    edx
        mov     edx,size _Seg_
        mul     edx
        pop     edx
        add     eax,4           ;skip dword count.
        add     eax,ObjectList  ;point to target segment details.
        mov     eax,Seg_BaseAddress[eax]        ;Get target segments offset from start of image.
        movzx   ebx,w[esi+3]            ;Get target offset.
        test    dh,4
        jz      @@Big0
        mov     ebx,[esi+3]             ;Get target offset.
@@Big0: add     eax,ebx
        mov     [edi],eax
        ;
        mov     edi,SegmentList
        mov     edi,Seg_BaseAddress[edi]        ;Get offset from image start.
        mov     eax,PageCount+4 ;Get page number within segment.
        shl     eax,12
        add     edi,eax         ;Include page offset.
        movzx   eax,w[esi]
        add     edi,eax         ;Include byte offset.
        mov     eax,edi
        or      eax,1 shl 28            ;Mark it as a 32-bit offset.
        call    AddRelocationEntry      ;Add it to the list.
        mov     ErrorNumber,6
        jc      @@9
        ;
@@Neg1: add     esi,2+1+2
        sub     ecx,2+1+2
        test    dh,4
        jz      @@3
        add     esi,2
        sub     ecx,2
        jmp     @@3

        ;
@@Self32Off:    ;Deal with a 32-bit self relative offset.
        ;
        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1
        ;w[esi+3] - target offset.
        ;
        mov     edi,SegmentList
        mov     ebx,Seg_BaseAddress[edi]
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     ebx,eax
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@sfNeg1
        add     ebx,eax
        add     edi,eax         ;Point to the right offset.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        push    edx
        mov     edx,size _Seg_
        mul     edx
        pop     edx
        add     eax,4           ;skip dword count.
        add     eax,ObjectList  ;point to target segment details.
        mov     eax,Seg_BaseAddress[eax]        ;Get target segments offset from start of image.
        push    ebx
        movzx   ebx,w[esi+3]            ;Get target offset.
        test    dh,4
        jz      @@sfBig0
        mov     ebx,[esi+3]             ;Get target offset.
@@sfBig0:       add     eax,ebx
        pop     ebx
        add     ebx,4
        sub     eax,ebx
        mov     [edi],eax
@@sfNeg1:       add     esi,2+1+2
        sub     ecx,2+1+2
        test    dh,4
        jz      @@3
        add     esi,2
        sub     ecx,2
        jmp     @@3


        ;
@@Seg1632BitOff: ;Deal with an FWORD fixup by splitting into a seg16 and 32-bit
        ;offset relocation entry.
        ;
        ;EBP    - Page offset within segment.
        ;w[esi] - offset within page.
        ;b[esi+2] - target object+1
        ;w[esi+3] - target offset.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movsx   eax,w[esi]
        or      eax,eax
        js      @@Neg2
        add     edi,eax         ;Point to the right offset.
        add     edi,4           ;Point to the seg bit.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        mov     [edi],ax                ;Store target.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_BaseAddress[edi]        ;Get offset from image start.
        mov     eax,PageCount+4 ;Get page number within segment.
        shl     eax,12
        add     edi,eax         ;Include page offset.
        movzx   eax,w[esi]
        add     edi,eax         ;Include byte offset.
        add     edi,4           ;Point to the seg bit.
        mov     eax,edi
        call    AddRelocationEntry      ;Add it to the list.
        mov     ErrorNumber,6
        jc      @@9
        ;
        mov     edi,SegmentList
        mov     edi,Seg_Memory[edi]
        mov     eax,PageCount+4 ;Get page number.
        shl     eax,12
        add     edi,eax         ;Point to the right page.
        movzx   eax,w[esi]
        add     edi,eax         ;Point to the right offset.
        movzx   eax,b[esi+2]            ;Get the target segment.
        dec     eax
        push    edx
        mov     edx,size _Seg_
        mul     edx
        pop     edx
        add     eax,4           ;skip dword count.
        add     eax,ObjectList  ;point to target segment details.

;       test    Seg_Type[eax],1 shl 13
        test    Seg_Type[eax],1 shl 27  ; check if flat, MED 01/23/96

        pushf
        mov     eax,Seg_BaseAddress[eax]        ;Get target segments offset from start of image.
        movzx   ebx,w[esi+3]            ;Get target offset.
        test    dh,4
        jz      @@Big1
        mov     ebx,[esi+3]             ;Get target offset.
@@Big1: popf
        jz      @@NotFlat1
        pushf
        add     ebx,eax
        popf
@@NotFlat1:     mov     [edi],ebx
        jz      @@Neg2          ;no FLAT stuff needed.
        ;
        mov     edi,SegmentList
        mov     edi,Seg_BaseAddress[edi]        ;Get offset from image start.
        mov     eax,PageCount+4 ;Get page number within segment.
        shl     eax,12
        add     edi,eax         ;Include page offset.
        movzx   eax,w[esi]
        add     edi,eax         ;Include byte offset.
        mov     eax,edi
        or      eax,1 shl 28            ;Mark it as a 32-bit offset.
        call    AddRelocationEntry      ;Add it to the list.
        mov     ErrorNumber,6
        jc      @@9
        ;
@@Neg2: add     esi,2+1+2
        sub     ecx,2+1+2
        test    dh,4
        jz      @@3
        add     esi,2
        sub     ecx,2
        jmp     @@3
        ;
@@3:    inc     RelocationCount
        or      ecx,ecx
        jnz     @@2
        ;
@@4:    inc     ebp
        inc     PageCount+4
        dec     PageCount
        jnz     @@1

mednextobj2:
        add     SegmentList,size _Seg_
        add     ObjectBase,18h
        dec     ObjectCount
        jnz     @@0
        ;
@@8:    clc
        ret
        ;
@@9:    stc
        ret
CreateRelocations endp


;------------------------------------------------------------------------------
;
;Add an entry to the relocation list.
;
;On Entry:
;
;EAX    - Item to add.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AddRelocationEntry proc near
        pushad
        cmp     RelocationList,0
        jnz     @@0
        push    eax
        mov     ecx,4
        callc   malloc,ecx
        mov     esi,eax
        or      eax,eax
        pop     eax
        jz      @@9
        mov     RelocationList,esi
        mov     d[esi],0
        ;
@@0:    mov     esi,RelocationList
        mov     ecx,[esi]               ;Get current number of entries.
        inc     ecx
        shl     ecx,2           ;dword per entry.
        add     ecx,4           ;allow for count dword.
        push    eax
        callc   ReAlloc,esi,ecx
        mov     esi,eax
        or      eax,eax
        pop     eax
        jz      @@9
        mov     RelocationList,esi      ;store new list address.
        inc     d[esi]          ;increase entry count.
        add     esi,ecx
        sub     esi,4           ;point to new entry.
        mov     [esi],eax               ;store entry.
        ;
        clc
        jmp     @@10
        ;
@@9:    stc
@@10:   popad
        ret
AddRelocationEntry endp


;------------------------------------------------------------------------------
;
;Write the file at last.
;
Write3PFile     proc    near
        mov     esi,offset Write3PText
        calls   PrintString,esi
        ;
        ;Load the extender stub from this program.
        ;
        mov     edx,__argv
        mov     edx,[edx]
        calls   OpenFile,edx,0
        mov     ebx,eax
        or      eax,eax
        mov     ErrorNumber,12
        jz      @@9
        mov     edx,offset ExeSignature
        mov     ecx,1bh
        calls   ReadFile,ebx,edx,ecx
        cmp     eax,-1
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        mov     ax,w[ExeLength+2]       ;get length in 512 byte blocks

; MED 01/17/96
        cmp     WORD PTR [ExeLength],0
        je      medexe3         ; not rounded if no modulo

        dec     ax              ;lose 1 cos its rounded up

medexe3:
        add     ax,ax           ;mult by 2
        mov     dh,0
        mov     dl,ah
        mov     ah,al
        mov     al,dh           ;mult by 256=*512
        add     ax,w[ExeLength] ;add length mod 512
        adc     dx,0            ;add any carry to dx
        mov     cx,ax
        xchg    cx,dx
        shl     ecx,16
        mov     cx,dx
        mov     ErrorNumber,6
        add     ecx,4
        callc   Malloc,ecx
        mov     esi,eax
        or      eax,eax
        jz      @@9
        sub     ecx,4
        mov     StubMem,esi
        mov     d[esi],ecx
        add     esi,4
        pushs   ecx,esi
        xor     ecx,ecx
        xor     al,al
        calls   SeekFile,ebx,ecx,0
        pops    ecx,esi
        mov     ErrorNumber,12
        mov     edx,esi
        calls   ReadFile,ebx,edx,ecx
        cmp     eax,-1
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        ;
        ;Create the output file.
        ;
        mov     edx,OptionPointers+4
        calls   CreateFile,edx,0
        mov     ebx,eax
        or      eax,eax
        mov     ErrorNumber,7
        jz      @@9
        ;
        ;Write the stub.
        ;
        mov     ErrorNumber,8
        mov     edx,StubMem
        mov     ecx,[edx]
        add     edx,4
        calls   WriteFile,ebx,edx,ecx
        cmp     eax,-1
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        ;
        ;Write the main header.
        ;
        mov     edx,offset RealHeader
        mov     ecx,size NewHeaderStruc
        calls   WriteFile,ebx,edx,ecx
        cmp     eax,-1
        mov     ErrorNumber,8
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        ;
        ;Write the segment definitions.
        ;
        mov     esi,ObjectList  ;Point to the segment list.
        mov     ebp,[esi]               ;Get number of entries.
        add     esi,4
@@0:    mov     edx,esi         ;Point to segment defintion.
        mov     ecx,4+4
        calls   WriteFile,ebx,edx,ecx           ;Write this entry.
        cmp     eax,-1
        mov     ErrorNumber,8
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        add     esi,size _Seg_  ;Next entry.
        dec     ebp
        jnz     @@0             ;Do all segments.
        ;
        ;Write the relocation entries.
        ;
        mov     esi,RelocationList
        or      esi,esi
        jz      @@1
        mov     ecx,[esi]               ;Get number of entries.
        shl     ecx,2           ;Dword per entry.
        add     esi,4
        mov     edx,esi         ;Point to data.
        calls   WriteFile,ebx,edx,ecx
        cmp     eax,-1
        mov     ErrorNumber,8
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        ;
@@1:    ;Write the EXE image.
        ;
        mov     esi,ObjectList  ;Point to object definitions.
        mov     ebp,[esi]               ;Get number of entries.
        add     esi,4           ;Point to real data.
@@2:    mov     ecx,Seg_Length[esi]     ;Get segments length.
        mov     edx,Seg_Memory[esi]     ;point to segments image.
        calls   WriteFile,ebx,edx,ecx
        cmp     eax,-1
        mov     ErrorNumber,8
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        add     esi,size _Seg_  ;point to next entry.
        dec     ebp
        jnz     @@2             ;Do all entries.
        ;
        ;Close the output file.
        ;
        calls   CloseFile,ebx
        clc
        ret
        ;
@@9:    stc
        ret
Write3PFile     endp


;------------------------------------------------------------------------------
;
;Create 3P header data.
;
Create3PHeader  proc    near
        mov     esi,offset BuildHeaderText
        calls   PrintString,esi
        ;
        mov     edi,offset RealHeader
        ;
        ;Set auto DS object number if there is one.
        ;
        mov     esi,LEAddress
        mov     eax,[esi+94h]
        mov     NewAutoDS[edi],ax
        ;
        ;Set number of segments and increase file length field.
        ;
        mov     esi,ObjectList
        mov     eax,[esi]               ;Get number of segments.
        mov     NewSegments[edi],ax
        shl     eax,3           ;8 bytes per seg.
        mov     NewSize[edi],eax
        ;
        ;Set number of relocations and increase file length field.
        ;
        mov     esi,RelocationList
        xor     eax,eax
        or      esi,esi
        jz      @@0
        mov     eax,[esi]
@@0:    mov     NewRelocs[edi],eax
        shl     eax,2           ;4 bytes per entry.
        add     NewSize[edi],eax
        ;
        ;Set EXE image length and increase file length field.
        ;
        mov     eax,SegmentBase ;This is now total length.
        mov     NewLength[edi],eax
        mov     NewAlloc[edi],eax
        add     NewSize[edi],eax
        ;
        ;Include header in length field.
        ;
        add     NewSize[edi],size NewHeaderStruc
        ;
        ;Set entry CS:EIP
        ;
        mov     esi,LEAddress
        mov     eax,[esi+18h]   ;CS object number.
        dec     eax
        mov     NewEntryCS[edi],ax
        mov     eax,[esi+1Ch]   ;EIP value.
        mov     NewEntryEIP[edi],eax
        ;
        ;Set entry SS:ESP
        ;
        mov     esi,LEAddress
        mov     eax,[esi+20h]   ;SS object number.
        dec     eax
        mov     NewEntrySS[edi],ax
        mov     eax,[esi+24h]   ;ESP value.
        mov     NewEntryESP[edi],eax
        ;
        clc
        ret
Create3PHeader  endp


;------------------------------------------------------------------------------
;
;Create 3P version of LE file in memory.
;
Create3PFile    proc    near
        mov     esi,offset BuildImageText
        calls   PrintString,esi
IFDEF DEBUG2
;       int     3
ENDIF
        ;
        mov     esi,LEAddress
        mov     ecx,[esi+44h]   ;Get number of Objects.
        add     esi,[esi+40h]   ;Point to object table.
        mov     ObjectCount,ecx
        mov     ObjectBase,esi
        ;
@@0:    cmp     ObjectList,0            ;Started object list yet?
        jnz     @@0_0
        mov     ecx,4
        callc   Malloc,ecx
        mov     esi,eax
        or      eax,eax
        mov     ErrorNumber,6
        jz      @@9
        mov     ObjectList,esi
        mov     d[esi],0
@@0_0:  mov     esi,ObjectList
        mov     eax,[esi]               ;Get number of entries.
        inc     eax
        mov     edx,size _Seg_
        mul     edx
        add     eax,4
        mov     ecx,eax
        callc   ReAlloc,esi,ecx         ;Enlarge it.
        mov     esi,eax
        or      eax,eax
        mov     ErrorNumber,6
        jz      @@9
        mov     ObjectList,esi
        inc     d[esi]
        add     esi,ecx
        sub     esi,size _Seg_  ;Point to new entry.
        mov     edi,esi
        mov     ecx,size _Seg_
        xor     al,al
        rep     stosb           ;Clear it out.
        mov     edi,esi
        ;
        mov     eax,SegmentBase
        mov     Seg_BaseAddress[edi],eax
        mov     esi,ObjectBase
        mov     ecx,[esi]               ;Get segments size.
        add     ecx,4095
        and     ecx,not 4095            ;page align it
        add     SegmentBase,ecx
        mov     Seg_Length[edi],ecx     ;Set segments size.
        ;
        callc   Malloc,ecx
        mov     esi,eax
        or      eax,eax
        mov     ErrorNumber,6
        jz      @@9
        mov     Seg_Memory[edi],esi     ;Store segments address.
        pushs   eax,ecx,edi
        mov     edi,esi
        xor     al,al
        rep     stosb
        pops    eax,ecx,edi
        ;
        mov     esi,ObjectBase
        mov     eax,[esi+08h]   ;Get objects flags.
        xor     ebx,ebx
        test    eax,4           ;Executable?
        jnz     @@1
        inc     ebx             ;Make it Data.
        test    eax,2           ;Writeable?
        jz      @@1
;       add     ebx,2           ;Read only data.
@@1:
;       shl     ebx,24
        shl     ebx,21          ; MED, 01/22/96

        test    eax,2000h               ;Big bit set?
        jz      @@2
        or      ebx,1 shl 26            ;Force 32-bit.
        jmp     @@3
@@2:    or      ebx,1 shl 25            ;Force 16-bit.
@@3:    mov     eax,Seg_Length[edi]
        cmp     eax,100000h             ;>1M?
        jc      @@4
        shr     eax,12
        or      eax,1 shl 20
@@4:
        or      ebx,eax         ;Include length.

; MED 01/23/96
        test    WORD PTR [esi+08h],2000h        ; check if D/B bit set
        je      med2            ; not set, don't mark segment as flat

        or      ebx,1 shl 27            ;mark target type

med2:
        mov     Seg_Type[edi],ebx       ;Store the 3P type.
        ;
        mov     esi,ObjectBase
        mov     ecx,[esi+10h]   ;Get number of pages.

; MED 09/06/97
        test    ecx,ecx
        je      mednextobj              ; no pages to process

        mov     edx,[esi+0Ch]   ;Get page table index.
        mov     ebp,Seg_Length[edi]     ;Get maximum length again.
        mov     edi,Seg_Memory[edi]     ;Point to segments memory.
        mov     ebx,LEAddress
        add     ebx,[ebx+80h]
        sub     ebx,LEOffset
        ;
@@5:    mov     eax,edx         ;Get page number.
        dec     eax             ;make it base 0.
        shl     eax,12          ;*4096.
        add     eax,ebx         ;Make offset from data pages.
        mov     esi,eax
        ;
        mov     eax,4096                ;Default page size.
@@6:    cmp     ebp,eax         ;Want whole page?
        jnc     @@7
        mov     eax,ebp         ;Force smaller value.
@@7:    push    ecx
        mov     ecx,eax         ;Get length to copy.

;       rep     movsb           ;Copy this page.
        push    ecx
        shr     ecx,2
        rep     movsd
        pop     ecx
        and     ecx,3
        rep     movsb

        pop     ecx
        sub     ebp,eax         ;Update length remaining.
        inc     edx             ;Next page index.

;; MED 09/06/97
;       test    ecx,ecx
;       je      mednextobj              ; no pages to process

        dec     ecx
        jnz     @@6             ;Get all pages.
        ;

mednextobj:
        add     ObjectBase,18h  ;Next object
        dec     ObjectCount
        jnz     @@0             ;Do all objects.
        ;
        clc
        ret
        ;
@@9:    stc
        ret
Create3PFile    endp


;------------------------------------------------------------------------------
;
;Fetch the specified LE file, just the LE bit not its stub.
;
FetchLEFile     proc    near

IFDEF DEBUG
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    ds
        mov     edx,OFFSET debug9
        mov     ecx,SIZEOF debug9
        mov     ebx,1
        mov     ah,40h
        int     21h
        jmp     med9
debug9  DB      'In Fetch...',13,10
med9:
        pop     ds
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
ENDIF

        mov     esi,offset ReadingLEText
        calls   PrintString,esi
        ;
        mov     edx,OptionPointers
        calls   OpenFile,edx,0
        mov     ebx,eax
        or      eax,eax
        mov     ErrorNumber,2
        jz      @@9
        mov     ecx,3ch
        xor     al,al
        calls   SeekFile,ebx,ecx,0      ;Move to the LE bit.
        push    edx
        mov     edx,esp
        mov     ecx,4
        calls   ReadFile,ebx,edx,ecx            ;Read offset to LE.
        pop     edx
        mov     LEOffset,edx
        mov     ErrorNumber,3
        cmp     eax,-1
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        xor     ecx,ecx
        mov     al,2
        calls   SeekFile,ebx,ecx,2      ;Get file length.
        mov     ecx,eax
        sub     ecx,edx         ;Lose stub section from length.
        xchg    ecx,edx
        xor     al,al
        calls   SeekFile,ebx,ecx,0      ;Point to LE section.
        mov     ecx,eax
        xchg    ecx,edx
        push    ecx

;       add     ecx,4095
        add     ecx,4095+4096

        and     ecx,not 4095

        calls   Malloc,ecx              ;Get memory for the file.

        mov     esi,eax
        or      eax,eax
        pop     ecx
        mov     ErrorNumber,4
        jz      @@9
        pushs   eax,ecx,edi

;       add     ecx,4095
        add     ecx,4095+4096

        and     ecx,not 4095
        mov     edi,esi

;       xor     al,al
;       rep     stosb
        xor     eax,eax
        push    ecx
        shr     ecx,2
        rep     stosd
        pop     ecx
        and     ecx,3
        rep     stosb

        pops    eax,ecx,edi
        mov     edx,esi
        calls   ReadFile,ebx,edx,ecx            ;Read the file.
        cmp     eax,-1
        mov     ErrorNumber,5
        jz      @@9
        cmp     eax,ecx
        jnz     @@9
        calls   CloseFile,ebx   ;Close the file again.
        ;
        mov     LEAddress,esi   ;Store LE files address.
        mov     LELength,ecx            ;Store its length as well.
        clc
        ret
        ;
@@9:    stc
        ret
FetchLEFile     endp


        .data

Copyright       db 13
        db 79 dup ("Ä"),13
        db "LE to 3P Converter v2.04 (C) 1994-2003 Michael Devore; all rights reserved.",13
        db 79 dup ("Ä"),13
        db 13,0
;
StackStore      dd ?
ErrorNumber     dd 0
ErrorList       dd ErrorM00,ErrorM01,ErrorM02,ErrorM03,ErrorM04,ErrorM05,ErrorM06,ErrorM07
        dd ErrorM08,ErrorM09,ErrorM10,ErrorM11,ErrorM12,ErrorM13
ErrorM00        db 0
ErrorM01        db "Usage: LE23P InFileName [OutFileName]",13
        db 13
        db 0
ErrorM02        db "Could not open specified LE file.",13,0
ErrorM03        db "Error reading LE offset.",13,0
ErrorM04        db "Not enough memory to load LE file.",13,0
ErrorM05        db "Error reading LE file.",13,0
ErrorM06        db "Not enough memory to build 3P image.",13,0
ErrorM07        db "Could not create output file.",13,0
ErrorM08        db "Error occurred while writing output file.",13,0
ErrorM09        db "Multiple fixup records not supported.",13,0
ErrorM10        db "Only internal fixups supported.",13,0
ErrorM11        db "Invalid fixup type, only Seg16 and Offset32 supported.",13,0
ErrorM12        db "Error reading extender stub loader.",13,0
ErrorM13        db "Unknown fixup flag settings.",13,0
;
IFDEF DEBUG
ReadingLEText   db "(debug) Reading specified LE file.",13,0
ELSE
ReadingLEText   db "Reading specified LE file.",13,0
ENDIF
BuildImageText  db "Building EXE and segment image.",13,0
BuildHeaderText db "Building 3P format header.",13,0
Write3PText     db "Writing 3P file.",13,0
BuildRelocsText db "Building relocation table.",13,0
;
LEAddress       dd 0
LELength        dd 0
LEOffset        dd 0
;
ObjectCount     dd 0
ObjectBase      dd 0
ObjectList      dd 0
;
RelocationList  dd 0
;
SegmentBase     dd 0
SegmentList     dd 0
;
PageCount       dd 0,0
;
RealHeader      NewHeaderStruc <>
;
StubMem dd 0
RelocationCount dd 0
;
ExeSignature    db ?    ;00 Identifier text.
        db ?    ;01 /
ExeLength       dw ?    ;02 Length of file MOD 512
        dw ?    ;04 Length of file in 512 byte blocks.
ExeRelocNum     dw ?    ;06 Number of relocation items.
ExeHeaderSize   dw ?    ;08 Length of header in 16 byte paragraphs.
ExeMinAlloc     dw ?    ;0A Minimum number of para's needed above program.
ExeMaxAlloc     dw ?    ;0C Maximum number of para's needed above program.
ExeStackSeg     dw ?    ;0E Segment displacement of stack module.
ExeEntrySP      dw ?    ;10 value for SP at entry.
ExeCheckSum     dw ?    ;12 Check sum...
ExeEntryIP      dw ?    ;14 Contents of IP at entry.
ExeEntryCS      dw ?    ;16 Segment displacement of CS at entry.
ExeRelocFirst   dw ?    ;18 First relocation item offset.
ExeOverlayNum   db ?    ;1A Overlay number.
        db 20h-1bh dup (0)

InNameSpace     db 256 dup (0)
OutNameSpace    db 256 dup (0)


        end
