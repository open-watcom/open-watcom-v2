/****************************************************************************
File: newlog.cpp

Description:

void* operator new(size_t) and operator delete(void*) - these maintain
   list of allocated pointers to make sure not deleting object twice and
   to catch memory leaks.


Notes:
   Need to test beginning and end of blocks for overwrites and store
   size of each block for log.
   
Date        Programmer  Modification
03 Jul 92   Jam         created from jamassrt
28 Sep 92   Jam         renamed and changed #includes to <jam/*.h>

****************************************************************************/

#ifndef JAM_IGNEWLOG  /* don't link anything */

#include <fstream.h>
#include <new.h>
#include <limits.h>  // CHAR_MAX filler
#include <stdlib.h>
#include <NewLog.h>
#include <PlaceNew.h>
#include <assert.hpp>
#include <VPSet.h>
#include <Iter.h>

#if defined(__TURBOC__) && !defined(_Windows)
#include <alloc.h>   // use heapcheck() function
const int HCSTEP = 10;  // how often to check the heap
static int hccounter = 0;  // when gets to zero, check heap; dec each new/del
#endif

struct CAllocater {
   static void* alloc(size_t s) { s = (s>0) ? s : 1; return malloc(s); }
   static void free(void* p) { ::free(p); }
};

typedef JAM_ControlledVoidPtrSet<CAllocater> VoidPtrSet;
static union { char buf[sizeof(VoidPtrSet)]; void* force_align1; long force_align2; } allocated_buffer;
VoidPtrSet* allocated;  // maintains list of "live" new blocks

ofstream* JAM_NewLog::newlog = 0;

void JAM_NewLog::list_still_allocated(ostream& os)
{
   JAM_assert(allocated!=0);
   for (JAM_Iter<VoidPtrSet> i(allocated); !i.offEnd(); ++i)
         os << "alive\t" << *i << "\n";
}

int JAM_NewLog::create_newlog()
{
   JAM_assert(newlog==0);
   static const char* logname = getenv("JAM_NEWLOG");
   if (logname && *logname=='\0') logname = 0;
   static union { char buf[sizeof(ofstream)]; void* force_align1; long force_align2; } newlog_buffer;
   static int creating_newlog = 0;
   if (logname && !creating_newlog) {
      creating_newlog = 1;    // set flag in case of reentry
      newlog = new (newlog_buffer.buf) ofstream(logname);
      if (!newlog->good())
         JAM_crash("Error opening newlog '%s'.", logname);
      creating_newlog = 0;
   }
   return newlog!=0;
}

ostream& JAM_NewLog::log()
{
   JAM_assert(newlog!=0);
   return *newlog;
}

void JAM_NewLog::put(const char* file, int line)
{
   if (activate()) log() << file << ',' << line << '\t' << flush;
}

typedef void (*new_handler_func)( void );

static new_handler_func get_new_handler()
{
    new_handler_func save;

    save = set_new_handler( NULL );
    set_new_handler( save );
    return( save );
}

void* JAM_NewLog::alloc(size_t size)
{
#if defined(__TURBOC__) && !defined(_Windows)
   if (newlog && hccounter==0)
      if ((hccounter=HCSTEP), heapcheck()<0)
         JAM_crash("Heap corrupted before new.");
#endif
   if (allocated==0) {
      static int already_in = 0;
      JAM_assert(!already_in);
      already_in = 1;
      allocated = new (allocated_buffer.buf) VoidPtrSet;
      }

   void* p;
   size = size ? size : 1;
   if (activate()) log() << "new \t" << size << "\t" << flush;
   new_handler_func _new_handler = get_new_handler();
   while ( (p = ::malloc(size)) == 0 && _new_handler != 0) {
      if (activate()) log() << "failed\n" << flush;
      _new_handler();
      }
   if (activate()) log() << p << "\n" << flush;
   if (p) {
      if (allocated->contains(p))
         JAM_crash("new ptr %p already allocated!", p);
      else
         allocated->enter(p);
      memset(p, CHAR_MAX, size); //##
      }
#if defined(__TURBOC__) && !defined(_Windows)
   if (newlog && hccounter==0)
      if ((hccounter=HCSTEP), heapcheck()<0)
         JAM_crash("Heap corrupted after new.");
   --hccounter;
#endif
   return p;
}

void JAM_NewLog::free(void* p)
{
#if defined(__TURBOC__) && !defined(_Windows)
   if (hccounter==0)
      if ((hccounter=HCSTEP), heapcheck()<0)
         JAM_crash("Heap corrupted before deleting.");
#endif
   if (activate()) log() << "delete \t" << p << "\n" << flush;
   if (p) {
      if (!allocated->contains(p)) {
         if (activate()) log() << "deleting unallocated pointer " << p << endl;
         JAM_crash("deleting unallocated pointer %p", p);
         }
      else {
         *((char*)p) = CHAR_MAX;  //##
         allocated->remove(p);
         ::free(p);
         }
      }
#if defined(__TURBOC__) && !defined(_Windows)
   if (hccounter==0)
      if ((hccounter=HCSTEP), heapcheck()<0)
         JAM_crash("Heap corrupted after deleting.");
   --hccounter;
#endif
}

void *operator new( size_t size )
{
   JAM_NewLog::activate();
   return JAM_NewLog::alloc(size);
}

void operator delete(void* p)
{
   JAM_NewLog::free(p);
}

#endif // JAM_IGNEWLOG  /* don't link anything */


