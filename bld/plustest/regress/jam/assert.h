/****************************************************************************
File: assert.h

Description: Error handling functions (assertions, panic crashes)

JAM_assert(condition) - similar to the C/C++ assert() macro except
   it calls the crash handler function.  Turn off with NDEBUG or
   JAM_NDEBUG.
 
void JAM_crash(const char*, ...) - pointer to crash handler function

JAM_CrashHandler JAM_set_crash_handler(JAM_CrashHandler) - returns
   former handler function, defaults to function which prints message
   to stderr and then calls abort().

Notes:
   jam/assert.cpp defines static objects JAM_crash and JAM_error_msg.
   
   
History:
1992 Feb 24 Jam         created
1992 Mar 02 Jam         Added JAM_out_of_mem() to set as new_handler
1992 Jul 03 Jam         moved NewLog stuff to separate file

****************************************************************************/

#ifndef JAM_ASSERT_H
#define JAM_ASSERT_H

#ifdef JAM_IGASSERT  // substitute dummy inlines; don't need to link

#  include <stdlib.h>
#  include <assert.h>
   inline void JAM_crash(const char*, ...) { abort(); }
#  define JAM_set_crash_handler(x) (void)0
#  define JAM_assert(p) assert(p)

void (*const JAM_out_of_mem)() = abort;

#else    // must link jam/assert.cpp

typedef void (*JAM_CrashHandler)(const char*, ...);

extern JAM_CrashHandler JAM_crash;

extern JAM_CrashHandler JAM_set_crash_handler(JAM_CrashHandler);

extern const char* JAM_error_msg; // "JAM internal error"

#if defined(NDEBUG) || defined(JAM_NDEBUG)
#  define JAM_assert(p) ((void)0)
#else
#  define JAM_assert(p) ((p) ? (void)0 : \
          (void)JAM_crash("%s: %s,%d\n", JAM_error_msg, __FILE__, __LINE__))
#endif

// call set_new_handler(JAM_out_of_mem) because JAM classes don't test
extern void JAM_out_of_mem();   // calls JAM_crash("Out of memory");

#endif   // JAM_IGASSERT

#endif // JAM_ASSERT_H


