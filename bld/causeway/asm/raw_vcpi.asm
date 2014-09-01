;
;The RAW/VCPI specific code.
;
        .386p

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Main (Kernal) section for RAW/VCPI stuff. This holds the mode switch, interrupt
;simulator and low level memory managers etc.
;
_cwRaw  segment para public 'raw kernal code' use16
        assume cs:_cwRaw, ds:_cwRaw
;
InWindows       db 0
BreakAddress    dd ?
VMMHandle       dw ?
VMMName db '?:\',128 dup (0)
;
PageStackSize   equ     32
ProposedPresentFlag db 0
ProposedRecentFlag db 0
ProposedPage    dd 0
CompareCount    dd 0
NoneLockedPages dd 0
RecentMapStack  dd PageStackSize dup (0)
;
PageingPointer  dd 0
SwapFileLength  dd 0
;
FreePages       dd 1
medAllocPages   dd      0
TotalPages      dd 1
TotalPhysical   dd 0
;
RawSelBuffer    db 16 dup (?)
;
_LM_BlockBase   dd ?
_LM_BlockEnd    dd ?
_LM_Needed      dd ?
_LM_Got         dd ?
;
MemIntBuffer    db size RealRegsStruc dup (?)
XMSList label byte
                db 32*(2+4+4) dup (0)
XMSTempList     dd 32 dup (0)
XMSTotal        dd 0
ConventionalList label byte
                dw 32*2 dup (0)
ConvTempList    dw 32*2 dup (0)
CONVTotal       dd 0
CONVSavePara    dw 0


IFDEF MAXSAVE
CONVSaveSize    dw      -1
ELSE
CONVSaveSize    dw 32768/16
ENDIF

Int15Table      dd 8*2 dup (0)
Int15Size       dd 0
INT15hTotal     dd ?
INT15hValue     dd ?
INT15hLevel2    dw ?
;
Protected2Real  dw VCPIProt2Real
Real2Protected  dw VCPIReal2Prot
;
A20HandlerCall  dw ?
A20Flag         db 0
LowMemory label dword         ; Set equal to 0000:0080
                dw 00080h
                dw 00000h
HighMemory label dword
                dw 00090h     ; Set equal to FFFF:0090
                dw 0FFFFh

XMSPresent      db 0
XMSVer3Present  DB 0          ; MED, 09/10/99

XMSControl      dd ?
XMSBlockSize    dw ?
XMSUMBUsed      dw 0

IFDEF PERMBIG1
Big1Flag        DB      1
ELSE
Big1Flag        DB      0
ENDIF

;
VCPI_CR3        dd 0            ;CR3
VCPI_pGDT       dd ?            ;Pointer to GDT descriptor.
VCPI_pIDT       dd ?            ;Pointer to IDT descriptor.
VCPI_LDT        dw 0            ;LDTR
VCPI_TR         dw 0            ;TR
VCPI_EIP        dd 0            ;CS:EIP client entry point.
VCPI_CS         dw 0            ;/
;
VCPI_GDT        df 0            ;GDTR
VCPI_IDT        df 0            ;IDTR
;
VCPI_Entry      dd ?
                dw VCPI_0
;
VCPI_SP dw ?,?
;
GDTVal  df ?
GDTSav  df ?
IDTVal  df 0
IDTSav  df ?
CR0Sav  dd ?
CR3Sav  dd ?

CR0ProtSav      DD ?            ; MED 10/15/96

;EFlagsSav      dd ?
;
IDTReal         dw ?            ;Real mode segment for IDT.
GDTReal         dw ?            ;Real mode segment for GDT.
GDTLinear       dd ?,?,-1
MDTLinear       dd ?,?
LDTReal         dw ?
LDTLinear       dd ?
LDTStart        dw 0
KernalTSSReal   dw ?            ;Real mode segment for kernal TSS.
PageDIRReal     dw ?            ;Real mode segment for page directory.
PageDirLinear   dd ?,-1,?
PageAliasReal   dw ?            ;Real mode segment for page table alias.
PageAliasLinear dd ?,-1,?
Page1stReal     dw ?            ;Real mode segment for 1st page table entry.
Page1stLinear   dd ?,-1,?
PageDETLinear   dd 0,-1,?
;
LinearEntry     dd ?,?,?
LinearBase      dd ?
LinearLimit     dd ?
;
PageBufferReal  dw ?
PageBufferLinear dd ?
PageInt         db size v86CallStruc dup (0)
;
RawSystemFlags  dw 0,0
;
Int2CallCheck   db 8 dup (0)            ;00-07
        db 8 dup (8)                    ;08-0F

;; MED 02/16/96, force INT15h to be treated as hardware interrupt
        db 8 dup (0)                    ;10-17
;       db      0,0,0,0,0,15h-19,0,0    ;10-17

        db 0,0,0,0,1ch-16,0,0,0         ;18-1F
        db 0,0,0,23h-17,24h-18,0,0,0    ;20-27
        db 70h-28h dup (0)              ;28-6F
        db 8 dup ((70h-8))              ;70-77
        db 100h-78h dup (0)             ;78-FF
;
LastCallBack    dw ?
CallBackTable   db size CallBackStruc*MaxCallBacks dup (0)
ALLCallBack     dw ?
ALLICallBack    dw ?
CallBackList    label byte
ICallBackList   label byte
        rept AutoCallBacks
        call    RawICallBack
        endm
        rept UserCallBacks
        call    RawCallBack
        endm
CallBackSize    equ     ($-CallBackList)/MaxCallBacks
;
VDiskSig        db 'VDISK V'
VDiskSigLen     equ     $-VDiskSig
VDiskSigVec     equ     19h
VDiskSigOffs    equ     12h
VDiskLoOffs     equ     2ch
VDiskHiOffs     equ     2eh
;
RawStackPos     dd RawStackTotal
RawStackReal    dw ?
;
RetAdd          dw ?,?
StackAdd        dw ?,?
;
DbgSize2Dr      db 0,0,1,0,3
DbgType2Dr      db 0,1,3
;
Dbg     struc
Dbg_Address     dd 0
Dbg_Size        db 0
Dbg_Type        db 0
Dbg_Flags       db 0
                db 0
Dbg     ends
;
DbgTable        db 4*size Dbg dup (0)
;
MaxMemLin       dd 1021 shl 20
MaxMemPhys      dd -1
;
ExtALLSwitch    db 0
NoPassFlag      DB      0       ; nonzero if not passing real mode hardware interrupts up to protect mode
PreAllocSize    DD      0       ; size of VMM disk preallocation
DesiredVMMName  DB      13 DUP (0)      ; VMM name desired by user via CAUSEWAY e-var
FirstUninitPage DW      0

fCR3Flush       label dword
        dw _fCR3Flush,KernalCS
fPhysicalGetPage label dword
        dw _fPhysicalGetPage,KernalCS
fPhysicalGetPages label dword
        dw _fPhysicalGetPages,KernalCS
fRawSimulateINT label dword
        dw _fRawSimulateINT,KernalCS
fRawSimulateFCALL label dword
        dw _fRawSimulateFCALL,KernalCS
fRawSimulateFCALLI label dword
        dw _fRawSimulateFCALLI,KernalCS



;-------------------------------------------------------------------------------
RawVCPIRealMode proc far
;
;Disable hardware INT call-backs.
;

        pop     d[rv1_RetAdd]
        ;
        mov     bx,offset CallBackTable ;list of call backs.
        mov     cx,AutoCallBacks        ;number of entries to scan.

rv1_6:  test    CallBackStruc.CallBackFlags[bx],1       ;in use?
        jz      rv1_7
        test    CallBackStruc.CallBackFlags[bx],2       ;interupt?
        jz      rv1_7
        mov     CallBackStruc.CallBackFlags[bx],0
        push    bx
        push    cx
        mov     cx,w[CallBackStruc.CallBackReal+2+bx]   ;get original vector value.
        mov     dx,w[CallBackStruc.CallBackReal+bx]
        mov     bl,CallBackStruc.CallBackNum[bx]        ;get vector number.
        xor     bh,bh
        shl     bx,2
        mov     WORD PTR es:[bx+0],dx
        mov     WORD PTR es:[bx+2],cx
        pop     cx
        pop     bx
rv1_7:  add     bx,size CallBackStruc   ;next entry.
        dec     cx
        jnz     rv1_6
;
;Move the GDT/LDT back down into conventional memory.
;

        cmp     GDTLinear+8,-1
        jz      rv1_NoGDTMove
        mov     eax,GDTLinear+8
        mov     GDTLinear,eax
        mov     d[GDTVal+2],eax
        mov     d[VCPI_GDT+2],eax
        mov     ah,1
        int     16h             ;force LDT/GDT re-load.
;
;Check if page 1st is in extended memory and move back to conventional if it is.
;
rv1_NoGDTMove:

        cmp     Page1stLinear+4,-1
        jz      rv1_No1stMove
        push    ds
        mov     esi,Page1stLinear
        mov     edi,Page1stLinear+4
        push    es
        pop     ds
        mov     ecx,4096/4
        cld
        rep     movs d[edi],[esi]
        pop     ds
        mov     eax,Page1stLinear+4
        mov     Page1stLinear,eax
        mov     esi,PageDIRLinear
        mov     eax,0
        mov     ebx,Page1stLinear+8
        and     ebx,not 4095
        or      ebx,111b
        mov     es:[esi+eax*4],ebx
        call    CR3Flush
;
;Check if page dir ALIAS is in extended memory and move back to conventional if
;it is.
;
rv1_No1stMove:

        cmp     PageALIASLinear+4,-1
        jz      rv1_NoALIASMove
        push    ds
        mov     esi,PageALIASLinear
        mov     edi,PageALIASLinear+4
        push    es
        pop     ds
        mov     ecx,4096/4
        cld
        rep     movs d[edi],[esi]
        pop     ds
        mov     eax,PageALIASLinear+4
        mov     PageALIASLinear,eax
        mov     esi,PageDIRLinear
        mov     eax,1023
        mov     ebx,PageALIASLinear+8
        and     ebx,not 4095
        or      ebx,111b
        mov     es:[esi+eax*4],ebx
        call    CR3Flush
;
;Check if page DIR is in extended memory and move back to conventional if it is.
;
rv1_NoALIASMove:

        cmp     PageDIRLinear+4,-1
        jz      rv1_NoDIRMove
        push    ds
        mov     esi,PageDIRLinear
        mov     edi,PageDIRLinear+4
        push    es
        pop     ds
        mov     ecx,4096/4
        cld
        rep     movs d[edi],[esi]
        pop     ds
        mov     eax,PageDIRLinear+4
        mov     PageDIRLinear,eax
        mov     eax,PageDIRLinear+8
        mov     VCPI_CR3,eax
        call    CR3Flush
;
;Release VCPI memory.
;
rv1_NoDIRMove:

; MED 10/31/95
; switch to PL0 and set IDT to point to benign memory
;  so stupid Ensoniq VIVO driver can punch holes in the IDT without
;  causing an exception.
; Then switch back to PL3
        mov     WORD PTR [IDTVal],03ffh
        mov     eax,PageBufferLinear
        mov     DWORD PTR [IDTVal+2],eax
        pushad
        mov     DWORD PTR [rv1_StackAdd],esp
        mov     WORD PTR [rv1_StackAdd+4],ss
        call    RAWPL32PL0
        lidt    FWORD PTR [IDTVal]
        mov     edx,DWORD PTR [rv1_StackAdd]
        mov     cx,WORD PTR [rv1_StackAdd+4]
        call    RAWPL02PL3
        popad

        call    VCPIRelExtended ;release VCPI memory.

;Release XMS memory.
;

        call    RAWRelXMS       ;release XMS memory.
;
;Release INT 15h memory.
;

        call    Int15Rel        ;release int 15 vectors.
;
;Restore A20 state.
;

        call    A20Handler
;
;Switch back to real mode.
;

        mov     ax,KernalDS             ;Get supervisor data descriptor,
        mov     ds,ax           ;DS,ES,FS,GS,SS must be data with 64k limit
        mov     es,ax           ;expand up, read/write for switch back to real
        mov     fs,ax           ;mode.
        mov     gs,ax           ;/
        ;
        mov     edi,GDTLinear
        add     edi,KernalPL3_2_PL0
        and     edi,not 7
        push    es
        push    ax
        mov     ax,KernalZero
        mov     es,ax
        mov     WORD PTR es:[edi],offset rv1_pl0        ;store low word of limit.
        pop     ax
        pop     es
        pop     edi
        db 09ah         ;Absolute 16-bit call, to clear
        dw rv1_pl0,KernalPL3_2_PL0      ;instruction pre-fetch & load CS.
rv1_pl0:
        mov     ax,KernalSwitchPL0
        mov     ss,ax
        ;
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1 ;VCPI?
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        jz      rv1_VCPI
;
;Use raw mode to switch back.
;
rv1_RAW:
        mov     eax,CR0Sav              ;Get machine control &

; MED 10/15/96, don't clear emulate math coprocessor bit
;       and     eax,07FFFFFF2h  ;clear PM bit.
        and     eax,07FFFFFF6h  ;clear PM bit.

        mov     cr0,eax         ;/
        db 0eah         ;Absolute 16-bit jump, to clear
        dw rv1_RAW0,seg _cwRaw  ;instruction pre-fetch & load CS.
rv1_RAW0:
        mov     ax,_cwRaw               ;set everything up for real mode again.
        mov     ds,ax           ;/
        mov     es,ax           ;/
        mov     fs,ax
        mov     gs,ax
        mov     ax,_cwStack
        mov     ss,ax           ;/
        mov     esp,offset _cwStackEnd-4
        mov     eax,CR3Sav
        mov     cr3,eax
        lidt    IDTSav          ;restore old IDT 0(3ff)
        lgdt    GDTSav
;       push    EFlagsSav
;       popfd
        jmp     rv1_InReal
;
;Use VCPI mode to switch back.
;
rv1_VCPI:
        xor     eax,eax
        mov     ax,_cwRaw
        push    eax             ;GS
        push    eax             ;FS
        push    eax             ;DS
        push    eax             ;ES
        mov     ax,_cwStack
        push    eax             ;SS
        mov     ax,offset _cwStackEnd-4
        push    eax             ;ESP
        push    eax             ;EFLags (reserved for)
        xor     eax,eax
        mov     ax,_cwRaw
        push    eax             ;CS
        mov     ax,offset rv1_VCPI0
        push    eax             ;EIP
        mov     ax,KernalZero
        mov     es,ax
        xor     esi,esi
        mov     si,_cwRaw
        shl     esi,4
        add     esi,offset rv1_Call5
        mov     ax,w[VCPI_Entry+4]
        mov     es:[esi+4],ax
        mov     eax,d[VCPI_Entry]
        mov     es:[esi],eax
        mov     ax,KernalZero
        mov     ds,ax
        mov     ax,0DE0Ch
        assume ds:nothing
        call    FWORD PTR cs:[rv1_Call5]        ;switch back.
        assume ds:_cwRaw
rv1_Call5:
        df ?
rv1_VCPI0:
        mov     ax,_cwStack
        mov     ss,ax
        mov     esp,offset _cwStackEnd-4
        mov     ax,_cwRaw
        mov     ds,ax
        mov     es,ax           ;/
        mov     fs,ax
        mov     gs,ax
;
;We're back in real mode so remove any patches.
;
rv1_InReal:
        cmp     w[OldInt2F+2],0
        jz      rv1_ir0
        push    ds
        mov     ax,252fh
        lds     dx,OldInt2F
        int     21h
        pop     ds
rv1_ir0:
        cmp     VMMHandle,0             ;Any VMM file?
        jz      rv1_v0
        mov     bx,VMMHandle            ;close VMM file.
        mov     ax,3e00h
        int     21h
        mov     dx,offset VMMName       ;delete VMM file.
        mov     ah,41h
        int     21h
rv1_v0:
;
;Go back to _cwMain/Init caller.
;
        push    d[rv1_RetAdd]
        retf
rv1_RetAdd:
        dd 0

; MED 10/31/96
rv1_StackAdd:
        df ?

RawVCPIRealMode endp


;-------------------------------------------------------------------------------
_fRawSimulateINT proc far
        call    RawSimulateINT
        ret
_fRawSimulateINT endp


;-------------------------------------------------------------------------------
_fRawSimulateINT2 proc far
        call    RawSimulateINT2
        ret
_fRawSimulateINT2 endp


;-------------------------------------------------------------------------------
_fRawSimulateFCall proc far
        call    RawSimulateFCALL
        ret
_fRawSimulateFCall endp


;-------------------------------------------------------------------------------
_fRawSimulateFCall2 proc far
        call    RawSimulateFCALL2
        ret
_fRawSimulateFCall2 endp


;-------------------------------------------------------------------------------
_fRawSimulateFCallI proc far
        call    RawSimulateFCALLI
        ret
_fRawSimulateFCallI endp


;-------------------------------------------------------------------------------
;
;Allocate a page of physical memory.
;
_fPhysicalGetPage proc far
        xor     eax,eax
        mov     ax,cs
        push    eax
        mov     eax,offset rv7_0
        push    eax
        mov     ax,DpmiEmuCS
        push    eax
        mov     eax,offset _ffPhysicalGetPage
        push    eax
        db 66h
        retf
rv7_0:  ret
_fPhysicalGetPage endp


;-------------------------------------------------------------------------------
;
;Find out how many physical page of memory are free.
;
_fPhysicalGetPages proc far
        xor     eax,eax
        mov     ax,cs
        push    eax
        mov     eax,offset rv8_0
        push    eax
        mov     ax,DpmiEmuCS
        push    eax
        mov     eax,offset _ffPhysicalGetPages
        push    eax
        db 66h
        retf
rv8_0:  ret
_fPhysicalGetPages endp


;-------------------------------------------------------------------------------
;
;Switch 2 PL0, flush CR3 value then switch back to PL3.
;
_fCR3Flush      proc    far
        call    CR3Flush
        ret
_fCR3Flush      endp


;-------------------------------------------------------------------------------
;
;Switch 2 PL0, flush CR3 value then switch back to PL3.
;
CR3Flush        proc    near
        pushf
        cli
        pushad
        mov     d[rv10_StackAdd],esp
        mov     w[rv10_StackAdd+4],ss
        call    RAWPL32PL0
        ;
;       mov     eax,cr3
        mov     eax,VCPI_CR3
        mov     cr3,eax         ;flush page cache.
        ;
        mov     edx,d[rv10_StackAdd]
        mov     cx,w[rv10_StackAdd+4]
        call    RAWPL02PL3
        popad
        popf
        ret
        ;
rv10_StackAdd:
        df ?
CR3Flush        endp


;-------------------------------------------------------------------------------
Int15PatchTable dw Int15Patch0,Int15Patch1,Int15Patch2,Int15Patch3
        dw Int15Patch4,Int15Patch5,Int15Patch6,Int15Patch7
Int15Patch0:
        push    esi
        mov     si,offset ITable+0
        jmp     Int15Patch
Int15Patch1:
        push    esi
        mov     si,offset ITable+8
        jmp     Int15Patch
Int15Patch2:
        push    esi
        mov     si,offset ITable+16
        jmp     Int15Patch
Int15Patch3:
        push    esi
        mov     si,offset ITable+24
        jmp     Int15Patch
Int15Patch4:
        push    esi
        mov     si,offset ITable+32
        jmp     Int15Patch
Int15Patch5:
        push    esi
        mov     si,offset ITable+40
        jmp     Int15Patch
Int15Patch6:
        push    esi
        mov     si,offset ITable+48
        jmp     Int15Patch
Int15Patch7:
        push    esi
        mov     si,offset ITable+56
        jmp     Int15Patch
        ;
        assume ds:nothing
Int15Patch:
        cmp     ah,88h          ;get memory size?
        jnz     Checke801h
        mov     eax,cs:4[si]

Int15PatchRet:
        pop     esi
        iret

Checke801h:
        cmp     ax,0e801h
        jnz     Int15Old
        xor     bx,bx
        mov     eax,cs:4[si]    ; get 32-bit memory size 1K pages
        cmp     eax,3c00h
        jbe     E801Done                ; <16M memory, ax/cx hold proper return value
        mov     ebx,eax
        mov     eax,3c00h               ; ax holds 1K memory between 1M and 16M
        sub     ebx,eax                 ; ebx holds 1K pages of high memory
        shr     ebx,6                   ; convert 1K to 64K

E801Done:
        mov     cx,ax
        mov     dx,bx
        jmp     Int15PatchRet

        ;
Int15Old:
        mov     esi,cs:[si]
        mov     DWORD PTR cs:[Int15Jump],esi
        pop     esi
        jmp     DWORD PTR cs:[Int15Jump]        ;pass to old handler.
        assume ds:_cwRaw
Int15Jump       dd ?
ILevel  dw 0
ITable  dd 8*2 dup (0)


;-------------------------------------------------------------------------------
;
;Release any XMS we claimed.
;
RAWRelXMS       proc    far
        cmp     XMSPresent,0
        jz      rv11_Done
        ;
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        ;
        mov     si,offset XMSList       ;+2             ;list of handles.
        mov     cx,32
rv11_0: or      cx,cx
        jz      rv11_Done
        cmp     d[si+2],0
        jz      rv11_1
        push    cx
        push    si
        mov     dx,[si]
        mov     ah,0dh
        push    dx
        push    ax
        mov     ax,w[XMSControl]
        mov     [edi].v86CallStruc.v86CallIP,ax
        mov     ax,w[XMSControl+2]
        mov     [edi].v86CallStruc.v86CallCS,ax
        pop     ax
        mov     [edi].v86CallStruc.v86CallEAX,eax
        mov     [edi].v86CallStruc.v86CallEDX,edx
        mov     [edi].v86CallStruc.v86CallSS,0
        mov     [edi].v86CallStruc.v86CallSP,0
        call    RawSimulateFCALL        ;un-lock it first.
        pop     dx
        mov     ah,0ah
        push    ax
        mov     ax,w[XMSControl]
        mov     [edi].v86CallStruc.v86CallIP,ax
        mov     ax,w[XMSControl+2]
        mov     [edi].v86CallStruc.v86CallCS,ax
        pop     ax
        mov     [edi].v86CallStruc.v86CallEAX,eax
        mov     [edi].v86CallStruc.v86CallEDX,edx
        mov     [edi].v86CallStruc.v86CallSS,0
        mov     [edi].v86CallStruc.v86CallSP,0
        call    RawSimulateFCALL        ;un-lock it first.
        pop     si
        pop     cx
rv11_1: add     si,2+4+4
        dec     cx
        jmp     rv11_0
        ;
rv11_Done:
        ret
RAWRelXMS       endp


;-------------------------------------------------------------------------------
;
;Release any VCPI memory aquired.
;
VCPIRelExtended proc far
        push    ds
        mov     ax,MainCS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1
        assume ds:_cwRaw
        pop     ds
        jnz     rv12_9
        ;

        cmp     PageDETLinear,0
        jz      rv12_NoDET
        ;
        ;Release DET pages.
        ;

        mov     ax,KernalZero
        mov     es,ax
        mov     ecx,1024
        mov     esi,PageDETLinear
rv12_d0:
        test    DWORD PTR es:[esi],1            ;present?
        jz      rv12_d1
        test    DWORD PTR es:[esi],1 shl 10     ;VCPI bit set?
        jz      rv12_d1
        push    ecx
        push    esi
        push    edi
        push    ds
        push    es
        mov     edx,es:[esi]
        and     edx,0FFFFFFFFh-4095
        and     DWORD PTR es:[esi],0FFFFFFFFh-1 ;mark as no longer present.
        call    CR3Flush

        push    edi
        push    es
        push    ds
        pop     es
        mov     ax,0DE05h               ;free 4k page.
;       mov     bl,67h
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx

        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h

        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0

        call    RawSimulateFCALL
        pop     es
        pop     edi

;       mov     ax,0DE05h               ;free 4k page.
;       call    VCPICall

        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ecx
rv12_d1:
        add     esi,4
        dec     ecx
        jnz     rv12_d0


        ;
rv12_NoDET:

        ;Release normal memory.
        ;
        mov     ax,KernalZero
        mov     es,ax
        mov     esi,PageDirLinear
        mov     ecx,1022
        mov     edi,1024*4096*1023      ;base of page alias's.

rv12_0:
        test    DWORD PTR es:[esi],1            ;Page table present?
        jz      rv12_1

        push    ecx
        push    esi
        push    edi
        mov     ecx,1024
rv12_2:
        test    DWORD PTR es:[edi],1            ;Page present?
        jz      rv12_3
        test    DWORD PTR es:[edi],1 shl 10     ;VCPI bit set?
        jz      rv12_3

; MED 11/05/96
        cmp     esi,PageDirLinear       ; see if 0th page table
        jne     notzeroth               ; no
        mov     eax,edi
        sub     eax,1024*4096*1023
        cmp     ax,FirstUninitPage      ; see if below first uninitialized page table
        jb      rv12_3                  ; yes, not our entry to mess with

notzeroth:

        push    ecx
        push    esi
        push    edi
        push    ds
        push    es
        mov     edx,es:[edi]
        and     DWORD PTR es:[edi],0FFFFFFFFh-1 ;mark as no longer present.
        and     edx,0FFFFFFFFh-4095

        call    CR3Flush

;       mov     ax,0DE05h               ;free 4k page.
;       call    VCPICall

;if 0
        push    edi
        push    es
        push    ds
        pop     es
        mov     ax,0DE05h               ;free 4k page.
;       mov     bl,67h
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx

        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h

        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0

        call    RawSimulateFCALL

        pop     es
        pop     edi
;endif

        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ecx
rv12_3:
        add     edi,4
        dec     ecx
        jnz     rv12_2

        pop     edi
        pop     esi
        pop     ecx

        test    DWORD PTR es:[esi],1 shl 10     ;VCPI bit set?
        jz      rv12_1

        push    ecx
        push    esi
        push    edi
        push    ds
        push    es
        mov     edx,es:[esi]
        and     edx,0FFFFFFFFh-4095
        and     DWORD PTR es:[esi],0FFFFFFFFh-1 ;mark as no longer present.

        call    CR3Flush

;       mov     ax,0DE05h               ;free 4k page.
;       call    VCPICall

;if 0
        push    edi
        push    es
        push    ds
        pop     es
        mov     ax,0DE05h               ;free 4k page.
;       mov     bl,67h
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx

        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h

        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0

        call    RawSimulateFCALL

        pop     es
        pop     edi
;endif

        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ecx
        ;
rv12_1:
        add     edi,4096                ;next page table alias.
        add     esi,4           ;next page dir entry.
        dec     ecx
        jnz     rv12_0

rv12_9: ret
VCPIRelExtended endp


if 0
;-------------------------------------------------------------------------------
;
;Call VCPI entry point directly rather than via INT 67h.
;
VCPICall        proc    near
        push    ebx
        push    edi
        push    ds
        push    es
        push    ax
        mov     ax,KernalDS
        mov     ds,ax
        mov     es,ax
        pop     ax
        ;
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        mov     bl,67h
        call    RawSimulateINT
        mov     eax,RealRegsStruc.Real_EAX[edi]
        mov     edx,RealRegsStruc.Real_EDX[edi]
        ;
        pop     es
        pop     ds
        pop     edi
        pop     ebx
        ret
VCPICall        endp
endif


;-------------------------------------------------------------------------------
Int67h  proc    far
        int     67h
        ret
Int67h  endp


;-------------------------------------------------------------------------------
;
;Release real mode int 15 patches.
;
Int15Rel        proc    far
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1 ;VCPI?
        assume ds:_cwRaw
        pop     ds
        jnc     rv15_9
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     XMSPresent,0
        assume ds:_cwRaw
        pop     ds
        jnz     rv15_9
        ;
        mov     esi,offset ITable+((4+4)*7)
        mov     ecx,8
rv15_0:
        push    ecx
        push    esi
        cmp     d[esi],0
        jz      rv15_1
        mov     dx,0[esi]
        mov     cx,2[esi]
        mov     bl,15h
        push    ax
        push    ebx
        push    es
        mov     ax,KernalZero
        mov     es,ax
        movzx   ebx,bl
        shl     ebx,2
        pushf
        cli
        mov     es:0[ebx],dx
        mov     es:2[ebx],cx
        popf
        pop     es
        pop     ebx
        pop     ax
rv15_1:
        pop     esi
        pop     ecx
        sub     esi,4+4
        dec     ecx
        jnz     rv15_0
rv15_9: ret
Int15Rel        endp


;-------------------------------------------------------------------------------
;
;Real to protected mode switch.
;
;On Entry:-
;
;CX:EDX - target stack.
;
RAWReal2Prot    proc    near
        pop     w[rv16_Return]
        mov     d[rv16_ReturnStack],edx ;store return stack.
        mov     w[rv16_ReturnStack+4],cx
        ;
        mov     es,GDTReal
        mov     BYTE PTR es:[(KernalTS-3)+5],DescPresent+DescPL3+Desc386Tss
        sidt    IDTSav          ;save old IDT value for switch back.
        lgdt    GDTVal          ;Setup GDT &
        lidt    IDTVal          ;IDT.
;       pushfd
;       pop     eax
;       mov     EFlagsSav,eax
        mov     eax,cr3
        mov     CR3Sav,eax
        mov     eax,cr0
        mov     CR0Sav,eax
        mov     eax,VCPI_CR3            ;PageDirLinear
        mov     cr3,eax         ;set page dir address.

; MED 10/15/96
;       mov     eax,cr0         ;Get machine status &
;       or      eax,080000001h  ;set PM+PG bits.
        mov     eax,CR0ProtSav  ; restore protected mode cr0 status

        mov     cr0,eax         ;/
        db 0eah         ;Absolute 16-bit jump, to clear
        dw rv16_0,KernalCS0             ;instruction pre-fetch & load CS.
rv16_0: mov     ax,KernalLDT            ;Point to empty LDT descriptor.
        lldt    ax              ;and set LDT.
        mov     cx,KernalTS             ;Get value for task register.
        ltr     cx              ;and set it.
        ;
        ;Make our stuff addresable.
        ;
        mov     ax,KernalPL0
        mov     ss,ax           ;/
;       mov     esp,offset tPL0StackSize-4
        mov     esp,tPL0StackSize-4

        mov     ax,KernalDS             ;Get data descriptor.
        mov     ds,ax           ;/
        mov     es,ax           ;/
        mov     gs,ax           ;/
        mov     fs,ax
        ;
        cld
        clts
        ;
        mov     edx,d[rv16_ReturnStack]
        mov     cx,w[rv16_ReturnStack+4]
        call    RAWPL02PL3
        ;
        push    es
        mov     ax,KernalZero
        mov     es,ax
        mov     esi,GDTLinear
        add     esi,KernalTS-3
        mov     BYTE PTR es:[esi+5],DescPresent+DescPL3+Desc386Tss
        pop     es
        ;
        push    w[rv16_Return]
        ret
        ;
rv16_Return:
        dw ?
rv16_ReturnStack:
        df ?
RAWReal2Prot    endp


;-------------------------------------------------------------------------------
;
;Protected to real mode switch.
;
;On Entry:-
;
;CX:DX  - target stack.
;
RAWProt2Real    proc    near
        pop     w[rv17_Return]
        mov     w[rv17_ReturnSP],dx
        mov     w[rv17_ReturnSS],cx
        mov     ax,KernalDS             ;Get supervisor data descriptor,
        mov     ds,ax           ;DS,ES,FS,GS,SS must be data with 64k limit
        mov     es,ax           ;expand up, read/write for switch back to real
        mov     fs,ax           ;mode.
        mov     gs,ax           ;/
        ;
        call    RAWPL32PL0
        mov     ax,KernalSwitchPL0
        mov     ss,ax
        ;

; MED 10/15/96
        mov     eax,cr0
        mov     CR0ProtSav,eax  ; save protected mode cr0 status

        mov     eax,CR0Sav              ;Get machine control &

; MED 10/15/96, don't clear emulate math coprocessor bit
;       and     eax,07FFFFFF2h  ;clear PM bit.
        and     eax,07FFFFFF6h  ;clear PM bit.

        mov     cr0,eax         ;/
        db 0eah         ;Absolute 16-bit jump, to clear
        dw rv17_Resume,seg _cwRaw       ;instruction pre-fetch & load CS.
rv17_Resume:
        mov     ax,_cwRaw
        mov     ds,ax
        mov     ss,w[rv17_ReturnSS]
        movzx   esp,w[rv17_ReturnSP]
        mov     eax,CR3Sav
        mov     cr3,eax
        lidt    IDTSav          ;restore old IDT 0(3ff)
;       lgdt    GDTSav
;       push    EFlagsSav
;       popfd
        ;
        push    w[rv17_Return]
        ret
        ;
rv17_Return:
        dw ?
rv17_ReturnSP:
        dw ?
rv17_ReturnSS:
        dw ?
RAWProt2Real    endp


;-------------------------------------------------------------------------------
;
;Real to protected mode switch.
;
;On Entry:-
;
;CX:EDX - target stack.
;
VCPIReal2Prot   proc    near
        pop     w[rv18_Return]
        mov     d[rv18_ReturnStack],edx
        mov     w[rv18_ReturnStack+4],cx
        mov     VCPI_CS,KernalCS0
        mov     VCPI_EIP,offset rv18_Resume486
        mov     ax,0de0ch
        mov     si,seg _cwRaw
        movzx   esi,si
        shl     esi,4
        add     esi,offset VCPI_CR3
        int     67h
        ;
rv18_Resume486:
        ;Make our stuff addresable.
        ;
        mov     ax,KernalPL0
        mov     ss,ax           ;/
;       mov     esp,offset tPL0StackSize-4
        mov     esp,tPL0StackSize-4

        mov     ax,KernalDS             ;Get data descriptor.
        mov     ds,ax           ;/
        mov     es,ax           ;/
        mov     gs,ax           ;/
        mov     fs,ax
        ;
        pushfd
        pop     eax
        and     ax,1011111111111111b    ;clear NT.
        push    eax
        popfd
        cld
        ;
        mov     edx,d[rv18_ReturnStack]
        mov     cx,w[rv18_ReturnStack+4]
        call    RAWPL02PL3
        ;
        push    es
        mov     ax,KernalZero
        mov     es,ax
        mov     esi,GDTLinear
        add     esi,KernalTS-3
        mov     BYTE PTR es:[esi+5],DescPresent+DescPL3+Desc386Tss
        pop     es
        ;
        push    w[rv18_Return]
        ret
;
rv18_Return:
        dw ?
rv18_ReturnStack:
        df ?
;
VCPIReal2Prot   endp


;-------------------------------------------------------------------------------
;
;Protected to real mode switch.
;
;On Entry:-
;
;CX:DX  - Target stack.
;
VCPIProt2Real   proc    near
        pop     w[rv19_Return]
        mov     w[rv19_ReturnSP],dx
        mov     w[rv19_ReturnSS],cx
        ;
        mov     ax,KernalDS             ;Get supervisor data descriptor,
        mov     ds,ax           ;DS,ES,FS,GS,SS must be data with 64k limit
        mov     es,ax           ;expand up, read/write for switch back to real
        mov     fs,ax           ;mode.
        mov     gs,ax           ;/
        call    RAWPL32PL0
        mov     ax,KernalSwitchPL0
        mov     ss,ax
        ;
        movzx   ebp,dx
        xor     eax,eax
        push    eax
        push    eax
        push    eax
        push    eax
        mov     ax,cx   ;ss
        push    eax
        push    ebp     ;ESP
        pushfd  ;EFLags (reserved for)
        mov     ax,_cwRaw
        push    eax     ;CS
        mov     ax,offset rv19_Resume
        push    eax     ;EIP
        mov     ax,KernalZero
        mov     ds,ax
        mov     ax,0DE0Ch
        assume ds:nothing
        call    FWORD PTR cs:[VCPI_Entry]       ;switch back.
        assume ds:_cwRaw
        ;
rv19_Resume:
        ;Make stack addresable.
        ;
        mov     ax,_cwRaw
        mov     ds,ax
        mov     ss,w[rv19_ReturnSS]
        movzx   esp,w[rv19_ReturnSP]
        push    w[rv19_Return]
        ret
;
rv19_Return:
        dw ?
rv19_ReturnSP:
        dw ?
rv19_ReturnSS:
        dw ?
VCPIProt2Real   endp


;-------------------------------------------------------------------------------
RAWPL02PL3      proc    near
        pop     w[rv20_RetAdd]
        movzx   ecx,cx
        push    ecx             ;SS
        push    edx             ;ESP
        pushfd          ;EFlags
        pop     eax
        and     ax,1000111111111111b    ;clear NT & IOPL.
        or      ax,0011000000000000b    ;force IOPL.
        push    eax
        popfd
        push    eax
        xor     eax,eax
        mov     ax,KernalCS
        push    eax             ;CS
        mov     eax,offset rv20_pl3
        push    eax             ;EIP
        db 66h
        iretd
        ;
rv20_pl3:
        push    w[rv20_RetAdd]
        ret
        ;
rv20_RetAdd:
        dw ?
RAWPL02PL3      endp


;-------------------------------------------------------------------------------
RawPL32PL0      proc    near
        pop     w[rv21_RetAdd]
        push    edi
        mov     edi,GDTLinear
        add     edi,KernalPL3_2_PL0
        and     edi,not 7
        push    es
        push    ax
        mov     ax,KernalZero
        mov     es,ax
        mov     WORD PTR es:[edi],offset rv21_pl0       ;store low word of limit.
        pop     ax
        pop     es
        pop     edi
        db 09ah         ;Absolute 16-bit call, to clear
        dw rv21_pl0,KernalPL3_2_PL0     ;instruction pre-fetch & load CS.
        ;
rv21_pl0:
        push    w[rv21_RetAdd]
        ret
        ;
rv21_RetAdd:
        dw ?
RawPL32PL0      endp


;-------------------------------------------------------------------------------
;
;Release INT 2F patch.
;
ReleaseINT2F    proc    near
        push    ds
        mov     ax,252fh
        lds     dx,OldInt2F
        int     21h
        pop     ds
        ret
ReleaseINT2F    endp


;-------------------------------------------------------------------------------
;
;Intercept for windows init broadcast.
;
Int2FPatch      proc    near
;
;check if it's an init broadcast that's being allowed.
;
        cmp     ax,1605h
        jnz     rv23_exit
        or      bx,bx
        jnz     rv23_ret
        or      si,si
        jnz     rv23_ret
        or      cx,cx
        jnz     rv23_ret
        test    dx,1
        jnz     rv23_ret
        ;
        push    ds
        push    cs
        pop     ds
        inc     InWindows
        pop     ds
        jmp     rv23_ret
;
;check if it's an exit broadcast.
;
rv23_exit:
        cmp     ax,1606h
        jnz     rv23_ret
        test    dx,1
        jnz     rv23_ret
        ;
        push    ds
        push    cs
        pop     ds
        dec     InWindows
        pop     ds
;
;Let previous handlers have a go at it.
;
rv23_ret:
        assume ds:nothing
        jmp     DWORD PTR cs:[OldInt2F]
        assume ds:_cwRaw

if 0
        push    ax
        push    dx
        push    ds
        push    cs
        pop     ds
        mov     dx,offset WinMessage
        mov     ah,9
        int     21h
        pop     ds
        mov     ax,4cffh
        int     21h

rv23_Old:
        jmp     DWORD PTR cs:[OldInt2F]
        assume ds:_cwRaw

WinMessage      db 'Cannot run Windows in enhanced mode while a CauseWay application is active.',13,10
        db 'Run Windows in standard mode or remove the CauseWay application.',13,10,'$'
endif

OldInt2F        dd 0
Int2FPatch      endp


;-------------------------------------------------------------------------------
;
;Simulate real mode interupt.
;
;On Entry:-
;
;ES:EDI - Parameter table.
;BL     - Interupt number.
;
;On Exit:-
;
;Parameter table updated.
;
RAWSimulateInt proc     near
        push    cx
        push    bx
        mov     bh,0
        mov     cx,0
        jmp     RAWSimulate
RAWSimulateInt  endp


;-------------------------------------------------------------------------------
;
;Simulate real mode interupt.
;
;On Entry:-
;
;ES:EDI - Parameter table.
;SS:EBP - Stacked parameters.
;CX     - stacked word count.
;BL     - Interupt number.
;
;On Exit:-
;
;Parameter table updated.
;
RAWSimulateInt2 proc    near
        push    cx
        push    bx
        mov     bh,0
        jmp     RAWSimulate
RAWSimulateInt2 endp


;-------------------------------------------------------------------------------
;
;Simulate real mode far call.
;
;On Entry:-
;
;ES:EDI - Parameter table.
;
;On Exit:-
;
;Parameter table updated.
;
RAWSimulateFCALL proc near
        push    cx
        push    bx
        mov     bh,1
        mov     cx,0
        jmp     RawSimulate
RAWSimulateFCALL endp


;-------------------------------------------------------------------------------
;
;Simulate real mode far call.
;
;On Entry:-
;
;ES:EDI - Parameter table.
;SS:EBP - Stacked parameters.
;CX     - stacked word count.
;
;On Exit:-
;
;Parameter table updated.
;
RAWSimulateFCALL2 proc near
        push    cx
        push    bx
        mov     bh,1
        jmp     RawSimulate
RAWSimulateFCALL2 endp


;-------------------------------------------------------------------------------
;
;Simulate real mode far call with IRET stack frame.
;
;On Entry:-
;
;ES:EDI - Parameter table.
;SS:EBP - Stacked parameters.
;CX     - stacked word count.
;
;On Exit:-
;
;Parameter table updated.
;
RAWSimulateFCALLI proc near
        push    cx
        push    bx
        mov     bh,2
        jmp     RawSimulate
RAWSimulateFCALLI endp


;-------------------------------------------------------------------------------
;
;Simulate either a real mode INT or far call.
;
RawSimulate     proc    near
        pushf                   ;Preserve IF state.
        cli                     ;Stop INTs interfering.
        cld                     ;make sure direction is right.
        pushad
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax           ;/
        mov     ax,KernalZero
        mov     fs,ax
        push    d[rv29_tVCPI_SP]
        push    d[rv29_IntAdd]
        push    w[rv29_CallAdd]
        push    w[rv29_ourstack]
        mov     w[rv29_ourstack],0
;
;setup the real mode stack.
;
        mov     ax,es:RealRegsStruc.Real_SP[edi]        ;check if stack is being
        or      ax,es:RealRegsStruc.Real_SS[edi]        ;supplied.
        jnz     rv29_GotStack
;
;Caller isn't supplying a stack so we will.
;
        mov     eax,RawStackPos
        sub     RawStackPos,RawStackDif ;update for re-entry.
        mov     es:RealRegsStruc.Real_SP[edi],ax
        xor     esi,esi
        mov     si,RawStackReal
        mov     es:RealRegsStruc.Real_SS[edi],si
        or      w[rv29_ourstack],-1
;
;Point to the real mode stack.
;
rv29_GotStack:
        movzx   esi,es:RealRegsStruc.Real_SS[edi]
        movzx   eax,es:RealRegsStruc.Real_SP[edi]
        sub     eax,(4+4)+(4+4)
        mov     w[rv29_tVCPI_SP+2],si
        mov     w[rv29_tVCPI_SP],ax
        mov     edx,esi
        shl     esi,4
        add     esi,eax
;
;Store current stack pointer on v86 stack.
;
        xor     eax,eax
        mov     ax,ss
        mov     fs:[esi+4],eax
        mov     eax,esp
        test    BYTE PTR RawSystemFlags,1
        jz      rv29_noextendstack
        movzx   eax,ax
rv29_noextendstack:
        mov fs:[esi+0],eax
;
;Store table address on v86 stack.
;
        xor     eax,eax
        mov     ax,es
        mov     fs:[esi+12],eax
        mov     fs:[esi+8],edi
;
;Copy stacked parameters.
;
        or      cx,cx
        jz      rv29_NoStacked
        movzx   eax,cx
        shl     eax,1
        add     ebp,eax
rv29_copystack0:
        sub     ebp,2
        sub     esi,2
        mov     ax,[ebp]
        mov     fs:[esi],ax
        dec     cx
        jnz     rv29_copystack0
;
;Put flags onto the real mode stack.
;
rv29_NoStacked:
        mov     ebp,esp
        test    BYTE PTR RawSystemFlags,1
        jz      rv29_Its32
        movzx   ebp,bp
rv29_Its32:
        mov     ax,[ebp+(2+4+4)+(2+2+2+2)+(4+4+4+4+4+4+4+4)+2]
        or      bh,bh           ;int or far?
        jnz     rv29_NoIF
        and     ax,1111110011111111b    ;clear Trap and INT flag.
rv29_NoIF:
        sub     esi,2
        mov     fs:[esi],ax
        ;
        ;See if the CS:IP is supplied or needs fetching.
        ;
        or      bh,bh
        jz      rv29_IsInt
        ;
        mov     cx,es:RealRegsStruc.Real_CS[edi]
        shl     ecx,16
        mov     cx,es:RealRegsStruc.Real_IP[edi]
        mov     w[rv29_calladd],offset rv29_fcall
        cmp     bh,2
        jnz     rv29_notint
        mov     w[rv29_calladd],offset rv29_fcalli
        jmp     rv29_notint
        ;
rv29_IsInt:
        ;See if this is a busy interrupt call back.
        ;
        xor     bh,bh
        mov     bp,bx
        shl     bp,2
        mov     al,[Int2CallCheck+bx]
        or      al,al
        jz      rv29_c3
        sub     bl,al
        shl     bx,3            ;*8
        mov     ax,bx
        shl     bx,1            ;*16
        add     bx,ax           ;*24
        add     bx,offset CallBackTable
        test    CallBackStruc.CallBackFlags[bx],128     ;this entry in use?
        jz      rv29_c3
        mov     ecx,CallBackStruc.CallBackReal[bx]
        jmp     rv29_c2
        ;
rv29_c3:
        ;Get interupt address to put on stack.
        ;
        mov     ecx,DWORD PTR fs:[bp]
        ;
rv29_c2:
        mov     w[rv29_calladd],offset rv29_int
        ;
rv29_NotInt:
        sub     esi,4
        mov     fs:[esi],ecx
;
;Copy register values onto real mode stack.
;
        sub     esi,4+4+4+4+4+4+4+4+2+2+2+2+2+2 ;extra +2 for movsD
        push    esi
        push    edi
        push    ds
        push    es
        xchg    esi,edi
        push    es
        push    fs
        pop     es
        pop     ds
        mov     ecx,(4+4+4+4+4+4+4+4+2+2+2+2+2+2)/4
        rep     movs d[edi],[esi]
        pop     es
        pop     ds
        pop     edi
        pop     esi
;
;Get ss:sp values again.
;
        mov     ecx,edx
        mov     eax,ecx
        shl     eax,4
        mov     edx,esi
        sub     edx,eax
;
;Switch back to v86 mode.
;
        call    w[Protected2Real]
;
;Fetch registers off the stack.
;
        assume ds:nothing
        popad
        pop     WORD PTR cs:[rv29_IntAdd]       ;lose dummy.
        pop     es
        pop     ds
        pop     fs
        pop     gs
        pop     WORD PTR cs:[rv29_IntAdd]       ;lose dummy.
        ;
        pop     DWORD PTR cs:[rv29_IntAdd]
        jmp     WORD PTR cs:[rv29_calladd]
        ;
rv29_fcall:
        popf
        call    DWORD PTR cs:[rv29_IntAdd]
        jmp     rv29_Back
        ;
rv29_fcalli:
rv29_int:
        call    DWORD PTR cs:[rv29_IntAdd]
rv29_Back:
        pushf
        cli
        cld
        pop     WORD PTR cs:[rv29_IntAdd]
        and     WORD PTR cs:[rv29_IntAdd],0000110011010101b
;
;Switch back to old stack.
;
        mov     ss,WORD PTR cs:[rv29_tVCPI_SP+2]
        mov     sp,WORD PTR cs:[rv29_tVCPI_SP]
;
;Save all registers.
;
        push    WORD PTR cs:[rv29_IntAdd]       ;save dummy.
        push    gs
        push    fs
        push    ds
        push    es
        push    WORD PTR cs:[rv29_IntAdd]       ;save flags.
        pushad
;
;Make our data addresable again and store stack values.
;
        mov     ax,_cwRaw
        mov     ds,ax
        assume ds:_cwRaw
        mov     w[rv29_tVCPI_SP],sp
        mov     w[rv29_tVCPI_SP+2],ss
        mov     bp,sp
;
;Retrieve protected mode stack address.
;
        mov     edx,d[bp+(4+4+4+4+4+4+4+4)+(2+2+2+2+2)+(2)]
        mov     cx,w[bp+4+(4+4+4+4+4+4+4+4)+(2+2+2+2+2)+(2)]
;
;switch back to protected mode.
;
        call    w[Real2Protected]
;
        mov     ax,KernalZero   ;/
        mov     fs,ax           ;/
;
;Retreive v86 stack address.
;
        movzx   esi,w[rv29_tVCPI_SP+2]
        shl     esi,4
        movzx   eax,w[rv29_tVCPI_SP]
        add     esi,eax
;
;Retrieve table address.
;
        les     edi,fs:[esi+(4+4+4+4+4+4+4+4)+(2+2+2+2+2)+(2)+(4+4)]
;
;Copy new register values into table.
;

        push    esi
        push    edi
        push    ds
        push    fs
        pop     ds
        mov     ecx,(4+4+4+4+4+4+4+4+2+2+2+2+2+2)/4
        rep     movs d[edi],[esi]
        pop     ds
        pop     edi
        pop     esi
        mov     bx,[esp+(2+2+2+2)+(4+4+4+4+4+4+4+4)+(4+4+2)+2]
        and     bx,1111001100101010b
        or      es:RealRegsStruc.Real_Flags[edi],bx
        ;
        cmp     w[rv29_ourstack],0
        jz      rv29_nostackadjust
        add     RawStackPos,RawStackDif ;update for re-entry.
rv29_nostackadjust:
        pop     w[rv29_ourstack]
        pop     w[rv29_CallAdd]
        pop     d[rv29_IntAdd]
        pop     d[rv29_tVCPI_SP]
        ;
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        popf
        clc
        ;
        pop     bx
        pop     cx
        ret
        ;
rv29_IntAdd:
        dd ?
rv29_CallAdd:
        dw ?
rv29_tVCPI_SP:
        dd ?
rv29_ourstack:
        dw 0
RAWSimulate     endp


;-------------------------------------------------------------------------------
RAWCallBack     proc    near
        pushf
        cli
        ;
        ;Check if Windows enhanced mode has been started.
        ;
        assume ds:nothing
        cmp     cs:InWindows,0
        assume ds:_cwRaw
        jz      rv30_Normal
        popf
        retf
        ;
rv30_Normal:
        push    ax
        push    bp
        push    ds
        mov     ax,_cwRaw
        mov     ds,ax
        mov     bp,sp
        mov     ax,[bp+2+2+2+2] ;get return address
        mov     RetAdd,ax
        mov     ax,[bp+2+2+2]   ;get flags
        mov     [bp+2+2+2+2],ax ;ovewrite return address.
        mov     StackAdd,bp
        add     StackAdd,2+2+2+2+2      ;correct for stacked registers.
        mov     StackAdd+2,ss
        pop     ds
        pop     bp
        pop     ax
        add     sp,2            ;remove local return address.
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
        mov     ax,_cwRaw
        mov     ds,ax           ;make our data addresable.
        mov     es,ax
        mov     fs,ax
        mov     gs,ax
        mov     VCPI_SP,sp
        mov     VCPI_SP+2,ss
        ;
        ;Check if this call back is int &| busy.
        ;
        mov     ax,RetAdd               ;get return address.
        sub     ax,CallBackSize ;back to start of call back entry.
        sub     ax,offset CallBackList  ;offset from start of list.
        xor     dx,dx
        mov     bx,CallBackSize
        div     bx              ;entry number.
        mov     bx,size CallBackStruc
        mul     bx              ;get offset into table.
        mov     bx,offset CallBackTable
        add     bx,ax           ;point to this entry.
        ;
        ;switch to protected mode.
        ;
        mov     cx,KernalSS
        mov     edx,RawStackPos
        sub     RawStackPos,RawStackDif
        call    Real2Protected
        ;
        mov     ax,RetAdd               ;get return address.
        sub     ax,CallBackSize ;back to start of call back entry.
        sub     ax,offset CallBackList  ;offset from start of list.
        jz      rv30_zero
        xor     dx,dx
        mov     bx,CallBackSize
        div     bx              ;entry number.
rv30_zero:
        mov     bx,size CallBackStruc
        mul     bx              ;get offset into table.
        mov     bx,offset CallBackTable
        add     bx,ax           ;point to this entry.
        movzx   esi,w[VCPI_SP+2]        ;point to stacked registers.
        shl     esi,4
        movzx   eax,w[VCPI_SP]
        add     esi,eax
        mov     ax,KernalZero   ;/
        mov     fs,ax           ;/
        ;
        les     edi,CallBackStruc.CallBackRegs[bx]      ;get register structure.
        mov     ax,fs:[esi]
        mov     es:RealRegsStruc.Real_GS[edi],ax
        mov     ax,fs:[esi+2]
        mov     es:RealRegsStruc.Real_FS[edi],ax
        mov     ax,fs:[esi+4]
        mov     es:RealRegsStruc.Real_ES[edi],ax
        mov     ax,fs:[esi+6]
        mov     es:RealRegsStruc.Real_DS[edi],ax
        mov     eax,fs:[esi+8]
        mov     es:RealRegsStruc.Real_EBP[edi],eax
        mov     eax,fs:[esi+12]
        mov     es:RealRegsStruc.Real_EDI[edi],eax
        mov     eax,fs:[esi+16]
        mov     es:RealRegsStruc.Real_ESI[edi],eax
        mov     eax,fs:[esi+20]
        mov     es:RealRegsStruc.Real_EDX[edi],eax
        mov     eax,fs:[esi+24]
        mov     es:RealRegsStruc.Real_ECX[edi],eax
        mov     eax,fs:[esi+28]
        mov     es:RealRegsStruc.Real_EBX[edi],eax
        mov     eax,fs:[esi+32]
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     ax,fs:[esi+36]
        mov     es:RealRegsStruc.Real_Flags[edi],ax
        mov     ax,RetAdd
        mov     es:RealRegsStruc.Real_IP[edi],ax
        mov     ax,_cwRaw
        mov     es:RealRegsStruc.Real_CS[edi],ax
        mov     ax,StackAdd
        mov     es:RealRegsStruc.Real_SP[edi],ax
        mov     ax,StackAdd+2
        mov     es:RealRegsStruc.Real_SS[edi],ax
        ;
        test    BYTE PTR RawSystemFlags,1
        jz      rv30_Use32Bit12
        mov     ax,w[CallBackStruc.CallBackProt+4+bx]
        mov     w[rv30_CallB0+2],ax
        mov     eax,d[CallBackStruc.CallBackProt+bx]
        mov     w[rv30_CallB0],ax
        jmp     rv30_Use16Bit12
rv30_Use32Bit12:
        mov     ax,w[CallBackStruc.CallBackProt+4+bx]
        mov     w[rv30_CallB0+4],ax
        mov     eax,d[CallBackStruc.CallBackProt+bx]
        mov     d[rv30_CallB0],eax
rv30_Use16Bit12:
        push    bx              ;save call back structure pointer.
        ;
rv30_oops:
        ;
        ;Setup stack referance.
        ;
        push    eax
        push    ebx
        push    esi
        push    edi
        push    es
        movzx   esi,es:RealRegsStruc.Real_SS[edi]
        shl     esi,4
        mov     ax,KernalZero
        mov     es,ax
        movzx   eax,w[CallBackStruc.CallBackStackSel+bx]
        and     ax,not 7
        mov     edi,GDTLinear
        add     edi,eax
        mov     es:[edi+2],si   ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     es:[edi+4],bl   ;store mid byte of linear base.
        mov     es:[edi+7],bh   ;store high byte of linear base.
        pop     es
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        mov     ds,w[CallBackStruc.CallBackStackSel+bx]
        movzx   esi,es:RealRegsStruc.Real_SP[edi]
        ;
        assume ds:nothing
        test    BYTE PTR cs:RawSystemFlags,1
        jz      rv30_Use32Bit13
        pushf
        call    DWORD PTR cs:[rv30_CallB0]
        jmp     rv30_Use16Bit13
rv30_Use32Bit13:
        pushfd
        call    FWORD PTR cs:[rv30_CallB0]
rv30_Use16Bit13:
        cli
        assume ds:_cwRaw
        mov     ax,KernalDS             ;make our data addresable.
        mov     ds,ax
        pop     bx              ;restore call back structure.
        ;
        movzx   esi,es:RealRegsStruc.Real_SS[edi]       ;point to stacked registers.
        mov     w[VCPI_SP+2],si
        shl     esi,4
        movzx   eax,es:RealRegsStruc.Real_SP[edi]
        sub     eax,(2)+(4+4+4+4+4+4+4)+(2+2+2+2)+(2+2)
        mov     w[VCPI_SP],ax
        add     esi,eax
        mov     ax,KernalZero   ;/
        mov     fs,ax           ;/
        mov     ax,es:RealRegsStruc.Real_GS[edi]
        mov     fs:[esi],ax
        mov     ax,es:RealRegsStruc.Real_FS[edi]
        mov     fs:[esi+2],ax
        mov     ax,es:RealRegsStruc.Real_ES[edi]
        mov     fs:[esi+4],ax
        mov     ax,es:RealRegsStruc.Real_DS[edi]
        mov     fs:[esi+6],ax
        mov     eax,es:RealRegsStruc.Real_EBP[edi]
        mov     fs:[esi+8],eax
        mov     eax,es:RealRegsStruc.Real_EDI[edi]
        mov     fs:[esi+12],eax
        mov     eax,es:RealRegsStruc.Real_ESI[edi]
        mov     fs:[esi+16],eax
        mov     eax,es:RealRegsStruc.Real_EDX[edi]
        mov     fs:[esi+20],eax
        mov     eax,es:RealRegsStruc.Real_ECX[edi]
        mov     fs:[esi+24],eax
        mov     eax,es:RealRegsStruc.Real_EBX[edi]
        mov     fs:[esi+28],eax
        mov     eax,es:RealRegsStruc.Real_EAX[edi]
        mov     fs:[esi+32],eax
        mov     ax,es:RealRegsStruc.Real_Flags[edi]
        mov     fs:[esi+36],ax
        mov     ax,es:RealRegsStruc.Real_IP[edi]
        mov     fs:[esi+38],ax
        mov     ax,es:RealRegsStruc.Real_CS[edi]
        mov     fs:[esi+40],ax
        ;
        ;Switch back to v86 mode.
        ;
        mov     cx,VCPI_SP+2
        mov     dx,VCPI_SP
        call    Protected2Real
        add     RawStackPos,RawStackDif
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
        popf
        retf
        ;
rv30_CallB0:
        df ?,0
RAWCallBack     endp


;-------------------------------------------------------------------------------
RAWICallBack    proc    near
        cli
        assume ds:nothing
        pop     cs:RetAdd               ;get return address.
        ;
        ;Check if this call back is busy.
        ;
        push    ax
        push    bx
        push    dx
        mov     ax,cs:RetAdd            ;get return address.
        sub     ax,CallBackSize ;back to start of call back entry.
        sub     ax,offset ICallBackList ;offset from start of list.
        xor     dx,dx
        mov     bx,CallBackSize
        div     bx              ;entry number.
        mov     bx,size CallBackStruc
        mul     bx              ;get offset into table.
        mov     bx,offset CallBackTable
        add     bx,ax           ;point to this entry.
        mov     WORD PTR cs:[rv31_CallTab],bx
        ;
        cmp     cs:InWindows,0
        jnz     rv31_ForceOld
        ;
        test    cs:CallBackStruc.CallBackFlags[bx],128  ;call back busy?
        jz      rv31_NotBusy
        ;
        ;This is a busy int call back so pass control to old real mode vector.
        ;
rv31_ForceOld:
        mov     ax,WORD PTR cs:[CallBackStruc.CallBackReal+2+bx]        ;fetch old real mode vector address.
        mov     WORD PTR cs:[rv31_tVCPI_SP+2],ax
        mov     ax,WORD PTR cs:[CallBackStruc.CallBackReal+bx]
        mov     WORD PTR cs:[rv31_tVCPI_SP],ax
        pop     dx
        pop     bx
        pop     ax
        jmp     DWORD PTR cs:[rv31_tVCPI_SP]    ;pass onto old handler.
        ;
rv31_tVCPI_SP:
        dd ?
        ;
rv31_NotBusy:
        or      cs:CallBackStruc.CallBackFlags[bx],128  ;mark it as busy.
        mov     bx,sp
        mov     ax,ss:[bx+(2+2+2)+(2+2)]
        and     ax,0000110011010101b
        or      ax,0000000000000010b
        mov     WORD PTR cs:[rv31_FlagsStore],ax
        pop     dx
        pop     bx
        pop     ax
        ;
        ;Switch to new stack.
        ;
        mov     WORD PTR cs:[rv31_tVCPI_SP],sp  ;store current stack.
        mov     WORD PTR cs:[rv31_tVCPI_SP+2],ss
        mov     ss,cs:RawStackReal      ;_cwRaw         ;use new stack.
        mov     esp,cs:RawStackPos
        sub     cs:RawStackPos,RawStackDif
        push    DWORD PTR cs:[rv31_tVCPI_SP]    ;put old stack onto new one.
        ;
        ;Preserve registers.
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
        ;
        ;Make our data addresable.
        ;
        mov     ax,_cwRaw
        mov     ds,ax           ;make our data addresable.
        assume ds:_cwRaw
        mov     w[rv31_tVCPI_SP],sp
        mov     w[rv31_tVCPI_SP+2],ss
        ;
        ;Switch to protected mode.
        ;
        mov     cx,KernalSS
        xor     edx,edx
        mov     dx,sp
;       mov     edx,RawStackPos
;       sub     RawStackPos,RawStackDif
        call    Real2Protected
        ;
        ;Get protected mode code address.
        ;
        mov     bx,w[rv31_CallTab]
        mov     bl,CallBackStruc.CallBackNum[bx]        ;get int number.
        push    ax
        push    ebx
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        movzx   eax,bl
        mov     ebx,eax
        shl     ebx,1           ;*2
        mov     eax,ebx
        shl     ebx,1           ;*4
        add     ebx,eax         ;*6
        assume ds:_cwDPMIEMU
        add     ebx,offset InterruptTable
        assume ds:_cwRaw
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     edx,[ebx]               ;get offset.
        mov     cx,4[ebx]               ;get segment selector.
        pop     ds
        pop     ds
        pop     ebx
        pop     ax
        test    BYTE PTR RawSystemFlags,1
        jz      rv31_Use32Bit12
        mov     w[rv31_CallB0+2],cx
        mov     w[rv31_CallB0],dx
        jmp     rv31_Use16Bit12
rv31_Use32Bit12:
        mov     w[rv31_CallB0+4],cx
        mov     d[rv31_CallB0],edx
rv31_Use16Bit12:
        ;
        ;Retrieve register values.
        ;
        mov     ax,KernalZero
        mov     fs,ax
        movzx   esi,w[rv31_tVCPI_SP+2]
        shl     esi,4
        movzx   eax,w[rv31_tVCPI_SP]
        add     esi,eax         ;Point to stacked registers.
        pushfd
        pop     eax
        shr     eax,16
        mov     w[rv31_FlagsStore+2],ax
        mov     eax,fs:[esi+32]
        mov     ebx,fs:[esi+28]
        mov     ecx,fs:[esi+24]
        mov     edx,fs:[esi+20]
        mov     edi,fs:[esi+12]
        mov     ebp,fs:[esi+8]
        mov     esi,fs:[esi+16]
        push    d[rv31_tVCPI_SP]
        push    w[rv31_CallTab]
        push    ds

        test    BYTE PTR RawSystemFlags,1
        jz      rv31_Use32Bit13

        push    w[rv31_FlagsStore]
;       pushf                   ;dummy return flags.
        push    cs              ;dummy return address.
        push    w[rv31_zero]            ;/

        push    w[rv31_FlagsStore]
        call    d[rv31_CallB0]

        lea     esp,[esp+(2*3)]

;       pushf
;       add     sp,2*3
;       popf

        jmp     rv31_Use16Bit13
rv31_Use32Bit13:
;       pushfd          ;dummy return flags.
        push    d[rv31_FlagsStore]
        push    0               ;\
        push    cs              ;dummy return address.
        push    d[rv31_zero]

        push    d[rv31_FlagsStore]
        call    f[rv31_CallB0]

        lea     esp,[esp+(4*3)]

rv31_Use16Bit13:
        pop     ds
        pushfd
        cli
        pop     d[rv31_FlagsStore]
        pop     w[rv31_CallTab]
        pop     d[rv31_tVCPI_SP]
        push    esi
        push    eax
        mov     ax,KernalZero
        mov     fs,ax
        movzx   esi,w[rv31_tVCPI_SP+2]
        shl     esi,4
        movzx   eax,w[rv31_tVCPI_SP]
        add     esi,eax         ;Point to stacked registers.
        ;
        ;Set new register values.
        ;
        pop     eax
        mov     fs:[esi+32],eax
        mov     fs:[esi+28],ebx
        mov     fs:[esi+24],ecx
        mov     fs:[esi+20],edx
        pop     eax
        mov     fs:[esi+16],eax
        mov     fs:[esi+12],edi
        mov     fs:[esi+8],ebp
        ;
        ;Update flags.
        ;
        movzx   eax,WORD PTR fs:[(2+2+2+2)+(4+4+4+4+4+4+4)+esi]
        movzx   esi,WORD PTR fs:[(2+2+2+2)+(4+4+4+4+4+4+4)+(2)+esi]
        shl     esi,4
        add     esi,eax
        mov     ax,w[rv31_FlagsStore]
        and     ax,1111100011111111b
        and     WORD PTR fs:[(2+2)+esi],0000011100000000b
        or      fs:[(2+2)+esi],ax
        ;
        mov     bx,w[rv31_CallTab]      ;restore call back structure.
        and     CallBackStruc.CallBackFlags[bx],255-128 ;clear busy flag.
        ;
        ;Switch back to v86 mode.
        ;
        mov     cx,w[rv31_tVCPI_SP+2]
        mov     dx,w[rv31_tVCPI_SP]
        call    Protected2Real
;       add     RawStackPos,RawStackDif
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
        assume ds:nothing
        lss     sp,[esp]                ;restore original stack.
        add     cs:RawStackPos,RawStackDif
        assume ds:_cwRaw
        iret
;
rv31_CallB0:
        df ?,0
rv31_FlagsStore:
        dd 0
rv31_CallTab:
        dw ?
rv31_zero:
        dd 0
RAWICallBack    endp


;-------------------------------------------------------------------------------
;
;Install relavent A20 handler for this machine.
;
InstallA20      proc far
        ;
rv32_IAChkPS2:
        ; Are we on a PS/2?
        ;
        call    IsPS2Machine
        cmp     ax,1
        jne     rv32_IAOnAT
        mov     A20HandlerCall,offset A20_PS2
        jmp     rv32_0
        ;
rv32_IAOnAT:
        ;Assume we're on an AT.
        ;
        mov     A20HandlerCall,offset A20_AT
        ;
rv32_0: ret
InstallA20      endp


;-------------------------------------------------------------------------------
IsPS2Machine    proc   near
        mov     ah,0C0h         ; Get System Description Vector
        stc
        int     15h
        jc      rv33_IPMNoPS2           ; Error?  Not a PS/2.
        ;
        ; Do we have a "Micro Channel" computer?
        ;
        mov     al,byte ptr es:[bx+5]   ; Get "Feature Information Byte 1"
        test    al,00000010b            ; Test the "Micro Channel Implemented" bit
        jz      rv33_IPMNoPS2
        ;
rv33_IPMFoundIt:
        xor     ax,ax           ; Disable A20. Fixes PS2 Ctl-Alt-Del bug
        call    A20_PS2
        mov     ax,1
        ret
        ;
rv33_IPMNoPS2:
        xor     ax,ax
        ret
IsPS2Machine    endp


;-------------------------------------------------------------------------------
;
;Control A20 with whatever method is apropriate.
;
A20Handler      proc    far
        push    ds
        push    ax
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,0
        assume ds:_cwRaw
        pop     ax
        pop     ds
        jnz     rv34_A20DoneOther
        cmp     XMSPresent,0
        jz      rv34_A20Raw
        cmp     ax,2            ;restore?
        jz      rv34_XMSA20OFF
        or      ax,ax
        jnz     rv34_XMSA20ON
rv34_XMSA20OFF:
        sub     esp,size v86CallStruc.v86CallStruc
        mov     ax,ss
        mov     es,ax
        mov     edi,esp
        mov     ax,w[XMSControl]
        mov     es:RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     es:RealRegsStruc.Real_CS[edi],ax
        mov     ah,06h
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        call    RawSimulateFCALL
        add     esp,size v86CallStruc.v86CallStruc
        jmp     rv34_A20Done
        ;
rv34_XMSA20ON:
        sub     esp,size v86CallStruc.v86CallStruc
        mov     ax,ss
        mov     es,ax
        mov     edi,esp
        mov     ax,w[XMSControl]
        mov     es:RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     es:RealRegsStruc.Real_CS[edi],ax
        mov     ah,05h
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        call    RawSimulateFCALL
        mov     ax,w[XMSControl]
        mov     es:RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     es:RealRegsStruc.Real_CS[edi],ax
        mov     ah,7
        mov     es:RealRegsStruc.Real_EAX[edi],eax
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        call    RawSimulateFCALL
        mov     eax,es:RealRegsStruc.Real_EAX[edi]
        xor     ax,1
        add     esp,size v86CallStruc.v86CallStruc
        or      ax,ax
        jmp     rv34_A20Done
        ;
rv34_A20Raw:
        call    A20HandlerCall
        ;
rv34_A20Done:
        ret
rv34_A20DoneOther:
        xor  ax,ax
        ret
A20Handler      endp


;-------------------------------------------------------------------------------
A20_AT  proc    near
        or      ax,ax
        jz      rv35_AAHDisable
        cmp     ax,2
        jnz     rv35_AAHEnable
        cmp     A20Flag,0
        jz      rv35_AAHDisable
rv35_AAHEnable:
        call    rv35_Sync8042           ; Make sure the Keyboard Controller is Ready
        jnz     rv35_AAHErr
        mov     al,0D1h         ; Send D1h
        out     64h,al
        call    rv35_Sync8042
        jnz     rv35_AAHErr
        mov     al,0DFh         ; Send DFh
        out     60h,al
        call    rv35_Sync8042
        jnz     rv35_AAHErr
        ;
        ; Wait for the A20 line to settle down (up to 20usecs)
        ;
        mov     al,0FFh         ; Send FFh (Pulse Output Port NULL)
        out     64h,al
        call    rv35_Sync8042
        jnz     rv35_AAHErr
        jmp     rv35_AAHExit
        ;
rv35_AAHDisable:
        call    rv35_Sync8042           ; Make sure the Keyboard Controller is Ready
        jnz     rv35_AAHErr
        mov     al,0D1h         ; Send D1h
        out     64h,al
        call    rv35_Sync8042
        jnz     rv35_AAHErr
        mov     al,0DDh         ; Send DDh
        out     60h,al
        call    rv35_Sync8042
        jnz     rv35_AAHErr
        ;
        ; Wait for the A20 line to settle down (up to 20usecs)
        ;
        mov     al,0FFh         ; Send FFh (Pulse Output Port NULL)
        out     64h,al
        call    rv35_Sync8042
rv35_AAHExit:
        xor     ax,ax
        ret
rv35_AAHErr:
        mov     ax,1
        or      ax,ax
        ret
        ;
rv35_Sync8042:
        xor     cx,cx
rv35_S8InSync:
        in      al,64h
        and     al,2
        loopnz rv35_S8InSync
        ret
A20_AT  endp


;-------------------------------------------------------------------------------
A20_PS2 proc    near
PS2_PORTA       equ     0092h
PS2_A20BIT      equ     00000010b
        or      ax,ax
        jz      rv36_PAHDisable
        cmp     ax,2
        jnz     rv36_PAHEnable
        cmp     A20Flag,0
        jz      rv36_PAHDisable
        ;
rv36_PAHEnable:
        in      al,PS2_PORTA            ; Get the current A20 state
        test    al,PS2_A20BIT   ; Is A20 already on?
        jnz     rv36_PAHErr
        ;
        or      al,PS2_A20BIT   ; Turn on the A20 line
        out     PS2_PORTA,al
        ;
        xor     cx,cx           ; Make sure we loop for awhile
rv36_PAHIsItOn:
        in      al,PS2_PORTA            ; Loop until the A20 line comes on
        test    al,PS2_A20BIT
        loopz   rv36_PAHIsItOn
        jz      rv36_PAHErr             ; Unable to turn on the A20 line
        jmp     rv36_PAHExit
        ;
rv36_PAHDisable:
        in      al,PS2_PORTA            ; Get the current A20 state
        and     al,NOT PS2_A20BIT       ; Turn off the A20 line
        out     PS2_PORTA,al
        ;
        xor     cx,cx           ; Make sure we loop for awhile
rv36_PAHIsItOff:
        in      al,PS2_PORTA            ; Loop until the A20 line goes off
        test    al,PS2_A20BIT
        loopnz rv36_PAHIsItOff
        jnz     rv36_PAHErr             ; Unable to turn off the A20 line
        ;
rv36_PAHExit:
        xor     ax,ax
        ret
        ;
rv36_PAHErr:
        mov     ax,1
        or      ax,ax
        ret
A20_PS2 endp


_cwRaw  ends


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Second level section for RAW/VCPI stuff. This holds the higher level management
;code like interrupt/exception handlers, memory management and DPMI emulation.
;This gets moved into extended memory during initialisation to keep conventional
;memory foot print down.
;
_cwDPMIEMU      segment para public 'DPMI emulator code' use32
        assume cs:_cwDPMIEMU, ds:_cwDPMIEMU
cwDPMIEMUStart  label byte


;-------------------------------------------------------------------------------
;
;Call _cwRaw SimulateINT
;
EmuRawSimulateInt proc near
        assume ds:nothing
        db 66h
        call    FWORD PTR cs:[rv38_calladd]
        assume ds:_cwDPMIEMU
        ret
        ;
rv38_calladd:
        dw offset _fRawSimulateINT,KernalCS
EmuRawSimulateInt endp


;-------------------------------------------------------------------------------
;
;Call _cwRaw SimulateINT
;
EmuRawSimulateInt2 proc near
        assume ds:nothing
        db 66h
        call    FWORD PTR cs:[rv39_calladd]
        assume ds:_cwDPMIEMU
        ret
        ;
rv39_calladd:
        dw offset _fRawSimulateINT2,KernalCS
EmuRawSimulateInt2 endp


;-------------------------------------------------------------------------------
;
;Call _cwRaw SimulateFCALL
;
EmuRawSimulateFCall proc near
        assume ds:nothing
        db 66h
        call    FWORD PTR cs:[rv40_calladd]
        assume ds:_cwDPMIEMU
        ret
        ;
rv40_calladd:
        dw offset _fRawSimulateFCALL,KernalCS
EmuRawSimulateFCall endp


;-------------------------------------------------------------------------------
;
;Call _cwRaw SimulateFCALL
;
EmuRawSimulateFCall2 proc near
        assume ds:nothing
        db 66h
        call    FWORD PTR cs:[rv41_calladd]
        assume ds:_cwDPMIEMU
        ret
        ;
rv41_calladd:
        dw offset _fRawSimulateFCALL2,KernalCS
EmuRawSimulateFCall2 endp


;-------------------------------------------------------------------------------
;
;Call _cwRaw SimulateFCALLI
;
EmuRawSimulateFCallI proc near
        assume ds:nothing
        db 66h
        call    FWORD PTR cs:[rv42_calladd]
        assume ds:_cwDPMIEMU
        ret
        ;
rv42_calladd:
        dw offset _fRawSimulateFCALLI,KernalCS
EmuRawSimulateFCallI endp


;-------------------------------------------------------------------------------
EmuCR3Flush     proc    near
        pushf
        cli
        pushad
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        push    d[rv43_StackAdd]
        push    w[rv43_StackAdd+4]
        push    d[erp0RetAdd]
        push    d[erp3RetAdd]
        ;
        mov     d[rv43_StackAdd],esp
        mov     w[rv43_StackAdd+4],ss
        call    EmuRAWPL32PL0
        ;
        mov     eax,cr3
        mov     cr3,eax         ;flush page cache.
        ;
        mov     edx,d[rv43_StackAdd]
        mov     cx,w[rv43_StackAdd+4]
        call    EmuRAWPL02PL3
        ;
        pop     d[erp3RetAdd]
        pop     d[erp0RetAdd]
        pop     w[rv43_StackAdd+4]
        pop     d[rv43_StackAdd]
        pop     ds
        popad
        popf
        ret
        ;
rv43_StackAdd:
        df ?
EmuCR3Flush     endp


;-------------------------------------------------------------------------------
EmuRAWPL02PL3   proc    near
        pop     d[erp0RetAdd]
        movzx   ecx,cx
        push    ecx             ;SS
        push    edx             ;ESP
        pushfd          ;EFlags
        pop     eax
        and     ax,1000111111111111b    ;clear NT & IOPL.
        or      ax,0011000000000000b    ;force IOPL.
        push    eax
        popfd
        push    eax
        xor     eax,eax
        mov     ax,DpmiEmuCS
        push    eax             ;CS
        mov     eax,offset rv44_pl3
        push    eax             ;EIP
        iretd
rv44_pl3:
        push    d[erp0RetAdd]
        ret
EmuRAWPL02PL3   endp


erp0RetAdd:
        dd ?

;-------------------------------------------------------------------------------
EmuRawPL32PL0   proc    near
        pop     d[erp3RetAdd]
        push    edi
        push    eax
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     edi,GDTLinear
        assume ds:_cwDPMIEMU
        pop     ds
        pop     eax
        add     edi,DpmiEmuPL3_2_PL0
        and     edi,not 7
        push    es
        push    ax
        mov     ax,KernalZero
        mov     es,ax
        mov     eax,offset rv45_pl0
        mov     WORD PTR es:[edi],ax            ;store low word of limit.
        pop     ax
        pop     es
        pop     edi
        db 9ah
        dd offset rv45_pl0,DpmiEmuPL3_2_PL0 ;instruction pre-fetch & load CS.
rv45_pl0:
        push    d[erp3RetAdd]
        ret
EmuRawPL32PL0   endp


erp3RetAdd:
        dd ?

;-------------------------------------------------------------------------------
;
;DPMI int 31h service simulation patch. This provides DPMI API services in a
;Raw/Vcpi environment.
;
RawDPMIPatch    proc    far
        or      ah,ah
        jz      rv46_DPMI_0000
        cmp     ah,1
        jz      rv46_DPMI_0100
        cmp     ah,2
        jz      rv46_DPMI_0200
        cmp     ah,3
        jz      rv46_DPMI_0300
        cmp     ah,4
        jz      rv46_DPMI_0400
        cmp     ah,5
        jz      rv46_DPMI_0500
        cmp     ah,6
        jz      rv46_DPMI_0600
        cmp     ah,7
        jz      rv46_DPMI_0700
        cmp     ah,8
        jz      rv46_DPMI_0800
        cmp     ah,9
        jz      rv46_DPMI_0900
        cmp     ah,0ah
        jz      rv46_DPMI_0A00
        cmp     ah,0bh
        jz      rv46_DPMI_0B00
        jmp     rv46_NotOurs
        ;
rv46_DPMI_0000:
        cmp     al,00h          ;Allocate LDT descriptors?
        jnz     rv46_DPMI_0001
        push    ebx
        push    eax
        call    RawGetDescriptors
        pop     ebx
        mov     bx,ax
        mov     eax,ebx
        pop     ebx
        clc                     ; MED, 01/31/99
        jmp     rv46_Done
        ;
rv46_DPMI_0001:
        cmp     al,01h          ;Free LDT descriptor?
        jnz     rv46_DPMI_0002
        call    RawRelDescriptor
        jmp     rv46_Done
        ;
rv46_DPMI_0002:
        cmp     al,02h          ;Real segment to protected selector?
        jnz     rv46_DPMI_0003
        push    ebx
        push    eax
        call    RawReal2ProtSel
        pop     ebx
        mov     bx,ax
        mov     eax,ebx
        pop     ebx
        jmp     rv46_Done
        ;
rv46_DPMI_0003:
        cmp     al,03h          ;Get selector increment value?
        jnz     rv46_DPMI_0004
        mov     ax,8
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0004:
rv46_DPMI_0005:
rv46_DPMI_0006:
        cmp     al,06h          ;Get selector base address?
        jnz     rv46_DPMI_0007
        call    RawGetSelBase
        jmp     rv46_Done
        ;
rv46_DPMI_0007:
        cmp     al,07h          ;Set selector base address?
        jnz     rv46_DPMI_0008
        call    RawSetSelBase
        jmp     rv46_Done
        ;
rv46_DPMI_0008:
        cmp     al,08h          ;Set segment limit?
        jnz     rv46_DPMI_0009
        call    RawSetSelLimit
        jmp     rv46_Done
        ;
rv46_DPMI_0009:
        cmp     al,09h          ;Set access rights bytes?
        jnz     rv46_DPMI_000A
        call    RawSetSelType
        jmp     rv46_Done
        ;
rv46_DPMI_000A:
        cmp     al,0Ah          ;create data alias of CS?
        jnz     rv46_DPMI_000B
        push    ebx
        push    eax
        push    ecx
        mov     cx,1
        call    RawGetDescriptors
        pop     ecx
        jnc     rv46_000A_0
        pop     eax
        pop     ebx
        jmp     rv46_Done
;
rv46_000A_0:
        push    eax
        push    ebx
        push    edi
        push    es
        push    eax
        mov     edi,offset RawSelBuffer
        mov     ax,KernalDS     ;DpmiEmuDS
        mov     es,ax
        call    RawBGetDescriptor       ;copy original details.
        mov     BYTE PTR es:[edi+5],DescPresent+DescPL3+DescMemory+DescRWData
        pop     ebx
        call    RawBPutDescriptor       ;set new descriptor details.
        pop     es
        pop     edi
        pop     ebx
        pop     eax
;
        pop     ebx
        mov     bx,ax
        mov     eax,ebx
        pop     ebx
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_000B:
        cmp     al,0Bh          ;fetch descriptor?
        jnz     rv46_DPMI_000C
        call    RawBGetDescriptor
        jmp     rv46_Done
        ;
rv46_DPMI_000C:
        cmp     al,0Ch          ;put descriptor?
        jnz     rv46_DPMI_000D
        call    RawBPutDescriptor
        jmp     rv46_Done
        ;
rv46_DPMI_000D:
        cmp     al,0Dh          ;allocate specific LDT descriptor?
        jnz     rv46_NotOurs
        stc
        jmp     rv46_Done
        ;
rv46_DPMI_0100:
        cmp     al,00h          ;allocate DOS memory?
        jnz     rv46_DPMI_0101
        push    ebp
        push    eax
        push    ebx
        push    edx
        call    RawGetDOSMemory
        jc      rv46_0100_0
        pop     ebp
        mov     bp,dx
        mov     edx,ebp
        pop     ebx
        pop     ebp
        mov     bp,ax
        mov     eax,ebp
        pop     ebp
        jmp     rv46_Done
rv46_0100_0:
        pop     edx
        pop     ebp
        mov     bp,bx
        mov     ebx,ebp
        pop     ebp
        mov     bp,ax
        mov     eax,ebp
        pop     ebp
        jmp     rv46_Done
        ;
rv46_DPMI_0101:
        cmp     al,01h          ;free DOS memory?
        jnz     rv46_DPMI_0102
        push    ebx
        push    eax

; MED 02/03/2003, Hey, they fixed this later, but I'm leaving the comment and
;  code here as a snapshot in the past of a display of semi-righteous anger
;  and to keep CW compatible with all those people using old Watcom versions.
;  Heck, it was intended for internal viewing only at the time. (RIP Powersoft)
; MED 03/25/97
; Watcom 11.0 is excessively stupid and attempts at startup to release
;  memory used by the stack and DGROUP via DOS memory free call.  Windows 95
;  fails this attempt with a carry flag set (but invalid error code, ax not
;  updated).  CauseWay will now have to check if the release selector in
;  DX is the same as SS and fail the call if so.  Thanks Powersoft.
        mov     ax,ss
        cmp     ax,dx
        pop     eax
        push    eax
        jne     med2_0101       ; not attempting to release SS selector
        stc                             ; flag failure, but no error code update
        jmp     med3_0101

med2_0101:
        call    RawRelDOSMemory
        jc      rv46_0101_0

med3_0101:
        pop     eax
        pop     ebx
        jmp     rv46_Done
rv46_0101_0:
        pop     ebx
        mov     bx,ax
        mov     eax,ebx
        pop     ebx
        jmp     rv46_Done
        ;
rv46_DPMI_0102:
        cmp     al,02h          ;re-size DOS memory?
        jnz     rv46_NotOurs
        push    ebp
        push    eax
        push    ebx
        call    RawResDOSMemory
        jc      rv46_0102_0
        pop     ebx
        pop     eax
        pop     ebp
        jmp     rv46_Done
rv46_0102_0:
        pop     ebp
        mov     bp,bx
        mov     ebx,ebp
        pop     ebp
        mov     bp,ax
        mov     eax,ebp
        pop     ebp
        jmp     rv46_Done
        ;
rv46_DPMI_0200:
        cmp     al,00h          ;get real mode vector?
        jnz     rv46_DPMI_0201
        call    RawGetRVector
        jmp     rv46_Done
        ;
rv46_DPMI_0201:
        cmp     al,01h          ;set real mode vector?
        jnz     rv46_DPMI_0202
        call    RawSetRVector
        jmp     rv46_Done
        ;
rv46_DPMI_0202:
        cmp     al,02h          ;get exception vector?
        jnz     rv46_DPMI_0203
        push    eax
        push    ecx
        push    edx
        call    RawGetEVector
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0202_0
        pop     eax
        mov     ax,dx
        mov     edx,eax
        jmp     rv46_0202_1
rv46_0202_0:
        pop     eax
rv46_0202_1:
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0203:
        cmp     al,03h          ;set exception vector?
        jnz     rv46_DPMI_0204
        push    edx
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0203_0
        movzx   edx,dx
rv46_0203_0:
        call    RawSetEVector
        pop     edx
        jmp     rv46_Done
        ;
rv46_DPMI_0204:
        cmp     al,04h          ;get vector?
        jnz     rv46_DPMI_0205
        push    eax
        push    ecx
        push    edx
        call    RawGetVector
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0204_0
        pop     eax
        mov     ax,dx
        mov     edx,eax
        jmp     rv46_0204_1
rv46_0204_0:
        pop     eax
rv46_0204_1:
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0205:
        cmp     al,05h          ;set vector?
        jnz     rv46_NotOurs
        push    edx
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0205_0
        movzx   edx,dx
rv46_0205_0:
        call    RawSetVector
        pop     edx
        jmp     rv46_Done
        ;
rv46_DPMI_0300:
        cmp     al,00h          ;Simulate int?
        jnz     rv46_DPMI_0301
;
;Extend [E]DI to EDI
;
        push    eax
        push    edi
        push    ebp
        mov     ebp,esp
        add     ebp,(4+4+4)+(4+4+4)
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0300_0a
        movzx   edi,di
        mov     ebp,esp
        movzx   ebp,bp
        add     ebp,(4+4+4)+(2+2+2)
;
;Simulate the INT
;
rv46_0300_0a:
        push    es:RealRegsStruc.Real_CS[edi]
        push    es:RealRegsStruc.Real_IP[edi]
        push    es:RealRegsStruc.Real_SS[edi]
        push    es:RealRegsStruc.Real_SP[edi]
        call    EmuRawSimulateInt2
        pop     es:RealRegsStruc.Real_SP[edi]
        pop     es:RealRegsStruc.Real_SS[edi]
        pop     es:RealRegsStruc.Real_IP[edi]
        pop     es:RealRegsStruc.Real_CS[edi]
;
;Mask real mode register structure flags.
;
        pushf
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0300_0
        mov     bx,sp
        mov     bx,ss:[bx+(4+4+4+2)+(2+2)]      ;get original flags.
        jmp     rv46_0300_1
rv46_0300_0:
        mov     bx,ss:[esp+(4+4+4+2)+(4+4)]     ;get original flags.
        and     bx,0000111000000000b            ;retain IF.
rv46_0300_1:
        and     es:RealRegsStruc.Real_Flags[edi],1111000111111111b      ;lose IF.
        or      es:RealRegsStruc.Real_Flags[edi],bx
        popf
;
        pop     ebp
        pop     edi
        pop     eax
        jmp     rv46_Done



rv46_DPMI_0301:
        cmp     al,01h          ;simulate far call?
        jnz     rv46_DPMI_0302
;
;Extend [E]DI to EDI
;
        push    eax
        push    edi
        push    ebp
        mov     ebp,esp
        add     ebp,(4+4+4)+(4+4+4)
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0301_0a
        movzx   edi,di
        mov     ebp,esp
        movzx   ebp,bp
        add     ebp,(4+4+4)+(2+2+2)
;
;Simulate the far call
;
rv46_0301_0a:
        push    es:RealRegsStruc.Real_CS[edi]
        push    es:RealRegsStruc.Real_IP[edi]
        push    es:RealRegsStruc.Real_SS[edi]
        push    es:RealRegsStruc.Real_SP[edi]
        call    EmuRawSimulateFCall2
        pop     es:RealRegsStruc.Real_SP[edi]
        pop     es:RealRegsStruc.Real_SS[edi]
        pop     es:RealRegsStruc.Real_IP[edi]
        pop     es:RealRegsStruc.Real_CS[edi]
;
;Mask real mode register structure flags.
;
        pushf
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0301_0
        mov     bx,sp
        mov     bx,ss:[bx+(4+4+4+2)+(2+2)]      ;get original flags.
        jmp     rv46_0301_1
rv46_0301_0:
        mov     bx,ss:[esp+(4+4+4+2)+(4+4)]     ;get original flags.
        and     bx,0000111000000000b            ;retain IF.
rv46_0301_1:
        and     es:RealRegsStruc.Real_Flags[edi],1111000111111111b      ;lose IF.
        or      es:RealRegsStruc.Real_Flags[edi],bx
        popf
;
        pop     ebp
        pop     edi
        pop     eax
        jmp     rv46_Done


rv46_DPMI_0302:
        cmp     al,02h          ;call real mode with iret stack frame?
        jnz     rv46_DPMI_0303
;
;Extend [E]DI to EDI
;
        push    eax
        push    edi
        push    ebp
        mov     ebp,esp
        add     ebp,(4+4+4)+(4+4+4)
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0302_0a
        movzx   edi,di
        mov     ebp,esp
        movzx   ebp,bp
        add     ebp,(4+4+4)+(2+2+2)
;
;Simulate the far call
;
rv46_0302_0a:
        push    es:RealRegsStruc.Real_CS[edi]
        push    es:RealRegsStruc.Real_IP[edi]
        push    es:RealRegsStruc.Real_SS[edi]
        push    es:RealRegsStruc.Real_SP[edi]
        call    EmuRawSimulateFCallI
        pop     es:RealRegsStruc.Real_SP[edi]
        pop     es:RealRegsStruc.Real_SS[edi]
        pop     es:RealRegsStruc.Real_IP[edi]
        pop     es:RealRegsStruc.Real_CS[edi]
;
;Mask real mode register structure flags.
;
        pushf
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_0302_0
        mov     bx,sp
        mov     bx,ss:[bx+(4+4+4+2)+(2+2)]      ;get original flags.
        jmp     rv46_0302_1
rv46_0302_0:
        mov     bx,ss:[esp+(4+4+4+2)+(4+4)]     ;get original flags.
        and     bx,0000111000000000b            ;retain IF.
rv46_0302_1:
        and     es:RealRegsStruc.Real_Flags[edi],1111000111111111b      ;lose IF.
        or      es:RealRegsStruc.Real_Flags[edi],bx
        popf
;
        pop     ebp
        pop     edi
        pop     eax
        jmp     rv46_Done

        ;
rv46_DPMI_0303:
        cmp     al,03h          ;get CallBack?
        jnz     rv46_DPMI_0304
        push    eax
        push    ecx
        push    edx
        call    RawGetCallBack
        pop     eax
        mov     ax,dx
        mov     edx,eax
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        jmp     rv46_Done
        ;
rv46_DPMI_0304:
        cmp     al,04h          ;release CallBack?
        jnz     rv46_DPMI_0305
        call    RawRelCallBack
        jmp     rv46_Done
        ;
rv46_DPMI_0305:
        cmp     al,05h          ;get state save restore address?
        jnz     rv46_DPMI_0306
        mov     ax,4
        mov     bx,0
        mov     cx,bx
        mov     si,cs
        mov     edi,offset StateSaveCode
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0306:
        cmp     al,06h          ;get raw mode switch address.
        jnz     rv46_NotOurs
        stc
        jmp     rv46_Done
        ;
rv46_DPMI_0400:
        cmp     al,00h          ;get DPMI version?
        jnz     rv46_NotOurs
        mov     ah,0
        mov     al,90                                   ; changed from 90h to 90 decimal, MED 01/24/96

; MED 01/25/96
;       mov     bx,00000111b
        mov     bx,00000011b
; see if should turn on virtual memory supported bit 2
        push    ds
        mov     dx,KernalDS
        mov     ds,dx
        assume ds:_cwRaw
        mov     dx,ds:[RawSystemFlags]
        pop     ds
        assume ds:_cwDPMIEMU
        and     dx,1 shl 1              ; isolate VMM bit
        shl     dx,1                    ; shift to proper position
        or      bx,dx                   ; turn on VMM bit if set in SystemFlags

; MED 01/25/96
;       mov     cl,3
        push    eax
        push    edx
        pushfd
        pop     eax
        mov     edx,eax
        xor     eax,00040000H
        push    eax
        popfd
        pushfd
        pop     eax
        and     eax,00040000H
        and     edx,00040000H
        mov     cl,3            ; flag 386
        cmp     eax,edx
        je      medcpu

        push    edx
        popfd
        pushfd
        pop     eax
        mov     edx,eax
        xor     eax,00200000H
        push    eax
        popfd
        pushfd
        pop     eax
        push    edx
        popfd
        and     eax,00200000H
        and     edx,00200000H
        mov     cl,4            ; flag 486
        cmp     eax,edx
        je      medcpu

        mov     cl,5            ; flag 586/Pentium

medcpu:
        pop     edx
        pop     eax

        mov     dh,08h
        mov     dl,70h
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0500:
        cmp     al,00h          ;get memory information?
        jnz     rv46_DPMI_0501

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    ebp
        push    edi
        push    es
        assume ds:nothing
        call    RawGetMemoryMax
        pop     es
        pop     edi

        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwRaw
        jz      rv46_0
        movzx   edi,di
rv46_0: mov     DWORD PTR es:[edi+0],ebx
        shr     ebx,12
        mov     DWORD PTR es:[edi+04h],ebx

        push    ds
        mov     ax,KernalDS
        mov     ds,ax

        call PhysicalGetPages   ;get number of un-claimed pages.
        add     edx,NoneLockedPages     ;include currently un-locked pages.
        mov eax,edx
        shr eax,10                              ;work out page tables needed for this.
        shl eax,1                               ;2 pages per table.
        sub edx,eax                             ;lose page tables.
        cmp edx,ebx
        jc rv46_500_0
        mov edx,ebx
rv46_500_0:
        mov     DWORD PTR es:[edi+08h],edx

; MED 01/25/96
;       mov     eax,LinearLimit
;       sub     eax,LinearBase
;       shr     eax,12
;       mov     es:d[edi+0ch],eax

        mov     eax,NoneLockedPages
        mov     DWORD PTR es:[edi+10h],eax

        ;
        ;Get free disk space remaining.
        ;
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        xor     edx,edx
        cmp     VMMHandle,0
        jz      rv46_500_1
        mov     dl,VMMName              ;get drive letter for this media.
        sub     dl,'A'          ;make it real.
        inc     dl              ;adjust for current type select.
        mov     ah,36h          ;get free space.
        int     21h             ;/
        xor     edx,edx
        cmp     ax,-1           ;invalid drive?
        jz      rv46_500_1
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
        ;
rv46_500_1:
        mov     ebx,edx         ; MED 01/25/96
;       add     edx,FreePages
        mov     edx,FreePages   ; MED 01/25/96

        push    edx
        call    PhysicalGetPages

; MED 01/25/96
        pop     eax                     ; save edx value off of stack
        pop     edi                     ; restore original edi value
        push    eax             ; restore edx value to stack
        test    ebx,ebx ; see if any virtual memory
        jnz     med2
        mov     ebx,edx
med2:
        mov     eax,LinearLimit
        sub     eax,LinearBase
        shr     eax,12
        add     ebx,eax
        mov     DWORD PTR es:[edi+0ch],ebx

        mov     eax,edx
        pop     edx
        add     edx,eax
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        mov     DWORD PTR es:[edi+14h],edx

; MED 01/25/96
;       mov     es:d[edi+1ch],edx
        mov     eax,DWORD PTR es:[edi+0ch]
        sub     eax,medAllocPages

; MED 02/15/96
        mov     edx,MaxMemLin
        shr     edx,12
        sub     edx,medAllocPages
        cmp     eax,edx                         ; see if greater than MAXMEM choke-off point
        jbe     med3
        mov     eax,edx

med3:
        mov     DWORD PTR es:[edi+1ch],eax

        add     DWORD PTR es:[edi+10h],edx      ; MED 01/25/96

        mov     eax,TotalPages
        add     eax,TotalPhysical       ; MED 01/25/96
        mov     DWORD PTR es:[edi+18h],eax

        mov     eax,SwapFileLength
        shr     eax,12
        mov     DWORD PTR es:[edi+20h],eax

        mov     DWORD PTR es:[edi+24h],-1
        mov     DWORD PTR es:[edi+28h],-1
        mov     DWORD PTR es:[edi+2ch],-1

        pop     ds

        pop     ebp
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0501:
        cmp     al,01h          ;get memory block?
        jnz     rv46_DPMI_0502
        push    eax
        push    esi
        push    edi
        push    ebx
        push    ecx
        call    RawGetMemory
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        mov     ax,bx
        mov     ebx,eax
        pop     eax
        mov     ax,di
        mov     edi,eax
        pop     eax
        mov     ax,si
        mov     esi,eax
        pop     eax
        jmp     rv46_Done
        ;
rv46_DPMI_0502:
        cmp     al,02h          ;free memory block?
        jnz     rv46_DPMI_0503
        call    RawRelMemory
        jmp     rv46_Done
        ;
rv46_DPMI_0503:
        cmp     al,03h          ;re-size memory block?
        jnz     rv46_NotOurs
        push    eax
        push    esi
        push    edi
        push    ebx
        push    ecx
        call    RawResMemory
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        mov     ax,bx
        mov     ebx,eax
        pop     eax
        mov     ax,di
        mov     edi,eax
        pop     eax
        mov     ax,si
        mov     esi,eax
        pop     eax
        jmp     rv46_Done
        ;
rv46_DPMI_0600:
        cmp     al,00h          ;lock memory?
        jnz     rv46_DPMI_0601
        call    RawLockMemory
        jmp     rv46_Done
        ;
rv46_DPMI_0601:
        cmp     al,01h          ;un-lock memory?
        jnz     rv46_DPMI_0602
        call    RawUnLockMemory
        jmp     rv46_Done
        ;
rv46_DPMI_0602:
        cmp     al,02h          ;mark real mode region as swapable?
        jnz     rv46_DPMI_0603
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0603:
        cmp     al,03h          ;re-lock real mode region?
        jnz     rv46_DPMI_0604
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0604:
        cmp     al,04h          ;get page size?
        jnz     rv46_NotOurs
        xor     bx,bx
        mov     cx,4096
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0700:
rv46_DPMI_0701:
rv46_DPMI_0702:
        cmp     al,02h          ;mark page as demand pageing?
        jnz     rv46_DPMI_0703
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0703:
        cmp     al,03h          ;discard page contents?
        jnz     rv46_NotOurs
        call    RawDiscardPages
        jmp     rv46_Done
        ;
rv46_DPMI_0800:
        cmp     al,00h          ;map physical to linear?
        jnz     rv46_DPMI_0801
        push    eax
        push    ebx
        push    ecx
        call    RawMapPhys2Lin
        pop     eax
        mov     ax,cx
        mov     ecx,eax
        pop     eax
        mov     ax,bx
        mov     ebx,eax
        pop     eax
        jmp     rv46_Done
        ;
rv46_DPMI_0801:
        cmp     al,01h          ;un-map physical to linear?
        jnz     rv46_NotOurs
        call    RawUnMapPhys2Lin
        jmp     rv46_Done
        ;
rv46_DPMI_0900:
        cmp     al,00h          ;get & disable virtual interupts.
        jnz     rv46_DPMI_0901
        push    ebp
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_1
        movzx   ebp,bp
        mov     al,[ebp+(4)+(2+2+2)+(2+2)+1]
        and     al,2
        shr     al,1
        and     b[ebp+(4)+(2+2+2)+(2+2)+1],255-2
        pop     ebp
        clc
        jmp     rv46_Done
rv46_1: mov     al,[ebp+(4)+(4+4+4)+(4+4)+1]
        and     al,2
        shr     al,1
        and     b[ebp+(4)+(4+4+4)+(4+4)+1],255-2
        pop     ebp
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0901:
        cmp     al,01h          ;get & enable virtual interupts.
        jnz     rv46_DPMI_0902
        push    ebp
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_2
        movzx   ebp,bp
        mov     al,[ebp+(4)+(2+2+2)+(2+2)+1]
        and     al,2
        shr     al,1
        or      b[ebp+(4)+(2+2+2)+(2+2)+1],2
        pop     ebp
        clc
        jmp     rv46_Done
rv46_2: mov     al,[ebp+(4)+(4+4+4)+(4+4)+1]
        and     al,2
        shr     al,1
        or      b[ebp+(4)+(4+4+4)+(4+4)+1],2
        pop     ebp
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0902:
        cmp     al,02h          ;get virtual interupt state?
        jnz     rv46_NotOurs
        push    ebp
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_3
        movzx   ebp,bp
        mov     al,[ebp+(4)+(2+2+2)+(2+2)+1]
        and     al,2
        shr     al,1
        pop     ebp
        clc
        jmp     rv46_Done
rv46_3: mov     al,[ebp+(4)+(4+4+4)+(4+4)+1]
        and     al,2
        shr     al,1
        pop     ebp
        clc
        jmp     rv46_Done
        ;
rv46_DPMI_0A00:
        cmp     al,00h          ;get vendor specific API?
        jnz     rv46_NotOurs

;MED, 11/30/95
; tell inquiring Watcom that CauseWay is DOS4/GW so that it sets up
;  the FPU emulation properly
        push    edi             ; maintain in case of failure (can be changed otherwise)
        push    esi
        push    ds
;       mov     ax,_cwMain
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        test    DOS4GFlag,-1
        assume ds:_cwDPMIEMU
        pop     ds
        je      DPMI_0A00_NotDOS4G

        mov     edi,OFFSET RationalCopyright

DPMI_0A00_loop:
        mov     al,ds:[esi]
        cmp     al,cs:[edi]
        jne     DPMI_0A00_NotDOS4G
        test    al,al
        je      DPMI_0A00_match
        inc     esi
        inc     edi
        jmp     DPMI_0A00_loop

DPMI_0A00_match:
        pop     esi
        pop     edi
        push    cs
        pop     es
        mov     edi,OFFSET DPMI_0A00_APIEntryPoint      ; es:edi -> dummy extension entry point
        clc                                     ; flag success
        jmp     rv46_Done

DPMI_0A00_NotDOS4G:
        pop     esi
        pop     edi

        mov     ax,8001h
        stc
        jmp     rv46_Done

; dummy entry point
DPMI_0A00_APIEntryPoint:
        stc
        retf
RationalCopyright       DB      "RATIONAL DOS/4G",0
;
;Set hardware break point.
;
rv46_DPMI_0B00:
        cmp     al,00h          ;set debug watch point?
        jnz     rv46_DPMI_0B01
        push    eax
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    ebx
        ;
        ;Check size value
        ;
        cmp     dl,0
        jz      rv46_0B00_9
        cmp     dl,3
        jz      rv46_0B00_9
        cmp     dl,4+1
        jnc     rv46_0B00_9
        ;
        ;Check type value
        ;
        cmp     dh,2+1
        jnc     rv46_0B00_9
        ;
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        shl     ebx,16
        mov     bx,cx
        ;
        ;Find a free table entry.
        ;
        mov     esi,offset DbgTable
        mov     ecx,4
        xor     edi,edi
rv46_0B00_0:
        test    Dbg.Dbg_Flags[esi],128  ;free?
        jz      rv46_0B00_1
        inc     edi
        add     esi,size Dbg
        dec     ecx
        jnz     rv46_0B00_0
        jmp     rv46_0B00_9
rv46_0B00_1:
        mov     Dbg.Dbg_Flags[esi],128
        mov     Dbg.Dbg_Address[esi],ebx
        mov     Dbg.Dbg_Size[esi],dl
        mov     Dbg.Dbg_Type[esi],dh
        mov     ebx,edi
        ;
        ;Program the hardware.
        ;
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     ebp,esp
        mov     di,ss
        call    EmuRAWPL32PL0
        ;
        mov     ax,KernalDS
        mov     ds,ax
        ;
        mov     eax,Dbg.Dbg_Address[esi]
        or      ebx,ebx
        jz      rv46_0B00_d0
        cmp     ebx,1
        jz      rv46_0B00_d1
        cmp     ebx,2
        jz      rv46_0B00_d2
        ;
rv46_0B00_d3:
        mov     dr3,eax
        mov     eax,dr6
        and     eax,not 8
        mov     dr6,eax
        jmp     rv46_0B00_2
rv46_0B00_d2:
        mov     dr2,eax
        mov     eax,dr6
        and     eax,not 4
        mov     dr6,eax
        jmp     rv46_0B00_2
rv46_0B00_d1:
        mov     dr1,eax
        mov     eax,dr6
        and     eax,not 2
        mov     dr6,eax
        jmp     rv46_0B00_2
rv46_0B00_d0:
        mov     dr0,eax
        mov     eax,dr6
        and     eax,not 1
        mov     dr6,eax
        ;
rv46_0B00_2:
        ;Set length/type/enable
        ;
        xor     edx,edx
        mov     dl,Dbg.Dbg_Size[esi]
        mov     dl,b[edx+DbgSize2Dr]
        shl     dl,2
        xor     eax,eax
        mov     al,Dbg.Dbg_Type[esi]
        mov     al,b[eax+DbgType2Dr]
        or      dl,al
        mov     ecx,ebx
        shl     ecx,2
        add     ecx,16
        shl     edx,cl
        mov     ecx,ebx
        shl     ecx,1
        mov     eax,3
        shl     eax,cl
        or      edx,eax
        mov     eax,15
        mov     ecx,ebx
        shl     ecx,2
        add     ecx,16
        shl     eax,cl
        push    eax
        mov     ecx,ebx
        shl     ecx,1
        mov     eax,3
        shl     eax,cl
        mov     ecx,eax
        pop     eax
        or      ecx,eax
        not     ecx
        ;
        mov     eax,dr7
        and     eax,ecx
        or      eax,edx
        mov     dr7,eax
        ;
        mov     edx,ebp
        mov     cx,di
        call    EmuRAWPL02PL3
        pop     ds
        ;
        mov     eax,ebx
        pop     ebx
        mov     bx,ax
        clc
        jmp     rv46_0B00_10
        ;
rv46_0B00_9:
        pop     ebx
        stc
rv46_0B00_10:
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     eax
        assume ds:_cwDPMIEMU
        jmp     rv46_Done
;
;Release hardware break point.
;
rv46_DPMI_0B01:
        cmp     al,01h          ;clear debug watch point?
        jnz     rv46_DPMI_0B02
        pushad
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        ;Check handle range.
        ;
        movzx   ebx,bx
        cmp     ebx,3+1
        jnc     rv46_0B01_9
        ;
        ;Point to Dbg entry.
        ;
        mov     eax,size Dbg
        mul     ebx
        add     eax,offset DbgTable
        mov     esi,eax
        test    Dbg.Dbg_Flags[esi],128
        jz      rv46_0B01_9
        ;
        ;Free Dbg entry.
        ;
        mov     Dbg.Dbg_Flags[esi],0
        ;
        ;Update hardware.
        ;
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     ebp,esp
        mov     di,ss
        call    EmuRAWPL32PL0
        ;
        mov     ecx,ebx
        shl     ecx,1
        mov     eax,3
        shl     eax,cl
        not     eax
        mov     ecx,dr7
        and     ecx,eax
        mov     dr7,ecx
        ;
        mov     edx,ebp
        mov     cx,di
        call    EmuRAWPL02PL3
        pop     ds
        ;
        clc
        jmp     rv46_0B01_10
        ;
rv46_0B01_9:
        stc
rv46_0B01_10:
        pop     es
        pop     ds
        popad
        assume ds:_cwDPMIEMU
        jmp     rv46_Done
;
;Get state of break point.
;
rv46_DPMI_0B02:
        cmp     al,02h          ;get debug watch point state?
        jnz     rv46_DPMI_0B03
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    eax
        mov     ax,KernalDS
        mov     ds,ax
        ;
        ;Check handle range.
        ;
        movzx   ebx,bx
        cmp     ebx,3+1
        jnc     rv46_0B02_9
        ;
        mov     eax,size Dbg
        mul     ebx
        add     eax,offset DbgTable
        ;
        ;Active?
        ;
        test    Dbg.Dbg_Flags[eax],128
        jz      rv46_0B02_9
        ;
        ;Check state in hardware.
        ;
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     ebp,esp
        mov     di,ss
        call    EmuRAWPL32PL0
        ;
        mov     eax,dr6
        mov     esi,eax
        ;
        mov     edx,ebp
        mov     cx,di
        call    EmuRAWPL02PL3
        pop     ds
        ;
        ;Isolate bit we want.
        ;
        mov     eax,esi
        mov     ecx,ebx
        mov     edx,1
        shl     edx,cl
        and     eax,edx
        shr     eax,cl
        mov     ecx,eax
        ;
        pop     eax
        mov     ax,cx
        clc
        jmp     rv46_0B02_10
        ;
rv46_0B02_9:
        pop     eax
        stc
rv46_0B02_10:
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jmp     rv46_Done
;
;Reset hardware breakpoint.
;
rv46_DPMI_0B03:
        cmp     al,03h          ;reset debug watch point?
        jnz     rv46_NotOurs
        pushad
        push    ds
        push    es
        mov     ax,KernalDS
        mov     ds,ax
        ;
        ;Check handle range.
        ;
        movzx   ebx,bx
        cmp     ebx,3+1
        jnc     rv46_0B03_9
        ;
        mov     eax,size Dbg
        mul     ebx
        add     eax,offset DbgTable
        ;
        ;Active?
        ;
        test    Dbg.Dbg_Flags[eax],128
        jz      rv46_0B03_9
        ;
        ;Update state in hardware.
        ;
        push    ds
        mov     ax,DpmiEmuDS
        mov     ds,ax
        mov     ebp,esp
        mov     di,ss
        call    EmuRAWPL32PL0
        ;
        mov     ecx,ebx
        mov     edx,1
        shl     edx,cl
        not     edx
        mov     eax,dr6
        and     eax,edx
        mov     dr6,eax
        ;
        mov     edx,ebp
        mov     cx,di
        call    EmuRAWPL02PL3
        pop     ds
        ;
        clc
        jmp     rv46_0B03_10
        ;
rv46_0B03_9:
        stc
rv46_0B03_10:
        pop     es
        pop     ds
        popad
        jmp     rv46_Done
        ;
rv46_Done:
        ;Now update stacked flags.
        ;
        push    eax
        push    ebx
        pushf
        pop     ax                      ;get new flags.
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_Use32Bit8
        mov     bx,sp
        mov     bx,ss:[bx+(4+4)+(2+2)]          ;get original flags.
        jmp     rv46_Use16Bit8
rv46_Use32Bit8:
        mov     bx,[esp+(4+4)+(4+4)]            ;get original flags.
rv46_Use16Bit8:
        and     bx,0000111000000000b            ;retain IF.
        and     ax,1111000111111111b            ;lose IF.
        or      ax,bx                   ;get old IF.
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_Use32Bit9
        mov     bx,sp
        mov     ss:[bx+(4+4)+(2+2)],ax          ;modify stack flags.
        jmp     rv46_Use16Bit9
rv46_Use32Bit9:
        mov     [esp+(4+4)+(4+4)],ax            ;modify stack flags.
rv46_Use16Bit9:
        pop     ebx
        pop     eax
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv46_Use32Bit10
        db 66h
        iret
rv46_Use32Bit10:
        iretd
        ;
rv46_NotOurs:
        ;Not a function recognised by us so pass control to previous handler.
        ;
        assume ds:nothing
        jmp     FWORD PTR cs:[OldInt31]         ;pass it onto previous handler.
        assume ds:_cwDPMIEMU
        ;
OldInt31        dd offset IntNN386Catch+(31h*8)
        dw DpmiEmuCS
RawDPMIPatch    endp


;-------------------------------------------------------------------------------
StateSaveCode   proc    near
        cmp     al,0
        jz      rv47_Save
        cmp     al,1
        jz      rv47_Restore
        jmp     rv47_Done
        ;
rv47_Save:
        push    eax
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        test    BYTE PTR RawSystemFlags,1
        jz      rv47_Save32
        movzx   edi,di
rv47_Save32:
        mov     eax,d[RawStackPos]
        mov     es:[edi],eax
        pop     ds
        pop     eax
        jmp     rv47_Done
        ;
rv47_Restore:
        push    eax
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        test    BYTE PTR RawSystemFlags,1
        jz      rv47_Restore32
        movzx   edi,di
rv47_Restore32:
        mov     eax,es:[edi]
        mov     d[RawStackPos],eax
        pop     ds
        pop     eax
        jmp     rv47_Done
        ;
rv47_Done:
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwRaw
        jz      rv47_Use32
        db 66h
        retf
rv47_Use32:
        retf
        ;
        assume ds:_cwDPMIEMU
StateSaveCode   endp


;-------------------------------------------------------------------------------
_ffPhysicalGetPage proc far
        call    PhysicalGetPage
        ret
_ffPhysicalGetPage endp


;-------------------------------------------------------------------------------
_ffPhysicalGetPages proc far
        call    PhysicalGetPages
        ret
_ffPhysicalGetPages endp


;-------------------------------------------------------------------------------
;
;Get a page of physical memory from one of the possible sources.
;
;On Exit:-
;
;CL     - Flags to apply to page.
;EDX    - Physical address of page if any.
;
PhysicalGetPage proc near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        cmp     MaxMemPhys,0
        assume ds:_cwDPMIEMU
        stc
        jz      rv50_9
        ;
        call    GetVCPIPage
        jnc     rv50_8
        call    GetXMSPage
        jnc     rv50_8
        call    GetINT15Page
        jnc     rv50_8
        call    GetCONVPage
        jnc     rv50_8
        jmp     rv50_9
        ;
rv50_8: and     edx,not 4095
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        dec     MaxMemPhys
        dec     TotalPhysical
        jns     rv50_nowrap
        mov     TotalPhysical,0
rv50_nowrap:
        assume ds:_cwDPMIEMU
        clc
        ;
rv50_9:
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
PhysicalGetPage endp


;-------------------------------------------------------------------------------
;
;Get number of memory pages suppliable by all current memory types.
;
;On Exit:-
;
;EDX    - Pages supported.
;
PhysicalGetPages proc near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,DpmiEmuDS
        mov     ds,ax
        ;
        mov     d[rv51_Total],0
        call    GetVCPIPages
        add     d[rv51_Total],edx

; MED, 11/11/99
        test    edx,edx
        jne     pgp2                    ; VCPI available memory exists
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        mov     VCPIHasNoMem,1  ; flag no memory, so XMS gets a crack at it
        pop     ds
        assume ds:_cwDPMIEMU
pgp2:

        call    GetXMSPages
        add     d[rv51_Total],edx

; MED, 11/11/99
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        mov     VCPIHasNoMem,0  ; reset flag
        pop     ds
        assume ds:_cwDPMIEMU

        call    GetINT15Pages
        add     d[rv51_Total],edx
        call    GetCONVPages
        jc      pgp3                    ; error allocating pages, MED, 11/15/99
        add     d[rv51_Total],edx

pgp3:
        mov     edx,d[rv51_Total]
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        cmp     edx,MaxMemPhys
        jc      rv51_0
        mov     edx,MaxMemPhys
rv51_0: mov     TotalPhysical,edx
        assume ds:_cwDPMIEMU
        clc                     ;exit with success.
        ;
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        ret
        ;
rv51_Total:
        dd ?
PhysicalGetPages endp


;-------------------------------------------------------------------------------
;
;Attempt to allocate a page of VCPI memory.
;
GetVCPIPage     proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1 ;VCPI?
        assume ds:_cwDPMIEMU
        jnz     rv52_9

        mov     ax,KernalDS
        mov     ds,ax
        mov     es,ax
        assume ds:_cwRaw
        mov     ax,0DE04h               ;allocate 4k page.
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        call    EMURawSimulateFCALL
        mov     eax,RealRegsStruc.Real_EAX[edi]

        or      ah,ah           ;get anything?
        jnz     rv52_9
        mov     edx,RealRegsStruc.Real_EDX[edi]
        mov     ecx,1           ;mark it as VCPI memory.
        clc
        jmp     rv52_10

rv52_9:
        stc

rv52_10:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        assume ds:_cwDPMIEMU
        ret
GetVCPIPage     endp


;-------------------------------------------------------------------------------
;
;Find out how many pages VCPI could supply.
;
;On Exit:-
;
;EDX    - Pages available.
;
GetVCPIPages    proc    near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        xor     edx,edx
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1 ;VCPI?
        assume ds:_cwDPMIEMU
        jnz     rv53_9
        ;
        mov     ax,KernalDS
        mov     ds,ax
        mov     es,ax
        assume ds:_cwRaw
        mov     ax,0DE03h               ;get number of free pages.
        mov     edi,offset MemIntBuffer
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_CS[edi],_cwRaw
        mov     RealRegsStruc.Real_IP[edi],offset Int67h
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        call    EMURawSimulateFCALL
        mov     eax,RealRegsStruc.Real_EAX[edi]
        mov     edx,RealRegsStruc.Real_EDX[edi]
        or      ah,ah           ;get anything?
        jnz     rv53_9
        clc
        jmp     rv53_10
        ;
rv53_9: xor     edx,edx
        stc
        ;
rv53_10:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        assume ds:_cwDPMIEMU
        ret
GetVCPIPages    endp



;-------------------------------------------------------------------------------
;
;Attempt to allocate another page of XMS memory.
;
GetXMSPage      proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        cmp     XMSPresent,0
        jz      rv54_9
        ;
        mov     edi,offset MemIntBuffer
        ;
rv54_3: mov     esi,offset XMSList
        xor     edi,edi
        mov     ecx,32
rv54_0: cmp     d[esi+2],0      ;This entry in use?
        jz      rv54_1
        ;
        ;Anything left in this block?
        ;
        mov     eax,2[esi]              ;get current base.
        add     eax,4096                ;next page.
        cmp     eax,6[esi]
        jc      rv54_GotOne
        jmp     rv54_2          ;nothing left in this block.
        ;
rv54_1: or      edi,edi         ;already got a free entry?
        jnz     rv54_2
        mov     edi,esi
rv54_2: add     esi,2+4+4               ;next entry.
        dec     ecx
        jnz     rv54_0
        ;
        ;Need to try and allocate a new block.
        ;
        mov     esi,edi         ;get free entry number.
        or      esi,esi
        jz      rv54_9
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        push    esi
        mov     ah,08h
        mov     RealRegsStruc.Real_EAX[edi],eax
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;get size of largest block.
        pop     esi
        mov     eax,RealRegsStruc.Real_EAX[edi]
        or      ax,ax
        jz      rv54_9
        cmp     ax,4096/1024
        jc      rv54_9
        ;
        ;Limit to pre-defined size.
        ;
        cmp     ax,XMSBlockSize
        jc      rv54_SizeOK
        mov     ax,XMSBlockSize
rv54_SizeOK:
        push    eax
        movzx   eax,ax
        add     eax,3
        shr     eax,2
        cmp     eax,MaxMemPhys
        pop     eax
        jc      rv54_nomaxlimit
        mov     eax,MaxMemPhys
        shl     eax,2
rv54_nomaxlimit:

        ;
        push    esi
        push    ax
        push    ds
        pop     es
        mov     RealRegsStruc.Real_EDX[edi],eax
        mov     ah,9
        mov     RealRegsStruc.Real_EAX[edi],eax
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;allocate memory.
        pop     bp
        pop     esi
        mov     eax,RealRegsStruc.Real_EAX[edi]
        cmp     ax,1
        jnz     rv54_9
        mov     edx,RealRegsStruc.Real_EDX[edi] ;get the handle.
        mov     [esi],dx
        push    esi
        push    bp
        push    dx
        mov     ah,0ch
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;lock block & get address.
        pop     cx
        pop     bp
        pop     esi
        mov     edx,RealRegsStruc.Real_EDX[edi]
        mov     ebx,RealRegsStruc.Real_EBX[edi]
        mov     eax,RealRegsStruc.Real_EAX[edi]
        cmp     ax,1
        jnz     rv54_9          ;should never happen.
        ;
        ;BP    - Block size in K.
        ;DX:BX - Block linear address.
        ;
        movzx   edi,dx          ;get base into 1 reg.
        shl     edi,16          ;/
        mov     di,bx           ;/
        mov     ebx,edi         ;copy into high address.
        movzx   ebp,bp          ;fetch size.
        shl     ebp,10          ;*1024 (1k)
        add     ebx,ebp         ;get real top.
        add     edi,4095                ;round up to next page.
        and     edi,0FFFFFFFFh-4095     ;/
        and     ebx,0FFFFFFFFh-4095     ;round down to nearest page.
        mov     2[esi],edi
        mov     6[esi],ebx              ;store base and end.
        jmp     rv54_3          ;start again.
        ;
rv54_GotOne:
        ;Update table entry indicated and return physical address.
        ;
        mov     edx,2[esi]
        add     d[esi+2],4096
        ;
        xor     ecx,ecx
        clc
        jmp     rv54_10
        ;
rv54_9: stc
        ;
rv54_10:
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
        assume ds:_cwDPMIEMU
GetXMSPage      endp


;-------------------------------------------------------------------------------
;
;Determine how many pages of XMS memory could be allocated.
;
GetXMSPages     proc    near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        mov     d[XMSTotal],0
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain

; MED, 11/11/99
;       cmp     ProtectedType,1 ;VCPI?
        cmp     VCPIHasNoMem,0  ; see if VCPI provided no memory, bail if it did

        assume ds:_cwRaw
        pop     ds
        jz      rv55_9
        cmp     XMSPresent,0
        jz      rv55_9
        ;
        mov     edi,offset XMSTempList
        mov     ecx,32
        xor     eax,eax
        push    ds
        pop     es
        cld
        rep     stosd
        ;
        mov     edi,offset MemIntBuffer
        ;
        mov     ecx,32
        mov     esi,offset XMSTempList
rv55_0: push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        ;
        push    ecx
        push    esi
        mov     ah,08h
        mov     RealRegsStruc.Real_EAX[edi],eax
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;get size of largest block.
        pop     esi
        pop     ecx
        mov     eax,RealRegsStruc.Real_EAX[edi]
        or      ax,ax
        jz      rv55_2
        ;
        ;Limit to pre-defined size.
        ;
        cmp     ax,XMSBlockSize
        jc      rv55_SizeOK
        mov     ax,XMSBlockSize
rv55_SizeOK:
        push    ecx
        push    esi
        push    ax
        mov     RealRegsStruc.Real_EDX[edi],eax
        mov     ah,9
        mov     RealRegsStruc.Real_EAX[edi],eax
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;allocate memory.
        pop     bp
        pop     esi
        pop     ecx
        mov     eax,RealRegsStruc.Real_EAX[edi]
        or      ax,ax
        jz      rv55_2
        mov     edx,RealRegsStruc.Real_EDX[edi] ;get the handle.
        mov     [esi],dx
        mov     w[esi+2],1
        push    ecx
        push    esi
        push    bp
        push    dx
        mov     ah,0ch
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;lock block & get address.
        pop     cx
        pop     bp
        pop     esi
        pop     ecx
        mov     edx,RealRegsStruc.Real_EDX[edi]
        mov     ebx,RealRegsStruc.Real_EBX[edi]
        mov     eax,RealRegsStruc.Real_EAX[edi]
        or      ax,ax
        jz      rv55_2          ;should never happen.
        ;
        ;BP    - Block size in K.
        ;DX:BX - Block linear address.
        ;
        movzx   edi,dx          ;get base into 1 reg.
        shl     edi,16          ;/
        mov     di,bx           ;/
        mov     ebx,edi         ;copy into high address.
        movzx   ebp,bp          ;fetch size.
        shl     ebp,10          ;*1024 (1k)
        add     ebx,ebp         ;get real top.
        add     edi,4095                ;round up to next page.
        shr     edi,12
        shr     ebx,12
        sub     ebx,edi
        js      rv55_1
        dec     ebx
        js      rv55_1
        add     d[XMSTotal],ebx
rv55_1: add     esi,4
        dec     ecx
        jnz     rv55_0
        ;
rv55_2: ;Now release all memory blocks again.
        ;
        mov     ecx,32
        mov     esi,offset XMSTempList+(31*4)
rv55_3:
        push    ecx
        push    esi
        cmp     d[esi],0
        jz      rv55_4
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     dx,[esi]
        mov     ah,0dh
        push    dx
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;un-lock it first.
        pop     dx
        mov     ah,0ah
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_EDX[edi],edx
        push    ax
        mov     ax,w[XMSControl]
        mov     RealRegsStruc.Real_IP[edi],ax
        mov     ax,w[XMSControl+2]
        mov     RealRegsStruc.Real_CS[edi],ax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        pop     ax
        call    EMURawSimulateFCALL     ;un-lock it first.
rv55_4:
        pop     esi
        pop     ecx
        sub     esi,4
        dec     ecx
        jnz     rv55_3
        ;
        ;Now find out how much of existing blocks is free.
        ;
        mov     esi,offset XMSList
        xor     edi,edi
        mov     ecx,32
rv55_5: cmp     d[esi+2],0      ;This entry in use?
        jz      rv55_6
        ;
        ;Anything left in this block?
        ;
        mov     eax,2[esi]              ;get current base.
        add     eax,4096                ;next page.
        cmp     eax,6[esi]
        jnc     rv55_6          ;nothing left in this block.
        mov     eax,6[esi]
        sub     eax,2[esi]
        shr     eax,12          ;free pages remaining
        dec     eax
        add     d[XMSTotal],eax
        ;
rv55_6: add     esi,2+4+4
        dec     ecx
        jnz     rv55_5
        ;
rv55_9: ;Now return pages found.
        ;
        mov     edx,d[XMSTotal]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        assume ds:_cwDPMIEMU
        ret
GetXMSPages     endp


;-------------------------------------------------------------------------------
GetINT15Page    proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        ;
        push    ds
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        cmp     XMSPresent,0
        assume ds:_cwDPMIEMU
        pop     ds
        jnz     rv56_9
        ;
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
rv56_3: mov     esi,offset Int15Table
        mov     ecx,8
        xor     edi,edi
rv56_0: cmp     d[esi],0
        jz      rv56_1
        mov     eax,[esi]
        add     eax,4096
        cmp     eax,4[esi]
        jc      rv56_GotOne
        jmp     rv56_2
rv56_1: or      edi,edi
        jnz     rv56_2
        mov     edi,esi
rv56_2: add     esi,4+4
        dec     ecx
        jnz     rv56_0
        ;
        ;Need to get another block of memory.
        ;
        or      edi,edi
        jz      rv56_9
        cmp     ILevel,8
        jnc     rv56_9          ;can't cope with any more.
        push    edi
        push    ax
        mov     ax,KernalZero
        mov     es,ax
        pop     ax
        movzx   edi,WORD PTR es:[(VDiskSigVec*4)+2]
        shl     edi,4
        mov     eax,100000h             ;Start of extended memory.
        add     edi,VDiskSigOffs
        mov     esi,offset VDiskSig
        mov     cx,VDiskSigLen
        push    edi
        cld
        repe    cmpsb
        pop     edi
        jne     rv56_GotBottom
        xor     eax,eax
        mov     al,es:[edi+VDiskHiOffs]
        shl     eax,16
        mov     ax,es:[edi+VDiskLoOffs]
rv56_GotBottom:
        mov     esi,eax
        ;
        ;Work out top of memory.
        ;
        push    esi

Big1Check2:
        cmp     Big1Flag,0      ; see if using alternate 0e801h means to get memory
        je      use88h2
        mov     ax,0e801h
        jmp     GIGetMem2

use88h2:
        mov     ah,88h          ; get top of extended memory

GIGetMem2:
        mov     bl,15h
        mov     edi,offset MemIntBuffer
        push    ds
        pop     es
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        call    EMURawSimulateInt

        test    BYTE PTR RealRegsStruc.Real_Flags[edi],1        ; see if carry returned
        je      GIProcess2      ; nope
        cmp     Big1Flag,0
        je      GIProcess2      ; not using alternate extended memory, process anyway
        mov     Big1Flag,0      ; turn off alternate
        jmp     Big1Check2      ; and retry

GIProcess2:
        cmp     Big1Flag,0
        je      use88hResult2   ; use results from 88h function

; using results from 0e801h function
        movzx   eax,WORD PTR RealRegsStruc.Real_EAX[edi]
        movzx   ebx,WORD PTR RealRegsStruc.Real_EBX[edi]
        push    ax
        or      ax,bx           ; see if ax=bx=0, if so use cx,dx return instead
        pop     ax
        jne     GIComputeBig1Size2
        movzx   eax,WORD PTR RealRegsStruc.Real_ECX[edi]
        movzx   ebx,WORD PTR RealRegsStruc.Real_EDX[edi]
        push    ax
        or      ax,bx           ; see if cx=dx=0, if so use regular 88h function
        pop     ax
        jne     GIComputeBig1Size2
        mov     Big1Flag,0      ; turn off alternate
        jmp     Big1Check2      ; and retry

GIComputeBig1Size2:
        shl     ebx,6           ; 64K chunks to 1K
        add     eax,ebx         ; add to 1K chunks below 64M
        jmp     GIComputeBytes2

use88hResult2:
        movzx   eax,WORD PTR RealRegsStruc.Real_EAX[edi]

;       mov     eax,RealRegsStruc.Real_EAX[edi]
;       movzx   eax,ax

GIComputeBytes2:
        shl     eax,10          ; * 1024
        add     eax,100000h             ;add in 1 meg base address.
        dec     eax
        and     eax,0FFFFFFFFh-4095     ;round down to nearest page.
        mov     ebx,eax
        pop     esi
        pop     edi
        ;
        ;ESI - base.
        ;EBX - limit.
        ;
        cmp     esi,ebx
        jnc     rv56_9          ;No more available.
        mov     ecx,ebx
        sub     ecx,esi         ;block size.
        cmp     ecx,4096                ;check enough for 1 page.
        jc      rv56_9
        ;
        pushad
        cmp     Int15Size,0             ;set size yet?
        jnz     rv56_GotSize
        mov     eax,ecx         ;get proposed maximum size.
        mov     ecx,8           ;number of chunks.
        xor     edx,edx
        div     ecx             ;get chunk size.
        inc     eax
        or      Int15Size,-1            ;set chunk size to use.
        and     eax,not 4095
        jz      rv56_GotSize
        mov     Int15Size,eax   ;set chunk size to use.
rv56_GotSize:
        popad
        cmp     ecx,Int15Size
        jc      rv56_SizeOK
        cmp     ExtALLSwitch,0
        jnz     rv56_SizeOK
        mov     ecx,Int15Size
rv56_SizeOK:

        mov     eax,ecx
        add     eax,4095
        shr     eax,12
        cmp     eax,MaxMemPhys
        jc      rv56_nomaxlimit
        mov     ecx,MaxMemPhys
        shl     ecx,12
rv56_nomaxlimit:

        sub     ebx,ecx         ;new int 15 value.
        ;
        ;EBX - base.
        ;ECX - size.
        ;
        mov     0[edi],ebx              ;store base address.
        mov     4[edi],ebx
        add     4[edi],ecx              ;set end address.
        ;
        movzx   esi,ILevel
        shl     esi,3
        add     esi,offset ITable
;       dec     ebx             ;move back to previous byte.
        sub     ebx,100000h             ;remove starting point.
        shr     ebx,10          ;convert to K.
        mov     [esi+4],ebx             ;set new base value.
        mov     bl,15h
        push    ax
        push    ebx
        push    es
        mov     ax,KernalZero
        mov     es,ax
        movzx   ebx,bl
        shl     ebx,2
        mov     dx,es:0[ebx]
        mov     cx,es:2[ebx]
        pop     es
        pop     ebx
        pop     ax
        mov     0[esi],dx
        mov     2[esi],cx               ;store old vector.
        movzx   edx,ILevel
        mov     dx,[Int15PatchTable+edx*2]
        mov     cx,_cwRaw               ;segment to use.
        mov     bl,15h
        push    ax
        push    ebx
        push    es
        mov     ax,KernalZero
        mov     es,ax
        movzx   ebx,bl
        shl     ebx,2
        pushf
        cli
        mov     es:0[ebx],dx
        mov     es:2[ebx],cx
        popf
        pop     es
        pop     ebx
        pop     ax
        inc     ILevel          ;move to next level.
        jmp     rv56_3
        ;
rv56_GotOne:
        ;Update table entry and exit.
        ;
        mov     edx,0[esi]              ;get base address.
        add     d[esi],4096             ;move pointer along.
        xor     ecx,ecx
        clc
        jmp     rv56_10
        ;
rv56_9: stc
        ;
rv56_10:
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
        assume ds:_cwDPMIEMU
GetINT15Page    endp


;-------------------------------------------------------------------------------
GetINT15Pages   proc    near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        ;
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        mov     [INT15hTotal],0
        ;
        push    ds
        mov     ax,MainDS
        mov     ds,ax
        assume ds:_cwMain
        cmp     ProtectedType,1 ;VCPI?
        assume ds:_cwRaw
        pop     ds
        jnc     rv57_9
        cmp     XMSPresent,0
        jnz     rv57_9
        ;
        ;Setup initial simulated int 15 values.
        ;

Big1Check1:
        cmp     Big1Flag,0      ; see if using alternate 0e801h means to get memory
        je      use88h1
        mov     ax,0e801h
        jmp     GIGetMem1

use88h1:
        mov     ah,88h          ; get top of extended memory

GIGetMem1:
        mov     bl,15h
        mov     edi,offset MemIntBuffer
        push    ds
        pop     es
        mov     RealRegsStruc.Real_EAX[edi],eax
        mov     RealRegsStruc.Real_SS[edi],0
        mov     RealRegsStruc.Real_SP[edi],0
        call    EMURawSimulateInt

        test    BYTE PTR RealRegsStruc.Real_Flags[edi],1        ; see if carry returned
        je      GIProcess1      ; nope
        cmp     Big1Flag,0
        je      GIProcess1      ; not using alternate extended memory, process anyway
        mov     Big1Flag,0      ; turn off alternate
        jmp     Big1Check1      ; and retry

GIProcess1:
        cmp     Big1Flag,0
        je      use88hResult1   ; use results from 88h function

; using results from 0e801h function
        movzx   eax,WORD PTR RealRegsStruc.Real_EAX[edi]
        movzx   ebx,WORD PTR RealRegsStruc.Real_EBX[edi]

        push    ax
        or      ax,bx           ; see if ax=bx=0, if so use cx,dx return instead
        pop     ax
        jne     GIComputeBig1Size1
        movzx   eax,WORD PTR RealRegsStruc.Real_ECX[edi]
        movzx   ebx,WORD PTR RealRegsStruc.Real_EDX[edi]
        push    ax
        or      ax,bx           ; see if cx=dx=0, if so use regular 88h function
        pop     ax
        jne     GIComputeBig1Size1
        mov     Big1Flag,0      ; turn off alternate
        jmp     Big1Check1      ; and retry

GIComputeBig1Size1:
        shl     ebx,6           ; 64K chunks to 1K
        add     eax,ebx         ; add to 1K chunks below 64M
        jmp     GIComputeBytes1

use88hResult1:
        movzx   eax,WORD PTR RealRegsStruc.Real_EAX[edi]

GIComputeBytes1:

;       mov     w[Int15hValue],ax
        mov     [Int15hValue],eax

        mov     ax,ILevel
        mov     [INT15hLevel2],ax
        ;
rv57_0: ;Need to get another block of memory.
        ;
        cmp     [INT15hLevel2],8
        jnc     rv57_1          ;can't cope with any more.
        push    ax
        mov     ax,KernalZero
        mov     es,ax
        pop     ax
        movzx   edi,WORD PTR es:[(VDiskSigVec*4)+2]
        shl     edi,4
        mov     eax,100000h             ;Start of extended memory.
        add     edi,VDiskSigOffs
        mov     esi,offset VDiskSig
        mov     cx,VDiskSigLen
        push    edi
        cld
        repe    cmpsb
        pop     edi
        jne     rv57_GotBottom
        xor     eax,eax
        mov     al,es:[edi+VDiskHiOffs]
        shl     eax,16
        mov     ax,es:[edi+VDiskLoOffs]
rv57_GotBottom:
        mov     esi,eax
        ;
        ;Work out top of memory.
        ;
;       push    esi

;       movzx   eax,w[Int15hValue]      ;get pretend value.
        mov     eax,[Int15hValue]       ;get pretend value.

        shl     eax,10          ; * 1024
        add     eax,100000h             ;add in 1 meg base address.
        dec     eax
        and     eax,0FFFFFFFFh-4095     ;round down to nearest page.
        mov     ebx,eax
;       pop     esi

        ;
        ;ESI - base.
        ;EBX - limit.
        ;
        cmp     esi,ebx
        jnc     rv57_1          ;No more available.
        mov     ecx,ebx
        sub     ecx,esi         ;block size.
        cmp     ecx,4096                ;check enough for 1 page.
        jc      rv57_1
        ;
        pushad
        cmp     Int15Size,0             ;set size yet?
        jnz     rv57_GotSize
        mov     eax,ecx         ;get proposed maximum size.
        mov     ecx,8           ;number of chunks.
        xor     edx,edx
        div     ecx             ;get chunk size.
        mov     Int15Size,-1            ;default to maximum.
        cmp     eax,4096                ;too small?
        jc      rv57_GotSize
        mov     Int15Size,eax   ;set chunk size to use.
rv57_GotSize:
        popad
        cmp     ecx,Int15Size
        jc      rv57_SizeOK
        mov     ecx,Int15Size
rv57_SizeOK:
        sub     ebx,ecx         ;new int 15 value.
        ;
        ;EBX - base.
        ;ECX - size.
        ;
        shr     ecx,12          ;get number of pages.
        add     [INT15hTotal],ecx
        ;
        dec     ebx             ;move back to previous byte.
        sub     ebx,100000h             ;remove starting point.
        shr     ebx,10          ;convert to K.

;       mov     w[Int15hValue],bx       ;set new base value.
        mov     [Int15hValue],ebx       ;set new base value.

        inc     [INT15hLevel2]  ;move to next level.
        jmp     rv57_0
        ;
rv57_1: ;Now include any remains of existing blocks.
        ;
        mov     esi,offset Int15Table
        mov     ecx,8
rv57_2: cmp     d[esi],0
        jz      rv57_3
        mov     eax,[esi]
        add     eax,4096
        cmp     eax,4[esi]
        jnc     rv57_3
        mov     eax,4[esi]
        sub     eax,0[esi]
        shr     eax,12          ;free pages remaining
        dec     eax
        add     [INT15hTotal],eax
rv57_3: add     esi,4+4
        dec     ecx
        jnz     rv57_2
        ;
rv57_9: mov     edx,[INT15hTotal]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        ret
        assume ds:_cwDPMIEMU
GetINT15Pages   endp


;-------------------------------------------------------------------------------
;
;Attempt to allocate another page of conventional memory.
;
GetCONVPage     proc    near
        push    eax
        push    ebx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        ;
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        cmp     CONVSaveSize,-1
        jz      rv58_9
        ;
        mov     CONVSavePara,0
        cmp     CONVSaveSize,0
        jz      rv58_200
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     ax,CONVSaveSize
        mov     es:RealRegsStruc.Real_EBX[edi],eax      ;set size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        mov     eax,es:RealRegsStruc.Real_EAX[edi]      ;get segment address.
        test    es:RealRegsStruc.Real_Flags[edi],1
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jnz     rv58_9          ;if not enough for user buffer then don't allocate any more.
        mov     CONVSavePara,ax ;store para we saved.
rv58_200:
rv58_3: mov     esi,offset ConventionalList
        xor     edi,edi
        mov     ecx,32
rv58_0: cmp     w[esi],0        ;This entry in use?
        jz      rv58_1
        push    ecx
        push    esi
        push    edi
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4a00h    ;resize memory block.
        mov     ax,[esi]                ;get real mode segment
        mov     es:RealRegsStruc.Real_ES[edi],ax
        mov     ax,2[esi]               ;get current size.
        add     ax,4096/16
        mov     es:RealRegsStruc.Real_EBX[edi],eax      ;set new size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        test    es:RealRegsStruc.Real_Flags[edi],1
        pop     edi
        pop     esi
        pop     ecx
        jz      rv58_GotOne
        ;
        ;Make sure block goes back to original size.
        ;
        push    ecx
        push    esi
        push    edi
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4a00h    ;resize memory block.
        mov     ax,[esi]                ;get real mode segment
        mov     es:RealRegsStruc.Real_ES[edi],ax
        mov     ax,2[esi]               ;get current size.
        mov     es:RealRegsStruc.Real_EBX[edi],eax      ;set new size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        pop     edi
        pop     esi
        pop     ecx
        jmp     rv58_2
        ;
rv58_1: or      edi,edi         ;already got a free entry?
        jnz     rv58_2
        mov     edi,esi
rv58_2: add     esi,4           ;next entry.
        dec     ecx
        jnz     rv58_0
        ;
        ;Need to allocate a new block.
        ;
        mov     esi,edi         ;get free entry number.
        or      esi,esi
        jz      rv58_9
        push    esi
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     es:RealRegsStruc.Real_EBX[edi],1        ;set new size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        pop     esi
        test    es:RealRegsStruc.Real_Flags[edi],1
        jnz     rv58_9
        mov     eax,es:RealRegsStruc.Real_EAX[edi]      ;get segment address.
        mov     [esi],ax                ;store it in the table.
        movzx   eax,ax
        shl     eax,4           ;linear address.
        mov     ebx,eax
        add     eax,4095
        and     eax,0ffffffffh-4095     ;round up to next page.
        sub     eax,ebx
        shr     eax,4
        mov     2[esi],ax               ;store new size.
        jmp     rv58_3          ;start again.
        ;
rv58_GotOne:
        ;Update table entry indicated and return physical address.
        ;
        movzx   eax,w[esi]              ;Get block base segment.
        add     ax,2[esi]               ;Add old length.
        add     w[esi+2],4096/16        ;update length.
        shl     eax,4           ;linear address.
        shr     eax,12          ;get page number.
        mov     bx,KernalZero
        mov     es,bx
        mov     esi,1024*4096*1023      ;base of page alias's.
        mov     eax,es:[esi+eax*4]      ;get physical address.
        and     eax,0ffffffffh-4095     ;lose user bits.
        mov     edx,eax
        ;
        xor     ecx,ecx
        clc
        jmp     rv58_10
        ;
rv58_9: stc
        ;
rv58_10:
        pushf
        cmp     CONVSavePara,0  ;did we save any memory?
        jz      rv58_100
        pushad
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4900h    ;release memory block.
        mov     ax,CONVSavePara
        mov     es:RealRegsStruc.Real_ES[edi],ax        ;set block to release.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        popad
        mov     CONVSavePara,0
rv58_100:
        popf
        ;
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     eax
        assume ds:_cwDPMIEMU
        ret
GetCONVPage     endp


;-------------------------------------------------------------------------------
;
;Determine how many pages of conventional memory could be allocated.
;
GetCONVPages    proc    near
        push    eax
        push    ebx
        push    ecx
        push    esi
        push    edi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,KernalDS
        mov     ds,ax
        assume ds:_cwRaw
        ;
        mov     d[CONVTotal],0
        ;
        cmp     CONVSaveSize,-1
        jz      rv59_9
        ;
        mov     CONVSavePara,0
        cmp     CONVSaveSize,0
        jz      rv59_200
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     ax,CONVSaveSize
        mov     es:RealRegsStruc.Real_EBX[edi],eax      ;set size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        mov     eax,es:RealRegsStruc.Real_EAX[edi]      ;get segment address.
        test    es:RealRegsStruc.Real_Flags[edi],1
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        jnz     rv59_9          ;if not enough for user buffer then don't allocate any more.
        mov     CONVSavePara,ax ;store para we saved.
rv59_200:
        mov     edi,offset ConvTempList
        mov     ecx,32
        xor     eax,eax
        push    ds
        pop     es
        cld
        rep     stosw
        ;
        mov     ecx,32
        mov     esi,offset ConvTempList
rv59_0:
        push    ecx
        push    esi
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     es:RealRegsStruc.Real_EBX[edi],-1       ;set new size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        pop     esi
        pop     ecx
        mov     ebx,es:RealRegsStruc.Real_EBX[edi]
        cmp     bx,(4096*2)/16
        jc      rv59_2
        push    ebx
        push    ecx
        push    esi
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        pop     esi
        pop     ecx
        pop     ebx
        test    es:RealRegsStruc.Real_Flags[edi],1
        jnz     rv59_2
        ;
        mov     eax,es:RealRegsStruc.Real_EAX[edi]
        mov     [esi],ax                ;store segment address.
        movzx   eax,ax
        shl     eax,4           ;linear address.
        movzx   ebx,bx
        shl     ebx,4
        add     ebx,eax         ;linear limit.
        add     eax,4095
        shr     eax,12          ;round up to next page.
        shr     ebx,12          ;round down to next page.
        sub     ebx,eax
        js      rv59_1
        add     d[CONVTotal],ebx
rv59_1: add     esi,2
        dec     ecx
        jnz     rv59_0
        ;
rv59_2: ;Now release all memory blocks again.
        ;
        mov     ecx,32
        mov     esi,offset ConvTempList+(31*2)
rv59_3:
        push    ecx
        push    esi
        cmp     w[esi],0
        jz      rv59_4
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4900h    ;release memory block.
        mov     ax,[esi]
        mov     es:RealRegsStruc.Real_ES[edi],ax        ;set block to release.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
rv59_4:
        pop     esi
        pop     ecx
        sub     esi,2
        dec     ecx
        jnz     rv59_3
        ;
        ;Ask for big block to try and improve cleanup process.
        ;
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4800h    ;allocate memory block.
        mov     es:RealRegsStruc.Real_EBX[edi],-1       ;set new size desired.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        ;
        ;Now return pages found.
        ;

;       mov     edx,d[CONVTotal]
        ;
rv59_9: pushf

        mov     edx,d[CONVTotal]        ; moved, MED, 11/15/99

        cmp     CONVSavePara,0  ;did we save any memory?
        jz      rv59_100
        pushad
        push    ds
        pop     es
        mov     edi,offset MemIntBuffer
        mov     es:RealRegsStruc.Real_EAX[edi],4900h    ;release memory block.
        mov     ax,CONVSavePara
        mov     es:RealRegsStruc.Real_ES[edi],ax        ;set block to release.
        mov     es:RealRegsStruc.Real_SS[edi],0
        mov     es:RealRegsStruc.Real_SP[edi],0
        mov     bl,21h
        call    EMURawSimulateINT
        popad
        mov     CONVSavePara,0
rv59_100:
        popf
        ;
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     eax
        ret
GetCONVPages    endp


;-------------------------------------------------------------------------------
;
;Build a segment descriptor.
;
;On Entry:-
;
;ES:EDI - Descriptor entry to use.
;ESI    - Linear base to set.
;ECX    - limit in bytes.
;AL     - Code size bit.
;AH     - Present/PL/memory|system/type bits.
;
EMUMakeDesc     proc    near
        pushad
        and     edi,not 7               ;lose RPL & TI
        cmp     ecx,0100000h    ; see if we need to set g bit
        jc      rv60_0
        shr     ecx,12          ; div by 4096
        or      al,80h          ; set g bit
rv60_0: mov     es:[edi],cx             ;store low word of limit.
        shr     ecx,16
        or      cl,al
        mov     es:[edi+6],cl   ;store high bits of limit and gran/code size bits.
        mov     es:[edi+2],si   ;store low word of linear base.
        shr     esi,16
        mov     bx,si
        mov     es:[edi+4],bl   ;store mid byte of linear base.
        mov     es:[edi+7],bh   ;store high byte of linear base.
        mov     es:[edi+5],ah   ;store pp/dpl/dt/type bits.
        popad
        ret
EMUMakeDesc     endp

;-------------------------------------------------------------------------------
;
;Convert number into ASCII Hex version.
;
;On Entry:-
;
;EAX    - Number to convert.
;ECX    - Digits to do.
;DS:EDI - Buffer to put string in.
;
Bin2HexE        proc    near
        push    edi
        push    ecx
        push    ebx
        mov     ebx,offset HexTableE
        add     edi,ecx
        dec     edi
        mov     edx,eax
rv62_0: mov     al,dl
        shr     edx,4
        and     al,15
        xlat
        mov     [edi],al
        dec     edi
        dec     ecx
        jnz     rv62_0
        pop     ebx
        pop     ecx
        pop     edi
        add     edi,ecx
        ret
Bin2HexE        endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Emulate INT 2Fh DPMI related functions.
;
Raw2FPatch      proc    near
        cmp     ax,1687h
        jz      rv64_DoneC
        cmp     ax,1686h
        jnz     rv64_NotOurs
        xor     ax,ax
        jmp     rv64_Done
rv64_DoneC:
        stc
        ;
rv64_Done:
        ;Now update stacked flags.
        ;
        push    eax
        push    ebx
        xor     eax,eax
        pushf
        pop     ax                      ;get new flags.
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv64_Use32Bit8
        mov     bx,sp
        mov     bx,ss:[bx+(4+4)+(2+2)]          ;get original flags.
        jmp     rv64_Use16Bit8
rv64_Use32Bit8:
        mov     ebx,[esp+(4+4)+(4+4)]           ;get original flags.
rv64_Use16Bit8:
        and     bx,0000111000000000b            ;retain IF.
        and     ax,1111000111111111b            ;lose IF.
        or      eax,ebx                 ;get old IF.
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv64_Use32Bit9
        mov     bx,sp
        mov     ss:[bx+(4+4)+(2+2)],ax          ;modify stack flags.
        jmp     rv64_Use16Bit9
rv64_Use32Bit9:
        mov     [esp+(4+4+4+4)],eax             ;modify stack flags.
rv64_Use16Bit9:
        pop     ebx
        pop     eax
        assume ds:nothing
        test    BYTE PTR cs:DpmiEmuSystemFlags,1
        assume ds:_cwDPMIEMU
        jz      rv64_Use32Bit10
        db 66h
        iret
rv64_Use32Bit10:
        iretd
        ;
rv64_NotOurs:
        ;Not a function recognised by us so pass control to previous handler.
        ;
        assume ds:nothing
        jmp     FWORD PTR cs:[OldpInt2f]                ;pass it onto previous handler.
        assume ds:_cwDPMIEMU
;
OldpInt2f       dd offset IntNN386Catch+(2fh*8)
        dw DpmiEmuCS
Raw2FPatch      endp


        include interrup.asm
        include ldt.asm
        include memory.asm


HexTableE       db '0123456789ABCDEF'

cwDPMIEMUEnd    label byte
_cwDPMIEMU      ends

        .286
