#ifndef errno
#define errno (*__get_errno_ptr())
#endif
:: cannot be made volatile because users are allowed to
:: declare "extern int errno;" in their code in the presence
:: of <errno.h>
_WCRTLINK extern int errno;
