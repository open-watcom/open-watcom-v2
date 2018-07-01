::
:: POSIX Threads typedef declaration 1
::
:segment LINUX
#ifndef _PTHREAD_T_DEFINED_
 #define _PTHREAD_T_DEFINED_
 typedef pid_t          pthread_t;
#endif
#ifndef _PTHREAD_ATTR_T_DEFINED_
 #define _PTHREAD_ATTR_T_DEFINED_
 typedef struct {
     __w_size_t         stack_size;
     void               *stack_addr;
     int                detached;
     int                sched_policy;
     int                sched_inherit;
     struct sched_param *sched_params;
 } pthread_attr_t;
#endif
:elsesegment QNX
:elsesegment
:endsegment
