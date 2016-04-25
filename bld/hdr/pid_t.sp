:segment LINUX
#ifndef _PID_T_DEFINED_
 #define _PID_T_DEFINED_
 typedef int            pid_t;  /* Used for process IDs & group IDs */
#endif
:elsesegment
typedef int             pid_t;  /* Used for process IDs & group IDs */
:endsegment
