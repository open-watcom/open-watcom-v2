:segment LINUX
#ifndef _PID_T_DEFINED_
 #define _PID_T_DEFINED_
 typedef int            pid_t;  /* Used for process IDs & group IDs */
#endif
:elsesegment QNX
typedef int             pid_t;  /* Used for process IDs & group IDs */
typedef short           mpid_t; /* Used for process & group IDs in messages */
:elsesegment
typedef int             pid_t;  /* Used for process IDs & group IDs */
:endsegment
