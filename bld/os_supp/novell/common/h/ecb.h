#ifndef _ECB_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _ECB_H_F39997A6_88FC_434B_B339_554BE343B3E8
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
* Description:  Defines Netware's low level ECB (Event Control Blocks) and
*               IPX/SPX structures.
*
****************************************************************************/

#include <ownwsupp.h>

struct ResourceTagStructure;

#pragma pack (push, 1)

typedef struct IPXAddress
{
    BYTE        network[4];     /* high-low */
    BYTE        node[6];        /* high-low */
    BYTE        socket[2];      /* high-low */
} IPXAddress;

typedef struct IPXHeader
{
    WORD        checkSum;       /* high-low */
    WORD        length;         /* high-low */
    BYTE        transportControl;
    BYTE        packetType;
    IPXAddress  destination;
    IPXAddress  source;
} IPXHeader;

typedef struct SPXHeader
{
    WORD        checksum;
    WORD        length;                 /* high-low*/
    BYTE        transportControl;
    BYTE        packetType;
    IPXAddress  destination;
    IPXAddress  source;
    BYTE        connectionControl;      /* bit flags */
    BYTE        dataStreamType;
    WORD        sourceConnectionID;     /* high-low unsigned */
    WORD        destConnectionID;       /* high-low unsigned */
    WORD        sequenceNumber;         /* high-low unsigned */
    WORD        acknowledgeNumber;      /* high-low unsigned */
    WORD        allocationNumber;       /* high-low unsigned */
} SPXHeader;

typedef struct ECBFragment {
    void     *address;
    LONG     size;                      /* low-high */
} ECBFragment;

typedef void (*ECB_ESRFunc)();

typedef struct ECB {
    void        *fLinkAddress;
    void        *bLinkAddress;
    WORD        status;
    ECB_ESRFunc ESRAddress;
    WORD        stackID;
    BYTE        protocolID[6];
    LONG        boardNumber;
    BYTE        immediateAddress[6];    /* high-low */
    BYTE        driverWorkspace[4];     /* N/A */
    LONG        ESREBXValue;
    WORD        socketNumber;           /* high-low */
    WORD        protocolWorkspace;      /* Session ID */
    LONG        packetLength;
    LONG        fragmentCount;          /* low-high */
    ECBFragment fragmentDescriptor[2];
} ECB;

/* This structure is idendical to the C structure TimeCallBackStructure */
/* CSL_TimeCallBackStructure is defined here so that CSL applications   */
/* do not need to include the C header file advanced.h, as the file     */
/* advanced.h may conflict with NetWare include files that some CSL     */
/* applications are using                                               */
typedef struct TimeCallBackStructure
   {
   struct TimeCallBackStructure *	TLink; /* Set by CScheduleInterruptTimeCallBack */
   void								(*TCallBackProcedure)(	LONG parameter);              /* Set to function to call; unchanged */
   LONG								TCallBackParameter;           /* Set to value to pass to func; unchanged */
   LONG								TCallBackWaitTime;            /* Set to # ticks to wait; unchanged; 
																		looked at only when call is made */
   //LONG								TResourceTag;                 /* Set to resource tag; unchanged */
   struct ResourceTagStructure *	TResourceTag;				  /* Set to resource tag; unchanged */
   LONG								TWorkWakeUpTime;              /* Set by CScheduleInterruptTimeCallBack */
   LONG								TSignature;                   /* Set by CScheduleInterruptTimeCallBack */
}TimeCallBackStructure;

#define TCallBackEBXParameter	TCallBackParameter
#define	TimerDataStructure		TimeCallBackStructure

#pragma pack (pop)

CPP_START

LONG CIPXCancelECB(
		void *eventControlBlock);

LONG CIPXCheckForSocket(
		LONG socketNumber);

LONG CIPXCloseSocket(
		LONG socketNumber);

LONG CIPXCountReceiveECBs(
		LONG socketNumber);

LONG CIPXGetECB(
		LONG socketNumber);

void CIPXGetInternetworkAddress(
		BYTE *networkAddress);

LONG CIPXGetLocalTarget(
		BYTE *networkAddress,
		BYTE *immediateAddress,
		LONG *transportTime);

LONG CIPXListen(
		void *eventControlBlock);

LONG CIPXOpenSocket(
		LONG *socketNumber);

LONG CIPXSendPacket(
		void *eventControlBlock);

LONG CIPXSendPacketSkipChkSum(
		void *eventControlBlock);

LONG CSPXAbortConnection(
		LONG sessionID);

LONG CSPXCancelSessionListen(
		void *eventControlBlock);

LONG CSPXCheckInstallation(
		WORD *version,
		LONG *maxConnections);

LONG CSPXEstablishConnection(
		BYTE retryCount,
		BYTE watchDog,
		LONG *sessionID,
		void *eventControlBlock);

LONG CSPXGetConnectionStatus(
		LONG sessionID,
		void *buffer);

LONG CSPXListenForConnectedPacket(
		void *eventControlBlock,
		LONG sessionID);

LONG CSPXListenForConnection(
		BYTE retryCount,
		BYTE watchDog,
		LONG *sessionID,
		void *eventControlBlock);

LONG CSPXListenForSequencedPacket(
		void *eventControlBlock);

LONG CSPXSendSequencedPacket(
		LONG sessionID,
		void *eventControlBlock);

LONG CSPXTerminateConnection(
		LONG sessionID,
		void *eventControlBlock);

LONG CIPXOpenSocketRTag( 
		LONG	*	socket, 
		struct ResourceTagStructure * pTag
		);

void CScheduleInterruptTimeCallBack(
		TimeCallBackStructure *TimerNode);

void CCancelInterruptTimeCallBack(
		TimeCallBackStructure *TimerNode);

CPP_END

#endif /* _ECB_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
