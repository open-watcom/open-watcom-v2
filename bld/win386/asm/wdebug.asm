;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  DEBUG - Windows 3.x/9x Virtual Device Driver
;*               
;*
;*****************************************************************************


;****************************************************************************
;***                                                                      ***
;*** WGOD.ASM - Ring 0 multi-purpose windows thingme                      ***
;***            (like God, only better)                                   ***
;***                                                                      ***
;****************************************************************************

        .386p
        .387

        INCLUDE win30vxd.inc
        INCLUDE wdebug.inc

MAX_EMU_REG     equ 32

SAMPLE_SIZE equ 6
Sample_struct   struc
SampleEIP       dd ?
SampleCS        dw ?
Sample_struct   ends

EMU_SIZE        equ 2*2+2*4
EMU_struct      struc
E_CS            dw ?
E_NULL          dw ?
E_8087          dd ?
E_ID            dd ?
EMU_struct      ends

MAX_SERVER_NAME equ 16
MAX_CONVS       equ 64
CONV_SIZE       equ 5*4+4*2+8+MAX_SERVER_NAME
Conv_struct     struc
C_MyID          dd ?
C_WhoBlocked    dd ?
C_ServerID      dd ?
C_Regs          dd ?
C_TimerHandle   dd ?
C_Seg           dw ?
C_Off           dw ?
C_Bytes         dw ?
C_ConvCount     dw ?
C_AckConv       db ?
C_StartedConv   db ?
C_GetBlocked    db ?
C_PutBlocked    db ?
C_IsPM          db ?
C_IsServer      db ?
C_InUse         db ?
C_PutPending    db ?
C_ServerName    db MAX_SERVER_NAME dup(?)
Conv_struct     ends

Interrupt_struct struc
I_SaveEAX               dd ?
I_SaveEBX               dd ?
I_SaveECX               dd ?
I_SaveEDX               dd ?
I_SaveEDI               dd ?
I_SaveESI               dd ?
I_SaveEFLAGS            dd ?
I_SaveEBP               dd ?
I_SaveEIP               dd ?
I_SaveESP               dd ?
I_SaveSS                dw ?
I_SaveCS                dw ?
I_SaveDS                dw ?
I_SaveES                dw ?
I_SaveFS                dw ?
I_SaveGS                dw ?
I_InterruptNumber       dw ?
Interrupt_struct        ends

;Declare_Virtual_Device WDEBUG, 3, 0, WGod_Control, Debug_Device_ID, 0f0000000h
Declare_Watcom_Debug_Virtual_Device 3, 0, WGod_Control, Debug_Device_ID, 00000001h

extrn __Win387_emulator:PROC

;*
;*** initialization data
;*
VxD_IDATA_SEG

VxD_IDATA_ENDS

;*
;*** local data
;*
VxD_DATA_SEG
CallTable LABEL DWORD
        dd      OFFSET SVC_GetVersion         ; request 00
        dd      OFFSET SVC_CopyMemory         ; request 01
        dd      OFFSET SVC_GetDescriptor      ; request 02
        dd      OFFSET SVC_GetLimit           ; request 03
        dd      OFFSET SVC_GetDR              ; request 04
        dd      OFFSET SVC_SetDR              ; request 05
        dd      OFFSET SVC_InitSampler        ; request 06
        dd      OFFSET SVC_QuitSampler        ; request 07
        dd      OFFSET SVC_StartSampler       ; request 08
        dd      OFFSET SVC_StopSampler        ; request 09
        dd      OFFSET SVC_GetCurrTick        ; request 0a
        dd      OFFSET SVC_SetTimerRate       ; request 0b
        dd      OFFSET SVC_GetTimerRate       ; request 0c
        dd      OFFSET SVC_GetCurrCount       ; request 0d
        dd      OFFSET SVC_GetSample0Tick     ; request 0e
        dd      OFFSET SVC_RegisterName       ; request 0f
        dd      OFFSET SVC_AccessName         ; request 10
        dd      OFFSET SVC_UnregisterName     ; request 11
        dd      OFFSET SVC_UnaccessName       ; request 12
        dd      OFFSET SVC_StartConv          ; request 13
        dd      OFFSET SVC_LookForConv        ; request 14
        dd      OFFSET SVC_EndConv            ; request 15
        dd      OFFSET SVC_ConvGet            ; request 16
        dd      OFFSET SVC_ConvPut            ; request 17
        dd      OFFSET SVC_IsConvAck          ; request 18
        dd      OFFSET SVC_MyID               ; request 19
        dd      OFFSET SVC_SetExecutionFocus  ; request 1a
        dd      OFFSET SVC_WhatHappened       ; request 1b
        dd      OFFSET SVC_ConvGetTimeout     ; request 1c
        dd      OFFSET SVC_ConvPutTimeout     ; request 1d
        dd      OFFSET SVC_EMUInit            ; request 1e
        dd      OFFSET SVC_EMUShutdown        ; request 1f
        dd      OFFSET SVC_EMURegister        ; request 20
        dd      OFFSET SVC_EMUUnRegister      ; request 21
        dd      OFFSET SVC_FPUPresent         ; request 22
        dd      OFFSET SVC_EMUSaveRestore     ; request 23
        dd      OFFSET SVC_PauseSampler       ; request 24
        dd      OFFSET SVC_UnPauseSampler     ; request 25
        dd      OFFSET SVC_EGAWrite           ; request 26
        dd      OFFSET SVC_VGARead            ; request 27
        dd      OFFSET SVC_DisableVideo       ; request 28
        dd      OFFSET SVC_RegisterInterruptCallback ; request 29
        dd      OFFSET SVC_UnRegisterInterruptCallback ; request 2a
        dd      OFFSET SVC_GetInterruptCallback ; request 2b
        dd      OFFSET SVC_RestartFromInterrupt; request 2c
        dd      OFFSET SVC_Is32BitSel         ; request 2d
        dd      OFFSET SVC_GetVMId            ; request 2e
        dd      OFFSET SVC_HookIDT            ; request 2f
        dd      OFFSET SVC_IDTFini            ; request 30
        dd      OFFSET SVC_IDTInit            ; request 31
        dd      OFFSET SVC_ConvPutPending     ; request 32
        dd      OFFSET SVC_UseHotKey          ; request 33
        dd      OFFSET SVC_RaiseInterruptInVM ; request 34
MaxAPI equ ($-CallTable)/4

PM_Int2FNextCS  dd 0
PM_Int2FNextEIP dd 0

PageTableBuf    dd 0
StartTime       dd 0
TimerHandle     dd 0
VMHandle        dd 0
WinVMHandle     dd 0
SampleUserSel   dd 0
SampleUserOff   dd 0
SampleUserVM    dd 0
SampleOffset    dd 0
CurrTick        dd 0
SampleCount     dd 0
StopSamples     dd 1
MaxSamples      dd 0
TimeOutTime     dd 0
DataTimeOut     dd 0
IntPeriod       dd 0
Sample0Tick     dd 0
StringAddr      dd 0
IDAddr          dd 0
OtherIDAddr     dd 0
SaveServer      dd 0
ServerID        dd 0
IsEMUInit       dd 0
OldCR0          dd 0
TaskSwitcherActive dd 0
WasHotKey       dd 0
HotEventHandle  dd 0
UseHotKey       dd 0

IDTIsInit       dd 0
RealSS          dw 0
RealCS          dw 0
RealESP         dd 0
RealEIP         dd 0
RealEFlags      dd 0
RealFault       dw 0
ReflectToCS     dw 0
ReflectToEIP    dd 0
IDTAddr         dd 0


EMUList         db MAX_EMU_REG*EMU_SIZE dup(0)
Convs           db MAX_CONVS*CONV_SIZE dup(0)

Idt01           db 8 dup(0)
Idt03           db 8 dup(0)

Descriptor      LABEL WORD
Desc1           dw 0
Desc2           dw 0
Desc3           dw 0
Desc4           dw 0

IsPM            dw 0
MapSeg          dw 0
MapOff          dw 0
HasFPU          dw 0

InUse           db 0
IsServer        db 0
IsGet           db 0

VxD_DATA_ENDS

VxD_LOCKED_DATA_SEG
Old00Handler    dd 0
Old06Handler    dd 0
Old07Handler    dd 0
Old0DHandler    dd 0
ICCodeFlat      dd 0
ICDataFlat      dd 0
ICVM            dd 0ffffffffh
FaultType       dw 0ffffh
ICCodeSeg       dw 0
ICCodeOff       dw 0
ICDataSeg       dw 0
ICDataOff       dw 0
ICStackSeg      dw 0
ICStackOff      dw 0
VxD_LOCKED_DATA_ENDS

VxD_ICODE_SEG
;**************************************************************
;***                                                        ***
;*** WGod_Sys_Critical_Init - hook int 2f here, for PM apps ***
;***                                                        ***
;**************************************************************
BeginProc WGod_Sys_Critical_Init
;*
;*** hook PM int 2f
;*
        mov     eax, 2Fh
        VxDcall Get_PM_Int_Vector
        mov     [PM_Int2FNextCS], ecx
        mov     [PM_Int2FNextEIP], edx

        mov     esi, OFFSET WDebugPM_Int2F
        VxDcall Allocate_PM_Call_Back

        movzx   edx, ax                         ;eax has cs:ip
        mov     ecx, eax
        shr     ecx, 16
        mov     eax, 2Fh
        VxDcall Set_PM_Int_Vector

;*
;*** hook gp fault
;*
        mov     eax,0dh
        mov     esi,OFFSET Fault0DHandler
        VxDcall Hook_PM_Fault
        cmp     esi,0
        jne     short aretd
        mov     esi,OFFSET JustReturn
aretd:
        mov     Old0DHandler,esi

;*
;*** find out if there is an FPU
;*
        mov     HasFPU,0                ; assume no FPU
        push    eax                     ; allocate space for control word
        fninit                          ; initialize math coprocessor
        fnstcw  word ptr [esp]          ; store control word in memory
        pop     eax                     ; get control word
        cmp     ah,03h                  ; upper byte is 03 if 8087 is present
        jne     short nofpu
        mov     HasFPU,1                ; got an FPU
nofpu:
        clc
        ret

EndProc WGod_Sys_Critical_Init

VxD_ICODE_ENDS


;*
;** locked code
;*
VxD_LOCKED_CODE_SEG
BeginProc WGod_Control

        Control_Dispatch Sys_Critical_Init, WGod_Sys_Critical_Init
        Control_Dispatch Device_Init, WGod_Device_Init
        Control_Dispatch VM_Suspend, WGod_Suspend
        Control_Dispatch VM_Resume, WGod_Resume
        clc
        ret

EndProc WGod_Control
;*
;*** ReflectTo16Bit - pass state to 16-bit application
;*
ReflectTo16Bit PROC near
        mov     edx,ICDataFlat
        call    IDTFini                         ; we want int 3 as normal

        cmp     RealFault,0                     ; did an int1/int3 occur?
        je      short not_special               ; nope, go get data from CRS

        mov     ax,RealFault                    ; was int1/int3, use Real...
        mov     FaultType,ax
        cmp     ax,257
        jne     short no_cancel
        push    edx
        VxDcall Get_Sys_VM_Handle
        mov     esi,HotEventHandle
        VxDcall Cancel_VM_Event
        pop     edx
no_cancel:
        mov     RealFault,0

        mov     eax,RealESP
        mov     [edx.I_SaveESP],eax
        mov     eax,RealEIP
        mov     [edx.I_SaveEIP],eax
        mov     eax,RealEFlags
        mov     [edx.I_SaveEFLAGS],eax
        mov     ax,RealCS
        mov     [edx.I_SaveCS],ax
        mov     ax,RealSS
        mov     [edx.I_SaveSS],ax

        jmp     short was_special
not_special:
        mov     eax,[ebp.Client_ESP]
        mov     [edx.I_SaveESP],eax
        mov     eax,[ebp.Client_EIP]
        mov     [edx.I_SaveEIP],eax
        mov     eax,[ebp.Client_EFlags]
        mov     [edx.I_SaveEFLAGS],eax
        mov     ax,[ebp.Client_CS]
        mov     [edx.I_SaveCS],ax
        mov     ax,[ebp.Client_SS]
        mov     [edx.I_SaveSS],ax
was_special:

        mov     eax,[ebp.Client_EAX]
        mov     [edx.I_SaveEAX],eax
        mov     eax,[ebp.Client_EBX]
        mov     [edx.I_SaveEBX],eax
        mov     eax,[ebp.Client_ECX]
        mov     [edx.I_SaveECX],eax
        mov     eax,[ebp.Client_EDX]
        mov     [edx.I_SaveEDX],eax
        mov     eax,[ebp.Client_ESI]
        mov     [edx.I_SaveESI],eax
        mov     eax,[ebp.Client_EDI]
        mov     [edx.I_SaveEDI],eax
        mov     eax,[ebp.Client_EBP]
        mov     [edx.I_SaveEBP],eax
        mov     ax,[ebp.Client_DS]
        mov     [edx.I_SaveDS],ax
        mov     ax,[ebp.Client_ES]
        mov     [edx.I_SaveES],ax
        mov     ax,[ebp.Client_FS]
        mov     [edx.I_SaveFS],ax
        mov     ax,[ebp.Client_GS]
        mov     [edx.I_SaveGS],ax
        mov     ax,FaultType
        mov     [edx.I_InterruptNumber],ax

;*
;*** point application at special code segment to handle the fault
;*
        and     word ptr [ebp.Client_EFlags],not 100h ; turn off trace trap bit
        mov     ax,ICCodeSeg
        mov     [ebp.Client_CS],ax
        movzx   eax,ICCodeOff
        mov     [ebp.Client_EIP],eax
        mov     ax,ICDataSeg
        mov     [ebp.Client_DS],ax
        mov     [ebp.Client_ES],ax
        mov     [ebp.Client_FS],ax
        mov     [ebp.Client_GS],ax
        mov     ax,ICStackSeg
        mov     [ebp.Client_SS],ax
        movzx   eax,ICStackOff
        mov     [ebp.Client_ESP],eax
        ret
ReflectTo16Bit ENDP

Is32BitSel PROC NEAR
        VxDcall3 _GetDescriptor, eax, ebx, 0
        mov     dword ptr Descriptor+4,edx
        test    Desc4,040h
        ret
Is32BitSel ENDP

;*
;*** various fault handlers
;*
BeginProc Fault00Handler
        mov     FaultType,0h
        cmp     ICVM,ebx
        jne     short ret00
        movzx   eax,[ebp.Client_SS]
        call    Is32BitSel
        je      short ret00
        jmp     ReflectTo16Bit
ret00:
        jmp     [Old00Handler]
EndProc Fault00Handler

BeginProc Fault06Handler
        mov     FaultType,6h
        cmp     ICVM,ebx
        jne     short ret06
        cmp     RealFault,0
        jne     ReflectTo16Bit
        movzx   eax,[ebp.Client_SS]
        call    Is32BitSel
        je      short ret06
        jmp     ReflectTo16Bit
ret06:
        jmp     [Old06Handler]
EndProc Fault06Handler

BeginProc Fault07Handler
        pushad
        movzx   edx,[ebp.Client_CS]
        xor     ecx,ecx
        mov     edi,OFFSET EMUList
again102:
        cmp     word ptr [edi.E_CS],dx
        je      short fnd102
        add     edi,EMU_SIZE
        inc     ecx
        cmp     ecx,MAX_EMU_REG
        jne     short again102

        mov     eax,edx
        call    Is32BitSel
        je      short fault07_16bit
        popad
        jmp     [Old07Handler]

fnd102:
        VxDcall3 _SelectorMapFlat, ebx, edx, 0    ; returns linear addr in eax
        mov     ecx,ebx                         ; VM Handle
        mov     ebx,dword ptr [edi.E_8087]      ; 8087 ptr
        add     ebx,eax                         ; add linear offset
        call    __Win387_emulator
        popad
        ret

fault07_16bit:
;
;       We are in a 16-bit segment. Decode the instruction to find the
;       start of the next instruction, and just return.
;
        push    es                      ; save es
        movzx   edx,[ebp.Client_CS]
        mov     es,dx                   ; point to code segment
        mov     esi,[ebp.Client_EIP]    ; get instruction pointer
luup:   mov     al,es:[esi]             ; get possible prefix in al
        sub     al,26H                  ; if its es:
        js      short endlup            ; if signed, then not a prefix byte
        je      short is_es             ; - continue with next byte
        cmp     al,09BH-26H             ; check for "fwait" opcode
        je      short inc_si            ; - yes, just skip over it
        sub     al,08H                  ; if its cs:
        je      short is_cs             ; - record cs in cx and continue
        sub     al,08H                  ; if its ss:
        je      short is_ss             ; - record ss in cx and continue
        sub     al,08H                  ; if its ds:
        je      short is_ds             ; - record ds in cx and continue
        sub     al,26H                  ; if its fs:
        je      short is_fs             ; - record fs in cx and continue
        dec     al                      ; if its gs:
        je      short is_gs             ; - record gs in cx and continue
        dec     al                      ; if its operand length
        je      short inc_si            ; - continue with next byte
        dec     al                      ; if its address length
        jne     short endlup            ; - its not handled!!!!
is_cs:
is_ds:
is_es:
is_fs:
is_gs:
is_ss:
inc_si: inc     esi                     ; just bump past prefix byte
        jmp     short luup              ;
endlup:
        mov     al,es:[esi]             ; load up opcode
        and     al,0F8h                 ; isolate top bits of opcode
        cmp     al,0D8h                 ; if it is not an 8087 opcode
        jne     short skip0             ; then get out
        inc     esi                     ; skip over opcode byte
        mov     al,es:[esi]             ; get modrm byte MMxxxRRR
        inc     esi                     ; skip over modrm byte
        cmp     al,0C0H                 ; if its a 8087 STACK operation
        jae     short skip0             ; then end of instruction
        test    al,80H                  ; if 16-bit displacement follows
        jne     short skip2             ; then skip 2 bytes
        test    al,40H                  ; if 8-bit displacement follows
        jne     short skip1             ; then skip one byte
        and     al,7                    ; isolate r/m bits
        cmp     al,6                    ; if not mode 6
        jne     short skip0             ; then end of instruction
skip2:  inc     esi                     ; skip first byte
skip1:  inc     esi                     ; skip one byte
skip0:
        mov     [ebp.Client_EIP],esi    ; update instruction pointer
        pop     es                      ; restore es
        popad
        ret

EndProc Fault07Handler


BeginProc Fault0DHandler
        mov     FaultType,0dh
        cmp     ICVM,ebx
        jne     short ret0d
        movzx   eax,[ebp.Client_SS]
        call    Is32BitSel
        je      short ret0d
        jmp     ReflectTo16Bit
ret0d:
        jmp     [Old0DHandler]
EndProc Fault0DHandler


JustReturn: ret

VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG
Int01IDT:
        cmp     ss:[WasHotKey],1
        jne     short not_hot
        mov     ss:[WasHotKey],0
        mov     ss:[RealFault],257
        jmp     short GenericIDT
not_hot:
        mov     ss:[RealFault],1

GenericIDT:
        push    eax

        mov     eax,ss:[esp+014h]       ; ss
        mov     ss:[RealSS],ax
        mov     eax,ss:[esp+010h]       ; esp
        cmp     eax,8000000h            ; did we have a 16-bit fault?
        jb      short no_zero           ; yes!
        movzx   eax,ax
no_zero:
        mov     ss:[RealESP],eax
        mov     eax,ss:[esp+04h]        ; eip
        mov     ss:[RealEIP],eax
        mov     eax,ss:[esp+08h]        ; cs
        mov     ss:[RealCS],ax
        mov     eax,ss:[esp+0ch]        ; eflags
        mov     ss:[RealEFlags],eax

        and     eax, not 0100h          ; turn off T bit

        mov     ss:[esp+0ch],eax        ; new flags
        mov     eax,ss:[ReflectToEIP]
        mov     ss:[esp+04h],eax        ; new cs
        movzx   eax,ss:[ReflectToCS]
        mov     ss:[esp+08h],eax        ; new eip
        pop     eax

        iretd

Int03IDT:
        mov     ss:[RealFault],3
        dec     dword ptr ss:[esp]              ; eip
        jmp     short GenericIDT

;************************************************
;***                                          ***
;*** WGod_Device_Init - do V86 initialization ***
;***                                          ***
;************************************************
BeginProc WGod_Device_Init
;*
;*** hook into V86 int 2f chain
;*
        mov     eax,02fh
        mov     esi,OFFSET WDebugV86_Int2F
        VxDcall Hook_V86_Int_Chain

;*
;*** hook div by 0 fault
;*
        xor     eax,eax
        mov     esi,OFFSET Fault00Handler
        VxDcall Hook_PM_Fault
        cmp     esi,0
        jne     short aret0
        mov     esi,OFFSET JustReturn
aret0:
        mov     Old00Handler,esi

;*
;*** hook illegal instruction fault
;*
        mov     eax,6
        mov     esi,OFFSET Fault06Handler
        VxDcall Hook_PM_Fault
        cmp     esi,0
        jne     short aret6
        mov     esi,OFFSET JustReturn
aret6:
        mov     Old06Handler,esi

;*
;*** hook Invalid Page Faults (08-feb-94)
;*
;       mov     eax,0eh
;       mov     esi,OFFSET Fault0EHandler
;       VxDcall Hook_PM_Fault
;       cmp     esi,0
;       jne     short arete
;       mov     esi,OFFSET JustReturn
;arete:
;       mov     Old0EHandler,esi

;*
;*** hot key for debugging (NYI)
;*
        mov     ax,33                           ; 'F' key
        ShiftState <SS_Either_Ctrl + SS_Either_Alt + SS_Toggle_mask>, <SS_Ctrl + SS_Alt>
        mov     cl,CallOnPress
        mov     esi,OFFSET HotKeyPressed
        xor     edi,edi
        VxDcall VKD_Define_Hot_Key

        clc
        ret
EndProc WGod_Device_Init

;*********************************************
;***                                       ***
;*** HotStop - stop the system VM          ***
;***                                       ***
;*********************************************
BeginProc HotStop
        or      [ebp.Client_EFlags],0100h
        mov     WasHotKey,1
        ret
EndProc HotStop

;****************************************************;
;***                                              ***;
;*** HotKeyPressed - someone tried an asynch stop ***;
;***                                              ***;
;****************************************************;
BeginProc HotKeyPressed
        cmp     UseHotKey,0
        jne     short useit
        ret
useit:
        VxDcall Get_Sys_VM_Handle
        mov     esi,OFFSET HotStop
        VxDcall Schedule_VM_Event
        mov     HotEventHandle,esi
        ret
EndProc HotKeyPressed

;***********************************
;***                             ***
;*** IDTFini - finished with IDT ***
;***                             ***
;***********************************
IDTFini PROC near
        push    edi
        push    esi
        push    ecx
        mov     edi,IDTAddr

        lea     esi,[edi+1*8]
        mov     ecx,dword ptr [Idt01]
        mov     dword ptr [esi],ecx
        mov     ecx,dword ptr [Idt01+4]
        mov     dword ptr [esi+4],ecx

        lea     esi,[edi+3*8]
        mov     ecx,dword ptr [Idt03]
        mov     dword ptr [esi],ecx
        mov     ecx,dword ptr [Idt03+4]
        mov     dword ptr [esi+4],ecx
        pop     ecx
        pop     esi
        pop     edi
        ret
IDTFini ENDP


;*************************************************************
;***                                                       ***
;*** GetIDFrom_CX_BX - get server id based on client CX:BX ***
;***                                                       ***
;*************************************************************
GetIDFrom_CX_BX PROC near
        mov     ax,[ebp.Client_CX]              ; hi word
        shl     eax,16
        mov     ax,[ebp.Client_BX]              ; lo word
        ret
GetIDFrom_CX_BX ENDP

;*****************************************************************
;***                                                           ***
;*** GetFlatAddr - get the flat address from MapSeg and MapOff ***
;***               (either V86 or protected mode apps)         ***
;***                                                           ***
;*** in:  MapSeg:MapOff - segment:offset of thingy to map      ***
;*** out: eax           - ring 0 flat address                  ***
;***                                                           ***
;*****************************************************************
GetFlatAddr PROC near

        mov     ebx,VMHandle                    ; get VM handle
        cmp     IsPM,1                          ; was it a PM request
        je      short pm_string                 ; yes
        push    esi                             ; preserve esi
        movzx   eax,MapSeg                      ; V86 segment
        shl     eax,4
        movzx   esi,MapOff                      ; V86 offset
        add     eax,esi                         ; offset into V86 selector
        add     eax,[ebx.CB_High_Linear]
        pop     esi
        ret
pm_string:
        movzx   edx,MapSeg                      ; selector
        VxDcall3 _SelectorMapFlat, ebx, edx, 0
        movzx   edx,MapOff                      ; offset
        add     eax,edx                         ; ring 0 addr
        ret
GetFlatAddr ENDP

;**************************************************************
;***                                                        ***
;*** GetFlatAddr_ES_BX - get flat address from client es:bx ***
;***                                                        ***
;**************************************************************
GetFlatAddr_ES_BX PROC near
        mov     ax,[ebp.Client_ES]
        mov     MapSeg,ax
        mov     ax,[ebp.Client_BX]
        mov     MapOff,ax
        call    GetFlatAddr
        ret
GetFlatAddr_ES_BX ENDP

;***************************************************
;***                                             ***
;*** FindServerPtrFromName - look up server name ***
;***                                             ***
;*** in:  eax - ptr to server name               ***
;*** out: eax - offset of server data (or 0)     ***
;***                                             ***
;***************************************************
FindServerPtrFromName PROC near
        push    edi
        push    esi
        xor     ecx,ecx
        mov     edi,eax                         ; save string ptr
        mov     ebx,OFFSET Convs              ; first conv
again9:
        cmp     [ebx.C_InUse],1                 ; active one?
        jne     short skip
        cmp     [ebx.C_IsServer],1              ; a server?
        jne     short skip                      ; nope
        lea     esi,[ebx.C_ServerName]          ; server name ptr
        mov     eax,edi                         ; string ptr
again8:
        mov     dl,byte ptr [eax]
        cmp     byte ptr [esi],dl               ; match?
        je      short ok                        ; yes
skip:
        inc     ecx                             ; no, try next block
        add     ebx,CONV_SIZE
        cmp     ecx,MAX_CONVS
        jne     again9
        xor     eax,eax                         ; no pointer
        jmp     short done11
ok:
        cmp     dl,0                            ; at string end?
        je      short found                     ; yes
        inc     esi
        inc     eax
        jmp     again8                          ; try next char

found:
        mov     eax,ebx                         ; pointer to data
done11:
        pop     esi
        pop     edi
        ret

FindServerPtrFromName ENDP

;****************************************
;***                                  ***
;*** FindAnyPtr - find any data       ***
;***                                  ***
;*** in : eax - id to search for      ***
;***      InUse - in use bit          ***
;***      IsServer - server bit       ***
;***      ServerId - server to test   ***
;*** out: eax - data pointer (or 0)   ***
;***                                  ***
;****************************************
FindAnyPtr PROC near
        mov     edx,OFFSET Convs
        xor     ecx,ecx
        mov     bl,InUse
        mov     bh,IsServer
again13:
        cmp     [edx.C_InUse],bl                ; an active block?
        jne     short notfnd                    ; no, skip
        cmp     bh,2                            ; no server test?
        je      short nostest
        cmp     [edx.C_IsServer],bh             ; a server?
        jne     short notfnd
nostest:
        cmp     eax,0                           ; are we looking for anyone?
        je      short testserver
        cmp     [edx.C_MyID],eax                ; found the id?
        jne     short notfnd                    ; nope
        cmp     ServerID,0
        je      short exit16                    ; nope
testserver:
        push    eax
        mov     eax,ServerID
        cmp     [edx.C_ServerID],eax            ; right server?
        pop     eax
        jne     short notfnd
exit16:
        mov     eax,edx                         ; pointer to id
        ret
notfnd:
        add     edx,CONV_SIZE                   ; point at data
        inc     ecx                             ; next client
        cmp     ecx,MAX_CONVS                   ; last client?
        jne     again13                         ; nope
        xor     eax,eax                         ; yes, not found
        ret
FindAnyPtr ENDP

;*********************************************
;***                                       ***
;*** FindServerPtr - find a server pointer ***
;***                                       ***
;*** in: eax - server id                   ***
;*** out: eax - server data ptr            ***
;***                                       ***
;*********************************************
FindServerPtr PROC near
        mov     InUse,1
        mov     ServerID,0
        mov     IsServer,1
        call    FindAnyPtr
        ret
FindServerPtr ENDP

;*********************************************
;***                                       ***
;*** FindClientPtr - find a client pointer ***
;***                                       ***
;*** in: eax - server id                   ***
;*** out: eax - server data ptr            ***
;***                                       ***
;*********************************************
FindClientPtr PROC near
        mov     InUse,1
        mov     IsServer,0
        call    FindAnyPtr
        ret
FindClientPtr ENDP

;***********************************************
;***                                         ***
;*** FindEmptySlot - get an empty conv block ***
;***                                         ***
;*** out: eax - pointer to slot (or 0)       ***
;***                                         ***
;***********************************************
FindEmptySlot PROC near
        xor     eax,eax                         ; look for a null slot
        mov     IsServer,2                      ; no server test
        mov     InUse,0                         ; can't be in use
        mov     ServerID,0                      ; no server id test
        call    FindAnyPtr
        test    eax,eax                         ; found a slot?
        jne     short zeroit                    ; yep
        ret
zeroit:
        push    eax                             ; save pointer
        push    edi                             ; preserve edi
        mov     edi,eax
        xor     eax,eax
        mov     ecx,CONV_SIZE
        cld
        rep     stosb                           ; zero out data area
        pop     edi
        pop     eax
        ret
FindEmptySlot ENDP

;***************************************
;***                                 ***
;*** GetLimit - get a selector limit ***
;***                                 ***
;*** selector in edx                 ***
;*** VM handle in ebx                ***
;*** result returned in eax          ***
;***                                 ***
;***************************************
GetLimit PROC NEAR
        VxDcall3 _GetDescriptor, edx, ebx, 0    ; ebx contains VM handle
        mov     dword ptr Descriptor,eax
        mov     dword ptr Descriptor+4,edx
        mov     dx,Desc1                        ; get low word
        mov     ax,Desc4                        ; get hi 4 bits
        and     ax,0fh;                         ;   of the limit
        shl     eax,16                          ; shift hi 4 bits
        mov     ax,dx                           ; move in low 16 bits
        mov     dx,Desc4
        test    dx,080h                         ; page granularity?
        jz      short small1                    ; nope
        inc     eax                             ; yes, then
        shl     eax,0ch                         ;    get real limit
        dec     eax                             ; make it limit-1
small1:
        ret
GetLimit ENDP

;***********************************************************
;***                                                     ***
;*** WGod_Suspend - process suspend requests for a V86 ***
;***                                                     ***
;***********************************************************
BeginProc WGod_Suspend
        ret
EndProc WGod_Suspend

;*********************************************************
;***                                                   ***
;*** WGod_Resume - process resume requests for a V86 ***
;***                                                   ***
;*********************************************************
BeginProc WGod_Resume
        mov     VMHandle,ebx
        mov     eax,ebx                 ; handle to search for
        mov     IsServer,2              ; no server test
        mov     InUse,1                 ; must be in use
        mov     ServerID,0              ; no server id test
        call    FindAnyPtr
        cmp     eax,0
        jne     short gotrdata
        clc
        ret
gotrdata:
        cmp     [eax.C_GetBlocked],1    ; are we get blocked?
        je      short notoktoresume
        cmp     [eax.C_PutBlocked],1    ; are we put blocked?
        je      short notoktoresume
        clc                             ; we aren't blocked, allow resume
        ret
notoktoresume:
        stc
        ret
EndProc WGod_Resume


;**********************************************************
;***                                                    ***
;*** WDebugPM_Int01 - int 01 handler for protected mode ***
;***                                                    ***
;**********************************************************
BeginProc WDebugPM_Int01, High_Freq, PUBLIC

        mov     eax,[ebp.Client_ESP]
        mov     [ebp.Client_EAX],eax
        VxDjmp  Simulate_Iret

EndProc WDebugPM_Int01


;**********************************************************
;***                                                    ***
;*** WDebugPM_Int2F - int 2F handler for protected mode ***
;***                                                    ***
;**********************************************************
BeginProc WDebugPM_Int2F, High_Freq, PUBLIC

        mov     eax,[ebp.Client_EAX]
        cmp     ah,0FAh                         ; is it ours?
        jz      short process                   ; yes
        mov     ecx,[PM_Int2FNextCS]            ; no, chain to next
        mov     edx,[PM_Int2FNextEIP]
        VxDjmp  Simulate_Far_Jmp

process:
        mov     VMHandle,ebx                    ; save the VM's handle
        movzx   eax,al
        cmp     eax,MaxAPI
        jge     short exit2
        mov     IsPM,1                          ; is a PM entry
        call    dword ptr CallTable[eax*4]
exit2:
        VxDjmp  Simulate_Iret

EndProc WDebugPM_Int2F

;**************************************************
;***                                            ***
;*** WDebugV86_Int2F - int 2F handler for V86's ***
;***                                            ***
;**************************************************
BeginProc WDebugV86_Int2F, High_Freq, PUBLIC

        mov     eax,[ebp.Client_EAX]
        cmp     ah,0FAh                         ; is it ours?
        jz      short process2                  ; yes
        stc
        ret

process2:
        mov     VMHandle,ebx                    ; save the VM's handle
        movzx   eax,al
        cmp     eax,MaxAPI
        jge     short exit3
        mov     IsPM,0                          ; flag NOT a PM entry
        call    dword ptr CallTable[eax*4]
exit3:
        clc                                     ; swallow interrupt
        ret

EndProc WDebugV86_Int2F

;****************************************************
;***                                              ***
;*** SVC_GetVersion - gets current version of API ***
;***                                              ***
;****************************************************
BeginProc SVC_GetVersion
        mov     [ebp.Client_AX], WGOD_VERSION
        ret
EndProc SVC_GetVersion

;********************************************************
;***                                                  ***
;*** SVC_CopyMemory   - read current register state   ***
;***                                                  ***
;*** client si:ebx points to input area               ***
;***        cx:edx points to output area              ***
;***        di    contains number of bytes            ***
;***                                                  ***
;*** return number of bytes copied in Client_AX       ***
;********************************************************
BeginProc SVC_CopyMemory

        mov     [ebp.Client_AX],0       ; nothing yet

        cmp     [ebp.Client_DI],0       ; anything to copy?
        jne     short ok2c
exitcm2:jmp     exitcm

ok2c:
        movzx   edx,[ebp.Client_CX]     ; destination selector
        VxDcall3 _SelectorMapFlat, ebx, edx, 0
        cmp     eax,0ffffffffh          ; could we do it?
        je      short exitcm2           ; no, leave
        mov     edi,eax                 ; save flat address
        add     edi,[ebp.Client_EDX]    ; add offset of destination

        movzx   edx,[ebp.Client_SI]     ; source selector
        VxDcall3 _SelectorMapFlat, ebx, edx, 0
        cmp     eax,0ffffffffh          ; could we do it?
        je      short exitcm2           ; no, leave
        mov     esi,eax                 ; save flat address
        add     esi,[ebp.Client_EBX]    ; add offset of source

        movzx   edx,[ebp.Client_SI]     ; source selector
        call    GetLimit                ; get limit of source
        movzx   ecx,[ebp.Client_DI]     ; get number of bytes
        add     ecx,[ebp.Client_EBX]    ; get offset of last one
        dec     ecx
        cmp     eax,ecx                 ; are we past the end?
        jae     short trydest           ; no, test destination area
        sub     ecx,eax                 ; get bytes over
        movzx   eax,[ebp.Client_DI]     ; get number of bytes
        cmp     ecx,eax                 ; are bytes over >= bytes to copy
        jae     short exitcm2           ; yes, quit
        sub     [ebp.Client_DI],cx
trydest:
        movzx   edx,[ebp.Client_CX]     ; destination selector
        call    GetLimit                ; limit of destination
        movzx   ecx,[ebp.Client_DI]     ; get number of bytes
        add     ecx,[ebp.Client_EDX]    ; get offset of last one
        dec     ecx
        cmp     eax,ecx                 ; are we past the end?
        jae     short docopy            ; no, then do copy
        sub     ecx,eax                 ; get bytes over
        movzx   eax,[ebp.Client_DI]     ; get number of bytes
        cmp     ecx,eax                 ; are bytes over >= bytes to copy
        jae     short exitcm            ; yes, quit
        sub     [ebp.Client_DI],cx

docopy:
        mov     eax,esi                 ; get source address
        shr     eax,12                  ; calc page number
        VxDcall3a _CopyPageTable, eax, 1, PageTableBuf
        or      eax,eax                 ; eax = 0 if invalid page table entry
        je      short exitcm            ; ...
        mov     eax,PageTableBuf        ; get entry
        or      eax,eax                 ; eax = 0 if invalid page table entry
        je      short exitcm            ; ...
        mov     eax,edi                 ; get destination address
        shr     eax,12                  ; calc page number
        VxDcall3a _CopyPageTable, eax, 1, PageTableBuf
        or      eax,eax                 ; eax = 0 if invalid page table entry
        je      short exitcm            ; ...
        mov     eax,PageTableBuf        ; get entry
        or      eax,eax                 ; eax = 0 if invalid page table entry
        je      short exitcm            ; ...
        movzx   ecx,[ebp.Client_DI]     ; get number of bytes
        mov     [ebp.Client_AX],cx      ; return the number of bytes copied
        cld
        push    ecx                     ; save length
        shr     ecx,2                   ; calc number of dwords
        rep     movsd                   ; move them
        pop     ecx                     ; restore length
        and     ecx,3                   ; isolate left over bytes
        rep     movsb                   ; copy the bytes
exitcm:
        ret
EndProc SVC_CopyMemory

;********************************************************
;***                                                  ***
;*** SVC_GetDescriptor- read current register state   ***
;***                                                  ***
;*** client es:bx  descriptor area to store into      ***
;***        cx     contains selector                  ***
;***                                                  ***
;********************************************************
BeginProc SVC_GetDescriptor

        movzx  ecx,[ebp.Client_CX]      ; get descriptor
        VxDcall3 _GetDescriptor, ecx, ebx, 0    ; ebx contains VM handle

        Client_Ptr_Flat edi,ES,BX       ; get destination
        mov     [edi+4],edx             ; store high word
        mov     [edi],eax               ; store low word
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_GetDescriptor

;********************************************************
;***                                                  ***
;*** SVC_GetLimit - get limit of a selector           ***
;***                                                  ***
;*** client bx     selector                           ***
;***                                                  ***
;********************************************************
BeginProc SVC_GetLimit

        movzx   edx,[ebp.Client_BX]
        call    GetLimit
        mov     edx,eax
        shr     edx,16
        mov     [ebp.Client_AX],ax
        mov     [ebp.Client_DX],dx
        ret
EndProc SVC_GetLimit

;********************************************************
;***                                                  ***
;*** SVC_GetDR - get a debugging register             ***
;***                                                  ***
;*** client ebx - value destination                   ***
;***         cx - register to read                    ***
;***                                                  ***
;********************************************************
BeginProc SVC_GetDR

        mov     cx,[ebp.Client_CX]
        cmp     cx,0
        jne     short gdr1
        mov     eax,dr0
        jmp     short donegdr
gdr1:   cmp     cx,1
        jne     short gdr2
        mov     eax,dr1
        jmp     short donegdr
gdr2:   cmp     cx,2
        jne     short gdr3
        mov     eax,dr2
        jmp     short donegdr
gdr3:   cmp     cx,3
        jne     short gdr6
        mov     eax,dr3
        jmp     short donegdr
gdr6:   cmp     cx,6
        jne     short gdr7
        mov     eax,dr6
        jmp     short donegdr
gdr7:   cmp     cx,7
        jne     short errdgr
        mov     eax,dr7
        jmp     short donegdr
errdgr:
        mov     [ebp.Client_AX],0ffffh
        ret
donegdr:
        mov     [ebp.Client_EBX],eax
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_GetDR

;********************************************************
;***                                                  ***
;*** SVC_SetDR - set a debugging register             ***
;***                                                  ***
;*** client ebx - value to set to                     ***
;***         cx - register to change                  ***
;***                                                  ***
;********************************************************
BeginProc SVC_SetDR

        mov     cx,[ebp.Client_CX]
        mov     eax,[ebp.Client_EBX]
        cmp     cx,0
        jne     short sdr1
        mov     dr0,eax
        jmp     short donesdr
sdr1:   cmp     cx,1
        jne     short sdr2
        mov     dr1,eax
        jmp     short donesdr
sdr2:   cmp     cx,2
        jne     short sdr3
        mov     dr2,eax
        jmp     short donesdr
sdr3:   cmp     cx,3
        jne     short sdr6
        mov     dr3,eax
        jmp     short donesdr
sdr6:   cmp     cx,6
        jne     short sdr7
        mov     dr6,eax
        jmp     short donesdr
sdr7:   cmp     cx,7
        jne     short errsdr
        mov     dr7,eax
        jmp     short donesdr
errsdr:
        mov     [ebp.Client_AX],0ffffh
        ret
donesdr:
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_SetDR

;*****************************************
;***                                   ***
;*** SetTimer - start a global timeout ***
;***                                   ***
;*****************************************
SetTimer PROC near
        mov     eax,TimeOutTime
        mov     edx,eax                     ; reference data
        mov     esi,OFFSET SampleStuff    ; callback routine
        VxDcall Set_Global_Time_Out
        mov     TimerHandle,esi
        ret
SetTimer ENDP

;********************************************************
;***                                                  ***
;*** SampleStuff - callback for sampler               ***
;***                                                  ***
;********************************************************
BeginProc SampleStuff

        call    SetTimer

        cmp     StopSamples,1                   ; frozen?
        jne     short testvm                    ; nope
        ret

testvm:
        cmp     ebx,SampleUserVM                ; our VM?
        je      short testindex                 ; yep
        ret

testindex:
        mov     eax,MaxSamples
        cmp     SampleCount,eax                 ; any room?
        jne     short roomleft                  ; yep
        ret                                     ; no, go back

roomleft:
        mov     edx,SampleUserSel               ; get flat pointer to buff
        mov     ebx,SampleUserVM
        VxDcall3 _SelectorMapFlat, ebx, edx, 0
        cmp     eax,0ffffffffh
        jne     short ok_map
        ret
ok_map:
        add     eax,SampleOffset                ; offset into table

        mov     edi,[ebp.Client_EIP]            ; EIP where int. took place
        mov     [eax.SampleEIP],edi             ; save it

        mov     di,[ebp.Client_CS]
        mov     [eax.SampleCS],di               ; save it

        inc     SampleCount
        inc     CurrTick                        ; yet another sample
        add     SampleOffset,SAMPLE_SIZE        ; bump to next table entry

        ret
EndProc SampleStuff

;********************************************************
;***                                                  ***
;*** SVC_InitSampler - start sampling going           ***
;***                                                  ***
;*** client es:bx  sample buffer size                 ***
;***        cx - number of samples in buffer          ***
;***        dx - milliseconds between samples         ***
;***                                                  ***
;********************************************************
BeginProc SVC_InitSampler

        mov     StopSamples,1               ; ignore samples for now
        mov     CurrTick,0

        movzx   eax,[ebp.Client_CX]         ; sample buffer limit
        mov     MaxSamples,eax              ; save it

        mov     ebx,VMHandle
        VxDcall Get_VM_Exec_Time
        mov     StartTime,eax               ; time windows has been active

        mov     SampleUserVM,ebx            ; save VM
        movzx   eax,[ebp.Client_ES]         ; destination selector
        mov     SampleUserSel,eax           ; save it
        movzx   eax,[ebp.Client_BX]         ; offset into selector
        mov     SampleUserOff,eax           ; save it
        mov     SampleOffset,eax

        mov     ebx,VMHandle                ; current vm (should be windows)
        mov     WinVMHandle,ebx

        movzx   eax,[ebp.Client_DX]         ; millseconds between samples
        mov     TimeOutTime,eax

        call    SetTimer

        mov     [ebp.Client_AX],0
        ret

EndProc SVC_InitSampler

;*************************************************************
;***                                                       ***
;*** SVC_QuitSampler - cancel timeouts in the Windows VM   ***
;***                                                       ***
;*************************************************************
BeginProc SVC_QuitSampler
        mov     esi,TimerHandle
        VxDcall Cancel_Time_Out

        mov     eax,IntPeriod
        cmp     eax,0
        je      short skiprip
        VxDcall VTD_End_Min_Int_Period

skiprip:
        mov     ebx,WinVMHandle
        VxDcall Get_VM_Exec_Time
        sub     eax,StartTime
        mov     [ebp.Client_EAX],eax
        ret
EndProc SVC_QuitSampler

;*************************************************************
;***                                                       ***
;*** SVC_StartSampler - start sampler after a stop         ***
;***                                                       ***
;***   NOTE: resets the sample buffer to the start         ***
;***                                                       ***
;*************************************************************
BeginProc SVC_StartSampler
        mov     eax,SampleUserOff
        mov     SampleOffset,eax
        mov     SampleCount,0
        mov     StopSamples,0
        mov     eax,CurrTick
        mov     Sample0Tick,eax
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_StartSampler

;****************************************
;***                                  ***
;*** SVC_StopSampler - Stop sampler   ***
;***                                  ***
;***   NOTE: returns sample count     ***
;***                                  ***
;****************************************
BeginProc SVC_StopSampler
        mov     StopSamples,1
        mov     eax,SampleCount
        mov     [ebp.Client_EAX],eax
        ret
EndProc SVC_StopSampler

;****************************************
;***                                  ***
;*** SVC_GetCurrTick - guess!         ***
;***                                  ***
;****************************************
BeginProc SVC_GetCurrTick
        mov     eax,CurrTick
        mov     [ebp.Client_EAX],eax
        ret
EndProc SVC_GetCurrTick

;*******************************************************
;***                                                 ***
;*** SVC_SetTimerRate - set a time timer tick rate   ***
;***                                                 ***
;*** client bx  sample rate                          ***
;***                                                 ***
;*******************************************************
BeginProc SVC_SetTimerRate
        movzx   eax,[ebp.Client_BX]
        mov     IntPeriod,eax
        VxDcall VTD_Begin_Min_Int_Period
        ret
EndProc SVC_SetTimerRate

;*******************************************************
;***                                                 ***
;*** SVC_GetTimerRate - get timer tick rate          ***
;***                                                 ***
;*******************************************************
BeginProc SVC_GetTimerRate
        VxDcall VTD_Get_Interrupt_Period
        mov     [ebp.Client_AX],ax
        ret
EndProc SVC_GetTimerRate

;****************************************
;***                                  ***
;*** SVC_GetCurrCount - guess!        ***
;***                                  ***
;****************************************
BeginProc SVC_GetCurrCount
        mov     eax,SampleCount
        mov     [ebp.Client_EAX],eax
        ret
EndProc SVC_GetCurrCount

;****************************************
;***                                  ***
;*** SVC_GetSample0Tick - guess!      ***
;***                                  ***
;****************************************
BeginProc SVC_GetSample0Tick
        mov     eax,Sample0Tick
        mov     [ebp.Client_EAX],eax
        ret
EndProc SVC_GetSample0Tick


;*******************************************************
;***                                                 ***
;*** SVC_RegisterName - register someone as a server ***
;***                                                 ***
;*** Client ES - segment (V86), selector (pm)        ***
;***        BX - offset                              ***
;***                                                 ***
;*******************************************************
BeginProc SVC_RegisterName

        call    GetFlatAddr_ES_BX
        mov     StringAddr,eax                  ; save string address

        call    FindServerPtrFromName
        cmp     eax,0                           ; was there a name?
        je      short findslot                  ; no, go find a slot
        mov     [ebp.Client_AX],ERR_SERVER_EXISTS
        ret

;*
;*** find empty name slot
;*
findslot:
        call    FindEmptySlot
        cmp     eax,0                           ; found a slot?
        jne     short fnd_slot_name             ; yep
        mov     [ebp.Client_AX],ERR_NO_MORE_CONVS
        ret

;*
;*** got a name, register it
;*
fnd_slot_name:
        mov     edx,eax                         ; get server data ptr

        mov     ebx,VMHandle                    ; get handle of server
        mov     [edx.C_MyID],ebx                ; stash it
        mov     [edx.C_InUse],1                 ; in use
        mov     [edx.C_IsServer],1              ; a server
        movzx   eax,IsPM
        mov     [edx.C_IsPM],al
        mov     eax,StringAddr                  ; server name
        lea     ebx,[edx.C_ServerName]          ; where to store it
again5:
        mov     dl,byte ptr [eax]               ; i'm too lazy to code
        mov     byte ptr [ebx],dl               ;   real string copy
        cmp     dl,0
        je      short donex
        inc     ebx
        inc     eax
        jmp     short again5
donex:
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_RegisterName

;******************************************************
;***                                                ***
;*** SVC_AccessName - get ID associated with a name ***
;***                                                ***
;*** Client es:si - name segment:offset             ***
;***        di:si - id segment:offset               ***
;***                                                ***
;******************************************************
BeginProc SVC_AccessName
        call    GetFlatAddr_ES_BX
        call    FindServerPtrFromName           ; go find name
        cmp     eax,0                           ; did we get a name?
        jne     short gotname                   ; yes
        mov     [ebp.Client_AX],ERR_NO_SUCH_SERVER
        ret
;*
;*** make sure this client isn't already talking to this server
;*
gotname:
        mov     SaveServer,eax                  ; save server data ptr
        mov     eax,[eax.C_MyID]
        mov     ServerID,eax
        mov     eax,VMHandle                    ; client id
        call    FindClientPtr
        cmp     eax,0                           ; client found?
        je      short findcl                    ; nope
        mov     [ebp.Client_AX],ERR_ALREADY_ACCESSED_SERVER
        ret
;*
;*** find client slot
;*
findcl:
        call    FindEmptySlot
        cmp     eax,0                           ; found?
        jne     short gotid                     ; yes, its a blank spot
        mov     [ebp.Client_AX],ERR_NO_MORE_CONVS
        ret
gotid:
        mov     ebx,eax                         ; client ptr
        mov     eax,VMHandle
        mov     [ebx.C_MyID],eax                ; save client id
        mov     [ebx.C_InUse],1
        movzx   eax,IsPM
        mov     [ebx.C_IsPM],al
        mov     eax,SaveServer
        mov     eax,[eax.C_MyID]                ; get server id
        mov     [ebx.C_ServerID],eax            ; save server id
        mov     ax,[ebp.Client_DI]
        mov     MapSeg,ax
        mov     ax,[ebp.Client_SI]
        mov     MapOff,ax
        call    GetFlatAddr                     ; get pointer to id save area
        mov     ebx,SaveServer                  ; get server data pointer
        mov     ecx,[ebx.C_MyID]                ; get server id
        mov     dword ptr [eax],ecx             ; give client server id
        mov     [ebp.Client_AX],0               ; ok!

        ret
EndProc SVC_AccessName

;*************************************************
;***                                           ***
;*** SVC_UnregisterName  - make server go away ***
;***                                           ***
;*** Client ES - segment (V86), selector (pm)  ***
;***        BX - offset                        ***
;***                                           ***
;*************************************************
BeginProc SVC_UnregisterName
        call    GetFlatAddr_ES_BX
        call    FindServerPtrFromName
        cmp     eax,0                           ; does the server exist?
        jne     short testforours               ; yes
        mov     [ebp.Client_AX],ERR_NO_SUCH_SERVER
        ret

testforours:
        mov     ebx,VMHandle
        cmp     [eax.C_MyID],ebx
        je      short testforclients
        mov     [ebp.Client_AX],ERR_NOT_THAT_SERVER
        ret
testforclients:
        push    eax
        mov     InUse,1
        mov     IsServer,0                              ; no servers
        mov     ServerID,ebx
        xor     eax,eax                                 ; any client
        call    FindAnyPtr
        cmp     eax,0
        je      short oktokill
        pop     eax
        mov     [ebp.client_AX],ERR_HAS_CLIENTS
        ret
oktokill:
        pop     eax
        mov     [eax.C_InUse],0
        mov     [ebp.Client_AX],0

        ret
EndProc SVC_UnregisterName

;*************************************************
;***                                           ***
;*** SVC_UnaccessName - remove a client        ***
;***                                           ***
;*** Client es:bx - server to disconnect from  ***
;***                                           ***
;*************************************************
BeginProc SVC_UnaccessName
        call    GetFlatAddr_ES_BX
        call    FindServerPtrFromName
        cmp     eax,0                           ; does the server exist?
        jne     short findclx                   ; yes
        mov     [ebp.Client_AX],ERR_NO_SUCH_SERVER
        ret

findclx:
        mov     eax,[eax.C_ServerID]            ; server id to test for
        mov     ServerID,eax                    ; server id to test for
        mov     eax,VMHandle                    ; client ID
        call    FindClientPtr
        cmp     eax,0                           ; is client attached?
        jne     short foundclx                  ; yes
        mov     [ebp.Client_AX],ERR_HAVE_NOT_ACCESSED_SERVER
        ret

foundclx:
        mov     [eax.C_InUse],0
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_UnaccessName

;**************************************************
;***                                            ***
;*** SVC_StartConv - start a conv with a server ***
;***                                            ***
;*** Client cx:bx  - handle of server           ***
;*** returns ax - number of convs               ***
;***                                            ***
;**************************************************
BeginProc SVC_StartConv
        call    GetIDFrom_CX_BX
        mov     ServerID,eax                    ; client must have this serv
        mov     eax,VMHandle                    ; client id
        call    FindClientPtr
        cmp     eax,0                           ; client found?
        jne     short oksc                      ; yes
        mov     [ebp.Client_AX],ERR_NO_SUCH_CONV
        ret
oksc:
        cmp     [eax.C_StartedConv],1           ; conv started?
        jne     short doit                      ; no
        mov     [ebp.Client_AX],ERR_CONV_NOT_STARTED
        ret
doit:
        mov     [eax.C_StartedConv],1           ; we want a conversation
        mov     [eax.C_AckConv],0               ; not acked yet
        mov     eax,ServerID
        call    FindServerPtr
        movzx   ebx,[eax.C_ConvCount]           ; get conv count
        inc     [eax.C_ConvCount]               ; increment it
        mov     [ebp.Client_AX],bx              ; return to user

        ret
EndProc SVC_StartConv

;***********************************************************
;***                                                     ***
;*** SVC_LookForConv - check for expecting conversations ***
;***                                                     ***
;***  Client ES:BX - pointer to client id                ***
;***                                                     ***
;***********************************************************
BeginProc SVC_LookForConv
        mov     eax,VMHandle
        call    FindServerPtr
        cmp     eax,0
        jne     short oklfc
        mov     [ebp.Client_AX],ERR_NOT_A_SERVER
        ret
oklfc:
        mov     ebx,VMHandle                    ; server id
        xor     ecx,ecx
        mov     edx,OFFSET Convs
again20:
        cmp     [edx.C_InUse],1                 ; in use?
        jne     short nextlfc                   ; nope, skip
        cmp     [edx.C_IsServer],1              ; a server?
        je      short nextlfc                   ; yes, skip
        cmp     [edx.C_ServerID],ebx            ; client with this server?
        jne     short nextlfc
        cmp     [edx.C_StartedConv],1           ; conv started
        jne     short nextlfc                   ; no
        cmp     [edx.C_AckConv],1               ; conv acked?
        je      short nextlfc                   ; yes
        mov     [edx.C_AckConv],1               ; ack conv

        push    edx
        call    GetFlatAddr_ES_BX
        pop     edx
        mov     ebx,[edx.C_MyID]                ; get client id
        mov     dword ptr [eax],ebx             ; write into server's area
        mov     [ebp.Client_AX],1               ; got a conv
        ret

nextlfc:
        inc     ecx
        add     edx,CONV_SIZE
        cmp     ecx,MAX_CONVS
        jne     again20

        mov     [ebp.Client_AX],0h              ; no clients waiting
        ret

EndProc SVC_LookForConv

;******************************************************
;***                                                ***
;*** SVC_EndConv - end a conversation with a server ***
;***                                                ***
;*** Client cx:bx  - handle of server               ***
;***                                                ***
;******************************************************
BeginProc SVC_EndConv
        call    GetIDFrom_CX_BX
        call    FindServerPtr
        cmp     eax,0
        jne     short okec
        mov     [ebp.Client_AX],ERR_NO_SUCH_SERVER
        ret
okec:
        mov     IDAddr,eax
        mov     eax,[eax.C_MyID]                ; server id
        mov     ServerID,eax
        mov     eax,VMHandle                    ; client ending
        call    FindClientPtr
        cmp     eax,0                           ; client with this server?
        jne     short okec1                     ; nope
        mov     [ebp.Client_AX],ERR_CONV_NOT_STARTED
        ret
okec1:
        mov     [eax.C_StartedConv],0
        mov     [eax.C_AckConv],0
        mov     eax,IDAddr
        dec     [eax.C_ConvCount]               ; one less conversation
        mov     [ebp.Client_AX],0
        ret

EndProc SVC_EndConv

;**************************************************
;***                                            ***
;*** MySuspend - a global event to suspend a VM ***
;***                                            ***
;**************************************************
BeginProc MySuspend
        push    ebx
        mov     ebx,edx
        VxDcall Suspend_VM
        pop     ebx
        ret
EndProc MySuspend

;****************************************
;***                                  ***
;*** DoGetPut - do get/put processing ***
;***                                  ***
;****************************************
DoGetPut PROC near
        mov     eax,VMHandle
        mov     InUse,1
        mov     IsServer,2                              ; isserver irrelevant
        mov     ServerID,0                              ; serverid irrelevant
        call    FindAnyPtr
        cmp     eax,0
        jne     short oktox
        mov     [ebp.Client_AX],ERR_NOT_CONNECTED
        ret
oktox:
        mov     IDAddr,eax
        call    GetIDFrom_CX_BX                         ; guy to get from
        cmp     eax,0                                   ; null, look for any
        jne     short specific

        ;*
        ;*** no id specified, so search all convs for someone blocked on
        ;*** the id of the guy doing the get
        ;*
        mov     eax,IDAddr                      ; our data
        mov     eax,[eax.C_MyID]                ; our id
        mov     edx,OFFSET Convs
        xor     ecx,ecx
loopmebaby:
        cmp     [edx.C_InUse],1
        jne     short nextmebaby
        cmp     [edx.C_WhoBlocked],eax          ; blocked on me?
        jne     short nextmebaby
        cmp     [edx.C_PutBlocked],1            ; is other guy put blocked?
        jne     short nextmebaby
        mov     eax,[edx.C_MyID]
        jmp     short specific
nextmebaby:
        add     edx,CONV_SIZE                   ; point at data
        inc     ecx                             ; next client
        cmp     ecx,MAX_CONVS                   ; last client?
        jne     loopmebaby
        jmp     short fried

specific:
        call    FindAnyPtr
        cmp     eax,0
        jne     short oktox2
fried:
        mov     [ebp.Client_AX],ERR_NO_SUCH_ID
        ret
oktox2:
        mov     OtherIDAddr,eax                         ; save other pointer
        mov     edx,IDAddr                              ; get our pointer

        ;*
        ;*** at this point: edx points to current guy, eax points to guy we
        ;*** are getting from/putting to
        ;*
        mov     [eax.C_PutPending],0
        mov     ebx,VMHandle
        cmp     IsGet,1                         ; are we doing ConvGet
        jne     short put1                      ; no, check for get blocked
        cmp     [eax.C_PutBlocked],1            ; is other guy put blocked?
        jmp     short put2
put1:
        cmp     [eax.C_GetBlocked],1            ; is other guy get blocked?
put2:
        jne     short mustblock                 ; no, we must block
        cmp     [eax.C_WhoBlocked],ebx          ; blocked on us?
        je      short noblock                   ; yes, do copy

mustblock:
        cmp     [ebp.Client_DI],BLOCK           ; should we block?
        je      short yesblock
        cmp     IsGet,1                         ; was it a put
        je      short ss1
        mov     [eax.C_PutPending],1            ; flag that we are trying a put
ss1:
        mov     [ebp.Client_AX],BLOCK           ; let him know he was to block
        ret
yesblock:
        cmp     IsGet,1                         ; are we doing ConvGet?
        jne     short put3                      ; no, then put block
        mov     [edx.C_GetBlocked],1            ; we are get blocked
        jmp     short put4
put3:
        mov     [edx.C_PutBlocked],1            ; we are put blocked
put4:
        mov     [edx.C_Regs],ebp                ; save state pointer
        mov     ebx,[eax.C_MyID]                ; other guys id
        mov     [edx.C_WhoBlocked],ebx          ; we are blocked on him
        mov     ax,[ebp.Client_SI]
        mov     [edx.C_Bytes],ax

        mov     ax,[ebp.Client_ES]
        mov     [edx.C_Seg],ax
        mov     ax,[ebp.Client_DX]
        mov     [edx.C_Off],ax

        mov     [ebp.Client_AX],NO_BLOCK
        mov     esi,OFFSET MySuspend
        mov     edx,VMHandle
        VxDcall Schedule_Global_Event
        ret

        ;*
        ;*** guy we are getting from is put blocked, so resume him and
        ;*** do the copy
        ;*
noblock:
        mov     eax,OtherIDAddr
        mov     [eax.C_WhoBlocked],0            ; not blocked on anyone

        ;*
        ;*** get the flat address for the other guy's data area
        ;*
        mov     cx,[eax.C_Seg]
        mov     MapSeg,cx
        mov     cx,[eax.C_Off]
        mov     MapOff,cx;
        push    VMHandle
        movzx   ecx,IsPM
        push    ecx
        movzx   ecx,[eax.C_IsPM]
        mov     IsPM,cx
        mov     eax,[eax.C_MyID]
        mov     VMHandle,eax
        call    GetFlatAddr                     ; go get address
        pop     ecx
        mov     IsPM,cx
        pop     VMHandle
        mov     esi,eax                         ; source (for get)

        ;*
        ;*** get flat address for current guy
        ;*
        mov     cx,[ebp.Client_ES]
        mov     MapSeg,cx
        mov     cx,[ebp.Client_DX]
        mov     MapOff,cx
        call    GetFlatAddr                     ; go get address
        mov     edi,eax                         ; ours is destination
                                                ;    (for get operation)

        cmp     IsGet,1                         ; are we doing ConvGet
        je      short put7                      ; yes, esi and edi are right
        xchg    esi,edi                         ; swap for put operation
put7:
        movzx   ecx,[ebp.Client_SI]             ; copy size
        mov     ebx,OtherIDAddr
        movzx   ebx,[ebx.C_Bytes]               ; get other size
        cmp     ebx,ecx                         ; is other size bigger?
        ja      short put8                      ; yes, keep smaller
        mov     ecx,ebx                         ; no, use smaller size
put8:
        mov     [ebp.Client_DX],cx              ; return number of bytes
        push    ecx
        cld
        rep     movsb
        mov     [ebp.Client_AX],NO_BLOCK        ; copy succeeded

        mov     eax,IDAddr                      ; cancel timer of requestor
        mov     esi,[eax.C_TimerHandle]
        VxDcall Cancel_Time_Out
        mov     [eax.C_TimerHandle],0

        mov     eax,OtherIDAddr
        mov     esi,[eax.C_TimerHandle]         ; cancel timer of other guy
        VxDcall Cancel_Time_Out
        mov     [eax.C_TimerHandle],0

        mov     [eax.C_PutBlocked],0            ; turn off for Resume Handler
        mov     [eax.C_GetBlocked],0            ; turn off for Resume Handler

        pop     ecx                             ; number of bytes
        mov     ebx,[eax.C_Regs]
        mov     [ebx.Client_DX],cx              ; give guy we unblock bytes
        mov     ebx,[eax.C_MyID]                ; resume other guy
        VxDcall Resume_VM
        jnc     short gr1                       ; did it work?
        cmp     IsGet,1                         ; no, are we doing ConvGet
        jne     short put5                      ; no, then doing put
        mov     [eax.C_PutBlocked],1            ; restore put blocked status
        jmp     short put6
put5:
        mov     [eax.C_GetBlocked],1            ; restore get blocked status
put6:
        mov     [ebp.Client_AX],ERR_COULD_NOT_RESUME
        ret
gr1:
        ret
DoGetPut ENDP

;*******************************************
;***                                     ***
;*** SVC_ConvGet - get data from someone ***
;***                                     ***
;*** client ES:DX - data to copy         ***
;***        CX:BX                        ***
;***        SI    - # of bytes           ***
;***        DI    - block/noblock        ***
;***                                     ***
;*******************************************
BeginProc SVC_ConvGet
        mov     IsGet,1
        call    DoGetPut
        ret
EndProc SVC_ConvGet

;*******************************************
;*** SVC_ConvPut - give data to someone  ***
;***                                     ***
;*** client ES:DX - data to copy         ***
;***        CX:BX                        ***
;***        SI    - # of bytes           ***
;***        DI    - block/noblock        ***
;***                                     ***
;*******************************************
BeginProc SVC_ConvPut
        mov     IsGet,0
        call    DoGetPut
        ret
EndProc SVC_ConvPut

;***********************************************
;***                                         ***
;*** SetDataTimeOut - start a global timeout ***
;***                                         ***
;***********************************************
SetDataTimeOut PROC near
        mov     edx,IDAddr                  ; reference data - ID ptr
        mov     esi,OFFSET DataTimedOut   ; callback routine
        VxDcall Set_Global_Time_Out
        ret
SetDataTimeOut ENDP

;********************************************************
;***                                                  ***
;*** DataTimedOut - Get/Put timed out                 ***
;***                                                  ***
;***        EDX - ptr to id for timedout VM           ***
;***                                                  ***
;********************************************************
BeginProc DataTimedOut
        mov     [edx.C_TimerHandle],0
        mov     eax,[edx.C_Regs]                ; retreive register pointer
        cmp     [edx.C_PutBlocked],0
        jne     short isblocked
        cmp     [edx.C_GetBlocked],0
        jne     short isblocked
        jmp     short notblocked
isblocked:
        mov     [eax.Client_AX],ERR_TIMED_OUT   ; set error code
        mov     [edx.C_PutBlocked],0            ; turn off blocked flag
        mov     [edx.C_GetBlocked],0
        mov     ebx,[edx.C_MyID]                ; resume VM
        VxDcall Resume_VM
                                                ; this is bad news
notblocked:
        ret
EndProc DataTimedOut

;************************************************************
;***                                                      ***
;*** DoGetPutTimeOut - do get/put processing with timeout ***
;***                                                      ***
;************************************************************
DoGetPutTimeOut PROC near
        movzx   eax,[ebp.Client_DI]
        mov     DataTimeOut,eax
        cmp     eax,0
        je      short doblock
        mov     [ebp.Client_DI],NO_BLOCK
        call    DoGetPut
        cmp     [ebp.Client_AX],BLOCK
        jne     short notimer
        mov     eax,DataTimeOut
        call    SetDataTimeOut
        mov     eax,IDAddr
        mov     [eax.C_TimerHandle],esi
doblock:
        mov     [ebp.Client_DI],BLOCK
        call    DoGetPut
notimer:
        ret
DoGetPutTimeOut ENDP

;***********************************************************************
;***                                                                 ***
;*** SVC_IsConvAck - check if client has it's conversation acked yet ***
;***                                                                 ***
;*** client cx:bx - server id to check for                           ***
;***                                                                 ***
;***********************************************************************
BeginProc SVC_IsConvAck
        call    GetIDFrom_CX_BX
        mov     ServerID,eax                    ; server and client pair
        mov     eax,VMHandle                    ; vm to look for
        call    FindClientPtr
        cmp     eax,0
        jne     short ok99
        mov     [ebp.Client_AX],ERR_NO_SUCH_CONV
        ret
ok98:
        cmp     [eax.C_StartedConv],1           ; conv started?
        je      short ok99                      ; yes
        mov     [ebp.Client_AX],ERR_CONV_NOT_STARTED
        ret
ok99:
        movzx   eax,byte ptr [eax.C_AckConv]
        mov     [ebp.Client_AX],ax              ; 0 (no) or 1 (yes)
        ret
EndProc SVC_IsConvAck

;**********************************
;***                            ***
;*** SVC_MyID - return id of VM ***
;***                            ***
;**********************************
BeginProc SVC_MyID
        mov     [ebp.Client_EAX],ebx
        ret
EndProc SVC_MyID

;****************************************************
;***                                              ***
;*** SVC_SetExecutionFocus - start a VM executing ***
;***                                              ***
;*** client cx:bx - id to start up                ***
;***                                              ***
;****************************************************
BeginProc SVC_SetExecutionFocus
        call    GetIDFrom_CX_BX
        mov     ebx,eax
        mov     eax,200
        VxDcall Adjust_Execution_Time
        ;VxDcall        Set_Execution_Focus
        ret
EndProc SVC_SetExecutionFocus

;************************************************************
;***                                                      ***
;*** SVC_WhatHappened - check why a breakpoint was placed ***
;***                                                      ***
;*** returns : ax - fault number, or ff00 (break)         ***
;***                or ffff (nothing)                     ***
;***                                                      ***
;************************************************************
BeginProc SVC_WhatHappened
        mov     ax,FaultType
        mov     [ebp.Client_AX],ax
        mov     FaultType,0ffffh
        ret
EndProc SVC_WhatHappened

;**************************************************
;***                                            ***
;*** SVC_ConvGetTimeout - get data from someone ***
;***                                            ***
;*** client ES:DX - data to copy                ***
;***        CX:BX                               ***
;***        SI    - # of bytes                  ***
;***        DI    - milliseconds to timeout     ***
;***                                            ***
;**************************************************
BeginProc SVC_ConvGetTimeout
        mov     IsGet,1
        call    DoGetPutTimeOut
        ret
EndProc SVC_ConvGetTimeout

;**************************************************
;***                                            ***
;*** SVC_ConvPutTimeout - give data to someone  ***
;***                                            ***
;*** client ES:DX - data to copy                ***
;***        CX:BX                               ***
;***        SI    - # of bytes                  ***
;***        DI    - milliseconds to timeout     ***
;***                                            ***
;**************************************************
BeginProc SVC_ConvPutTimeout
        mov     IsGet,0
        call    DoGetPutTimeOut
        ret
EndProc SVC_ConvPutTimeout

;*********************************************
;***                                       ***
;*** SetCR0 - scheduled event to reset CR0 ***
;***                                       ***
;*********************************************
BeginProc SetCR0
        push    eax
        mov     eax,cr0
        or      eax,4h                  ; turn on emulation
        and     eax,not 2h              ; no WAIT's
        mov     cr0,eax
        pop     eax
        ret
EndProc SetCR0

;**************************************************
;***                                            ***
;*** TaskSwitched - handles resetting CR0       ***
;***                                            ***
;**************************************************
TaskSwitched PROC NEAR
        cmp     IsEMUInit,0             ; anyone need emulation?
        jne     short try               ; yep
        ret                             ; no, leave NOW
try:
        xor     ecx,ecx
        mov     edi,OFFSET EMUList
again_ts:
        cmp     [edi.E_ID],ebx          ; current VM?
        je      short fnd_ts            ; yes, turn on emulation
        add     edi,EMU_SIZE
        inc     ecx
        cmp     ecx,MAX_EMU_REG
        jne     short again_ts
        ret
fnd_ts:
        mov     esi,OFFSET SetCR0
        VxDcall Schedule_Global_Event
        ret
TaskSwitched ENDP


;**************************************************
;***                                            ***
;*** SVC_EMUInit - initialize emulator          ***
;***                                            ***
;*** returns: ax = 0, -1 (success, failure)     ***
;***                                            ***
;**************************************************
BeginProc SVC_EMUInit
        cmp     IsEMUInit,0                     ; are we already emulating?
        je      short initmebaby                ; nope
        inc     IsEMUInit                       ; add another case
        mov     [ebp.Client_AX],0FFFFh
        ret

initmebaby:
        cmp     TaskSwitcherActive,1
        je      short yes_active
        mov     esi,OFFSET TaskSwitched
        VxDCall Call_When_Task_Switched
        mov     TaskSwitcherActive,1
yes_active:
        mov     IsEMUInit,1                     ; flag that we are emulating

        mov     eax,07h
        mov     esi,OFFSET Fault07Handler
        VxDcall Hook_PM_Fault                   ; grab int
        jnc     short ok07
        mov     [ebp.Client_AX],1
        ret
ok07:
        cmp     esi,0
        jne     short aret7
        mov     esi,OFFSET JustReturn
aret7:
        mov     Old07Handler,esi                ; stash old handler

;*
;*** turn on emulating bits
;*
        mov     eax,cr0
        mov     OldCR0,eax              ; save CR0
        or      eax,4h                  ; turn on emulation
        and     eax,not 2h              ; no WAIT's
        mov     cr0,eax

        mov     [ebp.Client_AX],0
        ret

EndProc SVC_EMUInit

;**************************************************
;***                                            ***
;*** SVC_EMUShutdown - turn off emulator        ***
;***                                            ***
;*** returns: ax = 0, -1 (success, failure)     ***
;***                                            ***
;**************************************************
BeginProc SVC_EMUShutdown
;*
;*** one less guy wanting emulation
;*
        dec     IsEMUInit
        cmp     IsEMUInit,0                     ; are we initialized
        je      short shutmedownbaby            ; yes, close down
        mov     [ebp.Client_AX],0FFFFh          ; not done yet
        ret

;*
;*** no one else wants emulation, kill it
;*
shutmedownbaby:
        mov     eax,07h
        mov     esi,Old07Handler
        VxDcall Hook_PM_Fault                   ; reset interrupt

        mov     eax,OldCR0                      ; put CR0 back
        mov     cr0,eax

        mov     [ebp.Client_AX],0               ; all done
        ret
EndProc SVC_EMUShutdown

;**********************************************************************
;***                                                                ***
;*** SVC_EMURegister - register a routine for getting FPU emulation ***
;***                                                                ***
;*** client dx - CS of client (must be FLAT, obviously)             ***
;***        cx:bx - 8087 work area ptr                              ***
;***                                                                ***
;*** returns - ax 0,-1 (success, failure)                           ***
;***                                                                ***
;**********************************************************************
BeginProc SVC_EMURegister

;*
;*** find an empty list element
;*
        xor     ecx,ecx
        mov     edi,OFFSET EMUList
again100:
        cmp     word ptr [edi.E_CS],0
        je      short fnd100
        add     edi,EMU_SIZE
        inc     ecx
        cmp     ecx,MAX_EMU_REG
        jne     short again100
        mov     [ebp.Client_AX],0ffffh
        ret
fnd100:
        mov     eax,VMHandle
        mov     [edi.E_ID],eax          ; current VM
;*
;*** get 8087 work area
;*
        call    GetIDFrom_CX_BX
        mov     [edi.E_8087],eax

;*
;*** save CS away
;*
        mov     bx,[ebp.Client_DX]
        mov     word ptr [edi.E_CS],bx
        mov     [ebp.Client_AX],0

        ret

EndProc SVC_EMURegister

;**********************************************************************
;***                                                                ***
;*** SVC_EMUUnRegister - unregister a routine for getting FPU       ***
;***                                                                ***
;*** client dx - CS of client (must be FLAT, obviously)             ***
;***                                                                ***
;*** returns - ax 0,-1 (success, failure)                           ***
;***                                                                ***
;**********************************************************************
BeginProc SVC_EMUUnRegister

;*
;*** find the specified CS
;*
        xor     ecx,ecx
        mov     eax,OFFSET EMUList
        mov     bx,[ebp.Client_DX]
again101:
        cmp     word ptr [eax.E_CS],bx
        je      short fnd101
        add     eax,EMU_SIZE
        inc     ecx
        cmp     ecx,MAX_EMU_REG
        jne     short again101
        mov     [ebp.Client_AX],0ffffh
        ret
;*
;*** clear out the register
;*
fnd101:
        mov     word ptr [eax.E_CS],0
        mov     [ebp.Client_AX],0
        ret

EndProc SVC_EMUUnRegister

;**********************************************************************
;***                                                                ***
;*** SVC_FPUPresent - tell if an FPU is there                       ***
;***                                                                ***
;*** returns - ax 1,0  (true, false)                                ***
;***                                                                ***
;**********************************************************************
BeginProc SVC_FPUPresent

        mov     ax,HasFPU
        mov     [ebp.Client_AX],ax
        ret

EndProc SVC_FPUPresent

just_an_fsave:
                fsave   [eax]
just_an_frstor:
                frstor  [eax]
                nop

;**********************************************************************
;***                                                                ***
;*** SVC_EMUSaveRestore - get a copy of an 8087 save area           ***
;***                                                                ***
;*** Client dx - CS of guy to copy                                  ***
;***        cx:bx - far ptr of where to copy it to                  ***
;***        si - 0 = fsave, 1 = frstor                              ***
;***                                                                ***
;*** returns - ax 0,-1 (success, failure)                           ***
;***                                                                ***
;**********************************************************************
BeginProc SVC_EMUSaveRestore

;*
;*** find an empty list element
;*
        xor     ecx,ecx
        mov     edi,OFFSET EMUList
        mov     ax,[ebp.Client_DX]
again103:
        cmp     word ptr [edi.E_CS],ax
        je      short fnd103
        add     edi,EMU_SIZE
        inc     ecx
        cmp     ecx,MAX_EMU_REG
        jne     short again103
        mov     [ebp.Client_AX],0ffffh
        ret
fnd103:
;*
;*** stash away info
;*
        mov     ax,[ebp.Client_CX]
        mov     MapSeg,ax
        mov     ax,[ebp.Client_BX]
        mov     MapOff,ax
        call    GetFlatAddr

        mov     [ebp.Client_EAX], eax           ; where to save/restore state

        push    [ebp.Client_EIP]
        push    dword ptr [ebp.Client_CS]
        push    dword ptr [ebp.Client_DS]

        mov     [ebp.Client_CS],cs
        mov     [ebp.Client_DS],ds

        mov     [ebp.Client_EIP],OFFSET just_an_fsave
        cmp     [ebp.Client_SI],0
        jne     short label199
        mov     [ebp.Client_EIP],OFFSET just_an_frstor
label199:
        mov     ebx,VMHandle
        movzx   edx,[edi.E_CS]
        VxDcall3 _SelectorMapFlat, ebx, edx, 0    ; returns linear addr in eax
        mov     ebx,dword ptr [edi.E_8087]      ; 8087 ptr
        add     ebx,eax                         ; add linear base addr

        mov     eax,0                           ; ring 0 offset of cs
        mov     ecx,0                           ; NO VM IS ACTIVE

        call    __Win387_emulator

        pop     dword ptr [ebp.Client_DS]
        pop     dword ptr [ebp.Client_CS]
        pop     [ebp.Client_EIP]

        mov     [ebp.Client_AX],0
        ret


EndProc SVC_EMUSaveRestore

;****************************************************************
;***                                                          ***
;*** SVC_PauseSampler - temporary suspension of sample taking ***
;***                                                          ***
;****************************************************************
BeginProc SVC_PauseSampler
        mov     StopSamples,1
        ret
EndProc SVC_PauseSampler

;*************************************************
;***                                           ***
;*** SVC_UnPauseSampler - resume sample taking ***
;***                                           ***
;*************************************************
BeginProc SVC_UnPauseSampler
        mov     StopSamples,0
        ret
EndProc SVC_UnPauseSampler

;*******************************************
;***                                     ***
;*** SVC_EGAWrite - write ega registers  ***
;*** Client dx - register to write to    ***
;***        bx - value to write          ***
;***                                     ***
;*******************************************
BeginProc SVC_EGAWrite
        mov     dx,[ebp.Client_DX]
        mov     ax,[ebp.Client_BX]
        out     dx,ax
        ret
EndProc SVC_EGAWrite

;*******************************************
;***                                     ***
;*** SVC_VGARead - read vga registers    ***
;*** Client dx - register to write to    ***
;***        bx - value to write          ***
;***                                     ***
;*******************************************
BeginProc SVC_VGARead
        mov     dx,[ebp.Client_DX]
        mov     ax,[ebp.Client_BX]
        out     dx,al
        inc     dx
        in      al,dx
        xor     ah,ah
        mov     [ebp.Client_AX],ax
        ret
EndProc SVC_VGARead

;*******************************************
;***                                     ***
;*** SVC_DisableVideo - disable vga      ***
;*** Client dx - register to use         ***
;***                                     ***
;*******************************************
BeginProc SVC_DisableVideo
        mov     dx,[ebp.Client_DX]
fooxx:
        in al,dx
        test al,08h
        jz fooxx
        mov dx,03c0h
        mov al,011h
        out dx,al
        xor al,al
        out dx,al
        ret
EndProc SVC_DisableVideo

;*****************************************************************************
;***                                                                       ***
;*** SVC_RegisterInterruptCallback - set up to call 16-bit code for faults ***
;*** Client cx:bx - callback routine addr                                  ***
;***        es:dx - data work area                                         ***
;***        si:di - stack area                                             ***
;***                                                                       ***
;*** returns - ax 0,-1 (success, failure)                                  ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_RegisterInterruptCallback
        cmp     ICCodeSeg,0
        je      short okric
        mov     [ebp.Client_AX],0FFFFh
        ret
okric:
        mov     ICVM,ebx                ; save VM
        mov     ax,[ebp.Client_CX]
        mov     ICCodeSeg,ax
        mov     ax,[ebp.Client_BX]
        mov     ICCodeOff,ax
        mov     ax,[ebp.Client_ES]
        mov     ICDataSeg,ax
        mov     ax,[ebp.Client_DX]
        mov     ICDataOff,ax
        mov     ax,[ebp.Client_DI]
        mov     ICStackSeg,ax
        mov     ax,[ebp.Client_SI]
        mov     ICStackOff,ax

        mov     ax,ICCodeSeg
        mov     MapSeg,ax
        mov     ax,ICCodeOff
        mov     MapOff,ax
        call    GetFlatAddr
        mov     ICCodeFlat,eax

        mov     ax,ICDataSeg
        mov     MapSeg,ax
        mov     ax,ICDataOff
        mov     MapOff,ax
        call    GetFlatAddr
        mov     ICDataFlat,eax
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_RegisterInterruptCallback

;*****************************************************************************
;***                                                                       ***
;*** SVC_UnRegisterInterruptCallback - no longer pass along faults         ***
;*** Client cx:bx - callback routine addr                                  ***
;***                                                                       ***
;*** returns - ax 0,-1 (success, failure)                                  ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_UnRegisterInterruptCallback
        mov     ax,[ebp.Client_CX]
        cmp     ICCodeSeg,ax
        je      short okuric
        mov     [ebp.Client_AX],0FFFFh
        ret
okuric:
        mov     ICVM,0ffffffffh
        mov     ICCodeSeg,0
        mov     ICCodeOff,0
        mov     ICCodeFlat,0
        mov     ICDataSeg,0
        mov     ICDataOff,0
        mov     ICDataFlat,0
        mov     ICStackSeg,0
        mov     ICStackOff,0
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_UnRegisterInterruptCallback


;*****************************************************************************
;***                                                                       ***
;*** SVC_GetInterruptCallback - info on current guy hooked                 ***
;*** Client cx:bx - data area                                              ***
;***                                                                       ***
;*** returns - ax 0,-1 (success, failure)                                  ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_GetInterruptCallback
        cmp     ICCodeSeg,0
        jne     short okgic
        mov     [ebp.Client_AX],0FFFFh
        ret
okgic:
        mov     ax,[ebp.Client_CX]
        mov     MapSeg,ax
        mov     ax,[ebp.Client_BX]
        mov     MapOff,ax
        call    GetFlatAddr

        mov     bx,ICCodeSeg
        mov     [eax],bx
        mov     bx,ICCodeOff
        mov     [eax+2],bx
        mov     bx,ICDataSeg
        mov     [eax+4],bx
        mov     bx,ICDataOff
        mov     [eax+6],bx
        mov     bx,ICStackSeg
        mov     [eax+8],bx
        mov     bx,ICStackOff
        mov     [eax+0ah],bx
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_GetInterruptCallback

;*****************************************************************************
;***                                                                       ***
;*** SVC_RestartFromInterrupt - restart current guy hooked, use register   ***
;***               area interrupt callback registered                      ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_RestartFromInterrupt
        mov     edx,ICDataFlat
        mov     eax,[edx.I_SaveEAX]
        mov     [ebp.Client_EAX],eax
        mov     eax,[edx.I_SaveEBX]
        mov     [ebp.Client_EBX],eax
        mov     eax,[edx.I_SaveECX]
        mov     [ebp.Client_ECX],eax
        mov     eax,[edx.I_SaveEDX]
        mov     [ebp.Client_EDX],eax
        mov     eax,[edx.I_SaveESI]
        mov     [ebp.Client_ESI],eax
        mov     eax,[edx.I_SaveEDI]
        mov     [ebp.Client_EDI],eax
        mov     eax,[edx.I_SaveEBP]
        mov     [ebp.Client_EBP],eax
        mov     eax,[edx.I_SaveESP]
        mov     [ebp.Client_ESP],eax
        mov     eax,[edx.I_SaveEIP]
        mov     [ebp.Client_EIP],eax
        mov     eax,[edx.I_SaveEFLAGS]
        mov     [ebp.Client_EFlags],eax
        mov     ax,[edx.I_SaveCS]
        mov     [ebp.Client_CS],ax
        mov     ax,[edx.I_SaveDS]
        mov     [ebp.Client_DS],ax
        mov     ax,[edx.I_SaveES]
        mov     [ebp.Client_ES],ax
        mov     ax,[edx.I_SaveFS]
        mov     [ebp.Client_FS],ax
        mov     ax,[edx.I_SaveGS]
        mov     [ebp.Client_GS],ax
        mov     ax,[edx.I_SaveSS]
        mov     [ebp.Client_SS],ax
        pop     eax
        ret
EndProc SVC_RestartFromInterrupt

;*****************************************************************************
;***                                                                       ***
;*** SVC_Is32BitSel -   restart current guy hooked, use register   ***
;***               area interrupt callback registered                      ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_Is32BitSel
        movzx   eax,[ebp.Client_BX]
        call    Is32BitSel
        je      short isnot
        mov     [ebp.Client_AX],1
        ret
isnot:  mov     [ebp.Client_AX],0
        ret
EndProc SVC_Is32BitSel

;*****************************************************************************
;***                                                                       ***
;*** SVC_GetVMId - return actual VM id                                     ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_GetVMId
        mov     eax,VMHandle
        mov     edx,eax
        shr     edx,16
        mov     [ebp.Client_AX],ax
        mov     [ebp.Client_DX],dx
        ret
EndProc SVC_GetVMId

;*******************************************************
;***                                                 ***
;*** SVC_HookIDT - hook IDT for int 1 and int 3      ***
;*** Client cx:bx - where to reflect interrupts to   ***
;***                                                 ***
;*******************************************************
BeginProc SVC_HookIDT

        mov     ax,[ebp.Client_CX]
        mov     ReflectToCS,ax
        movzx   eax,[ebp.Client_BX]
        mov     ReflectToEIP,eax

        mov     edi,IDTAddr

        lea     esi,[edi+1*8]

        mov     byte ptr [esi+5],0eeh           ; ring 3, present, 386 gate
        mov     word ptr [esi+2], cs            ; segment
        mov     edx, OFFSET Int01IDT
        mov     word ptr [esi], dx              ; lo word of offset
        shr     edx,16
        mov     word ptr [esi+6], dx            ; hi word of offset

        lea     esi,[edi+3*8]

        mov     byte ptr [esi+5],0eeh           ; ring 3, present, 386 gate
        mov     word ptr [esi+2], cs            ; segment
        mov     edx, OFFSET Int03IDT
        mov     word ptr [esi], dx              ; lo word of offset
        shr     edx,16
        mov     word ptr [esi+6], dx            ; hi word of offset

        ret
EndProc SVC_HookIDT

;*****************************************************************************
;***                                                                       ***
;*** SVC_IDTFini - done fiddleing IDT                                      ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_IDTFini

        cmp     IDTIsInit,1
        jne     short done_ifini
        mov     IDTIsInit,0
        call    IDTFini
done_ifini:
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_IDTFini

;*****************************************************************************
;***                                                                       ***
;*** SVC_IDTInit - initialize for fiddling IDT                             ***
;*** Client bx - idt sel                                                   ***
;***                                                                       ***
;*****************************************************************************
BeginProc SVC_IDTInit
        cmp     IDTIsInit,0
        jne     short done_iinit

        mov     IDTIsInit,1

        mov     ax,[ebp.Client_BX]
        mov     MapSeg,ax
        mov     MapOff,0
        call    GetFlatAddr
        mov     edi,eax

        mov     IDTAddr,edi                     ; save for later

        lea     esi,[edi+1*8]
        mov     ecx,dword ptr [esi]             ; first DWORD of entry
        mov     dword ptr [Idt01], ecx
        mov     ecx,dword ptr [esi+4]           ; second DWORD of entry
        mov     dword ptr [Idt01+4], ecx

        lea     esi,[edi+3*8]
        mov     ecx,dword ptr [esi]             ; first DWORD of entry
        mov     dword ptr [Idt03], ecx
        mov     ecx,dword ptr [esi+4]           ; second DWORD of entry
        mov     dword ptr [Idt03+4], ecx

done_iinit:
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_IDTInit


;*************************************************
;***                                           ***
;*** SVC_ConvPutPending - test for pending put ***
;***                                           ***
;*************************************************
BeginProc SVC_ConvPutPending
        mov     edx,OFFSET Convs
        xor     ecx,ecx
loopmetoo:
        cmp     [edx.C_InUse],1
        jne     short nextmetoo
        cmp     [edx.C_MyID],ebx
        je      short foundtoo
nextmetoo:
        add     edx,CONV_SIZE
        inc     ecx
        cmp     ecx,MAX_CONVS
        jne     short loopmetoo
        mov     [ebp.Client_AX],0
        ret
foundtoo:
        cmp     [edx.C_PutPending],1
        je      short putp
        mov     [ebp.Client_AX],0
        ret
putp:
        mov     [ebp.Client_AX],1
        ret

EndProc SVC_ConvPutPending

;*************************************************
;***                                           ***
;*** SVC_UseHotKey - use the hot key           ***
;*** Client bx - boolean value                 ***
;***                                           ***
;*************************************************
BeginProc SVC_UseHotKey
        movzx   eax,[ebp.Client_BX]
        mov     UseHotKey,eax
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_UseHotKey

BeginProc RaiseInterrupt
        Push_Client_State
        VxDcall Begin_Nest_Exec
        mov     eax,edx
        VxDcall Exec_Int
        VxDcall End_Nest_Exec
        Pop_Client_State
        ret
EndProc RaiseInterrupt


;*********************************************************************
;***                                                               ***
;*** SVC_RaiseInterruptInVM - Raise an interrupt in a specified VM ***
;*** client cx:bx - VM id                                          ***
;***           dx - interrupt to raise                             ***
;***                                                               ***
;*********************************************************************
BeginProc SVC_RaiseInterruptInVM
        call    GetIDFrom_CX_BX
        mov     ebx,eax
        movzx   edx,[ebp.Client_DX]
        mov     esi, OFFSET RaiseInterrupt
        VxDcall Schedule_VM_Event
        mov     [ebp.Client_AX],0
        ret
EndProc SVC_RaiseInterruptInVM

VxD_CODE_ENDS

        END
