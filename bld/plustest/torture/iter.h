/****************************************************************************
File: Iterator.h

Description:  class JAM_Iterator<T>
   

Usage:

Notes:

History:
28 Aug 1992 Jam      created from JAM_OrderedSet Iterator, some ideas from COOL
****************************************************************************/     

#ifndef JAM_Iterator_H
#define JAM_Iterator_H

#include <assert.hpp>

//**************************************************************************
// JAM_Iterator definition                                            
//**************************************************************************

template<class Container> class JAM_Iter {
public:
   JAM_Iter()
      : _container(0) {}

   JAM_Iter(const JAM_Iter<Container>& iter)
      : _container(iter._container), _pos(iter._pos) {}

   JAM_Iter(const Container* container)
      : _container(container)
      { JAM_assert(_container!=0);
        _container->gofirst(_pos); }

   JAM_Iter(const Container* container, const Container::IterState& pos)
      : _container(container), _pos(pos) { JAM_assert(_container!=0); }

   ~JAM_Iter()
      {}

   void operator=(const JAM_Iter<Container>& iter)
      { _container = iter._container; _pos = iter._pos; }

   void gofirst()
      { container().gofirst(_pos); }

   void golast()
      { container().golast(_pos); }

   int next()
      { container().next(_pos); return container().valid(_pos); }

   int prev()
      { container().prev(_pos); return container().valid(_pos); }

   int offEnd() const
      { return _container==0 || !container().valid(_pos); }

   operator const void*() const
      { return offEnd() ? 0 : this; }

   Container::IterItemType operator*() const
      { JAM_assert(!offEnd()); return container().val(_pos); }

//##   Container::IterItemTypeP operator->() const
//##      { JAM_assert(!offEnd()); return container().valp(_pos); }

   Container::IterItemType operator()() const   // deprecated
      { return *(*this); }

   // call next
   JAM_Iter<Container>& operator++()
      { next(); return *this; }
   JAM_Iter<Container> operator++(int)
      { JAM_Iter<Container> old(*this); next(); return old; }

   // call previous
   JAM_Iter<Container>& operator--()
      { prev(); return *this; }
   JAM_Iter<Container> operator--(int)
      { JAM_Iter<Container> old(*this); prev(); return old; }

protected:
   Container::IterState _pos;
   const Container* _container;

   const Container& container() const
      { JAM_assert(_container!=0); return *_container; }
};


//**************************************************************************
// JAM_Muter definition                                            
//**************************************************************************

template<class Container> class JAM_Muter : public JAM_Iter<Container> {
public:
   JAM_Muter()
      : JAM_Iter<Container>() {}

   JAM_Muter(Container* container)
      : JAM_Iter<Container>(container) {}

   JAM_Muter(Container* container, const Container::IterState& pos)
      : JAM_Iter<Container>(container, pos) {}

   Container::MuterItemType operator*() const
      { JAM_assert(!offEnd()); return mut_container().ref(_pos); }

//##   Container::MuterItemTypeP operator->() const
//##      { JAM_assert(!offEnd()); return mut_container().refp(_pos); }

   JAM_Muter<Container>& operator++()
      { next(); return *this; }
   JAM_Muter<Container> operator++(int)
      { JAM_Muter<Container> old(*this); next(); return old; }

   JAM_Muter<Container>& operator--()
      { prev(); return *this; }
   JAM_Muter<Container> operator--(int)
      { JAM_Muter<Container> old(*this); prev(); return old; }

   void remove()
   //: removes current item from container
      { JAM_assert(!offEnd()); mut_container().remiter(_pos); }

   void insert(Container::IterItemType item)
   //: removes current item from container
      { JAM_assert(!offEnd()); mut_container().insiter(_pos, item); }

protected:
   Container& mut_container() const
      { JAM_assert(_container!=0); return *(Container*)_container; }
};


#endif // JAM_Muter_H


