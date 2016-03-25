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


        modstart  386sind

        xdefp   __sinpoly
        xdefp   __cospoly

;;_PIby6  dt    0.523598775598298873078
;;
;;extern  double  _EvalPoly( double, double *, int );
;;extern  double  _OddPoly( double, double *, int );
;;
;;
;;static double _sinpoly[] = {
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15),
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11),
;;         1.0 / (2.*3*4*5*6*7*8*9),
;;        -1.0 / (2.*3*4*5*6*7),
;;         1.0 / (2.*3*4*5),
;;        -1.0 / (2.*3),
;;         1.0
;;};

        align   2
__sinpoly dt   0.0000000000000028114572543455207631  ; 1./17!
   dt  -0.00000000000076471637318198164759  ; 1./15!
   dt   0.00000000016059043836821614599  ; 1./13!
   dt  -0.000000025052108385441718775  ; 1./11!
   dt   0.0000027557319223985890652  ; 1./9!
   dt  -0.00019841269841269841269  ; 1./7!
   dt   0.0083333333333333333333  ; 1./5!
   dt  -0.16666666666666666666  ; 1./3!
   dt   1.00000000000000000000  ; 1./1!
;;
;;static double _cospoly[] = {
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14),
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10),
;;         1.0 / (2.*3*4*5*6*7*8),
;;        -1.0 / (2.*3*4*5*6),
;;         1.0 / (2.*3*4),
;;        -1.0 / (2.),
;;         1.0
;;};

__cospoly   dt   0.000000000000047794773323873852974  ; 1./16!
   dt  -0.000000000011470745597729724713  ; 1./14!
   dt   0.0000000020876756987868098979  ; 1./12!
   dt  -0.00000027557319223985890652  ; 1./10!
   dt   0.000024801587301587301587  ; 1./8!
   dt  -0.0013888888888888888888  ; 1./6!
   dt   0.041666666666666666666  ; 1./4!
   dt  -0.50000000000000000000  ; 1./2!
   dt   1.00000000000000000000  ; 1./1!
