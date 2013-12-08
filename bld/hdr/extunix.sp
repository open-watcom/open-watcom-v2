:segment QNX
#if defined(_POSIX_SOURCE) || defined(_QNX_SOURCE) || !defined(NO_EXT_KEYS) /* extensions enabled */
:elsesegment LINUX
#if defined(_POSIX_SOURCE) || defined(_LINUX_SOURCE) || !defined(NO_EXT_KEYS) /* extensions enabled */
:elsesegment
#if !defined(NO_EXT_KEYS) /* extensions enabled */
:endsegment
