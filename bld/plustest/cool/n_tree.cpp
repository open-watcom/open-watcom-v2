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

#include <cool/N_Tree.h>
#include <cool/Stack.C>     // stack.pop template needed

// CoolN_Tree -- Simple constructor that sets the root to the node provided
// Input:    Pointer to node object or NULL.
// Output:   CoolN_Tree object created with root initialized

template <class Node> 
CoolN_Tree<Node>::CoolN_Tree(Node* n) {
  this->root = n;                               // Point root to node
  this->t_mode = INORDER;                       // Default traversal mode
  if (n)
    this->number_nodes = 1;                     // Update node count
  else
    this->number_nodes = 0;
}

// CoolN_Tree -- Simple constructor that sets the root to the node provided
// Input:    Reference to node object
// Output:   CoolN_Tree object created with root initialized

template <class Node> 
CoolN_Tree<Node>::CoolN_Tree(Node& n) {
  this->root = &n;                              // Point root to node
  this->t_mode = INORDER;                       // Default traversal mode
  this->number_nodes = 1;                       // Update node count
}


// CoolN_Tree -- Copy constructor makes deep copy of all nodes
// Input:    Reference to CoolN_Tree object
// Output:   CoolN_Tree object duplicated

template <class Node> 
CoolN_Tree<Node>::CoolN_Tree(const CoolN_Tree<Node>& nt) 
{
  this->root = copy_nodes(nt.root);             // Deep copy of all nodes
  this->t_mode = nt.t_mode;                     // Copy traversal mode
  this->number_nodes = nt.number_nodes;         // Copy node count
  this->state = nt.state;                       // Copy state or curpos
}


// ~CoolN_Tree -- destructor (not inline because it's virtual)
// Input:    none
// Output:   none

template <class Node>
CoolN_Tree<Node>::~CoolN_Tree() {
  delete this->root;                            // Delete all nodes
}

// clear -- removes root and all subtrees
// input -- none
// output -- none

template <class Node> 
void CoolN_Tree<Node>::clear () {
  delete this->root;                            // Delete all nodes
  this->root = NULL;
  this->number_nodes = 0;
  this->reset();
}



// prev -- Move position to previous node in tree. If no more nodes
//         return FALSE 
// Input:  None
// Output: TRUE/FALSE

template <class Node> 
Boolean CoolN_Tree<Node>::prev() {
  Traversal_Type reverse_mode;
  switch (this->t_mode) {
  case INORDER:
    reverse_mode = INORDER_REVERSE;
    break;
  case INORDER_REVERSE:
    reverse_mode = INORDER;
    break;
  case PREORDER:
    reverse_mode = PREORDER_REVERSE;
    break;
  case PREORDER_REVERSE:
    reverse_mode = PREORDER;
    break;
  case POSTORDER:
    reverse_mode = POSTORDER_REVERSE;
    break;
  case POSTORDER_REVERSE:
    reverse_mode = POSTORDER;
    break;
  }
  return this->next_internal (reverse_mode);
}
  
// Get the next node in the tree based on the Traversal Type.  This
// maintains a stack of parents in the tree for current position and
// knows how to move forward and backward for preorder, inorder, or
// postorder traversals. Changes here are most likely necessary in
// Base_BT.C next_internal also.

template <class Node>
Boolean CoolN_Tree<Node>::next_internal (Traversal_Type ttype) {
  Node* node;
  Node* ptr1;
  Node* last_node = NULL;
  CoolNT_Stack_Entry stack_entry;
  int index;
  Boolean forward = TRUE;

  switch (ttype) {
  case INORDER_REVERSE:
  case PREORDER_REVERSE:                        // Are we going backward?
  case POSTORDER_REVERSE:
    forward = FALSE;
    break;
  }
    
  if (state.stack.is_empty()) {                 // If stack is empty
    node = this->root;                          //  start with the root
    if (forward)                                //  init starting subtree
      index = -1;                               //    start at first subtree
    else                                        //    or
      index = node->num_subtrees();             //    start at last subtree
    state.stack.push (CoolNT_Stack_Entry ((long)node,index));
    state.forward = forward;
    
  }
  else {                                        // Stack has some entries, so
    stack_entry = state.stack.top();            //  get top entry from stack
    node = (Node*)stack_entry.get_first();    //  load up node
    index = (int)stack_entry.get_second();      //  and subtree index
    last_node = node;                           //  remember current position

    if (state.forward != forward) {             // Need to modify index
      if (forward)                              //  if we've changed direction.
        index--;
      else
        index++;
    }
    state.forward = forward;                    // Update direction.
  }

  if (forward) {                                // Going left to right
    while (TRUE) {                              // loop until next node found
      if (++index < node->num_subtrees()) {     // incremented index in range?
        if (node != last_node &&                // If we moved to new node &&
            ((ttype == INORDER  && index == 1)  // Inorder after ltree or
             ||(ttype == PREORDER && index == 0)// Preorder before ltree
             ))
          return TRUE;                          // then this is next node

        state.stack.top().set_second(index);    // update stack with new index
        ptr1 = node->sub_trees[index];          // get node's next subtree
        if (ptr1) {                             // When subtree exists
          node = ptr1;                          //   point node at subtree
          index = -1;                           //   init index for new node
          state.stack.push (CoolNT_Stack_Entry ((long)node, index)); 
        }
      }
      else {                                    // No more subtrees for node
        if (node != last_node &&                // If a new node
            ttype == POSTORDER) {               //   and Postorder mode, 
          return TRUE;                          //   then this is next node
        }
        state.stack.pop();// pop this node from stack
        if (state.stack.is_empty())            // Stack empty?
          return FALSE;                         //   indicate we're at the end
        else {                                  // Stack not empty, so
          stack_entry = state.stack.top();      //  update stack_entry object
          node = (Node*)stack_entry.get_first(); //  load up node
          index = stack_entry.get_second();     //  and subtree index
        }
      }
    }
  }

// This is essesentially the same code as above, but going right to
// left, giving reverse order capability
  else {                                        // Going right to left
    while (TRUE) {                              // loop until next node found
      if (--index > -1) {                       // decremented index in range?
        if (node != last_node &&                // If we moved to new node &&
            ((ttype == INORDER_REVERSE &&       //  or Inorder_reverse node
              index == 0)                       //  is ready to do left subtree
             || (ttype == POSTORDER_REVERSE &&  //  or Postorder_reverse is
              index == (node->num_subtrees()-1))  // starting it's subtrees
             ))
          return TRUE;                          // then this is next node
        state.stack.top().set_second(index);    // update stack with new index
        ptr1 = node->sub_trees[index];          // get node's next subtree
        if (ptr1) {                             // When subtree exists
          node = ptr1;                          //   point node at subtree
          index = node->num_subtrees();         //   init index for new node
          state.stack.push (CoolNT_Stack_Entry ((long)node, index)); 
        }
      }
      else {                                    // No more subtrees for node
        if (node != last_node &&                // If a new node
        ttype == PREORDER_REVERSE)              //   and Preorder_reverse
          return TRUE;                          //   this is next node

        state.stack.pop();                      // pop this node from stack
        if (state.stack.is_empty())             // Stack empty?
          return FALSE;                         //   indicate we're at the end
        else {                                  // Stack not empty, so
          stack_entry = state.stack.top();      //  update stack_entry object
          node = (Node*)stack_entry.get_first(); //  load up node
          index = (int)stack_entry.get_second();        //  and subtree index
        }
      }
    }
  }
}


#if 0  //## BC++ 3.1 has problems with nested typedefs

// value -- Return value of node at current position
// Input: None
// Output: Reference to value of node at current position

template <class Node> 
Node::ItemType& CoolN_Tree<Node>::value () { 
#if ERROR_CHECKING 
  if (this->state.stack.is_empty() )            // If no position established
    this->value_error ();                       // Raise exception
#endif
  CoolNT_Stack_Entry stack_entry = this->state.stack.top();
  return (((Node*)stack_entry.get_first())->get());
}

// find -- Search the tree for a particular value. If found, update the current
//         position marker.
// Input:  Reference of value to search for
// Output: TRUE/FALSE

template <class Node> 
Boolean CoolN_Tree<Node>::find (const Node::ItemType& value) {
  for (this->reset (); this->next (); )         // For each node in tree
    if (this->value() == value)                 // If node found in tree
      return TRUE;                              // Inidicate success
  return FALSE;                                 // Inidicate failure
}


// preorder -- Perform a preorder traversal of tree by setting traversal mode,
//             making node pointer cache, and applying function to each node
// Input:      Pointer to function to apply to each node
// Output:     None

template <class Node> 
void CoolN_Tree<Node>::preorder (/*Apply_Function##*/Boolean (*fn)(const Node::ItemType&)) {

  if (this->t_mode != PREORDER)                 // If incorrect traversal mode
    this->t_mode = PREORDER;                    // Set preorder mode

  for (this->reset() ; this->next (); )         // For each preorder node
    (*fn)(this->value());                       // Apply function
}


// inorder -- Perform an inorder traversal of tree by setting traversal mode,
//            making node pointer cache, and applying function to each node
// Input:     Pointer to function to apply to each node
// Output:    None

template <class Node> 
void CoolN_Tree<Node>::inorder (/*Apply_Function##*/Boolean (*fn)(const Node::ItemType&)) {
  if (this->t_mode != INORDER)                  // If incorrect traversal mode
    this->t_mode = INORDER;                     // Set inorder mode

  for (this->reset() ; this->next (); )         // For each preorder node
    (*fn)(this->value());                       // Apply function
}


// postorder -- Perform a postorder traversal of tree by setting traversal mode,
//              making node pointer cache, and applying function to each node
// Input:       Pointer to function to apply to each node
// Output:      None

template <class Node> 
void CoolN_Tree<Node>::postorder (/*Apply_Function##*/Boolean (*fn)(const Node::ItemType&)) {
  if (this->t_mode != POSTORDER)                // If incorrect traversal mode
    this->t_mode = POSTORDER;                   // Set postorder mode

  for (this->reset() ; this->next (); )         // For each preorder node
    (*fn)(this->value());                       // Apply function
}
#endif

// current_position -- Get/Set iterator object for Tree
// Input:              None
// Output:             NT_State object of the Tree

template <class Node>
CoolNT_State& CoolN_Tree<Node>::current_position () {
  return this->state;
}


// do_count -- Perform an preorder traversal of N-ary tree to count nodes
// Input:       Pointer to sub-tree node
// Output:       None

template <class Node>
void CoolN_Tree<Node>::do_count (Node* t) {
  if (t != NULL) {                              // If there is a subtree
    this->number_nodes++;                       // Increment node count
    for (int i = 0; i < Node::max_children(); i++)              // For each pointer in vector
      this->do_count (t->sub_trees[i]);         // Traverse each subtree
  }
}

// value_error -- Raise exception for CoolN_Tree::value()
// Input:         None
// Output:        None

template <class Node> 
void CoolN_Tree<Node>::value_error () {
  //RAISE Error, SYM(CoolN_Tree), SYM(Invalid_Cpos),
  printf ("CoolN_Tree<%s,%s,%d>::value(): Invalid current position.\n",
          "Node::ItemType", "Node", Node::max_children());
  abort ();
}
