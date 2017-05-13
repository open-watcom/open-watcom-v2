/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT FILETIME to time_t conversion routine
*
****************************************************************************/


#include <time.h>
#include <windows.h>
#include "ft2timet.h"


#define WINDOWS_TICK        10000000
#define SEC_TO_UNIX_EPOCH   11644473600LL


time_t __NTfiletime_to_timet( const FILETIME *ft )
{
    ULARGE_INTEGER  ulint;

    ulint.u.LowPart   =   ft->dwLowDateTime; 
    ulint.u.HighPart  =   ft->dwHighDateTime; 
    return( ulint.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH );
}
