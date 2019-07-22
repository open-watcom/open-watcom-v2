/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2019 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT time_t -> FILETIME conversion routine
*
****************************************************************************/


#include <time.h>
#include <windows.h>
#include "ntext.h"


void __NT_timet_to_filetime( time_t t, FILETIME *ft )
{
    ULARGE_INTEGER  ulint;

    ulint.QuadPart = ( t + SEC_TO_UNIX_EPOCH ) * WINDOWS_TICK;
    ft->dwLowDateTime = ulint.u.LowPart;
    ft->dwHighDateTime = ulint.u.HighPart;
}
