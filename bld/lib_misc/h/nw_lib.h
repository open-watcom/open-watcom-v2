/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NetWare CRTL common definitions
*
****************************************************************************/


#ifndef _NW_LIB_H_INCLUDED
#define _NW_LIB_H_INCLUDED

#if defined( _NETWARE_CLIB )

/*****************************************************************************
 * NetWare Clib declarations
 *****************************************************************************/

// Thread related functions
extern void     ExitThread( int , int );
extern int      BeginThread( void (*)( void * ), void *, unsigned, void * );
extern void     ThreadSwitch( void );

// Semaphore related functions
extern long     OpenLocalSemaphore( long );
extern int      CloseLocalSemaphore( long );
extern long     ExamineLocalSemaphore( long );
extern int      SignalLocalSemaphore( long );
extern int      WaitOnLocalSemaphore( long );

#elif defined( _NETWARE_LIBC )

/*****************************************************************************
 * NetWare LibC declarations
 *****************************************************************************/

#ifndef BOOL
# define _BOOL
#define BOOL    unsigned int
#endif

#ifndef DWORD
# define _DWORD
#define DWORD   unsigned int
#endif

#ifndef FALSE
# define FALSE  (0)
#endif

#ifndef TRUE
# define TRUE   (1)
#endif

/*
//  NKS data / functionality
//  from $(LibC)\include\nks\synch.h
*/
/* values for thread priority... */
#define NX_PRIO_HIGH          10
#define NX_PRIO_MED           5
#define NX_PRIO_LOW           1
#define NX_PRIO_DEFAULT       NX_PRIO_MED

/* values for thread context flags... */
#define NX_CTX_NORMAL         0x00000000  /* normal thread                  */
#define NX_CTX_WORK           0x00000001  /* context for work-to-do thread  */

/* values for thread flags... */
#define NX_THR_JOINABLE       0x00000000  /* default thread type            */
#define NX_THR_DETACHED       0x00000010  /* thread cannot be joined        */
#define NX_THR_SUSPENDED      0x00000020  /* thread starts out suspended    */
#define NX_THR_DAEMON         0x00000040  /* detached and non-persistent    */
#define NX_THR_BIND_CONTEXT   0x00000080  /* context disappears with thread */
#define NX_THR_MASK           0x000000F0

/* value returned that specifies that the thread is unbound */
#define NX_THR_UNBOUND        0xFFFFFFFF

/* return from NXThreadGetId indicating no-context or other error */
#define NX_INVALID_THREAD_ID  ((NXThreadId_t)0xFFFFFFFF)

#define __SYS_ALLOCD_STACK    0

typedef int             NXKey_t;
typedef void            *NXContext_t;
typedef int             NXThreadId_t;
typedef struct NXSema_t NXSema_t;

/* Key-value pairs (per-context data)... */
extern int              NXKeyCreate( void (*destructor)(void *), void *value, NXKey_t *key );
extern int              NXKeyDelete( NXKey_t key );
extern int              NXKeyGetValue( NXKey_t key, void **value );
extern int              NXKeySetValue( NXKey_t key, void *value );

extern NXContext_t      NXContextAlloc( void (*start_routine)(void *arg), void *arg, int priority, unsigned stackSize, unsigned long flags, int *error );
extern int              NXThreadCreate( NXContext_t ctx, unsigned long flags, NXThreadId_t *idp );
extern NXThreadId_t     NXThreadGetId( void );
extern void             NXThreadYield( void );
extern void             NXThreadExit( void *status );

extern NXSema_t         *NXSemaAlloc( unsigned int count, void *arg );
extern void             NXSemaFree( NXSema_t *sema );
extern void             NXSemaPost( NXSema_t *sema );
extern void             NXSemaWait( NXSema_t *sema );

extern int              *___errno( void );
#define GetLastError()  (*___errno())
extern void             SetLastError( int error );

/*
//  CurrentProcess() is a THREADS.NLM (CLIB) export though it returns the
//  underlying NetWare TCO pointer. As unique as I can get on NetWare currently
*/
extern unsigned long    CurrentProcess( void );

#endif

/*****************************************************************************
 * NetWare common definitions (Clib and LibC)
 *****************************************************************************/

extern int              sys_nerr;
extern char             *sys_errlist[];

extern void             *GetThreadID( void );
extern void             GetFileServerDateAndTime( void * );
extern char             *ConvertNameToFullPath( const char *, char * );

/*
 * NW386 Server exported functions
 */

#define AllocSignature  0x54524C41
extern void             *Alloc( long __numberOfBytes, long __resourceTag );
extern void             Free( void *__address );
extern long             AllocateResourceTag( void *__NLMHandle, char *__descriptionString, long __resourceType );
extern long             SizeOfAllocBlock( void * );


#endif  /* _NW_LIB_H_INCLUDED */
