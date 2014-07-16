#include "fail.h"
#pragma inline_depth(0);

void *save;
size_t save_size;
unsigned location;

void *alloc( size_t s )
{
    save_size = s;
    return save = operator new( s );
}

struct X1 {
    double x;
    X1(double u = 0.0) : x(u) {}
    X1 *before_new() {
	return new X1[10];
    }
    void before_del( X1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~X1() {}
    X1 *after_new() {
      return new X1[20];
    }
    void after_del( X1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct X2 {
    double x;
    X2(double u = 0.0) : x(u) {}
    X2 *before_new() {
	return new X2[10];
    }
    void before_del( X2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~X2() {}
    X2 *after_new() {
      return new X2[20];
    }
    void after_del( X2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct X3 {
    double x;
    X3 *before_new() {
	return new X3[10];
    }
    void before_del( X3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~X3() {}
    X3 *after_new() {
      return new X3[20];
    }
    void after_del( X3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct X4 {
    double x;
    X4 *before_new() {
	return new X4[10];
    }
    void before_del( X4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~X4() {}
    X4 *after_new() {
      return new X4[20];
    }
    void after_del( X4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

int something;

struct DX1 {
    double x;
    DX1(double u = 0.0) : x(u) {}
    DX1 *before_new() {
	return new DX1[10];
    }
    void before_del( DX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~DX1() {++something;}
    DX1 *after_new() {
      return new DX1[20];
    }
    void after_del( DX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct DX2 {
    double x;
    DX2(double u = 0.0) : x(u) {}
    DX2 *before_new() {
	return new DX2[10];
    }
    void before_del( DX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~DX2() {++something;}
    DX2 *after_new() {
      return new DX2[20];
    }
    void after_del( DX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct DX3 {
    double x;
    DX3 *before_new() {
	return new DX3[10];
    }
    void before_del( DX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~DX3() {++something;}
    DX3 *after_new() {
      return new DX3[20];
    }
    void after_del( DX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct DX4 {
    double x;
    DX4 *before_new() {
	return new DX4[10];
    }
    void before_del( DX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~DX4() {++something;}
    DX4 *after_new() {
      return new DX4[20];
    }
    void after_del( DX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct VDX1 {
    double x;
    VDX1(double u = 0.0) : x(u) {}
    VDX1 *before_new() {
	return new VDX1[10];
    }
    void before_del( VDX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    virtual ~VDX1() {++something;}
    VDX1 *after_new() {
      return new VDX1[20];
    }
    void after_del( VDX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct VDX2 {
    double x;
    VDX2(double u = 0.0) : x(u) {}
    VDX2 *before_new() {
	return new VDX2[10];
    }
    void before_del( VDX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    virtual ~VDX2() {++something;}
    VDX2 *after_new() {
      return new VDX2[20];
    }
    void after_del( VDX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct VDX3 {
    double x;
    VDX3 *before_new() {
	return new VDX3[10];
    }
    void before_del( VDX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    virtual ~VDX3() {++something;}
    VDX3 *after_new() {
      return new VDX3[20];
    }
    void after_del( VDX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct VDX4 {
    double x;
    VDX4 *before_new() {
	return new VDX4[10];
    }
    void before_del( VDX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    virtual ~VDX4() {++something;}
    VDX4 *after_new() {
      return new VDX4[20];
    }
    void after_del( VDX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct NDX1 {
    double x;
    NDX1(double u = 0.0) : x(u) {}
    NDX1 *before_new() {
	return new NDX1[10];
    }
    void before_del( NDX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~NDX1() {++something;}
    NDX1 *after_new() {
      return new NDX1[20];
    }
    void after_del( NDX1 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct NDX2 {
    double x;
    NDX2(double u = 0.0) : x(u) {}
    NDX2 *before_new() {
	return new NDX2[10];
    }
    void before_del( NDX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~NDX2() {++something;}
    NDX2 *after_new() {
      return new NDX2[20];
    }
    void after_del( NDX2 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct NDX3 {
    double x;
    NDX3 *before_new() {
	return new NDX3[10];
    }
    void before_del( NDX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~NDX3() {++something;}
    NDX3 *after_new() {
      return new NDX3[20];
    }
    void after_del( NDX3 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

struct NDX4 {
    double x;
    NDX4 *before_new() {
	return new NDX4[10];
    }
    void before_del( NDX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    ~NDX4() {++something;}
    NDX4 *after_new() {
      return new NDX4[20];
    }
    void after_del( NDX4 *p , unsigned line ) {
	location = line;
	delete [] p;
    }
    void * operator new [] (size_t s) {
	void *p = alloc(s);
	return(p);
    }
    void operator delete [] (void *p, size_t __s) {
	if( __s != save_size ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete [] (p);
    }
};

int main() {
    X1 *p = new X1[5];
    delete [] p;
    X1 *q = new X1[0];
    delete [] q;
    {
	X1 *t = new X1;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	X2 *t = new X2;
	t->before_del( (t->before_new()), __LINE__ );
#if __WATCOMC__ > 1060
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
#endif
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	X3 *t = new X3;
	t->before_del( (t->before_new()), __LINE__ );
#if __WATCOMC__ > 1060
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
#endif
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	X4 *t = new X4;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	DX1 *t = new DX1;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	DX2 *t = new DX2;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	DX3 *t = new DX3;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	DX4 *t = new DX4;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	VDX1 *t = new VDX1;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	VDX2 *t = new VDX2;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	VDX3 *t = new VDX3;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	VDX4 *t = new VDX4;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	NDX1 *t = new NDX1;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	NDX2 *t = new NDX2;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	NDX3 *t = new NDX3;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    {
	NDX4 *t = new NDX4;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	delete t;
    }
    _PASS;
}
