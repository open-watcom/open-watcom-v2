#ifdef __cplusplus
extern "C" {
#endif

extern unsigned far __doserror_( unsigned );
#pragma aux __doserror_ "*"

extern unsigned __dos_close( unsigned handle );
#ifdef _M_I86
#pragma aux __dos_close = \
        "call far ptr __doserror_" \
        parm caller     [bx] \
        modify exact    [ax bx];
#else
#pragma aux __dos_close = \
        "call far ptr __doserror_" \
        parm caller     [ebx] \
        modify exact    [eax ebx];
#endif

extern unsigned __dos_commit( unsigned handle );
#ifdef _M_I86
#pragma aux __dos_commit = \
        "clc"           \
        "call __doserror_" \
        parm caller     [bx] \
        modify exact    [ax bx];
#else
#pragma aux __dos_commit = \
        "clc"           \
        "call __doserror_" \
        parm caller     [ebx] \
        modify exact    [eax ebx];
#endif

unsigned _dos_close( int handle )
{
    return( __dos_close( handle ) );
}

unsigned _dos_commit( int handle )
{
    return( __dos_commit( handle ) );
}

#ifdef __cplusplus
}
#endif
