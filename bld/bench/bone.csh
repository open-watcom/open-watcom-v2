proc runit( result_file extra_opts )
    setenv BENCH_REPORT_FILE $result_file
    pmake -d watcom  -h clean
    pmake -d watcom support .and -h
    pmake -d watcom  -h extra_w_opts=$extra_opts
end

runit $0 $1
