struct x{
};
typedef struct x X;

void junk( struct x*);
extern X x;

void foo()
{
  junk( &x);
//Zortech Error: '::' or '(' expected after class 'x'
//Borland raises no errors
}

/*
resolution: replace "struct x{};typedef struct x X;" with
                        struct X {};, and
        void junk(struct x*) with void junk(X*);
*/

