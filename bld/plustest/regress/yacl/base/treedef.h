

#ifndef _treedef_h_ /* Thu Nov  4 12:27:56 1993 */
#define _treedef_h_





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






// Trees:

typedef CL_Tree<CL_ObjectPtr>      CL_GenericTree;
typedef CL_TreeNode<CL_ObjectPtr>  CL_GenericTreeNode;

typedef CL_Tree<CL_VoidPtr>        CL_VoidPtrTree;
typedef CL_TreeNode<CL_VoidPtr>    CL_VoidPtrTreeNode;

typedef CL_Tree<long>              CL_IntegerTree;
typedef CL_TreeNode<long>          CL_IntegerTreeNode;




#endif
