
struct _junk {
   int a;
};

//Now, in a C++ program, you do something to the effect of:

main( )
{
   struct _junk * myjunk;

   myjunk = new struct _junk;

   // ...
}

/*
The compiler errors, because it "thinks" the "struct _junk" part of the call
to new() is a NEW CLASS DECLARATION, which of course has not been defined
yet. If the compiler's scoping is changed with the addition of parentheses,
then it works as expected (no warnings, no errors)... like:
*/

ack( )
{
   struct _junk * myjunk;

   ( myjunk = new struct _junk );

   // ...
}
