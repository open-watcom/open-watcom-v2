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
* Description:  OS/2 64-bit file function interface prototypes
*
****************************************************************************/


#ifndef _OS2FIL64_H_INCLUDED
#define _OS2FIL64_H_INCLUDED

#if !defined( _M_I86 )

#define _FILEAPI64()        (__os2_DosOpenL != NULL)

typedef APIRET APIENTRY     (*pfn_DosOpenL)(PCSZ,PHFILE,PULONG,LONGLONG,ULONG,ULONG,ULONG,PEAOP2);
typedef APIRET APIENTRY     (*pfn_DosSetFileLocksL)(HFILE,PFILELOCKL,PFILELOCKL,ULONG,ULONG);
typedef APIRET APIENTRY     (*pfn_DosSetFilePtrL)(HFILE,LONGLONG,ULONG,PLONGLONG);
typedef APIRET APIENTRY     (*pfn_DosSetFileSizeL)(HFILE,LONGLONG);

extern pfn_DosOpenL         __os2_DosOpenL;
extern pfn_DosSetFileLocksL __os2_DosSetFileLocksL;
extern pfn_DosSetFilePtrL   __os2_DosSetFilePtrL;
extern pfn_DosSetFileSizeL  __os2_DosSetFileSizeL;

#endif

#endif
