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


;
; PRWDATA.ASM -- System and Process specific data for PLIB.
;
;       Thread specific data is allocated by cppdata.c for non multi-thread
;       libraries.  For multi-thread libraries, the data is allocated by
;       the BeginThread() c library function.
;

        ;name    prwdata
        assume  nothing

        include mdef.inc

DGROUP  group   _DATA

_DATA   segment word public 'DATA'
        assume  DS:DGROUP

;_wint_new_handler              dd 0
;_wint_terminate_handler        dd 0
;_wint_unexpected_handler       dd 0
ifdef __MT__
    _wint_static_init_sema      dd 0
ifdef __QNX__
                                dd 0    ; QNX has an 8-byte semaphore object!!!
endif
                                dd 0
                                dd 0
endif
_wint_pure_error_flag           dw 0
_wint_undef_vfun_flag           dw 0
if _MODEL and ( _BIG_DATA or _HUGE_DATA or _USE_32_SEGS )
    _wint_module_init           dd 0
else
    _wint_module_init           dw 0
endif

;       public          "C",_wint_new_handler
;       public          "C",_wint_terminate_handler
;       public          "C",_wint_unexpected_handler
        ifdef __MT__
            public      "C",_wint_static_init_sema
        endif
        public          "C",_wint_pure_error_flag
        public          "C",_wint_undef_vfun_flag
        public          "C",_wint_module_init

_DATA   ends

        end
