#ifdef __cplusplus
extern "C" {
#endif

extern void TimerOn( void );
extern void TimerOff( void );
extern double TimerElapsed( void );

typedef __int64 p5_time;

typedef enum {
    T_STATE_STARTED,
    T_STATE_STOPPED
} timer_state;

typedef struct user_timer {
    p5_time	start;
    p5_time	stop;
    unsigned	old_priority;
    timer_state	state;
} user_timer;

// extern void UserTimerInit( user_timer * );
extern void UserTimerOn( user_timer * );
extern void UserTimerOff( user_timer * );
extern double UserTimerElapsed( user_timer * );

#ifdef __cplusplus
};
#endif
