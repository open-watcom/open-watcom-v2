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


/*
 *      MS-DOS definitions for C runtime
 */


#define _NFILE          20              /* maximum # files per process */

#define STACKSLOP       256             /* stack slop for interrupt overhead */


        /* _osfile flag values for DOS file handles */

#define FOPEN           0x01            /* file handle open */
#define FEOFLAG         0x02            /* end of file has been encountered */
#define FDEV            0x40            /* file handle refers to device */
#define FTEXT           0x80            /* file handle is in text mode */
#define FAPPEND         0x20            /* file handle opened O_APPEND */
#define FRDONLY         0x10            /* file handle associated with read only file */

#define DOS             0x21            /* MS-DOS interrupt */

#define DOS_exit        0x00000         /* exit offset          (PSP:0000) */
#define DOS_maxpara     0x00002         /* maximum paragraph    (PSP:0002) */
#define DOS_envp        0x0002c         /* environment address  (PSP:002c) */
#define DOS_cmdline     0x00080         /* command line offset  (PSP:0080) */

#define DOS_kill        0x00            /* terminate */
#define DOS_echoread    0x01            /* read keyboard and echo */
#define DOS_display     0x02            /* display character */
#define DOS_auxinput    0x03            /* auxiliary input */
#define DOS_auxoutput   0x04            /* auxiliary output */
#define DOS_print       0x05            /* print character */
#define DOS_conio       0x06            /* direct console i/o */
#define DOS_coninput    0x07            /* direct console input */
#define DOS_readkbd     0x08            /* read keyboard */
#define DOS_message     0x09            /* display string */
#define DOS_bufkbdin    0x0a            /* buffered keyboard input */
#define DOS_kbdstatus   0x0b            /* check keyboard status */
#define DOS_flshread    0x0c            /* flush buffer and read keyboard */
#define DOS_diskreset   0x0d            /* disk reset */
#define DOS_selectdisk  0x0e            /* select default disk */
#define DOS_fcbopen     0x0f            /* open file with fcb */
#define DOS_fcbclose    0x10            /* close file with fcb */
#define DOS_fcbfirst    0x11            /* search for first entry with fcb */
#define DOS_fcbnext     0x12            /* search for next entry with fcb */
#define DOS_fcbdelete   0x13            /* delete file with fcb */
#define DOS_fcbsread    0x14            /* sequential read with fcb */
#define DOS_fcbswrite   0x15            /* sequential write with fcb */
#define DOS_fcbcreate   0x16            /* create file with fcb */
#define DOS_fcbrename   0x17            /* rename file with fcb */
#define DOS_currentd    0x19            /* current default disk */
#define DOS_setDMA      0x1a            /* set DMA */
#define DOS_fcbrread    0x21            /* random read with fcb */
#define DOS_fcbrwrite   0x22            /* random write with fcb */
#define DOS_fcbsize     0x23            /* file size with fcb */
#define DOS_fcbsetrec   0x24            /* set relative record with fcb */
#define DOS_setvector   0x25            /* set interrupt vector */
#define DOS_fcbbread    0x27            /* random block read with fcb */
#define DOS_fcbbwrite   0x28            /* random block write with fcb */
#define DOS_fcbparse    0x29            /* parse file name with fcb */
#define DOS_getdate     0x2a            /* get date */
#define DOS_setdate     0x2b            /* set date */
#define DOS_gettime     0x2c            /* get time */
#define DOS_settime     0x2d            /* set time */
#define DOS_verify      0x2e            /* set/reset verify flag */
#define DOS_getDMA      0x2f            /* get DMA */
#define DOS_version     0x30            /* get version number */
#define DOS_keep        0x31            /* keep process */
#define DOS_cntlc       0x33            /* Cntl-C check */
#define DOS_dirty       0x34            /* get dos dirty bit (UNDOCUMENTED!) */
#define DOS_getvector   0x35            /* get interrupt vector */
#define DOS_getdskspc   0x36            /* get disk free space */
#define DOS_country     0x38            /* get country dependent info */
#define DOS_mkdir       0x39            /* make subdirectory */
#define DOS_rmdir       0x3a            /* remove subdirectory */
#define DOS_chdir       0x3b            /* change subdirectory */
#define DOS_create      0x3c            /* create pathname */
#define DOS_open        0x3d            /* open pathname */
#define DOS_close       0x3e            /* close file handle */
#define DOS_read        0x3f            /* read from file handle */
#define DOS_write       0x40            /* write from file handle */
#define DOS_delete      0x41            /* delete pathname */
#define DOS_lseek       0x42            /* move file pointer */
#define DOS_filemode    0x43            /* get/set attributes of pathname */
#define DOS_ioctl       0x44            /* ioctl for devices */
#define DOS_dup         0x45            /* duplicate file handle */
#define DOS_forcedup    0x46            /* force duplicate file handle */
#define DOS_curdir      0x47            /* get current directory */
#define DOS_allocmem    0x48            /* allocate memory block */
#define DOS_freemem     0x49            /* free memory block */
#define DOS_setmem      0x4a            /* set size of memory block */
#define DOS_exec        0x4b            /* load and execute program */
#define DOS_terminate   0x4c            /* terminate process with errorcode */
#define DOS_wait        0x4d            /* get child process return code */
#define DOS_findfirst   0x4e            /* find first file match */
#define DOS_findnext    0x4f            /* find next file match */
#define DOS_getverify   0x54            /* return current verify flag */
#define DOS_rename      0x56            /* rename pathname */
#define DOS_filedate    0x57            /* get/set file handle date/time */
#define DOS_locking     0x5C            /* file record locking/unlocking */
#define DOS_sleep       0x89            /* delay process execution */


        /* DOS error codes */

#define E_ifunc         1
#define E_nofile        2
#define E_nopath        3
#define E_toomany       4
#define E_access        5
#define E_ihandle       6
#define E_arena         7
#define E_nomem         8
#define E_iblock        9
#define E_badenv        10
#define E_badfmt        11
#define E_iaccess       12
#define E_idata         13
#define E_unknown       14
#define E_idrive        15
#define E_curdir        16
#define E_difdev        17
#define E_nomore        18
#define E_maxerr2       19

        /* the following errors can occur only in DOS 3.0 */

#define E_sharerr       32
#define E_lockerr       33
#define E_maxerr3       34


        /* DOS file attributes */

#define A_ro            0x01            /* read-only file */
#define A_h             0x02            /* hidden */
#define A_s             0x04            /* system */
#define A_v             0x08            /* volume ID */
#define A_d             0x10            /* directory */
#define A_a             0x20            /* archive */

#define A_mod           (A_ro|A_h|A_s|A_a)      /* modifiable attributes */
