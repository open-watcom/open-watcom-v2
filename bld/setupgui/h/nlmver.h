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



#ifndef RVERHDR
#define RVERHDR

#define MAX_DESCRIPTION_LENGTH  127
#define ESUCCESS    0
#define EFAILURE    -1
#define TRUE        1
#define FALSE       0
#define READ_SIZE   512
#define MAX_SCREEN_NAME_LENGTH 71
#define OLD_THREAD_NAME_LENGTH 5
#define MAX_THREAD_NAME_LENGTH 17

typedef struct _NLMHDR
{
        BYTE    signature[24];
        LONG    version;
        BYTE    moduleName[14];
        LONG    reserved[22];
        BYTE    descriptionLength;
        BYTE    descriptionText[ MAX_DESCRIPTION_LENGTH + 1 ];/* variable length  */
        LONG    reserved2[2];                                 /* for use by C Lib */
        BYTE    oldThreadName[OLD_THREAD_NAME_LENGTH];        /* for use by C Lib */
        BYTE    screenNameLength;
        BYTE    screenName[ MAX_SCREEN_NAME_LENGTH + 1 ];     /* variable length  */
        BYTE    threadNameLength;
        BYTE    threadName[ MAX_THREAD_NAME_LENGTH + 1 ];     /* variable length  */
        BYTE    otherData[400];
} NLMHDR;

int     ReturnNLMVersionInfoFromFile(char *,LONG *,LONG *,LONG *,LONG *,LONG *,LONG *,char *,char *);

#endif
