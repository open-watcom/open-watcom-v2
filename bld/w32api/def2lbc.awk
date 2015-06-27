BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
}
# get uppercased module name
/^[ \t]*LIBRARY/ { ModuleName = toupper( $2 ); next }

# skip uninteresting lines
/^[ \t]*(EXPORTS|;)/ { next }

# NB: Calling conventions essentially do not exist on non-x86 platforms,
# we simply strip the decoration unless 'cpu' equals 386.

# process fastcall symbols "@symbol@size"
/^[ \t]*@[A-Za-z0-9_]+@[0-9]+/ {
  split( $1, parts, "@" ) # split the import name on the at signs
  if( cpu == "386" )
    printf( "++'%s'.'%s'..'%s'\n", $1, ModuleName, parts[2] ) > OUTFILE
  else
    printf( "++'%s'.'%s'\n", parts[2], ModuleName ) > OUTFILE
  next
}

# process stdcall symbols using "symbol@size" format
/^[ \t]*[A-Za-z0-9_]+@[0-9]+/ {
  split( $1, parts, "@" ) # split the import name on the at sign
  if( cpu == "386" )
    printf( "++'_%s'.'%s'..'%s'\n", $1, ModuleName, parts[1] ) > OUTFILE
  else
    printf( "++'%s'.'%s'\n", parts[1], ModuleName ) > OUTFILE
  next
}

# process cdecl symbols using plain "symbol" format
/^[ \t]*[A-Za-z0-9_]+/ {
  split( $1, parts, "@" ) # split the import name on the at sign
  if( cpu == "386" )
    printf( "++'_%s'.'%s'..'%s'\n", $1, ModuleName, $1 ) > OUTFILE
  else
    printf( "++'%s'.'%s'\n", parts[1], ModuleName ) > OUTFILE
  next
}
