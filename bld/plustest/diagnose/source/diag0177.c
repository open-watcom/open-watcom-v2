// non-static member function cannot be used as operand of ":"

  void nonmemb( char* s );


  struct S {
   void foo(char* s);
   void bar(char* s);

   static void (S::*mf)( char* );

   void test( int j )
   {
    mf = j ? &S::foo : &S::bar;
    mf = & (j ? S::foo : S::bar );
    (j ? nonmemb : bar)("test"); 
    (j ? foo : nonmemb)("test"); 
    (j ? nonmemb : nonmemb)("test");
    (j ? foo : bar)("test");
   }
  };

  void main()
  {
   S s;
   s.test( 1 );
   s.test( 0 );
  }
