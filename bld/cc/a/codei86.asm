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


; CODEi86:     Pragma code bursts for built-in compiler pragmas
;
; - these routines are easier to code in assembler than in C

.286p
include struct.inc

        name    codei86

_DATA   segment word public 'DATA'
DGROUP  group   _DATA
        assume  CS:_DATA

strcat_body     macro
        mov     cx,-1                   ; set length to -1
        mov     al,0                    ; scan for null character
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
        mov     al,0                    ; scan for null character
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

        public  _C_strcat
_C_strcat_name: db      "C_strcat",0
_C_strcat_defs:
  db    "/* di strcat( di, si ) zaps ax,es,cx,si */",0
  db    "#define C_strcat_ret   HW_D( HW_DI )",0
  db    "#define C_strcat_parms P_DI_SI",0
  db    "#define C_strcat_saves HW_NotD_4( HW_AX, HW_ES, HW_CX, HW_SI )",0
  db    0

;       DI strcat( DI, SI )
;       ES,CX,AX and SI are modified
;
_C_strcat:                              ; small data model strcat for -ot
        db      E_C_strcat - _C_strcat - 1
        push    ds                      ; set es=ds
        pop     es                      ; ...
        push    di                      ; save destination address
        strcat_body                     ; concatenate strings
        pop     di                      ; restore destination address
E_C_strcat:

        public  _S_strcat
_S_strcat_name: db      "S_strcat",0
_S_strcat_defs:
  db    "/* di strcat( di, si ) zaps ax,es,si */",0
  db    "#define S_strcat_ret   HW_D( HW_DI )",0
  db    "#define S_strcat_parms P_DI_SI",0
  db    "#define S_strcat_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       DI strcat( DI, SI )
;       AX and SI are modified
;
_S_strcat:                              ; small data model strcat for -os
        db      E_S_strcat - _S_strcat - 1
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
E_S_strcat:

        public  _ZF_strcat
_ZF_strcat_name:        db      "ZF_strcat",0
_ZF_strcat_defs:
  db    "/* es:di strcat( es:di, ds:si ) zaps ax,si */",0
  db    "#define ZF_strcat_ret    HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZF_strcat_parms  P_ESDI_DSSI",0
  db    "#define ZF_strcat_saves  HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcat( ES:DI, DS:SI )
;       AX and SI are modified
;
_ZF_strcat:                             ; large data model strcat for -os
        db      E_ZF_strcat - _ZF_strcat - 1
        push    di                      ; save destination address
        push    cx                      ; save cx
        mov     cx,-1                   ; set length to -1
        mov     al,0                    ; scan for null character
        repnz   scasb                   ; ...
        pop     cx                      ; restore cx
        dec     di                      ; point to null character
        _loop                           ; loop (concatenate strings)
          lodsb                         ; - get char from src
          stosb                         ; - append to end of dest
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination string address
E_ZF_strcat:

        public  _ZP_strcat
_ZP_strcat_name:        db      "ZP_strcat",0
_ZP_strcat_defs:
  db    "/* es:di strcat( es:di, si:ax ) zaps ax,si */",0
  db    "#define ZP_strcat_ret    HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZP_strcat_parms  P_ESDI_SIAX",0
  db    "#define ZP_strcat_saves  HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcat( ES:DI, SI:AX )
;       AX and SI are modified
;
_ZP_strcat:                             ; large data model strcat for -os
        db      E_ZP_strcat - _ZP_strcat - 1
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        push    cx                      ; save cx
        mov     cx,-1                   ; set length to -1
        mov     al,0                    ; scan for null character
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
E_ZP_strcat:

        public  _DF_strcat
_DF_strcat_name:        db      "DF_strcat",0
_DF_strcat_defs:
  db    "/* es:di strcat( es:di, ds:si ) zaps cx,ax,si */",0
  db    "#define DF_strcat_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define DF_strcat_parms P_ESDI_DSSI",0
  db    "#define DF_strcat_saves HW_NotD_3( HW_CX, HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcat( ES:DI, DS:SI )
;       AX,CX and SI are modified
;
_DF_strcat:                             ; large data model strcat for -ot
        db      E_DF_strcat - _DF_strcat - 1
        push    di                      ; save destination address
        BD_strcat_body
        pop     di                      ; restore destination address
E_DF_strcat:

        public  _DP_strcat
_DP_strcat_name:        db      "DP_strcat",0
_DP_strcat_defs:
  db    "/* es:di strcat( es:di, si:ax ) zaps cx,ax,si */",0
  db    "#define DP_strcat_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define DP_strcat_parms P_ESDI_SIAX",0
  db    "#define DP_strcat_saves HW_NotD_3( HW_CX, HW_AX, HW_SI )",0
  db    0
;
;       ES:DI _fstrcat( ES:DI, SI:AX )
;       AX,CX and SI are modified
;
_DP_strcat:                             ; large data model strcat for -os
        db      E_DP_strcat - _DP_strcat - 1
        push    ds                      ; save ds
        push    di                      ; save destination address
        xchg    si,ax                   ; get offset into si, segment into ax
        mov     ds,ax                   ; load segment
        BD_strcat_body
        pop     di                      ; restore destination string address
        pop     ds                      ; restore ds
E_DP_strcat:

;=======================================================================
        public  _C_strchr
_C_strchr_name: db      "C_strchr",0
_C_strchr_defs:
  db    "/* si strchr( si, dl ) zaps ax,si */",0
  db    "#define C_strchr_ret   HW_D( HW_SI )",0
  db    "#define C_strchr_parms P_SI_DL",0
  db    "#define C_strchr_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       SI strchr( SI, DL )
;       AX and SI are modified
;
_C_strchr:                              ; small data model strchr for -ot
        db      E_C_strchr - _C_strchr - 1
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
E_C_strchr:

        public  _DF_strchr
_DF_strchr_name:        db      "DF_strchr",0
_DF_strchr_defs:
  db    "/* dx:si strchr( ds:si, cl ) zaps ds,si,ax */",0
  db    "#define DF_strchr_ret  HW_D_2( HW_DX, HW_SI )",0
  db    "#define DF_strchr_parms P_DSSI_CL",0
  db    "#define DF_strchr_saves HW_NotD_3( HW_DX, HW_SI, HW_AX )",0
  db    0
;
;       DX:SI strchr( DS:SI, CL )
;       AX,DX and SI are modified
;
_DF_strchr:                             ; large data model strchr for -ot
        db      E_DF_strchr - _DF_strchr - 1
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
E_DF_strchr:

        public  _DP_strchr
_DP_strchr_name:        db      "DP_strchr",0
_DP_strchr_defs:
  db    "/* dx:si strchr( dx:si, cl ) zaps dx,si,ax */",0
  db    "#define DP_strchr_ret  HW_D_2( HW_DX, HW_SI )",0
  db    "#define DP_strchr_parms P_DXSI_CL",0
  db    "#define DP_strchr_saves HW_NotD_3( HW_DX, HW_SI, HW_AX )",0
  db    0
;
;       DX:SI strchr( DX:SI, CL )
;       AX,DX and SI are modified
;
_DP_strchr:                             ; large data model strchr for -ot
        db      E_DP_strchr - _DP_strchr - 1
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
E_DP_strchr:

;=======================================================================
        public  _C_strcpy
_C_strcpy_name: db      "C_strcpy",0
_C_strcpy_defs:
  db    "/* di strcpy( di, si ) zaps ax,si */",0
  db    "#define C_strcpy_ret   HW_D( HW_DI )",0
  db    "#define C_strcpy_parms P_DI_SI",0
  db    "#define C_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       DI strcpy( DI, SI )
;       AX and SI are modified
;
_C_strcpy:                              ; small data model strcpy for -ot
        db      E_C_strcpy - _C_strcpy - 1
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
E_C_strcpy:

        public  _S_strcpy
_S_strcpy_name: db      "S_strcpy",0
_S_strcpy_defs:
  db    "/* di strcpy( di, si ) zaps ax,si */",0
  db    "#define S_strcpy_ret   HW_D( HW_DI )",0
  db    "#define S_strcpy_parms P_DI_SI",0
  db    "#define S_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       DI strcpy( DI, SI )
;       AX and SI are modified
;
_S_strcpy:                              ; small data model strcpy for -os
        db      E_S_strcpy - _S_strcpy - 1
        push    di                      ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          mov   [di],al                 ; - store in output buffer
          inc   di                      ; - increment pointer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
E_S_strcpy:

        public  _ZF_strcpy
_ZF_strcpy_name:        db      "ZF_strcpy",0
_ZF_strcpy_defs:
  db    "/* es:di strcpy( es:di, ds:si ) zaps ax,si */",0
  db    "#define ZF_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZF_strcpy_parms P_ESDI_DSSI",0
  db    "#define ZF_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcpy( ES:DI, DS:SI )
;       AX and SI are modified
;
_ZF_strcpy:                             ; large data model strcpy for -os
        db      E_ZF_strcpy - _ZF_strcpy - 1
        push    di                      ; save destination address
        _loop                           ; loop
          lodsb                         ; - get char from src
          stosb                         ; - store in output buffer
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        pop     di                      ; restore destination address
E_ZF_strcpy:

        public  _ZP_strcpy
_ZP_strcpy_name:        db      "ZP_strcpy",0
_ZP_strcpy_defs:
  db    "/* es:di strcpy( es:di, si:ax ) zaps ax,si */",0
  db    "#define ZP_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZP_strcpy_parms P_ESDI_SIAX",0
  db    "#define ZP_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcpy( ES:DI, SI:AX )
;       AX and SI are modified
;
_ZP_strcpy:                             ; large data model strcpy for -os
        db      E_ZP_strcpy - _ZP_strcpy - 1
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
E_ZP_strcpy:

        public  _DF_strcpy
_DF_strcpy_name:        db      "DF_strcpy",0
_DF_strcpy_defs:
  db    "/* es:di strcpy( es:di, ds:si ) zaps ax,si */",0
  db    "#define DF_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define DF_strcpy_parms P_ESDI_DSSI",0
  db    "#define DF_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI strcpy( ES:DI, DS:SI )
;       AX and SI are modified
;
_DF_strcpy:                             ; large data model strcpy for -ot
        db      E_DF_strcpy - _DF_strcpy - 1
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
E_DF_strcpy:

        public  _DP_strcpy
_DP_strcpy_name:        db      "DP_strcpy",0
_DP_strcpy_defs:
  db    "/* es:di strcpy( es:di, si:ax ) zaps ax,si */",0
  db    "#define DP_strcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define DP_strcpy_parms P_ESDI_SIAX",0
  db    "#define DP_strcpy_saves HW_NotD_2( HW_AX, HW_SI )",0
  db    0
;
;       ES:DI _fstrcpy( ES:DI, SI:AX )
;       AX and SI are modified
;
_DP_strcpy:                             ; model-independent _fstrcpy
        db      E_DP_strcpy - _DP_strcpy - 1
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
E_DP_strcpy:

;=======================================================================

        public  _S_memset
_S_memset_name: db      "S_memset",0
_S_memset_defs:
  db    "/* di memset( di, al, cx ) zaps es,cx */",0
  db    "#define S_memset_ret   HW_D( HW_DI )",0
  db    "#define S_memset_parms P_DI_AL_CX",0
  db    "#define S_memset_saves HW_NotD_2( HW_ES, HW_CX )",0
  db    0

_S_memset:
        db      E_S_memset - _S_memset - 1
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_S_memset:


        public  _C_memset
_C_memset_name: db      "C_memset",0
_C_memset_defs:
  db    "/* di memset( di, al, cx ) zaps es,ah,cx */",0
  db    "#define C_memset_ret   HW_D( HW_DI )",0
  db    "#define C_memset_parms P_DI_AL_CX",0
  db    "#define C_memset_saves HW_NotD_3( HW_ES, HW_AH, HW_CX )",0
  db    0

_C_memset:
        db      E_C_memset - _C_memset - 1
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_C_memset:


        public  _ZF_memset
_ZF_memset_name:        db      "ZF_memset",0
_ZF_memset_defs:
  db    "/* es:di memset( es:di, al, cx ) zaps cx */",0
  db    "#define ZF_memset_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZF_memset_parms  P_ESDI_AL_CX",0
  db    "#define ZF_memset_saves  HW_NotD( HW_CX )",0
  db    0

_ZF_memset:
        db      E_ZF_memset - _ZF_memset - 1
        push    di                      ; save destination address
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_ZF_memset:


        public  _ZP_memset
_ZP_memset_name:        db      "ZP_memset",0
_ZP_memset_defs:
  db    "/* es:di memset( es:di, al, cx ) zaps cx */",0
  db    "#define ZP_memset_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZP_memset_parms P_ESDI_AL_CX",0
  db    "#define ZP_memset_saves HW_NotD( HW_CX )",0
  db    0

_ZP_memset:
        db      E_ZP_memset - _ZP_memset - 1
        push    di                      ; save destination address
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_ZP_memset:


        public  _DF_memset
_DF_memset_name:        db      "DF_memset",0
_DF_memset_defs:
  db    "/* es:di memset( es:di, al, cx ) zaps ah,cx */",0
  db    "#define DF_memset_ret  HW_D_2( HW_ES, HW_DI )",0
  db    "#define DF_memset_parms P_ESDI_AL_CX",0
  db    "#define DF_memset_saves  HW_NotD_2( HW_AH, HW_CX )",0
  db    0

_DF_memset:
        db      E_DF_memset - _DF_memset - 1
        push    di                      ; save destination address
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_DF_memset:


        public  _DP_memset
_DP_memset_name:        db      "DP_memset",0
_DP_memset_defs:
  db    "/* es:di memset( es:di, al, cx ) zaps ah,cx */",0
  db    "#define DP_memset_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define DP_memset_parms P_ESDI_AL_CX",0
  db    "#define DP_memset_saves HW_NotD_2( HW_AH, HW_CX )",0
  db    0

_DP_memset:
        db      E_DP_memset - _DP_memset - 1
        push    di                      ; save destination address
        mov     ah,al                   ; duplicate byte to store in high byte
        shr     cx,1                    ; calc. number of words to store
        rep     stosw                   ; store words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     stosb                   ; do repeat store
        pop     di                      ; restore destination address
E_DP_memset:
;=======================================================================

        public  _S_memcpy
_S_memcpy_name: db      "S_memcpy",0
_S_memcpy_defs:
  db    "/* di memcpy( di, si, cx ) zaps es,si,cx */",0
  db    "#define S_memcpy_ret   HW_D( HW_DI )",0
  db    "#define S_memcpy_parms P_DI_SI_CX",0
  db    "#define S_memcpy_saves  HW_NotD_3( HW_ES, HW_SI, HW_CX )",0
  db    0

_S_memcpy:
        db      E_S_memcpy - _S_memcpy - 1
        push    di                      ; save destination address
        push    ds                      ; set ES=DS
        pop     es                      ; ...
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
E_S_memcpy:


        public  _C_memcpy
_C_memcpy_name: db      "C_memcpy",0
_C_memcpy_defs:
  db    "/* di memcpy( di, si, cx ) zaps ax,es,si,cx */",0
  db    "#define C_memcpy_ret   HW_D( HW_DI )",0
  db    "#define C_memcpy_parms P_DI_SI_CX",0
  db    "#define C_memcpy_saves  HW_NotD_4( HW_AX, HW_ES, HW_SI, HW_CX )",0
  db    0

_C_memcpy:
        db      E_C_memcpy - _C_memcpy - 1
        push    di                      ; save destination address
        mov     ax,ds                   ; set ES=DS
        mov     es,ax                   ; ...
        shr     cx,1                    ; calc. number of words to move
        rep     movsw                   ; move words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
E_C_memcpy:


        public  _ZF_memcpy
_ZF_memcpy_name:        db      "ZF_memcpy",0
_ZF_memcpy_defs:
  db    "/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */",0
  db    "#define ZF_memcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZF_memcpy_parms  P_ESDI_DSSI_CX",0
  db    "#define ZF_memcpy_saves   HW_NotD_2( HW_SI, HW_CX )",0
  db    0

_ZF_memcpy:
        db      E_ZF_memcpy - _ZF_memcpy - 1
        push    di                      ; save destination address
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
E_ZF_memcpy:


        public  _ZP_memcpy
_ZP_memcpy_name:        db      "ZP_memcpy",0
_ZP_memcpy_defs:
  db    "/* es:di memcpy( es:di, cx:si, ax ) zaps cx,si,ax */",0
  db    "#define ZP_memcpy_ret   HW_D_2( HW_ES, HW_DI )",0
  db    "#define ZP_memcpy_parms  P_ESDI_CXSI_AX",0
  db    "#define ZP_memcpy_saves  HW_NotD_3( HW_SI, HW_CX, HW_AX )",0
  db    0

_ZP_memcpy:
        db      E_ZP_memcpy - _ZP_memcpy - 1
        push    ds                      ; save DS
        push    di                      ; save destination address
        xchg    cx,ax                   ; move count to CX, segment to AX
        mov     ds,ax                   ; load segment
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
        pop     ds                      ; restore DS
E_ZP_memcpy:


        public  _DF_memcpy
_DF_memcpy_name:        db      "DF_memcpy",0
_DF_memcpy_defs:
  db    "/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */",0
  db    "#define DF_memcpy_ret  HW_D_2( HW_ES, HW_DI )",0
  db    "#define DF_memcpy_parms P_ESDI_DSSI_CX",0
  db    "#define DF_memcpy_saves  HW_NotD_2( HW_SI, HW_CX )",0
  db    0

_DF_memcpy:
        db      E_DF_memcpy - _DF_memcpy - 1
        push    di                      ; save destination address
        shr     cx,1                    ; calc. number of words to move
        rep     movsw                   ; move words
        adc     cx,cx                   ; see if 1 more byte to do
        rep     movsb                   ; do repeat move
        pop     di                      ; restore destination address
E_DF_memcpy:


        public  _DP_memcpy
_DP_memcpy_name:        db      "DP_memcpy",0
_DP_memcpy_defs:
  db    "/* es:di memcpy( es:di, cx:si, ax ) zaps cx,si,ax */",0
  db    "#define DP_memcpy_ret  HW_D_2( HW_ES, HW_DI )",0
  db    "#define DP_memcpy_parms P_ESDI_CXSI_AX",0
  db    "#define DP_memcpy_saves HW_NotD_3( HW_SI, HW_CX, HW_AX )",0
  db    0

_DP_memcpy:
        db      E_DP_memcpy - _DP_memcpy - 1
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
E_DP_memcpy:
;=======================================================================
func    macro   name
        dw      DGROUP:_&name&_defs,DGROUP:_&name&_name,DGROUP:_&name
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
        dw      0,0,0
_DATA   ends

        end
