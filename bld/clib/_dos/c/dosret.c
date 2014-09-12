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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#if   defined(__NT__)
#include <windows.h>
#elif defined(__OS2__)
#define INCL_DOSERRORS
#include <wos2.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include "seterrno.h"
#include "dosret.h"
#include "rtdata.h"

#if defined(__NT__) || defined(__OS2__)
/*
  * Translation table of Win32 and OS/2 error numbers to errno codes.
  * OS/2 and NT employ much the same error codes. Those entries marked
  * "OS/2" are not used by Win32 and appear to be no longer used in OS/2.
  * Only the first 255 messages are translated. The range of message
  * numbers appears to be from 0 to 65535 but it is a disjoint set.
 */

static signed char xlat[] = {
/*  errno values       Win32 & OS/2 error values */
    0,              /*   0 The operation completed successfully.  ERROR_SUCCESS */
    EINVAL,         /*   1 Incorrect function.  ERROR_INVALID_FUNCTION */
    ENOENT,         /*   2 The system cannot find the file specified.  ERROR_FILE_NOT_FOUND */
    ENOENT,         /*   3 The system cannot find the path specified.  ERROR_PATH_NOT_FOUND */
    EMFILE,         /*   4 The system cannot open the file.  ERROR_TOO_MANY_OPEN_FILES */
    EACCES,         /*   5 Access is denied.  ERROR_ACCESS_DENIED */
    EBADF,          /*   6 The handle is invalid.  ERROR_INVALID_HANDLE */
    ENOMEM,         /*   7 The storage control blocks were destroyed.  ERROR_ARENA_TRASHED */
    ENOMEM,         /*   8 Not enough storage is available to process this command.  ERROR_NOT_ENOUGH_MEMORY */
    ENOMEM,         /*   9 The storage control block address is invalid.  ERROR_INVALID_BLOCK */
    E2BIG,          /*  10 The environment is incorrect.  ERROR_BAD_ENVIRONMENT */
    ENOEXEC,        /*  11 An attempt was made to load a program with an incorrect format.  ERROR_BAD_FORMAT */
    EACCES,         /*  12 The access code is invalid.  ERROR_INVALID_ACCESS */
    ERANGE,         /*  13 The data is invalid.  ERROR_INVALID_DATA */
    ENOMEM,         /*  14 Not enough storage is available to complete this operation.  ERROR_OUTOFMEMORY */
    ENODEV,         /*  15 The system cannot find the drive specified.  ERROR_INVALID_DRIVE */
    EACCES,         /*  16 The directory cannot be removed.  ERROR_CURRENT_DIRECTORY */
    EXDEV,          /*  17 The system cannot move the file to a different disk drive.  ERROR_NOT_SAME_DEVICE */
    ENOENT,         /*  18 There are no more files.  ERROR_NO_MORE_FILES */
    EROFS,          /*  19 The media is write protected.  ERROR_WRITE_PROTECT */
    ENXIO,          /*  20 The system cannot find the device specified.  ERROR_BAD_UNIT */
    EAGAIN,         /*  21 The device is not ready.  ERROR_NOT_READY */
    EIO,            /*  22 The device does not recognize the command.  ERROR_BAD_COMMAND */
    EIO,            /*  23 Data error (cyclic redundancy check).  ERROR_CRC */
    ENOEXEC,        /*  24 The program issued a command but the command length is incorrect.  ERROR_BAD_LENGTH */
    EIO,            /*  25 The drive cannot locate a specific area or track on the disk.  ERROR_SEEK */
    ENODEV,         /*  26 The specified disk or diskette cannot be accessed.  ERROR_NOT_DOS_DISK */
    EIO,            /*  27 The drive cannot find the sector requested.  ERROR_SECTOR_NOT_FOUND */
    ENOSPC,         /*  28 The printer is out of paper.  ERROR_OUT_OF_PAPER */
    EIO,            /*  29 The system cannot write to the specified device.  ERROR_WRITE_FAULT */
    EIO,            /*  30 The system cannot read from the specified device.  ERROR_READ_FAULT */
    EIO,            /*  31 A device attached to the system is not functioning.  ERROR_GEN_FAILURE */
    EACCES,         /*  32 The process cannot access the file because it is being used by another process.  ERROR_SHARING_VIOLATION */
    EACCES,         /*  33 The process cannot access the file because another process has locked a portion of the file.  ERROR_LOCK_VIOLATION */
    EIO,            /*  34 The wrong diskette is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.  ERROR_WRONG_DISK */
    -1,             /*  35 OS/2 ERROR_FCB_UNAVAILABLE */
    EACCES,         /*  36 Too many files opened for sharing.  ERROR_SHARING_BUFFER_EXCEEDED */
    -1,             /*  37 OS/2 ERROR_CODE_PAGE_MISMATCHED */
    EIO,            /*  38 Reached the end of the file.  ERROR_HANDLE_EOF */
    ENOSPC,         /*  39 The disk is full.  ERROR_HANDLE_DISK_FULL */
    -1,             /*  40 */
    -1,             /*  41 */
    -1,             /*  42 */
    -1,             /*  43 */
    -1,             /*  44 */
    -1,             /*  45 */
    -1,             /*  46 */
    -1,             /*  47 */
    -1,             /*  48 */
    -1,             /*  49 */
    EIO,            /*  50 The network request is not supported.  ERROR_NOT_SUPPORTED */
    EIO,            /*  51 The remote computer is not available.  ERROR_REM_NOT_LIST */
    EIO,            /*  52 A duplicate name exists on the network.  ERROR_DUP_NAME */
    EIO,            /*  53 The network path was not found.  ERROR_BAD_NETPATH */
    EIO,            /*  54 The network is busy.  ERROR_NETWORK_BUSY */
    EIO,            /*  55 The specified network resource or device is no longer available.  ERROR_DEV_NOT_EXIST */
    EIO,            /*  56 The network BIOS command limit has been reached.  ERROR_TOO_MANY_CMDS */
    EIO,            /*  57 A network adapter hardware error occurred.  ERROR_ADAP_HDW_ERR */
    EIO,            /*  58 The specified server cannot perform the requested operation.  ERROR_BAD_NET_RESP */
    EIO,            /*  59 An unexpected network error occurred.  ERROR_UNEXP_NET_ERR */
    EIO,            /*  60 The remote adapter is not compatible.  ERROR_BAD_REM_ADAP */
    EIO,            /*  61 The printer queue is full.  ERROR_PRINTQ_FULL */
    ENOSPC,         /*  62 Space to store the file waiting to be printed is not available on the server.  ERROR_NO_SPOOL_SPACE */
    EIO,            /*  63 Your file waiting to be printed was deleted.  ERROR_PRINT_CANCELLED */
    EIO,            /*  64 The specified network name is no longer available.  ERROR_NETNAME_DELETED */
    EIO,            /*  65 Network access is denied.  ERROR_NETWORK_ACCESS_DENIED */
    EIO,            /*  66 The network resource type is not correct.  ERROR_BAD_DEV_TYPE */
    EIO,            /*  67 The network name cannot be found.  ERROR_BAD_NET_NAME */
    EIO,            /*  68 The name limit for the local computer network adapter card was exceeded.  ERROR_TOO_MANY_NAMES */
    EIO,            /*  69 The network BIOS session limit was exceeded.  ERROR_TOO_MANY_SESS */
    EIO,            /*  70 The remote server has been paused or is in the process of being started.  ERROR_SHARING_PAUSED */
    EIO,            /*  71 No more connections can be made to this remote computer at this time because there are already as many connections as the computer can accept.  ERROR_REQ_NOT_ACCEP */
    EIO,            /*  72 The specified printer or disk device has been paused.  ERROR_REDIR_PAUSED */
    EIO,            /*  73 OS/2 ERROR_SBCS_ATT_WRITE_PROT */
    EIO,            /*  74 OS/2 ERROR_SBCS_GENERAL_FAILURE */
    EIO,            /*  75 OS/2 ERROR_XGA_OUT_MEMORY */
    -1,             /*  76 */
    -1,             /*  77 */
    -1,             /*  78 */
    -1,             /*  79 */
    EEXIST,         /*  80 The file exists.  ERROR_FILE_EXISTS */
    -1,             /*  81 OS/2 ERROR_DUP_FCB */
    ENOENT,         /*  82 The directory or file cannot be created.  ERROR_CANNOT_MAKE */
    EIO,            /*  83 Fail on INT 24.  ERROR_FAIL_I24 */
    ENOMEM,         /*  84 Storage to process this request is not available.  ERROR_OUT_OF_STRUCTURES */
    EEXIST,         /*  85 The local device name is already in use.  ERROR_ALREADY_ASSIGNED */
    EACCES,         /*  86 The specified network password is not correct.  ERROR_INVALID_PASSWORD */
    EINVAL,         /*  87 The parameter is incorrect.  ERROR_INVALID_PARAMETER */
    EFAULT,         /*  88 A write fault occurred on the network.  ERROR_NET_WRITE_FAULT */
    EBUSY,          /*  89 The system cannot start another process at this time.  ERROR_NO_PROC_SLOTS */
    -1,             /*  90 OS/2 ERROR_NOT_FROZEN */
    -1,             /*  91 OS/2 ERROR_SYS_COMP_NOT_LOADED */
    -1,             /*  92 OS/2 ERR_TSTOVFL */
    -1,             /*  93 OS/2 ERR_TSTDUP */
    -1,             /*  94 OS/2 ERROR_NO_ITEMS */
    -1,             /*  95 OS/2 ERROR_INTERRUPT */
    -1,             /*  96 */
    -1,             /*  97 */
    -1,             /*  98 */
    EIO,            /*  99 OS/2 ERROR_DEVICE_IN_USE */
    EIO,            /* 100 Cannot create another system semaphore.  ERROR_TOO_MANY_SEMAPHORES */
    EPERM,          /* 101 The exclusive semaphore is owned by another process.  ERROR_EXCL_SEM_ALREADY_OWNED */
    EACCES,         /* 102 The semaphore is set and cannot be closed.  ERROR_SEM_IS_SET */
    EACCES,         /* 103 The semaphore cannot be set again.  ERROR_TOO_MANY_SEM_REQUESTS */
    EIO,            /* 104 Cannot request exclusive semaphores at interrupt time.  ERROR_INVALID_AT_INTERRUPT_TIME */
    EIO,            /* 105 The previous ownership of this semaphore has ended.  ERROR_SEM_OWNER_DIED */
    EIO,            /* 106 Insert the diskette for drive %1.  ERROR_SEM_USER_LIMIT */
    EIO,            /* 107 The program stopped because an alternate diskette was not inserted.  ERROR_DISK_CHANGE */
    EACCES,         /* 108 The disk is in use or locked by another process.  ERROR_DRIVE_LOCKED */
    EPIPE,          /* 109 The pipe has been ended.  ERROR_BROKEN_PIPE */
    ENOENT,         /* 110 The system cannot open the device or file specified.  ERROR_OPEN_FAILED */
    ENAMETOOLONG,   /* 111 The file name is too long.  ERROR_BUFFER_OVERFLOW */
    ENOSPC,         /* 112 There is not enough space on the disk.  ERROR_DISK_FULL */
    EMFILE,         /* 113 No more internal file identifiers available.  ERROR_NO_MORE_SEARCH_HANDLES */
    EBADF,          /* 114 The target internal file identifier is incorrect.  ERROR_INVALID_TARGET_HANDLE */
    -1,             /* 115 OS/2 ERROR_PROTECTION_VIOLATION */
    -1,             /* 116 OS/2 ERROR_VIOKBD_REQUEST */
    EIO,            /* 117 The IOCTL call made by the application program is not correct.  ERROR_INVALID_CATEGORY */
    EIO,            /* 118 The verify-on-write switch parameter value is not correct.  ERROR_INVALID_VERIFY_SWITCH */
    EIO,            /* 119 The system does not support the command requested.  ERROR_BAD_DRIVER_LEVEL */
    ENOSYS,         /* 120 This function is not supported on this system.  ERROR_CALL_NOT_IMPLEMENTED */
    EIO,            /* 121 The semaphore timeout period has expired.  ERROR_SEM_TIMEOUT */
    EINVAL,         /* 122 The data area passed to a system call is too small.  ERROR_INSUFFICIENT_BUFFER */
    ENOENT,         /* 123 The filename, directory name, or volume label syntax is incorrect.  ERROR_INVALID_NAME */
    EINVAL,         /* 124 The system call level is not correct.  ERROR_INVALID_LEVEL */
    EIO,            /* 125 The disk has no volume label.  ERROR_NO_VOLUME_LABEL */
    ENOSYS,         /* 126 The specified module could not be found.  ERROR_MOD_NOT_FOUND */
    ENOSYS,         /* 127 The specified procedure could not be found.  ERROR_PROC_NOT_FOUND */
    ECHILD,         /* 128 There are no child processes to wait for.  ERROR_WAIT_NO_CHILDREN */
    ECHILD,         /* 129 The %1 application cannot be run in Win32 mode.  ERROR_CHILD_NOT_COMPLETE */
    EIO,            /* 130 Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.  ERROR_DIRECT_ACCESS_HANDLE */
    ESPIPE,         /* 131 An attempt was made to move the file pointer before the beginning of the file.  ERROR_NEGATIVE_SEEK */
    ESPIPE,         /* 132 The file pointer cannot be set on the specified device or file.  ERROR_SEEK_ON_DEVICE */
    ENOENT,         /* 133 A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.  ERROR_IS_JOIN_TARGET */
    ENOENT,         /* 134 An attempt was made to use a JOIN or SUBST command on a drive that has already been joined.  ERROR_IS_JOINED */
    ENOENT,         /* 135 An attempt was made to use a JOIN or SUBST command on a drive that has already been substituted.  ERROR_IS_SUBSTED */
    ENOENT,         /* 136 The system tried to delete the JOIN of a drive that is not joined.  ERROR_NOT_JOINED */
    ENOENT,         /* 137 The system tried to delete the substitution of a drive that is not substituted.  ERROR_NOT_SUBSTED */
    ENOENT,         /* 138 The system tried to join a drive to a directory on a joined drive.  ERROR_JOIN_TO_JOIN */
    ENOENT,         /* 139 The system tried to substitute a drive to a directory on a substituted drive.  ERROR_SUBST_TO_SUBST */
    ENOENT,         /* 140 The system tried to join a drive to a directory on a substituted drive.  ERROR_JOIN_TO_SUBST */
    ENOENT,         /* 141 The system tried to SUBST a drive to a directory on a joined drive.  ERROR_SUBST_TO_JOIN */
    EBUSY,          /* 142 The system cannot perform a JOIN or SUBST at this time.  ERROR_BUSY_DRIVE */
    ENOENT,         /* 143 The system cannot join or substitute a drive to or for a directory on the same drive.  ERROR_SAME_DRIVE */
    ENOENT,         /* 144 The directory is not a subdirectory of the root directory.  ERROR_DIR_NOT_ROOT */
    ENOENT,         /* 145 The directory is not empty.  ERROR_DIR_NOT_EMPTY */
    ENOENT,         /* 146 The path specified is being used in a substitute.  ERROR_IS_SUBST_PATH */
    ENOENT,         /* 147 Not enough resources are available to process this command.  ERROR_IS_JOIN_PATH */
    EBUSY,          /* 148 The path specified cannot be used at this time.  ERROR_PATH_BUSY */
    ENOENT,         /* 149 An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.  ERROR_IS_SUBST_TARGET */
    ENOEXEC,        /* 150 System trace information was not specified in your CONFIG.SYS file, or tracing is disallowed.  ERROR_SYSTEM_TRACE */
    EIO,            /* 151 The number of specified semaphore events for DosMuxSemWait is not correct.  ERROR_INVALID_EVENT_COUNT */
    EIO,            /* 152 DosMuxSemWait did not execute; too many semaphores are already set.  ERROR_TOO_MANY_MUXWAITERS */
    EIO,            /* 153 The DosMuxSemWait list is not correct.  ERROR_INVALID_LIST_FORMAT */
    EIO,            /* 154 The volume label you entered exceeds the label character limit of the target file system.  ERROR_LABEL_TOO_LONG */
    ENOMEM,         /* 155 Cannot create another thread.  ERROR_TOO_MANY_TCBS */
    EIO,            /* 156 The recipient process has refused the signal.  ERROR_SIGNAL_REFUSED */
    EIO,            /* 157 The segment is already discarded and cannot be locked.  ERROR_DISCARDED */
    EIO,            /* 158 The segment is already unlocked.  ERROR_NOT_LOCKED */
    EFAULT,         /* 159 The address for the thread ID is not correct.  ERROR_BAD_THREADID_ADDR */
    ENOEXEC,        /* 160 The argument string passed to DosExecPgm is not correct.  ERROR_BAD_ARGUMENTS */
    ENOENT,         /* 161 The specified path is invalid.  ERROR_BAD_PATHNAME */
    EIO,            /* 162 A signal is already pending.  ERROR_SIGNAL_PENDING */
    -1,             /* 163 OS/2 ERROR_UNCERTAIN_MEDIA */
    ENOMEM,         /* 164 No more threads can be created in the system.  ERROR_MAX_THRDS_REACHED */
    -1,             /* 165 OS/2 ERROR_MONITORS_NOT_SUPPORTED */
    EIO,            /* 166 OS/2 ERROR_UNC_DRIVER_NOT_INSTALLED */
    EIO,            /* 167 Unable to lock a region of a file.  ERROR_LOCK_FAILED */
    EIO,            /* 168 OS/2 ERROR_SWAPIO_FAILED */
    EIO,            /* 169 OS/2 ERROR_SWAPIN_FAILED */
    EBUSY,          /* 170 The requested resource is in use.  ERROR_BUSY */
    -1,             /* 171 */
    -1,             /* 172 */
    EACCES,         /* 173 A lock request was not outstanding for the supplied cancel region.  ERROR_CANCEL_VIOLATION */
    ENOLCK,         /* 174 The file system does not support atomic changes to the lock type.  ERROR_ATOMIC_LOCKS_NOT_SUPPORTED */
    -1,             /* 175 OS/2 ERROR_READ_LOCKS_NOT_SUPPORTED */
    -1,             /* 176 */
    -1,             /* 177 */
    -1,             /* 178 */
    -1,             /* 179 */
    EFAULT,         /* 180 The system detected a segment number that was not correct.  ERROR_INVALID_SEGMENT_NUMBER */
    -1,             /* 181 OS/2 ERROR_INVALID_CALLGATE */
    ENOEXEC,        /* 182 The operating system cannot run %1.  ERROR_INVALID_ORDINAL */
    EEXIST,         /* 183 Cannot create a file when that file already exists.  ERROR_ALREADY_EXISTS */
    -1,             /* 184 OS/2 ERROR_NO_CHILD_PROCESS */
    -1,             /* 185 OS/2 ERROR_CHILD_ALIVE_NOWAIT */
    EINVAL,         /* 186 The flag passed is not correct.  ERROR_INVALID_FLAG_NUMBER */
    EINVAL,         /* 187 The specified system semaphore name was not found.  ERROR_SEM_NOT_FOUND */
    ENOEXEC,        /* 188 The operating system cannot run %1.  ERROR_INVALID_STARTING_CODESEG */
    ENOEXEC,        /* 189 The operating system cannot run %1.  ERROR_INVALID_STACKSEG */
    ENOEXEC,        /* 190 The operating system cannot run %1.  ERROR_INVALID_MODULETYPE */
    ENOEXEC,        /* 191 Cannot run %1 in Win32 mode.  ERROR_INVALID_EXE_SIGNATURE */
    ENOEXEC,        /* 192 The operating system cannot run %1.  ERROR_EXE_MARKED_INVALID */
    ENOEXEC,        /* 193 %1 is not a valid Win32 application.  ERROR_BAD_EXE_FORMAT */
    ENOEXEC,        /* 194 The operating system cannot run %1.  ERROR_ITERATED_DATA_EXCEEDS_64k */
    ENOEXEC,        /* 195 The operating system cannot run %1.  ERROR_INVALID_MINALLOCSIZE */
    ENOEXEC,        /* 196 The operating system cannot run this application program.  ERROR_DYNLINK_FROM_INVALID_RING */
    ENOEXEC,        /* 197 The operating system is not presently configured to run this application.  ERROR_IOPL_NOT_ENABLED */
    ENOEXEC,        /* 198 The operating system cannot run %1.  ERROR_INVALID_SEGDPL */
    ENOEXEC,        /* 199 The operating system cannot run this application program.  ERROR_AUTODATASEG_EXCEEDS_64k */
    ENOEXEC,        /* 200 The code segment cannot be greater than or equal to 64K.  ERROR_RING2SEG_MUST_BE_MOVABLE */
    ENOEXEC,        /* 201 The operating system cannot run %1.  ERROR_RELOC_CHAIN_XEEDS_SEGLIM */
    ENOEXEC,        /* 202 The operating system cannot run %1.  ERROR_INFLOOP_IN_RELOC_CHAIN */
    ENOEXEC,        /* 203 The system could not find the environment option that was entered.  ERROR_ENVVAR_NOT_FOUND */
    -1,             /* 204 OS/2 ERROR_NOT_CURRENT_CTRY */
    ENOEXEC,        /* 205 No process in the command subtree has a signal handler.  ERROR_NO_SIGNAL_SENT */
    EINVAL,         /* 206 The filename or extension is too long.  ERROR_FILENAME_EXCED_RANGE */
    ENOEXEC,        /* 207 The ring 2 stack is in use.  ERROR_RING2_STACK_IN_USE */
    EINVAL,         /* 208 The global filename characters, * or ?, are entered incorrectly or too many global filename characters are specified.  ERROR_META_EXPANSION_TOO_LONG */
    EIO,            /* 209 The signal being posted is not correct.  ERROR_INVALID_SIGNAL_NUMBER */
    EIO,            /* 210 The signal handler cannot be set.  ERROR_THREAD_1_INACTIVE */
    -1,             /* 211 OS/2 ERROR_INFO_NOT_AVAIL */
    EIO,            /* 212 The segment is locked and cannot be reallocated.  ERROR_LOCKED */
    -1,             /* 213 OS/2 ERROR_BAD_DYNALINK */
    ENOEXEC,        /* 214 Too many dynamic-link modules are attached to this program or dynamic-link module.  ERROR_TOO_MANY_MODULES */
    ENOEXEC,        /* 215 Cannot nest calls to LoadModule.  ERROR_NESTING_NOT_ALLOWED */
    ENOEXEC,        /* 216 The image file %1 is valid, but is for a machine type other than the current machine.  ERROR_EXE_MACHINE_TYPE_MISMATCH */
    -1,             /* 217 OS/2 ERROR_ZOMBIE_PROCESS */
    -1,             /* 218 OS/2 ERROR_STACK_IN_HIGH_MEMORY */
    -1,             /* 219 OS/2 ERROR_INVALID_EXITROUTINE_RING */
    -1,             /* 220 OS/2 ERROR_GETBUF_FAILED */
    -1,             /* 221 OS/2 ERROR_FLUSHBUF_FAILED */
    -1,             /* 222 OS/2 ERROR_TRANSFER_TOO_LONG */
    -1,             /* 223 OS/2 ERROR_FORCENOSWAP_FAILED */
    -1,             /* 224 OS/2 ERROR_SMG_NO_TARGET_WINDOW */
    -1,             /* 225 */
    -1,             /* 226 */
    -1,             /* 227 */
    -1,             /* 228 OS/2 ERROR_NO_CHILDREN */
    -1,             /* 229 OS/2 ERROR_INVALID_SCREEN_GROUP */
    EPIPE,          /* 230 The pipe state is invalid.  ERROR_BAD_PIPE */
    EBUSY,          /* 231 All pipe instances are busy.  ERROR_PIPE_BUSY */
    EPIPE,          /* 232 The pipe is being closed.  ERROR_NO_DATA */
    EPIPE,          /* 233 No process is on the other end of the pipe.  ERROR_PIPE_NOT_CONNECTED */
    EIO,            /* 234 More data is available.  ERROR_MORE_DATA */
    -1,             /* 235 */
    -1,             /* 236 */
    -1,             /* 237 */
    -1,             /* 238 */
    -1,             /* 239 */
    EIO,            /* 240 The session was canceled.  ERROR_VC_DISCONNECTED */
    -1,             /* 241 */
    -1,             /* 242 */
    -1,             /* 243 */
    -1,             /* 244 */
    -1,             /* 245 */
    -1,             /* 246 */
    -1,             /* 247 */
    -1,             /* 248 */
    -1,             /* 249 */
    -1,             /* 250 */
    -1,             /* 251 OS/2 ERROR_DIRECTORY_IN_CDS */
    -1,             /* 252 OS/2 ERROR_INVALID_FSD_NAME */
    -1,             /* 253 OS/2 ERROR_INVALID_PATH */
    EIO,            /* 254 The specified extended attribute name was invalid.  ERROR_INVALID_EA_NAME */
    EIO,            /* 255 The extended attributes are inconsistent.  ERROR_EA_LIST_INCONSISTENT */
    EIO             /*     - unmapped errors */
};

#define E_MAXERR       256  /* end of mapped errors */

#else

/* translation table of DOS error numbers to errno codes */

static signed char xlat[] = {
/* errno values        DOS error values */
    0,              /* 00 - no error */
    EINVAL,         /* 01 - invalid function number */
    ENOENT,         /* 02 - file not found */
    ENOENT,         /* 03 - path not found */
    EMFILE,         /* 04 - too many open files (no handles left) */
    EACCES,         /* 05 - access denied */
    EBADF,          /* 06 - invalid handle */
    ENOMEM,         /* 07 - memory control blocks destroyed */
    ENOMEM,         /* 08 - insufficient memory */
    ENOMEM,         /* 09 - invalid memory block address */
    E2BIG,          /* 10 - invalid environment */
    ENOEXEC,        /* 11 - invalid format */
    -1,             /* 12 - invalid access code */
    ERANGE,         /* 13 - invalid data */
    EEXIST,         /* 14 - (reserved) */
    ENODEV,         /* 15 - invalid drive was specified */
    EACCES,         /* 16 - attempt to remove current directory */
    EXDEV,          /* 17 - not same device */
    ENOENT,         /* 18 - no more files */
    -1              /*    - unknown error */
};

#define DOS_SHARERR     0x20    /* DOS 3.0, sharing violation */
#define DOS_MAXERR      0x22    /* DOS 3.0, invalid disk change */
#define DOS_EXIST       0x50    /* DOS 3.0, file already exists */

#define E_ACCESS        5       /* access denied */
#define E_EXIST         14      /* file already exists, DOS 3.0 */
#define E_MAXERR        19      /* unknown error, DOS 2.0 */
#endif

int _dosretax( int ax, int carry )
{
    if( carry == 0 ) {
        return( ax );
    }
    return( __set_errno_dos( ax ) );
}

int _dosret0( int ax, int carry )
{
    if( carry == 0 ) {
        return( 0 );
    }
    return( __set_errno_dos( ax ) );
}

_WCRTLINK int __set_errno_dos( unsigned int err )
{
#if defined(__NT__) || defined(__OS2__)
    __set_doserrno( err );
    // if we  can't map the Error code, use default EIO entry
    if( err > E_MAXERR )
        err = E_MAXERR;
    __set_errno( xlat[ err ] );
#else
    register unsigned char index;

    index = err & 0xff;
    __set_doserrno( index );
    if( err < 0x100 ) {
        if( _RWD_osmajor >= 3 ) {
            if( index == DOS_EXIST ) {
                index = E_EXIST;
            } else if( index >= DOS_MAXERR ) {
                index = E_MAXERR;
            } else if( index >= DOS_SHARERR ) {
                index = E_ACCESS;
            }
        }
        if( index > E_MAXERR )
            index = E_MAXERR;
        __set_errno( xlat[ index ] );
    } else {
        __set_errno( (err >> 8) & 0xff );
    }
#endif
    return( -1 );
}

_WCRTLINK int __set_errno_dos_reterr( unsigned int err )
{
    __set_errno_dos( err );
    return( err );
}


#ifdef __NT__
_WCRTLINK int __set_errno_nt( void )
{
    return( __set_errno_dos( GetLastError() ) );
}

_WCRTLINK int __set_errno_nt_reterr( void )
{
    return( __set_errno_dos_reterr( GetLastError() ) );
}
#endif
