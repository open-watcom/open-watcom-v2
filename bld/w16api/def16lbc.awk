# get uppercased module name
/^LIBRARY/ { ModuleName = toupper( $2 ); next }

# skip uninteresting lines
/^ *(EXPORTS|;)/ { next }

# process symbols with ordinals using "symbol  @ord" format
/^[ \t]*[A-Za-z0-9_]+[ \t]+@[0-9]+/ {
  sub( /@/, "", $2 ) # kill of the at sign in ordinal
  printf( "++%s.%s.%s\n", $1, ModuleName, $2 )
}
