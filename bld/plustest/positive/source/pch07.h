#include "fail.h"
#include <process.h>
#include <iostream.h>

template <class T>
T* allocate(int size, T*) {
    //set_new_handler(0);
    T* tmp = (T*)(::operator new((unsigned int)(size * sizeof(T))));
    if (tmp == 0) {
	fail(__LINE__);
	_PASS_EXIT;
    }
    return tmp;
}

struct S {
    void x() {
	allocate( 23, (S*) 0 );
    }
};


int *x() {
    return allocate( 23, (int*) 0 );
}
