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
* Description:  NetWare LibC common definitions
*
****************************************************************************/


#ifndef _NW_LIBC_MTHREAD_H
#define _NW_LIBC_MTHREAD_H

typedef int             NXThreadId_t;
typedef int             NXKey_t;
typedef struct NXSema_t NXSema_t;
typedef void *          NXContext_t;

unsigned long __GetSystemWideUniqueTID(
    void
    );

extern NXKey_t                __NXSlotID;
#define GetCurrentThreadId()  __GetSystemWideUniqueTID()
#define TID                   unsigned long 
extern void                   __LibCThreadFini(void);

/* Key-value pairs (per-context data)... */
extern int NXKeyCreate(
    void        (*destructor)(void *), 
    void *      value, 
    NXKey_t *   key
    );

extern int NXKeyDelete(
    NXKey_t     key
    );

extern int NXKeyGetValue(
    NXKey_t     key, 
    void **     value
    );

extern int NXKeySetValue(
    NXKey_t     key, 
    void *      value
    );

extern NXThreadId_t     NXThreadGetId(
    void
    );

extern void             NXThreadYield(
    void
    );

extern NXSema_t *       NXSemaAlloc(
                            unsigned int    count, 
                            void *          arg
                            );
extern void             NXSemaFree(
                            NXSema_t *      sema
                            );
extern void             NXSemaPost(
                            NXSema_t *      sema
                            );
extern void             NXSemaWait(
                            NXSema_t *      sema
                            );

extern NXContext_t      NXContextAlloc(
                            void            (*start_routine)(void *arg), 
                            void *          arg,
                            int             priority, 
                            size_t          stackSize, 
                            unsigned long   flags, 

                            int *           error);
int                     NXThreadCreate(
                            NXContext_t     ctx, 
                            unsigned long   flags,
                            NXThreadId_t *  idp
                            );

void                    NXThreadExit(
                            void *          status
                            );

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
#define NX_INVALID_THREAD_ID  ((NXThreadId_t) 0xFFFFFFFF)

#define __SYS_ALLOCD_STACK    0

#define GetLastError()      (*___errno())
void    SetLastError(int error);

#ifndef BOOL
# define _BOOL
#define BOOL unsigned int
#endif

#ifndef DWORD
# define _DWORD
#define DWORD	unsigned int
#endif

#ifndef FALSE
# define FALSE (0)
#endif

#ifndef TRUE
# define TRUE (1)
#endif

extern unsigned long __GetSystemWideUniqueTID(void);


#endif  /* _NW_LIBC_MTHREAD_H */
