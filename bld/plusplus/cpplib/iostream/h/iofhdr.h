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
* Description:  Floating-point support declaration for iostreams
*
****************************************************************************/

#ifndef __IOFHDR_H_INCLUDED
#define __IOFHDR_H_INCLUDED

#include "variety.h"
#include <float.h>
#include "xfloat.h"

extern std::ios::iostate __GetLDFloat( streambuf *, char *);

_WPRTLINK typedef void  (*_type_EFG_cnvs2d)( char *, double * );
_WPRTLINK typedef int   (*_type_EFG_cnvd2f)( double *, float * );
_WPRTLINK typedef void  (*_type_EFG_LDcvt)( long_double *, CVT_INFO *, char * );
_WPRTLINK typedef char  *(*_type_EFG_fcvt)( double, int, int *, int * );
#ifdef _LONG_DOUBLE_
_WPRTLINK typedef void  (*_type_EFG__FDLD)( double _WCNEAR *, long_double _WCNEAR * );
#endif

_WPRTLINK extern _type_EFG_cnvs2d __EFG_cnvs2d;
_WPRTLINK extern _type_EFG_cnvd2f __EFG_cnvd2f;
_WPRTLINK extern _type_EFG_LDcvt  __EFG_LDcvt;
_WPRTLINK extern _type_EFG_fcvt   __EFG_fcvt;
#ifdef _LONG_DOUBLE_
_WPRTLINK extern _type_EFG__FDLD  __EFG__FDLD;
#endif

// from math library
extern "C" {
_WMRTLINK extern void  __cnvs2d( char *, double * );
_WMRTLINK extern int   __cnvd2f( double *, float * );
#ifdef _LONG_DOUBLE_
_WMRTLINK extern void  __cnvd2ld( double _WCNEAR *, long_double _WCNEAR * );
#endif
};

#endif
