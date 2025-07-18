/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS function calls (extension of <dosfunc.h> header file)
*
****************************************************************************/


#define DOS_GET_CHAR_ECHO_CHECK     0x01
#define DOS_OUT_CHAR_CHECK          0x02
#define DOS_GET_CHAR_AUX            0x03
#define DOS_OUT_CHAR_AUX            0x04
#define DOS_OUT_CHAR_PRINTER        0x05
#define DOS_OUT_CHAR                0x06
#define DOS_GET_CHAR_NO_ECHO        0x07    /* dosfunc.h */
#define DOS_GET_CHAR_NO_ECHO_CHECK  0x08
#define DOS_BUFF_INPUT              0x0A
#define DOS_INPUT_STATUS            0x0B
#define DOS_SET_DRIVE               0x0E
#define DOS_DELETE_FCB              0x13
#define DOS_CUR_DISK                0x19    /* dosfunc.h */
#define DOS_SET_DTA                 0x1A    /* dosfunc.h */
#define DOS_GET_DEF_DRIVE_INFO      0x1B
#define DOS_GET_DRIVE_INFO          0x1C
#define DOS_SET_INT                 0x25    /* dosfunc.h */
#define DOS_CREATE_PSP              0x26
#define DOS_PARSE_FCB               0x29
#define DOS_GET_DATE                0x2A    /* dosfunc.h */
#define DOS_SET_DATE                0x2B
#define DOS_GET_TIME                0x2C    /* dosfunc.h */
#define DOS_SET_TIME                0x2D
#define DOS_SET_VERIFY_FLAG         0x2E
#define DOS_GET_DTA                 0x2F
#define DOS_GET_VERSION             0x30    /* dosfunc.h */
#define DOS_TERM_STAY_RESID         0x31
#define DOS_GET_DRIVE_PARAM_BLOCK   0x32
#define DOS_CTRL_BREAK              0x33    /* dosfunc.h */
#define DOS_GET_INT                 0x35    /* dosfunc.h */
#define DOS_DRIVE_FREE_SPACE        0x36
#define DOS_SWITCH_CHAR             0x37    /* dosfunc.h */
#define DOS_COUNTRY_INFO            0x38
#define DOS_MKDIR                   0x39    /* dosfunc.h */
#define DOS_RMDIR                   0x3A    /* dosfunc.h */
#define DOS_CHDIR                   0x3B    /* dosfunc.h */
#define DOS_CREAT                   0x3C    /* dosfunc.h */
#define DOS_OPEN                    0x3D    /* dosfunc.h */
#define DOS_CLOSE                   0x3E    /* dosfunc.h */
#define DOS_READ                    0x3F    /* dosfunc.h */
#define DOS_WRITE                   0x40    /* dosfunc.h */
#define DOS_UNLINK                  0x41    /* dosfunc.h */
#define DOS_LSEEK                   0x42    /* dosfunc.h */
#define DOS_CHMOD                   0x43    /* dosfunc.h */
#define DOS_IOCTL                   0x44    /* dosfunc.h */
#define DOS_DUP                     0x45    /* dosfunc.h */
#define DOS_DUP2                    0x46    /* dosfunc.h */
#define DOS_GETCWD                  0x47    /* dosfunc.h */
#define DOS_ALLOC_SEG               0x48    /* dosfunc.h */
#define DOS_FREE_SEG                0x49    /* dosfunc.h */
#define DOS_MODIFY_SEG              0x4A    /* dosfunc.h */
#define DOS_EXEC                    0x4B
#define DOS_EXIT                    0x4C    /* dosfunc.h */
#define DOS_CHILD_RC                0x4D
#define DOS_FIND_FIRST              0x4E    /* dosfunc.h */
#define DOS_FIND_NEXT               0x4F    /* dosfunc.h */
#define DOS_SET_PSP                 0x50
#define DOS_GET_PSP                 0x51
#define DOS_GET_LIST_OF_LIST        0x52
#define DOS_RENAME                  0x56    /* dosfunc.h */
#define DOS_FILE_DATE               0x57    /* dosfunc.h */
#define DOS_MEM_STRATEGY            0x58
#define DOS_EXT_ERR                 0x59
#define DOS_CREATE_TMP              0x5A
#define DOS_CREATE_NEW              0x5B
#define DOS_RECORD_LOCK             0x5C
#define DOS_TRUENAME                0x60
#define DOS_GET_PSP_2               0x62
#define DOS_SET_HCOUNT              0x67
#define DOS_COMMIT_FILE             0x68    /* dosfunc.h */
#define DOS_EXT_CREATE              0x6C
