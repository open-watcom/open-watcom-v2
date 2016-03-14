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
;
include mdef.inc
include struct.inc

        xrefp   "C",__set_errno_dos
        modstart doserror

        xdefp   __doserror_
;
        defp    __doserror_
        _if     c               ; if error
          and   EAX,0000FFFFh   ; - get rid of any garbage bits
          push  EAX             ; - save return code
          call  __set_errno_dos ; - set errno
          pop   EAX             ; - restore return code
        _else                   ; else
          sub   EAX,EAX         ; - return 0
        _endif                  ; endif
        ret                     ; return to caller
        endproc __doserror_

        xdefp   __doserror1_
;
        defp    __doserror1_
        _if     c               ; if error
          and   EAX,0000FFFFh   ; - get rid of any garbage bits
ifdef __STACK__
          push  EAX             ; - pass DOS error code on stack
endif
          call  __set_errno_dos ; - set errno, return -1
ifdef __STACK__
          add   esp,4           ; - cleanup stack
endif
        _else                   ; else
          sub   EAX,EAX         ; - return 0
        _endif                  ; endif
        ret                     ; return to caller
        endproc __doserror1_

        endmod
        end
