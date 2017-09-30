/**/
template<class T, T min, T max>
class Range {
   T d;
public:
   Range(T v) { if (v>=min && v<=max) d = v; else d = min; }
};

Range<int,1,10> score;
/* */
template<class T, void (T::*mfp)()>
class CB {
public:
   CB(T* t) { (t->*mfp)(); }
};

class Foo {
public:
   virtual void f();
};

Foo foo;
FooCB<Foo, &Foo::f> c(&foo);
/**/
template<class Letter>
    class Envelope;

template<class T>
    struct Vector {
	void operator=(Envelope< Vector<T> >&);
    };

template<class Letter>
    class Envelope : public Letter {};

typedef Vector<int> IntVector;
