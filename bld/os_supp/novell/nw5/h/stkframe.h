#ifndef _STKFRAME_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _STKFRAME_H_F39997A6_88FC_434B_B339_554BE343B3E8
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
* Description:  Details some of the NetWare 5.x structures that will be
*               passed to the debugger during software and hardware
*               exceptions.
*
****************************************************************************/

#include <ownwsupp.h>

/*
//	This info is from docs from Doug Thompson
*/

#pragma pack (push, 1)

typedef struct tagStackFrame /* V4.00 and 5+ */
{ 
	/* Reserved fields are undefined */ 
	LONG						ExceptionReserved[7]; 
	LONG *						ExceptionCR3; 
	LONG						ExceptionEIP; 
	LONG						ExceptionSystemFlags; 
	LONG						ExceptionEAX; 
	LONG						ExceptionECX; 
	LONG						ExceptionEDX; 
	LONG						ExceptionEBX; 
	LONG						ExceptionESP; 
	LONG						ExceptionEBP; 
	LONG						ExceptionESI; 
	LONG						ExceptionEDI; 
	WORD						ExceptionES[2]; 
	WORD						ExceptionCS[2]; 
	WORD						ExceptionSS[2]; 
	WORD						ExceptionDS[2]; 
	WORD						ExceptionFS[2]; 
	WORD						ExceptionGS[2];
	WORD						ExceptionLDT[2]; 
	WORD						ExceptionSpecial[2]; 
	LONG						ExceptionNumber; 
	BYTE *						ExceptionDescription; 
	LONG						ExceptionFlags; 
	LONG						ExceptionErrorCode; 
	LONG						ExceptionChainIO; 
	LONG						ExceptionChainMS; 
	LONG						ExceptionHistogram; 
	struct ProcessorStructure *	ExceptionProcessorID;
}T_TSS_StackFrame; 

struct ProcessorStructure  /* V4.00 */ 
{ 
	LONG						Reserved0; 
	struct ResourceTagStructure *MPRTag; 
	LONG						(*MPMainControlEntry)(LONG Function, ...); 
	LONG						(*MPNewControlEntry)(LONG Function, ...); 
	LONG						Reserved1[2]; 
	T_TSS_StackFrame *			MPStackFrame;
}; 

typedef T_TSS_StackFrame		StackFrame;


struct debuggerStructure
{
	struct debuggerStructure *		DDSlink;
	struct ResourceTagStructure *	DDSResourceTag;	/* 'DBUG' */
	LONG							(*DDSdebuggerEntry)(StackFrame *stackFrame);
	signed char						DDSPosition;/* changed from int for backwards compatibility*/
	BYTE							DDSFlags;
	BYTE							DDSReserved[2];
};

#pragma pack (pop)

#endif /* _STKFRAME_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
