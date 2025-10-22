/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT FILETIME <-> time_t conversion routines
*
****************************************************************************/


#define WINDOWS_TICK_PER_SEC    10000000LLU
#define TICK_TO_UNIX_EPOCH      116444736000000000LLU
#define SEC_TO_UNIX_EPOCH       11644473600LLU

extern time_t   __NT_filetime_to_timet( const FILETIME *ft );
extern void     __NT_timet_to_filetime( time_t t, FILETIME *ft );
