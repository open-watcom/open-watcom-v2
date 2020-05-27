/<h. id=\"(.*)\"> (.*) <\/h.>/ {
    newlevel = $0
    gsub( /<h/, "", newlevel )
    gsub( / id=\"(.*)\" (.*) <\/h.>/, "", newlevel )
    file = $0
    gsub( /<h. id=\"/, "", file )
    gsub( /\"> (.*) <\/h.>/, "", file )
    title = $0
    gsub( /<h. id=\"(.*)\"> /, "", title )
    gsub( / <\/h.>/, "", title )
    gsub( /\&nbsp\;/, "", title )
    print "HLP_" toupper( file ) " = " file ".htm"
}
