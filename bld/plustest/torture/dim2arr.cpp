#include <assert.hpp>
#include <array.h>

template<class T>
class Dim2Array : public JAM_Array< JAM_Array<T> > {
public:
   Dim2Array(size_t m = 0, size_t n = 0)
      : JAM_Array< JAM_Array<T> >(m,n) {}
};

main()
{
   Dim2Array<float> a(5,10);
   assert(a.length()==5);
   assert(a[0].length()==10);
   a[4][9] = 1.234;
   if( a[4][9] > 1 && a[4][9] < 2 ) {
       cout << "PASS" << endl;
   }
   return 0;
}
