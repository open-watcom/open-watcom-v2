#include "fail.h"
#pragma inline_depth(0);

#if __WATCOMC__ > 1060

void *save;
unsigned location;

struct X1 {
    long _x;
    X1(long u = 0) : _x(u) {}
    X1 *before_new() {
	return new X1;
    }
    void before_del( X1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~X1() {}
    X1 *after_new() {
      return new X1;
    }
    void after_del( X1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( X1 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct X2 {
    long _x;
    X2(long u = 0) : _x(u) {}
    X2 *before_new() {
	return new X2;
    }
    void before_del( X2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~X2() {}
    X2 *after_new() {
      return new X2;
    }
    void after_del( X2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct X3 {
    long _x;
    X3 *before_new() {
	return new X3;
    }
    void before_del( X3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~X3() {}
    X3 *after_new() {
      return new X3;
    }
    void after_del( X3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct X4 {
    long _x;
    X4 *before_new() {
	return new X4;
    }
    void before_del( X4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~X4() {}
    X4 *after_new() {
      return new X4;
    }
    void after_del( X4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( X4 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

int something;

struct DX1 {
    long _x;
    DX1(long u = 0) : _x(u) {}
    DX1 *before_new() {
	return new DX1;
    }
    void before_del( DX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~DX1() {++something;}
    DX1 *after_new() {
      return new DX1;
    }
    void after_del( DX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( DX1 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct DX2 {
    long _x;
    DX2(long u = 0) : _x(u) {}
    DX2 *before_new() {
	return new DX2;
    }
    void before_del( DX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~DX2() {++something;}
    DX2 *after_new() {
      return new DX2;
    }
    void after_del( DX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct DX3 {
    long _x;
    DX3 *before_new() {
	return new DX3;
    }
    void before_del( DX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~DX3() {++something;}
    DX3 *after_new() {
      return new DX3;
    }
    void after_del( DX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct DX4 {
    long _x;
    DX4 *before_new() {
	return new DX4;
    }
    void before_del( DX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~DX4() {++something;}
    DX4 *after_new() {
      return new DX4;
    }
    void after_del( DX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( DX4 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct VDX1 {
    long _x;
    VDX1(long u = 0) : _x(u) {}
    VDX1 *before_new() {
	return new VDX1;
    }
    void before_del( VDX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    virtual ~VDX1() {++something;}
    VDX1 *after_new() {
      return new VDX1;
    }
    void after_del( VDX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( VDX1 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct VDX2 {
    long _x;
    VDX2(long u = 0) : _x(u) {}
    VDX2 *before_new() {
	return new VDX2;
    }
    void before_del( VDX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    virtual ~VDX2() {++something;}
    VDX2 *after_new() {
      return new VDX2;
    }
    void after_del( VDX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct VDX3 {
    long _x;
    VDX3 *before_new() {
	return new VDX3;
    }
    void before_del( VDX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    virtual ~VDX3() {++something;}
    VDX3 *after_new() {
      return new VDX3;
    }
    void after_del( VDX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct VDX4 {
    long _x;
    VDX4 *before_new() {
	return new VDX4;
    }
    void before_del( VDX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    virtual ~VDX4() {++something;}
    VDX4 *after_new() {
      return new VDX4;
    }
    void after_del( VDX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( VDX4 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct NDX1 {
    long _x;
    NDX1(long u = 0) : _x(u) {}
    NDX1 *before_new() {
	return new NDX1;
    }
    void before_del( NDX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~NDX1() {++something;}
    NDX1 *after_new() {
      return new NDX1;
    }
    void after_del( NDX1 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( NDX1 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct NDX2 {
    long _x;
    NDX2(long u = 0) : _x(u) {}
    NDX2 *before_new() {
	return new NDX2;
    }
    void before_del( NDX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~NDX2() {++something;}
    NDX2 *after_new() {
      return new NDX2;
    }
    void after_del( NDX2 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct NDX3 {
    long _x;
    NDX3 *before_new() {
	return new NDX3;
    }
    void before_del( NDX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~NDX3() {++something;}
    NDX3 *after_new() {
      return new NDX3;
    }
    void after_del( NDX3 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p) {
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

struct NDX4 {
    long _x;
    NDX4 *before_new() {
	return new NDX4;
    }
    void before_del( NDX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    ~NDX4() {++something;}
    NDX4 *after_new() {
      return new NDX4;
    }
    void after_del( NDX4 *p , unsigned line ) {
	location = line;
	delete p;
    }
    void * operator new (size_t s) {
	void *p = ::operator new (s);
	save = p;
	return(p);
    }
    void operator delete (void *p, size_t __s) {
	if( __s != sizeof( NDX4 ) ) fail(location);
	if( p != save ) { fail( location ); return; }
	p = save;
	::operator delete (p);
    }
};

int main() {
    X1 *p = new X1;
    location = __LINE__, delete p;
    X1 *q = new X1;
    location = __LINE__, delete q;
    {
	X1 *t = new X1;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	X2 *t = new X2;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	X3 *t = new X3;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	X4 *t = new X4;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	DX1 *t = new DX1;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	DX2 *t = new DX2;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	DX3 *t = new DX3;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	DX4 *t = new DX4;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	VDX1 *t = new VDX1;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	VDX2 *t = new VDX2;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	VDX3 *t = new VDX3;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	VDX4 *t = new VDX4;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	NDX1 *t = new NDX1;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	NDX2 *t = new NDX2;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	NDX3 *t = new NDX3;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    {
	NDX4 *t = new NDX4;
	void *save_save = save;
	t->before_del( (t->before_new()), __LINE__ );
	t->after_del( (t->before_new()), __LINE__ );
	t->before_del( (t->after_new()), __LINE__ );
	t->after_del( (t->after_new()), __LINE__ );
	save = save_save;
	location = __LINE__, delete t;
    }
    _PASS;
}
#else
int main()
{
    _PASS;
}
#endif
