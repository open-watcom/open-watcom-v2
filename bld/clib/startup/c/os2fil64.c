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
* Description:  Initialize OS/2 64-bit file function interface
*
****************************************************************************/


#include "variety.h"
#define INCL_DOSMODULEMGR
#include <wos2.h>
#include "rtinit.h"
#include "os2fil64.h"

#define ORD_DosOpenL              981
#define ORD_DosSetFileLocksL      986
#define ORD_DosSetFilePtrL        988
#define ORD_DosSetFileSizeL       989


pfn_DosOpenL            __os2_DosOpenL = NULL;
pfn_DosSetFilePtrL      __os2_DosSetFilePtrL = NULL;
pfn_DosSetFileSizeL     __os2_DosSetFileSizeL = NULL;
pfn_DosSetFileLocksL    __os2_DosSetFileLocksL = NULL;


static void __init_fileapi64( void )
{
    HDIR            handle;

    if( DosQueryModuleHandle( "DOSCALLS", &handle ) == 0 ) {
        if( DosQueryProcAddr( handle, ORD_DosOpenL, NULL, (PFN *)&__os2_DosOpenL ) == 0 ) {
            if( DosQueryProcAddr( handle, ORD_DosSetFileLocksL, NULL, (PFN *)&__os2_DosSetFileLocksL ) == 0 ) {
                if( DosQueryProcAddr( handle, ORD_DosSetFilePtrL, NULL, (PFN *)&__os2_DosSetFilePtrL ) == 0 ) {
                    if( DosQueryProcAddr( handle, ORD_DosSetFileSizeL, NULL, (PFN *)&__os2_DosSetFileSizeL ) == 0 ) {
                        return;
                    }
                    __os2_DosSetFileSizeL = NULL;
                }
                __os2_DosSetFilePtrL = NULL;
            }
            __os2_DosSetFileLocksL = NULL;
        }
        __os2_DosOpenL = NULL;
    }
}

AXI( __init_fileapi64, INIT_PRIORITY_RUNTIME )
