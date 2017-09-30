//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// Created: MJF 03/27/89 -- Initial design and implementation.
// Updated: MJF 04/15/89 -- Implemented Base list class.
// Updated: JCB 06/05/89 -- Fixed merge and sort.
// Updated: JCB 06/20/89 -- Implemented next_union, next_intersection,
//                          next_difference, next_xor.
// Updated: JCB 06/21/89 -- Modified next() and prev() to check for a current
//                          position not in THIS list, i.e. !traversal
// Updated: MJF 08/03/89 -- Improved operator== by checking for identical nodes
// Updated: MJF 08/10/89 -- Changed return values of operator+=, etc to List ref
// Updated: MJF 08/11/89 -- Changed methods which return new list references:
//                          tail, last, but_last, member, sublist and copy
// Updated: LGO 09/07/89 -- Made next, reference and dereference inline
// Updated: LGO 09/07/89 -- Moved the guts of find to the template classes.
// Updated: LGO 09/08/89 -- use do_find instead of compare_data in most places.
// Updated: LGO 09/08/89 -- Move push to the template classes.
// Updated: MBN 09/20/89 -- Added conditional exception handling
// Updated: LGO 10/05/89 -- Don't sort lists with less than 2 elements
// Updated: MBN 10/11/89 -- Change "current_position" to "curpos" and also
//                          "previous_position" to "prevpos"
// Updated: LGO 12/04/89 -- Make binary set operators not inline
// Updated: LGO 12/05/89 -- Make sort/merge predicate ANSI compatable
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 05/22/90 -- Fixed remove_duplicates
// Updated:  VDN 02/21/92 -- new lite version and fix memory leaks
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- modernized template syntax, remove macro hacks
//
// This  file contains member and friend  function implementation code for  the
// CoolList class defined in the  Base_List.h header  file.  Where appropriate  and
// possible, interfaces  to, and  us of,  existing system   functions has  been
// incorporated.  An overview of the structure of the CoolList class, along  with a
// synopsis of each member and friend function, can be found in the Base_List.h
// header file.

#ifndef BASE_LISTH                              // If CoolBase_List class not defined,
#include <cool/Base_List.h>                     // include header file
#endif

CoolBase_List NIL = CoolBase_List();                    // NIL -- Global NIL CoolBase_List

// CoolBase_List_Node() -- Constructor
// Input:         None
// Output:        None

CoolBase_List_Node::CoolBase_List_Node() {}             // Constructor


// ~CoolBase_List_Node -- Destructor (not inline because it's virtual)
//                   Virtual is needed to call ~CoolList_Node<Type>.
// Input:         None
// Output:        None

CoolBase_List_Node::~CoolBase_List_Node() {;}


// get_data()  -- Gets data of node. There is no data for CoolBase_List class.
// Input:         None.
// Output:        A void* pointer.

const void* CoolBase_List_Node::get_data() {
  cout << "\nWarning:  CoolBase_List_Node::get_data() has been called.\n";
  return NULL;
}


// set_data()  -- Sets data of node to specified value. There is not data
//                to set for CoolBase_List class.
// Input:         A void* pointer.
// Output:        None.

void CoolBase_List_Node::set_data(const void*) {
#if ERROR_CHECKING
  //RAISE (Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::set_data(): Method called is redundant.\n");
#endif
}


// ~CoolBase_List -- Destructor (not inline because it's virtual)
// Input:         None
// Output:        None

CoolBase_List::~CoolBase_List () {;}

// Prev() -- decrement current position. If NULL, set it to last.
// Input:    None.
// Output:   TRUE or FALSE.

Boolean CoolBase_List::prev() {
  register CoolBase_List_Node* cp = this->curpos;
  CoolBase_List_Node* prev = this->prevpos;
  // if cp invalid (i.e. NULL) the following test will fail
  if (prev != NULL && prev->next == cp) {
    // already have the previous position
    cp = prev;
    this->prevpos = NULL; // this isn't needed, but why not...
  } else {
    // find previous node of current position
    // When cp is invalid, this finds the last node.
    register CoolBase_List_Node* np = this->node_ptr;
    prev = NULL;
    if (np != NULL) {
      while(np->next != cp)
        prev = np, np = np->next;
      cp = np;
      this->prevpos = prev;    
    }
  }
  // current position not found in CoolBase_List
  return (this->curpos = cp) != NULL;
}

// operator== -- Returns TRUE if similar data in THIS and the specified CoolBase_List.
//               Uses the static CoolBase_List comparer function on elements.
// Input:        A CoolBase_List reference.
// Output:       TRUE or FALSE.

Boolean CoolBase_List::operator==(const CoolBase_List& l) const {
  if (this == &l) return TRUE;  // same CoolBase_Lists
  CoolBase_List_Node *np, *np_l;
  for (np = this->node_ptr, np_l = l.node_ptr;
      np != NULL && np_l != NULL; 
      np = np->next, np_l = np_l->next) {
    if (np == np_l)
      return TRUE;   // same node pointers
    else if (!this->compare_data(np->get_data(), np_l->get_data()))
      return FALSE;
  }
  if (np == NULL && np_l == NULL) return TRUE;
  else return FALSE;
}


// tail() -- Sets CoolBase_List passed to the the nth tail of this CoolBase_List. With no 2nd
//           argument, tail() sets CoolBase_List passed to the 1st tail of this CoolBase_List.
// Input:    A CoolBase_List reference to store the nth tail elements of THIS and
//           the positive integer, n. (default 1)
// Output:   None.

void CoolBase_List::tail(CoolBase_List& l, int n) {
  this->dereference(l.node_ptr);
  int count = n;
  CoolBase_List_Node* np;
  for (np = this->node_ptr; np != NULL && count > 0; np = np->next, count--);
  // if n < length, return nth tail
  if (np != NULL && count == 0) {
    this->curpos = np;
    this->reference(np);
    l.node_ptr = np;
  }
  else l.node_ptr = NULL;
}


// last() -- Sets CoolBase_List passed to the n last elements of this CoolBase_List. With no
//           2nd argument, sets CoolBase_List passed to the last element of thie CoolBase_List.
// Input:    A CoolBase_List reference to store the n last elements of THIS and
//           the positive interger, n (default 1).
// Output:   None.

void CoolBase_List::last(CoolBase_List& l, int n) {
  // last n elements = (size-n)th tail
  int nth = this->length() - n;   
  if (nth >= 0)
    tail(l, nth);
  else {
    this->dereference(l.node_ptr);
    l.node_ptr = NULL;
  }
}


// but_last() -- Sets CoolBase_List passed to all but the last n elements of THIS List. 
//               With no 2nd argument, sets CoolBase_List passed to all but the last
//               element
// Input:        A positive integer (default 1).
// Output:       An altered THIS with all but the n last elements.

void CoolBase_List::but_last(CoolBase_List& l, int n) {
  this->reset();                                // Current position invalid
  CoolBase_List_Node* np = this->node_ptr;
  this->dereference(l.node_ptr);                // Delete nodes in l
  if (n == 0) {
    this->reference(np);                        
    l.node_ptr = np;                            // Points at all nodes of THIS
  }
  else {
    // get the number of nodes in this CoolBase_List and copy into CoolBase_List l
    int no_nodes = this->length() - n; 
    if (no_nodes > 0 && np != NULL)  {
      CoolBase_List_Node* first_np = this->insert_after_node(np->get_data(), NULL);
      CoolBase_List_Node* rest_np = first_np;   
      no_nodes--;
      for(np = np->next; 
          np != NULL && no_nodes > 0; 
          np = np->next, no_nodes--) {
        rest_np = this->insert_after_node(np->get_data(), rest_np);
      }
      l.node_ptr = first_np;                    // Create totally new nodes
    } else
      l.node_ptr = NULL;
  }
}


// clear() -- Removes all nodes of THIS CoolBase_List.
// Input:     None.
// Output:    None.

void CoolBase_List::clear() {
  this->reset();                                // make current position invalid   
  this->dereference(this->node_ptr);            // Delete nodes in this.
  this->node_ptr = NULL;
}


// length() -- Returns the number of elements in THIS CoolBase_List.
// Input:      None.
// Output:     An interger representing the number of elements in THIS.

int CoolBase_List::length() {
  int count = 0;
  for (CoolBase_List_Node* np = this->node_ptr; np != NULL; np = np->next, count++);
  return count;
}


// position -- Returns current position.
// Input:      THIS.
// Output:     An integer representing current position.

int CoolBase_List::position() {
  if (this->curpos == NULL) return -1;
  int index = 0;
  for (CoolBase_List_Node* np = this->node_ptr;  np != NULL; np = np->next, index++)
    if (np == this->curpos) return index;
  return -1;
}

// search() -- Returns TRUE if the specified CoolBase_List is a subset of this CoolBase_List
// Input:      A reference to subList to be searched.
// Output:     TRUE or FALSE.

Boolean CoolBase_List::search(const CoolBase_List& l) {
  CoolBase_List_Node* tnode = this->node_ptr;
  CoolBase_List_Node* lnode = l.node_ptr;
  if (tnode == lnode) {
    // this CoolBase_List and subList l have same head node pointers
    this->curpos = tnode;
    this->prevpos = NULL;
    return TRUE;  
  }
  if (lnode == NULL) return FALSE;      // subList l is nil
  while(tnode != NULL &&
        this->do_find(tnode, lnode->get_data(), this->curpos, this->prevpos)) {
    tnode = this->curpos;
    if (tnode == lnode) {
      // a node in this CoolBase_List and head node of CoolBase_List l are same
      return TRUE;
    }
    // data of node in this CoolBase_List is also in node of CoolBase_List l
    // continue searching rest of data
    CoolBase_List_Node *np2, *lnp;
    for (np2 = tnode->next, lnp = lnode->next; 
         np2 != NULL && lnp != NULL;
         np2 = np2->next, lnp = lnp->next) {
      if (np2 == lnp) {
        // node in this CoolBase_List and node in CoolBase_List l are same
        return TRUE;
      }
      if  (!this->compare_data(np2->get_data(), lnp->get_data()))
        break;
    }
    if (lnp == NULL) {
      // found data of CoolBase_List l in this CoolBase_List
      return TRUE;
    }
    tnode = tnode->next;
  } 
  return FALSE;
}


// sublist() -- Returns TRUE if THIS CoolBase_List contains second argument and sets
//              first argument to a sublist in THIS CoolBase_List starting at the 1st
//              occurence of second argument.
// Input:       A reference to the CoolBase_List which will be set to a sublist within
//              THIS CoolBase_List; and a reference to the CoolBase_List in search of.
// Output:      TRUE or FALSE.

Boolean CoolBase_List::sublist(CoolBase_List& l, const CoolBase_List& subl) {
  this->dereference(l.node_ptr);
  if (this->search(subl)) {
    this->reference(this->curpos);
    l.node_ptr = this->curpos;  
    return TRUE;
  } else {
    l.node_ptr = NULL;
    return FALSE;
  }
}


// copy() -- Mutates *this to store a copy of elements in specified List
// Input:    List containing elements to be copied from
// Output:   None.

void CoolBase_List::copy (const CoolBase_List& l) {
  cout << "****warning: copy from argument into *this****" << endl;
  this->reset();                                // make current position invalid
  this->dereference(this->node_ptr);            // free current nodes of *this
  CoolBase_List_Node* np = l.copy_nodes();              // make new list of nodes of l
  this->node_ptr = np;                          // all with ref_count = 1
}

// reverse() -- Reverses the order of the elements of THIS CoolBase_List.
// Input:       None.
// Output:      None.

void CoolBase_List::reverse() {
  this->reset();                                // Current position invalid
  CoolBase_List_Node *np, *np2, *rev_head;
  rev_head = NULL;
  for (np = this->node_ptr; np != NULL; np = np2) {
    np2 = np->next;
    np->next = rev_head;
    rev_head = np;
  }
  this->node_ptr = rev_head;
}


// prepend() -- Prepends the specified CoolBase_List to the front of this CoolBase_List and
//              returns TRUE if specified CoolBase_List is not NULL
// Input:       A reference to the CoolBase_List to be prepended.
// Output:      Always TRUE.

Boolean CoolBase_List::prepend(const CoolBase_List& l) {
  // first get copy of nodes in CoolBase_List l 
  CoolBase_List_Node* lnode = l.copy_nodes();
  if (lnode == NULL)
    return FALSE;
  // now prepend copy of l to front of THIS CoolBase_List.  
  this->curpos = lnode;
  this->prevpos = NULL;
  CoolBase_List_Node* last_np = this->curpos;
  if (last_np != NULL) {  
    // find last node of new copy of l
    while(last_np->next != NULL) last_np = last_np->next;
    // insert last node of CoolBase_List l in front of head node of this CoolBase_List
    // and set head node of this CoolBase_List to head node of CoolBase_List l
    last_np->next = this->node_ptr;
    this->node_ptr = this->curpos;
  }
  return TRUE;
}


// append() -- Appends the specified CoolBase_List to the end of this CoolBase_List and returns
//             TRUE if specified CoolBase_List is not NULL.
// Input:      A reference to the CoolBase_List to be appended.
// Output:     TRUE or FALSE.

Boolean CoolBase_List::append(const CoolBase_List& l) {
  CoolBase_List_Node* lnode = l.node_ptr;
  if (lnode == NULL)
    return FALSE;
  this->curpos = lnode;                         // curpos is head node of l
  CoolBase_List_Node* last_np = this->node_ptr;
  if (last_np == NULL) { 
    // there are no nodes in this;
    // just set head node of this CoolBase_List to head node of CoolBase_List l
    this->node_ptr = this->curpos;
    this->prevpos = NULL;
  }
  else {
    // find last node of this
    while(last_np->next != NULL) last_np = last_np->next;
    // add head node of CoolBase_List l after last node of this CoolBase_List
    last_np->next = this->curpos;
    this->prevpos = last_np;
  }
  this->reference(this->curpos);
  return TRUE;
}


// set_tail() -- Sets the nth tail of THIS CoolBase_List to the specified CoolBase_List
// Input:        A CoolBase_List reference, and a count (default 1).
// Output:       TRUE if nth tail exists, FALSE otherwise.

Boolean CoolBase_List::set_tail(const CoolBase_List& l, int n) {
  // find node at start of nth tail
  int count = n;
  CoolBase_List_Node *np, *prev_np;
  for (np = this->node_ptr, prev_np = NULL; 
      np != NULL && count > 0;
      prev_np = np, np = np->next, count--);
  if (np != NULL && count == 0) {
     this->curpos = l.node_ptr; // Current position=head node
     this->prevpos = prev_np;           // Previous position is nth-1
     // replace nth node with head node of CoolBase_List l
     if (prev_np == NULL) this->node_ptr = this->curpos; // when n=0
     else prev_np->next = this->curpos;        // when n>0
     this->reference(this->curpos);
     // removing nth tail
     this->dereference(np);
     return TRUE;
   }
  return FALSE;
}


// remove_duplicates() -- Removes all duplicate elements in THIS CoolBase_List.
// Input:                 None.
// Output:                TRUE if at least one item is removed, FALSE otherwise.

Boolean CoolBase_List::remove_duplicates() {
  Boolean success = FALSE;                      // success flag
  CoolBase_List_Node* np = this->node_ptr;
  CoolBase_List_Node* np_next;
  while ((np != NULL) && ((np_next = np->next) != NULL)) {
    while (this->do_find (np_next, np->get_data(), this->curpos, this->prevpos))
      {
        this->dereference(this->remove());      // duplicate nodes go to trash
        success = TRUE;
        if ((np_next = this->curpos) == NULL) break;
      }
    np = np->next;
  }
  return success;
}


// set_intersection() -- Changes THIS list to contain everything that is an element
//                   of both this and the specified CoolBase_List.
// Input:            A reference to CoolBase_List.
// Output:           None.

void CoolBase_List::set_intersection(const CoolBase_List& l) {
  this->reset();                                // make current position invalid
  if (l.node_ptr == NULL)
    this->clear();
  else {
    CoolBase_List_Node* np = this->node_ptr;
    CoolBase_List_Node* prev_np = NULL;
    CoolBase_List_Node* next_np;
    while(np != NULL) {
      CoolBase_List_Node* cp;
      CoolBase_List_Node* pp;
      if (!l.do_find(l.node_ptr, np->get_data(), cp, pp)) {
        next_np = np->next;
        if (prev_np == NULL) 
          this->node_ptr = np->next;            // next node is at head
        else 
          prev_np->next = np->next;             // next node is in middle
        this->reference(np->next);
        this->dereference(np);                  // delete np from this CoolBase_List.
        np = next_np;                           // set to next node to look at
      }
      else {
        prev_np = np; 
        np = np->next;
      }
    }
  }
}


// set_union() -- Changes THIS list to contain everything that is an element of
//             either THIS or the specified CoolBase_List
// Input:      A reference to CoolBase_List.
// Output:     None.

void CoolBase_List::set_union(const CoolBase_List& l) {
  this->reset();                                // make current position invalid
  CoolBase_List_Node* tnode = this->node_ptr;
  CoolBase_List_Node* lnode = l.node_ptr;
  if (tnode ==  NULL && lnode != NULL) { 
    this->node_ptr = lnode;                     // this is empty, so set head node 
    this->reference(lnode);                     // of this to head node of l
  }
  else
    for (; lnode !=NULL; lnode = lnode->next) {
      CoolBase_List_Node* cp;
      CoolBase_List_Node* pp;
      if (!do_find(this->node_ptr, lnode->get_data(), cp, pp))
        this->push(lnode->get_data());          // push new data of l into this.
    }
}


// set_difference() -- Removes from THIS CoolBase_List the elements which also appears in
//                 the specified CoolBase_List.
// Input:          A reference to CoolBase_List.
// Output:         None.

void CoolBase_List::set_difference(const CoolBase_List& l) {
  this->reset();                                // make current position invalid
  if (l.node_ptr != NULL) {
    CoolBase_List_Node* np = this->node_ptr;
    CoolBase_List_Node* prev_np = NULL;
    CoolBase_List_Node* next_np;
    // if node in this CoolBase_List is found in CoolBase_List l, remove it from this CoolBase_List
    while (np != NULL) {
      CoolBase_List_Node* cp;
      CoolBase_List_Node* pp;
      if (l.do_find(l.node_ptr, np->get_data(), cp, pp)) {
        next_np = np->next;
        if (prev_np == NULL) 
          this->node_ptr = np->next;            // Next node is at head
        else 
          prev_np->next = np->next;             // next node is in middle
        this->reference(np->next);
        this->dereference(np);                  // delete np from this CoolBase_List.
        np = next_np;                           // set to next node to look at
      }
      else {
        prev_np = np; 
        np = np->next;
      }
    }
  }
}


// set_xor() -- Changes CoolBase_List to contain those elements which appear in
//                   exactly one of THIS CoolBase_List and the specified CoolBase_List.
// Input:            A reference to CoolBase_List.
// Output:           None.

void CoolBase_List::set_xor(const CoolBase_List& l) {
  this->reset();        // make current position invalid
  // first get copy of nodes in CoolBase_List l
  CoolBase_List* lcopy = this->new_list((CoolBase_List_Node*)NULL);
  lcopy->copy(l);
  // remove elements in this CoolBase_List from copy of CoolBase_List l
  lcopy->set_difference(*this);
  // remove elements in CoolBase_List l from this CoolBase_List  
  this->set_difference(l);
  // append elements left in lcopy to end of this CoolBase_List
  this->append(*lcopy);
  delete lcopy;                                 // destructor is virtual
}

// next_intersection() -- Sets current position to next item in the
//                        intersection of this CoolBase_List and the specified CoolBase_List.
// Input:                 None.
// Output:                TRUE if the next item exists,  FALSE otherwise.

Boolean CoolBase_List::next_intersection(const CoolBase_List& l) {
  CoolBase_List_Node* np = this->curpos;
  if (!this->traversal) {                       // end of intersection
    this->curpos = NULL;
    return FALSE;
  }
  if (np == NULL)                               // starting fresh
    np = this->node_ptr;
  else
    np = np->next;                              // increment position
  for (; np != NULL; np = np->next) {
    CoolBase_List_Node* cp;
    CoolBase_List_Node* pp;
    if (l.do_find(l.node_ptr, np->get_data(), cp, pp)) {
      this->curpos = np;
      return TRUE;
    }
  }
  this->traversal = FALSE;                      // now traversing second CoolBase_List
  return FALSE;
}


// next_union() -- Sets current position to next item in the union of this
//                  CoolBase_List and the specified CoolBase_List.
// Input:           None.
// Output:          TRUE if the next item exists,  FALSE otherwise.

Boolean CoolBase_List::next_union(const CoolBase_List& l) {
  CoolBase_List_Node* np = this->curpos;
  if (np == NULL && !this->traversal)           // end of both CoolBase_Lists
    return FALSE;
  if (np == NULL && this->traversal)            // starting fresh
    np = this->node_ptr;
  else  
    if (np->next == NULL && this->traversal) {  // end of first CoolBase_List
      this->traversal = FALSE;                  // now traversing second CoolBase_List
      np = l.node_ptr;
    }
  else
    if (!this->traversal)  np = np->next;       // increment position

  if (this->traversal) {                        // still in this CoolBase_List
    if (this->curpos != NULL) 
      this->curpos = np->next;  // increment current position
    else this->curpos = np;
    return TRUE;                        
  }

  for (; np != NULL; np = np->next) {
    CoolBase_List_Node* cp;
    CoolBase_List_Node* pp;
    if (!do_find(this->node_ptr, np->get_data(), cp, pp)) {
      this->curpos = np;
      return TRUE;
    }
  }
  return FALSE;
}


// next_difference() -- Sets current position to next item in the difference
//                      of this CoolBase_List and the specified CoolBase_List
// Input:               None.
// Output:              TRUE if the next item exists,  FALSE otherwise.

Boolean CoolBase_List::next_difference(const CoolBase_List& l) {
  CoolBase_List_Node* np = this->curpos;
  if (np == NULL && !this->traversal) {         // end of this CoolBase_List
    return FALSE;
  }
  if (np == NULL)                               // starting fresh
    np = this->node_ptr;
  else {
    this->traversal = FALSE;                    // now traversing second CoolBase_List
    np = np->next;                              // incrementing position
  }
  for (; np != NULL; np = np->next) {
    CoolBase_List_Node* cp;
    CoolBase_List_Node* pp;
    if (!l.do_find(l.node_ptr, np->get_data(), cp, pp)) {
      this->curpos = np;
      return TRUE;
    }
  }
  return FALSE;
}


// next_xor() -- Sets current position to next item in exclusive_or
//                        of this CoolBase_List and the specified CoolBase_List.
// Input:                 None.
// Output:                TRUE if the next item exists,  FALSE otherwise.

Boolean CoolBase_List::next_xor(const CoolBase_List& l) {
  CoolBase_List_Node* cp;
  CoolBase_List_Node* pp;
  CoolBase_List_Node* np = this->curpos;
  if (!this->traversal && np == NULL)           // end of both CoolBase_Lists
    return FALSE;
  if (np == NULL && this->traversal)            // starting fresh
    np = this->node_ptr;
  else 
    if (np->next == NULL && this->traversal) {  // end of first CoolBase_List
      this->traversal = FALSE;                  // now traversing second CoolBase_List
      np = l.node_ptr;
    }
  else
    np = np->next;                              // increment position
  if (this->traversal) {                        // still in this CoolBase_List
    for (; np != NULL; np = np->next) 
      if (!l.do_find(l.node_ptr, np->get_data(), cp, pp)) {
        this->curpos = np;
        return TRUE;                    
      } 
    this->traversal = FALSE;                    // now traversing second CoolBase_List
    np = l.node_ptr; 
  }
  for (; np != NULL; np = np->next)
    if (!do_find(this->node_ptr, np->get_data(), cp, pp)) {
      this->curpos = np;
      return TRUE;
    }
  return FALSE;
}


// describe() -- Describes internal structure of each node of this CoolBase_List
// Input:        The output stream to display description on.
// Output:       None.

void CoolBase_List::describe(ostream& os) {
  os << "\nCoolBase_List " << this << ":\n";
  int count = 0;
  for (CoolBase_List_Node* np = this->node_ptr; np != NULL; np = np->next, count++) 
  {
    os << "Node" << count << ":\n";
    os << " Data = ";
    this->output_data(os,np);
    os << "\n";
    os << " Ref count = " << np->ref_count << "\n";
  }
}


// operator<<() -- Overload output operator for CoolBase_List objects
// Input:          An output stream reference and CoolBase_List reference
// Output:         An output stream reference.

ostream& operator<<(ostream& os, const CoolBase_List& l) {
  os << "(";
  CoolBase_List_Node* np = l.node_ptr;
  if (np != NULL) {
    l.output_data(os, np);
    for (np = np->next; np != NULL; np = np->next) {
      os << " ";
      l.output_data(os,np);
    }
  }
  return os << ")";
}


// new_list() -- Returns new CoolBase_List with head node initialized to specified node
// Input:        The node pointer.
// Output:       A pointer to the new CoolBase_List.

CoolBase_List* CoolBase_List::new_list(CoolBase_List_Node*) {
#if ERROR_CHECKING
  //RAISE Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::new_list(): Method called is redundant.\n");
#endif
  return (CoolBase_List*)NULL;  
}


// insert_before_node() -- Inserts a new node before the specified node.
// Input:                  A Type reference and a Node pointer.
// Output:                 A pointer to the new node.

CoolBase_List_Node* CoolBase_List::insert_before_node(const void*, CoolBase_List_Node*) {
#if ERROR_CHECKING
  //RAISE Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::insert_before_node(): Method called is redundant.\n");
#endif
 return NULL;
}


// insert_after_node() -- Inserts a new node after the specified node.
// Input:                 A Type reference and a Node pointer.
// Output:                A pointer to the new node.

CoolBase_List_Node* CoolBase_List::insert_after_node(const void*, CoolBase_List_Node*) const {
#if ERROR_CHECKING
  //RAISE Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::insert_after_node(): Method called is redundant.\n");
#endif
 return NULL;
}


// compare_data() -- Compares data using default compare function of this CoolBase_List
// Input:            Two const void* pointers which will be type cast.
// Output:           None.

Boolean CoolBase_List::compare_data(const void*, const void*) const {
#if ERROR_CHECKING
  //RAISE Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::compare_data(): Method called is redundant.\n");
#endif
  return FALSE;
}


// output_data()  -- Outputs node data from specified stream.
// Input:            An output stream reference and node pointer.
// Output:           A void* pointer of data.

void CoolBase_List::output_data(ostream&, const CoolBase_List_Node*) const {

#if ERROR_CHECKING
  //RAISE Warning, SYM(CoolBase_List_Node), SYM(Redundant_Method_Call),
  printf ("CoolBase_List_Node::output_data(): Method called is redundant.\n");
#endif
}


// copy_nodes() -- Returns a copy of nodes in THIS CoolBase_List.
// Input:          NONE.
// Output:         The first node pointer of copy.

CoolBase_List_Node* CoolBase_List::copy_nodes() const {
  CoolBase_List_Node *np, *first_np;
  if ((np = this->node_ptr) == NULL)  
     first_np = NULL;
  else {
    // copy first node of this CoolBase_List
    first_np = this->insert_after_node(np->get_data(), NULL);
    CoolBase_List_Node* rest_np = first_np;

    // copy rest of nodes of this CoolBase_List
    for (np = np->next; np != NULL; np = np->next) {
      rest_np = this->insert_after_node(np->get_data(), rest_np);
    }
  }
  return first_np;
}


void CoolBase_List::free_nodes(CoolBase_List_Node* np) {
  do {
    if (np->ref_count < 0) {
      cout << "\nWarning: negative ref count of " << np->ref_count;
      cout << " for data ";
      this->output_data(cout, np);
      cout << " and ";
#if GENERIC_TYPECHECK
      cout << (this->type_of())->name() << " = ";
#endif
      cout << this << ".\n";
    } 
    // no longer sharing this node with other CoolBase_Lists
    CoolBase_List_Node* next_np = np->next;     // save pointer to next node
    np->next = NULL;
//  delete np->data;                        // not nec. data is ptr or obj
    delete np;                              // delete node structure
    np = next_np;                           // check reference of next node
  }
  while (np != NULL && --(np->ref_count) <= 0);
}

// operator= -- Assigns THIS to the specified CoolBase_List.
// Input:       A reference to a CoolBase_List which THIS will be assigned to.
// Output:      The modified THIS.

CoolBase_List* CoolBase_List::operator=(const CoolBase_List& l) {
  this->reset();                                // make current position invalid
  this->reference(l.node_ptr);                  // will point to new stuff
  this->dereference(this->node_ptr);            // delete old stuff
  this->node_ptr = l.node_ptr;
  return this;
}


// operator[]() -- Returns the nth node of this CoolBase_List
// Input:          A positive integer index.
// Output:         A pointer to the nth node.

CoolBase_List_Node* CoolBase_List::operator[] (int n) {
  int count = n;
  CoolBase_List_Node *np, *prev_np;
  for (np = this->node_ptr, prev_np = NULL;
      np != NULL && count > 0;
      prev_np = np, np = np->next, count--);

  // if n<length,  found nth node
  if (np != NULL && count == 0) {
    this->curpos = np;
    this->prevpos = prev_np;
    return np;
  }
  else return (CoolBase_List_Node*)NULL;  // or error
}


// position() -- Returns the position of the specified data item in this CoolBase_List,
//               else returns -1 if not found
// Input:        A void* pointer of a data item.
// Output:       The integer position.

int CoolBase_List::position(const void* x) {
  int index = 0;
  // NOTE: It's faster to call do_find, then count the position,
  // than to do the search ourselves using compare_data and get_data.
  if(!this->do_find(this->node_ptr, x, this->curpos, this->prevpos))
    return -1;
  else {
    int index = 0;
    CoolBase_List_Node* cp = this->curpos;
    CoolBase_List_Node* np = this->node_ptr;
    while (np != cp) np = np->next, index++;
    return index;
  }
}


// do_find() -- Returns TRUE if the specified element is a member of THIS CoolBase_List.
// Input:       A void* pointer of data item to be searched.
// Output:      TRUE or FALSE.

Boolean CoolBase_List::do_find(CoolBase_List_Node*, const void*,
                      CoolBase_List_Node*&, CoolBase_List_Node*&) const {
  cout << "\nWarning:  CoolBase_List_Node::find(x) has been called.\n";
  return FALSE;
}

// member() -- Returns TRUE if THIS CoolBase_List contains the specified element and
//             sets the specified CoolBase_List to a subList in THIS CoolBase_List starting
//             at the first occurence of element.
// Input:      A reference to the CoolBase_List which will be set to a subCoolBase_List within
//             THIS CoolBase_List, and a void* pointer of data item to be searched.
// Output:     A CoolBase_List pointer to some tail of THIS.

Boolean CoolBase_List::member(CoolBase_List& l,const void* x) {
  this->dereference(l.node_ptr);
  if (this->do_find(this->node_ptr, x, this->curpos, this->prevpos)) {
    this->reference(this->curpos);
    l.node_ptr = this->curpos;
    return TRUE;
  } else {
    l.node_ptr = NULL;
    return FALSE;
  }
}


// push() -- Prepends the specified data item to the front of CoolBase_List
// Input:    A void* pointer of the data item to be prepended.
// Output:   Always TRUE.

Boolean CoolBase_List::push(const void* x) {
  this->curpos = this->insert_before_node(x, this->node_ptr);
  this->node_ptr = this->curpos;
  this->prevpos = NULL;
  return TRUE;
}


// push_new() -- Pushes a new element at head of THIS CoolBase_List if it is not already
//               a member of the CoolBase_List.
// Input:        A void* pointer of new data.
// Output:       TRUE if item not on CoolBase_List, FALSE otherwise.

Boolean CoolBase_List::push_new(const void* x) {
  CoolBase_List_Node* cp;
  CoolBase_List_Node* pp;
  if (!this->do_find(this->node_ptr, x, cp, pp)) // don't change curpos
    return push(x);
  else
    return FALSE;
}


// push_end() -- Appends the specified data item to the end of this CoolBase_List
// Input:        A void* pointer of the data item to be appended.
// Output:       Always TRUE.

Boolean CoolBase_List::push_end(const void* x) {
  CoolBase_List_Node* last_np = this->node_ptr;
  if (last_np == NULL) {  
    // there arn't any nodes in this CoolBase_List; x will be the head node
    this->node_ptr = this->curpos = this->insert_after_node(x, NULL);
    this->prevpos = NULL;
  }
  else {
    CoolBase_List_Node* cp = this->curpos;
    // find last node of this CoolBase_List
    if (cp != NULL && cp->next == NULL)
      last_np = cp;
    else
      while(last_np->next != NULL) last_np = last_np->next;
    // insert x after last node
    this->curpos = this->insert_after_node(x, last_np);  
    this->prevpos = last_np;
  }
 return TRUE;
}


// push_end_new() -- Pushes a new element at end of THIS CoolBase_List if it is not
//                   already a member of the CoolBase_List.
// Input:            A void* pointer of new data.
// Output:           TRUE if item not on CoolBase_List, FALSE otherwise.

Boolean CoolBase_List::push_end_new(const void* x) {
  if (!this->do_find(this->node_ptr, x, this->curpos, this->prevpos))
    return this->push_end(x);
  else
    return FALSE;
}


// pop() -- Removes and returns head element of THIS CoolBase_List.
// Input:   None.
// Output:  Node pointer containing head data element of THIS CoolBase_List.

CoolBase_List_Node* CoolBase_List::pop() {
  CoolBase_List_Node* old_head = this->node_ptr;
  if (old_head != NULL) {
    this->reset();                              // make current position invalid
    this->node_ptr = old_head->next;
    this->reference(old_head->next);
    return old_head;
  }
  else return NULL;
}


// remove() -- Removes item at current position.
// Input:      None.
// Output:     The node pointer of item removed.

CoolBase_List_Node*  CoolBase_List::remove() {
  if (this->curpos == NULL)
    return NULL;
  CoolBase_List_Node* np = this->prevpos;
  CoolBase_List_Node* cp = this->curpos;
  if (np == NULL || np->next != cp) { 
    np  = this->node_ptr;
    if (np == cp) {                             // node to remove is head node
      this->curpos = np->next;
      this->node_ptr = np->next;                // this points to next of head
      this->reference(np->next);                
      return np;
    }
    // find previous node current position
    while (np != NULL && np->next != this->curpos) np = np->next;
    if (np == NULL) return NULL;                // error: curpos was not in list
    else this->prevpos = np;
  }
  np->next = cp->next;
  this->curpos = cp->next;
  this->reference(cp->next);
  return cp;
}


// remove() -- Removes the first occurence of the specified item in this CoolBase_List
// Input:      A refernce to data item to be removed.
// Output:     TRUE if item found and removed, FALSE otherwise.

Boolean CoolBase_List::remove(const void* x) {
  // find node for x
  if (this->do_find(this->node_ptr, x, this->curpos, this->prevpos)) {
    this->dereference(this->remove());          // throw node to trash, since 
    return TRUE;                                // it is not returned
  }
  return FALSE;
}


// replace() -- Replaces the first occurence of specified data item in THIS
//              CoolBase_List with a new value
// Input:       Two void* pointers.
// Output:      TRUE if item found and replaced, FALSE otherwise.

Boolean CoolBase_List::replace(const void* old_data, const void* new_data) {
  // find node of old_data in this CoolBase_List
  if(this->do_find(this->node_ptr, old_data, this->curpos, this->prevpos)) {
    // replace old_data in node with new_data
    this->curpos->set_data(new_data);
    return TRUE;
  }
  return FALSE;
}


// replace_all() -- Replaces all occurences of the specified data item in THIS
//                  CoolBase_List with a new value.
// Input:           Two void* pointers.
// Output:          TRUE if at least one item found and replaced, else FALSE

Boolean CoolBase_List::replace_all(const void* old_data, const void* new_data) {
  this->reset();        // make current position invalid
  Boolean success = FALSE; // return value
  CoolBase_List_Node* pp;
  CoolBase_List_Node* np = this->node_ptr;
  while (np != NULL && this->do_find(np, old_data, np, pp)) {
    np->set_data(new_data);
    success = TRUE;
    np = np->next;
  }
  return success;
}


// sort() -- Sorts the elements of THIS using the specified predicate function.
//           The predicate function returns TRUE if and only if the first
//           element preceeds the second. The sort routine uses the Sort
//           algorithm as given in "Numerical Recipes in C" p247.
// Input:    A predicate function pointer.
// Output:   None.

void CoolBase_List::sort(Predicate f) {
  this->reset();        // make current position invalid
  CoolBase_List_Node* np;
  int l, j, ir, i;
  int n = this->length();
  if (n < 2) return;    // No sense sorting if less than two elements
  CoolBase_List_Node** node_array = new CoolBase_List_Node*[n+1];
  // put the nodes of THIS into an array which will be used by the
  // heap sort algorithm
  for (np = this->node_ptr, i = 1; np != NULL; np = np->next, i++)
     node_array[i] = np;
  // the heap sort algorithm
  CoolBase_List_Node* temp;
  l = (n >> 1) + 1;
  ir = n;
  while (TRUE) {
    if (l > 1)
      temp = node_array[--l];
    else {
      temp = node_array[ir];
      node_array[ir] = node_array[1];
      if (--ir == 1) {
        node_array[1] = temp;
        break;
      }
    }
    i = l;
    j = i << 1;
    while (j <= ir) {
      if (j < ir && (*f) ((node_array[j])->get_data(),
                          (node_array[j+1])->get_data()) < 0)
        ++j;
      if ((*f) (temp->get_data(), (node_array[j])->get_data()) < 0) {
        node_array[i] = node_array[j];
        j += (i = j);
      }
      else
        j = ir + 1;
    }
    node_array[i] = temp;
  }

  // put the sorted nodes of the array back into THIS
  this->node_ptr = node_array[1];
  for (i = 1; i < n; i++) {
     (node_array[i])->next = node_array[i+1];
   }
  (node_array[n])->next = NULL;

  delete [] node_array;
}


// merge() -- Merges the elements of THIS CoolBase_List with the elements of the
//            specified CoolBase_List sorted with the specified predicate function.
//            If THIS is sorted already, then the result of the merge will be
//            sorted. Otherwise there are no guarantees for the sortedness of
//            the result.
// Input:     A reference to a CoolBase_List to be merged and a predicate function 
//            pointer.
// Output:    None.

void CoolBase_List::merge(const CoolBase_List& l, Predicate f) {
  this->reset();        // make current position invalid
  CoolBase_List_Node* tnode = this->node_ptr;
  if (tnode == NULL)
    // if this CoolBase_List is NULL, just assign this CoolBase_List to CoolBase_List l
    this->operator=(l);
  else {
    // begin merging CoolBase_List l into this CoolBase_List
    CoolBase_List_Node *lnode;

    // first merge nodes of CoolBase_List l in front of first node of this CoolBase_List
    // until a node in this CoolBase_List is less than a node in CoolBase_List l
    for (lnode = l.node_ptr;
         lnode != NULL &&
         (*f) (tnode->get_data(), lnode->get_data()) >= 0;
         lnode = lnode->next) {
      tnode = this->insert_before_node(lnode->get_data(), tnode);
      this->node_ptr = tnode;
    }
      
    // merge rest of nodes in CoolBase_List l into this CoolBase_List
    CoolBase_List_Node* tnode_prev = tnode;
    tnode = tnode->next;
    while (lnode != NULL) {
      if (tnode == NULL) {
        // if the end of THIS is reached, 
        // add the CoolBase_List l nodes to the end of this CoolBase_List
        for (; lnode !=NULL; lnode = lnode->next)
          tnode_prev = this->insert_after_node(lnode->get_data(), tnode_prev);
      }
      else if ((*f)(tnode->get_data(), lnode->get_data()) < 0) {
        // if node in this CoolBase_List is less than node in CoolBase_List l
        // just go to next node in this CoolBase_List
        tnode_prev = tnode;
        tnode = tnode->next;      
      } else {
        // if node in CoolBase_List l is less than node in this CoolBase_List
        // insert the CoolBase_List l node into THIS
        // and goto next node in CoolBase_List l
        tnode_prev->next = this->insert_before_node(lnode->get_data(), tnode);
        tnode_prev = tnode_prev->next;
        lnode = lnode->next;
      }
    }
  }
}


// insert_before() -- Inserts the specified item before the current position
// Input:             A void* pointer.
// Output:            TRUE if current position is valid, FALSE otherwise.

Boolean CoolBase_List::insert_before(const void* new_item) {
  CoolBase_List_Node* cp = this->curpos;
  CoolBase_List_Node *np = this->prevpos;
  if (cp == NULL)
    return FALSE;
  if (np == NULL || np->next != cp) {
    np = this->node_ptr;
    // current position is head node    
    if (np == cp)
      return this->push(new_item);
    // find previous node of current position in CoolBase_List
    while (np != NULL && np->next != cp) np = np->next;
    if (np == NULL)
      return FALSE;  // couldn't find current position
    else
      this->prevpos = np;
  }
  // insert new_item before current position and
  // set current position to this new node
  this->curpos = this->insert_before_node(new_item, cp);
  np->next = this->curpos;
  return TRUE;
}


// insert_after() -- Inserts the specified item after the current position
// Input:            A void* pointer.
// Output:           TRUE if current position is valid, FALSE otherwise.

Boolean CoolBase_List::insert_after(const void* new_item) {
  if (this->curpos == NULL)
    return FALSE;
  this->prevpos = this->curpos;
  this->curpos = this->insert_after_node(new_item,
                                                   this->curpos);
  return TRUE;
}


// insert_before() -- Inserts the specified new item before the specified
//                    target item in this CoolBase_List
// Input:             Two data void* pointers.
// Output:            TRUE if target item found, FALSE otherwise.

Boolean CoolBase_List::insert_before(const void* new_item, const void* target_item) {
  // find node with target item
  if (this->do_find(this->node_ptr, target_item, this->curpos, this->prevpos)){
    CoolBase_List_Node *np = this->curpos;
    CoolBase_List_Node *prev_np = this->prevpos;
    // insert new item before target item
    this->curpos = this->insert_before_node(new_item, np);
    if (prev_np == NULL)
      this->node_ptr = this->curpos;     // new item is the head node
    else
      prev_np->next = this->curpos; // new item is a  middle node
    return TRUE;
  }
  return FALSE;
}


// insert_after() -- Inserts the specified new item after the specified target
//                   item in this CoolBase_List
// Input:            Two data void* pointers.
// Output:           TRUE if target item found, FALSE otherwise.

Boolean CoolBase_List::insert_after(const void* new_item, const void* target_item) {
  // find node with target item
  CoolBase_List_Node *np;
  if (this->do_find(this->node_ptr, target_item, np, this->prevpos)) {
    this->curpos = this->insert_after_node(new_item, np);
    this->prevpos = np;
    return TRUE;
  }       
  return FALSE;
}


// value_error -- Raise exception for CoolBase_List::value() method
// Input:         Type string
// Output:        None

void CoolBase_List::value_error (const char* Type) {
  //RAISE Error, SYM(CoolBase_List), SYM(Invalid_Cpos),
  printf ("CoolList<%s>::value(): Invalid current position.\n", Type);
  abort ();
}


// get_error -- Raise exception for CoolBase_List::get() method
// Input:       Type string
// Output:      None

void CoolBase_List::get_error (const char* Type, int n) {
  //RAISE Error, SYM(CoolBase_List), SYM(Negative_Index),
  printf ("CoolList<%s>::get(): Negative index %d.\n", Type, n);
  abort ();
}


// before_error -- Raise exception for CoolBase_List::insert_before() method
// Input:          Type string
// Output:         None

void CoolBase_List::before_error (const char* Type) {
  //RAISE Error, SYM(CoolBase_List), SYM(Invalid_Cpos),
  printf ("CoolList<%s>::insert_before(): Invalid current position.\n", Type);
  abort ();
}


// after_error -- Raise exception for CoolBase_List::insert_after() method
// Input:         Type string
// Output:        None

void CoolBase_List::after_error (const char* Type) {
  //RAISE Error, SYM(CoolBase_List), SYM(Invalid_Cpos),
  printf ("CoolList<%s>::insert_after(): Invalid current position.\n", Type);
  abort ();
}


// bracket_error -- Raise exception for CoolBase_List::operator[]() method
// Input:           Type string
// Output:          None

void CoolBase_List::bracket_error (const char* Type, int n) {
  //RAISE Error, SYM(CoolBase_List), SYM(Negative_Index),
  printf ("CoolList<%s>::operator[](): Negative index %d.\n", Type, n);
  abort ();
}


// pop_error -- Raise exception for CoolBase_List::pop() method
// Input:       Type string
// Output:      None

void CoolBase_List::pop_error (const char* Type) {
  //RAISE Error, SYM(CoolBase_List), SYM(No_Elements),
  printf ("CoolList<%s>::pop(): No elements in CoolBase_List.\n", Type);
  abort ();
}


// remove_error -- Raise exception for CoolBase_List::remove() method
// Input:          Type string
// Output:         None

void CoolBase_List::remove_error (const char* Type) {
  //RAISE Error, SYM(CoolBase_List), SYM(Invalid_Cpos),
  printf ("CoolList<%s>::remove(): Invalid current position.\n", Type);
  abort ();
}

// va_arg_error -- Raise exception for using class objects, or chars in (...)
// Input:          Type string
// Output:         None

void CoolBase_List::va_arg_error (const char* Type, int n) {
  //RAISE Error, SYM(CoolBase_List), SYM(Invalid_Va_Arg),
  printf ("CoolList<%s>::CoolList<%s>(): Invalid type in ... or wrong alignment with %d bytes.\n",
          Type, Type, n);
  abort ();
}


