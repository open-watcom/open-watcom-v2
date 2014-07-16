#include "fail.h"

class MyClass {
       public:
             enum MyEnum { First, Second,  Third  };
};

typedef MyClass::MyEnum MyType;

MyClass::MyEnum MyVariable;

ALWAYS_PASS;
