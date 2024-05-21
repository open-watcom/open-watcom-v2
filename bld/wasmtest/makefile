tree_depth = 2

wasmtest_name = result

!include testenv.mif

test : .SYMBOLIC
    set WASMTEST_LOG_FILE=$(log_file)
    $(noecho)%make full_set_result

clean: .SYMBOLIC
    @%make echo_clean
