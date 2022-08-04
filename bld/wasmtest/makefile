tree_depth = 2

wasmtest_name = result

!include testenv.mif

test : .symbolic
    set WASMTEST_LOG_FILE=$(log_file)
    $(noecho)%make full_set_result

clean: .symbolic
    @%make echo_clean
