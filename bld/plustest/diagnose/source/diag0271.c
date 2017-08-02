void foo( int c ) {
    switch( c ) {
    case 0:
	static int y = 3;	// should not require code to initialize
	break;
    default:
	break;
    }
}
