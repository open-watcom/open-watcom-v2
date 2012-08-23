/* fortify.h - V2.2 - All C & C++ source files to be fortified should #include this file */

/*
 *     This  software  is  not public domain.  All material in
 * this  archive  is (C) Copyright 1995 Simon P.  Bullen.  The
 * software  is  freely distributable, with the condition that
 * no   more   than  a  nominal  fee  is  charged  for  media.
 * Everything  in  this distribution must be kept together, in
 * original, unmodified form.
 *     The software may be modified for your own personal use,
 * but modified files may not be distributed.
 *     The  material  is  provided "as is" without warranty of
 * any  kind.  The author accepts no responsibility for damage
 * caused by this software.
 *     This  software  may not be used in any way by Microsoft
 * Corporation  or  its  subsidiaries, or current employees of
 * Microsoft Corporation or its subsidiaries.
 *     This  software  may  not  be used for the construction,
 * development,  production,  or  testing of weapon systems of
 * any kind.
 *     This  software  may  not  be used for the construction,
 * development,  production,  or  use  of plants/installations
 * which  include  the  processing  of radioactive/fissionable
 * material.
 */

/*  
 *     If  you use this software at all, I'd love to hear from
 * you.   All  questions,  criticisms, suggestions, praise and
 * postcards are most welcome.
 * 
 *            email:    sbullen@cybergraphic.com.au
 * 
 *            snail:    Simon P. Bullen
 *                      PO BOX 12138
 *                      A'Beckett St.
 *                      Melbourne 3000
 *                      Australia
 */

#ifndef __FORTIFY_H__
#define __FORTIFY_H__

#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && (_MSC_VER <= 600)
/*
 * MSC <= 6.0 has a identifier length of 32. Don't bother
 * rewriting to supress warnings.
 */

#else   /* rest of file */

/* the user's options */
#include "ufortify.h"

/* Ensure the configuration parameters have sensible defaults */
#ifndef FORTIFY_STORAGE
    #define FORTIFY_STORAGE
#endif

#ifndef FORTIFY_ALIGNMENT
    #define FORTIFY_ALIGNMENT                    sizeof(double)
#endif

#ifndef FORTIFY_BEFORE_SIZE
    #define FORTIFY_BEFORE_SIZE                  32
#endif
#ifndef FORTIFY_BEFORE_VALUE
    #define FORTIFY_BEFORE_VALUE                 0xA3
#endif

#ifndef FORTIFY_AFTER_SIZE
    #define FORTIFY_AFTER_SIZE                   32
#endif

#ifndef FORTIFY_AFTER_VALUE
    #define FORTIFY_AFTER_VALUE                  0xA5
#endif

#ifndef FORTIFY_FILL_ON_ALLOCATE_VALUE    
    #define FORTIFY_FILL_ON_ALLOCATE_VALUE       0xA7
#endif

#ifndef FORTIFY_FILL_ON_DEALLOCATE_VALUE
    #define FORTIFY_FILL_ON_DEALLOCATE_VALUE     0xA9
#endif

#ifndef FORTIFY_LOCK
    #define FORTIFY_LOCK()   
#endif

#ifndef FORTIFY_UNLOCK
    #define FORTIFY_UNLOCK()  
#endif

#ifndef FORTIFY_CHECKSUM_VALUE
    #define FORTIFY_CHECKSUM_VALUE               0x0AD0
#endif

#ifndef FORTIFY_DELETE_STACK_SIZE
    #define FORTIFY_DELETE_STACK_SIZE    256
#endif

#ifndef FORTIFY_NEW_HANDLER_FUNC
    typedef void (*Fortify_NewHandlerFunc)(void);
    #define FORTIFY_NEW_HANDLER_FUNC Fortify_NewHandlerFunc
#endif

/*
 * Code to detect and configure for various compilers lives here.
 */

#ifdef __GNUG__ 
    /* GCC configuration */
    #define FORTIFY_PROVIDE_ARRAY_NEW
    #define FORTIFY_PROVIDE_ARRAY_DELETE
#endif

#ifdef __HIGHC__
    /* Metaware HighC configuration */
    #define FORTIFY_PROVIDE_ARRAY_NEW
    #define FORTIFY_PROVIDE_ARRAY_DELETE
#endif

#ifdef __BC45__
    /* Borland C++ 4.5 configuration */
    #define FORTIFY_PROVIDE_ARRAY_NEW
    #define FORTIFY_PROVIDE_ARRAY_DELETE
    #define FORTIFY_FAIL_ON_ZERO_MALLOC
#endif

#ifdef __SASC
  /* SAS configuration */
  #define FORTIFY_FAIL_ON_ZERO_MALLOC
#endif

/* Allocators */
#define Fortify_Allocator_malloc            0    /* ANSI C   */
#define Fortify_Allocator_calloc            1    /* ANSI C   */
#define Fortify_Allocator_realloc           2    /* ANSI C   */
#define Fortify_Allocator_strdup            3    /*      C   */
#define Fortify_Allocator_new               4    /* ANSI C++ */
#define Fortify_Allocator_array_new         5    /* Some C++ */

/* Deallocators */
#define Fortify_Deallocator_nobody          0
#define Fortify_Deallocator_free            1    /* ANSI C   */
#define Fortify_Deallocator_realloc         2    /* ANSI C   */
#define Fortify_Deallocator_delete          3    /* ANSI C++ */
#define Fortify_Deallocator_array_delete    4    /* Some C++ */

/* Public Fortify Types */
typedef void (*Fortify_OutputFuncPtr)(const char *);

#ifdef __cplusplus
extern "C" {
#endif

/* Core Fortify Functions */
void *Fortify_Allocate  (size_t size, unsigned char allocator, const char *file, unsigned long line);
void  Fortify_Deallocate(void *uptr,  unsigned char deallocator, const char *file, unsigned long line);
unsigned long Fortify_CheckAllMemory(const char *file, unsigned long line);
unsigned long Fortify_ListAllMemory (const char *file, unsigned long line);
unsigned long Fortify_DumpAllMemory (const char *file, unsigned long line);
int   Fortify_CheckPointer(void *uptr, const char *file, unsigned long line);
void  Fortify_LabelPointer(void *uptr, const char *label, const char *file, unsigned long line);
unsigned char Fortify_EnterScope(const char *file, unsigned long line);
unsigned char Fortify_LeaveScope(const char *file, unsigned long line);
void  Fortify_OutputStatistics(const char *file, unsigned long line);
unsigned long Fortify_GetCurrentAllocation(const char *file, unsigned long line);
void  Fortify_SetAllocationLimit(unsigned long Limit, const char *file, unsigned long line);
int   Fortify_SetFailRate(int Percent);
Fortify_OutputFuncPtr Fortify_SetOutputFunc(Fortify_OutputFuncPtr Output);
void  Fortify_Disable(const char *file, unsigned long line);

/* Fortify versions of the ANSI C memory allocation functions */
void *Fortify_malloc(size_t size, const char *file, unsigned long line);
void *Fortify_realloc(void *ptr, size_t new_size, const char *file, unsigned long line);
void *Fortify_calloc(size_t num, size_t size, const char *file, unsigned long line);
void  Fortify_free(void *uptr, const char *file, unsigned long line);

/* Fortify versions of some non-ANSI C memory allocation functions */
#ifdef FORTIFY_STRDUP
    char *Fortify_strdup(const char *oldStr, const char *file, unsigned long line);
#endif

#ifdef __cplusplus
/* Magic global variable */
extern int gbl_FortifyMagic;
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <new.h>

    /* Fortify versions of new and delete */
    void *operator new(size_t size);
    void *operator new(size_t size, const char *file, unsigned long line);
    void  operator delete(void *pointer);
    void  Fortify_PreDelete(const char *file, unsigned long line);
    void  Fortify_PostDelete();

    /* Some compilers use a different new operator for newing arrays.
     * This includes GNU G++ (2.6.0) and Borland C++ (4.02)
     */
    #ifdef FORTIFY_PROVIDE_ARRAY_NEW
        void *operator new[](size_t size);
        void *operator new[](size_t size, const char *file, unsigned long line);
    #endif

    /* Some compilers provide a different delete operator for deleting arrays.
     * This incldues GNU G++ (2.6.0)
     */
    #ifdef FORTIFY_PROVIDE_ARRAY_DELETE
        void  operator delete[](void *pointer);
    #endif

#endif /* __cplusplus */

#ifndef __FORTIFY_C__ /* Only define the macros if we're NOT in fortify.c */

/* Add file and line information to the fortify calls */
#if defined(USE_FORTIFY) || defined(USE_BSD_FORTIFY)
    /* Core Fortify Functions */
    #define Fortify_CheckAllMemory()       Fortify_CheckAllMemory(__FILE__, __LINE__)
    #define Fortify_ListAllMemory()        Fortify_ListAllMemory (__FILE__, __LINE__)
    #define Fortify_DumpAllMemory()        Fortify_DumpAllMemory (__FILE__, __LINE__)
    #define Fortify_CheckPointer(ptr)      Fortify_CheckPointer(ptr, __FILE__, __LINE__)
    #define Fortify_LabelPointer(ptr,str)  Fortify_LabelPointer(ptr, str, __FILE__, __LINE__)
    #define Fortify_EnterScope()           Fortify_EnterScope(__FILE__, __LINE__)
    #define Fortify_LeaveScope()           Fortify_LeaveScope(__FILE__, __LINE__)
    #define Fortify_OutputStatistics()     Fortify_OutputStatistics(__FILE__, __LINE__)
    #define Fortify_GetCurrentAllocation() Fortify_GetCurrentAllocation(__FILE__, __LINE__)
    #define Fortify_SetAllocationLimit(x)  Fortify_SetAllocationLimit(x, __FILE__, __LINE__)    
    #define Fortify_Disable()              Fortify_Disable(__FILE__, __LINE__)

    /* Fortify versions of the ANSI C memory allocation functions */
    #define malloc(size)                   Fortify_malloc(size, __FILE__, __LINE__)
    #define realloc(ptr,new_size)          Fortify_realloc(ptr, new_size, __FILE__, __LINE__)
    #define calloc(num,size)               Fortify_calloc(num, size, __FILE__, __LINE__)
    #define free(ptr)                      Fortify_free(ptr, __FILE__, __LINE__)

    /* Fortify versions of some non-ANSI C memory allocation functions */
    #ifdef FORTIFY_STRDUP
        #define strdup(ptr)                Fortify_strdup(ptr, __FILE__, __LINE__)
    #endif

    /* Fortify versions of new and delete */
    #ifdef __cplusplus
        #define Fortify_New                new(__FILE__, __LINE__)
        #define Fortify_Delete             for(gbl_FortifyMagic = 1, \
                                               Fortify_PreDelete(__FILE__, __LINE__); \
                                               gbl_FortifyMagic; Fortify_PostDelete()) \
                                                       gbl_FortifyMagic = 0, delete
        #define new                        Fortify_New
        #define delete                     Fortify_Delete
    #endif /* __cplusplus */

#else /* Define the special fortify functions away to nothing */

    #define Fortify_CheckAllMemory()       ((void)0)
    #define Fortify_ListAllMemory()        ((void)0)
    #define Fortify_DumpAllMemory()        ((void)0)
    #define Fortify_CheckPointer(ptr)      1 
    #define Fortify_LabelPointer(ptr,str)  ((void)0)
    #define Fortify_SetOutputFunc(func)    ((void)0)
    #define Fortify_SetMallocFailRate(p)   ((void)0)
    #define Fortify_EnterScope()           ((void)0)
    #define Fortify_LeaveScope()           ((void)0)
    #define Fortify_OutputStatistics()     ((void)0)
    #define Fortify_GetCurrentAllocation() ((void)0)
    #define Fortify_SetAllocationLimit(x)  ((void)0)
    #define Fortify_Disable()              ((void)0)

    #ifdef __cplusplus    
        #define Fortify_New                    new
        #define Fortify_Delete                 delete
    #endif /* __cplusplus */

#endif /* USE_FORTIFY || USE_BSD_FORTIFY */
#endif /* __FORTIFY_C__   */
#endif /* __FORTIFY_H__   */
#endif /* _MSC_VER <= 6.0 */
