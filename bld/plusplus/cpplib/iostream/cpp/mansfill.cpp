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
//  92/01/27    Steve McDowell  Initial implementation.
//  92/09/08    Greg Bentz      Cleanup.
//  93/04/01    Greg Bentz      Add template support, conditionally removed.
//  93/10/29    Raymond Tang    Split into separate files.
//  94/01/26    Greg Bentz      switch to use of template support
//  94/04/06    Greg Bentz      combine header files
//  95/05/05    Greg Bentz      use _HUGEDATA
//  95/05/18    Greg Bentz      runtime dll linkage

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iomanip.h>
#include <iostream.h>
#endif

// Note that each manipulator function takes a single parameter called "__p".
// This name is defined in the SMANIP_make macro definition.
// Each function creates an object of type SMANIPint or SMANIPlong.
// It copies "__p" into the "__p" member of the returned object,
// and copies a function pointer into the "__f" member.
// Later, when the returned object is operated upon by << or >>, an indirect
// call is made via the stored function pointer, passing "__p" as a value.
// This called function is the one that actually changes the state of the
// stream.

static ios &__setfill( ios &strm, int fill ) {
/********************************************/
// Handles "setfill" manipulator.
    strm.fill( (char)fill );
    return( strm );
}

#ifdef MACRO_IOMANIP
SMANIP_make( int, setfill ) {
/***************************/
// SMANIPint setfill( int __p )
//      Set the fill character used for output conversion of integers.
    return SMANIPint( __setfill, __p );
}
#endif


#ifdef __SIMPLIFY__
smanip<int> setfill( int parm ) {
/*******************************/
    return smanip<int>( __setfill, parm );
}
#else
#pragma initialize 22; // just after the rest of the C++ library
_WPRTLINK sapp<int> _HUGEDATA setfill( __setfill );
#endif
