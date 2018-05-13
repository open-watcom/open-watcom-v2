::
:: POSIX pid_t typedef declaration
::
#ifndef _PID_T_DEFINED_
 #define _PID_T_DEFINED_
:segment LINUX
 typedef int            pid_t;  /* Used for process IDs & group IDs */
:elsesegment QNX
 typedef int            pid_t;  /* Used for process IDs & group IDs */
 typedef short          mpid_t; /* Used for process & group IDs in messages */
:elsesegment
 typedef int            pid_t;  /* Used for process IDs & group IDs */
:endsegment
#endif
