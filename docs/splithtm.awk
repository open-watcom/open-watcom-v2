BEGIN {
    file = ""
}

/<H. ID=\"(.*)\"> (.*) <\/H.>/ {
    if( length( file ) > 0 ) {
        print "</BODY>" >> file
        print "</HTML>" >> file
        close( file )
    }
    file = $0
    gsub( /<H. ID=\"/, "", file )
    gsub( /\"> (.*) <\/H.>/, "", file )
    file = file ".htm"
    if( length( dir ) > 0 ) {
        file = dir "/" file
    }
    title = $0
    gsub( /<H. ID=\"(.*)\"> /, "", title )
    gsub( / <\/H.>/, "", title )
    print "<HTML>" > file
    print "<HEAD><TITLE>" title "</TITLE></HEAD>" >> file
    print "<BODY>" >> file
    print "<H1>" title "</H1>" >> file
    next
}

length( file ) > 0 {
    line = $0
    while( match( line, /<A HREF=\"\#([^>]*)\">/ ) ) {
        temp = substr( line, 0, RSTART + 8 )
        temp = temp substr( line, RSTART + 10, RLENGTH - 12 ) ".htm"
        temp = temp substr( line, RSTART + RLENGTH - 2 )
        line = temp
    }
    print line >> file
}
