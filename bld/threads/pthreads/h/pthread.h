
#ifndef _PTHREAD_H_INCLUDED
#define _PTHREAD_H_INCLUDED

#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#ifdef _M_IX86
#pragma pack( __push, 1 )
#else
#pragma pack( __push, 8 )
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_THREADS_MAX 16

    typedef int pthread_t;

    typedef struct {
        int dummy;
    } pthread_attr_t;

    _WCRTLINK extern void _pthread_initialize( );
    _WCRTLINK extern void _pthread_cleanup( );

    _WCRTLINK extern int pthread_create
        ( pthread_t      *thread_id,
          pthread_attr_t *attributes,
          void           *(*thread_function)( void * ),
          void           *argument );

    _WCRTLINK extern int pthread_join( pthread_t thread_id, void **result );

#ifdef __cplusplus
}
#endif

#pragma pack( __pop )

#endif
