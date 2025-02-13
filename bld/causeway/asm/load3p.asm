ExeHeaderStruc  struc
apiExeSignature     db ?    ;00 Identifier text.
                    db ?    ;01 /
apiExeLength        dw ?    ;02 Length of file MOD 512
                    dw ?    ;04 Length of file in 512 byte blocks.
apiExeRelocNum      dw ?    ;06 Number of relocation items.
apiExeHeaderSize    dw ?    ;08 Length of header in 16 byte paragraphs.
apiExeMinAlloc      dw ?    ;0A Minimum number of para's needed above program.
apiExeMaxAlloc      dw ?    ;0C Maximum number of para's needed above program.
apiExeStackSeg      dw ?    ;0E Segment displacement of stack module.
apiExeEntrySP       dw ?    ;10 value for SP at entry.
apiExeCheckSum      dw ?    ;12 Check sum...
apiExeEntryIP       dw ?    ;14 Contents of IP at entry.
apiExeEntryCS       dw ?    ;16 Segment displacement of CS at entry.
apiExeRelocFirst    dw ?    ;18 First relocation item offset.
apiExeOverlayNum    db ?    ;1A Overlay number.
ExeHeaderStruc  ends
;
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
        push    ds
        push    es
        push    fs
        push    gs
        assume ds:nothing
        mov     ds,cs:apiDDSeg
        assume ds:_apiCode
        ;
        push    w[api89_SegSS]
        push    d[api89_SegMem]
        push    w[api89_EntryDS]
        push    d[api89_ExpMem]
        push    d[api89_EntryEIP]
        push    w[api89_SegCS]
        push    d[api89_EntryESP]
        push    d[api89_SegMem]
        push    d[api89_RelocMem]
        push    d[api89_ProgBase]
        push    d[api89_Segs]
        push    w[api89_PSP]
        push    d[api89_AutoOffset]
        push    d[api89_Command]
        push    w[api89_Command+4]
        push    d[api89_Flags]
        push    d[api89_Name]
        push    w[api89_Name+4]
        push    w[api89_Environment]
        push    d[api89_SystemFlags]
        push    d[api89_ModLink]
        push    d[api89_ImpMem]
        push    d[api89_ImpCnt]
        push    d[api89_3PStart]
        ;
        mov     d[api89_Name],edx
        mov     w[api89_Name+4],ax
        mov     d[api89_Flags],ebx
        mov     d[api89_Command],esi
        mov     w[api89_Command+4],es
        mov     w[api89_Environment],cx
        ;
        mov     w[api89_PSP],0
        mov     w[api89_Handle],0
        mov     d[api89_SegMem],0
        mov     d[api89_RelocMem],0
        mov     d[api89_AutoOffset],0
        mov     d[api89_ExpMem],0
        mov     d[api89_ModLink],0
        mov     d[api89_ImpMem],0
;
;Try and open the file.
;
        push    ds
        lds     edx,f[api89_Name]
        mov     ax,3d00h                            ;open, read only.
        int     21h
        pop     ds
        jc      api89_no_file_error
        mov     w[api89_Handle],ax                  ;store the handle.
;
;Check for just a 3P file with no extender.
;
api89_Look3P:
        mov     bx,w[api89_Handle]
        mov     edx,offset apiNewHeader             ;somewhere to put the info.
        mov     ecx,size NewHeaderStruc             ;size of it.
        mov     ah,3fh
        int     21h
        jc      api89_file_error
        cmp     ax,size NewHeaderStruc              ;did we read right amount?
        jnz     api89_file_error
        ;
        cmp     w[apiNewHeader],'ZM'
        jnz     api89_CheckNew
        ;
        ;Move back to EXE details.
        ;
        mov     ax,w[apiNewHeader+ExeHeaderStruc.apiExeLength+2] ;get length in 512 byte blocks
        cmp     WORD PTR [apiNewHeader+ExeHeaderStruc.apiExeLength],0
        je      medexe3                             ;not rounded if no modulo
        dec     ax                                  ;lose 1 cos its rounded up
medexe3:
        add     ax,ax                               ;mult by 2
        mov     dh,0
        mov     dl,ah
        mov     ah,al
        mov     al,dh                               ;mult by 256=*512
        add     ax,w[apiNewHeader+ExeHeaderStruc.apiExeLength] ;add length mod 512
        adc     dx,0                                ;add any carry to dx
        mov     cx,ax
        xchg    cx,dx                               ;swap round for DOS.
        mov     ax,4200h                            ;set absolute position.
        int     21h
        jmp     api89_Look3P
        ;
api89_CheckNew:
        cmp     w[apiNewHeader],'P3'                ;ID ok?
        jnz     api89_file_error
        ;
        ;Check if this is the right module.
        ;
        cmp     ExecMCount,0
        jz      api89_emc0
        dec     ExecMCount
        mov     edx,d[apiNewHeader.NewSize]
        sub     edx,size NewHeaderStruc
        Reg32To16hilo edx, dx, cx       ;edx -> dx:cx
        xchg    cx,dx
        mov     ax,4201h
        int     21h
        jmp     api89_Look3P
        ;
api89_emc0:
        ;
        ;Get file offset and store it.
        ;
        mov     bx,w[api89_Handle]
        xor     cx,cx
        xor     dx,dx
        mov     ax,4201h
        int     21h
        Reg16hiloTo32 dx, ax, edx       ;dx:ax -> edx
        mov     d[api89_3PStart],edx
        ;
        ;Close the file again.
        ;
        mov     bx,w[api89_Handle]
        mov     ah,3eh
        int     21h
        mov     w[api89_Handle],0
        ;
        ;Create a new PSP.
        ;
        push    ds
        mov     ebx,d[api89_Flags]
        mov     cx,w[api89_Environment]
        les     esi,f[api89_Command]
        lds     edx,f[api89_Name]
        call    CreatePSP
        pop     ds
        mov     w[api89_PSP],bx
        jc      api89_mem_error
        ;
        ;Open the input file again.
        ;
        push    ds
        lds     edx,f[api89_Name]
        mov     ax,3d00h                            ;open, read only.
        int     21h
        pop     ds
        jc      api89_file_error
        mov     w[api89_Handle],ax                  ;store the handle.
        ;
        ;Move past 3P header again.
        ;
        mov     edx,d[api89_3PStart]
        Reg32To16hilo edx, dx, cx       ;edx -> dx:cx
        xchg    dx,cx
        mov     ax,4200h
        mov     bx,w[api89_Handle]
        int     21h
        ;
        ;Grab flags.
        ;
        mov     esi,offset apiNewHeader
        mov     eax,NewHeaderStruc.NewFlags[esi]
        mov     d[api89_SystemFlags],eax
        test    b[api89_SystemFlags+3],128          ;compressed?
        jz      api89_noret
        cmp     d[api89_Flags],1                    ;debug load?
        jnz     api89_noret
        jmp     api89_file_error
        ;
api89_noret:
        ;
        ;Setup auto stack stuff if its needed.
        ;
        mov     esi,offset apiNewHeader
        cmp     NewHeaderStruc.NewEntryESP[esi],0   ;Need an automatic stack?
        jnz     api89_NotAutoESP
        mov     eax,NewHeaderStruc.NewAutoStack[esi] ;Get auto stack size.
        or      eax,eax
        jnz     api89_GotAutoSize
        mov     eax,1024
api89_GotAutoSize:
        mov     NewHeaderStruc.NewEntryESP[esi],eax ;Setup ESP value.
        mov     ebx,NewHeaderStruc.NewAlloc[esi]
        add     NewHeaderStruc.NewAlloc[esi],eax    ;update memory size needed.
        mov     d[api89_AutoOffset],ebx             ;store it for later.
api89_NotAutoESP:
        ;
        ;Get EXPORT memory.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewExports[esi]
        or      ecx,ecx
        jz      api89_NoExports0
        Sys     GetMemLinear32
        jc      api89_mem_error
        mov     d[api89_ExpMem],esi
api89_NoExports0:
        ;
        ;Get IMPORT module link table memory.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewImportModCnt[esi]
        or      ecx,ecx
        jz      api89_NoImports0
        shl     ecx,2                               ;dword per entry.
        add     ecx,4                               ;allow for count dword.
        Sys     GetMemLinear32
        jc      api89_mem_error
        mov     d[api89_ModLink],esi
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,RealSegment
        assume es:nothing
        mov     DWORD PTR es:[esi],0                ;clear entry count for now.
        mov     es,w[api89_PSP]
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Imports],esi
        pop     es
api89_NoImports0:
        ;
        ;Get program main memory.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewAlloc[esi]    ;get memory size required.
        Sys     GetMemLinear32                      ;Get segment/selector.
        jc      api89_mem_error                     ;Not enough memory.
        mov     d[api89_ProgBase],esi
        ;
        ;Get segment definition memory, selectors and details.
        ;
        mov     esi,offset apiNewHeader
        movzx   ecx,NewHeaderStruc.NewSegments[esi] ;get number of segments.
        cmp     d[api89_AutoOffset],0               ;need auto SS?
        jz      api89_NoAutoSeg
        inc     cx                                  ;one more for the stack.
api89_NoAutoSeg:
        push    cx
        shl     ecx,3                               ;8 bytes per entry.
        Sys     GetMemLinear32
        pop     cx
        jc      api89_mem_error
        mov     d[api89_SegMem],esi
        Sys     GetSels
        jc      api89_mem_error
        mov     w[api89_Segs],bx                    ;store base selector.
        mov     w[api89_Segs+2],cx                  ;store number of selectors.
        ;
        ;Update programs memory and selector details in PSP and variables.
        ;
        push    es
        mov     es,w[api89_PSP]
        mov     ax,w[api89_Segs]                            ;get base selector.
        mov     WORD PTR es:[EPSP_Struc.EPSP_SegBase],ax
        mov     ax,w[api89_Segs+2]                          ;get number of selectors.
        shl     ax,3                                        ;8 bytes per selector.
        mov     WORD PTR es:[EPSP_Struc.EPSP_SegSize],ax
        mov     eax,d[api89_ProgBase]                       ;get memory address.
        mov     DWORD PTR es:[EPSP_Struc.EPSP_MemBase],eax
        mov     DWORD PTR es:[EPSP_Struc.EPSP_NearBase],eax
        mov     edi,offset apiNewHeader
        mov     eax,NewHeaderStruc.NewAlloc[edi]            ;get memory size.
        mov     DWORD PTR es:[EPSP_Struc.EPSP_MemSize],eax
        pop     es
        ;
        ;Read segment definitions.
        ;
        test    b[api89_SystemFlags+3],128          ;compressed?
        jz      api89_ncp0
        mov     bx,w[api89_Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[api89_SegMem]
        Sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     api89_file_error
        jmp     api89_ncp1
        ;
api89_ncp0:
        mov     esi,offset apiNewHeader
        movzx   ecx,NewHeaderStruc.NewSegments[esi] ;get number of segments.
        shl     ecx,3                               ;8 bytes per entry.
        mov     bx,w[api89_Handle]
        mov     edx,d[api89_SegMem]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error
        cmp     eax,ecx
        jnz     api89_file_error
api89_ncp1:
        ;
        ;Setup a segment definition for auto-stack if needed.
        ;
        cmp     d[api89_AutoOffset],0
        jz      api89_NoAutoMake
        mov     esi,offset apiNewHeader
        movzx   edi,NewHeaderStruc.NewSegments[esi] ;get number of segments.
        mov     NewHeaderStruc.NewEntrySS[esi],di   ;store SS number.
        shl     edi,3                               ;8 bytes per entry.
        add     edi,d[api89_SegMem]                 ;offset to space for new entry.
        inc     NewHeaderStruc.NewSegments[esi]     ;update number of segments.
        mov     eax,d[api89_AutoOffset]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     es:[edi],eax                        ;setup base.
        mov     eax,NewHeaderStruc.NewEntryESP[esi] ;get limit.
        cmp     eax,100000h
        jc      api89_NoAutoGBit
        shr     eax,12
        or      eax,1 shl 20                        ;set G bit.
api89_NoAutoGBit:
        or      eax,2 shl 21                        ;set class 'stack'
        mov     es:[edi+4],eax                      ;setup limit entry.
api89_NoAutoMake:
        ;
        ;Get relocation table memory.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewRelocs[esi]   ;get number of relocation entries.
        or      ecx,ecx
        jz      api89_NoRelocsMem
        shl     ecx,2                               ;4 bytes per entry.
        Sys     GetMemLinear32
        jc      api89_mem_error
        mov     d[api89_RelocMem],esi
        ;
        ;Read relocation entries.
        ;
        test    b[api89_SystemFlags+3],128          ;compressed?
        jz      api89_ncp2
        mov     bx,w[api89_Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[api89_RelocMem]
        Sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     api89_file_error
        jmp     api89_NoRelocsMem
        ;
api89_ncp2:
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewRelocs[esi]   ;get number of relocation entries.
        shl     ecx,2                               ;4 bytes per entry.
        mov     bx,w[api89_Handle]
        mov     edx,d[api89_RelocMem]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error
        cmp     eax,ecx                             ;did we get enough?
        jnz     api89_file_error
api89_NoRelocsMem:
        ;
        ;Read export table.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewExports[esi]  ;get length of export table.
        or      ecx,ecx
        jz      api89_NoExpMem
        test    b[api89_SystemFlags+3],128          ;compressed?
        jz      api89_ncp3
        mov     bx,w[api89_Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[api89_ExpMem]
        Sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     api89_file_error
        jmp     api89_NoExpMem
        ;
api89_ncp3:
        mov     bx,w[api89_Handle]
        mov     edx,d[api89_ExpMem]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error
        cmp     eax,ecx                             ;did we get enough?
        jnz     api89_file_error
api89_NoExpMem:
        ;
        ;Get IMPORT memory.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewImports[esi]  ;get size of IMPORT table.
        or      ecx,ecx
        jz      api89_NoImpMem1
        shl     ecx,2                               ;4 bytes per entry.
        Sys     GetMemLinear32
        jc      api89_mem_error
        mov     d[api89_ImpMem],esi
        ;
        ;Read IMPORT entries.
        ;
        test    b[api89_SystemFlags+3],128          ;compressed?
        jz      api89_ncp20
        mov     bx,w[api89_Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[api89_ImpMem]
        Sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     api89_file_error
        jmp     api89_NoImpMem1
        ;
api89_ncp20:
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewImports[esi]  ;get size of IMPORT table.
        mov     bx,w[api89_Handle]
        mov     edx,d[api89_ImpMem]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error
        cmp     eax,ecx                             ;did we get enough?
        jnz     api89_file_error
api89_NoImpMem1:
        ;
        ;Read exe image.
        ;
        mov     esi,offset apiNewHeader
        test    NewHeaderStruc.NewFlags[esi],1 shl 31 ;compressed?
        jz      api89_NotComp
        mov     bx,w[api89_Handle]
        push    es
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     edi,d[api89_ProgBase]
        Sys     cwcLoad
        pop     es
        or      ax,ax
        jnz     api89_file_error
        jmp     api89_ImageLoaded
        ;
api89_NotComp:
        test    NewHeaderStruc.NewFlags[esi],1 shl 30
        jz      api89_LoadAll
        jmp     api89_LoadVMM
        ;
api89_LoadAll:
        mov     edx,d[api89_ProgBase]
        mov     ecx,NewHeaderStruc.NewLength[esi]   ;get image length.
        mov     bx,w[api89_Handle]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile                            ;read the file.
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error                    ;problems problems.
        cmp     eax,ecx                             ;did we get right amount?
        jnz     api89_file_error
        jmp     api89_ImageLoaded
        ;
api89_LoadVMM:
        ;
        ;Break the file read up into 8K chunks that suit the relocations.
        ;
        mov     eax,offset apiNewHeader
        mov     eax,NewHeaderStruc.NewRelocs[eax]   ;get number of relocation entries.
        mov     d[api89_SL_RelocLeft],eax
        mov     eax,d[api89_RelocMem]
        mov     d[api89_SL_RelocPnt],eax
        mov     eax,offset apiNewHeader
        mov     eax,NewHeaderStruc.NewLength[eax]
        mov     d[api89_SL_LoadLeft],eax
        mov     eax,d[api89_ProgBase]
        mov     d[api89_SL_LoadPnt],eax
        mov     d[api89_EntryEIP],0
api89_sl0:
        ;
        ;Anything left to load?
        ;
        cmp     d[api89_SL_LoadLeft],0
        jz      api89_ImageLoaded
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        ;
        ;Work out how much we should load.
        ;
        mov     ecx,d[api89_SL_LoadLeft]
        cmp     d[api89_SL_RelocLeft],0
        jz      api89_sl3
        mov     esi,d[api89_SL_RelocPnt]
        mov     eax,es:[esi]                        ;get fixup address.
        and     eax,0FFFFFFFh
        add     eax,4                               ;allow for cross page fixup.
        RoundUP eax,8192                            ;get terminal offset.
        mov     ebx,d[api89_SL_LoadPnt]
        sub     ebx,d[api89_ProgBase]
        sub     eax,ebx                             ;get relative offset.
        cmp     eax,ecx
        jnc     api89_sl1
        mov     ecx,eax
api89_sl1:
        ;
        ;Load next program chunk.
        ;
        or      ecx,ecx
        jnz     api89_sl3
        mov     eax,d[api89_EntryEIP]
        or      bx,-1
        mov     es,bx
api89_sl3:
        mov     edx,d[api89_SL_LoadPnt]
        mov     bx,w[api89_Handle]
        push    ecx
        push    ds
        mov     ds,apiDSeg
        assume ds:_cwMain
        mov     ds,RealSegment
        call    ReadFile                            ;read the file.
        assume ds:_apiCode
        pop     ds
        pop     ecx
        jc      api89_file_error                    ;problems problems.
        cmp     eax,ecx                             ;did we get right amount?
        jnz     api89_file_error
        ;
        ;Update variables.
        ;
        add     d[api89_SL_LoadPnt],ecx
        sub     d[api89_SL_LoadLeft],ecx
        ;
        ;Process all fixups in range of what we loaded.
        ;
        mov     esi,d[api89_SL_RelocPnt]
        mov     ecx,d[api89_SL_RelocLeft]
        mov     fs,w[api89_PSP]
        ;
        mov     bx,w[api89_Segs]
        mov     edi,DWORD PTR fs:[EPSP_Struc.EPSP_MemBase]
        add     edi,DWORD PTR fs:[EPSP_Struc.EPSP_MemSize]
        mov     dx,WORD PTR fs:[EPSP_Struc.EPSP_SegSize]
        shr     dx,3
api89_SL_RelocLoop:
        or      ecx,ecx
        jz      api89_SL_DoneRelocs
        mov     ebp,es:[esi]                        ;get linear offset.
        mov     eax,ebp
        and     ebp,0FFFFFFFh                       ;Lose relocation type.
        shr     eax,28                              ;Get relocation type.
        add     ebp,d[api89_ProgBase]               ;get linear address.
        cmp     ebp,edi
        jnc     api89_file_error2
        or      al,al
        jz      api89_SL_Seg16
        dec     al
        jz      api89_SL_Offset32
        jmp     api89_file_error2
        ;
api89_SL_Offset32:
        mov     eax,ebp
        add     eax,4-1
        cmp     eax,d[api89_SL_LoadPnt]             ;beyond what we loaded?
        jnc     api89_SL_DoneRelocs
        mov     eax,es:[ebp]                        ;Get offset.
        add     eax,d[api89_ProgBase]
        cmp     eax,edi                             ;Beyond program?
        jnc     api89_file_error2
        mov     eax,d[api89_ProgBase]
        add     DWORD PTR es:[ebp],eax              ;Do the relocation.
        jmp     api89_SL_NextReloc
        ;
api89_SL_Seg16:
        mov     eax,ebp
        add     eax,2-1
        cmp     eax,d[api89_SL_LoadPnt]             ;beyond what we loaded?
        jnc     api89_SL_DoneRelocs
        mov     ax,es:[ebp]                         ;get selector number.
        cmp     ax,dx
        jnc     api89_file_error2
        shl     ax,3                                ;*8 for selector value.
        add     ax,bx                               ;add base selector.
        mov     es:[ebp],ax                         ;set segment selector.
api89_SL_NextReloc:
        add     esi,4                               ;next relocation entry.
        inc     d[api89_EntryEIP]
        dec     ecx
        jnz     api89_SL_RelocLoop
api89_SL_DoneRelocs:
        mov     d[api89_SL_RelocPnt],esi
        mov     d[api89_SL_RelocLeft],ecx
        mov     esi,offset apiNewHeader
        mov     NewHeaderStruc.NewRelocs[esi],ecx
        jmp     api89_sl0
        ;
api89_ImageLoaded:
        ;
        ;Close the file now we're done with it.
        ;
        push    ds
        push    ds
        pop     fs
        pop     es
        mov     bx,w[api89_Handle]                  ;grab handle again,
        mov     w[api89_Handle],0
        mov     ah,3eh                              ;and close it.
        int     21h
        ;
        ;Process relocations.
        ;
        mov     esi,offset apiNewHeader
        mov     ecx,NewHeaderStruc.NewRelocs[esi]   ;get number of relocation entries.
        or      ecx,ecx
        jz      api89_DoneReloc
        mov     esi,d[api89_RelocMem]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     fs,w[api89_PSP]
        mov     d[api89_EntryEIP],0
        mov     bx,w[api89_Segs]
        mov     edi,DWORD PTR fs:[EPSP_Struc.EPSP_MemBase]
        add     edi,DWORD PTR fs:[EPSP_Struc.EPSP_MemSize]
        mov     dx,WORD PTR fs:[EPSP_Struc.EPSP_SegSize]
        shr     dx,3
api89_RelocLoop:
        mov     ebp,es:[esi]                        ;get linear offset.
        mov     eax,ebp
        and     ebp,0FFFFFFFh                       ;Lose relocation type.
        shr     eax,28                              ;Get relocation type.
        add     ebp,d[api89_ProgBase]               ;get linear address.
        cmp     ebp,edi
        jnc     api89_file_error2
        or      al,al
        jz      api89_Seg16
        dec     al
        jz      api89_Offset32
        jmp     api89_file_error2
        ;
api89_Offset32:
        mov     eax,es:[ebp]                        ;Get offset.
        add     eax,d[api89_ProgBase]
        cmp     eax,edi                             ;Beyond program?
        jnc     api89_file_error2
        mov     eax,d[api89_ProgBase]
        add     DWORD PTR es:[ebp],eax              ;Do the relocation.
        jmp     api89_NextReloc
        ;
api89_Seg16:
        mov     ax,es:[ebp]                         ;get selector number.
        cmp     ax,dx
        jnc     api89_file_error2
        shl     ax,3                                ;*8 for selector value.
        add     ax,bx                               ;add base selector.
        mov     es:[ebp],ax                         ;set segment selector.
api89_NextReloc:
        add     esi,4                               ;next relocation entry.
        inc     d[api89_EntryEIP]
        dec     ecx
        jnz     api89_RelocLoop
        push    ds
        push    ds
        pop     fs
        pop     es
api89_DoneReloc:
        ;
        ;Set selector details.
        ;
        mov     esi,offset apiNewHeader
        movzx   ecx,NewHeaderStruc.NewSegments[esi] ;get number of segments.
        mov     esi,d[api89_SegMem]
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
        mov     bx,w[api89_Segs]                    ;base selector.
api89_SegLoop:
        push    ebx
        push    ecx
        push    esi
        mov     eax,es:[esi+4]                      ;Get limit.
        mov     ecx,eax
        and     ecx,0fffffh                         ;mask to 20 bits.
        test    eax,1 shl 20                        ;G bit set?
        jz      api89_NoGBit
        shl     ecx,12
        or      cx,4095
api89_NoGBit:
        or      ecx,ecx
        jz      api89_NoDecLim
        cmp     ecx,-1
        jz      api89_NoDecLim
        dec     ecx
api89_NoDecLim:
        mov     edx,es:[esi]                        ;get base.
        test    eax,1 shl 27                        ;FLAT segment?
        jz      api89_NotFLATSeg
        push    fs
        mov     fs,w[api89_PSP]
        mov     DWORD PTR fs:[EPSP_Struc.EPSP_NearBase],0 ;Make sure NEAR functions work.
        pop     fs
        add     edx,d[api89_ProgBase]
        or      ecx,-1                              ;Update the limit.
        xor     edx,edx
        jmp     api89_DoSegSet
        ;
api89_NotFLATSeg:
        add     edx,d[api89_ProgBase]               ;offset within real memory.
api89_DoSegSet:
        Sys     SetSelDet32
        mov     cx,w[api89_SystemFlags]             ;use default setting.
        shr     cx,14
        xor     cl,1                                ;dual mode 16/32-bit
        or      cl,b[api89_SystemFlags+2]
        and     cl,SYSXFLAG_DUALM                   ;descriptor size bit
        call    _DSizeSelector
        mov     eax,es:[esi+4]                      ;Get class.
        shr     eax,21                              ;move type into useful place.
        and     eax,0fh                             ;isolate type.
        or      eax,eax
        jz      api89_CodeSeg
        jmp     api89_SegDone
        ;
api89_CodeSeg:
        mov     eax,es:[esi+4]                      ;Get type bits.
        mov     cx,0                                ;Set 16 bit seg.
        test    eax,1 shl 25
        jnz     api89_Default
        mov     cx,1
        test    eax,1 shl 26                        ;32 bit seg?
        jnz     api89_Default
        mov     cx,w[api89_SystemFlags]             ;use default setting.
        shr     cx,14
        and     cx,1                                ;dual mode 16/32-bit
        xor     cx,1                                ;descriptor size bit
api89_Default:
        Sys     CodeSel
api89_SegDone:
        pop     esi
        pop     ecx
        pop     ebx
        add     esi,8                               ;next definition.
        add     ebx,8                               ;next selector.
        dec     ecx
        jnz     api89_SegLoop
        ;
        ;Update export table values.
        ;
        cmp     d[api89_ExpMem],0
        jz      api89_exp9
        push    es
        mov     es,w[api89_PSP]
        mov     esi,d[api89_ExpMem]
        mov     edi,d[api89_SegMem]
        mov     DWORD PTR es:[EPSP_Struc.EPSP_Exports],esi
        mov     dx,es:[EPSP_Struc.EPSP_SegBase]
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
        add     es:[esi],ebp                        ;turn offset into an address.
        add     esi,4
api89_exp0:
        ;
        ;Do all other entries.
        ;
        add     es:[esi],ebp                        ;turn offset into an address.
        mov     ebx,es:[esi]                        ;get address of export entry.
        movzx   eax,WORD PTR es:[ebx+4]             ;get segment def.
        add     WORD PTR es:[ebx+4],dx              ;update seg
        shl     eax,3
        add     eax,edi                             ;point into seg defs.
        test    DWORD PTR es:[eax+4],1 shl 27       ;FLAT seg?
        jz      api89_exp1
        mov     eax,d[api89_ProgBase]
        add     DWORD PTR es:[ebx],eax              ;update to reflect FLAT
api89_exp1:
        add     esi,4
        dec     ecx
        jnz     api89_exp0
        pop     es
api89_exp9:
        ;
        ;Setup entry regs.
        ;
        mov     esi,offset apiNewHeader
        mov     ebx,NewHeaderStruc.NewEntryEIP[esi] ;get entry address.
        movzx   eax,NewHeaderStruc.NewEntryCS[esi]
        shl     eax,3
        add     ax,w[api89_Segs]
        mov     w[api89_SegCS],ax
        sub     ax,w[api89_Segs]
        add     eax,d[api89_SegMem]                 ;point to this segments entry.
        test    DWORD PTR es:[eax+4],1 shl 27       ;FLAT segment?
        jz      api89_NotFLATEIP
        add     ebx,es:[eax]                        ;include segments offset within image.
        add     ebx,d[api89_ProgBase]
api89_NotFLATEIP:
        mov     d[api89_EntryEIP],ebx
        mov     ebx,NewHeaderStruc.NewEntryESP[esi]
        movzx   eax,NewHeaderStruc.NewEntrySS[esi]
        shl     eax,3
        add     ax,w[api89_Segs]
        mov     w[api89_SegSS],ax
        sub     ax,w[api89_Segs]
        add     eax,d[api89_SegMem]                 ;point to this segments entry.
        test    DWORD PTR es:[eax+4],1 shl 27       ;FLAT segment?
        jz      api89_NotFLATESP
        add     ebx,es:[eax]                        ;include segments offset within image.
        add     ebx,d[api89_ProgBase]
api89_NotFLATESP:
        mov     d[api89_EntryESP],ebx
        push    es
        mov     es,w[api89_PSP]
        mov     ax,es
        pop     es
        cmp     NewHeaderStruc.NewAutoDS[esi],0
        jz      api89_NotAUTODS
        mov     ax,NewHeaderStruc.NewAutoDS[esi]
        dec     ax
        shl     ax,3
        add     ax,w[api89_Segs]
api89_NotAUTODS:
        mov     w[api89_EntryDS],ax
        ;
        ;Convert import module names to pointers to export tables.
        ;
        cmp     d[api89_ImpMem],0                   ;any imports?
        jz      api89_imp9
        mov     d[api89_ImpCnt],0                   ;Clear the counter.
        mov     es,apiDSeg
        assume es:_cwMain
        mov     es,es:RealSegment
        assume es:nothing
api89_imp0:
        mov     esi,d[api89_ImpMem]
        add     esi,DWORD PTR es:[esi]              ;move to module name list.
        mov     eax,d[api89_ImpCnt]
        cmp     eax,DWORD PTR es:[esi]              ;Done all modules?
        jnc     api89_imp6
        add     esi,es:[esi+4+eax*4]                ;Point to this module name.
        ;
        ;Try and find the module.
        ;
        call    FindModule
        jnc     api89_got_module
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
        jnz     api89_egm1
        mov     edi,offset ErrorM10_T
        movzx   ecx,BYTE PTR es:[esi]
        inc     esi
        mov     b[edi],'"'
        inc     edi
api89_egm0:
        mov     al,es:[esi]
        mov     [edi],al
        inc     esi
        inc     edi
        dec     ecx
        jnz     api89_egm0
        mov     b[edi+0],'"'
        mov     b[edi+1],13
        mov     b[edi+2],10
        mov     b[edi+3],"$"
api89_egm1:
        pop     ax
        assume ds:_apiCode
        pop     ds
        stc
        jmp     api89_error
        ;
api89_got_module:
        ;
        ;Update the module link table and usage count.
        ;
        mov     esi,d[api89_ModLink]
        inc     DWORD PTR es:[esi]                  ;update count.
        mov     eax,d[api89_ImpCnt]
        mov     es:[esi+4+eax*4],edi                ;store EPSP link pointer.
        ;
        ;Move onto next module name.
        ;
        inc     d[api89_ImpCnt]
        jmp     api89_imp0
        ;
api89_imp6:
        ;
        ;Now it's time to work through the relocations performing the fixups.
        ;
        mov     esi,d[api89_ImpMem]
        add     esi,DWORD PTR es:[esi+8]            ;move to fixup list.
        mov     eax,es:[esi]                        ;get number of entries.
        or      eax,eax
        jz      api89_imp9
        mov     d[api89_ImpCnt],eax
        add     esi,4
api89_imp7:
        xor     eax,eax
        mov     al,es:[esi]
        mov     d[api89_ImpType],eax
        mov     eax,es:[esi+1]
        mov     d[api89_ImpFlags],eax
        add     esi,1+4
        ;
        ;Point to the right modules export list.
        ;
        mov     ecx,d[api89_ImpFlags]               ;get flags
        shr     ecx,30
        xor     eax,eax
api89_imp8:
        shl     eax,8
        mov     al,es:[esi]
        inc     esi
        dec     ecx
        jnz     api89_imp8
        mov     edi,d[api89_ModLink]
        mov     edi,es:[edi+4+eax*4]
        mov     edi,es:EPSP_Struc.EPSP_Exports[edi]
        ;
        ;Point to the import name string.
        ;
        mov     ecx,d[api89_ImpFlags]
        shr     ecx,28
        and     ecx,3
        xor     eax,eax
api89_imp10:
        shl     eax,8
        mov     al,es:[esi]
        inc     esi
        dec     ecx
        jnz     api89_imp10
        ;
        ;Ordinal or string import?
        ;
        test    d[api89_ImpType],128                ;ordinal?
        jz      api89_imp14
        cmp     es:[edi],eax                        ;check number of entries.
        jc      api89_file_error
        mov     edi,es:[edi+4+eax*4]
        jmp     api89_imp11
        ;
api89_imp14:
        mov     ebp,d[api89_ImpMem]
        add     ebp,es:[ebp+4]                      ;point to name list.
        add     ebp,es:[ebp+4+eax*4]                ;point to name string.
        ;
        ;Try and find name string in export list.
        ;
        call    FindFunction
        jc      api89_file_error
api89_imp11:
        ;
        ;Found the right entry, now do the fixup.
        ;
        mov     ebx,d[api89_ImpFlags]
        and     ebx,0FFFFFFFh
        add     ebx,d[api89_ProgBase]
        mov     edx,es:[edi]
        xor     ecx,ecx
        mov     cx,es:[edi+4]
        mov     eax,d[api89_ImpType]
        ;
        ;Check if this is self relative.
        ;
        test    eax,64
        jz      api89_imp15
        and     eax,63
        mov     edx,ebx                             ;must be flat.
        cmp     eax,1
        jz      api89_imp18
        mov     edx,d[api89_ImpFlags]
        and     edx,0FFFFFFFh
api89_imp18:
        add     edx,2
        test    eax,1
        jz      api89_imp16
        add     edx,2
api89_imp16:
        sub     edx,es:[edi]
        neg     edx
api89_imp15:
        and     eax,63
        or      eax,eax
        jz      api89_imp_offset16
        dec     eax
        jz      api89_imp_offset32
        dec     eax
        jz      api89_imp_pointer16
        dec     eax
        jz      api89_imp_pointer32
        dec     eax
        jz      api89_imp_base
        jmp     api89_file_error
        ;
api89_imp_offset16:
        ;
        ;Do a 16-bit offset fixup.
        ;
        mov es:[ebx],dx
        jmp     api89_imp17
        ;
api89_imp_offset32:
        ;
        ;Do a 32-bit offset fixup.
        ;
        mov es:[ebx],edx
        jmp     api89_imp17
        ;
api89_imp_pointer16:
        ;
        ;Do a 16-bit pointer fixup.
        ;
        mov        es:[ebx],dx
        mov     es:[ebx+2],cx
        jmp     api89_imp17
        ;
api89_imp_pointer32:
        ;
        ;Do a 32-bit pointer fixup.
        ;
        mov        es:[ebx],edx
        mov     es:[ebx+4],cx
        jmp     api89_imp17
        ;
api89_imp_base:
        ;
        ;Do a base fixup.
        ;
        mov     es:[ebx],cx
api89_imp17:
        dec     d[api89_ImpCnt]
        jnz     api89_imp7
api89_imp9:
        ;
        ;Lose relocation memory.
        ;
        mov     esi,d[api89_RelocMem]
        Sys     RelMemLinear32                      ;release relocation list memory.
        mov     d[api89_RelocMem],0
        ;
        ;Lose import memory.
        ;
        mov     esi,d[api89_ImpMem]                 ;release IMPORT memory.
        or      esi,esi
        jz      api89_NoRelImp9
        Sys     RelMemLinear32
        mov     d[api89_ImpMem],0
api89_NoRelImp9:
        ;
        ;Lose segment definitions.
        ;
        cmp     d[api89_Flags],1
        jz      api89_NoSegMemRel
        mov     esi,d[api89_SegMem]
        Sys     RelMemLinear32                      ;release segment memory.
        mov     d[api89_SegMem],0
api89_NoSegMemRel:
        ;
        ;Check if this is an exec or just a load.
        ;
        cmp     d[api89_Flags],0
        jz      api89_Exec
        ;
        ;Switch back to parents PSP if this is a debug load.
        ;
        cmp     d[api89_Flags],2
        jz      api89_NoPSwitch2
        push    fs
        mov     fs,w[api89_PSP]
        mov     bx,WORD PTR fs:[EPSP_Struc.EPSP_Parent]
        pop     fs
        mov     ah,50h          ;set PSP
        int     21h
        mov     ebp,d[api89_SegMem]
api89_NoPSwitch2:
        ;
        ;Return program details to caller.
        ;
        mov     edx,d[api89_EntryEIP]
        mov     cx,w[api89_SegCS]
        mov     eax,d[api89_EntryESP]
        mov     bx,w[api89_SegSS]
        mov     si,w[api89_PSP]
        mov     di,w[api89_EntryDS]
        clc
        jmp     api89_exit
api89_Exec:
        ;
        ;Run it.
        ;
        mov     eax,d[api89_Flags]
        mov     ebx,d[api89_EntryEIP]
        mov     cx,w[api89_SegCS]
        mov     edx,d[api89_EntryESP]
        mov     si,w[api89_SegSS]
        mov     di,w[api89_PSP]
        mov     bp,w[api89_EntryDS]
        call    ExecModule
        clc
api89_error:
        ;
        ;Shut down anything still hanging around.
        ;
        pushf
        push    ax
        ;
        ;Make sure file is closed.
        ;
        pushf
        xor     bx,bx
        xchg    bx,w[api89_Handle]
        or      bx,bx
        jz      api89_NoClose
        mov     ah,3eh
        int     21h
api89_NoClose:
        ;
        ;Make sure all work spaces are released.
        ;
        mov     esi,d[api89_RelocMem]
        or      esi,esi
        jz      api89_NoRelRel
        Sys     RelMemLinear32                      ;release relocation list memory.
        mov     d[api89_RelocMem],0
api89_NoRelRel:
        mov     esi,d[api89_SegMem]
        or      esi,esi
        jz      api89_NoSegRel
        Sys     RelMemLinear32                      ;release segment memory.
        mov     d[api89_SegMem],0
api89_NoSegRel:
        mov     esi,d[api89_ImpMem]
        or      esi,esi
        jz      api89_NoImpRel
        Sys     RelMemLinear32
api89_NoImpRel:
        ;
        ;Restore previous state.
        ;
        popf
        jnc     api89_RelPSP
        cmp     w[api89_PSP],0
        jz      api89_NoRelRes
api89_RelPSP:
        ;
        ;Restore vectors & DPMI state.
        ;
        push    ds
        push    ds
        push    ds
        pop     gs
        pop     fs
        pop     es
        mov     eax,d[api89_Flags]
        mov     bx,w[api89_PSP]
        call    DeletePSP
api89_NoRelRes:
        ;
        ;Return to caller.
        ;
        pop     ax
        popf
api89_exit:
        pop     d[api89_3PStart]
        pop     d[api89_ImpCnt]
        pop     d[api89_ImpMem]
        pop     d[api89_ModLink]
        pop     d[api89_SystemFlags]
        pop     w[api89_Environment]
        pop     w[api89_Name+4]
        pop     d[api89_Name]
        pop     d[api89_Flags]
        pop     w[api89_Command+4]
        pop     d[api89_Command]
        pop     d[api89_AutoOffset]
        pop     w[api89_PSP]
        pop     d[api89_Segs]
        pop     d[api89_ProgBase]
        pop     d[api89_RelocMem]
        pop     d[api89_SegMem]
        pop     d[api89_EntryESP]
        pop     w[api89_SegCS]
        pop     d[api89_EntryEIP]
        pop     d[api89_ExpMem]
        pop     w[api89_EntryDS]
        pop     d[api89_SegMem]
        pop     w[api89_SegSS]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        ret
        ;
api89_mem_error:
        ;
        ;Not enough memory error.
        ;
        mov     ax,3
        stc
        jmp     api89_error
        ;
api89_no_file_error:
        ;
        ;Couldn't find the file.
        ;
        mov        ax,1
        stc
        jmp     api89_error
        ;
api89_file_error:
        ;
        ;Not a 3P file.
        ;
        mov     ax,2
        stc
        jmp     api89_error
        ;
api89_file_error2:
        ;
        ;Corrupt file.
        ;
        mov     eax,d[api89_EntryEIP]               ;Get the relocation number.
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
        jmp     api89_error
        ;
api89_Name:
        df 0,0
api89_Environment:
        dw 0
api89_SystemFlags:
        dd ?
api89_AutoOffset:
        dd ?
api89_Command:
        df ?,0
api89_Flags:
        dd 0
api89_ErrorCode:
        dw 0            ;error number.
api89_Handle:
        dw 0            ;file handle.
api89_EntryEIP:
        dd 0
api89_SegCS:
        dw 0
api89_EntryESP:
        dd 0
api89_SegSS:
        dw 0
api89_SegMem:
        dd 0
api89_RelocMem:
        dd 0
api89_ProgBase:
        dd 0
api89_Segs:
        dd 0
api89_PSP:
        dw 0
api89_EntryDS:
        dw 0
api89_ExpMem:
        dd 0
api89_ModLink:
        dd 0
api89_ImpMem:
        dd 0
api89_ImpCnt:
        dd 0
api89_ImpFlags:
        dd 0
api89_ImpType:
        dd 0
api89_3PStart:
        dd 0
api89_SL_RelocPnt:
        dd 0
api89_SL_RelocLeft:
        dd 0
api89_SL_LoadPnt:
        dd 0
api89_SL_LoadLeft:
        dd 0
Load3P  endp

