



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





// Instantiate a few B-trees.

#include "base/tbtreimp.cpp"

template class CL_BTree<CL_String>;
template class CL_BTree<CL_ObjectPtr>;
template class CL_BTree<long>;
template class CL_BTreeIterator<CL_String>;
template class CL_BTreeIterator<CL_ObjectPtr>;
template class CL_BTreeIterator<long>;

template class CL_TemplateNodeSpace<CL_String>;
template class CL_TemplateNodeSpace<CL_ObjectPtr>;
template class CL_TemplateNodeSpace<long>;

template class CL_Comparator<CL_ObjectPtr>;
template class CL_Comparator<long>;
