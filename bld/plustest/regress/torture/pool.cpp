// $Id: POOL.CPP 1.1 94/12/14 13:50:36 Anthony_Scian Exp $
//
// Author:
//
// Dag Bruck, Department of Automatic Control, Lund Institute of Technology,
// Box 118, S-221 00 Lund, Sweden (dag@control.lth.se).


#include <Pool.h>


const size_t UNIT = sizeof(DAG_Pool :: Pool_Element);


DAG_Pool :: DAG_Pool(size_t n)
    : free_list(0), blockp(0), sz(((n + UNIT - 1) / UNIT) * UNIT)
{
  alloc_block();
}


DAG_Pool :: ~DAG_Pool()
{
  register Pool_Block* b;

  while (blockp != 0) {
    b = blockp->next;
    delete blockp;
    blockp = b;
  }
}


void DAG_Pool :: alloc_block()
{
  register size_t n = (UNIT*1024) / sz;
  // The number of elements to allocate in a block

  // Allocate a new block of elements, and insert it into a list
  // so it can be released when the pool is destroyed.
  
  register Pool_Block* blk =
    (Pool_Block *) new char[sizeof(Pool_Block) + n * sz];
  blk->next = blockp;
  blockp = blk;

  // Link together all elements in the new block.
  
  register Pool_Element* pe = &blk->pe;
  while (--n > 0) {
    pe->next = (Pool_Element *) ((char *) pe + sz);
    pe = pe->next;
  }

  // The last element of the block points to the existing free list,
  // which normally is empty (but needn't be).  The new free list
  // starts with the first element of the newly allocated block.

  pe->next = free_list;
  free_list = &blk->pe;
}
