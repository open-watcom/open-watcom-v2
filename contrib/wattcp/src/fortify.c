/* fortify.cxx - A fortified memory allocation shell - V2.2 */

/*
 * This software is not public domain. All material in
 * this archive is (C) Copyright 1995 Simon P. Bullen. The
 * software is freely distributable, with the condition that
 * no more than a nominal fee is charged for media.
 * Everything in this distribution must be kept together, in
 * original, unmodified form.
 *
 * The software may be modified for your own personal use,
 * but modified files may not be distributed.
 *
 * The material is provided "as is" without warranty of
 * any kind. The author accepts no responsibility for damage
 * caused by this software.
 *
 * This software may not be used in any way by Microsoft
 * Corporation or its subsidiaries, or current employees of
 * Microsoft Corporation or its subsidiaries.
 *
 * This software may not be used for the construction,
 * development, production, or testing of weapon systems of
 * any kind.
 *
 * This software may not be used for the construction,
 * development, production, or use of plants/installations
 * which include the processing of radioactive/fissionable
 * material.
 */

/*
 * If you use this software at all, I'd love to hear from
 * you.  All questions, criticisms, suggestions, praise and
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

/* Prototypes and such */
#define __FORTIFY_C__

#include "wattcp.h"
#include "misc.h"

#if (DOSX)  /* Using inchksum_fast() function */
  #undef  FORTIFY_CHECKSUM_VALUE
  #define FORTIFY_CHECKSUM_VALUE 0xFFFF
#endif

#include "fortify.h"
#include "chksum.h"

#if defined(USE_FORTIFY) || defined(USE_BSD_FORTIFY)


#define STATIC

/*
 * struct Header - this structure is used 
 * internally by Fortify to manage it's 
 * own private lists of memory.
 */
struct Header {
       WORD           Checksum;     /* For the integrity of our goodies  */
       const char    *File;         /* The sourcefile of the allocator   */
       DWORD          Line;         /* The sourceline of the allocator   */
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
       const char    *FreedFile;    /* The sourcefile of the deallocator */
       DWORD          FreedLine;    /* The sourceline of the deallocator */
       BYTE           Deallocator;  /* The deallocator used              */
#endif
       size_t         Size;         /* The size of the malloc'd block    */
       struct Header *Prev;         /* Previous link                     */
       struct Header *Next;         /* Next link                         */
       char          *Label;        /* User's Label (may be null)        */
       BYTE           Scope;        /* Scope level of the caller         */
       BYTE           Allocator;    /* malloc/realloc/new/etc            */
     };

/*
 * Round x up to the nearest multiple of n.
 */
#define ROUND_UP(x,n)        ((((x) + (n)-1)/(n))*(n))

#define FORTIFY_HEADER_SIZE  ROUND_UP(sizeof(struct Header), sizeof(WORD))

/*
 * FORTIFY_ALIGNED_BEFORE_SIZE is FORTIFY_BEFORE_SIZE rounded up to the
 * next multiple of FORTIFY_ALIGNMENT. This is so that we can guarantee
 * the alignment of user memory for such systems where this is important
 * (eg storing doubles on a SPARC)
 */
#define FORTIFY_ALIGNED_BEFORE_SIZE ( \
        ROUND_UP (FORTIFY_HEADER_SIZE+FORTIFY_BEFORE_SIZE, FORTIFY_ALIGNMENT) \
        - FORTIFY_HEADER_SIZE)

/*
 * FORTIFY_OVERHEAD is the total overhead added by Fortify to each
 * memory block.
 */
#define FORTIFY_OVERHEAD (FORTIFY_HEADER_SIZE         + \
                          FORTIFY_ALIGNED_BEFORE_SIZE + \
                          FORTIFY_AFTER_SIZE)


/*
 * Static Function Prototypes
 */
STATIC int  st_CheckBlock         (struct Header *h, const char *file, DWORD line);
STATIC int  st_CheckFortification (BYTE *ptr, BYTE value, size_t size);
STATIC void st_SetFortification   (BYTE *ptr, BYTE value, size_t size);
STATIC void st_OutputFortification(BYTE *ptr, BYTE value, size_t size);
STATIC void st_HexDump            (BYTE *ptr, size_t offset, size_t size, int title);
STATIC int  st_IsHeaderValid      (struct Header *h);
STATIC void st_MakeHeaderValid    (struct Header *h);
STATIC WORD st_ChecksumHeader     (struct Header *h);
STATIC int  st_IsOnAllocatedList  (struct Header *h);
STATIC void st_OutputHeader       (struct Header *h);
STATIC void st_OutputMemory       (struct Header *h);
STATIC void st_OutputLastVerifiedPoint(void);
STATIC void st_DefaultOutput      (const char *String);
STATIC const char *st_MemoryBlockString(struct Header *h);
STATIC void st_OutputDeleteTrace (void);

#if defined (FORTIFY_TRACK_DEALLOCATED_MEMORY)
#if defined (FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY) && \
    defined (FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY)
    STATIC const char *st_DeallocatedMemoryBlockString(struct Header *h);
#endif
    STATIC int st_IsOnDeallocatedList(struct Header *h);
    STATIC int st_PurgeDeallocatedBlocks(DWORD Bytes, const char *file, DWORD line);
    STATIC int st_PurgeDeallocatedScope(BYTE Scope, const char *file, DWORD line);
    STATIC int st_CheckDeallocatedBlock(struct Header *h, const char *file, DWORD line);
    STATIC void st_FreeDeallocatedBlock(struct Header *h, const char *file, DWORD line);
#endif


/*
 * Static variables
 */
STATIC char                  st_Buffer[4096]     = { 0 }; /* don't use BSS */
STATIC struct Header        *st_AllocatedHead    = 0;
STATIC int                   st_AllocateFailRate = 0;
STATIC Fortify_OutputFuncPtr st_Output           = st_DefaultOutput;
STATIC const char           *st_LastVerifiedFile = "unknown";
STATIC DWORD                 st_LastVerifiedLine = 0;
STATIC BYTE                  st_Scope            = 0;
STATIC BYTE                  st_Disabled         = 0;

#if defined(__HIGHC__) && 0 // !! not yet
STATIC char st_LockDataStart = 0;
#endif

#ifdef __cplusplus
  int    gbl_FortifyMagic = 0;
  STATIC const char *st_DeleteFile[FORTIFY_DELETE_STACK_SIZE];
  STATIC DWORD       st_DeleteLine[FORTIFY_DELETE_STACK_SIZE];
  STATIC DWORD       st_DeleteStackTop;
#endif

/* statistics */
STATIC DWORD  st_MaxBlocks        = 0;
STATIC DWORD  st_MaxAllocation    = 0;
STATIC DWORD  st_CurBlocks        = 0;
STATIC DWORD  st_CurAllocation    = 0;
STATIC DWORD  st_Allocations      = 0;
STATIC DWORD  st_Frees            = 0;
STATIC DWORD  st_TotalAllocation  = 0;
STATIC DWORD  st_AllocationLimit  = 0xffffffff;

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
  STATIC struct Header *st_DeallocatedHead  = 0;
  STATIC struct Header *st_DeallocatedTail  = 0;
  STATIC DWORD          st_TotalDeallocated = 0;
#endif

 
/* allocators */
STATIC const char *st_AllocatorName[] = {
                  "malloc()",
                  "calloc()",
                  "realloc()",
                  "strdup()",
                  "new",
                  "new[]"
                };

/* deallocators */
STATIC const char *st_DeallocatorName[] = {
                  "nobody",
                  "free()",
                  "realloc()",
                  "delete",
                  "delete[]"
                };

STATIC const BYTE st_ValidDeallocator[] = {
    (1 << Fortify_Deallocator_free) | (1 << Fortify_Deallocator_realloc),
    (1 << Fortify_Deallocator_free) | (1 << Fortify_Deallocator_realloc),
    (1 << Fortify_Deallocator_free) | (1 << Fortify_Deallocator_realloc),
    (1 << Fortify_Deallocator_free) | (1 << Fortify_Deallocator_realloc),
#if defined(FORTIFY_PROVIDE_ARRAY_NEW) && defined(FORTIFY_PROVIDE_ARRAY_DELETE)
    (1 << Fortify_Deallocator_delete),
    (1 << Fortify_Deallocator_array_delete)
#else
    (1 << Fortify_Deallocator_delete) | (1 << Fortify_Deallocator_array_delete),
    (1 << Fortify_Deallocator_delete) | (1 << Fortify_Deallocator_array_delete)
#endif
};

STATIC char st_LockDataFill[4096] = { 0 };
STATIC char st_LockDataEnd        = 0;


#if defined(__HIGHC__) && 0 // !! not yet

#include <pharlap.h>
#include <hw386.h>

/*
 *  Change page-attributes for local data.
 *  Set to present or non-present pages.
 */
STATIC Fortify_LockLocalData (int lock)
{
  UINT  lockSize  = (ULONG)&st_LockDataEnd - (ULONG)&st_LockDataStart;
  ULONG num_pages = (lockSize + 4095) / 4096;
  ULONG page      = ((ULONG)&st_LockDataStart + 4095) / 4096;

  for ( ; page < page + num_pages; page++)
  {
    ULONG pte, ptInfo;
    if (_dx_rd_ptinfl (page << 12, &pte, &ptInfo))
       break;
    if (lock)
         pte &= ~PE_PRESENT;
    else pte |=  PE_PRESENT;
    if (_dx_wr_ptinfl (page << 12, pte, ptInfo))
       break;
  }
}
#endif


/*
 * Fortify_Allocate() - allocate a block of fortified memory
 */
void *FORTIFY_STORAGE
Fortify_Allocate (size_t size, BYTE allocator, const char *file, DWORD line)
{
  struct Header *h;
  int    another_try;
  BYTE  *ptr = NULL;

  /*
   * If Fortify has been disabled, then it's easy
   */
  if (st_Disabled)
  {
#ifdef FORTIFY_FAIL_ON_ZERO_MALLOC
    if (size == 0 &&
        (allocator == Fortify_Allocator_new ||
         allocator == Fortify_Allocator_array_new))
    {
      /*
       * A new of zero bytes must succeed, but a malloc of
       * zero bytes probably won't
       */
      return malloc(1);
    }
#endif

    return malloc(size);
  }

#ifdef FORTIFY_CHECK_ALL_MEMORY_ON_ALLOCATE
  Fortify_CheckAllMemory (file, line);
#endif  

  if (st_AllocateFailRate > 0)
  {
    if (rand() % 100 < st_AllocateFailRate)
    {
#ifdef FORTIFY_WARN_ON_FALSE_FAIL
      sprintf (st_Buffer,
               "\nFortify: A \"%s\" of %lu bytes \"false failed\" at %s.%lu\n",
               st_AllocatorName[allocator], (DWORD)size, file, line);
      st_Output (st_Buffer);
#endif
      return (0);
    }
  }

  /* Check to see if this allocation will
   * push us over the artificial limit
   */
  if (st_CurAllocation + size > st_AllocationLimit)
  {
#ifdef FORTIFY_WARN_ON_FALSE_FAIL
    sprintf (st_Buffer,
             "\nFortify: A \"%s\" of %lu bytes \"false failed\" at %s.%lu\n",
             st_AllocatorName[allocator], (DWORD)size, file, line);
    st_Output (st_Buffer);
#endif
    return (0);
  }

#ifdef FORTIFY_WARN_ON_ZERO_MALLOC
  if (size == 0 && (allocator == Fortify_Allocator_malloc ||
                    allocator == Fortify_Allocator_calloc ||
                    allocator == Fortify_Allocator_realloc))
  {
    sprintf(st_Buffer,
            "\nFortify: A \"%s\" of 0 bytes attempted at %s.%lu\n",
            st_AllocatorName[allocator], file, line);
    st_Output(st_Buffer);
  }
#endif /* FORTIFY_WARN_ON_ZERO_MALLOC */

#ifdef FORTIFY_FAIL_ON_ZERO_MALLOC
  if (size == 0 && (allocator == Fortify_Allocator_malloc ||
                    allocator == Fortify_Allocator_calloc ||
                    allocator == Fortify_Allocator_realloc))
  {
#ifdef FORTIFY_WARN_ON_ALLOCATE_FAIL
    sprintf(st_Buffer, "\nFortify: A \"%s\" of %lu bytes failed at %s.%lu\n",
            st_AllocatorName[allocator], (DWORD)size, file, line);
    st_Output(st_Buffer);
#endif /* FORTIFY_WARN_ON_ALLOCATE_FAIL */
    return (0);
  }
#endif /* FORTIFY_FAIL_ON_ZERO_MALLOC */    

#ifdef FORTIFY_WARN_ON_SIZE_T_OVERFLOW
    /*
     * Ensure the size of the memory block
     * plus the overhead isn't bigger than
     * size_t (that'd be a drag)
     */
    {
        size_t private_size = FORTIFY_HEADER_SIZE
                            + FORTIFY_ALIGNED_BEFORE_SIZE + size + FORTIFY_AFTER_SIZE;

        if (private_size < size)
        {
            sprintf(st_Buffer, 
                    "\nFortify: A \"%s\" of %lu bytes has overflowed size_t at %s.%lu\n",
                    st_AllocatorName[allocator], (DWORD)size, file, line);
            st_Output(st_Buffer);
            return (0);
        }
#if (DOSX) && defined(__WATCOMC__) && 0
        if (private_size >= 65536UL)
        {
            sprintf(st_Buffer, 
                    "\nFortify: A \"%s\" of %lu bytes has exceeded 64kB limit by %d bytes, at %s.%lu\n",
                    st_AllocatorName[allocator], (DWORD)size,
                    private_size - size, file, line);
            st_Output(st_Buffer);
            return (0);
        }
#endif
    }
#endif                              

    another_try = 1;
    do
    {
        /*
         * malloc the memory, including the space 
         * for the header and fortification buffers
         */  
        ptr = (BYTE *)malloc ( FORTIFY_HEADER_SIZE
                                   + FORTIFY_ALIGNED_BEFORE_SIZE
                                   + size
                                   + FORTIFY_AFTER_SIZE);

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
        /*
         * If we're tracking deallocated memory, then
         * we can free some of it, rather than let
         * this malloc fail
         */
        if (!ptr)
           another_try = st_PurgeDeallocatedBlocks(size, file, line);

#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

    }
    while (!ptr && another_try);

    if (!ptr)
    {
#ifdef FORTIFY_WARN_ON_ALLOCATE_FAIL
        sprintf(st_Buffer, "\nFortify: A \"%s\" of %lu bytes failed at %s.%lu\n",
                st_AllocatorName[allocator], (DWORD)size, file, line);
        st_Output(st_Buffer);
#endif
        return (0);
    }

    /*
     * Begin Critical Region
     */
    FORTIFY_LOCK();


    /* 
     * Make the head's prev pointer point to us
     * ('cos we're about to become the head)
     */
    if (st_AllocatedHead)
    {
        st_CheckBlock(st_AllocatedHead, file, line);
        /* what should we do if this fails? (apart from panic) */

        st_AllocatedHead->Prev = (struct Header *)ptr;
        st_MakeHeaderValid(st_AllocatedHead);  
    }

    /*
     * Initialize and validate the header
     */
    h = (struct Header *)ptr;
    h->Size      = size;
    h->File      = file;
    h->Line      = line;  
    h->Next      = st_AllocatedHead;
    h->Prev      = 0;
    h->Scope     = st_Scope;
    h->Allocator = allocator;
    h->Label     = 0;
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    h->FreedFile = 0;
    h->FreedLine = 0;
    h->Deallocator = Fortify_Deallocator_nobody;
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */
    st_MakeHeaderValid(h);
    st_AllocatedHead = h;
  
    /*
     * Initialize the fortifications
     */    
    st_SetFortification(ptr + FORTIFY_HEADER_SIZE,
                     FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE);
    st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + size,
                     FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE);

#ifdef FORTIFY_FILL_ON_ALLOCATE
    /*
     * Fill the actual user memory
     */
    st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                        FORTIFY_FILL_ON_ALLOCATE_VALUE, size);
#endif

    /*
     * End Critical Region
     */
    FORTIFY_UNLOCK();


    /*
     * update the statistics
     */
    st_TotalAllocation += size;
    st_Allocations++;
    st_CurBlocks++;
    st_CurAllocation += size;
    if (st_CurBlocks > st_MaxBlocks)
        st_MaxBlocks = st_CurBlocks;
    if (st_CurAllocation > st_MaxAllocation)
        st_MaxAllocation = st_CurAllocation;

    /*
     * We return the address of the user's memory, not the start of the block,
     * which points to our magic cookies
     */
    return (ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE);
}



/*
 * Fortify_Deallocate() - Free a block of memory allocated with Fortify_Allocate()
 */
void FORTIFY_STORAGE
Fortify_Deallocate(void *uptr, BYTE deallocator, const char *file, DWORD line)
{
    BYTE *ptr = (BYTE *)uptr
                        - FORTIFY_HEADER_SIZE
                        - FORTIFY_ALIGNED_BEFORE_SIZE;
    struct Header *h   = (struct Header *)ptr;

#ifdef FORTIFY_CHECK_ALL_MEMORY_ON_DEALLOCATE
    Fortify_CheckAllMemory(file, line);
#endif

    /*
     * If Fortify has been disabled, then it's easy
     * (well, almost)
     */
    if (st_Disabled)
    {
        /* there is a possibility that this memory
         * block was allocated when Fortify was
         * enabled, so we must check the Allocated
         * list before we free it.
         */
        if (!st_IsOnAllocatedList(h))
        {
            free(uptr);    
            return;
        }    
        else
        {
            /* the block was allocated by Fortify, so we 
             * gotta free it differently.
             */
            /*
             * Begin critical region
             */
            FORTIFY_LOCK();
        
            /*
             * Remove the block from the list
             */
            if (h->Prev)
                h->Prev->Next = h->Next;
            else
                st_AllocatedHead = h->Next;
        
            if (h->Next)
                h->Next->Prev = h->Prev;
        
            /*
             * End Critical Region
             */
            FORTIFY_UNLOCK();

            /*
             * actually free the memory
             */
            free(ptr);
            return;
        }
    }


#ifdef FORTIFY_PARANOID_DEALLOCATE
    if (!st_IsOnAllocatedList(h))
    {
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
        if (st_IsOnDeallocatedList(h))
        {
            sprintf(st_Buffer, "\nFortify: \"%s\" twice of %s detected at %s.%lu\n",
                                st_DeallocatorName[deallocator],
                                st_MemoryBlockString(h), file, line);
            st_Output(st_Buffer);

            sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                                st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
            st_Output(st_Buffer);
            st_OutputDeleteTrace();
            return;
        }
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

        sprintf (st_Buffer, "\nFortify: Possible \"%s\" twice of (%08lX) was detected at %s.%lu\n",
                 st_DeallocatorName[deallocator], (DWORD)uptr, file, line);

        st_Output(st_Buffer);
        st_OutputDeleteTrace();
        return;
    }
#endif /* FORTIFY_PARANOID_DELETE */

    /*
     * Make sure the block is okay before we free it.
     * If it's not okay, don't free it - it might not
     * be a real memory block. Or worse still, someone
     * might still be writing to it
     */
    if (!st_CheckBlock(h, file, line))
    {
        st_OutputDeleteTrace();
        return;
    }

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    /*
     * Make sure the block hasn't been freed already
     * (we can get to here if FORTIFY_PARANOID_DELETE
     * is off, but FORTIFY_TRACK_DEALLOCATED_MEMORY
     * is on).
     */
    if (h->Deallocator != Fortify_Deallocator_nobody)
    {
        sprintf(st_Buffer, "\nFortify: \"%s\" twice of %s detected at %s.%lu\n",
                              st_DeallocatorName[deallocator],
                            st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);

        sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                            st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
        st_Output(st_Buffer);
        st_OutputDeleteTrace();
        return;
    }
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

    /*
     * Make sure the block is being freed with a valid
     * deallocator. If not, complain. (but free it anyway)
     */
    if ((st_ValidDeallocator[h->Allocator] & (1 << deallocator)) == 0)
    {
        sprintf(st_Buffer, "\nFortify: Incorrect deallocator \"%s\" detected at %s.%lu\n",
                              st_DeallocatorName[deallocator], file, line);
        st_Output(st_Buffer);
        sprintf(st_Buffer,   "         %s was allocated with \"%s\"\n",
                              st_MemoryBlockString(h), st_AllocatorName[h->Allocator]);
        st_Output(st_Buffer);
        st_OutputDeleteTrace();
    }

    /*
     * Begin critical region
     */
    FORTIFY_LOCK();

    /*
     * Remove the block from the list
     */
    if (h->Prev)
    {
        if (!st_CheckBlock(h->Prev, file, line))
        {
            FORTIFY_UNLOCK();
            st_OutputDeleteTrace();
            return;
        }

        h->Prev->Next = h->Next;
        st_MakeHeaderValid(h->Prev);
    }
    else
        st_AllocatedHead = h->Next;

    if (h->Next)
    {
        if (!st_CheckBlock(h->Next, file, line))
        {
            FORTIFY_UNLOCK();
            st_OutputDeleteTrace();
            return;
        }

        h->Next->Prev = h->Prev;
        st_MakeHeaderValid(h->Next);
    }

    /*
     * End Critical Region
     */
    FORTIFY_UNLOCK();

    /*
     * update the statistics
     */
    st_Frees++;
    st_CurBlocks--;
    st_CurAllocation -= h->Size;


#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    if (st_Scope > 0)
    {
        /*
         * Don't _actually_ free the memory block, just yet.
         * Place it onto the deallocated list, instead, so
         * we can check later to see if it's been written to.
         */
    #ifdef FORTIFY_FILL_ON_DEALLOCATE
        /*
         * Nuke out all user memory that is about to be freed
         */
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                                    FORTIFY_FILL_ON_DEALLOCATE_VALUE,
                                  h->Size);
    #endif /* FORTIFY_FILL_ON_DEALLOCATE */

        /*
         * Begin critical region
         */
        FORTIFY_LOCK();

        /*
         * Place the block on the deallocated list
         */
        if (st_DeallocatedHead)
        {
            st_DeallocatedHead->Prev = (struct Header *)ptr;
            st_MakeHeaderValid(st_DeallocatedHead);
        }

        h = (struct Header *)ptr;
        h->FreedFile   = file;
        h->FreedLine   = line;
        h->Deallocator = deallocator;
        h->Next        = st_DeallocatedHead;
        h->Prev        = 0;
        st_MakeHeaderValid(h);
        st_DeallocatedHead = h;

        if (!st_DeallocatedTail)
        {
            st_DeallocatedTail = h;
        }

        st_TotalDeallocated += h->Size;

    #ifdef FORTIFY_DEALLOCATED_MEMORY_LIMIT
        /*
         * If we've got too much on the deallocated list; free some
         */
        if (st_TotalDeallocated > FORTIFY_DEALLOCATED_MEMORY_LIMIT)
        {
             st_PurgeDeallocatedBlocks(st_TotalDeallocated - FORTIFY_DEALLOCATED_MEMORY_LIMIT, file, line);
        }
    #endif

        /*
         * End critical region
         */
        FORTIFY_UNLOCK();
    }
    else
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */
    {
           /*
         * Free the User Label
         */
        if (h->Label)
        {
            free(h->Label);
        }             

#ifdef FORTIFY_FILL_ON_DEALLOCATE
        /*
         * Nuke out all memory that is about to be freed, including the header
         */
        st_SetFortification(ptr, FORTIFY_FILL_ON_DEALLOCATE_VALUE,
                              FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size + FORTIFY_AFTER_SIZE);
#endif /* FORTIFY_FILL_ON_DEALLOCATE */

        /*
         * And do the actual free
         */
        free(ptr);
    }
}


/*
 * Fortify_LabelPointer() - Labels the memory block
 * with a string provided by the user. This function
 * takes a copy of the passed in string.
 * The pointer MUST be one returned by a Fortify
 * allocation function.
 */
/*@-redef@*/
void
Fortify_LabelPointer(void *uptr, const char *label, const char *file, DWORD line)
{
    if (!st_Disabled)
    {
        BYTE *ptr = (BYTE *)uptr
                              - FORTIFY_HEADER_SIZE - FORTIFY_ALIGNED_BEFORE_SIZE;
        struct Header *h = (struct Header *)ptr;

        /* make sure the pointer is okay */
        Fortify_CheckPointer(uptr, file, line);
    
        /* free the previous label */
        if (h->Label)
        {
            free(h->Label);
        }    
    
        /* make sure the label is sensible */
        assert(label);

        /* copy it in */
        h->Label = (char*)malloc(strlen(label)+1);
        strcpy(h->Label, label);
        
        /* update the checksum */
        st_MakeHeaderValid(h);
    }
}

/*
 * Fortify_CheckPointer() - Returns true if the uptr
 * points to a valid piece of Fortify_Allocated()'d
 * memory. The memory must be on the allocated list,
 * and it's fortifications must be intact.
 * Always returns TRUE if Fortify is disabled.
 */
int FORTIFY_STORAGE
Fortify_CheckPointer(void *uptr, const char *file, DWORD line)
{
    BYTE *ptr = (BYTE *)uptr
                              - FORTIFY_HEADER_SIZE - FORTIFY_ALIGNED_BEFORE_SIZE;
    struct Header *h = (struct Header *)ptr;
    int r;

    if (st_Disabled)
        return 1;

    FORTIFY_LOCK();

    if (!st_IsOnAllocatedList(h))
    {
        sprintf (st_Buffer, "\nFortify: Invalid pointer (%08lX) detected at %s.%lu\n",
                 (DWORD) uptr, file, line);

        st_Output(st_Buffer);
        FORTIFY_UNLOCK();
        return (0);
    }

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    if (st_IsOnDeallocatedList(h))
    {
        sprintf (st_Buffer, "\nFortify: Deallocated pointer (%08lX) detected at %s.%lu\n",
                 (DWORD) uptr, file, line);

        st_Output(st_Buffer);
        sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                           st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
        st_Output(st_Buffer);
        FORTIFY_UNLOCK();
        return (0);
    }
#endif

    r = st_CheckBlock(h, file, line);
    FORTIFY_UNLOCK();
    return r;
}

/*
 * Fortify_SetOutputFunc(Fortify_OutputFuncPtr Output) -
 * Sets the function used to output all error and
 * diagnostic messages. The output function  takes 
 * a single const BYTE * argument, and must be
 * able to handle newlines. This function returns the 
 * old output function.
 */
Fortify_OutputFuncPtr FORTIFY_STORAGE
Fortify_SetOutputFunc(Fortify_OutputFuncPtr Output)
{
    Fortify_OutputFuncPtr Old = st_Output;

    st_Output = Output;
  
    return (Old);
}

/*
 * Fortify_SetAllocateFailRate(int Percent) - 
 * Fortify_Allocate() will "fail" this Percent of 
 * the time, even if the memory is available. 
 * Useful to "stress-test" an application. 
 * Returns the old value.
 * The fail rate defaults to 0 (a good default I think).
 */
int FORTIFY_STORAGE
Fortify_SetAllocateFailRate(int Percent)
{
    int Old = st_AllocateFailRate;
  
    st_AllocateFailRate = Percent;
  
    return (Old);
}

 
/*
 * Fortify_CheckAllMemory() - Checks the fortifications
 * of all memory on the allocated list. And, if 
 * FORTIFY_DEALLOCATED_MEMORY is enabled, all the
 * known deallocated memory as well.
 * Returns the number of blocks that failed. 
 * Always returns 0 if Fortify is disabled.
 */
DWORD FORTIFY_STORAGE
Fortify_CheckAllMemory(const char *file, DWORD line)
{
    struct Header *curr = st_AllocatedHead;
    DWORD count = 0;

    if (st_Disabled)
        return 0;

    FORTIFY_LOCK();

    /*
     * Check the allocated memory
     */ 
    while (curr)
    {
        if (!st_CheckBlock(curr, file, line))
        {
          count++;
          break;    // !! added, GV 13/7-98
        }
        curr = curr->Next;      
    }

    /*
     * Check the deallocated memory while you're at it
     */
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    curr = st_DeallocatedHead;
    while (curr)
    {
        if (!st_CheckDeallocatedBlock(curr, file, line))
            count++;

        curr = curr->Next;      
    }
#endif    

    /*  
     * If we know where we are, and everything is cool,
     * remember that. It might be important.
     */
    if (file && count == 0)
    {
        st_LastVerifiedFile = file;
        st_LastVerifiedLine = line;
    }

    FORTIFY_UNLOCK();
    return (count);
}


/*
 * Fortify_EnterScope() - enters a new Fortify scope 
 * level. Returns the new scope level.
 */
BYTE FORTIFY_STORAGE
Fortify_EnterScope(const char *file, DWORD line)
{
    ARGSUSED (file);
    ARGSUSED (line);
    return (++st_Scope);
}

/* Fortify_LeaveScope - leaves a Fortify scope level,
 * also prints a memory dump of all non-freed memory 
 * that was allocated during the scope being exited.
 * Does nothing and returns 0 if Fortify is disabled.
 */
BYTE FORTIFY_STORAGE
Fortify_LeaveScope(const char *file, DWORD line)
{
    struct Header *curr = st_AllocatedHead;
    DWORD size = 0, count = 0;

    if (st_Disabled)
        return 0;

    FORTIFY_LOCK();

    st_Scope--;
    while (curr)
    {
        if (curr->Scope > st_Scope)
        {
            if (count == 0)
            {
                sprintf(st_Buffer, "\nFortify: Memory leak detected leaving scope at %s.%lu\n", file, line);
                st_Output(st_Buffer);
                sprintf(st_Buffer, "%10s %8s %s\n", "Address", "Size", "Allocator");
                st_Output(st_Buffer);
            }
            
            st_OutputHeader(curr);
            count++;
            size += curr->Size;
        }

        curr = curr->Next;      
    }

    if (count)
    {
        sprintf(st_Buffer,"%8s %8lu bytes in %lu blocks with %lu bytes overhead\n",
                "total", size, count, count * FORTIFY_OVERHEAD);
        st_Output(st_Buffer);
    }

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    /* 
     * Quietly free all the deallocated memory 
     * that was allocated in this scope that 
     * we are still tracking
     */
    st_PurgeDeallocatedScope (st_Scope, file, line);
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

    FORTIFY_UNLOCK();
    return (st_Scope);
}

/*
 * Fortify_ListAllMemory() - Outputs the entire 
 * list of currently allocated memory. For each block 
 * is output it's Address, Size, and the SourceFile and 
 * Line that allocated it.
 *
 * If there is no memory on the list, this function 
 * outputs nothing.
 *
 * It returns the number of blocks on the list, unless 
 * Fortify has been disabled, in which case it always 
 * returns 0.
 */
DWORD FORTIFY_STORAGE
Fortify_ListAllMemory(const char *file, DWORD line)
{
    struct Header *curr = st_AllocatedHead;
    DWORD size = 0, count = 0;

    if (st_Disabled)
        return 0;

    Fortify_CheckAllMemory(file, line);

    FORTIFY_LOCK();

    if (curr)
    {
        sprintf(st_Buffer, "\n\nFortify: Memory List at %s.%lu\n", file, line);
        st_Output(st_Buffer);
        sprintf(st_Buffer, "%10s %8s %s\n", "Address", "Size", "Allocator");
        st_Output(st_Buffer);
                                
        while (curr)
        {
            st_OutputHeader(curr);
            count++;
            size += curr->Size;
            curr = curr->Next;      
        }
                     
        sprintf(st_Buffer, "%8s %8lu bytes in %lu blocks and %lu bytes overhead\n",
                           "total", size, count, count * FORTIFY_OVERHEAD);
        st_Output(st_Buffer);
    }
  
    FORTIFY_UNLOCK();
    return (count);
}

/*
 * Fortify_DumpAllMemory() - Outputs the entire list of 
 * currently allocated memory. For each allocated block 
 * is output it's Address, Size, the SourceFile and Line
 * that allocated it, a hex dump of the contents of the 
 * memory and an ascii dump of printable characters.
 *
 * If there is no memory on the list, this function outputs nothing.
 */
DWORD FORTIFY_STORAGE
Fortify_DumpAllMemory(const char *file, DWORD line)
{
    struct Header *curr = st_AllocatedHead;
    DWORD count = 0;

    if (st_Disabled)
        return 0;

    Fortify_CheckAllMemory(file, line);

    FORTIFY_LOCK();

    while (curr)
    {
        sprintf(st_Buffer, "\nFortify: Hex Dump of %s at %s.%lu\n",
                st_MemoryBlockString(curr), file, line);
        st_Output(st_Buffer);
        st_OutputMemory(curr);
        st_Output("\n");
        count++;

        curr = curr->Next;
    }

    FORTIFY_UNLOCK();
    return (count);
}

/* Fortify_OutputStatistics() - displays statistics 
 * about the maximum amount of memory that was 
 * allocated at any one time.
 */
void FORTIFY_STORAGE
Fortify_OutputStatistics(const char *file, DWORD line)
{
    if (st_Disabled)
        return;

    sprintf(st_Buffer, "\nFortify: Statistics at %s.%lu\n", file, line);
    st_Output(st_Buffer);

    sprintf(st_Buffer, "         Memory currently allocated: %lu bytes in %lu blocks\n",
                                 st_CurAllocation, st_CurBlocks);
    st_Output(st_Buffer);
    sprintf(st_Buffer, "         Maximum memory allocated at one time: %lu bytes in %lu blocks\n", 
                                 st_MaxAllocation, st_MaxBlocks);
    st_Output(st_Buffer);
    sprintf(st_Buffer, "         There have been %lu allocations and %lu deallocations\n",
                                 st_Allocations, st_Frees);
    st_Output(st_Buffer);
    sprintf(st_Buffer, "         There was a total of %lu bytes allocated\n",
                                 st_TotalAllocation);
    st_Output(st_Buffer);
    
    if (st_Allocations > 0)
    {
        sprintf(st_Buffer, "         The average allocation was %lu bytes\n",
                                     st_TotalAllocation / st_Allocations);
        st_Output(st_Buffer);                   
    }    
}

/* Fortify_GetCurrentAllocation() - returns the number of
 * bytes currently allocated.
 */
DWORD FORTIFY_STORAGE
Fortify_GetCurrentAllocation(const char *file, DWORD line)
{
    ARGSUSED (file);
    ARGSUSED (line);

    if (st_Disabled)
        return 0;

    return st_CurAllocation;
}

/* Fortify_SetAllocationLimit() - set a limit on the total
 * amount of memory allowed for this application.
 */
void FORTIFY_STORAGE
Fortify_SetAllocationLimit(DWORD NewLimit, const char *file, DWORD line)
{
    ARGSUSED (file);
    ARGSUSED (line);

    st_AllocationLimit = NewLimit;
}

/*
 * Fortify_Disable() - Run time method of disabling Fortify.
 * Useful if you need to turn off Fortify without recompiling
 * everything. Not as effective as compiling out, of course.
 * The less memory allocated by Fortify when it is disabled
 * the better.
 * (Previous versions of Fortify did not allow it to be 
 * disabled if there was any memory allocated at the time,
 * but since in C++ memory is often allocated before main
 * is even entered, this was useless so Fortify is now
 * able to cope).
 */
void FORTIFY_STORAGE
Fortify_Disable(const char *file, DWORD line)
{
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
    /* free all deallocated memory we might be tracking */
    st_PurgeDeallocatedScope (0, file, line);
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

    st_Disabled = 1;
}



/*
 * st_CheckBlock - Check a block's header and fortifications.
 * Returns true if the block is happy.
 */
STATIC int
st_CheckBlock(struct Header *h, const char *file, DWORD line)
{
    BYTE *ptr = (BYTE *)h;
    int result = 1;

    if (!st_IsHeaderValid(h))
    {
      sprintf (st_Buffer,
               "\nFortify: Invalid pointer (%08lX) or corrupted header detected at %s.%lu\n",
               (DWORD)(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE), file, line);
                
      st_Output (st_Buffer);
      st_OutputLastVerifiedPoint();
      return (0);
    }

    if (!st_CheckFortification(ptr + FORTIFY_HEADER_SIZE,
                           FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE))
    {
        sprintf(st_Buffer, "\nFortify: Underwrite detected before block %s at %s.%lu\n",
                           st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);

        st_OutputLastVerifiedPoint();
        st_OutputFortification(ptr + FORTIFY_HEADER_SIZE,
                            FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE);
        result = 0;                        

#ifdef FORTIFY_FILL_ON_CORRUPTION
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE, FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE);
#endif
    }
                           
    if (!st_CheckFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                           FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE))
    {
        sprintf(st_Buffer, "\nFortify: Overwrite detected after block %s at %s.%lu\n",
                           st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);

        st_OutputLastVerifiedPoint();
        st_OutputFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                            FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE);
        result = 0;

#ifdef FORTIFY_FILL_ON_CORRUPTION
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                         FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE);
#endif
    }
 
    return (result);
}

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY

/* 
 * st_CheckDeallocatedBlock - Check a deallocated block's header and fortifications.
 * Returns true if the block is happy.
 */
STATIC int
st_CheckDeallocatedBlock(struct Header *h, const char *file, DWORD line)
{
    BYTE *ptr = (BYTE *)h;
    int result = 1;

    if (!st_IsHeaderValid(h))
    {
      sprintf (st_Buffer,
               "\nFortify: Invalid deallocated pointer (%08lX) or corrupted header detected at %s.%lu\n",
               (DWORD)(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE), file, line);
                
      st_Output(st_Buffer);
      st_OutputLastVerifiedPoint();
      return (0);
    }

    if (!st_CheckFortification(ptr + FORTIFY_HEADER_SIZE,
            FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE))
    {
        sprintf(st_Buffer, "\nFortify: Underwrite detected before deallocated block %s at %s.%lu\n",
                           st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);
        sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                           st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
        st_Output(st_Buffer);

        st_OutputLastVerifiedPoint();
        st_OutputFortification(ptr + FORTIFY_HEADER_SIZE,
                            FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE);

#ifdef FORTIFY_FILL_ON_CORRUPTION
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE, FORTIFY_BEFORE_VALUE, FORTIFY_ALIGNED_BEFORE_SIZE);
#endif
        result = 0;
    }
                           
    if (!st_CheckFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                           FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE))
    {
        sprintf(st_Buffer, "\nFortify: Overwrite detected after deallocated block %s at %s.%lu\n",
                           st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);
        sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                           st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
        st_Output(st_Buffer);

        st_OutputLastVerifiedPoint();
        st_OutputFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                            FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE);

#ifdef FORTIFY_FILL_ON_CORRUPTION
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size,
                         FORTIFY_AFTER_VALUE, FORTIFY_AFTER_SIZE);
#endif
        result = 0;
    }

#ifdef FORTIFY_FILL_ON_DEALLOCATE
    if (!st_CheckFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                           FORTIFY_FILL_ON_DEALLOCATE_VALUE, h->Size))
    {
        sprintf(st_Buffer, "\nFortify: Write to deallocated block %s detected at %s.%lu\n",
                           st_MemoryBlockString(h), file, line);
        st_Output(st_Buffer);

        sprintf(st_Buffer, "         Memory block was deallocated by \"%s\" at %s.%lu\n",
                           st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
        st_Output(st_Buffer);
        st_OutputLastVerifiedPoint();
    
        st_OutputFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                            FORTIFY_FILL_ON_DEALLOCATE_VALUE, h->Size);

#ifdef FORTIFY_FILL_ON_CORRUPTION
        st_SetFortification(ptr + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                            FORTIFY_FILL_ON_DEALLOCATE_VALUE, h->Size);
#endif /* FORTIFY_FILL_ON_CORRUPTION */
        result = 0;
    }
#endif /* FORTIFY_FILL_ON_DEALLOCATE */
    return result;
 }

#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */


/*
 * st_CheckFortification - Checks if the _size_ 
 * bytes from _ptr_ are all set to _value_
 * Returns true if all is happy.
 */
STATIC int
st_CheckFortification(BYTE *ptr, BYTE value, size_t size)
{
    while (size--)
        if (*ptr++ != value)
            return (0);
      
    return (1);
}

/*
 * st_SetFortification - Set the _size_ bytes from _ptr_ to _value_.
 */
STATIC void
st_SetFortification(BYTE *ptr, BYTE value, size_t size)
{
    memset(ptr, value, size);
}

/*
 * st_OutputFortification - Output the corrupted section of the fortification 
 */
STATIC void
st_OutputFortification(BYTE *ptr, BYTE value, size_t size)
{
    size_t offset, skipped, advance;
    offset = 0;

    sprintf(st_Buffer, "   Address   Offset Data (%02x)", value);
    st_Output(st_Buffer);

    while (offset < size)
    {
        /* 
         * Skip 3 or more 'correct' lines
         */
        if ((size - offset) < 3 * 16)
            advance = size - offset;
        else
            advance = 3 * 16;
        if (advance > 0 && st_CheckFortification(ptr+offset, value, advance))
        {
            offset += advance;
            skipped = advance;
                 
            if (size - offset < 16)
                advance = size - offset;
            else
                advance = 16;
             
            while (advance > 0 && st_CheckFortification(ptr+offset, value, advance))
            {
                offset  += advance;
                skipped += advance;
                if (size - offset < 16)
                    advance = size - offset;
                else
                    advance = 16;
            }    
            sprintf(st_Buffer, "\n                        ...%lu bytes skipped...", (DWORD)skipped);
            st_Output(st_Buffer);
            continue;
        }
        else
        {
            if (size - offset < 16)
                st_HexDump(ptr, offset, size-offset, 0);
            else
                st_HexDump(ptr, offset, 16, 0);
                
            offset += 16;
        }
    }

    st_Output("\n");
}

/*
 * st_HexDump - output a nice hex dump of "size" bytes, starting at "ptr" + "offset"
 */
STATIC void
st_HexDump(BYTE *ptr, size_t offset, size_t size, int title)
{
    char ascii[17];
    int  column;
    int  output;

    if (title)
        st_Output("   Address   Offset Data");

    column = 0;
    ptr += offset;
    output = 0;

    while (output < size)
    {
        if (column == 0)
        {
          sprintf(st_Buffer, "\n%08lX %8lu ", (DWORD)ptr, (DWORD)offset);
          st_Output(st_Buffer);
        }

        sprintf (st_Buffer, "%02x%s", *ptr, ((column % 4) == 3) ? " " : "");
        st_Output (st_Buffer);

        ascii [column]   = isprint(*ptr) ? (char)*ptr : '.';
        ascii [column+1] = '\0';

        ptr++;
        offset++;
        output++;
        column++;

        if (column == 16)
        {
            st_Output ("   \"");
            st_Output (ascii);
            st_Output ("\"");
            column = 0;
        }
    }

    if  (column != 0)
    {
        while  (column < 16)
        {
            if  (column % 4 == 3)
                st_Output ("   ");
            else
                st_Output ("  ");

            column++;
        }
        st_Output ("   \"");
        st_Output (ascii);
        st_Output ("\"");
    }
}

/*
 * st_IsHeaderValid - Returns true if the 
 * supplied pointer does indeed point to a 
 * real Header
 */
STATIC int
st_IsHeaderValid(struct Header *h)                                
{
    return (st_ChecksumHeader(h) == FORTIFY_CHECKSUM_VALUE);
}

/*
 * st_MakeHeaderValid - Updates the checksum 
 * to make the header valid
 */
STATIC void
st_MakeHeaderValid(struct Header *h)
{
    h->Checksum = 0;
    h->Checksum = (WORD)(FORTIFY_CHECKSUM_VALUE - st_ChecksumHeader(h));
}

/*
 * st_ChecksumHeader - Calculate (and return) 
 * the checksum of the header. (Including the 
 * Checksum field itself. If all is well, the 
 * checksum returned by this function should
 * be FORTIFY_CHECKSUM_VALUE
 */
STATIC WORD
st_ChecksumHeader(struct Header *h)
{
#if (DOSX)
    return inchksum_fast (h, FORTIFY_HEADER_SIZE/sizeof(WORD));
#else
    WORD c, checksum, *p;
  
    for(c = 0, checksum = 0, p = (WORD *)h;
        c < FORTIFY_HEADER_SIZE/sizeof(WORD); c++)
    {    
        checksum += *p++;  
    }    
    
    return (checksum);
#endif
}                  

/* 
 * st_IsOnAllocatedList - Examines the allocated 
 * list to see if the given header is on it.
 */  
STATIC int
st_IsOnAllocatedList(struct Header *h)
{
    struct Header *curr;
  
    curr = st_AllocatedHead;
    while (curr)
    {
        if (curr == h)
            return (1);
      
        curr = curr->Next;
    }
  
    return (0);
}

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
/* 
 * st_IsOnDeallocatedList - Examines the deallocated 
 * list to see if the given header is on it.
 */  
STATIC int
st_IsOnDeallocatedList(struct Header *h)
{
    struct Header *curr;
  
    curr = st_DeallocatedHead;
    while (curr)
    {
        if (curr == h)
            return (1);
      
        curr = curr->Next;
    }
  
    return (0);
}

/*
 * st_PurgeDeallocatedBlocks - free at least "Bytes"
 * worth of deallocated memory, starting at the 
 * oldest deallocated block.
 * Returns true if any blocks were freed.
 */
STATIC int
st_PurgeDeallocatedBlocks(DWORD Bytes, const char *file, DWORD line)
{
    DWORD FreedBytes = 0;
    DWORD FreedBlocks = 0;

#ifdef FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY
    sprintf(st_Buffer, "\nFortify: Warning - Discarding deallocated memory at %s.%lu\n", 
                       file, line);
    st_Output(st_Buffer);
#endif /* FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY */

    while (st_DeallocatedTail && FreedBytes < Bytes)
    {
        st_CheckDeallocatedBlock(st_DeallocatedTail, file, line);
        FreedBytes += st_DeallocatedTail->Size;
        FreedBlocks++;
#ifdef FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY
#ifdef FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY
        sprintf(st_Buffer, "                %s\n",
                           st_DeallocatedMemoryBlockString(st_DeallocatedTail));
        st_Output(st_Buffer);
#endif /* FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY */
#endif /* FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY */
        st_FreeDeallocatedBlock(st_DeallocatedTail, file, line);
    }

    return FreedBlocks != 0;
}

/*
 * st_PurgeDeallocatedScope - free all deallocated
 * memory blocks that were allocated within "Scope"
 */
STATIC int
st_PurgeDeallocatedScope(BYTE Scope, const char *file, DWORD line)
{
    struct Header *curr;
    DWORD FreedBlocks = 0;

    curr = st_DeallocatedHead;
    while (curr)
    {
        struct Header *next = curr->Next;
        if (curr->Scope >= Scope)
        {
            st_FreeDeallocatedBlock(curr, file, line);
            FreedBlocks++;
        }

        curr = next;
    }
    
    return FreedBlocks != 0;
}

/*
 * st_FreeDeallocatedBlock - actually remove
 * a deallocated block from the deallocated 
 * list, and actually free it's memory.
 */
STATIC void
st_FreeDeallocatedBlock(struct Header *h, const char *file, DWORD line)
{
    st_CheckDeallocatedBlock (h, file, line);

   /* 
    * Begin Critical region 
    */
    FORTIFY_LOCK();

    st_TotalDeallocated -= h->Size;
    
    if (st_DeallocatedHead == h)
    {
        st_DeallocatedHead = h->Next;
    }
    
    if (st_DeallocatedTail == h)
    {
        st_DeallocatedTail = h->Prev;
    }
    
    if (h->Prev)
    {
        st_CheckDeallocatedBlock(h->Prev, file, line);
        h->Prev->Next = h->Next;
        st_MakeHeaderValid(h->Prev);    
    }
    
    if (h->Next)
    {
        st_CheckDeallocatedBlock(h->Next, file, line);
        h->Next->Prev = h->Prev;
        st_MakeHeaderValid(h->Next);    
    }

    /*
     * Free the label
     */
    if (h->Label)
    {
        free(h->Label);
    }

    /*
     * Nuke out all memory that is about to be freed, including the header
     */
    st_SetFortification((BYTE*)h, FORTIFY_FILL_ON_DEALLOCATE_VALUE,
                        FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE + h->Size + FORTIFY_AFTER_SIZE);

    /*
     * And do the actual free
     */
    free(h);
    
    /*
     * End critical region
     */
    FORTIFY_UNLOCK();    
}

#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */

/*
 * st_OutputMemory - Hex and ascii dump the 
 * user memory of a block.
 */
STATIC void
st_OutputMemory(struct Header *h)
{
    st_HexDump((BYTE*)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
               0, h->Size, 1);
}


/*
 * st_OutputHeader - Output the header
 */
STATIC void st_OutputHeader (struct Header *h)
{
  BYTE *adr = (BYTE*)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE;

  if (h->Label == NULL)
       sprintf (st_Buffer, "%08lX %8lu %s.%lu\n",
                (DWORD) adr, (DWORD)h->Size, h->File, h->Line);

  else sprintf (st_Buffer, "%08lX %8lu %s.%lu %s\n",
                (DWORD) adr, (DWORD)h->Size, h->File, h->Line, h->Label);
  st_Output(st_Buffer);
}

/*
 * st_OutputLastVerifiedPoint - output the last
 * known point where everything was hoopy.
 */
STATIC void
st_OutputLastVerifiedPoint()
{
    sprintf(st_Buffer, "         Memory integrity was last verified at %s.%lu\n", 
                       st_LastVerifiedFile,
                       st_LastVerifiedLine);
    st_Output(st_Buffer);
}

/*
 * st_MemoryBlockString - constructs a string that 
 * desribes a memory block. (pointer,size,allocator,label)
 */
STATIC const char *
st_MemoryBlockString(struct Header *h)
{
    static char st_BlockString[512];
    
    if (h->Label == 0)
    {
        sprintf (st_BlockString,"(%08lX,%lu,%s.%lu)",
                (DWORD)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                (DWORD)h->Size, h->File, h->Line);
    }            
    else
    {
        sprintf(st_BlockString,"(%08lX,%lu,%s.%lu,%s)",
                (DWORD)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                (DWORD)h->Size, h->File, h->Line, h->Label);
    }

    return st_BlockString;
}

#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
#ifdef FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY
#ifdef FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY
/*
 * st_DeallocatedMemoryBlockString - constructs
 * a string that desribes a deallocated memory
 * block. (pointer,size,allocator,deallocator)
 */

STATIC const char *
st_DeallocatedMemoryBlockString(struct Header *h)
{
    static char st_BlockString[256];

    if (h->Label == 0)
    {
        sprintf(st_BlockString,"(%08lX,%lu,%s.%lu,%s.%lu)",
                (DWORD)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                (DWORD)h->Size, h->File, h->Line, h->FreedFile, h->FreedLine);
    }            
    else
    {
        sprintf(st_BlockString,"(%08lX,%lu,%s.%lu,%s.%lu,%s)",
                (DWORD)h + FORTIFY_HEADER_SIZE + FORTIFY_ALIGNED_BEFORE_SIZE,
                (DWORD)h->Size, h->File, h->Line, h->FreedFile, h->FreedLine, h->Label);
    }

    return st_BlockString;
}
#endif /* FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY */
#endif /* FORTIFY_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY */
#endif /* FORTIFY_TRACK_DEALLOCATED_MEMORY */


/*
 * st_DefaultOutput - the default output function
 */
STATIC void
st_DefaultOutput(const char *String)
{
    fprintf(stdout, String);
    fflush(stdout);
}

/*
 * Fortify_malloc - Fortify's replacement malloc()
 */
void *FORTIFY_STORAGE
Fortify_malloc(size_t size, const char *file, DWORD line)
{
    return Fortify_Allocate(size, Fortify_Allocator_malloc, file, line);
}

/*
 * Fortify_realloc - Fortify's replacement realloc()
 */
void *
Fortify_realloc(void *uptr, size_t new_size, const char *file, DWORD line)
{
    BYTE *ptr = (BYTE *)uptr - FORTIFY_HEADER_SIZE - FORTIFY_ALIGNED_BEFORE_SIZE;
    struct Header *h = (struct Header *)ptr;
    void *new_ptr;

    /*
     * If Fortify is disabled, we gotta do this a little 
     * differently.
     */
    if (!st_Disabled)
    {
        if (!uptr)
            return (Fortify_Allocate(new_size, Fortify_Allocator_realloc, file, line));
        
        if (!st_IsOnAllocatedList(h))
        {
#ifdef FORTIFY_TRACK_DEALLOCATED_MEMORY
            if (st_IsOnDeallocatedList(h))
            {
                sprintf(st_Buffer, "\nFortify: Deallocated memory block passed to \"%s\" at %s.%lu\n",
                                    st_AllocatorName[Fortify_Allocator_realloc], file, line);
                st_Output(st_Buffer);
                sprintf(st_Buffer,   "         Memory block %s was deallocated by \"%s\" at %s.%lu\n",
                                   st_MemoryBlockString(h),
                                   st_DeallocatorName[h->Deallocator], h->FreedFile, h->FreedLine);
                st_Output(st_Buffer);
                return 0;
            }
#endif


            sprintf (st_Buffer, "\nFortify: Invalid pointer (%08lX) passed to realloc at %s.%lu\n",
                     (DWORD)ptr, file, line);

            st_Output(st_Buffer);
            return 0;
        }

        if (!st_CheckBlock(h, file, line))
            return 0;
      
        new_ptr = Fortify_Allocate(new_size, Fortify_Allocator_realloc, file, line);
        if (!new_ptr)
        {
            return (0);
        }
      
        if (h->Size < new_size)
            memcpy(new_ptr, uptr, h->Size);
        else
            memcpy(new_ptr, uptr, new_size);
    
        Fortify_Deallocate(uptr, Fortify_Deallocator_realloc, file, line);
        return (new_ptr);
    }
    else
    {
        /*
         * If the old block was fortified, we can't use normal realloc.
         */
        if (st_IsOnAllocatedList(h))
        {
            new_ptr = Fortify_Allocate(new_size, Fortify_Allocator_realloc, file, line);
            if (!new_ptr)
                return (0);
          
            if (h->Size < new_size)
                memcpy(new_ptr, uptr, h->Size);
            else
                memcpy(new_ptr, uptr, new_size);
        
            Fortify_Deallocate(uptr, Fortify_Deallocator_realloc, file, line);
            return (new_ptr);
        }
        else /* easy */
        {
            return realloc(uptr, new_size);
        }
    }
}

/*
 * Fortify_calloc - Fortify's replacement calloc
 */
void *
Fortify_calloc(size_t num, size_t size, const char *file, DWORD line)
{
    if (!st_Disabled)
    {
        void *ptr = Fortify_Allocate(size * num, Fortify_Allocator_calloc, file, line);
        if (ptr)
        {
            memset(ptr, 0, size*num);
        }
        return ptr;
    }    
    else
    {
        return calloc(num, size);
    }
}

/*
 * Fortify_free - Fortify's replacement free
 */
void  
Fortify_free(void *uptr, const char *file, DWORD line)
{
    /* it is defined to be safe to free(0) */
    if (uptr == 0)
        return;

    Fortify_Deallocate(uptr, Fortify_Deallocator_free, file, line);
}

/*
 * Fortify_strdup - Fortify's replacement strdup. Since strdup isn't
 * ANSI, it is only provided if FORTIFY_STRDUP is defined.
 */
#ifdef FORTIFY_STRDUP
char *FORTIFY_STORAGE
Fortify_strdup(const char *oldStr, const char *file, DWORD line)
{
    if (!st_Disabled)
    {
        char *newStr = Fortify_Allocate(strlen(oldStr)+1, Fortify_Allocator_strdup, file, line);
        if (newStr)
        {
            strcpy(newStr, oldStr);
        }
    
        return newStr;
    }    
    else
    {
        return strdup(oldStr);
    }
}
#endif /* FORTIFY_STRDUP */

STATIC void st_OutputDeleteTrace (void)
{
#ifdef __cplusplus
  if (st_DeleteStackTop > 1)
  {
    sprintf(st_Buffer, "Delete Trace: %s.%lu\n", st_DeleteFile[st_DeleteStackTop-1],
                                                 st_DeleteLine[st_DeleteStackTop-1]);
    st_Output(st_Buffer);
    for(int c = st_DeleteStackTop-2; c >= 0; c--)
    {
        sprintf(st_Buffer, "              %s.%lu\n", st_DeleteFile[c],
                                                     st_DeleteLine[c]);
        st_Output(st_Buffer);
    }
  }
#endif
}

#ifdef __cplusplus

/*
 * st_NewHandler() - there is no easy way to get
 * the new handler function. And isn't it great
 * how the new handler doesn't take a parameter
 * giving the size of the request that failed.
 * Thanks Bjarne!
 */
Fortify_NewHandlerFunc
st_NewHandler()
{
    /* get the current handler */
    Fortify_NewHandlerFunc handler = set_new_handler(0);

    /* and set it back (since we cant 
     * get it without changing it)
     */
    set_new_handler(handler);
    
    return handler;
}

/*
 * operator new - Fortify's replacement new, 
 * without source-code information.
 */
void *FORTIFY_STORAGE
operator new(size_t size)
{
    void *p;
    
    while ((p = Fortify_Allocate(size, Fortify_Allocator_new,
                                st_AllocatorName[Fortify_Allocator_new], 0)) == 0)
    {                            
        if (st_NewHandler())
            (*st_NewHandler())();
        else
            return 0;
    }

    return p;
}

/*
 * operator new - Fortify's replacement new,
 * with source-code information
 */
void *FORTIFY_STORAGE
operator new(size_t size, const char *file, DWORD line)
{
    void *p;

    while ((p = Fortify_Allocate(size, Fortify_Allocator_new, file, line)) == 0)
    {
        if (st_NewHandler())
            (*st_NewHandler())();
        else
            return 0;
    }
    
    return p;
}

#ifdef FORTIFY_PROVIDE_ARRAY_NEW

/*
 * operator new[], without source-code info
 */
void *FORTIFY_STORAGE
operator new[](size_t size) 
{
    void *p;

    while ((p = Fortify_Allocate(size, Fortify_Allocator_array_new,
                                st_AllocatorName[Fortify_Allocator_array_new], 0)) == 0)
    {                                
        if (st_NewHandler())
            (*st_NewHandler())();    
        else    
            return 0;
    }
    
    return p;
}

/*
 * operator new[], with source-code info
 */
void *FORTIFY_STORAGE
operator new[](size_t size, const char *file, DWORD line)
{
    void *p;

    while ((p = Fortify_Allocate(size, Fortify_Allocator_array_new, file, line)) == 0)
    {
        if (st_NewHandler())
            (*st_NewHandler())(); 
        else
            return 0;
    }

    return p;
}

#endif /* FORTIFY_PROVIDE_ARRAY_NEW */

/*
 * Fortify_PreDelete - C++ does not allow overloading
 * of delete, so the delete macro calls Fortify_PreDelete
 * with the source-code info, and then calls delete.
 */
void FORTIFY_STORAGE
Fortify_PreDelete(const char *file, DWORD line)
{
    FORTIFY_LOCK();

    /*
     * Push the source code info for the delete onto the delete stack
     * (if we have enough room, of course)
     */
    if (st_DeleteStackTop < FORTIFY_DELETE_STACK_SIZE)
    {
        st_DeleteFile[st_DeleteStackTop] = file;
        st_DeleteLine[st_DeleteStackTop] = line;
    }

    st_DeleteStackTop++;
}

/*
 * Fortify_PostDelete() - Pop the delete source-code info
 * off the source stack.
 */
void FORTIFY_STORAGE
Fortify_PostDelete()
{
    st_DeleteStackTop--;

    FORTIFY_UNLOCK();
}

/*
 * operator delete - fortify's replacement delete
 */
void FORTIFY_STORAGE
operator delete(void *uptr)
{
    const char *file;
    DWORD line;

     /*
      * It is defined to be harmless to delete 0
      */
    if (uptr == 0)
        return;

    /*
     * find the source-code info
     */
    if (st_DeleteStackTop)
    {
        if (st_DeleteStackTop < FORTIFY_DELETE_STACK_SIZE)
        {
            file = st_DeleteFile[st_DeleteStackTop-1];
            line = st_DeleteLine[st_DeleteStackTop-1];
        }
        else
        {
            file = st_DeleteFile[FORTIFY_DELETE_STACK_SIZE-1];
            line = st_DeleteLine[FORTIFY_DELETE_STACK_SIZE-1];
        }
    }
    else
    {
        file = st_DeallocatorName[Fortify_Deallocator_delete];
        line = 0;
    }

    Fortify_Deallocate(uptr, Fortify_Deallocator_delete, file, line);
}

#ifdef FORTIFY_PROVIDE_ARRAY_DELETE

/*
 * operator delete[] - fortify's replacement delete[]
 */
void FORTIFY_STORAGE
operator delete[](void *uptr)
{
    const char *file;
    DWORD line;

     /*
      * It is defined to be harmless to delete 0
      */
    if (uptr == 0)
        return;

    /*
     * find the source-code info
     */
    if (st_DeleteStackTop)
    {
        if (st_DeleteStackTop < FORTIFY_DELETE_STACK_SIZE)
        {
            file = st_DeleteFile[st_DeleteStackTop-1];
            line = st_DeleteLine[st_DeleteStackTop-1];
        }
        else
        {
            file = st_DeleteFile[FORTIFY_DELETE_STACK_SIZE-1];
            line = st_DeleteLine[FORTIFY_DELETE_STACK_SIZE-1];
        }
    }
    else
    {
        file = st_DeallocatorName[Fortify_Deallocator_array_delete];
        line = 0;
    }

    Fortify_Deallocate(uptr, Fortify_Deallocator_array_delete, file, line);
}

#endif /* FORTIFY_PROVIDE_ARRAY_DELETE */

#ifdef FORTIFY_AUTOMATIC_LOG_FILE
/* Automatic log file stuff!
 *
 * AutoLogFile class. There can only ever be ONE of these
 * instantiated! It is a static class, which means that
 * it's constructor will be called at program initialization,
 * and it's destructor will be called at program termination.
 * We don't know if the other static class objects have been
 * constructed/destructed yet, but this pretty much the best
 * we can do with standard C++ language features.
 */
class Fortify_AutoLogFile
{
    static FILE *fp;
    static int   written_something;
    static char *init_string, *term_string;

public:
    Fortify_AutoLogFile()
    {
        written_something = 0;
        Fortify_SetOutputFunc(Fortify_AutoLogFile::Output);
        Fortify_EnterScope(init_string, 0);
    }

    static void Output(const char *s)
    {
        if (written_something == 0)
        {
            FORTIFY_FIRST_ERROR_FUNCTION;
            fp = fopen(FORTIFY_LOG_FILENAME, "w");
            if (fp)
            {
                time_t t;
                time(&t);
                fprintf(fp, "Fortify log started at %s\n", ctime(&t));
                written_something = 1;
            }    
        }

        if (fp)
        {
            fputs(s, fp);
            fflush(fp);
        }    
    }
    
    ~Fortify_AutoLogFile()
    {
        Fortify_LeaveScope(term_string, 0);
        Fortify_CheckAllMemory(term_string, 0);
        if (fp)
        {
            time_t t;
            time(&t);
            fprintf(fp, "\nFortify log closed at %s\n", ctime(&t));
            fclose(fp);
            fp = 0;
        }    
    }
};

FILE *Fortify_AutoLogFile::fp = 0;
int   Fortify_AutoLogFile::written_something = 0;
char *Fortify_AutoLogFile::init_string = "Program Initialization";
char *Fortify_AutoLogFile::term_string = "Program Termination";

static Fortify_AutoLogFile Abracadabra;

#endif /* FORTIFY_AUTOMATIC_LOG_FILE */

#endif /* __cplusplus */

#endif /* USE_FORTIFY || USE_BSD_FORTIFY */

