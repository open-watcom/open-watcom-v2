struct S1 {
    int a[1];
    S1();
};

extern int side_effect();

void sam( void )
{
    /* can 'side_effect' be called before 'operator new'? */
    S1 *p = new S1[side_effect()];
}
