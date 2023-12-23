::
:: Linux off64_t typedef declaration
::
#ifndef _OFF64_T_DEFINED_
 #define _OFF64_T_DEFINED_
:segment LINUX
 typedef long long      off64_t;    /* Used for file sizes, offsets     */
:elsesegment QNX
 typedef long long      off64_t;    /* Used for file sizes, offsets     */
:elsesegment
 typedef long long      off64_t;    /* file offset value */
:endsegment
#endif
