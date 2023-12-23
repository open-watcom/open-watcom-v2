tree_depth = 2

clibtest_name = result

!include testenv.mif

test : .symbolic
    set CLIBTEST_LOG_FILE=$(log_file)
    $(noecho)%make full_set_result

clean: .symbolic
    @%make echo_clean
