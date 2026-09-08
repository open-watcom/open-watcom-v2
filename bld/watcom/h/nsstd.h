/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  NSSTD macro for handling C++ std namespace for shared
*                   C and C++ code
*
****************************************************************************/


#ifndef _NSSTD_H_INCLUDED
#define _NSSTD_H_INCLUDED

#ifdef __cplusplus
#define NSSTD(x)    std::x
#else
#define NSSTD(x)    x
#endif

#endif
