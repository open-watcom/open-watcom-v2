/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef __MRRELOC_H__
#define __MRRELOC_H__

template <class K_T,class O_T> class WCValHashDict;

class MergeRelocate {
public:
                    MergeRelocate( int numFiles );
                    ~MergeRelocate();

    void            addReloc( MergeOffset & replaced, MergeDIE * replacer );
    MergeDIE *      getReloc( const MergeOffset & off );

    static void *   allocHNode( size_t ) { return _hPool.alloc(); }
    static void     freeHNode( void * item, size_t ) { _hPool.free( item ); }

private:
    int                                     _numFiles;
    WCValHashDict<uint_32,MergeDIE *> **    _replacedDies;
    static MemoryPool                       _hPool;
};

#endif // __MRRELOC_H__
