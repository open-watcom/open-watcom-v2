# add header to contents file
BEGIN {
    print ":Base " file ">main"
    print ":Title " title
    print ":Index " title " Help=" file
    print ""
    print "1 " title
}
#
# join line with "=" on the end of line with next one
#
/.*=$/ {
    prev_line = $0
    getline
    $0 = prev_line $0
}
#
# remove topics links for first level
#
/^1 / { 
    if( multi != 1 ) {
        next
    }
}
#
# resequence topics links levels, each to one level up
#
{
    if( multi != 1 ) {
        sub( /^2 /, "1 " )
        sub( /^3 /, "2 " )
        sub( /^4 /, "3 " )
        sub( /^5 /, "4 " )
        sub( /^6 /, "5 " )
    }
    print $0
}
