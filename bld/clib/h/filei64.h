/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Macros for switching between symbols for 64-bit and 32-bit
*                   data file function version
*
****************************************************************************/


#ifndef _FILEI64_H_INCLUDED
#define _FILEI64_H_INCLUDED

#ifdef __INT64__
    #define __64_NAME(n1,n2)     n2
#else
    #define __64_NAME(n1,n2)     n1
#endif

#endif
