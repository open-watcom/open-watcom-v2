:segment QNX
#ifndef MB_CUR_MAX
    #define MB_CUR_MAX  6
#endif
#ifndef MB_LEN_MAX
    #define MB_LEN_MAX  6
#endif
:elsesegment
#ifndef MB_CUR_MAX
    #define MB_CUR_MAX  2
#endif
#ifndef MB_LEN_MAX
    #define MB_LEN_MAX  2
#endif
:endsegment
