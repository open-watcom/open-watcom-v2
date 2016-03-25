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



#ifdef __WINDOWS__
    #include <windows.h>
#else
    #ifndef PROLOG
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    /*#include "prolog.h"*/
    #endif
#endif

#ifdef __WINDOWS__
#define ESRFUNC __export __far __pascal
#else
#define ESRFUNC
#endif

typedef struct NetworkAddress
{
    BYTE        a[4];           /* high-low */
} NetworkAddress;

typedef struct NodeAddress
{
    BYTE        a[6];           /* high-low */
} NodeAddress;

typedef struct IPXAddress
{
        NetworkAddress  network;
        NodeAddress     node;
        WORD            socket; /* high-low */
} IPXAddress;

#define _IPXFIELDS \
        WORD            checkSum;                       /* high-low */ \
        WORD            length;                         /* high-low */ \
        BYTE            transportControl; \
        BYTE            packetType; \
        IPXAddress      destination; \
        IPXAddress      source


typedef struct IPXHeader
{
        _IPXFIELDS;
} IPXHeader;

typedef struct SPXHeader
{
        _IPXFIELDS;
        BYTE            connectControl;
        BYTE            dataStreamType;
        WORD            sourceConnectID;
        WORD            destConnectID;
        WORD            sequenceNumber;
        WORD            acknowledgeNumber;
        WORD            allocationNumber;
} SPXHeader;

#define MAX_NAME_LEN    47

typedef struct SIP {            /* Service Identification Packet */
        _IPXFIELDS;
        WORD                infoType;               /* high-low */
        WORD                serverType;             /* high-low */
        char                name[MAX_NAME_LEN+1];
        IPXAddress          address;
        WORD                intermediateNetworks;   /* high-low */
} SIP;

typedef struct ECBFragment {
        void    __far *address;
        WORD    size;                   /* low-high */
} ECBFragment;


typedef struct ECB
{
        void            __far *linkAddress;
        void            (__far *ESRAddress)();
        BYTE            volatile inUseFlag;
        BYTE            completionCode;
        WORD            socketNumber;           /* high-low */
        WORD            SPXConnectionID;
        WORD            IPXWorkspace;
        BYTE            xxx_driverWorkspace[12];    /* N/A */
        NodeAddress     immediateAddress;
        WORD            fragmentCount;          /* low-high */
        ECBFragment     fragmentDescriptor[2];
} ECB;

typedef struct {
        BYTE            connectionStatus;
        BYTE            connectionFlags;

        WORD            sourceConnectionID;
        WORD            destinationConnectionID;
        WORD            sequenceNumber;
        WORD            acknowledgeNumber;
        WORD            allocationNumber;

        WORD            remoteAcknowledgeNumber;
        WORD            remoteAllocationNumber;
        WORD            connectionSocket;

        NodeAddress     immediateAddress;
        IPXAddress      destination;

        WORD            retransCount;
        WORD            roundtripDelay;
        WORD            retransmittedPacktes;
        WORD            supressedPackets;
} CSB; // connection status block

extern int  _IPXCancelEvent(ECB *);
extern void _IPXCloseSocket(WORD);
extern void _IPXDisconnectFromTarget(BYTE *);
extern void _IPXGetInternetworkAddress(BYTE *);
extern WORD _IPXGetIntervalMarker(void);
extern int  _IPXGetLocalTarget(BYTE *, BYTE *, WORD *);
extern int  _IPXInitialize(void);
extern void _IPXListenForPacket(ECB *);
extern int  _IPXOpenSocket(WORD *, BYTE);
extern void _IPXRelinquishControl(void);
extern void _IPXScheduleIPXEvent(WORD, ECB *);
extern void _IPXSendPacket(ECB *);

extern int _SPXEstablishConnection(BYTE,BYTE,WORD*,ECB*);
extern int _SPXListenForConnection(BYTE,BYTE,ECB*);
extern void _SPXListenForSequencedPacket(ECB*);
extern void _SPXSendSequencedPacket(WORD,ECB*);
extern void _SPXTerminateConnection(WORD,ECB*);
extern void _SPXAbortConnection(WORD);
extern int _SPXGetConnectionStatus(WORD,CSB*);

extern void ESRFUNC SAPWait( void );
extern void ESRFUNC SAPBroad( void );
extern void ESRFUNC ServResp( void );

#ifndef __WINDOWS__
    #define IPXSPXDeinit()
    extern BYTE SPXInitialize(BYTE*,BYTE*,WORD*,WORD*);
    #define _SPXInitialize(a,b,c,d,e,f) \
                    SPXInitialize(c,d,e,f)
    extern void __far SAPWaitESR( void );
    extern void __far SAPBroadESR( void );
    extern void __far ServRespESR( void );
#else

    extern void __far __pascal SAPWaitESR( void );
    extern void __far __pascal SAPBroadESR( void );
    extern void __far __pascal ServRespESR( void );


enum {
    NWReadPropertyValue,
    IPXInitialize,
    IPXSPXDeinit,
    IPXCancelEvent,
    IPXCloseSocket,
    IPXDisconnectFromTarget,
    IPXGetInternetworkAddress,
    IPXGetIntervalMarker,
    IPXGetLocalTarget,
    IPXListenForPacket,
    IPXOpenSocket,
    IPXRelinquishControl,
    IPXScheduleIPXEvent,
    IPXSendPacket,
    SPXAbortConnection,
    SPXEstablishConnection,
    SPXInitialize,
    SPXListenForConnection,
    SPXListenForSequencedPacket,
    SPXSendSequencedPacket,
    SPXTerminateConnection,
    IPX_MAX_FUNCS
};

#define NWReadPropertyValue( a,b,c,d,e,f,g,h ) \
    IPXFuncs[ NWReadPropertyValue ]( (unsigned)(a), (char*)(b), (WORD)(c), (char*)(d), \
                                     (BYTE)(e), (BYTE*)(f), (BYTE*)(g), (BYTE*)(h) )
#define IPXInitialize( a,b,c ) \
    IPXFuncs[ IPXInitialize ]( a, (WORD)(b), (WORD)(c) )
#define IPXSPXDeinit() \
    IPXFuncs[ IPXSPXDeinit ]( (DWORD)IPXTaskID )
#define _IPXCancelEvent(a) \
    IPXFuncs[ IPXCancelEvent ]( (DWORD)IPXTaskID, (ECB*)(a) )
#define _IPXCloseSocket(a) \
    IPXFuncs[ IPXCloseSocket ]( (DWORD)IPXTaskID, (WORD)(a) )
#define _IPXDisconnectFromTarget(a) \
    IPXFuncs[ IPXDisconnectFromTarget ]( (DWORD)IPXTaskID, (BYTE*)(a) )
#define _IPXGetInternetworkAddress(a) \
    IPXFuncs[ IPXGetInternetworkAddress ]( (DWORD)IPXTaskID, (BYTE*)(a) )
#define _IPXGetIntervalMarker() \
    IPXFuncs[ IPXGetIntervalMarker ]( (DWORD)IPXTaskID )
#define _IPXGetLocalTarget(a,b,c) \
    IPXFuncs[ IPXGetLocalTarget ]( (DWORD)IPXTaskID, (BYTE*)(a), (BYTE*)(b), (WORD*)(c) )
#define _IPXListenForPacket(a) \
    IPXFuncs[ IPXListenForPacket ]( (DWORD)IPXTaskID, (ECB*)(a) )
#define _IPXOpenSocket(a,b) \
    IPXFuncs[ IPXOpenSocket ]( (DWORD)IPXTaskID, (WORD*)(a), (BYTE)(b) )
#define _IPXRelinquishControl() \
    IPXFuncs[ IPXRelinquishControl ]()
#define _IPXScheduleIPXEvent(a,b) \
    IPXFuncs[ IPXScheduleIPXEvent ]( (DWORD)IPXTaskID, (WORD)(a), (ECB*)(b) )
#define _IPXSendPacket(a) \
    IPXFuncs[ IPXSendPacket ]( (DWORD)IPXTaskID, (ECB*)(a) )
#define _SPXInitialize(a,b,c,d,e,f) \
    IPXFuncs[ SPXInitialize ]( (DWORD*)&IPXTaskID, (WORD)(a), (WORD)(b), (BYTE*)(c), (BYTE*)(d), (WORD*)(e), (WORD*)(f) )
#define _SPXEstablishConnection(a,b,c,d) \
    IPXFuncs[ SPXEstablishConnection ]( (DWORD)IPXTaskID, (BYTE)(a), (BYTE)(b), (WORD*)(c), (ECB*)(d) )
#define _SPXListenForConnection(a,b,c) \
    IPXFuncs[ SPXListenForConnection ]( (DWORD)IPXTaskID, (BYTE)(a), (BYTE)(b), (ECB*)(c) )
#define _SPXListenForSequencedPacket(a) \
    IPXFuncs[ SPXListenForSequencedPacket ]( (DWORD)IPXTaskID, (ECB*)(a) )
#define _SPXSendSequencedPacket(a,b) \
    IPXFuncs[ SPXSendSequencedPacket ]( (DWORD)IPXTaskID, (WORD)(a), (ECB*)(b) )
#define _SPXTerminateConnection(a,b) \
    IPXFuncs[ SPXTerminateConnection ]( (DWORD)IPXTaskID, (WORD)(a), (ECB*)(b) )
#define _SPXAbortConnection(a) \
    IPXFuncs[ SPXAbortConnection ]( (WORD)(a) )
#endif

#ifdef __WINDOWS__
    extern HANDLE Instance;
    #define _ESR( r1 )      (void __far *)MakeProcInstance( (FARPROC)(r1), Instance )
#else
    #define _ESR( r1 )      &(r1##ESR)
#endif
