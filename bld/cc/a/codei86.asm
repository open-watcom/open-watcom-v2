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

.8086

; CODEi86:     Pragma code bursts for built-in compiler pragmas
;
; - these routines are easier to code in assembler than in C

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

        name    codei86

.286p

_DATA   segment word public 'DATA'
        assume  CS:_DATA

module_start:
        dw      _Functions - module_start

strcat_body     macro
        mov     cx,-1                   ; set length to -1
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     di                      ; point to null character
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          mov   [di],al                 ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          mov   al,1[si]                ; - get next char
          add   si,2                    ; - bump up pointer
          mov   1[di],al                ; - copy it
          add   di,2                    ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        endm

BD_strcat_body  macro
        mov     cx,-1                   ; set length to -1
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        dec     di                      ; point to null character
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          mov   al,1[si]                ; - get next char
          add   si,2                    ; - bump up pointer
          stosb                         ; - copy it
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        endm

defsb   C_strcat
        db      "/* di strcat( di, si ) zaps ax,es,cx,si */",0
        db      "/* di strcat( di, si ) zaps ax,es,cx,si */",0
        db      "#define C_strcat_ret   HW_D( HW_DI )",0
        db      "#define C_strcat_parms P_DI_SI",0
        db      "#define C_strcat_saves HW_NotD_4( HW_AX, HW_ES, HW_CX, HW_SI )",0
        db      0

;       DI strcat( DI, SI )
;       ES,CX,AX and SI are modified
;
beginb  C_strcat
        push    ds                      ; set es=ds
        pop     es                      ; ...
        push    di                      ; save destination address
        strcat_body                     ; concatenate strings
        pop     di                      ; restore destination address
endb    C_strcat

defsb   S_strcat
        db      "/* di strcat( di, si ) zaps ax,es,si */",0
        db      "#define S_strcat_ret   HW_D( HW_DI )",0
        db      "#define S_strcat_parms P_DI_SI",0
        db      "#define S_strcat_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       DI strcat( DI, SI )
;       AX and SI are modified
;
beginb  S_strcat
        push    di                      ; save destination address
        dec     di                      ; back up one to start
        _loop                           ; loop (find end of first string)
          inc   di                      ; - point to next byte
          cmp   byte ptr [di],0         ; - check for null character
        _until  e                       ; until end of first string
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          mov   [di],al                 ; - append to end of dest
          inc   di                      ; - point to next byte
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination string address
endb    S_strcat

defsb   ZF_strcat
        db      "/* es:di strcat( es:di, ds:si ) zaps ax,si */",0
        db      "#define ZF_strcat_ret    HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZF_strcat_parms  P_ESDI_DSSI",0
        db      "#define ZF_strcat_saves  HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcat( ES:DI, DS:SI )
;       AX and SI are modified
;
beginb  ZF_strcat
        push    di                      ; save destination address
        push    cx                      ; save cx
        mov     cx,-1                   ; set length to -1
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        pop     cx                      ; restore cx
        dec     di                      ; point to null character
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          stosb                         ; - append to end of dest
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination string address
endb    ZF_strcat

defsb   ZP_strcat
        db      "/* es:di strcat( es:di, si:ax ) zaps ax,si */",0
        db      "#define ZP_strcat_ret    HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZP_strcat_parms  P_ESDI_SIAX",0
        db      "#define ZP_strcat_saves  HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcat( ES:DI, SI:AX )
;       AX and SI are modified
;
beginb  ZP_strcat
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        push    cx                      ; save cx
        mov     cx,-1                   ; set length to -1
        xor     al,al                   ; scan for null character
        repnz   scasb                   ; ...
        pop     cx                      ; restore cx
        dec     di                      ; point to null character
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          stosb                         ; - append to end of dest
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination string address
        pop     ds                      ; restore ds
endb    ZP_strcat

defsb   DF_strcat
        db      "/* es:di strcat( es:di, ds:si ) zaps cx,ax,si */",0
        db      "#define DF_strcat_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define DF_strcat_parms P_ESDI_DSSI",0
        db      "#define DF_strcat_saves HW_NotD_3( HW_CX, HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcat( ES:DI, DS:SI )
;       AX,CX and SI are modified
;
beginb  DF_strcat
        push    di                      ; save destination address
        BD_strcat_body
        pop     di                      ; restore destination address
endb    DF_strcat

defsb   DP_strcat
        db      "/* es:di strcat( es:di, si:ax ) zaps cx,ax,si */",0
        db      "#define DP_strcat_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define DP_strcat_parms P_ESDI_SIAX",0
        db      "#define DP_strcat_saves HW_NotD_3( HW_CX, HW_AX, HW_SI )",0
        db      0
;
;       ES:DI _fstrcat( ES:DI, SI:AX )
;       AX,CX and SI are modified
;
beginb  DP_strcat
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        BD_strcat_body
        pop     di                      ; restore destination string address
        pop     ds                      ; restore ds
endb    DP_strcat

;=======================================================================

defsb   C_strchr
        db      "/* si strchr( si, dl ) zaps ax,si */",0
        db      "#define C_strchr_ret   HW_D( HW_SI )",0
        db      "#define C_strchr_parms P_SI_DL",0
        db      "#define C_strchr_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       SI strchr( SI, DL )
;       AX and SI are modified
;
beginb  C_strchr
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          inc   si                      ; - increment pointer
          mov   al,[si]                 ; - get next char
          cmp   al,dl                   ; - quit if char found
          je    short E_C_strchr        ; - ...
          inc   si                      ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        sub     si,si                   ; return NULL
endb    C_strchr

defsb   DF_strchr
        db      "/* dx:si strchr( ds:si, cl ) zaps ds,si,ax */",0
        db      "#define DF_strchr_ret  HW_D_2( HW_DX, HW_SI )",0
        db      "#define DF_strchr_parms P_DSSI_CL",0
        db      "#define DF_strchr_saves HW_NotD_3( HW_DX, HW_SI, HW_AX )",0
        db      0
;
;       DX:SI strchr( DS:SI, CL )
;       AX,DX and SI are modified
;
beginb  DF_strchr
        mov     dx,ds                   ; save ds
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          cmp   al,cl                   ; - quit if char found
          je    short E_DF_strchr       ; - ...
          inc   si                      ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        sub     si,si                   ; return NULL
        sub     dx,dx                   ; ...
endb    DF_strchr

defsb   DP_strchr
        db      "/* dx:si strchr( dx:si, cl ) zaps dx,si,ax */",0
        db      "#define DP_strchr_ret  HW_D_2( HW_DX, HW_SI )",0
        db      "#define DP_strchr_parms P_DXSI_CL",0
        db      "#define DP_strchr_saves HW_NotD_3( HW_DX, HW_SI, HW_AX )",0
        db      0
;
;       DX:SI strchr( DX:SI, CL )
;       AX,DX and SI are modified
;
beginb  DP_strchr
        push    ds                      ; save ds
        mov     ds,dx                   ; set ds
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          cmp   al,cl                   ; - quit if char found
          je    short _DP_strchr9       ; - ...
          inc   si                      ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        sub     si,si                   ; return NULL
        sub     dx,dx                   ; ...
_DP_strchr9:
        pop     ds                      ; restore ds
endb    DP_strchr

;=======================================================================

defsb   C_strcpy
        db      "/* di strcpy( di, si ) zaps ax,si */",0
        db      "#define C_strcpy_ret   HW_D( HW_DI )",0
        db      "#define C_strcpy_parms P_DI_SI",0
        db      "#define C_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       DI strcpy( DI, SI )
;       AX and SI are modified
;
beginb  C_strcpy
        push    di                      ; save destination address
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          mov   [di],al                 ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          mov   al,1[si]                ; - get next char
          add   si,2                    ; - bump up pointer
          mov   1[di],al                ; - copy it
          add   di,2                    ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
endb    C_strcpy

defsb   S_strcpy
        db      "/* di strcpy( di, si ) zaps ax,si */",0
        db      "#define S_strcpy_ret   HW_D( HW_DI )",0
        db      "#define S_strcpy_parms P_DI_SI",0
        db      "#define S_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       DI strcpy( DI, SI )
;       AX and SI are modified
;
beginb  S_strcpy
        push    di                      ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          mov   [di],al                 ; - store in output buffer
          inc   di                      ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
endb    S_strcpy

defsb   ZF_strcpy
        db      "/* es:di strcpy( es:di, ds:si ) zaps ax,si */",0
        db      "#define ZF_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZF_strcpy_parms P_ESDI_DSSI",0
        db      "#define ZF_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcpy( ES:DI, DS:SI )
;       AX and SI are modified
;
beginb  ZF_strcpy
        push    di                      ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
endb    ZF_strcpy

defsb   ZP_strcpy
        db      "/* es:di strcpy( es:di, si:ax ) zaps ax,si */",0
        db      "#define ZP_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZP_strcpy_parms P_ESDI_SIAX",0
        db      "#define ZP_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcpy( ES:DI, SI:AX )
;       AX and SI are modified
;
beginb  ZP_strcpy
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        _loop                           ; loop
          lodsb                         ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
        pop     ds                      ; restore ds
endb    ZP_strcpy

defsb   DF_strcpy
        db      "/* es:di strcpy( es:di, ds:si ) zaps ax,si */",0
        db      "#define DF_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define DF_strcpy_parms P_ESDI_DSSI",0
        db      "#define DF_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI strcpy( ES:DI, DS:SI )
;       AX and SI are modified
;
beginb  DF_strcpy
        push    di                      ; save destination address
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          mov   es:[di],al              ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          mov   al,1[si]                ; - get next char
          add   si,2                    ; - bump up pointer
          mov   es:1[di],al             ; - copy it
          add   di,2                    ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
endb    DF_strcpy

defsb   DP_strcpy
        db      "/* es:di strcpy( es:di, si:ax ) zaps ax,si */",0
        db      "#define DP_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define DP_strcpy_parms P_ESDI_SIAX",0
        db      "#define DP_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
        db      0
;
;       ES:DI _fstrcpy( ES:DI, SI:AX )
;       AX and SI are modified
;
beginb  DP_strcpy
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        _loop                           ; loop
          mov   al,[si]                 ; - get char from src
          mov   es:[di],al              ; - store in output buffer
          cmp   al,0                    ; - quit if end of string
          _quif e                       ; - ...
          mov   al,1[si]                ; - get next char
          add   si,2                    ; - bump up pointer
          mov   es:1[di],al             ; - copy it
          add   di,2                    ; - bump up pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
        pop     ds                      ; restore ds
endb    DP_strcpy

;=======================================================================

defsb   S_memset
        db      "/* di memset( di, al, cx ) zaps es,cx */",0
        db      "#define S_memset_ret   HW_D( HW_DI )",0
        db      "#define S_memset_parms P_DI_AL_CX",0
        db      "#define S_memset_saves HW_NotD_2( HW_ES, HW_CX )",0
        db      0

beginb  S_memset
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    S_memset


defsb   C_memset
        db      "/* di memset( di, al, cx ) zaps es,ah,cx */",0
        db      "#define C_memset_ret   HW_D( HW_DI )",0
        db      "#define C_memset_parms P_DI_AL_CX",0
        db      "#define C_memset_saves HW_NotD_3( HW_ES, HW_AH, HW_CX )",0
        db      0

beginb  C_memset
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    C_memset


defsb   ZF_memset
        db      "/* es:di memset( es:di, al, cx ) zaps cx */",0
        db      "#define ZF_memset_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZF_memset_parms  P_ESDI_AL_CX",0
        db      "#define ZF_memset_saves  HW_NotD( HW_CX )",0
        db      0

beginb  ZF_memset
        push    di                      ; save destination address
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    ZF_memset


defsb   ZP_memset
        db      "/* es:di memset( es:di, al, cx ) zaps cx */",0
        db      "#define ZP_memset_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZP_memset_parms P_ESDI_AL_CX",0
        db      "#define ZP_memset_saves HW_NotD( HW_CX )",0
        db      0

beginb  ZP_memset
        push    di                      ; save destination address
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    ZP_memset


defsb   DF_memset
        db      "/* es:di memset( es:di, al, cx ) zaps ah,cx */",0
        db      "#define DF_memset_ret  HW_D_2( HW_ES, HW_DI )",0
        db      "#define DF_memset_parms P_ESDI_AL_CX",0
        db      "#define DF_memset_saves  HW_NotD_2( HW_AH, HW_CX )",0
        db      0

beginb  DF_memset
        push    di                      ; save destination address
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    DF_memset


defsb   DP_memset
        db      "/* es:di memset( es:di, al, cx ) zaps ah,cx */",0
        db      "#define DP_memset_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define DP_memset_parms P_ESDI_AL_CX",0
        db      "#define DP_memset_saves HW_NotD_2( HW_AH, HW_CX )",0
        db      0

beginb  DP_memset
        push    di                      ; save destination address
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
endb    DP_memset
;=======================================================================

defsb   S_memcpy
        db      "/* di memcpy( di, si, cx ) zaps es,si,cx */",0
        db      "#define S_memcpy_ret   HW_D( HW_DI )",0
        db      "#define S_memcpy_parms P_DI_SI_CX",0
        db      "#define S_memcpy_saves  HW_NotD_3( HW_ES, HW_SI, HW_CX )",0
        db      0

beginb  S_memcpy
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
endb    S_memcpy


defsb   C_memcpy
        db      "/* di memcpy( di, si, cx ) zaps ax,es,si,cx */",0
        db      "#define C_memcpy_ret   HW_D( HW_DI )",0
        db      "#define C_memcpy_parms P_DI_SI_CX",0
        db      "#define C_memcpy_saves  HW_NotD_4( HW_AX, HW_ES, HW_SI, HW_CX )",0
        db      0

beginb  C_memcpy
        push    di                      ; save destination address
        mov     ax,ds                   ; set ES=DS
        mov     es,ax                   ; ...
        shr     cx,1                    ; calc. number of words to move
        rep     movsw                   ; move words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
endb    C_memcpy


defsb   ZF_memcpy
        db      "/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */",0
        db      "#define ZF_memcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZF_memcpy_parms  P_ESDI_DSSI_CX",0
        db      "#define ZF_memcpy_saves   HW_NotD_2( HW_SI, HW_CX )",0
        db      0

beginb  ZF_memcpy
        push    di                      ; save destination address
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
endb    ZF_memcpy


defsb   ZP_memcpy
        db      "/* es:di memcpy( es:di, cx:si, ax ) zaps cx,si,ax */",0
        db      "#define ZP_memcpy_ret   HW_D_2( HW_ES, HW_DI )",0
        db      "#define ZP_memcpy_parms  P_ESDI_CXSI_AX",0
        db      "#define ZP_memcpy_saves  HW_NotD_3( HW_SI, HW_CX, HW_AX )",0
        db      0

beginb  ZP_memcpy
        push    ds                      ; save DS
        push    di                      ; save destination address
        xchg    cx,ax                   ; move count to CX, segment to AX
        mov     ds,ax                   ; load segment
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
        pop     ds                      ; restore DS
endb    ZP_memcpy


defsb   DF_memcpy
        db      "/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */",0
        db      "#define DF_memcpy_ret  HW_D_2( HW_ES, HW_DI )",0
        db      "#define DF_memcpy_parms P_ESDI_DSSI_CX",0
        db      "#define DF_memcpy_saves  HW_NotD_2( HW_SI, HW_CX )",0
        db      0

beginb  DF_memcpy
        push    di                      ; save destination address
        shr     cx,1                    ; calc. number of words to move
        rep     movsw                   ; move words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
endb    DF_memcpy


defsb   DP_memcpy
        db      "/* es:di memcpy( es:di, cx:si, ax ) zaps cx,si,ax */",0
        db      "#define DP_memcpy_ret  HW_D_2( HW_ES, HW_DI )",0
        db      "#define DP_memcpy_parms P_ESDI_CXSI_AX",0
        db      "#define DP_memcpy_saves HW_NotD_3( HW_SI, HW_CX, HW_AX )",0
        db      0

beginb  DP_memcpy
        push    ds                      ; save DS
        push    di                      ; save destination address
        xchg    cx,ax                   ; move count to CX, segment to AX
        mov     ds,ax                   ; load segment
        shr     cx,1                    ; calc. number of words to move
        rep     movsw                   ; move words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
        pop     ds                      ; restore DS
endb    DP_memcpy

;=======================================================================


defsb   C_div
        db      "/* dx:ax  div( ax, cx ) */",0
        db      "#define C_div_ret       HW_D_2( HW_AX, HW_DX )",0
        db      "#define C_div_parms     P_AX_CX",0
        db      "#define C_div_saves     HW_NotD_2( HW_AX, HW_DX )",0
        db      0
beginb  C_div
        CWD
        IDIV    CX
endb    C_div

defsb   DF_stosw
        db      "/* es:di stosw( es:di, ds:si, cx ) zaps si,cx */",0
        db      "#define DF_stosw_ret   HW_D( HW_DI )",0
        db      "#define DF_stosw_parms P_ESDI_AX_CX",0
        db      "#define DF_stosw_saves HW_NotD_2( HW_DI, HW_CX )",0
        db      0
beginb  DF_stosw
        REP     STOSW
endb    DF_stosw

defsb   DF_stoswb
        db      "/* es:di stoswb( es:di, ds:si, cx ) zaps si,cx */",0
        db      "#define DF_stoswb_ret   HW_D( HW_DI )",0
        db      "#define DF_stoswb_parms P_ESDI_AX_CX",0
        db      "#define DF_stoswb_saves HW_NotD_2( HW_DI, HW_CX )",0
        db      0
beginb  DF_stoswb
        REP     STOSW
        STOSB
endb    DF_stoswb

defsb   C_memcmp
        db      "/* ax memcmp( si, di, cx ) zaps ax,es,si,di,cx */",0
        db      "#define C_memcmp_ret    HW_D( HW_AX )",0
        db      "#define C_memcmp_parms  P_SI_DI_CX",0
        db      "#define C_memcmp_saves  HW_NotD_5( HW_AX, HW_ES, HW_SI, HW_DI, HW_CX )",0
        db      0
beginb  C_memcmp
        MOV     AX,DS
        MOV     ES,AX
        XOR     AX,AX
        REPZ    CMPSB
        JZ      E_C_memcmp
        SBB     AX,AX
        SBB     AX,-1
endb    C_memcmp

defsb   DF_memcmp
        db      "/* cx memcmp( ds:si, es:di, cx ) zaps si,di,cx */",0
        db      "#define DF_memcmp_ret   HW_D( HW_CX )",0
        db      "#define DF_memcmp_parms P_DSSI_ESDI_CX",0
        db      "#define DF_memcmp_saves HW_NotD_3( HW_SI, HW_DI, HW_CX )",0
        db      0
beginb  DF_memcmp
        OR      CX,CX
        REPZ    CMPSB
        JZ      E_DF_memcmp
        SBB     CX,CX
        SBB     CX,-1
endb    DF_memcmp

defsb   DP_memcmp
        db      "/* cx memcmp( cx:si, es:di, ax ) zaps si,di,cx,ax */",0
        db      "#define DP_memcmp_ret   HW_D( HW_CX )",0
        db      "#define DP_memcmp_parms P_CXSI_ESDI_AX",0
        db      "#define DP_memcmp_saves HW_NotD_4( HW_SI, HW_DI, HW_CX, HW_AX )",0
        db      0
beginb  DP_memcmp
        PUSH    DS
        XCHG    AX,CX
        MOV     DS,AX
        OR      CX,CX
        REPZ    CMPSB
        JZ      E_DP_memcmp1
        SBB     CX,CX
        SBB     CX,-1
E_DP_memcmp1:
        POP     DS
endb    DP_memcmp

defsb   C_memchr
        db      "/* di memchr( di, al, cx ) zaps dx,es,di,cx */",0
        db      "#define C_memchr_ret    HW_D( HW_DI )",0
        db      "#define C_memchr_parms  P_DI_AL_CX",0
        db      "#define C_memchr_saves  HW_NotD_4( HW_DX, HW_ES, HW_DI, HW_CX )",0
        db      0
beginb  C_memchr
        JCXZ    E_C_memchr1
        MOV     DX,DS
        MOV     ES,DX
        REPNZ   SCASB
        JNZ     E_C_memchr1
        DEC     DI
        DB      0A9h
E_C_memchr1:
        MOV     DI,CX
endb    C_memchr

defsb   DF_memchr
        db      "/* cx:di memchr( es:di, al, cx ) zaps di,cx */",0
        db      "#define DF_memchr_ret   HW_D_2( HW_DI, HW_CX )",0
        db      "#define DF_memchr_parms P_ESDI_AL_CX",0
        db      "#define DF_memchr_saves HW_NotD_2( HW_DI, HW_CX )",0
        db      0
beginb  DF_memchr
        JCXZ    E_DF_memchr1
        REPNZ   SCASB
        JNZ     E_DF_memchr1
        DEC     DI
        MOV     CX,ES
        DB      0A9h
E_DF_memchr1:
        MOV     DI,CX
endb    DF_memchr

defsb   DP_memchr
        db      "/* cx:di memchr( es:di, al, cx ) zaps di,cx */",0
        db      "#define DP_memchr_ret   HW_D_2( HW_DI, HW_CX )",0
        db      "#define DP_memchr_parms P_ESDI_AL_CX",0
        db      "#define DP_memchr_saves HW_NotD_2( HW_DI, HW_CX )",0
        db      0
beginb  DP_memchr
        JCXZ    E_DP_memchr1
        REPNZ   SCASB
        JNZ     E_DP_memchr1
        DEC     DI
        MOV     CX,ES
        DB      0A9h
E_DP_memchr1:
        MOV     DI,CX
endb    DP_memchr

defsb   S_strcmp
        db      "/* ax strcmp( si, di ) zaps ax,si,di */",0
        db      "#define S_strcmp_ret    HW_D( HW_AX )",0
        db      "#define S_strcmp_parms  P_SI_DI",0
        db      "#define S_strcmp_saves  HW_NotD_3( HW_AX, HW_SI, HW_DI )",0
        db      0
beginb  S_strcmp
        _loop
          LODSB
          MOV   AH,Byte Ptr [DI]
          SUB   AL,AH
          JNZ   E_S_strcmp1
          INC   DI
          CMP   AL,AH
        _until e
E_S_strcmp1:
        SBB     AH,AH
endb    S_strcmp

defsb   C_strcmp
        db      "/* ax strcmp( si, di ) zaps ax,es,si,di,cx */",0
        db      "#define C_strcmp_ret    HW_D( HW_AX )",0
        db      "#define C_strcmp_parms  P_SI_DI",0
        db      "#define C_strcmp_saves  HW_NotD_5( HW_AX, HW_ES, HW_SI, HW_DI, HW_CX )",0
        db      0
beginb  C_strcmp
        MOV     AX,DS
        MOV     ES,AX
        CMPSB
        JNZ     E_C_strcmp1
        DEC     DI
        DEC     SI
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        SUB     DI,CX
        REPZ    CMPSB
        JZ      E_C_strcmp
E_C_strcmp1:
        SBB     AX,AX
        SBB     AX,-1
endb    C_strcmp

defsb   ZF_strcmp
        db      "/* ax strcmp( ds:si, es:di ) zaps ax,si,di */",0
        db      "#define ZF_strcmp_ret    HW_D( HW_AX )",0
        db      "#define ZF_strcmp_parms  P_DSSI_ESDI",0
        db      "#define ZF_strcmp_saves  HW_NotD_3( HW_AX, HW_SI, HW_DI )",0
        db      0
beginb  ZF_strcmp
        _loop
          LODSB
          MOV   AH,Byte Ptr ES:[DI]
          SUB   AL,AH
          JNZ   E_ZF_strcmp1
          INC   DI
          CMP   AL,AH
        _until e
E_ZF_strcmp1:
        SBB     AH,AH
endb    ZF_strcmp

defsb   ZP_strcmp
        db      "/* ax strcmp( si:ax, es:di ) zaps ax,si,di */",0
        db      "#define ZP_strcmp_ret    HW_D( HW_AX )",0
        db      "#define ZP_strcmp_parms  P_SIAX_ESDI",0
        db      "#define ZP_strcmp_saves  HW_NotD_3( HW_AX, HW_SI, HW_DI )",0
        db      0
beginb  ZP_strcmp
        PUSH    DS
        XCHG    AX,SI
        MOV     DS,AX
        _loop
          LODSB
          MOV   AH,Byte Ptr ES:[DI]
          SUB   AL,AH
          JNZ   E_ZP_strcmp1
          INC   DI
          CMP   AL,AH
        _until e
E_ZP_strcmp1:
        SBB     AH,AH
        POP     DS
endb    ZP_strcmp

defsb   DF_strcmp
        db      "/* cx strcmp( ds:si, es:di ) zaps ax,si,di,cx */",0
        db      "#define DF_strcmp_ret   HW_D( HW_CX )",0
        db      "#define DF_strcmp_parms P_DSSI_ESDI",0
        db      "#define DF_strcmp_saves HW_NotD_4( HW_AX, HW_SI, HW_DI, HW_CX )",0
        db      0
beginb  DF_strcmp
        CMPSB
        JNZ     E_DF_strcmp1
        DEC     DI
        DEC     SI
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        SUB     DI,CX
        REPZ    CMPSB
        JZ      E_DF_strcmp
E_DF_strcmp1:
        SBB     CX,CX
        SBB     CX,-1
endb    DF_strcmp

defsb   DP_strcmp
        db      "/* cx strcmp( si:ax, es:di ) zaps ax,si,di,cx */",0
        db      "#define DP_strcmp_ret   HW_D( HW_CX )",0
        db      "#define DP_strcmp_parms P_SIAX_ESDI",0
        db      "#define DP_strcmp_saves HW_NotD_4( HW_AX, HW_SI, HW_DI, HW_CX )",0
        db      0
beginb  DP_strcmp
        PUSH    DS
        XCHG    AX,SI
        MOV     DS,AX
        CMPSB
        JNZ     E_DP_strcmp1
        DEC     DI
        DEC     SI
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        SUB     DI,CX
        REPZ    CMPSB
        JZ      E_DP_strcmp2
E_DP_strcmp1:
        SBB     CX,CX
        SBB     CX,-1
E_DP_strcmp2:
        POP     DS
endb    DP_strcmp

defsb   C_strlen
        db      "/* cx strlen( di ) zaps ax,es,cx,di */",0
        db      "#define C_strlen_ret    HW_D( HW_CX )",0
        db      "#define C_strlen_parms  P_DI",0
        db      "#define C_strlen_saves  HW_NotD_4( HW_AX, HW_ES, HW_CX, HW_DI )",0
        db      0
beginb  C_strlen
        MOV     AX,DS
        MOV     ES,AX
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        DEC     CX
endb    C_strlen

defsb   DF_strlen
        db      "/* cx strlen( es:di ) zaps ax,cx,di */",0
        db      "#define DF_strlen_ret   HW_D( HW_CX )",0
        db      "#define DF_strlen_parms P_ESDI",0
        db      "#define DF_strlen_saves HW_NotD_3( HW_AX, HW_CX, HW_DI )",0
        db      0
beginb  DF_strlen
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        DEC     CX
endb    DF_strlen

defsb   DP_strlen
        db      "/* cx strlen( es:di ) zaps ax,cx,di */",0
        db      "#define DP_strlen_ret   HW_D( HW_CX )",0
        db      "#define DP_strlen_parms P_ESDI",0
        db      "#define DP_strlen_saves HW_NotD_3( HW_AX, HW_CX, HW_DI )",0
        db      0
beginb  DP_strlen
        MOV     CX,-1
        XOR     AX,AX
        REPNZ   SCASB
        NOT     CX
        DEC     CX
endb    DP_strlen

defsb   C_abs
        db      "#define C_abs_ret    HW_D( HW_AX )",0
        db      "#define C_abs_parms  P_AX",0
        db      "#define C_abs_saves  HW_NotD_2( HW_AX, HW_DX )",0
        db      0
beginb  C_abs
        CWD
        XOR     AX,DX
        SUB     AX,DX
endb    C_abs

defsb   C_labs
        db      "#define C_labs_ret    HW_D_2( HW_AX, HW_DX )",0
        db      "#define C_labs_parms  P_AXDX",0
        db      "#define C_labs_saves  HW_NotD_2( HW_AX, HW_DX )",0
        db      0
beginb  C_labs
        OR      DX,DX
        JGE     E_C_labs
        NEG     AX
        ADC     DX,0
        NEG     DX
endb    C_labs

defsb   C_inp
        db      "#define C_inp_ret    HW_D( HW_AX )",0
        db      "#define C_inp_parms  P_DX",0
        db      "#define C_inp_saves  HW_NotD( HW_AX )",0
        db      0
beginb  C_inp
        IN      AL,DX
        SUB     AH,AH
endb    C_inp

defsb   C_inpw
        db      "#define C_inpw_ret    HW_D( HW_AX )",0
        db      "#define C_inpw_parms  P_DX",0
        db      "#define C_inpw_saves  HW_NotD( HW_AX )",0
        db      0
beginb  C_inpw
        IN      AX,DX
endb    C_inpw

defsb   C_outp
        db      "#define C_outp_ret    HW_D( HW_AX )",0
        db      "#define C_outp_parms  P_DX_AL",0
        db      "#define C_outp_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_outp
        OUT     DX,AL
endb    C_outp

defsb   C_outpw
        db      "#define C_outpw_ret    HW_D( HW_AX )",0
        db      "#define C_outpw_parms  P_DX_AX",0
        db      "#define C_outpw_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_outpw
        OUT     DX,AX
endb    C_outpw

defsb   C_movedata
        db      "#define C_movedata_ret    HW_D( HW_EMPTY )",0
        db      "#define C_movedata_parms  P_AX_SI_ES_DI_CX",0
        db      "#define C_movedata_saves  HW_NotD_3( HW_CX, HW_SI, HW_DI )",0
        db      0
beginb  C_movedata
        PUSH    DS
        MOV     DS,AX
        SHR     CX,1
        REP     MOVSW
        ADC     CX,CX
        REP     MOVSB
        POP     DS
endb    C_movedata

defsb   C_enable
        db      "#define C_enable_ret    HW_D( HW_EMPTY )",0
        db      "#define C_enable_parms  P_AX",0
        db      "#define C_enable_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_enable
        STI
        CLD
endb    C_enable

defsb   C_disable
        db      "#define C_disable_ret    HW_D( HW_EMPTY )",0
        db      "#define C_disable_parms  P_AX",0
        db      "#define C_disable_saves  HW_NotD( HW_EMPTY )",0
        db      0
beginb  C_disable
        CLI
endb    C_disable

defsb   C_rotl
        db      "#define C_rotl_ret    HW_D( HW_AX )",0
        db      "#define C_rotl_parms  P_AX_CX",0
        db      "#define C_rotl_saves  HW_NotD( HW_AX )",0
        db      0
beginb  C_rotl
        ROL     AX,CL
endb    C_rotl

defsb   C_rotr
        db      "#define C_rotr_ret    HW_D( HW_AX )",0
        db      "#define C_rotr_parms  P_AX_CX",0
        db      "#define C_rotr_saves  HW_NotD( HW_AX )",0
        db      0
beginb  C_rotr
        ROR     AX,CL
endb    C_rotr

defsb   C_fabs
        db      "#define C_fabs_ret    HW_D( HW_ABCD )",0
        db      "#define C_fabs_parms  P_AXBXCXDX",0
        db      "#define C_fabs_saves  HW_NotD( HW_ABCD )",0
        db      0
beginb  C_fabs
        AND     AH,7Fh
endb    C_fabs

defsb   C_min
        db      "#define C_min_ret   HW_D( HW_AX )",0
        db      "#define C_min_parms P_AX_DX",0
        db      "#define C_min_saves HW_NotD_2( HW_CX, HW_AX )",0
        db      0
beginb  C_min
        SUB     AX,DX
        SBB     CX,CX
        AND     AX,CX
        ADD     AX,DX
endb    C_min

defsb   C_max
        db      "#define C_max_ret   HW_D( HW_AX )",0
        db      "#define C_max_parms P_AX_DX",0
        db      "#define C_max_saves HW_NotD_2( HW_CX, HW_AX )",0
        db      0
beginb  C_max
        SUB     AX,DX
        CMC
        SBB     CX,CX
        AND     AX,CX
        ADD     AX,DX
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
        func    ZF_strcpy
        func    ZP_strcpy
        func    DF_strcpy
        func    DP_strcpy
        func    C_strcat
        func    S_strcat
        func    ZF_strcat
        func    ZP_strcat
        func    DF_strcat
        func    DP_strcat
        func    C_strchr
        func    DF_strchr
        func    DP_strchr
        func    C_memset
        func    S_memset
        func    ZF_memset
        func    ZP_memset
        func    DF_memset
        func    DP_memset
        func    C_memcpy
        func    S_memcpy
        func    ZF_memcpy
        func    ZP_memcpy
        func    DF_memcpy
        func    DP_memcpy

        func    C_div
        func    DF_stosw
        func    DF_stoswb
        func    C_memcmp
        func    DF_memcmp
        func    DP_memcmp
        func    C_memchr
        func    DF_memchr
        func    DP_memchr
        func    S_strcmp
        func    C_strcmp
        func    ZF_strcmp
        func    ZP_strcmp
        func    DF_strcmp
        func    DP_strcmp
        func    C_strlen
        func    DF_strlen
        func    DP_strlen
        func    C_abs
        func    C_labs
        func    C_inp
        func    C_inpw
        func    C_outp
        func    C_outpw
        func    C_movedata
        func    C_enable
        func    C_disable
        func    C_rotl
        func    C_rotr
        func    C_fabs
        func    C_min
        func    C_max

        dw      0,0,0
_DATA   ends

        end
