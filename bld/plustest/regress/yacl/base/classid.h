

#ifndef _classid_h_
#define _classid_h_





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




#ifdef __GNUC__
#pragma interface
#endif

enum CL_ClassidEnum {
    _CL_Object_CLASSID        = 1
    , _CL_Binding_CLASSID
    , _CL_Integer_CLASSID    
    , _CL_String_CLASSID     
    , _CL_ByteArray_CLASSID  
    , _CL_ByteString_CLASSID 
    , _CL_Date_CLASSID       
    , _CL_TimeOfDay_CLASSID  
    , _CL_Map_CLASSID
    , _CL_IntStringMap_CLASSID
    , _CL_IntIntMap_CLASSID
    , _CL_IntPtrMap_CLASSID
    , _CL_StringStringMap_CLASSID
    , _CL_StringIntMap_CLASSID
    , _CL_StringPtrMap_CLASSID
    , _CL_GenericMap_CLASSID
    , _CL_PtrIntMap_CLASSID
    , _CL_Tree_CLASSID       
    , _CL_BitSet_CLASSID
    , _CL_Sequence_CLASSID
    , _CL_IntegerSequence_CLASSID
    , _CL_ObjectSequence_CLASSID
    , _CL_StringSequence_CLASSID
    , _CL_Set_CLASSID        
    , _CL_IntegerSet_CLASSID        
    , _CL_ObjectSet_CLASSID        
    , _CL_StringSet_CLASSID        
    , _CL_Stream_CLASSID
    , _CL_ByteStream_CLASSID
    , _CL_BinaryFile_CLASSID
    , _CL_SlottedFile_CLASSID
    , _CL_BTreeNode_CLASSID
    , _CL_BTree_CLASSID
};


#endif
