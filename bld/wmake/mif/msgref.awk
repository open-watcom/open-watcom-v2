{
    print "/" $1 "/i\\"
    print $1
}

END {
    print "1,$d"
}

