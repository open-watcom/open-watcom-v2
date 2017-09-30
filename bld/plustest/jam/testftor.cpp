#pragma inline_depth 0;
// Just a simple, meaningless, but pretty comprehensive set of
// tests for my Functor classes
// by Jamshid Afshar.  All code is in the public domain.

#include <iostream.h>
#include <assert.h>
#include <jam/functor0.h>
#include <jam/functor1.h>
#include <jam/functor2.h>

class Model {
public:
   int save1() { cout << "save1" << endl; return -3; }
   void save2() { cout << "save2" << endl; }
   int enter1(const char* s) { cout << s << endl; return -1; }
   void enter2(char c) { cout << c << endl; }
   int enterat1(int i, const char* s) { cout << i << ',' << s << endl; return -2; }
   void enterat2(int i, const char* s) { cout << i << ',' << s << endl; }
};

typedef JAM_Functor1<int,const char*> FP1;
typedef JAM_Functor1v<char> FP2;
typedef JAM_Functor0<int> FP3;
typedef JAM_Functor0v FP4;
typedef JAM_Functor2<int, int, const char*> FP5;
typedef JAM_Functor2v<int, const char*> FP6;

class View {
   FP1 _f1;
   FP2 _f2;
   FP3 _f3;
   FP4 _f4;
   FP5 _f5;
   FP6 _f6;
public:
   View(const FP1& f1, const FP2& f2, const FP3& f3, const FP4& f4, const FP5& f5, const FP6& f6)
      : _f1(f1), _f2(f2), _f3(f3), _f4(f4), _f5(f5), _f6(f6) {}
   void input() {
      if (_f1) cout << _f1("Hello") << endl;
      else cout << "_f1==0" << endl;
      if (_f2) _f2('J');
      else cout << "_f2==0" << endl;
      if (_f3) cout << _f3() << endl;
      else cout << "_f3==0" << endl;
      if (_f4) _f4();
      else cout << "_f4==0" << endl;
      if (_f5) cout << _f5(5, "Str1") << endl;
      else cout << "_f5==0" << endl;
      if (_f6) _f6(6, "Str2");
      else cout << "_f6==0" << endl;
      }
};

main() {
   Model m;
   FP1 f;
   assert(!f);
   f = 0; assert(!f);
   f = functor(&m, &Model::enter1);
   assert(f);
   FP1 f2;
   f = f2; assert(f==0);
   f = functor(&m, &Model::enter1); assert(f);
   f = functor((Model*)0, &Model::enter1); assert(!f);
   f = functor(&m, (int(Model::*)(const char*))0); assert(!f);
   f = functor(&m, &Model::enter1); assert(f);
   f = functor((Model*)0, (int(Model::*)(const char*))0); assert(!f);
   FP1 f3(0);
   assert(!f3);
   View v(functor(&m, &Model::enter1), functorv(&m, &Model::enter2),
         functor(&m, &Model::save1), functorv(&m, &Model::save2),
         functor(&m, &Model::enterat1), functorv(&m, &Model::enterat2));
   v.input();
   View vz(FP1(0), FP2(0), FP3(0), FP4(0), FP5(0), FP6(0));
   vz.input();

   f3 = f2;
   FP1 f4 = f3;
   if (!f4) f4=functor(&m, &Model::enter1);
   cout << f4("Hello");

   return 0;
}

