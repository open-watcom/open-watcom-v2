typedef struct {
  char j,k;
}bar;  

class fooo
{
public:
 fooo();
 ~fooo(){};
 private:
 bar * b;  
};

fooo::fooo() 
{
  int x = 100;  
// no constructor of this type
  b = new bar(x);

    if(!b)
      x = 0;
}

struct X1 {
    X1( int x ) : a(x), b(x
    {
    }
};

template <class T>
    struct X8 {
	void foo() {
	}
    }

)


#line 4

#error should be 5
#line 28 "foo.c"


#error should be "foo.c" line 30
