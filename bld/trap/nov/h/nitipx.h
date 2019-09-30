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

typedef struct NetworkAddress
{
    BYTE            a[4];                       /* high-low */
} NetworkAddress;

typedef struct NodeAddress
{
    BYTE            a[6];                       /* high-low */
} NodeAddress;

typedef struct IPXAddress
{
    NetworkAddress  network;
    NodeAddress     node;
    WORD            socket;                     /* high-low */
} IPXAddress;

#define _IPXFIELDS \
    WORD            checkSum;                   /* high-low */ \
    WORD            length;                     /* high-low */ \
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

typedef struct SIP {        /* Service Identification Packet */
    _IPXFIELDS;
    WORD            infoType;                   /* high-low */
    WORD            serverType;                 /* high-low */
    char            name[MAX_NAME_LEN+1];
    IPXAddress      address;
    WORD            intermediateNetworks;       /* high-low */
} SIP;

typedef struct ECBFragment {
    void            __far *address;
    WORD            size;                       /* low-high */
} ECBFragment;

typedef struct ECB
{
    void            __far *linkAddress;
    void            (__far *ESRAddress)();
    BYTE            volatile inUseFlag;
    BYTE            completionCode;
    WORD            socketNumber;               /* high-low */
    WORD            SPXConnectionID;
    WORD            IPXWorkspace;
    BYTE            xxx_driverWorkspace[12];    /* N/A */
    NodeAddress     immediateAddress;
    WORD            fragmentCount;              /* low-high */
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

#define IPX_ALL_FUNCS \
    IPX_pick( IPXInitialize ) \
    IPX_pick( IPXSPXDeinit ) \
    IPX_pick( IPXCancelEvent ) \
    IPX_pick( IPXCloseSocket ) \
    IPX_pick( IPXDisconnectFromTarget ) \
    IPX_pick( IPXGetInternetworkAddress ) \
    IPX_pick( IPXGetIntervalMarker ) \
    IPX_pick( IPXGetLocalTarget ) \
    IPX_pick( IPXListenForPacket ) \
    IPX_pick( IPXOpenSocket ) \
    IPX_pick( IPXRelinquishControl ) \
    IPX_pick( IPXScheduleIPXEvent ) \
    IPX_pick( IPXSendPacket ) \
    IPX_pick( SPXAbortConnection ) \
    IPX_pick( SPXEstablishConnection ) \
    IPX_pick( SPXInitialize ) \
    IPX_pick( SPXListenForConnection ) \
    IPX_pick( SPXListenForSequencedPacket ) \
    IPX_pick( SPXSendSequencedPacket ) \
    IPX_pick( SPXTerminateConnection ) \
    IPX_pick( SPXGetConnectionStatus )

#ifdef __WINDOWS__

typedef int  WINAPI NWReadPropertyValue_fnw(unsigned,const char __far *,WORD,const char __far *,BYTE,BYTE __far *,BYTE __far *,BYTE __far *);

typedef void WINAPI IPXSPXDeinit_fnw(DWORD);
typedef int  WINAPI SPXInitialize_fnw(DWORD __far *,WORD,WORD,BYTE __far *,BYTE __far *,WORD __far *,WORD __far *);

typedef int  WINAPI IPXCancelEvent_fnw(DWORD,ECB __far *);
typedef void WINAPI IPXCloseSocket_fnw(DWORD,WORD);
typedef void WINAPI IPXDisconnectFromTarget_fnw(DWORD,BYTE __far *);
typedef void WINAPI IPXGetInternetworkAddress_fnw(DWORD,BYTE __far *);
typedef WORD WINAPI IPXGetIntervalMarker_fnw(DWORD);
typedef int  WINAPI IPXGetLocalTarget_fnw(DWORD,BYTE __far *,BYTE __far *,WORD __far *);
typedef int  WINAPI IPXInitialize_fnw(DWORD __far *,WORD,WORD);
typedef void WINAPI IPXListenForPacket_fnw(DWORD,ECB __far *);
typedef int  WINAPI IPXOpenSocket_fnw(DWORD,WORD __far *,BYTE);
typedef void WINAPI IPXRelinquishControl_fnw(void);
typedef void WINAPI IPXScheduleIPXEvent_fnw(DWORD,WORD,ECB __far *);
typedef void WINAPI IPXSendPacket_fnw(DWORD,ECB __far *);
typedef int  WINAPI SPXEstablishConnection_fnw(DWORD,BYTE,BYTE,WORD __far *,ECB __far *);
typedef int  WINAPI SPXListenForConnection_fnw(DWORD,BYTE,BYTE,ECB __far *);
typedef void WINAPI SPXListenForSequencedPacket_fnw(DWORD,ECB __far *);
typedef void WINAPI SPXSendSequencedPacket_fnw(DWORD,WORD,ECB __far *);
typedef void WINAPI SPXTerminateConnection_fnw(DWORD,WORD,ECB __far *);
typedef void WINAPI SPXAbortConnection_fnw(WORD);
typedef int  WINAPI SPXGetConnectionStatus_fnw(DWORD,WORD,CSB __far *);

#define IPX_pick(f) FN_##f,
enum {
    FN_NWReadPropertyValue,
    IPX_ALL_FUNCS
    IPX_MAX_FUNCS
};
#undef IPX_pick

#define IPX_FUNC(f) ((f##_fnw *)(IPXFuncs[FN_##f]))

#define NWReadPropertyValue( a,b,c,d,e,f,g,h )  IPX_FUNC( NWReadPropertyValue )( a, b, c, d, e, f, g, h )
#define IPXInitialize( a,b,c )                  IPX_FUNC( IPXInitialize )( a, b, c )
#define IPXSPXDeinit()                          IPX_FUNC( IPXSPXDeinit )( IPXTaskID )
#define _IPXCancelEvent(a)                      IPX_FUNC( IPXCancelEvent )( IPXTaskID, a )
#define _IPXCloseSocket(a)                      IPX_FUNC( IPXCloseSocket )( IPXTaskID, a )
#define _IPXDisconnectFromTarget(a)             IPX_FUNC( IPXDisconnectFromTarget )( IPXTaskID, a )
#define _IPXGetInternetworkAddress(a)           IPX_FUNC( IPXGetInternetworkAddress )( IPXTaskID, a )
#define _IPXGetIntervalMarker()                 IPX_FUNC( IPXGetIntervalMarker )( IPXTaskID )
#define _IPXGetLocalTarget(a,b,c)               IPX_FUNC( IPXGetLocalTarget )( IPXTaskID, a, b, c )
#define _IPXListenForPacket(a)                  IPX_FUNC( IPXListenForPacket )( IPXTaskID, a )
#define _IPXOpenSocket(a,b)                     IPX_FUNC( IPXOpenSocket )( IPXTaskID, a, b )
#define _IPXRelinquishControl()                 IPX_FUNC( IPXRelinquishControl )()
#define _IPXScheduleIPXEvent(a,b)               IPX_FUNC( IPXScheduleIPXEvent )( IPXTaskID, a, b )
#define _IPXSendPacket(a)                       IPX_FUNC( IPXSendPacket )( IPXTaskID, a )
#define _SPXInitialize(a,b,c,d,e,f)             IPX_FUNC( SPXInitialize )( &IPXTaskID, a, b, c, d, e, f )
#define _SPXEstablishConnection(a,b,c,d)        IPX_FUNC( SPXEstablishConnection )( IPXTaskID, a, b, c, d )
#define _SPXListenForConnection(a,b,c)          IPX_FUNC( SPXListenForConnection )( IPXTaskID, a, b, c )
#define _SPXListenForSequencedPacket(a)         IPX_FUNC( SPXListenForSequencedPacket )( IPXTaskID, a )
#define _SPXSendSequencedPacket(a,b)            IPX_FUNC( SPXSendSequencedPacket )( IPXTaskID, a, b )
#define _SPXTerminateConnection(a,b)            IPX_FUNC( SPXTerminateConnection )( IPXTaskID, a, b )
#define _SPXAbortConnection(a)                  IPX_FUNC( SPXAbortConnection )( a )
#define _SPXGetConnectionStatus(a,b)            IPX_FUNC( SPXGetConnectionStatus )( IPXTaskID, a, b )

#else

typedef void    IPXSPXDeinit_fn(void);
typedef int     SPXInitialize_fn(BYTE*,BYTE*,WORD*,WORD*);

typedef int     IPXCancelEvent_fn(ECB *);
typedef void    IPXCloseSocket_fn(WORD);
typedef void    IPXDisconnectFromTarget_fn(BYTE *);
typedef void    IPXGetInternetworkAddress_fn(BYTE *);
typedef WORD    IPXGetIntervalMarker_fn(void);
typedef int     IPXGetLocalTarget_fn(BYTE *, BYTE *, WORD *);
typedef int     IPXInitialize_fn(void);
typedef void    IPXListenForPacket_fn(ECB *);
typedef int     IPXOpenSocket_fn(WORD *, BYTE);
typedef void    IPXRelinquishControl_fn(void);
typedef void    IPXScheduleIPXEvent_fn(WORD, ECB *);
typedef void    IPXSendPacket_fn(ECB *);
typedef int     SPXEstablishConnection_fn(BYTE,BYTE,WORD*,ECB*);
typedef int     SPXListenForConnection_fn(BYTE,BYTE,ECB*);
typedef void    SPXListenForSequencedPacket_fn(ECB*);
typedef void    SPXSendSequencedPacket_fn(WORD,ECB*);
typedef void    SPXTerminateConnection_fn(WORD,ECB*);
typedef void    SPXAbortConnection_fn(WORD);
typedef int     SPXGetConnectionStatus_fn(WORD,CSB*);

#define IPX_pick(f) extern f##_fn _##f;
    IPX_ALL_FUNCS
#undef IPX_pick

#define IPXSPXDeinit()
extern SPXInitialize_fn SPXInitialize;
#define _SPXInitialize(a,b,c,d,e,f) SPXInitialize(c,d,e,f)

extern void __far SAPWaitESR( void );
extern void __far SAPBroadESR( void );
extern void __far ServRespESR( void );

#endif

#ifdef __WINDOWS__
    extern HANDLE       Instance;
    #define _ESR( r1 )  (void __far *)MakeProcInstance( (FARPROC)(r1), Instance )
    #define ESRFUNC     __export WINAPI
#else
    #define _ESR( r1 )  &(r1##ESR)
    #define ESRFUNC
#endif

extern void ESRFUNC SAPWait( void );
extern void ESRFUNC SAPBroad( void );
extern void ESRFUNC ServResp( void );
