class Y
{
   public:
     Y(Y&);
     void stuff();  // non const member fn
};

void foo(const Y& y)
{
    ((Y&)y).stuff();
}

// TopSpeed says "initialization of non 'const' reference needs a temporary"
// but ((Y*)&y)->stuff() is ok -> Should (Y&) cast create temporary?
// other workaround is to declare Y(const Y&);
// (Borland does not complain)

