// For multi-thread support.
#ifndef __lock_it

class __lock;

#ifdef __SW_BM
class _WPRTLINK __get_lock {
public:
    __get_lock( __lock * );
    ~__get_lock();
private:
    __lock *__lk;
};
#define __lock_it( __l ) __get_lock __lock_name( __LINE__ )( __l )
#define __lock_name( __ln ) __lock_glue( __lock__, __ln )
#define __lock_glue( __pre, __lin ) __pre ## __lin
#else
#define __lock_it( __l )
#endif
#endif
