#
# this awk script is used to help convert pmddim.h... run it on the output
# of the h to fi conversion
#

{
    print $0;
}

/c##[ \t]+#define[ \t]+[A-Za-z0-9_]+\(.*\)/ {

    name = $3;
    sub( /\(.*/, "", name );
    parm = substr( $3, index( $3, "(" ) );
    gsub( /[a-z],/, "value,", parm );
    sub( /[a-z]\)/, "value)", parm );
    print "c$pragma aux", name, "parm" parm;
    print "\tinteger function", $3;
    print "\t" name " = " substr( $0, index( $0, ")" ) + 1 );
    print "\tend"
}
