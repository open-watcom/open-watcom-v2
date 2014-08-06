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


#ifndef WRESRTNS_INLCUDED
#define WRESRTNS_INLCUDED

#define WRESOPEN(n,m,a)     WResRtns.open(n,m,a)
#define WRESCLOSE(fh)       WResRtns.close(fh)
#define WRESWRITE(fh,b,s)   WResRtns.write(fh,b,s)
#define WRESREAD(fh,b,s)    WResRtns.read(fh,b,s)
#define WRESSEEK(fh,o,d)    WResRtns.seek(fh,o,d)
#define WRESTELL(fh)        WResRtns.tell(fh)
#define WRESALLOC           WResRtns.alloc
#define WRESFREE            WResRtns.free

#define WRESHANDLE          hInstance->handle

#define WRESIOERR(fh,rc)    (rc == -1)

#endif
