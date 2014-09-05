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


beginb  macro   name
_&name&_name:
        db      "&name&",0
public  _&name
_&name:
        db      E_&name - _&name - 1
endm

endb    macro   name
E_&name:
endm

defsb    macro   name
_&name&_defs:
endm

        name    code386

_DATA   segment word public 'DATA'
        assume  CS:_DATA

module_start:
        dw      _Functions - module_start

defsb   C_strcat
        db      "/* edi strcat( edi, esi ) zaps eax,ecx,esi */",0
        db      "#define C_strcat_ret   HW_D( HW_EDI )",0
        db      "#define C_strcat_parms P_EDI_ESI",0
        db      "#define C_strcat_saves HW_NotD_3( HW_EAX, HW_ECX, HW_ESI )",0
        db      0
;
;       EDI strcat( EDI, ESI )
;       EAX,ECX and ESI are modified
;
beginb  C_strcat
        push    es                      ; save es
        push    ds                      ; set es=ds
        pop     es                      ; ...
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_C_strcat1       ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_C_strcat1:
        pop     edi                     ; restore destination address
        pop     es                      ; restore es
endb    C_strcat

defsb   FC_strcat
        db      "/* edi strcat( edi, esi ) zaps eax,ecx,esi */",0
        db      "#define FC_strcat_ret   HW_D( HW_EDI )",0
        db      "#define FC_strcat_parms P_EDI_ESI",0
        db      "#define FC_strcat_saves HW_NotD_3( HW_EAX, HW_ECX, HW_ESI )",0
        db      0
;
;       EDI strcat( EDI, ESI )
;       EAX,ECX and ESI are modified
;
beginb  FC_strcat
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_FC_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_FC_strcat1:
        pop     edi                     ; restore destination address
endb    FC_strcat

defsb   S_strcat
        db      "/* edi strcat( edi, esi ) zaps eax,esi */",0
        db      "#define S_strcat_ret   HW_D( HW_EDI )",0
        db      "#define S_strcat_parms P_EDI_ESI",0
        db      "#define S_strcat_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       EDI strcat( EDI, ESI )
;       EAX and ESI are modified
;
beginb  S_strcat
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
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination string address
endb    S_strcat

defsb   Z_strcat
        db      "/* es:edi strcat( es:edi, ds:esi ) zaps eax,esi */",0
        db      "#define Z_strcat_ret   HW_D_2( HW_ES, HW_EDI )",0
        db      "#define Z_strcat_parms P_ESEDI_DSESI",0
        db      "#define Z_strcat_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       ES:EDI strcat( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
beginb  Z_strcat
        push    edi                     ; save destination address
        push    ecx                     ; save ecx
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        pop     ecx                     ; restore ecx
        dec     edi                     ; point to null character
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          stosb                         ; - append to end of dest
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination string address
endb    Z_strcat

defsb   BD_strcat
        db      "/* es:edi strcat( es:edi, ds:esi ) zaps ecx,eax,esi */",0
        db      "#define BD_strcat_ret   HW_D_2( HW_ES, HW_EDI )",0
        db      "#define BD_strcat_parms P_ESEDI_DSESI",0
        db      "#define BD_strcat_saves HW_NotD_3( HW_ECX, HW_EAX, HW_ESI )",0
        db      0
;
;       ES:EDI strcat( ES:EDI, DS:ESI )
;       EAX,ECX and ESI are modified
;
beginb  BD_strcat
        push    edi                     ; save destination address
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          stosb                         ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_BD_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          stosb                         ; - copy it
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_BD_strcat1:
        pop     edi                     ; restore destination address
endb    BD_strcat

defsb   DP_strcat
        db      "/* dx:eax strcat( dx:eax, cx:ebx ) zaps ecx,eax,esi */",0
        db      "#define DP_strcat_ret   HW_D_2( HW_DX, HW_EAX )",0
        db      "#define DP_strcat_parms P_DXEAX_CXEBX",0
        db      "#define DP_strcat_saves HW_NotD_3( HW_ECX, HW_ESI, HW_EDI )",0
        db      0
;
;       DX:EAX _fstrcat( DX:EAX, CX:EBX )
;       EAX,ECX,EDI and ESI are modified
;
beginb  DP_strcat
        push    ds                      ; save segment registers
        push    es                      ; ...
        push    eax                     ; save destination address
        mov     es,edx                  ; load segment registers
        mov     ds,ecx                  ; ...
        mov     esi,ebx                 ; get source offset
        mov     edi,eax                 ; get destination offset
        sub     ecx,ecx                 ; set length to -1
        dec     ecx                     ; ...
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     edi                     ; point to null character
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          stosb                         ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_DP_strcat1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          stosb                         ; - copy it
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_DP_strcat1:
        pop     eax                     ; restore destination address
        pop     es                      ; restore segment registers
        pop     ds                      ; ...
endb    DP_strcat

;=======================================================================

defsb   C_strchr
        db      "/* esi strchr( esi, dl ) zaps eax,esi */",0
        db      "#define C_strchr_ret   HW_D( HW_ESI )",0
        db      "#define C_strchr_parms P_ESI_DL",0
        db      "#define C_strchr_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       ESI strchr( ESI, DL )
;       EAX and ESI are modified
;
beginb  C_strchr
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          test  al,al                   ; - quit if end of string
          je    short E_C_strchr1       ; - ...
          inc   esi                     ; - increment pointer
          mov   al,[esi]                ; - get next char
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          inc   esi                     ; - increment pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_C_strchr1:
        sub     esi,esi                 ; return NULL
endb    C_strchr

defsb   S_strchr
        db      "/* esi strchr( esi, dl ) zaps eax,esi */",0
        db      "#define S_strchr_ret   HW_D( HW_ESI )",0
        db      "#define S_strchr_parms P_ESI_DL",0
        db      "#define S_strchr_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       ESI strchr( ESI, DL )
;       EAX and ESI are modified
;
beginb  S_strchr
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,dl                   ; - quit if char found
          je    short E_S_strchr        ; - ...
          inc   esi                     ; - increment pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        sub     esi,esi                 ; return NULL
endb    S_strchr

defsb   BD_strchr
        db    "/* dx:esi strchr( ds:esi, cl ) zaps dx,esi,eax */",0
        db    "#define BD_strchr_ret   HW_D_2( HW_DX, HW_ESI )",0
        db    "#define BD_strchr_parms P_DSESI_CL",0
        db    "#define BD_strchr_saves HW_NotD_3( HW_DX, HW_ESI, HW_EAX )",0
        db    0
;
;       DX:ESI strchr( DS:ESI, CL )
;       EAX,EDX and ESI are modified
;
beginb  BD_strchr
        mov     edx,ds                  ; save ds
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          cmp   al,cl                   ; - quit if char found
          je    short E_BD_strchr       ; - ...
          inc   esi                     ; - increment pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        sub     esi,esi                 ; return NULL
        sub     edx,edx                 ; ...
endb    BD_strchr


defsb   DP_strchr
        db      "/* cx:edi strchr( dx:edi, cl ) zaps edi,edx,eax */",0
        db      "#define DP_strchr_ret   HW_D_2( HW_DX, HW_EDI )",0
        db      "#define DP_strchr_parms P_DXEDI_CL",0
        db      "#define DP_strchr_saves HW_NotD_3( HW_EDI, HW_EDX, HW_EAX )",0
        db      0
beginb  DP_strchr
        push    ds
        mov     ds,edx
        _loop                           ; loop
          mov   al,[edi]                ; - get char from src
          cmp   al,cl                   ; - quit if char found
          je    short E_DP_strchr       ; - ...
          inc   edi                     ; - increment pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        sub     edi,edi                 ; return NULL
        sub     edx,edx                 ; ...
        pop     ds
endb    DP_strchr

;=======================================================================

defsb   C_strcpy
        db    "/* edi strcpy( edi, esi ) zaps eax,esi */",0
        db    "#define C_strcpy_ret   HW_D( HW_EDI )",0
        db    "#define C_strcpy_parms P_EDI_ESI",0
        db    "#define C_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db    0
;
;       EDI strcpy( EDI, ESI )
;       EAX and ESI are modified
;
beginb  C_strcpy
        push    edi                     ; save destination address
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   [edi],al                ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_C_strcpy1       ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   1[edi],al               ; - copy it
          add   edi,2                   ; - bump up pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_C_strcpy1:
        pop     edi                     ; restore destination address
endb    C_strcpy

defsb   S_strcpy
        db      "/* edi strcpy( edi, esi ) zaps eax,esi */",0
        db      "#define S_strcpy_ret   HW_D( HW_EDI )",0
        db      "#define S_strcpy_parms P_EDI_ESI",0
        db      "#define S_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       EDI strcpy( EDI, ESI )
;       EAX and ESI are modified
;
beginb  S_strcpy
        push    edi                     ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          mov   [edi],al                ; - store in output buffer
          inc   edi                     ; - increment pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination address
endb    S_strcpy

defsb   Z_strcpy
        db      "/* es:edi strcpy( es:edi, ds:esi ) zaps eax,esi */",0
        db      "#define Z_strcpy_ret   HW_D_2( HW_ES, HW_EDI )",0
        db      "#define Z_strcpy_parms P_ESEDI_DSESI",0
        db      "#define Z_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       ES:EDI strcpy( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
beginb  Z_strcpy
        push    edi                     ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          stosb                         ; - store in output buffer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
        pop     edi                     ; restore destination address
endb    Z_strcpy

defsb   BD_strcpy
        db      "/* es:edi strcpy( es:edi, ds:esi ) zaps eax,esi */",0
        db      "#define BD_strcpy_ret   HW_D_2( HW_ES, HW_EDI )",0
        db      "#define BD_strcpy_parms P_ESEDI_DSESI",0
        db      "#define BD_strcpy_saves HW_NotD_2( HW_EAX, HW_ESI )",0
        db      0
;
;       ES:EDI strcpy( ES:EDI, DS:ESI )
;       EAX and ESI are modified
;
beginb  BD_strcpy
        push    edi                     ; save destination address
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   es:[edi],al             ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_BD_strcpy1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   es:1[edi],al            ; - copy it
          add   edi,2                   ; - bump up pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_BD_strcpy1:
        pop     edi                     ; restore destination address
endb    BD_strcpy

defsb   DP_strcpy
        db      "/* dx:eax strcpy( dx:eax, cx:ebx ) zaps esi,edi */",0
        db      "#define DP_strcpy_ret   HW_D_2( HW_DX, HW_EAX )",0
        db      "#define DP_strcpy_parms P_DXEAX_CXEBX",0
        db      "#define DP_strcpy_saves HW_NotD_2( HW_ESI, HW_EDI )",0
        db      0
;
;       DX:EAX _fstrcpy( DX:EAX, CX:EBX )
;       EDI and ESI are modified
;
beginb  DP_strcpy
        push    ds                      ; save segment registers
        push    es                      ; ...
        push    eax                     ; save destination address
        mov     es,edx                  ; load segment registers
        mov     ds,ecx                  ; ...
        mov     esi,ebx                 ; get source offset
        mov     edi,eax                 ; get destination offset
        _loop                           ; loop
          mov   al,[esi]                ; - get char from src
          mov   es:[edi],al             ; - store in output buffer
          test  al,al                   ; - quit if end of string
          je    short E_DP_strcpy1      ; - ...
          mov   al,1[esi]               ; - get next char
          add   esi,2                   ; - bump up pointer
          mov   es:1[edi],al            ; - copy it
          add   edi,2                   ; - bump up pointer
          test  al,al                   ; - check for end of string
        _until  e                       ; until end of string
E_DP_strcpy1:
        pop     eax                     ; restore destination address
        pop     es                      ; restore segment registers
        pop     ds                      ; ...
endb    DP_strcpy


defsb   C_div
        db      "/* edx:eax  div( eax, ecx ) */",0
        db      "#define C_div_ret   HW_D( HW_EAX )",0
        db      "#define C_div_parms P_EAX_ECX",0
        db      "#define C_div_saves HW_NotD_2( HW_EAX, HW_EDX )",0
        db      0
beginb  C_div
        CDQ
        IDIV    ECX
        MOV     Dword Ptr [ESI],EAX
        MOV     Dword Ptr [ESI+4],EDX
endb    C_div

defsb   S_memset
        db      "/* edi memset( edi, al, ecx ) zaps es,ecx */",0
        db      "#define S_memset_ret   HW_D( HW_EDI )",0
        db      "#define S_memset_parms P_EDI_AL_ECX",0
        db      "#define S_memset_saves HW_NotD( HW_ECX )",0
        db      0
beginb  S_memset
        PUSH    ES
        PUSH    EDI
        PUSH    DS
        POP     ES
        REP     STOSB
        POP     EDI
        POP     ES
endb    S_memset

defsb   FS_memset
        db      "/* edi memset( edi, al, ecx ) zaps es,ecx */",0
        db      "#define FS_memset_ret    HW_D( HW_EDI )",0
        db      "#define FS_memset_parms  P_EDI_AL_ECX",0
        db      "#define FS_memset_saves  HW_NotD( HW_ECX )",0
        db      0
beginb  FS_memset
        PUSH    EDI
        REP     STOSB
        POP     EDI
endb    FS_memset

defsb   C_memset
        db      "/* edi memset( edi, al, ecx ) zaps es,ah,ecx */",0
        db      "#define C_memset_ret    HW_D( HW_EDI )",0
        db      "#define C_memset_parms  P_EDI_AL_ECX",0
        db      "#define C_memset_saves  HW_NotD_2( HW_AH, HW_ECX )",0
        db      0
beginb  C_memset
        PUSH    ES
        PUSH    DS
        POP     ES
        PUSH    EDI
        MOV     AH,AL
        SHR     ECX,1
        REP     STOSW
        ADC     ECX,ECX
        REP     STOSB
        POP     EDI
        POP     ES
endb    C_memset

defsb   FC_memset
        db      "/* edi memset( edi, al, ecx ) zaps es,ah,ecx */",0
        db      "#define FC_memset_ret    HW_D( HW_EDI )",0
        db      "#define FC_memset_parms  P_EDI_AL_ECX",0
        db      "#define FC_memset_saves  HW_NotD_2( HW_AH, HW_ECX )",0
        db      0
beginb  FC_memset
        PUSH    EDI
        MOV     AH,AL
        SHR     ECX,1
        REP     STOSW
        ADC     ECX,ECX
        REP     STOSB
        POP     EDI
endb    FC_memset

defsb   Z_memset
        db      "/* es:edi memset( es:edi, al, ecx ) zaps ecx */",0
        db      "#define Z_memset_ret    HW_D_2( HW_ES, HW_EDI )",0
        db      "#define Z_memset_parms  P_ESEDI_AL_ECX",0
        db      "#define Z_memset_saves  HW_NotD( HW_ECX )",0
        db      0
beginb  Z_memset
        PUSH    EDI
        REP     STOSB
        POP     EDI
endb    Z_memset

defsb   BD_memset
        db      "/* es:edi memset( es:edi, al, ecx ) zaps ah,ecx */",0
        db      "#define BD_memset_ret    HW_D_2( HW_ES, HW_EDI )",0
        db      "#define BD_memset_parms  P_ESEDI_AL_ECX",0
        db      "#define BD_memset_saves  HW_NotD_2( HW_AH, HW_ECX )",0
        db      0
beginb  BD_memset
        PUSH    EDI
        MOV     AH,AL
        SHR     ECX,1
        REP     STOSW
        ADC     ECX,ECX
        REP     STOSB
        POP     EDI
endb    BD_memset

defsb   DP_memset
        db      "/* dx:edi memset( dx:edi, al, ecx ) zaps ah,ecx */",0
        db      "#define DP_memset_ret    HW_D_2( HW_DX, HW_EDI )",0
        db      "#define DP_memset_parms  P_DXEDI_AL_ECX",0
        db      "#define DP_memset_saves  HW_NotD_2( HW_AH, HW_ECX )",0
        db      0
beginb  DP_memset
        PUSH    ES
        PUSH    EDI
        MOV     ES,EDX
        MOV     AH,AL
        SHR     ECX,1
        REP     STOSW
        ADC     ECX,ECX
        REP     STOSB
        POP     EDI
        POP     ES
endb    DP_memset

defsb   C_stosw
        db      "/* edi stosw( es:edi, ax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be even */",0
        db      "#define C_stosw_ret    HW_D( HW_EDI )",0
        db      "#define C_stosw_parms  P_DXEDI_AX_ECX",0
        db      "#define C_stosw_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  C_stosw
        PUSH    ES
        MOV     ES,EDX
        REP     STOSW
        POP     ES
endb    C_stosw

defsb   F_stosw
        db      "/* edi stosw( edi, ax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be even */",0
        db      "#define F_stosw_ret    HW_D( HW_EDI )",0
        db      "#define F_stosw_parms  P_EDI_AX_ECX",0
        db      "#define F_stosw_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  F_stosw
        REP     STOSW
endb    F_stosw

defsb   BD_stosw
        db      "/* es:edi stosw( es:edi, ax, ecx ) zaps edi,ecx */",0
        db      "#define BD_stosw_ret   HW_D( HW_EDI )",0
        db      "#define BD_stosw_parms P_ESEDI_AX_ECX",0
        db      "#define BD_stosw_saves HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  BD_stosw
        REP     STOSW
endb    BD_stosw

defsb   C_stosd
        db      "/* edi stosd( es:edi, eax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be multiple of 4 */",0
        db      "#define C_stosd_ret    HW_D( HW_EDI )",0
        db      "#define C_stosd_parms  P_DXEDI_EAX_ECX",0
        db      "#define C_stosd_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  C_stosd
        PUSH    ES
        MOV     ES,EDX
        REP     STOSD
        POP     ES
endb    C_stosd

defsb   F_stosd
        db      "/* edi stosd( edi, eax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be multiple of 4 */",0
        db      "#define F_stosd_ret    HW_D( HW_EDI )",0
        db      "#define F_stosd_parms  P_EDI_EAX_ECX",0
        db      "#define F_stosd_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  F_stosd
        REP     STOSD
endb    F_stosd

defsb   BD_stosd
        db      "/* es:edi stosd( es:edi, eax, ecx ) zaps edi,ecx */",0
        db      "#define BD_stosd_ret   HW_D( HW_EDI )",0
        db      "#define BD_stosd_parms P_ESEDI_EAX_ECX",0
        db      "#define BD_stosd_saves HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  BD_stosd
        REP     STOSD
endb    BD_stosd

defsb   C_stoswb
        db      "/* edi stoswb( es:edi, ax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be odd */",0
        db      "#define C_stoswb_ret    HW_D( HW_EDI )",0
        db      "#define C_stoswb_parms  P_DXEDI_AX_ECX",0
        db      "#define C_stoswb_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  C_stoswb
        PUSH    ES
        MOV     ES,EDX
        REP     STOSW
        STOSB
        POP     ES
endb    C_stoswb

defsb   F_stoswb
        db      "/* edi stoswb( edi, ax, ecx ) zaps edi,ecx */",0
        db      "/* used when value of ECX is known to be odd */",0
        db      "#define F_stoswb_ret    HW_D( HW_EDI )",0
        db      "#define F_stoswb_parms  P_EDI_AX_ECX",0
        db      "#define F_stoswb_saves  HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  F_stoswb
        REP     STOSW
        STOSB
endb    F_stoswb

defsb   BD_stoswb
        db      "/* es:edi stoswb( es:edi, ax, ecx ) zaps edi,ecx */",0
        db      "#define BD_stoswb_ret   HW_D( HW_EDI )",0
        db      "#define BD_stoswb_parms P_ESEDI_AX_ECX",0
        db      "#define BD_stoswb_saves HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  BD_stoswb
        REP     STOSW
        STOSB
endb    BD_stoswb

defsb   S_memcpy
        db      "/* edi memcpy( edi, esi, ecx ) zaps es,esi,ecx */",0
        db      "#define S_memcpy_ret    HW_D( HW_EDI )",0
        db      "#define S_memcpy_parms  P_EDI_ESI_ECX",0
        db      "#define S_memcpy_saves  HW_NotD_2( HW_ESI, HW_ECX )",0
        db      0
beginb  S_memcpy
        PUSH    ES
        PUSH    EDI
        PUSH    DS
        POP     ES
        REP     MOVSB
        POP     EDI
        POP     ES
endb    S_memcpy

defsb   FS_memcpy
        db      "/* edi memcpy( edi, esi, ecx ) zaps es,esi,ecx */",0
        db      "#define FS_memcpy_ret    HW_D( HW_EDI )",0
        db      "#define FS_memcpy_parms  P_EDI_ESI_ECX",0
        db      "#define FS_memcpy_saves  HW_NotD_2( HW_ESI, HW_ECX )",0
        db      0
beginb  FS_memcpy
        PUSH    EDI
        REP     MOVSB
        POP     EDI
endb    FS_memcpy

defsb   C_memcpy
        db      "/* edi memcpy( edi, esi, ecx ) zaps eax,es,esi,ecx */",0
        db      "#define C_memcpy_ret    HW_D( HW_EDI )",0
        db      "#define C_memcpy_parms  P_EDI_ESI_ECX",0
        db      "#define C_memcpy_saves  HW_NotD_3( HW_EAX, HW_ESI, HW_ECX )",0
        db      0
beginb  C_memcpy
        PUSH    ES
        MOV     EAX,DS
        MOV     ES,EAX
        PUSH    EDI
        MOV     EAX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     CL,AL
        AND     CL,3
        REP     MOVSB
        POP     EDI
        POP     ES
endb    C_memcpy

defsb   FC_memcpy
        db      "/* edi memcpy( edi, esi, ecx ) zaps eax,es,esi,ecx */",0
        db      "#define FC_memcpy_ret    HW_D( HW_EDI )",0
        db      "#define FC_memcpy_parms  P_EDI_ESI_ECX",0
        db      "#define FC_memcpy_saves  HW_NotD_3( HW_EAX, HW_ESI, HW_ECX )",0
        db      0
beginb  FC_memcpy
        PUSH    EDI
        MOV     EAX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     CL,AL
        AND     CL,3
        REP     MOVSB
        POP     EDI
endb    FC_memcpy

defsb   Z_memcpy
        db      "/* es:edi memcpy( es:edi, ds:esi, ecx ) zaps ecx,esi */",0
        db      "#define Z_memcpy_ret    HW_D_2( HW_ES, HW_EDI )",0
        db      "#define Z_memcpy_parms  P_ESEDI_DSESI_ECX",0
        db      "#define Z_memcpy_saves  HW_NotD_2( HW_ESI, HW_ECX )",0
        db      0
beginb  Z_memcpy
        PUSH    EDI
        REP     MOVSB
        POP     EDI
endb    Z_memcpy

defsb   BD_memcpy
        db      "/* es:edi memcpy( es:edi, ds:esi, ecx ) zaps ecx,esi */",0
        db      "#define BD_memcpy_ret   HW_D_2( HW_ES, HW_EDI )",0
        db      "#define BD_memcpy_parms P_ESEDI_DSESI_ECX",0
        db      "#define BD_memcpy_saves  C_memcpy_saves",0
        db      0
beginb  BD_memcpy
        PUSH    EDI
        MOV     EAX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     CL,AL
        AND     CL,3
        REP     MOVSB
        POP     EDI
endb    BD_memcpy

defsb   DP_memcpy
        db      "/* dx:eax memcpy( dx:edi, cx:esi, eax ) zaps ecx,esi,edi,eax */",0
        db      "#define DP_memcpy_ret   HW_D_2( HW_DX, HW_EAX )",0
        db      "#define DP_memcpy_parms P_DXEDI_CXESI_EAX",0
        db      "#define DP_memcpy_saves HW_NotD_4( HW_ESI, HW_ECX, HW_EDI, HW_EAX )",0
        db      0
beginb  DP_memcpy
        PUSH    DS
        PUSH    ES
        PUSH    EDI
        XCHG    EAX,ECX
        MOV     DS,EAX
        MOV     ES,EDX
        MOV     EAX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     CL,AL
        AND     CL,3
        REP     MOVSB
        POP     EAX
        POP     ES
        POP     DS
endb    DP_memcpy

defsb   C_memcmp
        db      "/* eax memcmp( esi, edi, ecx ) zaps eax,es,esi,edi,ecx */",0
        db      "#define C_memcmp_ret    HW_D( HW_EAX )",0
        db      "#define C_memcmp_parms  P_ESI_EDI_ECX",0
        db      "#define C_memcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  C_memcmp
        PUSH    ES
        MOV     EAX,DS
        MOV     ES,EAX
        XOR     EAX,EAX
        REPZ    CMPSB
        JZ      E_C_memcmp1
        SBB     EAX,EAX
        SBB     EAX,-1
E_C_memcmp1:
        POP     ES
endb    C_memcmp

defsb   FC_memcmp
        db      "/* eax memcmp( esi, edi, ecx ) zaps eax,es,esi,edi,ecx */",0
        db      "#define FC_memcmp_ret    HW_D( HW_EAX )",0
        db      "#define FC_memcmp_parms  P_ESI_EDI_ECX",0
        db      "#define FC_memcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  FC_memcmp
        XOR     EAX,EAX
        REPZ    CMPSB
        JZ      E_FC_memcmp
        SBB     EAX,EAX
        SBB     EAX,-1
endb    FC_memcmp

defsb   BD_memcmp
        db      "/* ecx memcmp( ds:esi, es:edi, ecx ) zaps esi,edi,ecx */",0
        db      "#define BD_memcmp_ret   HW_D( HW_ECX )",0
        db      "#define BD_memcmp_parms P_DSESI_ESEDI_ECX",0
        db      "#define BD_memcmp_saves HW_NotD_3( HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  BD_memcmp
        OR      ECX,ECX
        REPZ    CMPSB
        JZ      E_BD_memcmp
        SBB     ECX,ECX
        SBB     ECX,-1
endb    BD_memcmp

defsb   DP_memcmp
        db      "/* ecx memcmp( dx:esi, cx:edi, eax ) zaps esi,edi,ecx,eax */",0
        db      "#define DP_memcmp_ret   HW_D( HW_ECX )",0
        db      "#define DP_memcmp_parms P_DXESI_CXEDI_EAX",0
        db      "#define DP_memcmp_saves HW_NotD_4( HW_ESI, HW_EDI, HW_ECX, HW_EAX )",0
        db      0
beginb  DP_memcmp
        PUSH    DS
        PUSH    ES
        XCHG    EAX,ECX
        MOV     DS,EDX
        MOV     ES,EAX
        OR      ECX,ECX
        REPZ    CMPSB
        JZ      E_DP_memcmp1
        SBB     ECX,ECX
        SBB     ECX,-1
E_DP_memcmp1:
        POP     ES
        POP     DS
endb    DP_memcmp

defsb   C_memchr
        db      "/* edi memchr( edi, al, ecx ) zaps edx,es,edi,ecx */",0
        db      "#define C_memchr_ret    HW_D( HW_EDI )",0
        db      "#define C_memchr_parms  P_EDI_AL_ECX",0
        db      "#define C_memchr_saves  HW_NotD_3( HW_EDX, HW_EDI, HW_ECX )",0
        db      0
beginb  C_memchr
        PUSH    ES
        JECXZ   E_C_memchr1
        MOV     EDX,DS
        MOV     ES,EDX
        REPNZ   SCASB
        JNZ     E_C_memchr1
        DEC     EDI
        db      66h,0A9h
E_C_memchr1:
        MOV     EDI,ECX
        POP     ES
endb    C_memchr

defsb   FC_memchr
        db      "/* edi memchr( edi, al, ecx ) zaps edx,es,edi,ecx */",0
        db      "#define FC_memchr_ret    HW_D( HW_EDI )",0
        db      "#define FC_memchr_parms  P_EDI_AL_ECX",0
        db      "#define FC_memchr_saves  HW_NotD_3( HW_EDX, HW_EDI, HW_ECX )",0
        db      0
beginb  FC_memchr
        JECXZ   E_FC_memchr1
        REPNZ   SCASB
        JNZ     E_FC_memchr1
        DEC     EDI
        db      66h,0A9h
E_FC_memchr1:
        MOV     EDI,ECX
endb    FC_memchr

defsb   BD_memchr
        db      "/* cx:edi memchr( es:edi, al, ecx ) zaps edi,ecx */",0
        db      "#define BD_memchr_ret   HW_D_2( HW_EDI, HW_CX )",0
        db      "#define BD_memchr_parms P_ESEDI_AL_ECX",0
        db      "#define BD_memchr_saves HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  BD_memchr
        JECXZ   E_BD_memchr1
        REPNZ   SCASB
        JNZ     E_BD_memchr1
        DEC     EDI
        MOV     ECX,ES
        db      66h,0A9h
E_BD_memchr1:
        MOV     EDI,ECX
endb    BD_memchr

defsb   DP_memchr
        db      "/* cx:edi memchr( dx:edi, al, ecx ) zaps edi,ecx */",0
        db      "#define DP_memchr_ret   HW_D_2( HW_CX, HW_EDI )",0
        db      "#define DP_memchr_parms P_DXEDI_AL_ECX",0
        db      "#define DP_memchr_saves HW_NotD_2( HW_EDI, HW_ECX )",0
        db      0
beginb  DP_memchr
        PUSH    ES
        JECXZ   E_DP_memchr1
        MOV     ES,EDX
        REPNZ   SCASB
        JNZ     E_DP_memchr1
        DEC     EDI
        MOV     ECX,ES
        db      66h,0A9h
E_DP_memchr1:
        MOV     EDI,ECX
        POP     ES
endb    DP_memchr

defsb   S_strcmp
        db      "/* eax strcmp( esi, edi ) zaps eax,esi,edi */",0
        db      "#define S_strcmp_ret    HW_D( HW_EAX )",0
        db      "#define S_strcmp_parms  P_ESI_EDI",0
        db      "#define S_strcmp_saves  HW_NotD_3( HW_EAX, HW_ESI, HW_EDI )",0
        db      0
beginb  S_strcmp
        _loop
          LODSB
          MOV   AH,Byte Ptr [EDI]
          SUB   AL,AH
          JNZ   E_S_strcmp1
          INC   EDI
          CMP   AL,AH
        _until  e
E_S_strcmp1:
        SBB     AH,AH
        CWDE
endb    S_strcmp

defsb   C_strcmp
        db      "/* eax strcmp( esi, edi ) zaps eax,es,esi,edi,ecx */",0
        db      "#define C_strcmp_ret    HW_D( HW_EAX )",0
        db      "#define C_strcmp_parms  P_ESI_EDI",0
        db      "#define C_strcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  C_strcmp
        PUSH    ES
        MOV     EAX,DS
        MOV     ES,EAX
        CMPSB
        JNZ     E_C_strcmp1
        DEC     EDI
        DEC     ESI
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        SUB     EDI,ECX
        REPZ    CMPSB
        JZ      E_C_strcmp2
E_C_strcmp1:
        SBB     EAX,EAX
        SBB     EAX,-1
E_C_strcmp2:
        POP     ES
endb    C_strcmp

defsb   F_strcmp
        db      "/* eax strcmp( esi, edi ) zaps eax,esi,edi,ecx */",0
        db      "#define F_strcmp_ret    HW_D( HW_EAX )",0
        db      "#define F_strcmp_parms  P_ESI_EDI",0
        db      "#define F_strcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  F_strcmp
        CMPSB
        JNZ     E_F_strcmp1
        DEC     EDI
        DEC     ESI
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        SUB     EDI,ECX
        REPZ    CMPSB
        JZ      E_F_strcmp
E_F_strcmp1:
        SBB     EAX,EAX
        SBB     EAX,-1
endb    F_strcmp

defsb   Z_strcmp
        db      "/* eax strcmp( ds:esi, es:edi ) zaps eax,esi,edi */",0
        db      "#define Z_strcmp_ret    HW_D( HW_EAX )",0
        db      "#define Z_strcmp_parms  P_DSESI_ESEDI",0
        db      "#define Z_strcmp_saves  HW_NotD_3( HW_EAX, HW_ESI, HW_EDI )",0
        db      0
beginb  Z_strcmp
        _loop
          LODSB
          MOV   AH,Byte Ptr ES:[EDI]
          SUB   AL,AH
          JNZ   E_Z_strcmp1
          INC   EDI
          CMP   AL,AH
        _until  e
E_Z_strcmp1:
        SBB     AH,AH
        CWDE
endb    Z_strcmp

defsb   BD_strcmp
        db      "/* ecx strcmp( ds:esi, es:edi ) zaps eax,esi,edi,ecx */",0
        db      "#define BD_strcmp_ret   HW_D( HW_ECX )",0
        db      "#define BD_strcmp_parms P_DSESI_ESEDI",0
        db      "#define BD_strcmp_saves HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  BD_strcmp
        CMPSB
        JNZ     E_BD_strcmp1
        DEC     EDI
        DEC     ESI
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        SUB     EDI,ECX
        REPZ    CMPSB
        JZ      E_BD_strcmp
E_BD_strcmp1:
        SBB     ECX,ECX
        SBB     ECX,-1
endb    BD_strcmp

defsb   DP_strcmp
        db      "/* ecx strcmp( si:eax, cx:edi ) zaps eax,esi,edi,ecx */",0
        db      "#define DP_strcmp_ret   HW_D( HW_ECX )",0
        db      "#define DP_strcmp_parms P_SIEAX_CXEDI",0
        db      "#define DP_strcmp_saves HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )",0
        db      0
beginb  DP_strcmp
        PUSH    DS
        PUSH    ES
        XCHG    EAX,ESI
        MOV     DS,EAX
        MOV     ES,ECX
        CMPSB
        JNZ     E_DP_strcmp1
        DEC     EDI
        DEC     ESI
        XOR     EAX,EAX
        MOV     ECX,EAX
        DEC     ECX
        REPNZ   SCASB
        NOT     ECX
        SUB     EDI,ECX
        REPZ    CMPSB
        JZ      E_DP_strcmp2
E_DP_strcmp1:
        SBB     ECX,ECX
        SBB     ECX,-1
E_DP_strcmp2:
        POP     ES
        POP     DS
endb    DP_strcmp

defsb   C_strlen
        db      "/* ecx strlen( edi ) zaps eax,es,ecx,edi */",0
        db      "#define C_strlen_ret    HW_D( HW_ECX )",0
        db      "#define C_strlen_parms  P_EDI",0
        db      "#define C_strlen_saves  HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )",0
        db      0
beginb  C_strlen
        PUSH    ES
        MOV     EAX,DS
        MOV     ES,EAX
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        DEC     ECX
        POP     ES
endb    C_strlen

defsb   FC_strlen
        db      "/* ecx strlen( edi ) zaps eax,es,ecx,edi */",0
        db      "#define FC_strlen_ret    HW_D( HW_ECX )",0
        db      "#define FC_strlen_parms  P_EDI",0
        db      "#define FC_strlen_saves  HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )",0
        db      0
beginb  FC_strlen
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        DEC     ECX
endb    FC_strlen

defsb   BD_strlen
        db      "/* ecx strlen( es:edi ) zaps eax,ecx,edi */",0
        db      "#define BD_strlen_ret   HW_D( HW_ECX )",0
        db      "#define BD_strlen_parms P_ESEDI",0
        db      "#define BD_strlen_saves HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )",0
        db      0
beginb  BD_strlen
        SUB     ECX,ECX
        DEC     ECX
        XOR     EAX,EAX
        REPNZ   SCASB
        NOT     ECX
        DEC     ECX
endb    BD_strlen

defsb   DP_strlen
        db      "/* ecx strlen( cx:edi ) zaps eax,ecx,edi */",0
        db      "#define DP_strlen_ret   HW_D( HW_ECX )",0
        db      "#define DP_strlen_parms P_CXEDI",0
        db      "#define DP_strlen_saves HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )",0
        db      0
beginb  DP_strlen
        PUSH    ES
        MOV     ES,ECX
        XOR     EAX,EAX
        MOV     ECX,EAX
        DEC     ECX
        REPNZ   SCASB
        NOT     ECX
        DEC     ECX
        POP     ES
endb    DP_strlen

defsb   C_abs
        db      "#define C_abs_ret   HW_D( HW_EAX )",0
        db      "#define C_abs_parms P_EAX",0
        db      "#define C_abs_saves HW_NotD_2( HW_EAX, HW_EDX )",0
        db      0
beginb  C_abs
        CDQ
        XOR     EAX,EDX
        SUB     EAX,EDX
endb    C_abs

defsb   C_labs
        db      "#define C_labs_ret   HW_D( HW_EAX )",0
        db      "#define C_labs_parms P_EAX",0
        db      "#define C_labs_saves HW_NotD_2( HW_EAX, HW_EDX )",0
        db      0
beginb  C_labs
        CDQ
        XOR     EAX,EDX
        SUB     EAX,EDX
endb    C_labs

defsb   C_inp
        db      "#define C_inp_ret    HW_D( HW_EAX )",0
        db      "#define C_inp_parms  P_EDX",0
        db      "#define C_inp_saves  HW_NotD( HW_EAX )",0
        db      0
beginb  C_inp
        SUB     EAX,EAX
        IN      AL,DX
endb    C_inp

defsb   C_inpw
        db      "#define C_inpw_ret   HW_D( HW_EAX )",0
        db      "#define C_inpw_parms P_EDX",0
        db      "#define C_inpw_saves HW_NotD( HW_EAX )",0
        db      0
beginb  C_inpw
        SUB     EAX,EAX
        IN      AX,DX
endb    C_inpw

defsb   C_inpd
        db      "#define C_inpd_ret   HW_D( HW_EAX )",0
        db      "#define C_inpd_parms P_EDX",0
        db      "#define C_inpd_saves HW_NotD( HW_EAX )",0
        db      0
beginb  C_inpd
        IN      EAX,DX
endb    C_inpd

defsb   C_outp
        db      "#define C_outp_ret   HW_D( HW_EAX )",0
        db      "#define C_outp_parms P_EDX_AL",0
        db      "#define C_outp_saves HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_outp
        OUT     DX,AL
endb    C_outp

defsb   C_outpw
        db      "#define C_outpw_ret    HW_D( HW_EAX )",0
        db      "#define C_outpw_parms  P_EDX_EAX",0
        db      "#define C_outpw_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_outpw
        OUT     DX,AX
endb    C_outpw

defsb   C_outpd
        db      "#define C_outpd_ret   HW_D( HW_EAX )",0
        db      "#define C_outpd_parms P_EDX_EAX",0
        db      "#define C_outpd_saves HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_outpd
        OUT     DX,EAX
endb    C_outpd

defsb   C_movedata
        db      "#define C_movedata_ret   HW_D( HW_EMPTY )",0
        db      "#define C_movedata_parms P_EAX_ESI_EDX_EDI_ECX",0
        db      "#define C_movedata_saves HW_NotD_4( HW_EAX, HW_ECX, HW_ESI, HW_EDI )",0
        db      0
beginb  C_movedata
        PUSH    DS
        PUSH    ES
        MOV     DS,EAX
        MOV     ES,EDX
        MOV     EAX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     CL,AL
        AND     CL,3
        REP     MOVSB
        POP     ES
        POP     DS
endb    C_movedata

defsb   C_enable
        db      "#define C_enable_ret   HW_D( HW_EMPTY )",0
        db      "#define C_enable_parms P_EAX",0
        db      "#define C_enable_saves HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_enable
        STI
        CLD
endb    C_enable

defsb   C_disable
        db      "#define C_disable_ret    HW_D( HW_EMPTY )",0
        db      "#define C_disable_parms  P_EAX",0
        db      "#define C_disable_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_disable
        CLI
endb    C_disable

defsb   C_rotl
        db      "#define C_rotl_ret   HW_D( HW_EAX )",0
        db      "#define C_rotl_parms P_EAX_CL",0
        db      "#define C_rotl_saves HW_NotD( HW_EAX )",0
        db      0
beginb  C_rotl
        ROL     EAX,CL
endb    C_rotl

defsb   C_rotr
        db      "#define C_rotr_ret   HW_D( HW_EAX )",0
        db      "#define C_rotr_parms P_EAX_CL",0
        db      "#define C_rotr_saves HW_NotD( HW_EAX )",0
        db      0
beginb  C_rotr
        ROR     EAX,CL
endb    C_rotr

defsb   _8087_fabs
        db      "#define C_8087_fabs_ret    HW_D( HW_FLTS )",0
        db      "#define C_8087_fabs_parms  P_8087",0
        db      "#define C_8087_fabs_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  _8087_fabs
        FABS
endb    _8087_fabs

defsb   C_fabs
        db      "#define C_fabs_ret   HW_D_2( HW_EDX, HW_EAX )",0
        db      "#define C_fabs_parms P_EDXEAX",0
        db      "#define C_fabs_saves HW_NotD( HW_EDX )",0
        db      0
beginb  C_fabs
        SHL     EDX,1
        SHR     EDX,1
endb    C_fabs

defsb   C_min
        db      "#define C_min_ret   HW_D( HW_EAX )",0
        db      "#define C_min_parms P_EAX_EDX",0
        db      "#define C_min_saves HW_NotD_2( HW_ECX, HW_EAX )",0
        db      0
beginb  C_min
        SUB     EAX,EDX
        SBB     ECX,ECX
        AND     EAX,ECX
        ADD     EAX,EDX
endb    C_min

defsb   C_max
        db      "#define C_max_ret   HW_D( HW_EAX )",0
        db      "#define C_max_parms P_EAX_EDX",0
        db      "#define C_max_saves HW_NotD_2( HW_ECX, HW_EAX )",0
        db      0
beginb  C_max
        SUB     EAX,EDX
        CMC
        SBB     ECX,ECX
        AND     EAX,ECX
        ADD     EAX,EDX
endb    C_max



func    macro   name
        dw      _&name&_defs - module_start
        dw      _&name&_name - module_start
        dw      _&name - module_start
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
        func    DP_strchr

        func    C_div
        func    S_memset
        func    FS_memset
        func    C_memset
        func    FC_memset
        func    Z_memset
        func    BD_memset
        func    DP_memset
        func    C_stosw
        func    F_stosw
        func    BD_stosw
        func    C_stosd
        func    F_stosd
        func    BD_stosd
        func    C_stoswb
        func    F_stoswb
        func    BD_stoswb
        func    S_memcpy
        func    FS_memcpy
        func    C_memcpy
        func    FC_memcpy
        func    Z_memcpy
        func    BD_memcpy
        func    DP_memcpy
        func    C_memcmp
        func    FC_memcmp
        func    BD_memcmp
        func    DP_memcmp
        func    C_memchr
        func    FC_memchr
        func    BD_memchr
        func    DP_memchr
        func    S_strcmp
        func    C_strcmp
        func    F_strcmp
        func    Z_strcmp
        func    BD_strcmp
        func    DP_strcmp
        func    C_strlen
        func    FC_strlen
        func    BD_strlen
        func    DP_strlen
        func    C_abs
        func    C_labs
        func    C_inp
        func    C_inpw
        func    C_inpd
        func    C_outp
        func    C_outpw
        func    C_outpd
        func    C_movedata
        func    C_enable
        func    C_disable
        func    C_rotl
        func    C_rotr
        func    _8087_fabs
        func    C_fabs
        func    C_min
        func    C_max
        dw      0,0,0
_DATA   ends

        end
