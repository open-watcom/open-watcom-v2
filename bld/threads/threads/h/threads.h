
#ifndef _THREADS_H_INCLUDED
#define _THREADS_H_INCLUDED

/* Needed for time_t. */
#include <time.h>

#define ONCE_FLAG_INIT xyzzy
#define TSS_DTOR_ITERATIONS xyzzy

typedef struct {
    int dummy;
} cnd_t;

typedef struct {
    int dummy;
} thrd_t;

typedef struct {
    int dummy;
} tss_t;

typedef struct {
    int dummy;
} mtx_t;

typedef void ( *tss_dtor_t )( void * );
typedef int ( *thrd_start_t )( void * );

typedef struct {
    int dummy;
} once_flag;

typedef struct {
    time_t sec;
    long   nsec;
} xtime;

enum { mtx_plain, mtx_recursive, mtx_timed, mtx_try };
enum { thrd_timeout, thrd_success, thrd_busy, thrd_error, thrd_nomem };

void   call_once( once_flag *flag, void ( *func )( void ) );

int    cnd_broadcast( cnd_t *cond );
void   cnd_destroy( cnd_t *cond );
int    cnd_init( cnd_t *cond );
int    cnd_signal( cnd_t *cond );
int    cnd_timedwait( cnd_t *cond, mtx_t *mtx, const xtime *xt );
int    cnd_wait( cnd_t *cond, mtx_t *mtx );

void   mtx_destroy( mtx_t *mtx );
int    mtx_init( mtx_t *mtx, int type );
int    mtx_lock( mtx_t *mtx );
int    mtx_timedlock( mtx_t *mtx, const xtime *xt );
int    mtx_trylock( mtx_t *mtx );
int    mtx_unlock( mtx_t *mtx );

int    thrd_create( thrd_t *thr, thrd_start_t func, void *arg );
thrd_t thrd_current( void );
int    thrd_detach( thrd_t thr );
int    thrd_equal( thrd_t thr0, thrd_t thr1 );
void   thrd_exit( int res );
int    thrd_join( thrd_t thr, int *res );
void   thrd_sleep( const xtime *xt );
void   thrd_yield( void );

int    tss_create( tss_t *key, tss_dtor_t dtor );
void   tss_delete( tss_t key );
void  *tss_get( tss_t key );
int    tss_set( tss_t key, void *val );

int    xtime_get( xtime *xt, int base );


#endif
