/****************************************************************************
File: List.cpp

Description:  template class JAM_List<T>


Usage:

Notes:


History:
1992 Feb 20 Jam      created from JAM_List, name changes, etc.
1992 Apr 28 Jam      made JAM_List Iterator funcs non-inlined because of BC++ 3.0 bug
1993 Mar 30 Jam      added remove_all(item) member function

****************************************************************************/
#ifndef JAM_List_CPP
#define JAM_List_CPP

#include <List.h>

//************************************************************************
// JAM_List member functions
//************************************************************************

template<class T>
int operator==(const JAM_List<T>& list1, const JAM_List<T>& list2)
{
   JAM_assert(list1.state==JAM_List<T>::ALIVE);//###
   JAM_assert(list2.state==JAM_List<T>::ALIVE);//###
   JAM_assert(list1._data.allocated()!=size_t(-1) && list2._data.allocated()!=size_t(-1));  //###
   if (&list1==&list2) return 1;
   else if (list1.length() != list2.length()) return 0;
   else
      for (size_t pos=0; pos<list1.length(); ++pos)
         if (!(list1._data[pos]==list2._data[pos])) return 0;
   return 1;
}

template<class T> int JAM_List<T>::contains(const T& item) const
{
   JAM_assert(_data.allocated()!=size_t(-1));  //###
   for (size_t pos=0; pos<_data.length(); ++pos)
      if (_data[pos]==item) return 1;
   return 0;
}

template<class T> JAM_List<T>& JAM_List<T>::append(const JAM_List<T>& list)
{
   JAM_assert(_data.allocated()!=size_t(-1));  //###
   JAM_assert(list._data.allocated()!=size_t(-1));  //###
   if (_data.length()+list._data.length() > _data.allocated())
      _data.allocate(_data.length()+list._data.length());
   for (size_t pos=0; pos<list.length(); ++pos)
      _data.append(list._data[pos]);
   return *this;
}

template<class T> void JAM_List<T>::remove_all(const T& item)
{
   JAM_assert(_data.allocated()!=size_t(-1));  //###
   JAM_Muter< JAM_List<T> > m(this);
   while (m)
      if (*m == item) m.remove();
      else ++m;
}

template<class T>
JAM_List<T> JAM_List<T>::of(const T& item)
{
   JAM_List<T> tmp; tmp.append(item); return tmp;
}

template<class T>
JAM_List<T> JAM_List<T>::of(const T& item1, const T& item2)
{
   JAM_List<T> tmp;
   tmp.append(item1).append(item2);
   return tmp;
}

template<class T>
JAM_List<T> JAM_List<T>::of(const T& item1, const T& item2, const T& item3)
{
   JAM_List<T> tmp;
   tmp.append(item1).append(item2).append(item3);
   return tmp;
}


//************************************************************************
// JAM_Iterator member functions
//************************************************************************


//************************************************************************
// JAM_List functions
//************************************************************************

template<class T>
JAM_List<T> concat(const JAM_List<T>& list1, const JAM_List<T>& list2)
{
   JAM_List<T> tmp(list1);
   tmp.append(list2);
   return tmp;
}

template<class T>
ostream& operator<<(ostream& os, const JAM_List<T>& list)
{
   os << '(';
   if (!list.empty()) {
      for (JAM_Iter< JAM_List<T> > i(&list); i; ++i)
         os << ' ' << *i;
      os << ' ';
   }
   os << ')';
   return os;
}

template<class T>
istream& operator>>(istream& is, JAM_List<T>& list)
{
   char c = 0;
   is >> c;    // skip whitespace and get first character
   if (is.eof()) return is;   // do not set badbit if just eof
   if (c!='(') {
      is.clear(ios::badbit || is.rdstate());
      return is;
      }
   JAM_List<T> newlist;  // don't change parameter until sure input succeeds
   while (is>>ws && is.peek()!=')') {
      T item;
      is >> item >> ws;
      newlist += item;
      }
   if (is.get() != ')') {
      is.clear(ios::badbit || is.rdstate());
      return is;
      }
   list = newlist;
   return is;
}

#endif // JAM_List_CPP

template<class T>
JAM_List<T>::JAM_List()
      { state = ALIVE; }  //###

template<class T>
JAM_List<T>::JAM_List(const JAM_List<T>& l)
      : _data(l._data)
      { JAM_assert(l.state==ALIVE);
        state = ALIVE; }  //###

template<class T>
void JAM_List<T>::operator=(const JAM_List<T>& l)
      { JAM_assert(state==ALIVE);
         JAM_assert(l.state==ALIVE);
         _data = l._data; }  //###

template<class T>
JAM_List<T>::~JAM_List()  //###
      { JAM_assert(state==ALIVE); state = DEAD; }


