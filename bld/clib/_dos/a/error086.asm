;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  16-bit code only
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
        defn    __doserror_
        _if     c               ; if error
          push  AX              ; - save return code
          call  __set_errno_dos ; - set errno
          pop   AX              ; - restore return code
        _else                   ; else
          sub   AX,AX           ; - return 0
        _endif                  ; endif
        ret                     ; return to caller
        endproc __doserror_

        xdefp   __doserror1_
;
        defn    __doserror1_
        _if     c               ; if error
          call  __set_errno_dos ; - set errno, return -1
        _else                   ; else
          sub   AX,AX           ; - return 0
        _endif                  ; endif
        ret                     ; return to caller
        endproc __doserror1_

        endmod
        end
