#ifndef TIMECODE_DEFINED
#define TIMECODE_DEFINED

/* Time code */
typedef union _timecode {
    struct {
        WORD    wFrameRate;
        WORD    wFrameFract;
        DWORD   dwFrames;
    };
    DWORDLONG   qw;
} TIMECODE;
typedef TIMECODE    *PTIMECODE;

/* Time code sample */
typedef struct tagTIMECODE_SAMPLE {
    LONGLONG    qwTick;
    TIMECODE    timecode;
    DWORD       dwUser;
    DWORD       dwFlags;
} TIMECODE_SAMPLE;
typedef TIMECODE_SAMPLE *PTIMECODE_SAMPLE;

#endif /* TIMECODE_DEFINED */
