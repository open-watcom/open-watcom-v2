#include "link8.h"

TEM::~TEM()
{ }

C::C() {
    w();
}

void C::w() {
    throw 5;
}
