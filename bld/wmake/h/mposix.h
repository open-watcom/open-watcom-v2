/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  POSIX functions wrapper macros
*
****************************************************************************/


#ifdef _WIN64

#define posix_read      __w64_read
#define posix_write     __w64_write

#else

#define posix_read      read
#define posix_write     write

#endif
