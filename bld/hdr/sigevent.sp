:segment LINUX | QNX | RDOS
:segment LINUX
#define SIGEV_SIGNAL    0
#define SIGEV_NONE      1
#define SIGEV_THREAD    2

:endsegment
:segment RDOS
#define SIGEV_NONE      1
#define SIGEV_THREAD    2

union sigval {
    int     sival_int;
    void    *sival_ptr;
};

:endsegment
struct sigevent {
    int          sigev_signo;
    union sigval sigev_value;
    int          sigev_notify;
};
struct msigevent {
    long         sigev_signo;
    union sigval sigev_value;
:segment LINUX | RDOS
    int          sigev_notify;
:endsegment
};
:endsegment
