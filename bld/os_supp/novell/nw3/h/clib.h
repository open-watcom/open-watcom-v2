#ifndef _CLIB_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5
#define _CLIB_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5

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
* Description:  Details some of the NetWare 3.x structures that will be
*               passed to the debugger during software and hardware
*               exceptions.
*
****************************************************************************/

#include <ownwsupp.h>
#include <miniproc.h>

struct	LoadDefinitionStructure;

typedef struct ExternalPublicDefinitionStructure
{
	struct ExternalPublicDefinitionStructure *	EPDLink;
	void *										___EPDAddress;
	BYTE *										EPDName;
	LONG										___EPDType;
}EPDSTRUCTURE;

extern struct ExternalPublicDefinitionStructure *	ExternalPublicList;

extern LONG		RunningProcess;
extern LONG		FileServerMajorVersionNumber;
extern LONG		FileServerMinorVersionNumber;

#pragma pack (push, 1)

#define __THREAD_NAME_SIZE  18

#define NW_TCS3		struct NW_TCS3
#define NW_TGCS3	struct NW_TGCS3
#define NW_NCS3		struct NW_NCS3
#define NW_OSS3		struct NW_OSS3

typedef NW_TCS3
{
	LONG			____Undefined00[5];						/*	0x00	*/
	LONG			PCBStructID;							/*	0x14	*/
	LONG			____Undefined01[1];						/*	0x18	*/
	BYTE			____Undefined02[2];						/*	0x1C	*/
	MEON			PCBProcessName[__THREAD_NAME_SIZE];		/*	0x1E	*/
	NW_TGCS3 *		TCSTGCSp;								/*	0x30	*/
	NW_TCS3	*		TCSLink;								/*	0x34	*/
	LONG			____Undefined03[6];						/*	0x38	*/
	BYTE			TCSSuspendOrStop;						/*	0x50	*/
	/*
	//	rest of struct undefined
	*/
}T_ThreadControlStruct;	/* this is a PCB!!!!! */

typedef NW_TGCS3
{
	NW_TGCS3 *		TGCSLink;
	LONG			TGCSStructID;
	NW_NCS3 *		TGCSNCSp;
	NW_TCS3 *		TGCSTCSLHead;
	/*
	//	rest of struct undefined
	*/
}T_ThreadGroupControlStruct;

#define TGCSSignature3	TGCSSignature4
#define TGCSSignature	TGCSSignature3

typedef NW_NCS3
{
	struct LoadDefinitionStructure *	NCSNLMHandle;			/*	0x00	*/
	LONG								NCSStructID;			/*	0x04	*/
	LONG								____Undefined00[2];		/*	0x08	*/
	NW_TGCS3 *							NCSTGCSLHead;			/*	0x10	*/
	NW_OSS3 *							NCSOSSLHead;			/*	0x14	*/
	LONG								____Undefined01[23];	/*	0x18	*/
	LONG								NCSExitCount;			/*	0x74	*/
	LONG								____NCSStartTime;		/*	0x78	*/
	void								(*NCSUnloadFunc)(void);	/*	0x7C	*/
	/*
	//	rest of struct undefined
	*/
}T_NLMControlStruct;

#define NCSSignature3	NCSSignature4
#define NCSSignature	NCSSignature3

typedef NW_OSS3
{
	NW_OSS3	*							OSSLink;				/*	0x00	*/
	LONG								____OSSStructID;		/*	0x04	*/
	LONG *								____OSSScreenID;		/*	0x08	*/
	LONG								____Undefined00[5];		/*	0x0C	*/
	BYTE								OSSAttributes;			/*	0x20	*/
}T_OpenScreenStruct;

#pragma pack (pop)

CPP_START

int	ReserveABreakpointRTag(
	struct ResourceTagStructure * pRTag
	);

LONG RegisterDebuggerRTag( 
	/* IN */	struct DebuggerStruct3X *,	/*	alternateDebugger,				*/
    /* IN */	int						/*	position							*/
	); 

LONG UnRegisterDebugger( 
	/* IN */	struct DebuggerStruct3X *	/*	alternateDebugger,				*/
	); 

CPP_END

#endif /* _CLIB_H_6E48ECAB_182D_497B_949E_6CDFEE6824A5 */
