{ gsub( /\\r/, "" ) }

/^c$pragma.*\\/ {

    print $0
    do {
	getline;
	gsub( /\\r/, "" );
	print $0
    } while( /\\$/ );
    processed = 1;
}

!processed && /^c[^\$]/ {
    processed = 1;
}

!processed && /^[ \t\r]*$/ {
    processed = 1;
}

!processed && /!/ {
    print substr( $0, 1, index( $0, "!" ) - 1 )
    processed = 1;
}

{
    if( !processed ) {
	print $0;
    } else {
	processed = 0;
    }
}
