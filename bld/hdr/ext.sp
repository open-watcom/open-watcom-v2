:segment QNX
#if defined(_QNX_SOURCE) || !defined(NO_EXT_KEYS) /*extensions enabled*/
:elsesegment PENPOINT
#if defined(_PP_SOURCE) || !defined(NO_EXT_KEYS)
:elsesegment
#if !defined(NO_EXT_KEYS) /* extensions enabled */
:endsegment
