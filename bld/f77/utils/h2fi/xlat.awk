#
# display the sections between XLATOFF and XLATON comments
#

/\/\* XLATOFF \*\// {
    for(;;) {
	print $0
	getline;
	if( match( $0, /\/\* XLATON \*\// ) ) break;
    }
    print $0
}

{
    # don't print these lines
}
