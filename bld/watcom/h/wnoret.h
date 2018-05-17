/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Macros to declare and handling no returning functions.
*
****************************************************************************/


#if defined( _WCNORETURN )
#define NO_RETURN(x)        _WCNORETURN x
#define NO_RETURN_FAKE(x)
#elif defined( __GNUC__ ) || defined( __clang__ )
#define NO_RETURN(x)        x  __attribute__ ((__noreturn__))
#define NO_RETURN_FAKE(x)
#elif defined( _MSC_VER )
#define NO_RETURN(x)        __declspec(noreturn)  x
#define NO_RETURN_FAKE(x)
#else
#define NO_RETURN(x)        x
#define NO_RETURN_FAKE(x)   x
#endif
