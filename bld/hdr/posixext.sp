:segment QNX | LINUX
#if defined(_POSIX_SOURCE) || !defined(NO_EXT_KEYS) /* extensions enabled */
:elsesegment
#if !defined(NO_EXT_KEYS) /* extensions enabled */
:endsegment
