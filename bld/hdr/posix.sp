:segment QNX
#if defined(_POSIX_SOURCE) || !defined(NO_EXT_KEYS) /*extensions enabled*/
:elsesegment
:include ext.sp
:endsegment
