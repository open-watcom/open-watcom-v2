#include "fail.h"

// crashes Watcom 11 compiler if exceptions enabled

// extracted from Rogue Wave Tools.h++ 6.1:
class  RWCStringRef
{

};


class  RWCString
{
public:
  RWCString();                             // Null string

  RWCString(const RWCString& S)            // Copy constructor
  {
    data_ = S.data_;
    pref()/*->addReference()*/;                // Executing this method causes the exception.
  }
  
  ~RWCString(){};

  RWCStringRef* pref() const
  {
    return (((RWCStringRef*) data_) - 1);
  }

  char*         data_;   // ref. counted data (RWCStringRef is in front)
};


// extracted from my code:
struct C1
{
  virtual ~C1(){};
};

struct C2 : C1
{
  ~C2()
  {
    int x;
  }
};

struct C3 : C2
{
  C3(RWCString){};
};

struct C4 : C3

{ 
  C4(RWCString);
};

C4::C4(RWCString p) : C3 (p) {}

int main() {
    _PASS;
}
