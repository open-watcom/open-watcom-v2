
#ifndef _error_h_
#define _error_h_





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




#include "base/defs.h"


// This class is an encapsulation of a simple error-handling
// mechanism. The Warning and Fatal methods accept parameters in the
// same style as printf does; thus, for example, a user of this class
// may say:
// \par{\small \begin{verbatim}
//          CL_Error::Warning ("Bad array bound %d limit %d", i, n);
//\end{verbatim}
// }\par
// The Warning method formats the string, and then invokes the warning
// handler with the formatted string. The default warning handler
// simply prints out the message.
//
// The Fatal handler behaves similarly; it invokes the fatal-error
// handler, and exits the program if the latter returns TRUE.
//
// The SetWarningHandler and SetFatalHandler methods can be used to
// provide user-specific warning and fatal handlers instead of the
// default ones. These methods return the previous values for their handlers.


#ifdef __GNUC__
#pragma interface
#endif

typedef bool (*CL_ErrorHandler) (const char* message);


class __CLASSTYPE CL_Error {

public:

    static void Warning (const char *fmt, ...);
    // Issue a warning message, using the current warning handler.

    static void Fatal   (const char *fmt, ...);

    static CL_ErrorHandler SetWarningHandler    (CL_ErrorHandler handler);

    static CL_ErrorHandler SetFatalErrorHandler (CL_ErrorHandler handler);
    
};

#endif
