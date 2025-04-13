:segment QNX
#if defined( _POSIX_SOURCE ) || defined( _QNX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 201112L /* extensions enabled or C11 */
:elsesegment LINUX
#if defined( _POSIX_SOURCE ) || defined( _LINUX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 201112L /* extensions enabled or C11 */
:elsesegment
#if defined( _POSIX_SOURCE ) || !defined( _NO_EXT_KEYS ) || __STDC_VERSION__ >= 201112L /* extensions enabled or C11 */
:endsegment
