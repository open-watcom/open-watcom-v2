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


; CODE386:     Pragma code bursts for built-in compiler pragmas
;
; - these routines are easier to code in assembler than in C
;
.386p
include struct.inc

        name    code386

_DATA   segment word public 'DATA'
DGROUP  group   _DATA
        assume  CS:_DATA

        public  _C_strcat
_C_strcat_name: db      "C_strcat",0
_C_strcat_defs:
_FC_strcat_defs:
  db    "/* edi strcat( edi, esi ) zaps eax,ecx,esi */",0
  db    "#define C_strcat_ret   HW_D( HW_EDI )",0
  db    "#define C_strcat_parms P_DI_SI",0
  db    "#define C_strcat_saves HW_NotD_3( HW_EAX, HW_ECX, HW_ESI )",0
  db    0
;
;       EDI strcat( EDI, ESI )
;       EAX,ECX and ESI are modified
;
_C_strcat:                              ; small data model strcat for -ot
        db      E_C_strcat - _C_strcat - 1
        push    es                      ; save es
        push    ds                      ; set es=ds
        pop     es                      ; ...
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_C_strcat1       ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_C_strcat1:
        pop     edi                     ; restore destination address
        pop     es                      ; restore es
E_C_strcat:

        public  _FC_strcat
_FC_strcat_name:        db      "FC_strcat",0
;
;       EDI strcat( EDI, ESI )
;       EAX,ECX and ESI are modified
;
_FC_strcat:                             ; flat data model strcat for -ot
        db      E_FC_strcat - _FC_strcat - 1
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_FC_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_FC_strcat1:
        pop     edi                     ; restore destination address
E_FC_strcat:

        public  _S_strcat
_S_strcat_name: db      "S_strcat",0
_S_strcat_defs:
  db    "/* edi strcat( edi, esi ) zaps eax,esi */",0
  db    "#define S_strcat_ret   HW_D( HW_EDI )",0
  db    "#define S_strcat_parms P_DI_SI",0
  db    "#define S_strcat_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       EDI strcat( EDI, ESI )
;       EAX and ESI are modified
;
_S_strcat:                              ; small data model strcat for -os
        db      E_S_strcat - _S_strcat - 1
        push    edi                     ; save destination address
        dec     edi                     ; back up one to start
        _loop                           ; loop (find end of first string)
          inc   edi                     ; - point to next byte
          cmp   byte ptr [edi],0        ; - check for null character
        _until  e                       ; until end of first string
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          mov   [edi],al                ; - append to end of dest
          inc   edi                     ; - point to next byte
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination string address
E_S_strcat:

        public  _Z_strcat
_Z_strcat_name: db      "Z_strcat",0
_Z_strcat_defs:
  db    "/* es:edi strcat( es:edi, ds:esi ) zaps eax,esi */",0
  db    "#define Z_strcat_ret   HW_D_2( HW_ES, HW_EDI )",0
  db    "#define Z_strcat_parms P_ESDI_DSSI",0
  db    "#define Z_strcat_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       ES:EDI strcat( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
_Z_strcat:                              ; large data model strcat for -os
        db      E_Z_strcat - _Z_strcat - 1
        push    edi                     ; save destination address
        push    ecx                     ; save ecx
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        pop     ecx                     ; restore ecx
        dec     edi                     ; point to null character
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          stosb                         ; - append to end of dest
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination string address
E_Z_strcat:

        public  _BD_strcat
_BD_strcat_name:        db      "BD_strcat",0
_BD_strcat_defs:
  db    "/* es:edi strcat( es:edi, ds:esi ) zaps ecx,eax,esi */",0
  db    "#define BD_strcat_ret  HW_D_2( HW_ES, HW_EDI )",0
  db    "#define BD_strcat_parms        P_ESDI_DSSI",0
  db    "#define BD_strcat_saves        HW_NotD_3( HW_ECX, HW_EAX, HW_ESI )",0
  db    0
;
;       ES:EDI strcat( ES:EDI, DS:ESI )
;       EAX,ECX and ESI are modified
;
_BD_strcat:                             ; large data model strcat for -ot
        db      E_BD_strcat - _BD_strcat - 1
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_BD_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          stosb                         ; - copy it
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_BD_strcat1:
        pop     edi                     ; restore destination address
E_BD_strcat:

        public  _DP_strcat
_DP_strcat_name:        db      "DP_strcat",0
_DP_strcat_defs:
  db    "/* dx:eax strcat( dx:eax, cx:ebx ) zaps ecx,eax,esi */",0
  db    "#define DP_strcat_ret   HW_D_2( HW_DX, HW_EAX )",0
  db    "#define DP_strcat_parms  P_DXEAX_CXEBX",0
  db    "#define DP_strcat_saves  HW_NotD_3( HW_ECX, HW_ESI, HW_EDI )",0
  db    0
;
;       DX:EAX _fstrcat( DX:EAX, CX:EBX )
;       EAX,ECX,EDI and ESI are modified
;
_DP_strcat:                             ; model-independent _fstrcat for -ot
        db      E_DP_strcat - _DP_strcat - 1
        push    ds                      ; save segment registers
        push    es                      ; ...
        push    eax                     ; save destination address
        mov     es,dx                   ; load segment registers
        mov     ds,cx                   ; ...
        mov     esi,ebx                 ; get source offset
        mov     edi,eax                 ; get destination offset
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_DP_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          stosb                         ; - copy it
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_DP_strcat1:
        pop     eax                     ; restore destination address
        pop     es                      ; restore segment registers
        pop     ds                      ; ...
E_DP_strcat:

;=======================================================================
        public  _C_strchr
_C_strchr_name: db      "C_strchr",0
_C_strchr_defs:
  db    "/* esi strchr( esi, dl ) zaps eax,esi */",0
  db    "#define C_strchr_ret   HW_D( HW_ESI )",0
  db    "#define C_strchr_parms P_SI_DL",0
  db    "#define C_strchr_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       ESI strchr( ESI, DL )
;       EAX and ESI are modified
;
_C_strchr:                              ; small data model strchr for -ot
        db      E_C_strchr - _C_strchr - 1
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          cmp   al,0                    ; - quit if end of string
          je    short E_C_strchr1       ; - ...
          inc   esi                     ; - increment pointer
          mov   al,[esi]                ; - get next char
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          inc   esi                     ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_C_strchr1:
        sub     esi,esi                 ; return NULL
E_C_strchr:

        public  _S_strchr
_S_strchr_name: db      "S_strchr",0
_S_strchr_defs:
  db    "/* esi strchr( esi, dl ) zaps eax,esi */",0
  db    "#define S_strchr_ret   HW_D( HW_ESI )",0
  db    "#define S_strchr_parms P_SI_DL",0
  db    "#define S_strchr_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       ESI strchr( ESI, DL )
;       EAX and ESI are modified
;
_S_strchr:                              ; small data model strchr for -os
        db      E_S_strchr - _S_strchr - 1
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,dl                   ; - quit if char found
          je    short E_S_strchr        ; - ...
          inc   esi                     ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        sub     esi,esi                 ; return NULL
E_S_strchr:

        public  _BD_strchr
_BD_strchr_name:        db      "BD_strchr",0
_BD_strchr_defs:
  db    "/* ds:esi strchr( ds:esi, cl ) zaps dx,esi,eax */",0
  db    "#define BD_strchr_ret  HW_D_2( HW_DX, HW_ESI )",0
  db    "#define BD_strchr_parms P_DSSI_CL",0
  db    "#define BD_strchr_saves  HW_NotD_3( HW_DX, HW_ESI, HW_EAX )",0
  db    0
;
;       DX:ESI strchr( DS:ESI, CL )
;       EAX,EDX and ESI are modified
;
_BD_strchr:                             ; large data model strchr for -ot
        db      E_BD_strchr - _BD_strchr - 1
        mov     dx,ds                   ; save ds
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,cl                   ; - quit if char found
          je    short E_BD_strchr       ; - ...
          inc   esi                     ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        sub     esi,esi                 ; return NULL
        sub     edx,edx                 ; ...
E_BD_strchr:

;=======================================================================
        public  _C_strcpy
_C_strcpy_name: db      "C_strcpy",0
_C_strcpy_defs:
  db    "/* edi strcpy( edi, esi ) zaps eax,esi */",0
  db    "#define C_strcpy_ret   HW_D( HW_EDI )",0
  db    "#define C_strcpy_parms P_DI_SI",0
  db    "#define C_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       EDI strcpy( EDI, ESI )
;       EAX and ESI are modified
;
_C_strcpy:                              ; small data model strcpy for -ot
        db      E_C_strcpy - _C_strcpy - 1
        push    edi                     ; save destination address
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_C_strcpy1       ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_C_strcpy1:
        pop     edi                     ; restore destination address
E_C_strcpy:

        public  _S_strcpy
_S_strcpy_name: db      "S_strcpy",0
_S_strcpy_defs:
  db    "/* edi strcpy( edi, esi ) zaps eax,esi */",0
  db    "#define S_strcpy_ret   HW_D( HW_EDI )",0
  db    "#define S_strcpy_parms P_DI_SI",0
  db    "#define S_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       EDI strcpy( EDI, ESI )
;       EAX and ESI are modified
;
_S_strcpy:                              ; small data model strcpy for -os
        db      E_S_strcpy - _S_strcpy - 1
        push    edi                     ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          mov   [edi],al                ; - store in output buffer
          inc   edi                     ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination address
E_S_strcpy:

        public  _Z_strcpy
_Z_strcpy_name: db      "Z_strcpy",0
_Z_strcpy_defs:
  db    "/* es:edi strcpy( es:edi, ds:esi ) zaps eax,esi */",0
  db    "#define Z_strcpy_ret   HW_D_2( HW_ES, HW_EDI )",0
  db    "#define Z_strcpy_parms P_ESDI_DSSI",0
  db    "#define Z_strcpy_saves  HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       ES:EDI strcpy( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
_Z_strcpy:                              ; large data model strcpy for -os
        db      E_Z_strcpy - _Z_strcpy - 1
        push    edi                     ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination address
E_Z_strcpy:

        public  _BD_strcpy
_BD_strcpy_name:        db      "BD_strcpy",0
_BD_strcpy_defs:
  db    "/* es:edi strcpy( es:edi, ds:esi ) zaps eax,esi */",0
  db    "#define BD_strcpy_ret  HW_D_2( HW_ES, HW_EDI )",0
  db    "#define BD_strcpy_parms P_ESDI_DSSI",0
  db    "#define BD_strcpy_saves  HW_NotD_2( HW_EAX, HW_ESI )",0
  db    0
;
;       ES:EDI strcpy( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
_BD_strcpy:                             ; large data model strcpy for -ot
        db      E_BD_strcpy - _BD_strcpy - 1
        push    edi                     ; save destination address
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   es:[edi],al             ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_BD_strcpy1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   es:1[edi],al            ; - copy it
          add   edi,2                   ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_BD_strcpy1:
        pop     edi                     ; restore destination address
E_BD_strcpy:

        public  _DP_strcpy
_DP_strcpy_name:        db      "DP_strcpy",0
_DP_strcpy_defs:
  db    "/* dx:eax strcpy( dx:eax, cx:ebx ) zaps esi,edi */",0
  db    "#define DP_strcpy_ret  HW_D_2( HW_DX, HW_EAX )",0
  db    "#define DP_strcpy_parms P_DXEAX_CXEBX",0
  db    "#define DP_strcpy_saves  HW_NotD_2( HW_ESI, HW_EDI )",0
  db    0
;
;       DX:EAX _fstrcpy( DX:EAX, CX:EBX )
;       EDI and ESI are modified
;
_DP_strcpy:                             ; model-independent _fstrcpy
        db      E_DP_strcpy - _DP_strcpy - 1
        push    ds                      ; save segment registers
        push    es                      ; ...
        push    eax                     ; save destination address
        mov     es,dx                   ; load segment registers
        mov     ds,cx                   ; ...
        mov     esi,ebx                 ; get source offset
        mov     edi,eax                 ; get destination offset
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   es:[edi],al             ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          je    short E_DP_strcpy1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   es:1[edi],al            ; - copy it
          add   edi,2                   ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
E_DP_strcpy1:
        pop     eax                     ; restore destination address
        pop     es                      ; restore segment registers
        pop     ds                      ; ...
E_DP_strcpy:

func    macro   name
        dd      _&name&_defs,_&name&_name,_&name
        endm

        public  _Functions

_Functions:
        func    C_strcpy
        func    S_strcpy
        func    Z_strcpy
        func    BD_strcpy
        func    DP_strcpy
        func    C_strcat
        func    FC_strcat
        func    S_strcat
        func    Z_strcat
        func    BD_strcat
        func    DP_strcat
        func    C_strchr
        func    S_strchr
        func    BD_strchr
        dd      0,0,0
_DATA   ends

        end
