//#pragma inline_depth(1);
//
// Copyright (C) 1991 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company provides this software "as is" without
// express or implied warranty.
//
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks

#include <cool/Vector.h>
#include <cool/Vector.C>

template <class Type>
class CoolSharedVector : public CoolVector<Type>, public CoolShared {
public:
  CoolSharedVector() : CoolVector<Type>() {}
  CoolSharedVector(size_t n) : CoolVector<Type>(n) {}
  CoolSharedVector(void* p, size_t n) : CoolVector<Type>(p,n) {}
  CoolSharedVector(size_t n, const Type& t) : CoolVector<Type>(n,t) {}
  // did not do CoolVector(size_t n, size_t num_init, const T&, ...)
};

#include <cool/Handle.h>
#include <cool/test.h>


void test_constructor () {                      // test constructors
  typedef CoolSharedVector<int> SharedVec;
  CoolHandle< SharedVec > h0;
//  CoolHandle< CoolSharedVector<int> > h0;
  TEST ("Handle h0", ptr(h0), NULL);            // empty handle

  CoolSharedVector<int>* v0 = new CoolSharedVector<int>();
  TEST ("Type* ptr", v0->reference_count(), 0); // count initially 0

  CoolHandle<CoolSharedVector<int> > h1(v0);            // construct from ptr
  TEST ("Handle h1(Type*)", h1->reference_count(), 1); 
  {
    CoolHandle<CoolSharedVector<int> > h2(*v0); // construct from obj
    TEST ("Handle h2(Type&)", v0->reference_count(), 2);
    {
      CoolHandle<CoolSharedVector<int> > h3(h1);        // copy constructor
      TEST ("Handle h3(Handle&)", v0->reference_count(), 3);
    }
    TEST ("~Handle", v0->reference_count(), 2);
  }
  TEST ("~Handle", v0->reference_count(), 1);
}

void test_assign () {                           // test assignment operators.
  CoolSharedVector<int>* v0 = new CoolSharedVector<int>();
  TEST ("Type* ptr", v0->reference_count(), 0); // count initially 0
  
  CoolHandle<CoolSharedVector<int> > h1 = v0;           // assign ptr
  TEST ("Handle h1 = Type*", v0->reference_count(), 1);
  {
    CoolHandle<CoolSharedVector<int> > h2 = *v0;        // assign obj
    TEST ("Handle h2 = Type&", v0->reference_count(), 2);
    {
      CoolHandle<CoolSharedVector<int> > h3 = h1;       // assign hdl
      TEST ("Handle h3 = Handle&", v0->reference_count(), 3);
    }
    TEST ("~Handle", v0->reference_count(), 2);
  }
  TEST ("~Handle", v0->reference_count(), 1);
}


void test_conversion_deref () {         // test conversion and ->
  CoolSharedVector<int>* v0 = new CoolSharedVector<int>();
  CoolHandle<CoolSharedVector<int> > h1(v0);
  TEST ("Handle h(Type*)", h1->reference_count(), 1); // operator ->
  
  CoolSharedVector<int>* v1 = ptr(h1);          // manually convert to ptr
  TEST ("ptr(Handle&)", v1, v0);
  
  CoolSharedVector<int>& v11 = ref(h1);         // manually convert to ref
  TEST ("ref(Handle&)", &v11, v0);
  
  {
    CoolHandle<CoolSharedVector<int> > h2;
    {
      h2 = hdl(v0);                             // manually convert to hdl
    }                                           // make 2 handles (stack & value)
    TEST ("hdl(Type*)", h2->reference_count(), 2); // tmp hdl deleted 
    
    CoolSharedVector<int>* v2 = h2;                     // automatic conversion to ptr
    TEST ("(Type*) h", v2, v1);
  }
  TEST ("~Handle", h1->reference_count(), 1);
}


void test_bind_cmp () {                 // test bind and compare ops
  CoolSharedVector<int>* v0 = new CoolSharedVector<int>();
  CoolHandle<CoolSharedVector<int> > h1(v0);
  TEST ("Handle h(Type*)", v0->reference_count(), 1); 
  
  CoolHandle<CoolSharedVector<int> > h2;
  h2 = v0;
  TEST ("h2 = v0", v0->reference_count(), 2);   
  
  h1 = v0;
  TEST ("h1 = v0", v0->reference_count(), 2);   
  
  TEST ("h1 == h2", h1 == h2, 1);
  
  h2 = new CoolSharedVector<int>();
  TEST ("h1 != h2", h1 != h2, 1);  
}

void test_use () {                              // example of use and style
  typedef CoolSharedVector<int> Object;
  typedef CoolHandle<CoolSharedVector<int> > ObjectH;
  typedef CoolSharedVector<int>* ObjectP;
  typedef CoolSharedVector<int>& ObjectR;
  typedef CoolSharedVector<CoolHandle<CoolSharedVector<int> > > Container;
  
  ObjectH h = hdl(new Object(1, 1));            // smart compilers reuse temp hdl
  TEST ("ObjectH h = hdl(p)", 
        (h->reference_count()==1 || h->reference_count()==2), TRUE);
  
  ObjectP p = new Object(1, 1);
  TEST ("ObjectP p", p->reference_count(), 0);

  {
#if __WATCOMC__ < 1000
const int temp_adjust = 1;
#else
const int temp_adjust = 0;
#endif
  
  Container c(5, hdl(new Object()));
  TEST ("Container c", c[0]->reference_count(), 5 + temp_adjust );
  
  {
    c[0] = hdl(p);                              
    TEST ("c[0] = hdl(p)", p->reference_count()==(1+temp_adjust) && c[4]->reference_count()==(4+temp_adjust), TRUE);
    c[1] = p;
    TEST ("c[1] = p", p->reference_count()==(2+temp_adjust) && c[4]->reference_count()==(3+temp_adjust), TRUE);
    c[2] = *p;
    TEST ("c[2] = *p", p->reference_count()==(3+temp_adjust) && c[4]->reference_count()==(2+temp_adjust), TRUE);
    c[3] = c[4];
    TEST ("c[3] = c[4]", p->reference_count()==(3+temp_adjust) && c[4]->reference_count()==(2+temp_adjust), TRUE);
  }
  TEST ("c[i] = p", p->reference_count()==3 && c[3]->reference_count()==(2+temp_adjust),
        TRUE);
  
  {
    ObjectH h0 = c[0];
    TEST ("ObjectH h = [ci]", p->reference_count(), 4);
  }
  TEST ("~ObjectH", p->reference_count(), 3);
  
  ObjectP p0 = c[0];
  TEST ("ObjectP p = [ci]", p->reference_count(), 3);
  
  ObjectR r0 = c[0];
  TEST ("ObjectR r = [ci]", p->reference_count(), 3);

  }
}


// handle<obj> must follow obj declaration, to access handle_count.
// Ordering of class declarations should be enforced in header files.

// Children are shared between 2 parents, through handle and handle_count.
// If one parent died, the count on the children are deleted. 
// If all parents died, the children are also deleted.
// Backward pointers from children to parents are implemented with plain pointers,
// and do not use handle_count.

class Parent;                                   // forward declaration.

class Child                                     // child with 2 parents.
: public CoolSharedVector<Parent*> {    
//##friend class CoolHandle<Child>;                     // for handle use

public:
  Child ()
    : CoolSharedVector<Parent*>() {;}
  Child (Parent* p1, Parent* p2)
    : CoolSharedVector<Parent*>(2) { put(p1,0); put(p2,1); }
  ~Child() {;}
};

typedef CoolHandle<Child> ChildH;    // used to be done by Handle

class Parent                                    // parent with 2 children.
: public CoolSharedVector<CoolHandle<Child> > {
//##friend class CoolHandle<Parent>;            // for handle use

public:
  Parent ()
    : CoolSharedVector<CoolHandle<Child> >() {;}
  Parent (ChildH& c1, ChildH& c2)
    : CoolSharedVector<CoolHandle<Child> >(2, hdl(new Child()))
      { 
        put(c1,0);
        put(c2,1);
      }
  // the following code doesnot work because of va_arg is macro
  // and cannot pass objects in ... by reference. 
  // Compiler doesnot have typeinfo in ... to call constructors...
  //: (2, 2, c1, c2)                    

  ~Parent() {;}
};


void test_cycle () {                            // test deletion of cycles
  ChildH c1(new Child(NULL, NULL));
  ChildH c2(new Child(NULL, NULL));
  TEST ("ChildH c1", c1->reference_count(), 1);
  TEST ("ChildH c2", c2->reference_count(), 1);

  {
    Parent p1(c1, c2);
    ref(c1)[0] = &p1;
    ref(c2)[0] = &p1;
    TEST ("Parent p1(c1, c2)", c1->reference_count(), 2);
    TEST ("Parent p1(c1, c2)", c2->reference_count(), 2);
    TEST ("Parent p1(c1, c2)", p1.reference_count(), 0);

    {
      Parent p2(c1, c2);
      ref(c1)[1] = &p2;
      ref(c2)[1] = &p2;    
      TEST ("Parent p2(c1, c2)", c1->reference_count(), 3);
      TEST ("Parent p2(c1, c2)", c2->reference_count(), 3);
      TEST ("Parent p2(c1, c2)", p2.reference_count(), 0);
    }
    TEST ("~Parent(c1)", c1->reference_count(), 2);
    TEST ("~Parent(c2)", c2->reference_count(), 2);
    TEST ("~Parent", p1.reference_count(), 0);
  }
  TEST ("~Parent(c1)", c1->reference_count(), 1);
  TEST ("~Parent(c2)", c2->reference_count(), 1);
}


void test_leak () {                             // test memory leak.
  for (;;) {                                    // use top4.1 to watch memory usage
    test_constructor();
    test_assign();
    test_conversion_deref();
    test_bind_cmp();
    test_use();
    test_cycle();
  }
}

int main (void) {
  START("CoolHandle");
  test_constructor();
  test_assign();
  test_conversion_deref();
  test_bind_cmp();
  test_use();
  test_cycle();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
