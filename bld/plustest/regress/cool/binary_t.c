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


#include <cool/Binary_Tree.h>

// CoolBinary_Tree -- Simple constructor to initialize a CoolBinary_Tree object
// Input:         None
// Output:        None

template <class Type>
CoolBinary_Tree<Type>::CoolBinary_Tree() {
  this->compare = &default_node_compare; // Pointer to compare function
}



// CoolBinary_Tree -- constructor to initialize a CoolBinary_Tree object to have the
//                same size and values as some other CoolBinary_Tree object
// Input:         Reference to CoolBinary_Tree
// Output:        None

template <class Type>
CoolBinary_Tree<Type>::CoolBinary_Tree(const CoolBinary_Tree<Type>& b)
{
  this->compare = b.compare;                    // Pointer to compare function
  this->number_nodes = b.number_nodes;
  this->root = this->copy_nodes (b.get_root());
  this->current_position () = b.current_position ();
}



// ~CoolBinary_Tree -- Destructor for the CoolBinary_Tree class
// Input:          None
// Output:         None

template <class Type>
CoolBinary_Tree<Type>::~CoolBinary_Tree() {
  delete this->root;                            // Virtual destructor called recurs.
}                                               // on all nodes 

// value -- Return value of node at current position
// Input:   None
// Output:  Reference to value of node at current position

template <class Type>
Type& CoolBinary_Tree<Type>::value () {
#if ERROR_CHECKING
 if (this->state.stack.is_empty() )             // If invalid current position
   this->curpos_error (#Type, "value()");       // Raise exception
#endif
  Stack_Entry stack_entry = this->state.stack.top(); // get top stack
  return (((CoolBinary_Node<Type>*)stack_entry.get_first())->get()); // Return value
}


template <class Type>
Boolean CoolBinary_Tree<Type>::put_internal
                            (const Type& value, Boolean avl) {

  if (this->root == NULL) {                     // If this is the first node
    this->root = new CoolBinary_Node<Type> (value);     // Add new node and value
    this->number_nodes++;                       // Update node count
    return TRUE;                                // Indicate success
  }
  CoolBinary_Node<Type>* ptr = (CoolBinary_Node<Type>*)this->root; // Start at root
  BT_Stack stack;                               // Stack for AVL balancing
  while (TRUE) {                                // Until we find location
    int pos = (*this->compare)(ptr->get(),value); // Compare data values
    if (pos == 0)                               // If data value exists in tree
      return FALSE;                             //    indicate node not added
    else if (pos > 0) {                         // Data down left subtree?
      if (avl)
        stack.push (Stack_Entry (/*(CoolBinary_Node*)##*/ptr,LEFT)); // Push parent
      if (ptr->get_ltree() == NULL) {           // If at leaf node
        ptr->set_ltree ((new CoolBinary_Node<Type> (value))); // Add node to ltree
        this->number_nodes++;                   // Update node count
        break;                                  // Break out of loop and exit
      }
      else {
        ptr = ptr->get_ltree();                 // Else point to left subtree
        continue;                               // And continue search
      }
    }
    else {                                      // Else down right subtree
      if (avl)
        stack.push (Stack_Entry (/*(CoolBinary_Node*)##*/ptr,RIGHT)); // Push on stack
      if (ptr->get_rtree() == NULL) {           // If at leaf node
        ptr->set_rtree ((new CoolBinary_Node<Type> (value))); // Add node to rtree
        this->number_nodes++;                   // Update node count
        break;                                  // Break out of loop and exit
      }
      else {
        ptr = ptr->get_rtree();                 // Grab right subtree dir
        continue;                               // And continue to search;
      }
    }
  }
  if (avl)                                      // Balance it if an AVL tree
    CoolBase_Binary_Tree::avl_put_balance (stack);

  this->reset();                                // Invalidate current position
  return TRUE;                                  // Return success
}


// remove_internal -- does the actual work of removing a value from bin tree
//                    for avl trees, will make call to check avl balance
// Input:          -- value to remove + optional Boolean for AVL trees
// output          -- TRUE if item sucessfully removed. FALSE otherwise.
template <class Type>
Boolean CoolBinary_Tree<Type>::remove_internal
                           (const Type& value, Boolean avl) {
  if (this->root == NULL)                       // If there are no nodes
    return FALSE;                               // indicate failure
  BT_Stack stack1;                              // Allocate traversal stack
  Left_Right route = NONE;                      // Last subtree taken
  CoolBinary_Node<Type> *ptr = (CoolBinary_Node<Type>*)this->root; // Start at root
  CoolBinary_Node<Type> *parent_ptr = NULL;             // Save pointer to parent
  CoolBinary_Node<Type> *ptr1, *ptr2;           // temp ptrs to nodes
  while (TRUE) {                                // Until we find location
    int pos = (*this->compare)(ptr->get(),value); // Compare data values
    if (pos == 0) {                             // If node to delete is found
      if (ptr->get_rtree() == NULL) {           // If no right subtree
        if (route == LEFT)                      // If child of parent's ltree
          parent_ptr->set_ltree(ptr->get_ltree()); // Set to left subtree
        else if (route == RIGHT)
          parent_ptr->set_rtree(ptr->get_ltree()); // Set to right subtree
        else this->root = ptr->get_ltree();     // Make ltree the new root
      }
      else if (ptr->get_ltree() == NULL) {      // Else if no left subtree
        if (route == LEFT)                      // If child of parent ltree
          parent_ptr->set_ltree(ptr->get_rtree()); // Set to left subtree
        else if (route == RIGHT)
          parent_ptr->set_rtree(ptr->get_rtree()); // Set to right subtree
        else this->root = ptr->get_rtree();        //    rtree is the new root.
      }
  
      // Node(a) with value to be deleted has both a left and right subtree.
      // We need to look for the node(b) with the next smallest value and
      // copy it's value into node(a). Then adjust the parent of node(b) 
      // to point at node(b)'s left subtree (right subtree will always be 
      // NULL).  ptr is left pointing at node(b) so it is the
      // one which is eventually deleted.
      else {
        if (avl)                                // for avl trees
          stack1.push (Stack_Entry(/*(CoolBinary_Node*)##*/ptr,LEFT)); // push on stack
        ptr1 = ptr;                             // Save node with matched data
        ptr2 = ptr1;                            // last parent initially ptr
        ptr = ptr->get_ltree();                 // Start with node's ltree
        while (ptr->get_rtree() != NULL) {      // Follow rtree til null rtree
          if (avl)                              // for avl trees
            stack1.push (Stack_Entry(/*(CoolBinary_Node*)##*/ptr,RIGHT)); // push on stack
          ptr2 = ptr;                           // save last parent
          ptr = ptr->get_rtree();               // get right subtree
        }
        ptr1->set (ptr->get());                 // Move next smallest value up
        if (ptr1 == ptr2)                       // ltree had no rtrees
          ptr2->set_ltree (ptr->get_ltree());   // Del node is parents ltree
        else 
          ptr2->set_rtree (ptr->get_ltree());   // Del node is parents rtree
      }

      // DELETE the node ptr points at
      this->number_nodes--;                     // Decrement node count
      if (this->number_nodes == 0)              // If no more nodes in tree
        this->root = NULL;                      // Nullify root pointer
      ptr->set_ltree (NULL);                    // Nullify left subtree pointer
      ptr->set_rtree (NULL);                    // Nullify right subtree pointer
      delete ptr;                               // Deallocate memory
      this->reset();                            // Invalidate current position<
      break;                                    // exit loop
    }
    else if (pos > 0) {                         // Data down left subtree?
      if (ptr->get_ltree() == NULL) {           // If at leaf node
        return FALSE;                           // Indicate node not found
      }
      else {
        if (avl)                                // if avl tree
          stack1.push (Stack_Entry(/*(CoolBinary_Node*)##*/ptr,LEFT)); // push on stack1
        parent_ptr = ptr;                       // Save parent pointer
        route = LEFT;                           // Save route taken
        ptr = ptr->get_ltree();                 // point to left subtree
        continue;                               // And continue to search;
      }
    }
    else {                                      // Else down right subtree
      if (ptr->get_rtree() == NULL)             // If at leaf node
        return FALSE;                           // Indicate success
      else {
        if (avl)                                // If an avl tree
          stack1.push (Stack_Entry(/*(CoolBinary_Node*)##*/ptr,RIGHT)); // push on stack
        parent_ptr = ptr;                       // Save parent pointer
        route = RIGHT;                          // Save route taken
        ptr = ptr->get_rtree();                 // Point to right subtree
        continue;                               // And continue to search;
      }
    }
  }
  // The node has been removed.  Now every node in the path of the deleted
  // node must be checked for possible re-balancing if this is an AVL tree
  if (avl)
    CoolBase_Binary_Tree::avl_remove_balance (stack1);
  return TRUE;
}



      

// find -- Find a value in the sorted binary tree 
// Input:  Reference to value to find
// Output: Current position updated and TRUE if item found, FALSE otherwise

template <class Type>
Boolean CoolBinary_Tree<Type>::find (const Type& value) {
  for (this->reset (); this->next (); )         // For each node in the tree
    if ((*this->compare)(this->value(), value) == 0) // If value found in cache
      return TRUE;                              // Indicate item found
  return FALSE;                                 // Else indicate failure
}


// operator<< -- Output a binary tree by printing it sideways where the root is
//               printed at the left margin. To obtain the normal orientation,
//               rotate the output 90 degrees clockwise
// Input:        Reference to output stream, reference to CoolBinary_Tree<Type>
// Output:       Reference to output stream

template <class Type>
ostream& operator<< (ostream& os, const CoolBinary_Tree<Type>& b) {
  print_tree((CoolBinary_Node<Type>*)b.get_root(),os); // Print tree 
  return os;                                     // Return output stream
}



template <class Type>
void print_tree (const CoolBinary_Node<Type>* b, ostream& os) {
  static int indent = 0;                        // Temporary variables
  if (b != NULL) {                              // If not at bottom of tree
    indent += 1;                                // Indent node
    print_tree (b->get_rtree(),os);             // Output the right subtree
    for (int i = 1; i < indent; i++)            // For each level of tree
      os << "     ";                            // Indent some number of spaces
      os << b->get() << "\n";                   // Output node data value
    print_tree (b->get_ltree(),os);             // Output the left subtree
    indent -= 1;                                // Decrement indent level
  }
}


// operator= -- Overload the assignment operator to copy a CoolBinary_Tree object
//              to another Binary Tree object
// Input:       Reference to CoolBinary_Tree
// Output:      Reference to CoolBinary_Tree

template <class Type>
CoolBinary_Tree<Type>& CoolBinary_Tree<Type>::operator= (CoolBinary_Tree<Type>& b) {
  delete this->root;                            // Delete old tree nodes, virtual
  this->compare = b.compare;                    // Pointer to compare function
  this->number_nodes = b.number_nodes;
  this->current_position () = b.current_position ();
  this->root = this->copy_nodes (b.get_root());
  return *this;                                 // Return tree reference
}


template <class Type>
inline CoolBinary_Node<Type>* CoolBinary_Tree<Type>::copy_nodes(const CoolBinary_Node<Type>* bn) const{
  CoolBinary_Node<Type>* new_nodes = NULL;
  if (bn)
    new_nodes = bn->copy_nodes(bn);
  return new_nodes;
}


// balance -- Build a perfectly balanced binary tree from the existing tree
//            and delete old tree and storage. This uses the private recursive
//            method baltree() to construct the left and then right subtrees.
// Input:     None
// Output:    None

template <class Type>
void CoolBinary_Tree<Type>::balance () {
  CoolBinary_Node<Type>* p;                             // Temporary node pointer
  if (this->number_nodes != 0) {                // If there are nodes in tree
    this->reset ();                             // Recalculate node cache
    p = this->baltree (this->number_nodes);     // Generate balanced tree
    delete this->root;                          // Delete old tree, virtual
    this->root = p;                             // Point to new balanced tree
  }
}

template <class Type>
CoolBinary_Node<Type>* CoolBinary_Tree<Type>::baltree (long n) {
  long nleft, nright;                           // Number nodes in ltree,rtree
  CoolBinary_Node<Type>* p;                             // Temporary pointer
  if (n == 0)                                   // If no more nodes left
    return NULL;                                // Return NULL pointer
  nleft = n >> 1;                               // Node count for left subtree
  nright = n - nleft - 1;                       // Node count for right subtree
  p = new CoolBinary_Node<Type> ();                     // Allocate new node
  p->set_ltree(this->baltree (nleft));          // Create left subtree
  p->set ((this->next(), this->value()));       // Set node value
  p->set_rtree(this->baltree (nright));         // Create right subtree
  return p;                                     // Return pointer to subtree
}


// operator== -- Compare binary trees for same values and structure
// Input:        constant reference to another binary tree
// Output:       TRUE/FALSE

template <class Type>
Boolean CoolBinary_Tree<Type>::operator== (CoolBinary_Tree<Type>& t) {
  Boolean t1, t2;
  for (this->reset(), t.reset();                // Start at first node of each
       (t1 = this->next()) && (t2 = t.next());) // For each node in tree
    if ((*this->compare)(this->value(), t.value()) != 0) // If different value
      return FALSE;                                      // Trees not equal
  if (t1 == FALSE)                                       // If no more nodes 
    if (t.next () == FALSE)                              // in either tree
      return TRUE;                                       // Trees are equal
    else
      return FALSE;
  return FALSE;                                          // Else not equal
}

// set_compare -- Specify the comparison function to be used in logical tests
//                of node data values
// Input:         Pointer to a compare function
// Output:        None

template <class Type>
void CoolBinary_Tree<Type>::set_compare (register /*Compare##*/int (*cf)(const Type&, const Type&)) {
  if (cf)                                       // If compare function given
    this->compare = cf;                         // Set pointer
  else
    this->compare = &default_node_compare; // Pointer to default compare
}

// default_node_compare -- Default node comparison function utilizing builtin
//                         less than, equal, and greater than operators
// Input:                  Reference to two Type data values
// Output:                 -1, 0, or 1 if less than, equal to, or greater than

template <class Type>
int default_node_compare (const Type& v1, const Type& v2) {
    if (v1 == v2)                               // If data items equal
      return 0;                                 // Return zero
    if (v1 < v2)                                // If this less than data
      return -1;                                // Return negative one
    return 1;                                   // Else return positive one
}

