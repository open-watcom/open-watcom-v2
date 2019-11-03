:segment QNX
#if defined( _QNX_SOURCE ) || !defined( _NO_EXT_KEYS ) /* extensions enabled */
:elsesegment LINUX
#if defined( _LINUX_SOURCE ) || !defined( _NO_EXT_KEYS ) /* extensions enabled */
:elsesegment
#if !defined( _NO_EXT_KEYS ) /* extensions enabled */
:endsegment
