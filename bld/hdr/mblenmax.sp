:segment LINUX | QNX
#ifndef MB_LEN_MAX
 #define MB_LEN_MAX     6
#endif
:elsesegment
#ifndef MB_LEN_MAX
 #define MB_LEN_MAX     2
#endif
:endsegment
