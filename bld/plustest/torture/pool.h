// This is a fast memory allocator modelled after class Pool in the
// AT&T Standard Library Extension.
//
// Every Pool is a collection of elements, each of which is a chunk of
// contigous memory of unspecified type.  All elements of a Pool are
// the same size.
//
// If "n" is an unsigned integer, the constructor Pool(n) generates
// a pool whose elements are of size "n".  Destoying a Pool frees all
// the memory occupied by its elements.
//
// .SS Performance
// The use of pools substantially reduces the number of calls to malloc().
// Allocation of a new element is done inline, except when a new
// block of elements must be allocated.
//
// .SS Author
// Dag Bruck, Department of Automatic Control, Lund Institute of Technology,
// Box 118, S-221 00 Lund, Sweden (dag@control.lth.se).
//
// Modifications
// 30 Jul 1992 Jam      renamed Pool to DAG_Pool
// 30 Jul 1992 Jam      use size_t instead of unsigned
// 
// $Id: POOL.H 1.1 93/06/24 11:17:40 Anthony_Scian Exp $


#ifndef DAG_Pool_H
#define DAG_Pool_H

#include <stddef.h>    // for size_t

class DAG_Pool {
public:
  DAG_Pool(size_t n);
  // Creates a pool whose elements are of size "n".

  void* alloc();
  // A new element is allocated in the pool.  The result is a pointer
  // to the element.

  void free(void* vp);
  // The element of the pool addressed by "vp" is freed.  The element must
  // have been allocated from this pool; if not the results are undefined.

  ~DAG_Pool();
  // Returns all memory allocated by the pool to the system.

private:
  struct Pool_Element { Pool_Element* next; };
  // The beginning of a pool element, as seen in the free list.
  
  struct Pool_Block { Pool_Block* next; Pool_Element pe; };
  // The beginning of a block of allocated pool memory.
  
  Pool_Element* free_list;
  // The list of free pool elements.

  size_t sz;
  // The size of the pool elements, in bytes.  This number is rounded up
  // to a multiple of some small number (to ensure alignment).

  Pool_Block* blockp;
  // List of memory blocks allocated by alloc_block().

  void alloc_block();
  // Allocates a new block of memory and splits it up in elements
  // that are inserted into the free list.
};


inline void* DAG_Pool :: alloc()
{
  if (free_list == 0)
    alloc_block();
  register Pool_Element* pe = free_list;
  free_list = pe->next;
  return pe;
}


inline void DAG_Pool :: free(void* p)
{
  register Pool_Element* pe = (Pool_Element *) p;
  pe->next = free_list;
  free_list = pe;
}


#endif
