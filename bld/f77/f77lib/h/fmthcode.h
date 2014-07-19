/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Declaration of math error handling routines
*
****************************************************************************/

#include "mathcode.h"

/*      The first parm to __...math1err and __...math2err is an unsigned int
        that indicates the math function, the type of error, and the
        expected result value.
*/

extern  intstar4        __imath1err(unsigned int,intstar4 *);
extern  intstar4        __imath2err(unsigned int,intstar4 *,intstar4 *);
extern  single          __amath1err(unsigned int,single *);
extern  single          __amath2err(unsigned int,single *,single *);
extern  complex         __zmath1err(unsigned int,complex *);
extern  complex         __zmath2err(unsigned int,complex *,complex *);
extern  dcomplex        __qmath1err(unsigned int,dcomplex *);
extern  dcomplex        __qmath2err(unsigned int,dcomplex *,dcomplex *);
extern  double          __math1err(unsigned int,double *);
extern  double          __math2err(unsigned int,double *,double *);
