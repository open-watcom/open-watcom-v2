#include "exc01.h"

TEM::~TEM()
{ }

C::C() {
    w();
}

void C::w() {
    throw 5;
}
