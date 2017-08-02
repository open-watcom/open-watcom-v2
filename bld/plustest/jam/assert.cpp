/****************************************************************************
File: assert.cpp

Description:
   Error handling (assertions, panic crashes) and free store validity
   verification.

JAM_assert(condition) - similar to the C/C++ assert() macro except
   it calls the crash handler function.  Turn off with NDEBUG or
   JAM_NDEBUG.
 
void JAM_crash(const char*, ...) - pointer to crash handler function

JAM_CrashHandler JAM_set_crash_handler(JAM_CrashHandler) - returns
   format handler, defaults to function which prints message to stderr
   and then calls abort().

void* operator new(size_t) and operator delete(void*) - these maintain
   list of allocated pointers to make sure not deleting object twice and
   to catch memory leaks.

Notes:
   Defines static objects JAM_crash and JAM_error_msg.
   
   
History:
1992 Feb 24 Jam         created
1992 Mar 24 Jam         removed conditional compilation based on
                        USEJAMASSERT and WRITENEWLOG -- don't link this
                        file in if you don't want the stuff
1992 Jul 03 Jam         moved NewLog stuff to separate file
1992 Sep 28 Jam         renamed and changed #includes to <jam/*.h>
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef JAM_USEJAMASSERT
#define JAM_USEJAMASSERT
#endif
#include <jam/assert.h>

const char* JAM_error_msg = "JAM internal error";

static void JAM_default_crash_handler(const char* format, ...)
{
        va_list args;
        va_start(args, format);
   vfprintf(stderr, format, args);
   fprintf(stderr, "\n");
   abort();
}

JAM_CrashHandler JAM_crash = JAM_default_crash_handler;

JAM_CrashHandler JAM_set_crash_handler(JAM_CrashHandler newhandler)
{
   if (newhandler==0) return 0;
   JAM_CrashHandler old = JAM_crash;
   JAM_crash = newhandler;
   return old;
}

void JAM_out_of_mem()
{
   putchar('\a');
   JAM_crash("Out of memory");   // why aren't you using a real os?! :-)
}

