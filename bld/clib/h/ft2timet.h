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


extern time_t __NTfiletime_to_timet( const FILETIME *ft );
