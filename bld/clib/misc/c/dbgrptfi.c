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


#include "variety.h"
#define _DEBUG                  /* so prototypes are included */
#include <crtdbg.h>
#ifdef __NT__
    #include <windows.h>
#else
    #ifdef __UNIX__
        #include <unistd.h>
    #else
        #include <io.h>
    #endif
#endif
#include "dbgdata.h"


_WCRTLINK _HFILE _CrtSetReportFile( int reporttype, _HFILE reportfile )
/*********************************************************************/
{
    _HFILE              prevfile;
    int                 reportmode;

    /*** Ensure reporttype is valid ***/
    if( reporttype < _CRT_WARN  ||  reporttype > _CRT_ASSERT ) {
        return( _CRTDBG_HFILE_ERROR );
    }

    /*** Ok, now do something useful ***/
    prevfile = __DbgReportFiles[reporttype];
    if( reportfile != _CRTDBG_REPORT_FILE ) {
        if( reportfile == _CRTDBG_FILE_STDOUT ) {
            #ifdef __NT__
                __DbgReportFiles[reporttype] = GetStdHandle( STD_OUTPUT_HANDLE );
            #else
                __DbgReportFiles[reporttype] = STDOUT_FILENO;
            #endif
        } else if( reportfile == _CRTDBG_FILE_STDERR ) {
            #ifdef __NT__
                __DbgReportFiles[reporttype] = GetStdHandle( STD_ERROR_HANDLE );
            #else
                __DbgReportFiles[reporttype] = STDERR_FILENO;
            #endif
        } else {
            __DbgReportFiles[reporttype] = reportfile;
        }

        reportmode = _CrtSetReportMode( reporttype, _CRTDBG_REPORT_MODE );
        _CrtSetReportMode( reporttype, reportmode|_CRTDBG_MODE_FILE );
    }

    return( prevfile );
}
