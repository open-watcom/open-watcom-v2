:segment LINUX
#ifndef _OFF_T_DEFINED_
 #define _OFF_T_DEFINED_
 typedef long           off_t;  /* Used for file sizes, offsets     */
#endif
:elsesegment QNX
typedef long            off_t;  /* Used for file sizes, offsets     */
:elsesegment
typedef long            off_t;  /* file offset value */
:endsegment
