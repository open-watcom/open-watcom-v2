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
* Description:  Definition of __iob array.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "rtdata.h"
#include "rtinit.h"
#include "tmpfname.h"
#include "ioctrl.h"


_WCRTDATA FILE _WCDATA __iob[_NFILES] = {
    { NULL, 0, NULL, _READ,         0, 0, 0  }  /* stdin */
   ,{ NULL, 0, NULL, _WRITE,        1, 0, 0  }  /* stdout */
   ,{ NULL, 0, NULL, _WRITE,        2, 0, 0  }  /* stderr */
#if defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( __OSI__ )
   ,{ NULL, 0, NULL, _READ|_WRITE,  3, 0, 0  }  /* stdaux */
   ,{ NULL, 0, NULL, _WRITE,        4, 0, 0  }  /* stdprn */
#endif
};

__stream_link *__ClosedStreams;
__stream_link *__OpenStreams;

AXI(__InitFiles,INIT_PRIORITY_LIBRARY);
AYI(__full_io_exit,INIT_PRIORITY_LIBRARY);
