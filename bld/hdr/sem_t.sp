::
:: POSIX sem_t typedef declaration
::
#ifndef _SEM_T_DEFINED_
 #define _SEM_T_DEFINED_
 typedef struct {
     volatile int futex;
     volatile int value;
 } sem_t;
#endif
