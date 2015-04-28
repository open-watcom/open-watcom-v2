:: need idempotency lock as this bit might be included multiple times
#ifndef _ERRNO_DEFINED
#define _ERRNO_DEFINED
 _WCRTLINK extern int *__get_errno_ptr( void );
 #ifndef errno
  #define errno (*__get_errno_ptr())
 #else
::  cannot be made volatile because users are allowed to
::  declare "extern int errno;" in their code in the presence
::  of <errno.h>
  _WCRTDATA extern int errno;
 #endif
#endif
