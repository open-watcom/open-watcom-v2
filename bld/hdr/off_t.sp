::
:: POSIX off_t typedef declaration
::
#ifndef _OFF_T_DEFINED_
 #define _OFF_T_DEFINED_
:segment LINUX
 typedef long           off_t;  /* Used for file sizes, offsets     */
:elsesegment QNX
 typedef long           off_t;  /* Used for file sizes, offsets     */
:elsesegment
 typedef long           off_t;  /* file offset value */
:endsegment
#endif
