// mis-parses constructor for B as a bit-field
class A
{
 public:
   A(){}
   void B(){}
};

class B: public A
{
 public:
   B():A(){}
};

class C: public B
{
 public:
   C():B(){}
};
