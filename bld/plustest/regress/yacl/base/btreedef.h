



#ifndef _btreedef_h_ /* Sat Mar 26 15:07:45 1994 */
#define _btreedef_h_



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


// B-tree definitions:


typedef CL_BTreeNode<CL_String>     StringBTreeNode;
typedef CL_BTreeOnHeap<CL_String>   StringBTree;
typedef CL_BTreeIterator<CL_String> StringBTreeIterator;

typedef CL_BTreeNode<CL_ObjectPtr>     CL_GenericBTreeNode;
typedef CL_BTreeOnHeap<CL_ObjectPtr>   CL_GenericBTree;
typedef CL_BTreeIterator<CL_ObjectPtr> CL_GenericBTreeIterator;


#endif /* _btreedef_h_ */
