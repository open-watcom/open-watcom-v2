/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Linux system calls interface - i386 specifics.
*
****************************************************************************/


/* On i386, syscall return value and error number are both returned
 * in eax register. Values between -125 and -1 indicate an error,
 * everything else is a valid return value.
 */

/* user-visible error numbers are in the range -1 - -124 */

#define __syscall_return( type, res )                   \
    if( (u_long)(res) >= (u_long)(-125) ) {             \
        errno = -(res);                                 \
        res = (u_long)-1;                               \
    }                                                   \
    return( (type)(res) );

#define __syscall_return_pointer( type, res )           \
    if( (u_long)(res) >= (u_long)(-125) ) {             \
        errno = -(res);                                 \
        res = (u_long)0;                                \
    }                                                   \
    return( (type)(res) );


/*
 * Linux system call numbers
 */

#define SYS_exit                  1
#define SYS_fork                  2
#define SYS_read                  3
#define SYS_write                 4
#define SYS_open                  5
#define SYS_close                 6
#define SYS_waitpid               7
#define SYS_creat                 8
#define SYS_link                  9
#define SYS_unlink               10
#define SYS_execve               11
#define SYS_chdir                12
#define SYS_time                 13
#define SYS_mknod                14
#define SYS_chmod                15
#define SYS_lchown               16
#define SYS_break                17
#define SYS_oldstat              18
#define SYS_lseek                19
#define SYS_getpid               20
#define SYS_mount                21
#define SYS_umount               22
#define SYS_setuid               23
#define SYS_getuid               24
#define SYS_stime                25
#define SYS_ptrace               26
#define SYS_alarm                27
#define SYS_oldfstat             28
#define SYS_pause                29
#define SYS_utime                30
#define SYS_stty                 31
#define SYS_gtty                 32
#define SYS_access               33
#define SYS_nice                 34
#define SYS_ftime                35
#define SYS_sync                 36
#define SYS_kill                 37
#define SYS_rename               38
#define SYS_mkdir                39
#define SYS_rmdir                40
#define SYS_dup                  41
#define SYS_pipe                 42
#define SYS_times                43
#define SYS_prof                 44
#define SYS_brk                  45
#define SYS_setgid               46
#define SYS_getgid               47
#define SYS_signal               48
#define SYS_geteuid              49
#define SYS_getegid              50
#define SYS_acct                 51
#define SYS_umount2              52
#define SYS_lock                 53
#define SYS_ioctl                54
#define SYS_fcntl                55
#define SYS_mpx                  56
#define SYS_setpgid              57
#define SYS_ulimit               58
#define SYS_oldolduname          59
#define SYS_umask                60
#define SYS_chroot               61
#define SYS_ustat                62
#define SYS_dup2                 63
#define SYS_getppid              64
#define SYS_getpgrp              65
#define SYS_setsid               66
#define SYS_sigaction            67
#define SYS_sgetmask             68
#define SYS_ssetmask             69
#define SYS_setreuid             70
#define SYS_setregid             71
#define SYS_sigsuspend           72
#define SYS_sigpending           73
#define SYS_sethostname          74
#define SYS_setrlimit            75
#define SYS_getrlimit            76     /* Back compatible 2Gig limited rlimit */
#define SYS_getrusage            77
#define SYS_gettimeofday         78
#define SYS_settimeofday         79
#define SYS_getgroups            80
#define SYS_setgroups            81
#define SYS_select               82
#define SYS_symlink              83
#define SYS_oldlstat             84
#define SYS_readlink             85
#define SYS_uselib               86
#define SYS_swapon               87
#define SYS_reboot               88
#define SYS_readdir              89
#define SYS_mmap                 90
#define SYS_munmap               91
#define SYS_truncate             92
#define SYS_ftruncate            93
#define SYS_fchmod               94
#define SYS_fchown               95
#define SYS_getpriority          96
#define SYS_setpriority          97
#define SYS_profil               98
#define SYS_statfs               99
#define SYS_fstatfs             100
#define SYS_ioperm              101
#define SYS_socketcall          102
#define SYS_syslog              103
#define SYS_setitimer           104
#define SYS_getitimer           105
#define SYS_stat                106
#define SYS_lstat               107
#define SYS_fstat               108
#define SYS_olduname            109
#define SYS_iopl                110
#define SYS_vhangup             111
#define SYS_idle                112
#define SYS_vm86old             113
#define SYS_wait4               114
#define SYS_swapoff             115
#define SYS_sysinfo             116
#define SYS_ipc                 117
#define SYS_fsync               118
#define SYS_sigreturn           119
#define SYS_clone               120
#define SYS_setdomainname       121
#define SYS_uname               122
#define SYS_modify_ldt          123
#define SYS_adjtimex            124
#define SYS_mprotect            125
#define SYS_sigprocmask         126
#define SYS_create_module       127
#define SYS_init_module         128
#define SYS_delete_module       129
#define SYS_get_kernel_syms     130
#define SYS_quotactl            131
#define SYS_getpgid             132
#define SYS_fchdir              133
#define SYS_bdflush             134
#define SYS_sysfs               135
#define SYS_personality         136
#define SYS_afs_syscall         137 /* Syscall for Andrew File System */
#define SYS_setfsuid            138
#define SYS_setfsgid            139
#define SYS__llseek             140
#define SYS_getdents            141
#define SYS__newselect          142
#define SYS_flock               143
#define SYS_msync               144
#define SYS_readv               145
#define SYS_writev              146
#define SYS_getsid              147
#define SYS_fdatasync           148
#define SYS__sysctl             149
#define SYS_mlock               150
#define SYS_munlock             151
#define SYS_mlockall            152
#define SYS_munlockall          153
#define SYS_sched_setparam      154
#define SYS_sched_getparam      155
#define SYS_sched_setscheduler  156
#define SYS_sched_getscheduler  157
#define SYS_sched_yield         158
#define SYS_sched_get_priority_max      159
#define SYS_sched_get_priority_min      160
#define SYS_sched_rr_get_interval       161
#define SYS_nanosleep           162
#define SYS_mremap              163
#define SYS_setresuid           164
#define SYS_getresuid           165
#define SYS_vm86                166
#define SYS_query_module        167
#define SYS_poll                168
#define SYS_nfsservctl          169
#define SYS_setresgid           170
#define SYS_getresgid           171
#define SYS_prctl               172
#define SYS_rt_sigreturn        173
#define SYS_rt_sigaction        174
#define SYS_rt_sigprocmask      175
#define SYS_rt_sigpending       176
#define SYS_rt_sigtimedwait     177
#define SYS_rt_sigqueueinfo     178
#define SYS_rt_sigsuspend       179
#define SYS_pread               180
#define SYS_pwrite              181
#define SYS_chown               182
#define SYS_getcwd              183
#define SYS_capget              184
#define SYS_capset              185
#define SYS_sigaltstack         186
#define SYS_sendfile            187
#define SYS_getpmsg             188     /* some people actually want streams */
#define SYS_putpmsg             189     /* some people actually want streams */
#define SYS_vfork               190
#define SYS_ugetrlimit          191     /* SuS compliant getrlimit */
#define SYS_mmap2               192
#define SYS_truncate64          193
#define SYS_ftruncate64         194
#define SYS_stat64              195
#define SYS_lstat64             196
#define SYS_fstat64             197
#define SYS_lchown32            198
#define SYS_getuid32            199
#define SYS_getgid32            200
#define SYS_geteuid32           201
#define SYS_getegid32           202
#define SYS_setreuid32          203
#define SYS_setregid32          204
#define SYS_getgroups32         205
#define SYS_setgroups32         206
#define SYS_fchown32            207
#define SYS_setresuid32         208
#define SYS_getresuid32         209
#define SYS_setresgid32         210
#define SYS_getresgid32         211
#define SYS_chown32             212
#define SYS_setuid32            213
#define SYS_setgid32            214
#define SYS_setfsuid32          215
#define SYS_setfsgid32          216
#define SYS_pivot_root          217
#define SYS_mincore             218
#define SYS_madvise             219
#define SYS_madvise1            219     /* delete when C lib stub is removed */
#define SYS_getdents64          220
#define SYS_fcntl64             221
#define SYS_security            223     /* syscall for security modules */
#define SYS_gettid              224
#define SYS_readahead           225
#define SYS_setxattr            226
#define SYS_lsetxattr           227
#define SYS_fsetxattr           228
#define SYS_getxattr            229
#define SYS_lgetxattr           230
#define SYS_fgetxattr           231
#define SYS_listxattr           232
#define SYS_llistxattr          233
#define SYS_flistxattr          234
#define SYS_removexattr         235
#define SYS_lremovexattr        236
#define SYS_fremovexattr        237
#define SYS_tkill               238
#define SYS_sendfile64          239
#define SYS_futex               240
#define SYS_sched_setaffinity   241
#define SYS_sched_getaffinity   242
#define SYS_set_thread_area     243

/*
 * internal sub-numbers for SYS_socketcall
 */
#define SYS_SOCKET                1
#define SYS_BIND                  2
#define SYS_CONNECT               3
#define SYS_LISTEN                4
#define SYS_ACCEPT                5
#define SYS_GETSOCKNAME           6
#define SYS_GETPEERNAME           7
#define SYS_SOCKETPAIR            8
#define SYS_SEND                  9
#define SYS_RECV                 10
#define SYS_SENDTO               11
#define SYS_RECVFROM             12
#define SYS_SHUTDOWN             13
#define SYS_SETSOCKOPT           14
#define SYS_GETSOCKOPT           15
#define SYS_SENDMSG              16
#define SYS_RECVMSG              17

/*
 * Inline assembler for calling Linux system calls
 */

u_long sys_call0( u_long func );
#pragma aux sys_call0 =                         \
    "int    0x80"                               \
    parm [eax]                                  \
    value [eax];

u_long sys_call1( u_long func, u_long r_ebx );
#pragma aux sys_call1 =                         \
    "int    0x80"                               \
    parm [eax] [ebx]                            \
    value [eax];

u_long sys_call2( u_long func, u_long r_ebx, u_long r_ecx );
#pragma aux sys_call2 =                         \
    "int    0x80"                               \
    parm [eax] [ebx] [ecx]                      \
    value [eax];

u_long sys_call3( u_long func, u_long r_ebx, u_long r_ecx, u_long r_edx );
#pragma aux sys_call3 =                         \
    "int    0x80"                               \
    parm [eax] [ebx] [ecx] [edx]                \
    value [eax];

u_long sys_call4( u_long func, u_long r_ebx, u_long r_ecx, u_long r_edx, u_long r_esi );
#pragma aux sys_call4 =                         \
    "int    0x80"                               \
    parm [eax] [ebx] [ecx] [edx] [esi]          \
    value [eax];

u_long sys_call5( u_long func, u_long r_ebx, u_long r_ecx, u_long r_edx, u_long r_esi, u_long r_edi );
#pragma aux sys_call5 =                         \
    "int    0x80"                               \
    parm [eax] [ebx] [ecx] [edx] [esi] [edi]    \
    value [eax];
