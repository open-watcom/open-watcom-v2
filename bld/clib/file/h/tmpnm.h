#if defined( __SW_BM ) && defined( __NETWARE__ )
    #define _TMPNAMBUF      (__THREADDATAPTR->__tmpnambuf)
#else
    static CHAR_TYPE        _tmpname[L_tmpnam];

    #define _TMPNAMBUF      _tmpname
#endif
