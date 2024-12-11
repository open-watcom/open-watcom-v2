/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS clib internal functions declaration
*
****************************************************************************/

extern int __create_timer_thread(void (*Start)(void *Args, void *Param), void *Args);
#pragma aux __create_timer_thread "*" __parm [__edx] [__eax] __value [__eax]

extern int __wait_timer_event();
#pragma aux __wait_timer_event "*"  __value [__eax]

