/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT FILETIME -> time_t conversion routine
*
****************************************************************************/


#include <time.h>
#include <windows.h>
#include "ntext.h"


time_t __NT_filetime_to_timet( const FILETIME *ft )
{
    ULARGE_INTEGER  ulint;

    ulint.u.LowPart   =   ft->dwLowDateTime;
    ulint.u.HighPart  =   ft->dwHighDateTime;
    return( ( ulint.QuadPart - TICK_TO_UNIX_EPOCH ) / WINDOWS_TICK_PER_SEC );
}
