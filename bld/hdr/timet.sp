#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
#define _TIME_T_DEFINED_
:segment QNX
typedef signed long time_t; /* time value */
:endsegment
:segment NOT_QNX
typedef unsigned long time_t; /* time value */
:endsegment
#endif
