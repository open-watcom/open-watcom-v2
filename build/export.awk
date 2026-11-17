# Generate unique export list.
# Each output line is enclosed in single quotes.

!seen[$0]++ {
    print "'" $0 "'"
}
