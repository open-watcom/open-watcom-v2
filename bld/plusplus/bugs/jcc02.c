class X
{
  public:
    X(const char* v...);
//TopSpeed Error: Syntax error, expected:  [ ( ) = , ::
};

X::X(const char* v...)
//TopSpeed Error: Syntax error, expected:  [ ( ) = , ::
{
}
// insert comma before ellipses, all is ok
// no similar error in non-member functions
