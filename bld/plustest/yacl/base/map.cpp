



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






#ifndef _map_cxx_ /* Tue Nov 16 22:12:10 1993 */
#define _map_cxx_

#ifdef __GNUC__
#pragma implementation
#endif


#include "base/string.h"
#include "base/integer.h"

#define _no_cl_map_typedefs_
#include "base/mapimp.cpp"




template class CL_Map<long, CL_String>;
template class CL_Map<long, long>;
template class CL_Map<long, CL_ObjectPtr>;
template class CL_Map<CL_String, CL_String>;
template class CL_Map<CL_String, long>;
template class CL_Map<CL_String, CL_ObjectPtr>;
template class CL_Map<CL_ObjectPtr, CL_ObjectPtr>;
template class CL_Map<CL_ObjectPtr, long>;


template class CL_MapAssoc<long, CL_String>;
template class CL_MapAssoc<long, long>;
template class CL_MapAssoc<long, CL_ObjectPtr>;
template class CL_MapAssoc<CL_String, CL_String>;
template class CL_MapAssoc<CL_String, long>;
template class CL_MapAssoc<CL_String, CL_ObjectPtr>;
template class CL_MapAssoc<CL_ObjectPtr, CL_ObjectPtr>;
template class CL_MapAssoc<CL_ObjectPtr, long>;


template class CL_MapIterator<long, CL_String>;
template class CL_MapIterator<long, long>;
template class CL_MapIterator<long, CL_ObjectPtr>;
template class CL_MapIterator<CL_String, CL_String>;
template class CL_MapIterator<CL_String, long>;
template class CL_MapIterator<CL_String, CL_ObjectPtr>;
template class CL_MapIterator<CL_ObjectPtr, CL_ObjectPtr>;
template class CL_MapIterator<CL_ObjectPtr, long>;

template class CL_Basics<CL_MapAssoc<long, CL_String> >;
template class CL_Basics<CL_MapAssoc<long, long> >;
template class CL_Basics<CL_MapAssoc<long, CL_ObjectPtr> >;
template class CL_Basics<CL_MapAssoc<CL_String, CL_String> >;
template class CL_Basics<CL_MapAssoc<CL_String, long> >;
template class CL_Basics<CL_MapAssoc<CL_String, CL_ObjectPtr> >;
template class CL_Basics<CL_MapAssoc<CL_ObjectPtr, CL_ObjectPtr> >;
template class CL_Basics<CL_MapAssoc<CL_ObjectPtr, long> >;



#include "base/mapdef.h"



// CL_DEFINE_CLASS(CL_GenericMap,      _CL_GenericMap_CLASSID);
CL_DEFINE_CLASS(CL_IntStringMap,    _CL_IntStringMap_CLASSID);
CL_DEFINE_CLASS(CL_IntIntMap,       _CL_IntIntMap_CLASSID);
CL_DEFINE_CLASS(CL_IntPtrMap,       _CL_IntPtrMap_CLASSID);
CL_DEFINE_CLASS(CL_StringStringMap, _CL_StringStringMap_CLASSID);
CL_DEFINE_CLASS(CL_StringIntMap,    _CL_StringIntMap_CLASSID);
CL_DEFINE_CLASS(CL_StringPtrMap,    _CL_StringPtrMap_CLASSID);
CL_DEFINE_CLASS(CL_PtrIntMap,       _CL_PtrIntMap_CLASSID);




#endif /* _map_cxx_ */

