#include "fail.h"

namespace ns {
enum E1 {
    E1_VAL0,
    E1_VAL1
};
};

typedef struct cls {
  enum E2 {
    E2_VAL0,
    E2_VAL1
  };

  enum E2 f();
  enum E2 g();
  E2 h();
  E2 i();
} CLS;

enum CLS::E2 CLS::f() {
  return E2_VAL0;
}

enum cls::E2 cls::g() {
  return E2_VAL0;
}

CLS::E2 CLS::h() {
  return E2_VAL0;
}

cls::E2 cls::i() {
  return E2_VAL0;
}


int main() {
    enum ns::E1 e1 = ns::E1_VAL0;
    ns::E1 e2 = ns::E1_VAL0;

    _PASS;
}
