while( <STDIN> ) {
    if( /^(pick|wdpick).*\(\s*(\w+).*/ ) {
	push( @picks, $2 );
    }
}
print( "#define MSG_RC_BASE\t0\n" );
$offset = 0;
for $i ( @picks ) {
    print( "#define ${i}\t(MSG_RC_BASE+$offset)\n" );
    $offset = $offset + 1;
}

