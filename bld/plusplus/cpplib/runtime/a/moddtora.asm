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
; MODDTORA.ASM -- reference for program DTOR
;
; An external reference to this object file is placed in the compiled
; object when the module being compiled has static objects which
; require DTORing.
;
; The close-up code will call the routine __wcpp_module_dtor__ to DTOR
; all such static objects.
;
        name    moddtora

        include mdef.inc
        include xinit.inc

#ifdef __3S__
#define __wcpp_module_dtor__     __wcpp_module_dtor_
#define __wcpp_module_dtor_ref__ __wcpp_module_dtor_ref_
#endif

        codeptr __wcpp_module_dtor__

YI      segment word public 'DATA'
        public  __wcpp_module_dtor_ref__
__wcpp_module_dtor_ref__ label word
YI      ends

        xfini   __wcpp_module_dtor__,DEF_PRIORITY+8

        end
