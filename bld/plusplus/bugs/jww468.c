// This one is low priority and probably needs some study but it
// looks like we should allow it.
// Borland:     yes
// WATCOM C:    yes
// CFRONT 2.1:  yes
// GNU C++:     yes
// MSC:         no
// MetaWare:    no
void foo( const double mat[4][4] );

main()
{
    double my_mat[4][4];

    foo(my_mat);
}
