#ifndef _MINIPROC_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _MINIPROC_H_F39997A6_88FC_434B_B339_554BE343B3E8
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
* Description:  Defines most of the O/S level APIs required by OpenWatcom
*               tools. This is the not the full miniproc.h from Novell.
*
****************************************************************************/

#include <ownwsupp.h>

#include <debugapi.h>
#include <loader.h>		/* defines MODULE_HANDLE (LoadDefinitionStructure) */
#include <nw_sigs.h>
#include <ownwcnio.h>
#include <ecb.h>

CPP_START
/*
// forward define required structures that we don't want to expose
*/
struct ScreenStruct;
struct ResourceTagStructure;
struct LoadDefinitionStructure;

extern struct LoadDefinitionStructure *	    LoadedList;
extern struct ScreenStruct *		        systemConsoleScreen;
extern LONG							        FileServerMajorVersionNumber;

CPP_END

CPP_START

extern void CRescheduleLast(
	void
	);

extern void CDestroyProcess(
	LONG							processID
);

extern LONG CMakeProcess(
	LONG							schedulingPriority,
	void							(*codeAddress)(void),
	void *							stackTopAddress,
	LONG							stackLength,
	void *							processName,
	struct ResourceTagStructure *	RTag);

extern LONG CGetMyProcessID(
	void
);

struct ResourceTagStructure *AllocateResourceTag(
	struct LoadDefinitionStructure *NLMHandle,
	const BYTE *					descriptionString,
	LONG							resourceType
);

extern void OutputToScreen(
	struct ScreenStruct *			screenID,
	void *							controlString,
	...
);

extern void OutputToScreenWithPointer(
	struct ScreenStruct *			screenID,
	void *							controlString,
	void *							arguments
);

extern void CloseScreen(
	struct ScreenStruct *			screenID
);

extern struct ScreenStruct *GetSystemConsoleScreen(
	void
);

extern void EnableInputCursor(
	struct ScreenStruct *			screenID
);

extern void SetInputToOutputCursorPosition(
	struct ScreenStruct *			screenID
);

extern LONG OpenScreen(
	void *							screenName,
	struct ResourceTagStructure *	resourceTag,
	struct ScreenStruct **			newScreenID
);

extern void ActivateScreen(
	struct ScreenStruct *			screenID
);

extern void *Alloc(
	LONG							numberOfBytes,
	struct ResourceTagStructure *	RTag
);

extern void Free(
	void *							address
);

extern void CDeAllocateSemaphore(
	LONG							semaphoreNumber
);

extern void CSleepUntilInterrupt(
	void
);

extern void CPSemaphore(
	LONG							semaphoreNumber
);

extern void CVSemaphore(
	LONG							semaphoreNumber
);

extern LONG GetRunningProcess(
	void
);

extern LONG CAllocSemaphore(
	LONG							initialSemaphoreValue,
	struct ResourceTagStructure *	RTag
);

extern LONG LoadModule(
	struct ScreenStruct *			screenID,
	BYTE *							commandLine,
	LONG							loadOptions
);

extern void KillMe(
	struct LoadDefinitionStructure *MyLoadRecord);

extern void Enable(
	void
);

extern void Disable(
	void
);

void UnReserveABreakpoint(
	/* IN */	int						/* BP number							*/
	);

extern void CSemaphoreReleaseAll(
	LONG								semaphoreNumber
	);

int	CSetABreakpoint(
	/* IN */	int,					/* breakpoint number					*/
	/* IN */	LONG,					/* address								*/
	/* IN */	int,					/* type									*/
	/* IN */	LONG					/* size									*/
	);

/*
//
//		from miniproc.h
*/

extern LONG CheckKeyStatus(
	struct ScreenStruct *				screenID
	);

extern void GetKey(
	struct ScreenStruct *				screenID,
	BYTE *								keyType,
	BYTE *								keyValue,
	BYTE *								keyStatus,
	BYTE *								scanCode,
	LONG								linesToProtect
	);

extern LONG OpenFile(
	LONG								Station,
	LONG								Task,
	LONG								Volume,
	LONG								PathBase,
	BYTE *								PathString,
	LONG								PathCount,
	LONG								NameSpace,
	LONG								MatchBits,
	LONG								RequestedRights,
	BYTE								DataStreamNumber,
	LONG *								Handle,
	LONG *								DirectoryNumber,
	void **								DirectoryEntry
);

extern LONG CreateFile(
		LONG Station,
		LONG Task,
		LONG Volume,
		LONG PathBase,
		BYTE *PathString,
		LONG PathCount,
		LONG NameSpace,
		LONG CreatedAttributes,
		LONG FlagBits,
		BYTE DataStreamNumber,
		LONG *Handle,
		LONG *DirectoryNumber,
		void **DirectoryEntry);

extern LONG WriteFile(
		LONG stationNumber,
		LONG handle,
		LONG startingOffset,
		LONG bytesToWrite,
		void *buffer);

extern LONG ReadFile(
		LONG stationNumber,
		LONG handle,
		LONG startingOffset,
		LONG bytesToRead,
		LONG *actualBytesRead,
		void *buffer);

extern LONG INWDOSRead(
		LONG handle,
		LONG offset,
		void *buffer,
		LONG numberOfBytes,
		LONG *bytesRead);

/*
//
//		from miniproc.h
*/
void Abend(
	/* IN */	void *				/*	abendDescription						*/
	);

LONG OpenFileUsingSearchPath(
		BYTE *fileNameAndPath,
		LONG *handle,
		BYTE *isDOSFlag,
		BYTE *openedFilePath,
		BYTE *openedFileName,
		BYTE overrideExistingExtension,
		LONG numberOfExtensions,
		...);

extern LONG INWDOSFindFirstFile(
		BYTE *fileName,
		WORD attributes,
		void *DTA);

extern LONG CloseFile(
		LONG Station,
		LONG Task,
		LONG Handle);

extern LONG GetFileSize(
		LONG station,
		LONG handle,
		LONG *filesize);

extern LONG INWDOSClose(
		LONG handle);

extern LONG MapAbsoluteAddressToDataOffset(
	LONG	AbsoluteAddress
	);

extern LONG MapAbsoluteAddressToCodeOffset(
	LONG	AbsoluteAddress
	);

extern LONG MapDataOffsetToAbsoluteAddress(
	LONG	DataOffset
	);

extern LONG MapCodeOffsetToAbsoluteAddress(
	LONG	CodeOffset
	);

extern LONG GetServerPhysicalOffset(
	void
	);

extern LONG RegisterForEventNotification(
	struct ResourceTagStructure *	resourceTag,
	LONG							eventType,
	LONG							priority,
	LONG							(*warnProcedure)(
										void (*OutputRoutine)(void *controlString, ...),
										LONG parameter),
	void							(*reportProcedure)(
										LONG parameter)
	);

extern LONG UnRegisterEventNotification(
	LONG eventID
	);

CPP_END

#endif /* _MINIPROC_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
