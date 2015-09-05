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
* Description:  Internal OS/2 trap file routines.
*
****************************************************************************/


#define OPEN_CREATE  1
#define OPEN_PRIVATE 2

#define NIL_DOS_HANDLE  ((HFILE)0xFFFF)
typedef enum { USER_SCREEN, DEBUG_SCREEN } scrtype;

#ifdef _M_I86

/*
 * from BSEDOS16.H, OS/2 2.0
 */
typedef struct _NEWSTARTDATA {  /* stdata */
    USHORT  Length;
    USHORT  Related;
    USHORT  FgBg;
    USHORT  TraceOpt;
    PSZ     PgmTitle;
    PSZ     PgmName;
    PBYTE   PgmInputs;
    PBYTE   TermQ;
    PBYTE   Environment;
    USHORT  InheritOpt;
    USHORT  SessionType;
    PSZ     IconFile;
    ULONG   PgmHandle;
    USHORT  PgmControl;
    USHORT  InitXPos;
    USHORT  InitYPos;
    USHORT  InitXSize;
    USHORT  InitYSize;
    USHORT  Reserved;
    PSZ     ObjectBuffer;
    ULONG   ObjectBuffLen;
} NEWSTARTDATA;

#define SSF_TYPE_DEFAULT        0
#define SSF_TYPE_FULLSCREEN     1
#define SSF_TYPE_WINDOWABLEVIO  2
#define SSF_TYPE_PM             3
#define SSF_TYPE_VDM            4
#define SSF_TYPE_GROUP          5
#define SSF_TYPE_DLL            6
#define SSF_TYPE_WINDOWEDVDM    7
#define SSF_TYPE_PDD            8
#define SSF_TYPE_VDD            9


/* Global Information Segment */

typedef struct __GINFOSEG {      /* gis */
    ULONG   time;
    ULONG   msecs;
    UCHAR   hour;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    USHORT  timezone;
    USHORT  cusecTimerInterval;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    UCHAR   weekday;
    UCHAR   uchMajorVersion;
    UCHAR   uchMinorVersion;
    UCHAR   chRevisionLetter;
    UCHAR   sgCurrent;
    UCHAR   sgMax;
    UCHAR   cHugeShift;
    UCHAR   fProtectModeOnly;
    USHORT  pidForeground;
    UCHAR   fDynamicSched;
    UCHAR   csecMaxWait;
    USHORT  cmsecMinSlice;
    USHORT  cmsecMaxSlice;
    USHORT  bootdrive;
    UCHAR   amecRAS[32];
    UCHAR   csgWindowableVioMax;
    UCHAR   csgPMMax;
} __GINFOSEG;

/* Local Information Segment */

typedef struct __LINFOSEG {      /* lis */
    PID     pidCurrent;
    PID     pidParent;
    USHORT  prtyCurrent;
    TID     tidCurrent;
    USHORT  sgCurrent;
    UCHAR   rfProcStatus;
    UCHAR   dummy1;
    BOOL    fForeground;
    UCHAR   typeProcess;
    UCHAR   dummy2;
    SEL     selEnvironment;
    USHORT  offCmdLine;
    USHORT  cbDataSegment;
    USHORT  cbStack;
    USHORT  cbHeap;
    HMODULE hmod;
    SEL     selDS;
} __LINFOSEG;

/* Process Type codes (local information segment typeProcess field)           */

#define _PT_FULLSCREEN              0 /* Full screen application               */
#define _PT_REALMODE                1 /* Real mode process                     */
#define _PT_WINDOWABLEVIO           2 /* VIO windowable application            */
#define _PT_PM                      3 /* Presentation Manager application      */
#define _PT_DETACHED                4 /* Detached application                  */

#endif

char        *StrCopy( char *, char * );
long        OpenFile( char *, USHORT, int );
void        RestoreScreen( void );
long        TryPath( char *, char *, const char * );
long        FindFilePath( char *, char *, const char * );
char        *AddDriveAndPath( char *, char * );
void        MergeArgvArray( char *, char *, unsigned );
long        TaskOpenFile( char *name, int mode, int flags );
HFILE       TaskDupFile( HFILE old, HFILE new );
long        TaskCloseFile( HFILE hdl );
