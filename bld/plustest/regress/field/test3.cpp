// @(#)test_2.C	1.1   Release Date: 2/23/93

#include <stdlib.h>
#include <iostream.h>
#include <time.h>

#include "field.h"

main(int argc, char *argv[]){
    #if defined(__386__)
    __chipbug = (__typeof(__chipbug)) ~0;
    #endif
  size_t SIZE, ITER;

  if (argc == 3){
    sscanf(argv[1], "%ui", &SIZE);
    sscanf(argv[2], "%ui", &ITER);
  }else if (argc == 1){
    cout << "Size of fields? ";
    cin >> SIZE;
    cout << "Number of iterations? ";
    cin >> ITER;
  }else{
    cout << "Usage: test_2.x [size iterations]\n";
    exit(1);
  }

  {
    const field a(SIZE, 2.0), b(SIZE, 1.0), c(SIZE, -3.0);
    field x, y, z;

    for (register long i=0; i<ITER; i++){
      x = a*b + c;
      y = a + x*(b + x*c);
      z = y/(1.0 + a);
    }
  }

  cout << "*****Test completed successfully*****\n";
  exit(0);
}
