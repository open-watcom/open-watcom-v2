/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT FILETIME <-> time_t conversion routines
*
****************************************************************************/


#define WINDOWS_TICK        10000000LL
#define SEC_TO_UNIX_EPOCH   11644473600LL

extern time_t   __NT_filetime_to_timet( const FILETIME *ft );
extern void     __NT_timet_to_filetime( time_t t, FILETIME *ft );
