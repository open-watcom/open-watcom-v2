#ifndef _STKFRAME_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5
#define _STKFRAME_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5

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
* Description:  This file details the exception structures that are used
*               by the NetWare 386 debugger on NetWare version 3.x.
*
****************************************************************************/

#include <ownwsupp.h>

#pragma pack (push, 1)

typedef struct StackFrame3X
{ 
/* Reserved fields are undefined */ 
	WORD						ExceptionGS[2]; 
	WORD						ExceptionFS[2]; 
	WORD						ExceptionES[2]; 
	WORD						ExceptionDS[2]; 
	LONG						ExceptionEDI; 
	LONG						ExceptionESI; 
	LONG						ExceptionEBP; 
	LONG						ExceptionESP; 
	LONG						ExceptionEBX; 
	LONG						ExceptionEDX; 
	LONG						ExceptionECX; 
	LONG						ExceptionEAX; 
	LONG						ExceptionNumber; 
	BYTE *						ExceptionDescription; 
	LONG						ExceptionFlags; 
	LONG						ExceptionErrorCode; 
	LONG						ExceptionEIP; 
	WORD						ExceptionCS[2]; 
	LONG						ExceptionSystemFlags;
}T_StackFrame; 

#define exceptionNumber			ExceptionNumber
#define exceptionDescription	ExceptionDescription
#define	errorCode				ExceptionErrorCode
#define	EFLAGS					ExceptionFlags
#define	EAX						ExceptionEAX
#define	EBX						ExceptionEBX
#define	ECX						ExceptionECX
#define	EDX						ExceptionEDX
#define	ESI						ExceptionESI
#define	EDI						ExceptionEDI
#define	EIP						ExceptionEIP
#define	EBP						ExceptionEBP
#define	ESP						ExceptionESP
#define	CS						ExceptionCS
#define	DS						ExceptionDS
#define	ES						ExceptionES
#define	FS						ExceptionFS
#define	GS						ExceptionGS
#define	SS						ExceptionSS


typedef struct DebuggerStruct3X
{
	LONG							Undefined0;		/* LINK ? */
	struct ResourceTagStructure *	ddRTag; 
	LONG							(*DDSdebuggerEntry)(T_StackFrame *stackFrame);
}T_DebuggerStruct;

#pragma pack (pop)


#endif /* _STKFRAME_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5 */
