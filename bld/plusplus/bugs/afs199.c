/* "Strange" error report because of ':' instead of ';' */

class A {
    void f():  // Ooops hit the wrong key ':' instead of ';'
    };   // E585, but void f() could not be a ctor!

class B {
    void f():  // Ooops hit the wrong key ':' instead of ';'
    int g()  { return 0; } // E420
    };
