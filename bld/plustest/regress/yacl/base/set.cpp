



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





#ifndef _set_cxx_ /* Tue Nov 16 22:11:34 1993 */
#define _set_cxx_

#ifdef __GNUC__
#pragma implementation
#endif




#include "base/set.h"
#include "base/setimp.cpp"


//   "Exception" methods  for the set class


template class CL_Set<CL_ObjectPtr>;
template class CL_Set<long>;

template class CL_SetIterator<CL_ObjectPtr>;
template class CL_SetIterator<long>;


#endif /* _set_cxx_ */

