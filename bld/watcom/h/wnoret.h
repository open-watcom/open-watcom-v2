/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Macro to declare no returning function.
*
****************************************************************************/


#if defined( _WCNORETURN )
#define NO_RETURN(x)    _WCNORETURN x
#elif defined( __GNUC__ ) || defined( __clang__ )
#define NO_RETURN(x)    x  __attribute__ ((__noreturn__))
#elif defined( _MSC_VER )
#define NO_RETURN(x)    __declspec(noreturn)  x
#else
#define NO_RETURN(x)    x
#endif
