#include "assert.hpp"
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <stdlib.h>
#include <new.h>

#include <NewLog.h>
#include <List.h>
#include <OrdSet.h>
#include <OrdKeyMa.h> //## p

typedef JAM_List<int> IntList;
typedef JAM_Iter<IntList> IntLIter;
typedef JAM_Muter<IntList> IntLMuter;

typedef IntList IndexList;

ostream& operator<<(ostream& os, const IndexList& list) {
   os << "<";
   for (IntLIter it(&list); !it.offEnd(); ) {
      os << it();
      if (++it) os << ",";
      }
   os << ">";
   return os;
}

struct IndexListComp {
   static int lessthan(const IndexList& a, const IndexList& b);
   inline static int equal(const IndexList& a, const IndexList& b)
      { return a==b; }
};

int IndexListComp::lessthan(const IndexList& list1, const IndexList& list2)
{
   assert(list1.length()==list2.length());
   for (IntLIter i1(&list1), i2(&list2); !i1.offEnd(); i1.next(), i2.next()) {
      if (i1()<i2()) return 1;
      else if (i1()>i2()) return 0;
      }
   return 0;
}

inline int operator<(const IndexList& a, const IndexList& b)
{ return IndexListComp::lessthan(a,b); }

typedef JAM_OrdKeyMapCompIter<IndexList, int, IndexListComp > KeyIterator;

template<class T> class MDSlice;

template<class T>
class MDArray {
friend class MDSlice<T>;
template<class TT>
friend ostream& operator<<(ostream&, const MDArray<TT>&);
template<class TT>
friend int operator==(const MDArray<TT>& arr1, const MDArray<TT>& arr2);
public:
   MDArray();
   MDArray(int sz) : _dimensions(IntList::of(sz)) {}
   MDArray(int m, int n) : _dimensions(IntList::of(m, n)) {}
   MDArray(int x, int y, int z) : _dimensions(IntList::of(x, y, z)) {}

   MDArray(const IntList& dimensions)
      : _dimensions(dimensions), _data() {}

   MDArray<T>& enter(const IndexList& idxs, const MDArray<T>& mdarr); 

   MDArray<T>& enter(const IndexList& idxs, T t); 

   MDArray<T>& enter(int i, T t)
      { return enter(IndexList::of(i), t); }

   MDArray<T>& enter(int i, int j, T t)
      { return enter(IndexList::of(i, j), t); }

   MDArray<T>& enter(int x, int y, int z, T t)
      { return enter(IndexList::of(x, y, z), t); }

   MDArray<T> operator[](const IndexList& idxs) const; 

   MDArray<T> operator()(int subscript) const;

   MDSlice<T> operator()() const;

   operator T() const {
      assert(_dimensions.length()==0);
      return _data.elem(IndexList());
      }

   int rank() const
      { return _dimensions.length(); }

protected:
   IntList _dimensions;
   JAM_OrdKeyMapComp<IndexList, T, IndexListComp > _data;
};

typedef MDArray<int> IntMDArray;

typedef JAM_KeyValue<IndexList,int,IndexListComp> E;

//void JAM_construct(E&, const E&);

void __dummy(E&d,E const &s) {
    JAM_construct(d,s);
}

void JAM_ControlledVector<E,JAM_MemDefault>::move(E* dst, E* src, size_t n, int overlap)
{
   memmove(dst, src, n*sizeof(E));
}

template<class T>
class MDSlice {
protected:
   MDSlice(const MDArray<T>& arr, int n)
      : _arr(arr), _n(n) {}
public:
   MDSlice(const MDArray<T>& arr)
      : _arr(arr), _n(1) {}

   MDSlice<T> operator()(); 
   MDArray<T> operator()(int subscript); 
protected:
   MDArray<T> _arr;
   int _n;
};

//************************************************************************
// class MDArray definition
//************************************************************************

template<class T>
MDArray<T>& MDArray<T>::enter(const IndexList& idxs, T t)
{
   if (idxs.length()!=_dimensions.length()) {
      cout << "idxs.length()=" << idxs.length() << "  " << "_dim.length()=" << _dimensions.length() << endl;
      abort();
      }
   else if (!(IndexListComp::lessthan(idxs,_dimensions))) {
      cout << "Index " << idxs << " out of range of " << _dimensions << endl;
      abort();
      }
   else
      _data.enter(idxs, t);
   return *this;
}

template<class T>
MDArray<T>& MDArray<T>::enter(const IndexList& idxs, const MDArray<T>& mdarr)
{
   if (idxs.length() + mdarr.rank() != rank()) {
      // mdarr not correct rank for idxs into this array
      cout << "IndexList " << idxs << " + " << mdarr._dimensions << " not correct rank for " << _dimensions << endl;
      abort();
      }
   else {
      // make sure the array size is okay
      int range_error = 0;
      IntLIter mydims(&_dimensions);
      for (IntLIter idx(&idxs); !idx.offEnd(); idx.next(), mydims.next())
         if (idx() >= mydims()) range_error = 1;
      for (IntLIter dims(&mdarr._dimensions); !dims.offEnd(); dims.next(), mydims.next())
         if (dims() != mydims()) range_error = 1;

      if (range_error) {
         cout << "Array " << mdarr._dimensions << " too big for " << _dimensions << " at " << idxs << endl;
         abort();
         }
      else {   // enter mdarr -- it's the right rank and size

         assert(idxs.length()>0);
         IndexList begrange = idxs;    // start index: (m,n,0,0,0)
         IndexList endrange = idxs;    // finish index: (m,n,D2-1,D3-1,D4-1)
         for (IntLIter mydim(&_dimensions, idxs.length()); mydim; ++mydim) {
            begrange += 0;
            endrange += mydim()-1;
            }

         // remove current data
         _data.remove(begrange, endrange);

         // enter new mdarr data (slow! -- needs to insert a lot into map)
         for (KeyIterator kv(&mdarr._data); !kv.offEnd(); kv.next()) {
            _data.enter(concat(idxs, kv.key()), kv.value());
            }

         }  // end enter mdarr
   }

   return *this;
}

template<class T>
MDArray<T> MDArray<T>::operator[](const IndexList& idxs) const
{
   int range_error = 0;
   IntLIter mydims(&_dimensions);
   for (IntLIter idx(&idxs); !idx.offEnd(); idx.next(), mydims.next())
      if (idx() >= mydims()) range_error = 1;
   if (range_error) {
      cout << "Index " << idxs << " out of range in operator[] for array " << _dimensions << "." << endl;
      abort();
      }

   IndexList resdims;
   for (; !mydims.offEnd(); mydims.next())
      resdims.append(mydims());

   MDArray<T> result(resdims);

   assert(idxs.length()>0);
   IndexList begrange = idxs;    // start index: (m,n,0,0,0)
   IndexList endrange = idxs;    // finish index: (m,n+1,0,0,0)
   endrange.remove_last();
   endrange += idxs.last() + 1;
   int num_zeroes = rank() - idxs.length();
   for (int i=0; i<num_zeroes; i++) {
      begrange += 0;
      endrange += 0;
      }

   int idxlen = idxs.length();
   for (KeyIterator kv(&_data,_data.search(begrange)); !kv.offEnd() && kv.key()<endrange; kv.next()) {
      IndexList residxs = kv.key();
      for (int i=0; i<idxlen; i++)
         residxs.remove_first();
      result._data.enter(residxs, kv.value());
      }
   return result;
}

template<class T>
MDArray<T> MDArray<T>::operator()(int subscript) const
{
   if (rank()==0 || subscript>= _dimensions.first()) {
      cout << "Subscript " << subscript << " out of range in operator()(index) for array " << _dimensions << "." << endl;
      abort();
      }

   IndexList resdims = _dimensions;
   resdims.remove_first();

   MDArray<T> result(resdims);

   IndexList begrange = IndexList::of(subscript);
   IndexList endrange = IndexList::of(subscript+1);
   int num_zeroes = rank() - 1;
   for (int i=0; i<num_zeroes; i++) {
      begrange += 0;
      endrange += 0;
      }

   for (KeyIterator kv(/*##*/&_data, _data.search(begrange)); !kv.offEnd() && kv.key()<endrange; kv.next()) {
      IndexList residxs = kv.key();
      residxs.remove_first();
      result._data.enter(residxs, kv.value());
      }

   return result;
}

template<class T>
MDSlice<T> MDArray<T>::operator()() const
{
   return MDSlice<T>(*this);
}


template<class T>
ostream& operator<<(ostream& os, const MDArray<T>& mdarr)
{
   if (mdarr.rank()==0) {
      os << mdarr._data.elem(IntList());
      return os;
      }
   else {
      for (IntLIter iter(&mdarr._dimensions); !iter.offEnd(); ) {
         os << "[" << iter() << "]";
         if (!++iter) os << endl;
         }
      }
   for (KeyIterator kv(&mdarr._data); !kv.offEnd(); ) {
      os << kv.key() << "=" << setw(3) << setfill('0') << kv.value();
      if (++kv) os << '\t';
      else os << '\n';
      }
   return os;
}

template<class T>
int operator==(const MDArray<T>& arr1, const MDArray<T>& arr2)
{
   if (!(arr1._dimensions == arr2._dimensions)) return 0;
   if (arr1._data.num() != arr2._data.num()) return 0;
   for (KeyIterator kv1(&arr1._data), kv2(&arr2._data);
                                          kv1; ++kv1, ++kv2)
      if (kv1.key()!=kv2.key() || kv1.value()!=kv2.value()) {
         cout << kv1.key() << "!=" << kv2.key() << " || "
               << kv1.value() << "!=" << kv2.value() << endl;
         return 0;
         }
   return 1;
}

//************************************************************************
// class MDSlice definition
//************************************************************************

template<class T>
MDSlice<T> MDSlice<T>::operator()()
{
   return MDSlice<T>(this->_arr, _n+1);
}


template<class T>
MDArray<T> MDSlice<T>::operator()(int subscript)
{
   assert(_arr.rank()>_n);
   // remove nth dim
   IntList new_dims = _arr._dimensions;
   IntLMuter iter(&new_dims);
   for (int i=0; i<_n; i++) iter.next();

   assert(subscript < *iter);
   iter.remove();

   MDArray<T> result(new_dims);

   for (KeyIterator kv(&_arr._data); !kv.offEnd(); kv.next()) {
      IndexList new_idx = kv.key();
      IntLMuter iter(&new_idx);
      for (int i=0; i<_n; i++) iter.next();
      if (iter()==subscript) {
         iter.remove();
         result.enter(new_idx, kv.value());
         }
      }

   return result;
}

//************************************************************************
// main() definition
//************************************************************************

#ifdef XX
IntList input_dimensions(istream& is)
{
   IntList il;
   for (;;) {
      char c = 0;
      cin >> ws;
      if (c.peek()!='[') return il;
      cin >> c;      // get "["
      assert(c=='[');
      int i;
      cin >> i >> c;    // get dimension and "]"
      assert(c==']');
      il.append(i);
      }
}
#endif

void MEM_ERROR()
{
   cout << "yea!" << endl;
   abort();
}

extern unsigned _stklen = 10000U;   // originally 20000

void hack() {
     JAM_Vector<int> a, b; a.resize(5); b = a;
}

main()
{
   ofstream output("output.out");
   cout << "main()" << endl;
   set_new_handler(JAM_out_of_mem);
   JAM_NewLog newlog;

   IntList dims;
   dims.append(1); dims.append(2); dims.append(3); dims.append(4); dims.append(5);
   const int N1=5, N2=5, N3=5, N4=5;      // 7 is test, 5 shows bug
   IntList nn; nn.append(N1); nn.append(N2); nn.append(N3); nn.append(N4);
   MDArray<int> darr(nn);

   for (int i1=0; i1<N1; i1++) {
      output << (N1-i1) << " ";
      int v1 = i1;
      for (int i2=0; i2<N2; i2++) {
         int v2 = i2 + v1*10;
         for (int i3=0; i3<N3; i3++) {
            int v3 = i3 + v2*10;
            for (int i4=0; i4<N4; i4++) {
               int v4 = i4 + v3*10;
               IndexList ii; ii.append(i1).append(i2).append(i3).append(i4);
               darr.enter(ii, v4);
               }
            }
         }
      }

   output << darr << endl;
   {  output << "(N1-1)()(N3-1):\n" << darr(N1-1)()(N3-1) << endl; }
   { output << "()()(1):\n" << darr()()(1) << endl; }
   { output << "(0)(1)(1):\n" << darr(0)(1)(1) << endl; }

   MDArray<int> x(N3, N4);
   for (int j=0; j<N3; j++)
      for (int k=0; k<N4; k++)
         x.enter(j, k, (j+1)*5);
   output << "x 2d array = " << x << endl;

   IndexList i; i.append(1); i.append(1);
   cout << "entering...\n"; //###
   darr.enter(i, x);
   cout << "Finished entering.\n"; //###
   output << "new darr: " << darr << endl;
   output << "darr[i]: " << darr[i] << endl;
   assert(darr[i]==x);

   return 0;
}

