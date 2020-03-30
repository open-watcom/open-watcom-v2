:segment QNX
#if defined( _POSIX_SOURCE ) || defined( _QNX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 199901L /* extensions enabled or C99 */
:elsesegment LINUX
#if defined( _POSIX_SOURCE ) || defined( _LINUX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 199901L /* extensions enabled or C99 */
:elsesegment
#if defined( _POSIX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 199901L /* extensions enabled or C99 */
:endsegment
