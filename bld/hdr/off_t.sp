::
:: POSIX off_t typedef declaration
:: off64_t is not part of the POSIX standard; it is LFS extension.
::
#ifndef _OFF_T_DEFINED_
 #define _OFF_T_DEFINED_
:segment LINUX
 typedef long           off_t;   /* Used for file sizes, offsets     */
:elsesegment QNX
 typedef long           off_t;   /* Used for file sizes, offsets     */
:elsesegment RDOS
 typedef long long      off_t;   /* Used for file sizes, offsets     */
:elsesegment
 typedef long           off_t;   /* file offset value */
:endsegment
:include ext.sp
 typedef long long      off64_t;
:include extepi.sp
#endif
