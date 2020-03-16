typedef struct {
        unsigned long sig[2];
} sigset_t;   /* Used for signal sets             */

struct sigaction {
    void        (*sa_handler)(int);
    unsigned long sa_flags;
    void        (*sa_restorer)(void);
    sigset_t    sa_mask;
};

struct _sigaction {
    void        (*sa_handler)(int);
    unsigned long sa_flags;
    void        (*sa_restorer)(void);
    sigset_t sa_mask;
};

union sigval {
    int         sigval_int;
    void        *sigval_ptr;
};

struct sigevent {
    int          sigev_signo;
    union sigval sigev_value;
};

struct msigevent {
    long         sigev_signo;
    union sigval sigev_value;
};
