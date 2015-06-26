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


#include "clib.h"
#include "debugme.h"
#include "nw3to5.h"
#include "nlmclib.h"

#if defined ( __NW50__ )

    /*
    //  LibC definitions
    */
    extern void *getnlmhandle( void );
    extern void *ImportPublicObject( void *NLMHandle, const char *name );

    /*
    //  These are statically linked against rather than dynamically imported
    */
    extern int      DebugValidatePID( void *pPID );
    extern void     DebugBoobyTrapPID( void *pPID );
    extern void     DebugUnBoobyTrapPID( void *pPID );
    extern char *   DebugGetPIDName( void *pPID );
    extern struct   LoadDefinitionStructure * DebugGetNLMFromPID( void *pPID );
    extern void     DebugSetupPIDForACleanExit( void *pPID );

#elif defined ( __NW40__ )
    #define PCBProcessName ProcessName
    #define T_ThreadControlStruct T_ThreadStruct
    #define T_ThreadGroupControlStruct T_ThreadGroupStruct
    #define T_NLMControlStruct T_NLMStruct
    extern T_ThreadStruct *__GetThreadIDFromPCB( T_PCBStruct * );

    extern unsigned int GetNLMHandle( void );
    extern void *ImportSymbol( unsigned int, char* );

    typedef int (*tDebugValidatePID)( T_PCBStruct *pPID );
    typedef void (*tDebugBoobyTrapPID)( T_PCBStruct *pPID );
    typedef void (*tDebugUnBoobyTrapPID)( T_PCBStruct *pPID );
    typedef char *(*tDebugGetPIDName)( T_PCBStruct *pPID );
    typedef struct LoadDefinitionStructure *(*tDebugGetNLMFromPID)( T_PCBStruct *pPID );
    typedef void (*tDebugSetupPIDForACleanExit)( T_PCBStruct *pPID );

    tDebugValidatePID pDebugValidatePID = NULL;
    tDebugBoobyTrapPID pDebugBoobyTrapPID = NULL;
    tDebugUnBoobyTrapPID pDebugUnBoobyTrapPID = NULL;
    tDebugGetPIDName pDebugGetPIDName = NULL;
    tDebugGetNLMFromPID pDebugGetNLMFromPID = NULL;
    tDebugSetupPIDForACleanExit pDebugSetupPIDForACleanExit = NULL;

    static int SymbolsImported = FALSE;

#elif defined ( __NW30__ )
#else
    #error No Netware Version defined.
#endif

#if defined ( __NW40__ )
int ImportCLIBSymbols( void )
{
    unsigned int handle;
    if( SymbolsImported )
        return TRUE;

    handle = GetNLMHandle();
    pDebugValidatePID = (tDebugValidatePID)ImportSymbol( handle, "DebugValidatePID" );
    pDebugBoobyTrapPID = (tDebugBoobyTrapPID)ImportSymbol( handle, "DebugBoobyTrapPID" );
    pDebugUnBoobyTrapPID = (tDebugUnBoobyTrapPID)ImportSymbol( handle, "DebugUnBoobyTrapPID" );
    pDebugGetPIDName = (tDebugGetPIDName)ImportSymbol( handle, "DebugGetPIDName" );
    pDebugGetNLMFromPID = (tDebugGetNLMFromPID)ImportSymbol( handle, "DebugGetNLMFromPID" );
    pDebugSetupPIDForACleanExit = (tDebugSetupPIDForACleanExit)ImportSymbol( handle, "DebugSetupPIDForACleanExit" );

    SymbolsImported = TRUE;

    return SymbolsImported;
}
#endif

int ValidatePID( T_ProcessID *pPID )
{
/*
//  NW5 and later cannot use PCB ID etc
*/
#if defined ( __NW50__)
    return(DebugValidatePID(pPID));
#else
    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugValidatePID ) {
            return( pDebugValidatePID( pPID ) );
        }
    #endif
    if( pPID == NULL ) return( FALSE );
    #ifdef __NW40__
        return( pPID->UniquePCBID == PCBSignature );
    #else
        return( ((T_ThreadControlStruct*)pPID)->PCBStructID == PCBSignature );
    #endif
#endif
}


#if !defined (  __NW50__ )
static T_ThreadControlStruct *GetTCSFromPID( T_ProcessID *pPID )
{
    if( pPID == NULL ) return( NULL );
    #ifdef __NW40__
        return( __GetThreadIDFromPCB( pPID ) );
    #else
        return( (T_ThreadControlStruct*)pPID );
    #endif
}
#endif

#if !defined (  __NW50__ )
static T_NLMControlStruct *GetNCSFromPID( T_ProcessID *pPID )
{
    T_ThreadGroupControlStruct  *pTGCS;
    T_NLMControlStruct          *pNCS;
    T_ThreadControlStruct       *pTCS;

    if( pPID == NULL ) return( NULL );
    pTCS = GetTCSFromPID( pPID );
    if( pTCS == NULL ) return( NULL );
    pTGCS = pTCS->TCSTGCSp;
    if( pTGCS->TGCSStructID != TGCSSignature ) return( NULL );
    if( pTGCS == NULL ) return( NULL );
    pNCS = pTGCS->TGCSNCSp;
    if( pNCS->NCSStructID != NCSSignature ) return( NULL );
    return( pNCS );
}
#endif

void BoobyTrapPID( T_ProcessID *pPID )
{
#if defined (  __NW50__ )
    DebugBoobyTrapPID( pPID );
#else
    T_ThreadGroupControlStruct  *pTGCS;
    T_ThreadControlStruct       *pTCS;
    T_NLMControlStruct          *pNCS;

    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugBoobyTrapPID ) {
            pDebugBoobyTrapPID( pPID );
            return;
        }
    #endif
    pNCS = GetNCSFromPID( pPID );
    if( pNCS == NULL ) return;
    for( pTGCS = pNCS->NCSTGCSLHead; pTGCS != NULL; pTGCS = pTGCS->TGCSLink ) {
        for( pTCS = pTGCS->TGCSTCSLHead; pTCS != NULL; pTCS = pTCS->TCSLink ) {
            pTCS->TCSSuspendOrStop |= THREAD_DEBUG;
        }
    }
#endif
}

void UnBoobyTrapPID( T_ProcessID *pPID )
{
#if defined ( __NW50__ )
    DebugUnBoobyTrapPID( pPID );
    return;
#else
    T_ThreadGroupControlStruct  *pTGCS;
    T_ThreadControlStruct       *pTCS;
    T_NLMControlStruct          *pNCS;

    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugUnBoobyTrapPID ) {
            pDebugUnBoobyTrapPID( pPID );
            return;
        }
    #endif
    pNCS = GetNCSFromPID( pPID );
    if( pNCS == NULL ) return;
    for( pTGCS = pNCS->NCSTGCSLHead; pTGCS != NULL; pTGCS = pTGCS->TGCSLink ) {
        for( pTCS = pTGCS->TGCSTCSLHead; pTCS != NULL; pTCS = pTCS->TCSLink ) {
            pTCS->TCSSuspendOrStop &= ~THREAD_DEBUG;
        }
    }
#endif
}

char *GetPIDName( T_ProcessID *pPID )
{
#if defined ( __NW50__ )
    return(DebugGetPIDName( pPID ));
#else
    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugGetPIDName ) {
            return( pDebugGetPIDName( pPID ) );
        }
    #endif
    if( pPID == NULL ) return( NULL );
    return( (char *)pPID->PCBProcessName );
#endif
}

struct LoadDefinitionStructure *GetNLMFromPID( T_ProcessID *pPID )
{
#if defined ( __NW50__ )
    return( DebugGetNLMFromPID( pPID ) );
#else
    T_NLMControlStruct          *pNCS;

    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugGetNLMFromPID ) {
            return( pDebugGetNLMFromPID( pPID ) );
        }
    #endif
    pNCS = GetNCSFromPID( pPID );
    if( pNCS == NULL ) return( NULL );
    return( pNCS->NCSNLMHandle );
#endif
}

void SetupPIDForACleanExit( T_ProcessID *pPID )
{
#if defined ( __NW50__ )
    DebugSetupPIDForACleanExit( pPID );
    return;
#else
    T_OpenScreenStruct  *screen;
    T_NLMControlStruct          *pNCS;

    #ifdef __NW40__
        ImportCLIBSymbols();
        if( pDebugSetupPIDForACleanExit ) {
            pDebugSetupPIDForACleanExit( pPID );
            return;
        }
    #endif
    pNCS = GetNCSFromPID( pPID );
    if( pNCS == NULL ) return;
    for( screen = pNCS->NCSOSSLHead; screen; screen = screen->OSSLink ) {
        screen->OSSAttributes |= AUTO_DESTROY_SCREEN;
    }
    pNCS->NCSUnloadFunc = NULL;
    pNCS->NCSExitCount = EXIT_LIMIT + 1;
#endif
}
