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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  93/05/26    Greg Bentz      pull floating point out of istream/ostream
//  93/10/08    Greg Bentz      make LDFloatToString set scale_factor to 1
//                              for _Ftos when 'G' format
//  93/10/25    Raymond Tang    Split into separate files.
//  95/06/19    Greg Bentz      indirect calls to math library
//  99/10/12    J.B.Schueler    moved definition of enums to IOFOUTFL.CPP
//                              where only they are used

#ifndef __IOFHDR_H_INCLUDED
#define __IOFHDR_H_INCLUDED

#include "variety.h"
#include <float.h>
#include "xfloat.h"

extern ios::iostate __GetLDFloat( streambuf *, char *);
extern void __LDFloatToString( char *, double const *, int, ios::fmtflags);

typedef void  (*_type_EFG_cnvs2d)( char *, double * );
typedef int   (*_type_EFG_cnvd2f)( double *, float * );
typedef void  (*_type_EFG_LDcvt)( long_double *, CVT_INFO *, char * );
typedef char *(*_type_EFG_fcvt)( double, int, int *, int * );

_WPRTLINK extern _type_EFG_cnvs2d __EFG_cnvs2d;
_WPRTLINK extern _type_EFG_cnvd2f __EFG_cnvd2f;
_WPRTLINK extern _type_EFG_LDcvt  __EFG_LDcvt;
_WPRTLINK extern _type_EFG_fcvt   __EFG_fcvt;

// from math library
extern "C" {
_WMRTLINK extern void  __cnvs2d( char *, double * );
_WMRTLINK extern int   __cnvd2f( double *, float * );
};

#endif
