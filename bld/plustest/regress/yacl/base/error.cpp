



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */




#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdarg.h>

#include "base/error.h"

#ifdef __GNUC__
#pragma implementation
#endif



static bool YACLWarnFunction (const char* message)
{
    cerr << "YACL: " << message << endl << flush;
    return TRUE;
}

static CL_ErrorHandler warnPtr  = YACLWarnFunction,
                       fatalPtr = YACLWarnFunction;

void CL_Error::Warning (const char *fmt, ...)
{
    va_list args;
    char msg[200];

    va_start (args, fmt);
    vsprintf (msg, fmt,  args);
    (*warnPtr) (msg); 
}



void CL_Error::Fatal (const char *fmt, ...)
{
    va_list args;
    char msg[200];

    va_start (args, fmt);
    vsprintf (msg, fmt,  args);
    if ((*fatalPtr) (msg))
        exit (1);
}



CL_ErrorHandler CL_Error::SetWarningHandler    (CL_ErrorHandler
                                                handler)
{
    if (!handler)
        return warnPtr;
    CL_ErrorHandler h = warnPtr;
    warnPtr = handler;
    return h;
}


CL_ErrorHandler CL_Error::SetFatalErrorHandler (CL_ErrorHandler handler)
{
    if (!handler)
        return fatalPtr;
    CL_ErrorHandler h = fatalPtr;
    fatalPtr = handler;
    return h;
}



