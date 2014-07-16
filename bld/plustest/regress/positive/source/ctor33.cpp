#include "fail.h"

#include <fstream.h>    // fstream
#include <stdlib.h>     // EXIT_SUCCESS
#include <iostream.h>

class TextFile {
   public:
   ifstream solidTextFile;
};
class SolidTextFile : public TextFile {
};


int main() {
    SolidTextFile aFile;
    _PASS;
}
