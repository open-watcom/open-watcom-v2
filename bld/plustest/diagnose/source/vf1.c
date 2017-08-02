struct V {
    int v;
    virtual void foo( void );
    V();
    V(int);
    V(char);
    ~V();
};
    V::V(){
	foo();
	v = 0;
    }
    V::V(int x){
	foo();
	v = x;
    }
    V::V(char y){
	foo();
	v = y;
    }
    V::~V(){
	foo();
    }

struct S : virtual V {
    int s;
    virtual void foo( void );
    S();
    S(int);
    S(char);
    ~S();
};
    S::S(){
	foo();
	s = 0;
    }
    S::S(int x){
	foo();
	s = x;
    }
    S::S(char y){
	foo();
	s = y;
    }
    S::~S(){
	foo();
    }
