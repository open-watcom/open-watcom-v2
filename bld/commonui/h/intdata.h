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
* Description:  Fault context structure for Windows 3.x.
*
****************************************************************************/


#ifndef _INTDATA_H_INCLUDED
#define _INTDATA_H_INCLUDED

#pragma pack( __push, 1 )

typedef struct {
    unsigned short      SS;
    unsigned short      GS;
    unsigned short      FS;
    unsigned short      ES;
    unsigned short      DS;
    unsigned long       EDI;
    unsigned long       ESI;
    unsigned long       EBP;
    unsigned long       ESP;
    unsigned long       EBX;
    unsigned long       EDX;
    unsigned long       ECX;
    unsigned long       oldEAX;
    unsigned long       oldEBP;
    unsigned short      retIP;
    unsigned short      retCS;
    unsigned short      AX;
    unsigned short      intnumber;
    unsigned short      handle;
    unsigned short      IP;
    unsigned short      CS;
    unsigned short      FLAGS;
} fault_frame;

#pragma pack( __pop )

enum {
    KILL_APP = 0,
    RESTART_APP,
    CHAIN
};

#define EXCESS_CRAP_ON_STACK    0x14

void    RestoreState( interrupt_struct *idata, fault_frame *ff );
void    SaveState( interrupt_struct *idata, fault_frame *ff );

#endif /* _INTDATA_H_INCLUDED */
