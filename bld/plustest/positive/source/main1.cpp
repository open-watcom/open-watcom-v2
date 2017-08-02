#include "fail.h"

int main() {
    // Must print the pass message without using the _PASS macros
    // because those use return or exit().
//    puts( "PASS " __FILE__ );
}
