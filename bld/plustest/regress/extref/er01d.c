#include "er01.h"

X::~X() {
    x = -1;
}
X::X( X const &r ) : x(r.x) {
}
