/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux system calls interface - MIPS specifics.
*
****************************************************************************/


/* On MIPS, return values and error numbers are both returned in
 * register v0 (regular function return register). Register a3
 * (fourth argument register) specifies whether the return value was an
 * error or not: zero a3 means valid return value, nonzero a3 means error
 * number. Don't look at me, I didn't invent this.
 *
 * To fix this 64-bit value is used for returning from sys_call.. routines.
 * High 32-bit contains value/error flag and low 32-bit contains value.
 */

typedef unsigned long long      syscall_res;

/* macros to access sys_call.. routines return/error value */

#define __syscall_iserror( res )    ((res)>>32)
#define __syscall_errno( res )      ((res)&0xFFFFFFFF)
#define __syscall_val( type, res )  ((type)((res)&0xFFFFFFFF))

#define __syscall_retcode( res, val )                   \
    if( __syscall_iserror( res ) ) {                    \
        lib_set_errno( __syscall_errno( res ) );        \
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

#define SYS_exit                    4001
#define SYS_fork                    4002
#define SYS_read                    4003
#define SYS_write                   4004
#define SYS_open                    4005
#define SYS_close                   4006
#define SYS_waitpid                 4007
#define SYS_creat                   4008
#define SYS_link                    4009
#define SYS_unlink                  4010
#define SYS_execve                  4011
#define SYS_chdir                   4012
#define SYS_time                    4013
#define SYS_mknod                   4014
#define SYS_chmod                   4015
#define SYS_lchown                  4016
#define SYS_break                   4017
#define SYS_oldstat                 4018
#define SYS_lseek                   4019
#define SYS_getpid                  4020
#define SYS_mount                   4021
#define SYS_umount                  4022
#define SYS_setuid                  4023
#define SYS_getuid                  4024
#define SYS_stime                   4025
#define SYS_ptrace                  4026
#define SYS_alarm                   4027
#define SYS_oldfstat                4028
#define SYS_pause                   4029
#define SYS_utime                   4030
#define SYS_stty                    4031
#define SYS_gtty                    4032
#define SYS_access                  4033
#define SYS_nice                    4034
#define SYS_ftime                   4035
#define SYS_sync                    4036
#define SYS_kill                    4037
#define SYS_rename                  4038
#define SYS_mkdir                   4039
#define SYS_rmdir                   4040
#define SYS_dup                     4041
#define SYS_pipe                    4042
#define SYS_times                   4043
#define SYS_prof                    4044
#define SYS_brk                     4045
#define SYS_setgid                  4046
#define SYS_getgid                  4047
#define SYS_signal                  4048
#define SYS_geteuid                 4049
#define SYS_getegid                 4050
#define SYS_acct                    4051
#define SYS_umount2                 4052
#define SYS_lock                    4053
#define SYS_ioctl                   4054
#define SYS_fcntl                   4055
#define SYS_mpx                     4056
#define SYS_setpgid                 4057
#define SYS_ulimit                  4058
#define SYS_oldolduname             4059
#define SYS_umask                   4060
#define SYS_chroot                  4061
#define SYS_ustat                   4062
#define SYS_dup2                    4063
#define SYS_getppid                 4064
#define SYS_getpgrp                 4065
#define SYS_setsid                  4066
#define SYS_sigaction               4067
#define SYS_sgetmask                4068
#define SYS_ssetmask                4069
#define SYS_setreuid                4070
#define SYS_setregid                4071
#define SYS_sigsuspend              4072
#define SYS_sigpending              4073
#define SYS_sethostname             4074
#define SYS_setrlimit               4075
#define SYS_getrlimit               4076     /* Back compatible 2Gig limited rlimit */
#define SYS_getrusage               4077
#define SYS_gettimeofday            4078
#define SYS_settimeofday            4079
#define SYS_getgroups               4080
#define SYS_setgroups               4081
//#define SYS_select                  4082
#define SYS_symlink                 4083
#define SYS_oldlstat                4084
#define SYS_readlink                4085
#define SYS_uselib                  4086
#define SYS_swapon                  4087
#define SYS_reboot                  4088
#define SYS_readdir                 4089
#define SYS_mmap                    4090
#define SYS_munmap                  4091
#define SYS_truncate                4092
#define SYS_ftruncate               4093
#define SYS_fchmod                  4094
#define SYS_fchown                  4095
#define SYS_getpriority             4096
#define SYS_setpriority             4097
#define SYS_profil                  4098
#define SYS_statfs                  4099
#define SYS_fstatfs                 4100
#define SYS_ioperm                  4101
#define SYS_socketcall              4102
#define SYS_syslog                  4103
#define SYS_setitimer               4104
#define SYS_getitimer               4105
#define SYS_stat                    4106
#define SYS_lstat                   4107
#define SYS_fstat                   4108
#define SYS_olduname                4109
#define SYS_iopl                    4110
#define SYS_vhangup                 4111
#define SYS_idle                    4112
#define SYS_vfork                   4113
#define SYS_wait4                   4114
#define SYS_swapoff                 4115
#define SYS_sysinfo                 4116
#define SYS_ipc                     4117
#define SYS_fsync                   4118
#define SYS_sigreturn               4119
#define SYS_clone                   4120
#define SYS_setdomainname           4121
#define SYS_uname                   4122
#define SYS_modify_ldt              4123
#define SYS_adjtimex                4124
#define SYS_mprotect                4125
#define SYS_sigprocmask             4126
#define SYS_create_module           4127
#define SYS_init_module             4128
#define SYS_delete_module           4129
#define SYS_get_kernel_syms         4130
#define SYS_quotactl                4131
#define SYS_getpgid                 4132
#define SYS_fchdir                  4133
#define SYS_bdflush                 4134
#define SYS_sysfs                   4135
#define SYS_personality             4136
#define SYS_afs_syscall             4137 /* Syscall for Andrew File System */
#define SYS_setfsuid                4138
#define SYS_setfsgid                4139
#define SYS__llseek                 4140
#define SYS_getdents                4141
#define SYS__newselect              4142
#define SYS_flock                   4143
#define SYS_msync                   4144
#define SYS_readv                   4145
#define SYS_writev                  4146
#define SYS_cacheflush              4147
#define SYS_cachectl                4148
#define SYS_sysmips                 4149
#define SYS_unused150               4150
#define SYS_getsid                  4151
#define SYS_fdatasync               4152
#define SYS__sysctl                 4153
#define SYS_mlock                   4154
#define SYS_munlock                 4155
#define SYS_mlockall                4156
#define SYS_munlockall              4157
#define SYS_sched_setparam          4158
#define SYS_sched_getparam          4159
#define SYS_sched_setscheduler      4160
#define SYS_sched_getscheduler      4161
#define SYS_sched_yield             4162
#define SYS_sched_get_priority_max  4163
#define SYS_sched_get_priority_min  4164
#define SYS_sched_rr_get_interval   4165
#define SYS_nanosleep               4166
#define SYS_mremap                  4167
#define SYS_accept                  4168
#define SYS_bind                    4169
#define SYS_connect                 4170
#define SYS_getpeername             4171
#define SYS_getsockname             4172
#define SYS_getsockopt              4173
#define SYS_listen                  4174
#define SYS_recv                    4175
#define SYS_recvfrom                4176
#define SYS_recvmsg                 4177
#define SYS_send                    4178
#define SYS_sendmsg                 4179
#define SYS_sendto                  4180
#define SYS_setsockopt              4181
#define SYS_shutdown                4182
#define SYS_socket                  4183
#define SYS_socketpair              4184
#define SYS_setresuid               4185
#define SYS_getresuid               4186
#define SYS_query_module            4187
#define SYS_poll                    4188
#define SYS_nfsservctl              4189
#define SYS_setresgid               4190
#define SYS_getresgid               4191
#define SYS_prctl                   4192
#define SYS_rt_sigreturn            4193
#define SYS_rt_sigaction            4194
#define SYS_rt_sigprocmask          4195
#define SYS_rt_sigpending           4196
#define SYS_rt_sigtimedwait         4197
#define SYS_rt_sigqueueinfo         4198
#define SYS_rt_sigsuspend           4199
#define SYS_pread                   4200
#define SYS_pwrite                  4201
#define SYS_chown                   4202
#define SYS_getcwd                  4203
#define SYS_capget                  4204
#define SYS_capset                  4205
#define SYS_sigaltstack             4206
#define SYS_sendfile                4207
#define SYS_getpmsg                 4208
#define SYS_putpmsg                 4209
#define SYS_mmap2                   4210
#define SYS_truncate64              4211
#define SYS_ftruncate64             4212
#define SYS_stat64                  4213
#define SYS_lstat64                 4214
#define SYS_fstat64                 4215
#define SYS_pivot_root              4216
#define SYS_mincore                 4217
#define SYS_madvise                 4218
#define SYS_getdents64              4219
#define SYS_fcntl64                 4220
//#define SYS_ni_syscall              4221
#define SYS_gettid                  4222
#define SYS_readahead               4223
#define SYS_setxattr                4224
#define SYS_lsetxattr               4225
#define SYS_fsetxattr               4226
#define SYS_getxattr                4227
#define SYS_lgetxattr               4228
#define SYS_fgetxattr               4229
#define SYS_listxattr               4230
#define SYS_llistxattr              4231
#define SYS_flistxattr              4232
#define SYS_removexattr             4233
#define SYS_lremovexattr            4234
#define SYS_fremovexattr            4235
#define SYS_tkill                   4236
#define SYS_sendfile64              4237
#define SYS_futex                   4238
#define SYS_sched_setaffinity       4239
#define SYS_sched_getaffinity       4240
#define SYS_io_setup                4241
#define SYS_io_destroy              4242
#define SYS_io_getevents            4243
#define SYS_io_submit               4244
#define SYS_io_cancel               4245
#define SYS_exit_group              4246
#define SYS_lookup_dcookie          4247
#define SYS_epoll_create            4248
#define SYS_epoll_ctl               4249
#define SYS_epoll_wait              4250
#define SYS_remap_file_pages        4251
#define SYS_set_tid_address         4252
#define SYS_restart_syscall         4253
#define SYS_fadvise64_64            4254
#define SYS_statfs64                4255
#define SYS_fstatfs64               4256
#define SYS_timer_create            4257
#define SYS_timer_settime           4258
#define SYS_timer_gettime           4259
#define SYS_timer_getoverrun        4260
#define SYS_timer_delete            4261
#define SYS_clock_settime           4262
#define SYS_clock_gettime           4263
#define SYS_clock_getres            4264
#define SYS_clock_nanosleep         4265
#define SYS_tgkill                  4266
#define SYS_utimes                  4267
#define SYS_mbind                   4268
#define SYS_get_mempolicy           4269
#define SYS_set_mempolicy           4270
#define SYS_mq_open                 4271
#define SYS_mq_unlink               4272
#define SYS_mq_timedsend            4273
#define SYS_mq_timedreceive         4274
#define SYS_mq_notify               4275
#define SYS_mq_getsetattr           4276
#define SYS_vserver                 4277
#define SYS_waitid                  4278
//#define SYS_ni_syscall              4279       /* available, was setaltroot */
#define SYS_add_key                 4280
#define SYS_request_key             4281
#define SYS_keyctl                  4282
#define SYS_set_thread_area         4283

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

syscall_res sys_call0( u_long func );
#pragma aux sys_call0 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] \
    __value [$v1 $v0];

syscall_res sys_call1( u_long func, u_long r_4 );
#pragma aux sys_call1 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] [$a0] \
    __value [$v1 $v0];

void sys_call1n( u_long func, u_long r_4 );
#pragma aux sys_call1n =                        \
    "syscall"                                   \
    __parm [$v0] [$a0];

syscall_res sys_call2( u_long func, u_long r_4, u_long r_5 );
#pragma aux sys_call2 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] [$a0] [$a1] \
    __value [$v1 $v0];

syscall_res sys_call3( u_long func, u_long r_4, u_long r_5, u_long r_6 );
#pragma aux sys_call3 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] [$a0] [$a1] [$a2] \
    __value [$v1 $v0];

syscall_res sys_call4( u_long func, u_long r_4, u_long r_5, u_long r_6, u_long r_7 );
#pragma aux sys_call4 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] [$a0] [$a1] [$a2] [$a3] \
    __value [$v1 $v0];

syscall_res sys_call5( u_long func, u_long r_4, u_long r_5, u_long r_6, u_long r_7, u_long arg5 );
#pragma aux sys_call5 =                         \
    "syscall"                                   \
    "move $v1,$a3"                              \
    __parm [$v0] [$a0] [$a1] [$a2] [$a3] \
    __value [$v1 $v0];

#define _sys_exit(rc)       sys_call1n(SYS_exit, rc)
#define _sys_exit_group(rc) sys_call1n(SYS_exit_group, rc)
