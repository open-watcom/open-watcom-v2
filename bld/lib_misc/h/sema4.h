#ifndef _SEMA4_H_INCLUDED
#define _SEMA4_H_INCLUDED

/* Make sure these are in C linkage */
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__QNX__)
#include "semaphor.h"
#elif defined(__LINUX__)
#include <semaphore.h>
#elif defined(__RDOSDEV__)
#include "rdosdev.h"
#endif

/*
 * Note that this structure must not grow larger than 32bytes without
 * also updating the C++ runtime file prwdata.asm
 */
typedef struct  semaphore_object {
  #if defined(__NT__)
        void            *semaphore;
  #elif defined(__QNX__) || defined(__LINUX__)
        sem_t           semaphore;
  #elif defined(__RDOS__)
        int             semaphore; // RDOS only have critical sections, which should work
  #elif defined(__RDOSDEV__)
        struct TKernelSection semaphore;
  #else
        unsigned long   semaphore;
  #endif
        unsigned        initialized;
        unsigned long   owner;
        unsigned        count;
} semaphore_object;

_WCRTLINK void __AccessSemaphore( semaphore_object * );
_WCRTLINK void __ReleaseSemaphore( semaphore_object * );
_WCRTLINK void __CloseSemaphore( semaphore_object * );

// the following is for the C++ library
#if defined( _M_I86 )
    #define _AccessSemaphore( sema ) __AccessSemaphore( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSemaphore( sema )
    #define _CloseSemaphore( sema ) __CloseSemaphore( sema )
#else
    _WCRTLINK extern void (*__AccessSema4)( semaphore_object *);
    _WCRTLINK extern void (*__ReleaseSema4)( semaphore_object *);
    _WCRTLINK extern void (*__CloseSema4)( semaphore_object *);
    #define _AccessSemaphore( sema ) __AccessSema4( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSema4( sema )
    #define _CloseSemaphore( sema ) __CloseSema4( sema )
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
