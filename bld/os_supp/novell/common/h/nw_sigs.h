/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Portions Copyright (c) 1989-2002 Novell, Inc.  All Rights Reserved.                      
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
*   This header file was generated for the OpenWatcom project by Carl Young
*       carl.young@keycomm.co.uk
*   Any problems or updates required, please either contact the author or
*   the OpenWatcom contributors forums. 
*       http://www.openwatcom.com/
*
* Description:  Defines resource tags as used in AllocateResourceTag and
*               resource tracking functions. Some were well known, some were
*               viewed in situ and others were provided by Novell.
*
****************************************************************************/
/*
 *  carl.young@keycomm.co.uk
 *
 *  define missing resource tag values
 */
#ifndef _NW_SIGS_H
#define _NW_SIGS_H

#if !defined (DebuggerSignature)
#define DebuggerSignature		0x47554244		/* 'GUBD' */ 
#endif

#if !defined (BreakpointSignature)
#define BreakpointSignature		0x54504B42		/* 'TPKB' */ 
#endif

#if !defined (AllocSignature)
#define AllocSignature			0x54524C41		/* 'TRLA' */
#endif

#if !defined(ScreenSignature)
#define ScreenSignature         0x4E524353      /* 'SCRN' */
#endif

#if !defined(SemaphoreSignature)
#define SemaphoreSignature      0x504D4553      /* 'SEMP' */
#endif

#if !defined(SocketSignature)
#define SocketSignature         0x4B434F53      /* 'SOCK' */
#endif

#if !defined(ProcessSignature)
#define ProcessSignature        0x53435250      /* 'PRCS' */
#endif

#if !defined(TimerSignature)
#define TimerSignature          0x524D4954      /* 'TIMR' */
#endif

#if !defined(InterruptSignature)
#define InterruptSignature      0x50544E49      /* 'INTP' */
#endif

#if !defined(TCSSignature4)
#define TCSSignature4           0x20534354      /* 'TCS ' */
#endif

#if !defined(TCSSignature5)
#define TCSSignature5           0x54435320      /* ' SCT' - 'TCS ' reversed */
#endif

#if !defined(TGCSSignature4)
#define TGCSSignature4          0x53434754      /* 'TGCS' */
#endif

#if !defined(TGCSSignature5)
#define TGCSSignature5          0x54474353      /* 'SCGT' - 'TGCS' reversed */
#endif

#if !defined(PCBSignature)
#define PCBSignature            0x12345678
#endif

#if !defined(PCBNoSleepSignature)
#define PCBNoSleepSignature     0x12233445
#endif

#if !defined(NCSSignature4)
#define NCSSignature4           0x2053434E      /* 'NCS ' */
#endif

#if !defined(NCSSignature5)
#define NCSSignature5           0x4E435320      /* ' SCN' - 'NCS ' reversed */
#endif

#if !defined(OSSSignature4)
#define OSSSignature4           0x2053534F      /* 'OSS ' */
#endif

#if !defined(ASYNCIOSignature)
#define ASYNCIOSignature		0x4E595341		/* 'NYSA' */
#endif

#if !defined(AESProcessSignature)
#define AESProcessSignature		0x50534541		/* 'AESP' */
#endif

#if !defined(EventSignature)
#define EventSignature			0x544E5645		/* 'EVNT' */
#endif

#endif
