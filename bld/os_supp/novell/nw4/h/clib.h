#ifndef _CLIB_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _CLIB_H_F39997A6_88FC_434B_B339_554BE343B3E8

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
* Description:  Defines NetWare 4.0 CLIB structures such as thread control
*               structures for use in the debugging of NetWare.
*
****************************************************************************/

#include <miniproc.h>

#define	TGCSSignature	TGCSSignature4
#define NCSSignature	NCSSignature4

#define NW_TCS4         struct NW_TCS4          /* 4.1 TCS */
#define NW_TGCS4        struct NW_TGCS4
#define NW_PCB4         struct NW_PCB4
#define NW_NCS4         struct NW_NCS4
#define NW_OSS4         struct NW_OSS4

#pragma pack (push, 1)

typedef NW_TCS4
{
    NW_TCS4 *           TCSLink;			/*	0x00	*/
	LONG				__Undefined00;
    NW_TGCS4 *          TCSTGCSp;			/*	0x08	*/
	LONG				__Undefined01[5];	
    LONG                TCSSuspendOrStop;   /*	0x20	*/
	/*
	//	rest of struct undefined
	*/
}T_ThreadStruct4, T_ThreadStruct;

/*
// ThreadGroupControlStructure  TGCS
*/
typedef NW_TGCS4
{
    NW_TGCS4 *          TGCSLink;
    LONG                TGCSStructID;   /* should be TGCSSignature                  */
    NW_NCS4 *           TGCSNCSp;       /* pointer to NCS                           */
    NW_TCS4 *           TGCSTCSLHead;   /* link to first TCS                        */
	/*
	//	rest of struct undefined
	*/
}T_ThreadGroupStruct4, T_ThreadGroupStruct;

#define __THREAD_NAME_SIZE  18
typedef NW_PCB4
{
	LONG				__Undefined00[5];
    LONG                UniquePCBID;						/*	0x14	*/
	LONG				__Undefined01[3];
    BYTE                ProcessName[__THREAD_NAME_SIZE];	/*	0x1E	*/
	/*
	//	rest of struct undefined
	*/
}T_PCBStruct4, T_PCBStruct;

/*
// NetWare Module Control Structure NCS
*/
typedef NW_NCS4
{
    struct LoadDefinitionStructure *    NCSNLMHandle;		/* +00                                      */
    LONG								NCSStructID;		/* +04 Should be NCSSignature               */
    LONG								__Undefined00[2];
    NW_TGCS4 *							NCSTGCSLHead;		/* +10 link to TGCS list                    */
    NW_OSS4 *							NCSOSSLHead;		/* +14 link to open screens                 */
    LONG								__Undefined01[25];	/* +18 */
    LONG								NCSExitCount;		/* +7C */
    LONG								__Undefined02;		/* +80 */
    LONG								NCSUnloadFunc;		/* +84 */
	/*
	//	rest of struct undefined
	*/
}T_NLMStruct4, T_NLMStruct;

/*
//  Open Screen Structs OSS
*/
typedef NW_OSS4
{
    NW_OSS4 *           OSSLink;					/* +00 */
    LONG                OSSStructID;				/* +04  OSS_SIGNATURE				*/
	struct ScreenStruct *OSSScreenID;				/* +08	ptr to NW386 ScreenStruct	*/
	int                 OSSGetchFlag;				/* +0C	// for special keys			*/
	LONG                OSSCursorShape;				/* +10	valid if HAS_CUSTOM_CURSOR	*/
	LONG                OSSSavedCursorShape;		/* +14	valid if HAS_CUSTOM_CURSOR	*/
	LONG                OSSActivateEventHandle;		/* +18 */
	LONG                OSSDeactivateEventHandle;	/* +1C */
	BYTE                OSSAttributes;				/* +20 */
}T_OpenScreenStruct4, T_OpenScreenStruct;

#pragma pack (pop)


CPP_START

int	ReserveABreakpointRTag(
	/* IN */	LONG					/* struct RTAG *						*/
	);

LONG RegisterDebuggerRTag( 
	/* IN */	struct debuggerStructure *,	/*	alternateDebugger,				*/
    /* IN */	int						/*	position							*/
	); 

LONG UnRegisterDebugger( 
	/* IN */	struct debuggerStructure *	/*	alternateDebugger,				*/
	); 

extern LONG GetFileServerMajorVersionNumber(
	void
);

extern LONG GetFileServerMinorVersionNumber(
	void
);

extern LONG GetFileServerRevisionNumber(
	void
);

CPP_END

#endif /* _CLIB_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
