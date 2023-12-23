/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  Linux system calls interface - PPC specifics.
*
****************************************************************************/


/* On PPC, the system call number is specified in r0.
 * There is a maximum of 6 integer parameters to a syscall, passed in r3-r8.
 * For the sc instruction, both a value and an error condition are returned.
 * cr0.SO is the error condition, and r3 is the return value.
 * When cr0.SO is clear, the syscall succeeded and r3 is the return value.
 * When cr0.SO is set, the syscall failed and r3 is the error value (that
 * normally corresponds to errno).
 *
 * Return value or error number are both returned in
 * register v0 (regular function return register). Register a3
 * (fourth argument register) specifies whether the return value was an
 * error or not: zero a3 means valid return value, nonzero a3 means error
 * number. Don't look at me, I didn't invent this.
 *
 * To fix this 64-bit value is used for returning from sys_call.. routines.
 * High 32-bit contains value/error flag and low 32-bit contains value.
 */

#include "rterrno.h"

typedef unsigned long long      syscall_res;

/* macros to access sys_call.. routines return/error value */

#define __syscall_iserror( res )    ((res)>>32)
#define __syscall_errno( res )      ((res)&0xFFFFFFFF)
#define __syscall_val( type, res )  ((type)((res)&0xFFFFFFFF))

#define __syscall_retcode( res, val )                   \
    if( __syscall_iserror( res ) & (1 << 28)) {         \
        _RWD_errno = __syscall_errno( res );            \
        res = (syscall_res)(val);                       \
    }

#define __syscall_return( type, res )                   \
    __syscall_retcode( res, -1 )                        \
    return( __syscall_val( type, res ) );

#define __syscall_return_pointer( type, res )           \
    __syscall_retcode( res, 0 )                         \
    return( __syscall_val( type, res ) );

/*
 * Linux system call numbers
 */

#define SYS_restart_syscall         0
#define SYS_exit                    1
#define SYS_fork                    2   /* ppc */
#define SYS_read                    3
#define SYS_write                   4
#define SYS_open                    5
#define SYS_close                   6
#define SYS_waitpid                 7
#define SYS_creat                   8
#define SYS_link                    9
#define SYS_unlink                  10
#define SYS_execve                  11
#define SYS_chdir                   12
#define SYS_time                    13
#define SYS_mknod                   14
#define SYS_chmod                   15
#define SYS_lchown                  16
#define SYS_break                   17
#define SYS_oldstat                 18
#define SYS_lseek                   19
#define SYS_getpid                  20
#define SYS_mount                   21
#define SYS_umount                  22
#define SYS_setuid                  23
#define SYS_getuid                  24
#define SYS_stime                   25
#define SYS_ptrace                  26
#define SYS_alarm                   27
#define SYS_oldfstat                28
#define SYS_pause                   29
#define SYS_utime                   30
#define SYS_stty                    31
#define SYS_gtty                    32
#define SYS_access                  33
#define SYS_nice                    34
#define SYS_ftime                   35
#define SYS_sync                    36
#define SYS_kill                    37
#define SYS_rename                  38
#define SYS_mkdir                   39
#define SYS_rmdir                   40
#define SYS_dup                     41
#define SYS_pipe                    42
#define SYS_times                   43
#define SYS_prof                    44
#define SYS_brk                     45
#define SYS_setgid                  46
#define SYS_getgid                  47
#define SYS_signal                  48
#define SYS_geteuid                 49
#define SYS_getegid                 50
#define SYS_acct                    51
#define SYS_umount2                 52
#define SYS_lock                    53
#define SYS_ioctl                   54
#define SYS_fcntl                   55
#define SYS_mpx                     56
#define SYS_setpgid                 57
#define SYS_ulimit                  58
#define SYS_oldolduname             59
#define SYS_umask                   60
#define SYS_chroot                  61
#define SYS_ustat                   62
#define SYS_dup2                    63
#define SYS_getppid                 64
#define SYS_getpgrp                 65
#define SYS_setsid                  66
#define SYS_sigaction               67
#define SYS_sgetmask                68
#define SYS_ssetmask                69
#define SYS_setreuid                70
#define SYS_setregid                71
#define SYS_sigsuspend              72
#define SYS_sigpending              73
#define SYS_sethostname             74
#define SYS_setrlimit               75
#define SYS_getrlimit               76
#define SYS_getrusage               77
#define SYS_gettimeofday            78
#define SYS_settimeofday            79
#define SYS_getgroups               80
#define SYS_setgroups               81
#define SYS_select                  82      /* ppc */
#define SYS_symlink                 83
#define SYS_oldlstat                84
#define SYS_readlink                85
#define SYS_uselib                  86
#define SYS_swapon                  87
#define SYS_reboot                  88
#define SYS_readdir                 89
#define SYS_mmap                    90
#define SYS_munmap                  91
#define SYS_truncate                92
#define SYS_ftruncate               93
#define SYS_fchmod                  94
#define SYS_fchown                  95
#define SYS_getpriority             96
#define SYS_setpriority             97
#define SYS_profil                  98
#define SYS_statfs                  99
#define SYS_fstatfs                 100
#define SYS_ioperm                  101
#define SYS_socketcall              102
#define SYS_syslog                  103
#define SYS_setitimer               104
#define SYS_getitimer               105
#define SYS_stat                    106
#define SYS_lstat                   107
#define SYS_fstat                   108
#define SYS_olduname                109
#define SYS_iopl                    110
#define SYS_vhangup                 111
#define SYS_idle                    112
#define SYS_vm86                    113
#define SYS_wait4                   114
#define SYS_swapoff                 115
#define SYS_sysinfo                 116
#define SYS_ipc                     117
#define SYS_fsync                   118
#define SYS_sigreturn               119
#define SYS_clone                   120     /* ppc */
#define SYS_setdomainname           121
#define SYS_uname                   122
#define SYS_modify_ldt              123
#define SYS_adjtimex                124
#define SYS_mprotect                125
#define SYS_sigprocmask             126
#define SYS_create_module           127
#define SYS_init_module             128
#define SYS_delete_module           129
#define SYS_get_kernel_syms         130
#define SYS_quotactl                131
#define SYS_getpgid                 132
#define SYS_fchdir                  133
#define SYS_bdflush                 134
#define SYS_sysfs                   135
#define SYS_personality             136
#define SYS_afs_syscall             137
#define SYS_setfsuid                138
#define SYS_setfsgid                139
#define SYS__llseek                 140
#define SYS_getdents                141
#define SYS__newselect              142
#define SYS_flock                   143
#define SYS_msync                   144
#define SYS_readv                   145
#define SYS_writev                  146
#define SYS_getsid                  147
#define SYS_fdatasync               148
#define SYS__sysctl                 149
#define SYS_mlock                   150
#define SYS_munlock                 151
#define SYS_mlockall                152
#define SYS_munlockall              153
#define SYS_sched_setparam          154
#define SYS_sched_getparam          155
#define SYS_sched_setscheduler      156
#define SYS_sched_getscheduler      157
#define SYS_sched_yield             158
#define SYS_sched_get_priority_max  159
#define SYS_sched_get_priority_min  160
#define SYS_sched_rr_get_interval   161
#define SYS_nanosleep               162
#define SYS_mremap                  163
#define SYS_setresuid               164
#define SYS_getresuid               165
#define SYS_query_module            166
#define SYS_poll                    167
#define SYS_nfsservctl              168
#define SYS_setresgid               169
#define SYS_getresgid               170
#define SYS_prctl                   171
#define SYS_rt_sigreturn            172
#define SYS_rt_sigaction            173
#define SYS_rt_sigprocmask          174
#define SYS_rt_sigpending           175
#define SYS_rt_sigtimedwait         176
#define SYS_rt_sigqueueinfo         177
#define SYS_rt_sigsuspend           178
#define SYS_pread64                 179
#define SYS_pwrite64                180
#define SYS_chown                   181
#define SYS_getcwd                  182
#define SYS_capget                  183
#define SYS_capset                  184
#define SYS_sigaltstack             185
#define SYS_sendfile                186
#define SYS_getpmsg                 187
#define SYS_putpmsg                 188
#define SYS_vfork                   189     /* ppc */
#define SYS_ugetrlimit              190
#define SYS_readahead               191
#define SYS_mmap2                   192
#define SYS_truncate64              193
#define SYS_ftruncate64             194
#define SYS_stat64                  195
#define SYS_lstat64                 196
#define SYS_fstat64                 197
#define SYS_pciconfig_read          198
#define SYS_pciconfig_write         199
#define SYS_pciconfig_iobase        200
#define SYS_multiplexer             201
#define SYS_getdents64              202
#define SYS_pivot_root              203
#define SYS_fcntl64                 204
#define SYS_madvise                 205
#define SYS_mincore                 206
#define SYS_gettid                  207
#define SYS_tkill                   208
#define SYS_setxattr                209
#define SYS_lsetxattr               210
#define SYS_fsetxattr               211
#define SYS_getxattr                212
#define SYS_lgetxattr               213
#define SYS_fgetxattr               214
#define SYS_listxattr               215
#define SYS_llistxattr              216
#define SYS_flistxattr              217
#define SYS_removexattr             218
#define SYS_lremovexattr            219
#define SYS_fremovexattr            220
#define SYS_futex                   221
#define SYS_sched_setaffinity       222
#define SYS_sched_getaffinity       223
//#define SYS_ni_syscall              224
#define SYS_tuxcall                 225
#define SYS_sendfile64              226
#define SYS_io_setup                227
#define SYS_io_destroy              228
#define SYS_io_getevents            229
#define SYS_io_submit               230
#define SYS_io_cancel               231
#define SYS_set_tid_address         232
#define SYS_fadvise64               233
#define SYS_exit_group              234
#define SYS_lookup_dcookie          235
#define SYS_epoll_create            236
#define SYS_epoll_ctl               237
#define SYS_epoll_wait              238
#define SYS_remap_file_pages        239
#define SYS_timer_create            240
#define SYS_timer_settime           241
#define SYS_timer_gettime           242
#define SYS_timer_getoverrun        243
#define SYS_timer_delete            244
#define SYS_clock_settime           245
#define SYS_clock_gettime           246
#define SYS_clock_getres            247
#define SYS_clock_nanosleep         248
#define SYS_swapcontext             249
#define SYS_tgkill                  250
#define SYS_utimes                  251
#define SYS_statfs64                252
#define SYS_fstatfs64               253
#define SYS_fadvise64_64            254     /* ppc */
#define SYS_rtas                    255
#define SYS_sys_debug_setcontext    256
//#define SYS_ni_syscall              257
#define SYS_migrate_pages           258
#define SYS_mbind                   259
#define SYS_get_mempolicy           260
#define SYS_set_mempolicy           261
#define SYS_mq_open                 262
#define SYS_mq_unlink               263
#define SYS_mq_timedsend            264
#define SYS_mq_timedreceive         265
#define SYS_mq_notify               266
#define SYS_mq_getsetattr           267
#define SYS_kexec_load              268
#define SYS_add_key                 269
#define SYS_request_key             270
#define SYS_keyctl                  271
#define SYS_waitid                  272
#define SYS_ioprio_set              273
#define SYS_ioprio_get              274
#define SYS_inotify_init            275
#define SYS_inotify_add_watch       276
#define SYS_inotify_rm_watch        277
#define SYS_spu_run                 278
#define SYS_spu_create              279
#define SYS_pselect6                280
#define SYS_ppoll                   281
#define SYS_unshare                 282
#define SYS_splice                  283
#define SYS_tee                     284
#define SYS_vmsplice                285
#define SYS_openat                  286
#define SYS_mkdirat                 287
#define SYS_mknodat                 288
#define SYS_fchownat                289
#define SYS_futimesat               290
#define SYS_fstatat64               291
#define SYS_unlinkat                292
#define SYS_renameat                293
#define SYS_linkat                  294
#define SYS_symlinkat               295
#define SYS_readlinkat              296
#define SYS_fchmodat                297
#define SYS_faccessat               298
#define SYS_get_robust_list         299
#define SYS_set_robust_list         300
#define SYS_move_pages              301
#define SYS_getcpu                  302
#define SYS_epoll_pwait             303
#define SYS_utimensat               304
#define SYS_signalfd                305
#define SYS_timerfd_create          306
#define SYS_eventfd                 307
#define SYS_sync_file_range2        308
#define SYS_fallocate               309
#define SYS_subpage_prot            310
#define SYS_timerfd_settime         311
#define SYS_timerfd_gettime         312
#define SYS_signalfd4               313
#define SYS_eventfd2                314
#define SYS_epoll_create1           315
#define SYS_dup3                    316
#define SYS_pipe2                   317
#define SYS_inotify_init1           318
#define SYS_perf_event_open         319
#define SYS_preadv                  320
#define SYS_pwritev                 321
#define SYS_rt_tgsigqueueinfo       322
#define SYS_fanotify_init           323
#define SYS_fanotify_mark           324
#define SYS_prlimit64               325
#define SYS_socket                  326
#define SYS_bind                    327
#define SYS_connect                 328
#define SYS_listen                  329
#define SYS_accept                  330
#define SYS_getsockname             331
#define SYS_getpeername             332
#define SYS_socketpair              333
#define SYS_send                    334
#define SYS_sendto                  335
#define SYS_recv                    336
#define SYS_recvfrom                337
#define SYS_shutdown                338
#define SYS_setsockopt              339
#define SYS_getsockopt              340
#define SYS_sendmsg                 341
#define SYS_recvmsg                 342
#define SYS_recvmmsg                343
#define SYS_accept4                 344
#define SYS_name_to_handle_at       345
#define SYS_open_by_handle_at       346
#define SYS_clock_adjtime           347
#define SYS_syncfs                  348
#define SYS_sendmmsg                349
#define SYS_setns                   350
#define SYS_process_vm_readv        351
#define SYS_process_vm_writev       352
#define SYS_finit_module            353
#define SYS_kcmp                    354
#define SYS_sched_setattr           355
#define SYS_sched_getattr           356
#define SYS_renameat2               357
#define SYS_seccomp                 358
#define SYS_getrandom               359
#define SYS_memfd_create            360
#define SYS_bpf                     361
#define SYS_execveat                362
#define SYS_switch_endian           363
#define SYS_userfaultfd             364
#define SYS_membarrier              365
//#define SYS_ni_syscall              366
//#define SYS_ni_syscall              367
//#define SYS_ni_syscall              368
//#define SYS_ni_syscall              369
//#define SYS_ni_syscall              370
//#define SYS_ni_syscall              371
//#define SYS_ni_syscall              372
//#define SYS_ni_syscall              373
//#define SYS_ni_syscall              374
//#define SYS_ni_syscall              375
//#define SYS_ni_syscall              376
//#define SYS_ni_syscall              377
#define SYS_mlock2                  378
#define SYS_copy_file_range         379
#define SYS_preadv2                 380
#define SYS_pwritev2                381
#define SYS_kexec_file_load         382
#define SYS_statx                   383
#define SYS_pkey_alloc              384
#define SYS_pkey_free               385
#define SYS_pkey_mprotect           386
#define SYS_rseq                    387
#define SYS_io_pgetevents           388
//#define SYS_ni_syscall              389
//#define SYS_ni_syscall              390
//#define SYS_ni_syscall              391
//#define SYS_ni_syscall              392
#define SYS_semctl                  393
#define SYS_semget                  394
#define SYS_shmget                  395
#define SYS_shmctl                  396
#define SYS_shmat                   397
#define SYS_shmdt                   398
#define SYS_msgget                  399
#define SYS_msgsnd                  400
#define SYS_msgrcv                  401
#define SYS_msgctl                  402
#define SYS_clock_gettime64         403
#define SYS_clock_settime64         404
#define SYS_clock_adjtime64         405
#define SYS_clock_getres_time64     406
#define SYS_clock_nanosleep_time64  407
#define SYS_timer_gettime64         408
#define SYS_timer_settime64         409
#define SYS_timerfd_gettime64       410
#define SYS_timerfd_settime64       411
#define SYS_utimensat_time64        412
#define SYS_pselect6_time64         413
#define SYS_ppoll_time64            414
//#define SYS_ni_syscall              415
#define SYS_io_pgetevents_time64    416
#define SYS_recvmmsg_time64         417
#define SYS_mq_timedsend_time64     418
#define SYS_mq_timedreceive_time64  419
#define SYS_semtimedop_time64       420
#define SYS_rt_sigtimedwait_time64  421
#define SYS_futex_time64            422
#define SYS_sched_rr_get_interval_time64    423
#define SYS_pidfd_send_signal       424
#define SYS_io_uring_setup          425
#define SYS_io_uring_enter          426
#define SYS_io_uring_register       427
#define SYS_open_tree               428
#define SYS_move_mount              429
#define SYS_fsopen                  430
#define SYS_fsconfig                431
#define SYS_fsmount                 432
#define SYS_fspick                  433
#define SYS_pidfd_open              434
#define SYS_clone3                  435
#define SYS_close_range             436
#define SYS_openat2                 437
#define SYS_pidfd_getfd             438
#define SYS_faccessat2              439
#define SYS_process_madvise         440
#define SYS_epoll_pwait2            441
#define SYS_mount_setattr           442
#define SYS_quotactl_fd             443
#define SYS_landlock_create_ruleset 444
#define SYS_landlock_add_rule       445
#define SYS_landlock_restrict_self  446
//#define SYS_ni_syscall              447
#define SYS_process_mrelease        448
#define SYS_futex_waitv             449

/*
 * Inline assembler for calling Linux system calls
 */

syscall_res sys_call0( u_long func );
#pragma aux sys_call0 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0]        \
    __value [$r4 $r3];

syscall_res sys_call1( u_long func, u_long r_3 );
#pragma aux sys_call1 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] \
    __value [$r4 $r3];

void sys_call1n( u_long func, u_long r_3 );
#pragma aux sys_call1n = \
        "sc"            \
    __parm [$r0] [$r3];

syscall_res sys_call2( u_long func, u_long r_3, u_long r_4 );
#pragma aux sys_call2 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] [$r4] \
    __value [$r4 $r3];

syscall_res sys_call3( u_long func, u_long r_3, u_long r_4, u_long r_5 );
#pragma aux sys_call3 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] [$r4] [$r5] \
    __value [$r4 $r3];

syscall_res sys_call4( u_long func, u_long r_3, u_long r_4, u_long r_5, u_long r_6 );
#pragma aux sys_call4 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] [$r4] [$r5] [$r6] \
    __value [$r4 $r3];

syscall_res sys_call5( u_long func, u_long r_3, u_long r_4, u_long r_5, u_long r_6, u_long r_7 );
#pragma aux sys_call5 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] [$r4] [$r5] [$r6] [$r7] \
    __value [$r4 $r3];

syscall_res sys_call6( u_long func, u_long r_3, u_long r_4, u_long r_5, u_long r_6, u_long r_7, u_long r_8 );
#pragma aux sys_call6 = \
        "sc"            \
        "mr 4,0"        \
    __parm [$r0] [$r3] [$r4] [$r5] [$r6] [$r7] [$r8] \
    __value [$r4 $r3];

#define _sys_exit(rc)       sys_call1n(SYS_exit, rc)
#define _sys_exit_group(rc) sys_call1n(SYS_exit_group, rc)
