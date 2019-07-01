/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Windows NT FILETIME <-> DOS time/date conversion routines
*
****************************************************************************/


extern void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t );
extern void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp );
