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
// Created: MBN 07/19/89 -- Initial design and implementation
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version
//
// The Binary_Tree class implements the type-generic  structural methods of the
// parameterized Binary_Tree<Type>  class  and is  a friend of  the Binary Node
// class.  The  Binary_Tree   class  is  intended for  the    sole  use  of the
// parameterized  Binary_Tree<Type>    class.    The    Binary_Tree<Type> class
// implements simple,  dynamic,   sorted  sequences.  Users who  require a data
// structure  for unsorted  sequences  whose structure and organization is more
// under the control of the programmer are refered to the N_Tree class.
//

#include <cool/Base_Binary_Tree.h>

#include <cool/Pair.C>
#include <cool/Stack.C>

#define IGNORE(arg) (void) arg

// CoolBase_Binary_Tree -- Simple constructor to initialize a CoolBase_Binary_Tree object
// Input:              None
// Output:             None

CoolBase_Binary_Tree::CoolBase_Binary_Tree () {
  this->root = NULL;                            // Initialize root pointer
  this->number_nodes = 0;                       // Initialize node count
  this->state.forward = TRUE;                   // Assume Forward direction;
}


// ~CoolBase_Binary_Tree -- destructor must be virtual to delete both state and data.
// Input:              None
// Output:             None

CoolBase_Binary_Tree::~CoolBase_Binary_Tree () {}               // state is deleted automatically


// clear -- removes root and all subtrees
// input -- none
// output -- none

void CoolBase_Binary_Tree::clear () {
  delete root;                                  // virtually delete all nodes
  this->root = NULL;
  this->number_nodes = 0;
  this->reset();                                // reset state of iterator
}

  

// calc_depth (recurse thru the Tree and return zero-based depth)
//   if update_bal is not NULL, then the avl balance for this node is
//   updated.  This avl update should be called after balancing the tree
//   as an AVL tree cannot have it's balance othere than -1, 0 1.

long CoolBase_Binary_Tree::calc_depth
                (CoolBase_Binary_Node* node, long depth, Boolean update_bal) {
  if (node == NULL) {                           // Null nodes don't count
    if (depth > 0)                              // If not the root
      --depth;                                  //   depth bumped 1 too many
    return depth;
  }
  depth++;                                      // Do left and right subtrees
  long ldepth = this->calc_depth (node->ltree, depth, update_bal);
  long rdepth = this->calc_depth (node->rtree, depth, update_bal);

  // It is an error if rdepth - ldepth is other than -1 0 or 1.
  if (update_bal)                               // If we need to update balance
    node->avl_balance =(int)(rdepth - ldepth);  // it's right tree depth - left

  if (ldepth > rdepth)                          // Return count of deepest tree
    return ldepth;
  else
    return rdepth;
}

// current-node -- Returns node at the current position

CoolBase_Binary_Node* CoolBase_Binary_Tree::node () {
  if (this->state.stack.is_empty())
    return NULL;
  else {
    Stack_Entry se = this->state.stack.top();
    return se.get_first();
  }
}

// Get the next node in the tree based on the Traversal Type.  This
// maintains a stack of parents in the tree for current position and
// knows how to move forward and backward for preorder, inorder, or
// postorder traversals.  Binary trees only use inorder and inorder_reverse
// so the code for dealing with the other traversal types is commented out
// (almost identical copy of this is in N_Tree)

Boolean CoolBase_Binary_Tree::next_internal (Traversal_Type ttype) {
  CoolBase_Binary_Node *node, *ptr1;
  CoolBase_Binary_Node *last_node = NULL;
  Stack_Entry stack_entry;
  int index;
  Boolean forward = TRUE;

  switch (ttype) {
  case INORDER_REVERSE:
//case PREORDER_REVERSE:                        // Are we going backward?
//case POSTORDER_REVERSE:
    forward = FALSE;
    break;
  }
    
  if (state.stack.is_empty()) {         // If stack is empty
    node = this->root;                          //  start with the root
    if (forward)                                //  init starting subtree
      index = -1;                               //    start at first subtree
    else                                        //    or
      index = node->num_subtrees();             //    start at last subtree
    state.stack.push (Stack_Entry (node,index));
    state.forward = forward;
    
  }
  else {                                        // Stack has some entries, so
    stack_entry = state.stack.top();            //  get top entry from stack
    node = stack_entry.get_first();             //  load up node
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
//           ||(ttype == PREORDER && index == 0)// Preorder before ltree
             ))
          return TRUE;                          // then this is next node

        state.stack.top().set_second(index);    // update stack with new index
        ptr1 = node->subtree(index);            // get node's next subtree
        if (ptr1) {                             // When subtree exists
          node = ptr1;                          //   point node at subtree
          index = -1;                           //   init index for new node
          state.stack.push (Stack_Entry (node, index)); 
        }
      }
      else {                                    // No more subtrees for node
//      if (node != last_node &&                // If a new node
//          ttype == POSTORDER) {               //   and Postorder mode, 
//        return TRUE;                          //   then this is next node
//      }
        state.stack.pop();// pop this node from stack
        if (state.stack.is_empty())            // Stack empty?
          return FALSE;                         //   indicate we're at the end
        else {                                  // Stack not empty, so
          stack_entry = state.stack.top();      //  update stack_entry object
          node = stack_entry.get_first(); //  load up node
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
//           || (ttype == POSTORDER_REVERSE &&  //  or Postorder_reverse is
//            index == (node->num_subtrees()-1))  // starting it's subtrees
             ))
          return TRUE;                          // then this is next node
        state.stack.top().set_second(index);    // update stack with new index
        ptr1 = node->subtree(index);            // get node's next subtree
        if (ptr1) {                             // When subtree exists
          node = ptr1;                          //   point node at subtree
          index = node->num_subtrees();         //   init index for new node
          state.stack.push (Stack_Entry (node, index)); 
        }
      }
      else {                                    // No more subtrees for node
//      if (node != last_node &&                // If a new node
//        ttype == PREORDER_REVERSE)            //   and Preorder_reverse
//        return TRUE;                          //   this is next node

        state.stack.pop();                      // pop this node from stack
        if (state.stack.is_empty())             // Stack empty?
          return FALSE;                         //   indicate we're at the end
        else {                                  // Stack not empty, so
          stack_entry = state.stack.top();      //  update stack_entry object
          node = stack_entry.get_first();       //  load up node
          index = (int)stack_entry.get_second();        //  and subtree index
        }
      }
    }
  }
}




  // maintain the balance of an AVL tree after a put.  The balance of
  // depths between the right subtree and left subtree are maintained
  // for each node. When this balance differs by more than 1, a single
  // or double rotation is done depending on the constellation to put
  // the balance of that node back to zero.  A single or double
  // rotation on the parent of the inserted node does the job.  Nodes
  // above the parent are not affected.

void CoolBase_Binary_Tree::avl_put_balance (BT_Stack &stack) {
  CoolBase_Binary_Node *p1, *p2, *raised_node=NULL;     // alloc tmp ptrs to Node
  CoolBase_Binary_Node *ptr;
  Left_Right route=NONE;
  Boolean more_todo = TRUE;
  while (stack.length() > 0) {                  // Loop thru nodes on stack
    Stack_Entry stack_entry = stack.pop();      // Pop next stack_entry
    ptr = stack_entry.get_first();              // Get Node 
    route = (Left_Right)stack_entry.get_second();// Get the subtree
    if (raised_node != NULL) {                  // A node was rotated up
      if (route == LEFT)                        // For a left route
        ptr->ltree = raised_node;               //  change ltree to raised node
      else if (route == RIGHT)                  // or for right route
        ptr->rtree = raised_node;               //  change rtree to raised node
      raised_node = NULL;
    }
    if (more_todo == FALSE)                     // Exit loop when all balanced
      break;
    
    if (route == LEFT) {                        // Node inserted as an LTREE
      switch (ptr->avl_balance) {               // Depending on node's balance
      case 1:
        ptr->avl_balance = 0;                   // Set node as balanced
        more_todo = FALSE;
        break;
      case 0:
        ptr->avl_balance = -1;                  // Set node as 1 heavy on left
        break;
      case -1:                                  // Will need to rotate
        more_todo = FALSE;
        p1 = ptr->ltree;                        
        if (p1->avl_balance == -1) {            // Single Right Rotation
          ptr->ltree = p1->rtree;
          p1->rtree = ptr;
          p1->avl_balance = 0;
          ptr->avl_balance = 0;
          raised_node = p1;                     // This node bubbled up
        }                                       // End Single right rotation
        else {                                  // Double Right Rotation
          p2 = p1->rtree;
          p1->rtree = p2->ltree;
          p2->ltree = p1;
          ptr->ltree = p2->rtree;
          p2->rtree = ptr;
          if (p2->avl_balance == -1)            // update balance for ptr
            ptr->avl_balance = 1;
          else
            ptr->avl_balance = 0;
          if (p2->avl_balance == 1)             // update balance for p1
            p1->avl_balance = -1;
          else
            p1->avl_balance = 0;
          p2->avl_balance = 0;                  // update balance for p2
          raised_node = p2;                     // This node now moved up
          break;
        }                                       // End Left Right Rotation
      }                                         // End switch
    }                                           // End LTREE path
    
    else {                                      // Node inserted as an RTREE
      switch (ptr->avl_balance) {
      case -1:
        ptr->avl_balance = 0;                   // Node is now balanced
        more_todo = FALSE;                      // Tree is now balanced
        break;
      case 0:
        ptr->avl_balance = 1;                   // Node is 1 heavy on right
        break;
      case 1:                                   // Will need to rotate
        more_todo = FALSE;
        p1 = ptr->rtree;
        if (p1->avl_balance == 1) {             // Single Left Rotation
          ptr->rtree = p1->ltree;
          p1->ltree = ptr;
          p1->avl_balance = 0;
          ptr->avl_balance = 0;
          raised_node = p1;
        }                                       // End Single Left Rotation
        else {                                  // Right/Left Rotation
          p2 = p1->ltree;
          p1->ltree = p2->rtree;
          p2->rtree = p1;
          ptr->rtree = p2->ltree;
          p2->ltree = ptr;
          if (p2->avl_balance == 1)             // Update balance for ptr
            ptr->avl_balance = -1;
          else
            ptr->avl_balance = 0;
          if (p2->avl_balance == -1)            // Update balance for p1
            p1->avl_balance = 1;
          else
            p1->avl_balance = 0;
          p2->avl_balance = 0;                  // update balance for p2
          raised_node = p2;                     // This node now moved up
          break;
        }                                       // End Right/Left Rotation
      }                                         // End switch
    }                                           // End RTREE path
  }

  if (raised_node != NULL) {                    // When a node gets raised
    this->root = raised_node;                   //  raised_node to be root
  }
}

// Balances the nodes in the path of a removed node for an AVL Binary Tree
// The parent nodes and index to their subtrees are maintained in a Stack of 
// Stack_entrys
 
void CoolBase_Binary_Tree::avl_remove_balance (BT_Stack &stack) {
  CoolBase_Binary_Node *p, *p1, *p2, *raised_node=NULL; // Temporary pointers to nodes
  Boolean more_todo=TRUE;                       // When more_todo is FALSE, done.
  Left_Right route=NONE;
  while (stack.length() > 0) {                  // Loop thru nodes on stack
    Stack_Entry stack_entry = stack.pop();      // Get next stack_entry
    p = stack_entry.get_first();                // Get Node 
    route = (Left_Right)stack_entry.get_second();// Get the subtree

    // UPDATE PARENT TO POINT AT RAISED NODE FROM PREVIOUS ITERATION
    if (raised_node != NULL) {                  // If there is a raised node
      if (route == LEFT)                        //    See what side was changed
          p->ltree = raised_node;               //    Update ltree
      else
        p->rtree = raised_node;                 //    or update rtree
      raised_node = NULL;                       //    raised_node taken care of.
    }

    if (more_todo == FALSE)                     // Tree is balanced
      break;

    // BALANCE LEFT SUBTREE OF NODE P.  Note that this is VERY similar 
    // to the code below to balance the right side.  Changes here are likely
    // necessary below also.

    if (route == LEFT) {
      switch (p->avl_balance) {
      case -1:                                  // Set balance to 0. 
        p->avl_balance = 0;                     //   balance parent node
        break;
      case 0:                                   // Set balance to 1.  Tree is 
        p->avl_balance = 1;                     //   all balanced.
        more_todo = FALSE;
        break;
      case 1:                                   // Need to rotate
        p1 = p->rtree;
        if (p1->avl_balance >= 0) {             
          p->rtree = p1->ltree;                 // Single left rotate
          p1->ltree = p;
          raised_node = p1;
          if (p1->avl_balance == 0) {           // Update p and p1 balance
            p->avl_balance = 1;
            p1->avl_balance = -1;
            more_todo = FALSE;
          }
          else {
            p->avl_balance = 0;
            p1->avl_balance = 0;
          }
        }
        else {                                  // Double right left rotate
          p2 = p1->ltree;
          p1->ltree = p2->rtree;
          p2->rtree = p1;
          p->rtree = p2->ltree;
          p2->ltree = p;
          int b2 = p2->avl_balance;
          if (b2 == 1)                          // Set new balance for node p
            p->avl_balance = -1;
          else
            p->avl_balance = 0;
          if (b2 == -1)                         // Set new balance for node p1
            p1->avl_balance = 1;
          else 
            p1->avl_balance = 0;

          p2->avl_balance = 0;                  // Set new balance for node p2
          raised_node = p2;             
        }
      }
    }
    // BALANCE RIGHT SUBTREE OF NODE P.  Note that this is VERY similar
    // to the code above to balance the left side.  Changes here are likely
    // necessary above also.
    else {
      switch (p->avl_balance) {
      case 1:                                   // Set balance to 0. 
        p->avl_balance = 0;                     //   balance parent node
        break;
      case 0:                                   // Set balance to 1.  Tree is 
        p->avl_balance = -1;                    //   all balanced.
        more_todo = FALSE;
        break;
      case -1:                                  // Need to rotate
        p1 = p->ltree;
        if (p1->avl_balance <= 0) {             // Single right rotate
          p->ltree = p1->rtree;
          p1->rtree = p;
          raised_node = p1;
          if (p1->avl_balance == 0) {           // Update p and p1 balance
            p->avl_balance = -1;
            p1->avl_balance = 1;
            more_todo = FALSE;                  
          }
          else {
            p->avl_balance = 0;
            p1->avl_balance = 0;
          }
        }
        else {                                  // Double left right rotate
          p2 = p1->rtree;
          p1->rtree = p2->ltree;
          p2->ltree = p1;
          p->ltree = p2->rtree;
          p2->rtree = p;
          int b2 = p2->avl_balance;
          if (b2 == -1)                         // Set new balance for node p
            p->avl_balance = 1;
          else
            p->avl_balance = 0;
          if (b2 == 1)                          // Set new balance for node p1
            p1->avl_balance = -1;
          else
            p1->avl_balance = 0;

          p2->avl_balance = 0;                  // Set new balance for node p2
          raised_node = p2;
        }
      }
    }
  }

  // If raised_node is non_null, after all the balancing is done, it means
  // we have a new root. 
  if (raised_node != NULL) {
    this->root = raised_node;
  }
}
                          

// curpos_error -- Raise exception for invalid current position
// Input:         Character string of function and type
// Output:        None

void CoolBase_Binary_Tree::curpos_error (const char* Type, const char* fcn) {
  //RAISE Error, SYM(CoolBase_Binary_Tree), SYM(Invalid_Cpos),
  printf ("CoolBase_Binary_Tree<%s>::%s: Invalid current position.\n", Type, fcn);
  abort ();
}



