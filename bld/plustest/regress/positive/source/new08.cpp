#include "fail.h"

struct X {
    double x;
    X(double u = 0.0) : x(u) {}
    X *before_new() {
	return new X[10];
    }
    void before_del( X *p ) {
	delete [] p;
    }
    ~X() {}
    X *after_new() {
      return new X[20];
    }
    void after_del( X *p ) {
	delete [] p;
    }
    void * operator new [] (size_t s);
    void operator delete [] (void *p, size_t s);
};

struct Y {
    double x;
    Y(double u = 0.0) : x(u) {}
    Y *before_new() {
	return new Y[10];
    }
    void before_del( Y *p ) {
	delete [] p;
    }
    ~Y() {}
    Y *after_new() {
      return new Y[20];
    }
    void after_del( Y *p ) {
	delete [] p;
    }
    void * operator new [] (size_t s);
    void operator delete [] (void *p);
};

void *save;

void * X::operator new [] (size_t s) {
    void *p = ::operator new [] (s);
    save = p;
    return(p);
}

void X::operator delete [] (void *p, size_t) {
    if( p != save ) fail(__LINE__);
    p = save;
    ::operator delete [] (p);
}

void * Y::operator new [] (size_t s) {
    void *p = ::operator new [] (s);
    save = p;
    return(p);
}

void Y::operator delete [] (void *p) {
    if( p != save ) fail(__LINE__);
    p = save;
    ::operator delete [] (p);
}

int main() {
    X *p = new X[5];
    delete [] p;
    X *q = new X[0];
    delete [] q;
    {
	X *t = new X;
	t->before_del( (t->before_new()) );
	t->after_del( (t->before_new()) );
	t->before_del( (t->after_new()) );
	t->after_del( (t->after_new()) );
	delete t;
    }
    {
	Y *t = new Y;
	t->before_del( (t->before_new()) );
#if __WATCOMC__ > 1060
	t->after_del( (t->before_new()) );
	t->before_del( (t->after_new()) );
#endif
	t->after_del( (t->after_new()) );
	delete t;
    }
    _PASS;
}
