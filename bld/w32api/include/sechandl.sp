/* Security handle */
#ifndef _SECHANDLE_DEFINED
#define _SECHANDLE_DEFINED
typedef struct _SecHandle {
    ULONG_PTR   dwLower;
    ULONG_PTR   dwUpper;
} SecHandle;
typedef SecHandle   *PSecHandle;
#endif
