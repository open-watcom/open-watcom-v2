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
* Description:  Determine the flavour du jour of editor.
*
****************************************************************************/


#include "vi.h"

#if defined( __WINDOWS_386__ )
  #define TT "vi/win386"
#elif defined( __WINDOWS__ )
  #define TT "vi/win"
#elif defined( __OS2V2__ )
  #define TT "vi/os2v2"
#elif defined( __OS2__ )
  #define TT "vi/os2"
#elif defined( __NT__ )
  #if defined( __WIN__ )
    #define TT "vi/ntwin"
  #else
    #define TT "vi/nt"
  #endif
#elif defined( __PHAR__ ) || defined( __4G__ )
  #define TT "vi/386"
#elif defined( __QNX__ )
  #define TT "vi/qnx"
#elif defined( __LINUX__ )
  #define TT "vi/linux"
#elif defined( __UNIX__ )
  #define TT "vi/unix"
#elif defined( __286__ )
  #define TT "vi/286"
#else
  #define TT "vi/86"
#endif

char _NEAR TITLE[] = TT;
