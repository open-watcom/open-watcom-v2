



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */






#ifdef __GNUC__
#pragma implementation
#include <string.h>
#endif

#include "base/error.h"
#include "base/memory.h"

#undef new // Have to do this within this scope.

#include <iostream.h>
#include <iomanip.h>

#ifdef __BORLANDC__
typedef unsigned size_t;
#include <mem.h>
#endif


#include <stdlib.h>
#include <malloc.h>

const  ulong magic_cookie = 0xf9a42bb1; // Used to indicate "allocated" state


struct BlockHeader {
    ulong marker;
    long  time;
    BlockHeader* prev;
    BlockHeader* next;
    const char* file;
    long  lineNo;
    long  size;
};

static BlockHeader* _AllocListHead = 0; // Head of list of allocated blocks
static BlockHeader* _AllocListTail = 0; // Tail of list of allocated blocks
static short        _LeakCheckerActive = 0;
static long         _Time = 0;          // Clock: ticks on every new and
                                        // delete

void* operator new (size_t n)
{
    if (n == 0)
        return NULL;
    long size = n;
    // Allocate  extra bytes
    uchar* p = (uchar*) malloc (n + sizeof (BlockHeader));
    if (!p) {
        CL_Error::Fatal ("YACL base: allocating %u bytes: no memory!", n);
    }
    if (!_LeakCheckerActive)
        return (uchar*)p + sizeof (BlockHeader);  // Do logging only if at
                                                  // least one
                                                  // LeakChecker is alive
    BlockHeader* q = (BlockHeader*) p;
    // Put a magic marker
    q->marker = magic_cookie;
    q->time   = _Time++;
    q->lineNo = 0;
    q->file   = "";
    q->size   = size;
    memset (p +sizeof(BlockHeader),  '\02', size); // Uninitialized allocated
                                                   // memory has 02 in it
    // Insert at tail of allocated list
    if (_AllocListTail) {
        _AllocListTail->next = q;
        q->prev = _AllocListTail;
        q->next = 0;
        _AllocListTail = q;
    }
    else {
        _AllocListHead = _AllocListTail = q;
        q->prev = q->next = 0;
    }
    return p + sizeof(BlockHeader);
}


void* operator new (size_t size, short line_no, const char* file_name)
{
    uchar* p = new uchar [size]; // Appeal to above new operator
    BlockHeader* q = (BlockHeader*) (p - sizeof (BlockHeader));
    q->file = file_name;
    q->lineNo = line_no;
    return p;
}



#if (defined(__GNUC__) && __GNUC_MINOR__ >= 6) ||  \
    (defined(__BORLANDC__) && __BCPLUSPLUS__ >= 0x0330) || \
    (defined(__WATCOMC__) && __WATCOMC__ >= 1000)
void* operator new[] (size_t size, short line_no, const char* file_name)
{
    return operator new (size, line_no, file_name);
}
#endif


void operator delete (void* p)
{
    BlockHeader* q = (BlockHeader*) ( (uchar*) p - sizeof (BlockHeader));
    if (!_LeakCheckerActive) {
        free (q);
        return;
    }
    if (q->marker != magic_cookie)
        CL_Error::Warning ("YACL op delete (%8lx): memory corrupted", p);
    else {
        if (_AllocListHead) { // Without this test, we get problems with
                              // static objects. So we set AllocListHead to
                              // zero in LeakChecker's destructor.
            if (q->prev)
                q->prev->next = q->next;
            if (q->next)
                q->next->prev = q->prev;
            if (q == _AllocListHead)
                _AllocListHead = q->next;
            if (q == _AllocListTail)
                _AllocListTail = q->prev;
            memset (q,  '\03', sizeof(BlockHeader) + q->size);
                // Freed  memory has 03 in it
        }
        free (q);
    }
    _Time++;
}




CL_MemoryLeakChecker::CL_MemoryLeakChecker (ostream& stream)
: _stream (stream)
{
    _LeakCheckerActive++;
}

CL_MemoryLeakChecker::~CL_MemoryLeakChecker ()
{
    _LeakCheckerActive--;
    if (!_stream.good())
        return;
    if (!_AllocListHead) {
        // _stream << "\n\n\nCL_MemoryLeakChecker: No memory leaks.\n";
        return;
    }
    BlockHeader* q = _AllocListHead;
    _stream << "\n\n\nCL_MemoryLeakChecker: Memory leaks:\n"
            <<       "-----------------------------------\n";
    while (q) {
        _stream << "Time: " << /* setw (8) << */ q->time;
        _stream << " Address: " << hex << (void*) q << dec;
        _stream << " Size: " << /* setw (6) << */ q->size << " bytes";
        if (q->lineNo)
            _stream << " File: " << q->file << " line # " << q->lineNo;
        _stream << endl;
        q = q->next;
    }
    _AllocListHead = _AllocListTail = 0;
}
