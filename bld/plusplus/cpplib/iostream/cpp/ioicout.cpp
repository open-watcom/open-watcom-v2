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
* Description:
*
****************************************************************************/

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <unistd.h>
#include <iostream>
#include <stdiobuf.h>
#endif
#include "ioutil.h"

#pragma initialize 20;

// This struct is just a place holder to control the initialization and
// finalization of cout
struct cout_initfini {
  cout_initfini();
 ~cout_initfini();
};

#if defined(__PENPOINT__)
#error This file has not been properly set up for global data under PENPOINT
#error It needs to ensure that the data allocated here is allocated in the
#error task specific data area under PENPOINT.
#endif

// This is the definition of the predefined stream buffer:
static stdiobuf cout_strmbuf( __get_std_stream( STDOUT_FILENO ) );

// This is the definition of the predefined stream:
namespace std {
  ostream _HUGEDATA cout( &cout_strmbuf );
}

// This will force the extra initialization and finalization
static cout_initfini __standard_io_instance_cout;

cout_initfini::cout_initfini() {
  // Ensure the correct flag settings.
  std::cout.setf( ios::unitbuf );
}

cout_initfini::~cout_initfini() {
  // Flush standard io buffers:
  std::cout.flush();
}
