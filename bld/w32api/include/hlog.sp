#ifndef _LMHLOGDEFINED_
#define _LMHLOGDEFINED_

/* Log flags */
#define LOGFLAGS_FORWARD    0x00000000L
#define LOGFLAGS_BACKWARD   0x00000001L
#define LOGFLAGS_SEEK       0x00000002L

/* Error log handle */
typedef struct _HLOG {
    DWORD   time;
    DWORD   last_flags;
    DWORD   offset;
    DWORD   rec_offset;
} HLOG;
typedef HLOG    *PHLOG;
typedef HLOG    *LPHLOG;

#endif /* _LMHLOGDEFINED_ */
