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


#ifndef _fcb_h
#define _fcb_h

typedef unsigned char   byte;

#pragma pack(__push,1);
typedef struct an_fcb {
    byte                drive;          /* Drive number. 0 before open
                                           indicates default drive.  0 is
                                           replaced by the actual drive number
                                           (A = 1, B = 2, ... ) during open. */
    char                name[8];        /* Filename, left-justified with
                                           trailing blanks.  If a reserved
                                           device name is placed here (such as
                                           LPT1), do not include the optional
                                           colon. */
    char                ext[3];         /* Filename extension, left-justified
                                           with trailing blanks. */
    unsigned            curblock;       /* Current block number relative to the
                                           beginnning of the file, starting with
                                           0 (set to 0 by the open function
                                           call.)  A block consists of 128
                                           records, each of the size specified
                                           in the logical record size field.
                                           The current blcok number is used with
                                           the current record field for
                                           sequential reads and writes. */
    unsigned            recsize;        /* Logical record size in bytes.  Set to
                                           80h by the open function call.  If
                                           this is not correct, you must set the
                                           value because DOS uses it to
                                           determine the proper locations in the
                                           file for all disk reads and
                                           writes. */
} an_fcb;

typedef struct a_std_fcb {
    an_fcb              fcb;
    unsigned long       size;           /* File size in bytes. */
    unsigned            date;           /* Date the file was created or last
                                           updated.  The mm/dd/yy are mapped in
                                           the bits as follows:
                                             yyyyyyymmmmddddd
                                           where:  mm is 1-12, dd is 1-31,
                                           yy is 0-119 (1980-2099). */
    byte                res1[10];
    unsigned            currec;         /* Current relative record number
                                          (0-127) within the current block.  You
                                          must set this field before doing
                                          sequential read/write operations.
                                          This field is not initialized by the
                                          open function call. */
    unsigned long       randomrec;      /* Record number relative to the
                                           beginning of the file, starting with
                                           0.  You must set this field before
                                           doing random read/write operations.
                                           This field is not initialized by the
                                           open function call.
                                           If the record size is less than 64
                                           bytes, both words are used.
                                           Otherwise, only the first 3 bytes are
                                           used.  Note that if you use the FCB
                                           at 5dh in the PSP, the last byte of
                                           the FCP overlaps the first byte of
                                           the unformatted parameter area. */
} a_std_fcb;

typedef struct an_ext_fcb {
    byte                hexff;          /* Contains 0xff. */
    byte                res1[6];        /* Currently contain zeroes. */
    byte                attr;           /* Attribute byte. */
    a_std_fcb           fcb;
} an_ext_fcb;
#pragma pack(__pop);
#endif
