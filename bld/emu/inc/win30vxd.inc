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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; VMM definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Get_Sys_VM_Handle        = 0010003h
Allocate_PM_Call_Back    = 001000Ch
Schedule_Global_Event    = 001000Eh
Schedule_VM_Event        = 001000Fh
Cancel_VM_Event          = 0010013h
Map_Flat                 = 001001Ch
Call_When_Task_Switched  = 001002Ah
Suspend_VM               = 001002Bh
Resume_VM                = 001002Ch
Adjust_Execution_Time    = 0010037h
Set_Global_Time_Out      = 001003Ch
Cancel_Time_Out          = 001003Eh
Get_VM_Exec_Time         = 0010040h
Hook_V86_Int_Chain       = 0010041h
Get_PM_Int_Vector        = 0010044h
Set_PM_Int_Vector        = 0010045h
Simulate_Iret            = 0010047h
Simulate_Far_Jmp         = 0010049h
_CopyPageTable           = 0010061h
_SelectorMapFlat         = 001006Eh
_GetDescriptor           = 001007Bh
Hook_PM_Fault            = 0010080h
Begin_Nest_Exec          = 0010083h
Exec_Int                 = 0010084h
End_Nest_Exec            = 0010086h
Save_Client_State        = 001008Dh
Restore_Client_State     = 001008Eh

; VMM services
SYS_CRITICAL_INIT    EQU    0000H
DEVICE_INIT          EQU    0001H
VM_SUSPEND           EQU    000DH
VM_RESUME            EQU    000EH

; Device ID's
UNDEFINED_DEVICE_ID  EQU    00000H
VMM_DEVICE_ID        EQU    00001H
DEBUG_DEVICE_ID      EQU    00002H


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; VKD definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VKD_Define_Hot_Key = 00D0001h


SS_LCtrl      equ 4
SS_LAlt       equ 8
SS_ScrlLock   equ 10h
SS_NumLock    equ 20h
SS_CapLock    equ 40h
SS_Ctrl       equ 80h
SS_Alt        equ 100h
SS_RCtrl      equ 400h
SS_RAlt       equ 800h

SS_Either_Ctrl   equ SS_LCtrl + SS_RCtrl
SS_Either_Alt    equ SS_LAlt + SS_RAlt
SS_Toggle_mask   equ SS_CapLock + SS_NumLock + SS_ScrlLock

CallOnPress equ 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; VTD definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VTD_Get_Interrupt_Period    = 0050002h
VTD_Begin_Min_Int_Period    = 0050003h
VTD_End_Min_Int_Period      = 0050004h


;******************************************************************************
; The following structures are pointed to by EBP when VxD routines are entered,
;   both for VxD control calls and traps(I/O traps, software INT traps, etc.).
;   The first structure as DWORD values, the second WORD values and the last
;   has BYTE values.
;

Client_Reg_Struc   struc
Client_EDI      dd      ?               ; Client's EDI
Client_ESI      dd      ?               ; Client's ESI
Client_EBP      dd      ?               ; Client's EBP
                dd      ?               ; ESP at pushall
Client_EBX      dd      ?               ; Client's EBX
Client_EDX      dd      ?               ; Client's EDX
Client_ECX      dd      ?               ; Client's ECX
Client_EAX      dd      ?               ; Client's EAX
Client_Error    dd      ?               ; Dword error code
Client_EIP      dd      ?               ; EIP
Client_CS       dw      ?               ; CS
                dw      ?               ;   (padding)
Client_EFlags   dd      ?               ; EFLAGS
Client_ESP      dd      ?               ; ESP
Client_SS       dw      ?               ; SS
                dw      ?               ;   (padding)
Client_ES       dw      ?               ; ES
                dw      ?               ;   (padding)
Client_DS       dw      ?               ; DS
                dw      ?               ;   (padding)
Client_FS       dw      ?               ; FS
                dw      ?               ;   (padding)
Client_GS       dw      ?               ; GS
                dw      ?               ;   (padding)
Client_Alt_EIP  dd      ?
Client_Alt_CS   dw      ?
                dw      ?
Client_Alt_EFlags  dd   ?
Client_Alt_ESP  dd      ?
Client_Alt_SS   dw      ?
                dw      ?
Client_Alt_ES   dw      ?
                dw      ?
Client_Alt_DS   dw      ?
                dw      ?
Client_Alt_FS   dw      ?
                dw      ?
Client_Alt_GS   dw      ?
                dw      ?
Client_Reg_Struc   ends

Client_Word_Reg_Struc    STRUC
Client_DI        DW    ?
Client_res13     DW    ?
Client_SI        DW    ?
Client_res14     DW    ?
Client_BP        DW    ?
Client_res15     DW    ?
Client_res16     DD    ?
Client_BX        DW    ?
Client_res17     DW    ?
Client_DX        DW    ?
Client_res18     DW    ?
Client_CX        DW    ?
Client_res19     DW    ?
Client_AX        DW    ?
Client_res20     DW    ?
Client_res21     DD    ?
Client_IP        DW    ?
Client_res22     DW    ?
Client_res23     DD    ?
Client_Flags     DW    ?
Client_res24     DW    ?
Client_SP        DW    ?
Client_res25     DW    ?
Client_res26     DD    5 DUP (?)
Client_Alt_IP    DW    ?
Client_res27     DW    ?
Client_res28     DD    ?
Client_Alt_Flags DW    ?
Client_res29     DW    ?
Client_Alt_SP    DW    ?
Client_Word_Reg_Struc    ENDS

cb_s STRUC
CB_VM_Status      DD ?
CB_High_Linear    DD ?
CB_Client_Pointer DD ?
CB_VMID           DD ?
CB_Signature      DD ?
cb_s ENDS

VxD_Desc_Block    STRUC
DDB_Next                 DD    ?
DDB_SDK_Version          DW    30AH
DDB_Req_Device_Number    DW    UNDEFINED_DEVICE_ID
DDB_Dev_Major_Version    DB    0
DDB_Dev_Minor_Version    DB    0
DDB_Flags                DW    0
DDB_Name                 DB    '        '
DDB_Init_Order           DD    080000000H

DDB_Control_Proc         DD    ?
DDB_V86_API_Proc         DD    0
DDB_PM_API_Proc          DD    0
DDB_V86_API_CSIP         DD    0
DDB_PM_API_CSIP          DD    0
DDB_Reference_Data       DD    ?
DDB_Service_Table_Ptr    DD    0
DDB_Service_Table_Size   DD    0
;********************
; Windows 9x, Me
;********************
DDB_Win32_Service_Table  DD    0
DDB_Prev                 DD    0
DDB_Size                 DD    SIZE(VxD_Desc_Block)
DDB_Reserved1            DD    0
DDB_Reserved2            DD    0
DDB_Reserved3            DD    0
;********************
VxD_Desc_Block    ENDS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Segments definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_LTEXT      SEGMENT DWORD PUBLIC USE32 'LCODE'
_LTEXT      ENDS

_LDATA      SEGMENT DWORD PUBLIC USE32 'LCODE'
_LDATA      ENDS

_TEXT       SEGMENT DWORD PUBLIC USE32 'LCODE'
_TEXT       ENDS

_DATA       SEGMENT DWORD PUBLIC USE32 'LCODE'
_DATA       ENDS

_ITEXT      SEGMENT DWORD PUBLIC USE32 'ICODE'
_ITEXT      ENDS

_IDATA      SEGMENT DWORD PUBLIC USE32 'ICODE'
_IDATA      ENDS


VxD_LOCKED_CODE_SEG macro
  _LTEXT    SEGMENT
  ASSUME    cs:FLAT, ds:FLAT, es:FLAT, ss:FLAT
endm

VxD_LOCKED_CODE_ENDS macro
  _LTEXT    ENDS
endm

VxD_ICODE_SEG macro
  _ITEXT    SEGMENT
  ASSUME    cs:FLAT, ds:FLAT, es:FLAT, ss:FLAT
endm

VxD_ICODE_ENDS macro
  _ITEXT    ENDS
endm

VxD_CODE_SEG macro
  _LTEXT    SEGMENT
  ASSUME    cs:FLAT, ds:FLAT, es:FLAT, ss:FLAT
endm

VxD_CODE_ENDS macro
  _LTEXT    ENDS
endm

VxD_LOCKED_DATA_SEG macro
  _LDATA    SEGMENT
endm

VxD_LOCKED_DATA_ENDS macro
  _LDATA    ENDS
endm

VxD_IDATA_SEG macro
  _IDATA    SEGMENT
endm

VxD_IDATA_ENDS macro
  _IDATA    ENDS
endm

VxD_DATA_SEG macro
  _LDATA    SEGMENT
endm

VxD_DATA_ENDS macro
  _LDATA    ENDS
endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Other macros definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

argscnt = 0

BeginProc macro Name, P1, P2, P3, P4, P5, P6, P7, LastArg
  Name proc near
  public Name
endm

EndProc macro Name
  Name endp
endm

Declare_Virtual_Device macro Name, Major_Ver, Minor_Ver, Ctrl_Proc, Device_Num, Init_Order, V86_Proc, PM_Proc, Reference_Data
  VxD_LOCKED_DATA_SEG
  PUBLIC Name&_DDB
  Name&_DDB VxD_Desc_Block <,,Device_Num,Major_Ver,Minor_Ver,,"&Name",Init_Order, OFFSET Ctrl_Proc, 0, 0,,, 0, 0, 0>
  VxD_LOCKED_DATA_ENDS
endm

VxDcall macro P
  int 20h
  dd  P
endm

VxDcall3 macro P, Param1, Param2, Param3
  push Param3
  push Param2
  push Param1
  int 20h
  dd  P
  add esp,0ch
endm

VxDcall3a macro P, Param1, Param2, Param3
  push OFFSET Param3
  push Param2
  push Param1
  int 20h
  dd  P
  add esp,0ch
endm

VxDjmp macro P
  int 20h
  dd  P + 8000h
endm

Control_Dispatch macro Service, Procedure
  cmp  eax, Service
  jz   Procedure
endm

ShiftState macro mask, compare
  mov  ebx, ((NOT mask) SHL 16) + compare
endm

Push_Client_State macro
  sub  esp, SIZE Client_Reg_Struc
  push edi
  lea  edi, [esp+4]
  VxDCall Save_Client_State
  pop  edi
endm

Pop_Client_State macro
  push esi
  lea  esi, [esp+4]
  VxDCall Restore_Client_State
  pop  esi
  add  esp, SIZE Client_Reg_Struc
endm

Client_Ptr_Flat MACRO Reg_32, Cli_Seg, Cli_Off, Can_Trash_EAX
  IFDIFI <Reg_32>, <EAX>
    xchg Reg_32, eax
  ENDIF
  IFB <Cli_Off>
    mov ax, (Client_&Cli_Seg * 100h) + 0FFh
  ELSE
    mov ax, (Client_&Cli_Seg * 100h) + Client_&Cli_Off
  ENDIF
    VxDCall Map_Flat

  IFDIFI <Reg_32>, <EAX>
    xchg Reg_32, eax
  ENDIF
endm
