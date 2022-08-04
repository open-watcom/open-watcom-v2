mathtest_name = result

!include testenv.mif

test : .symbolic
    set MATHTEST_LOG_FILE=$(log_file)
    $(noecho)%make full_set_result

clean: .symbolic
    @%make echo_clean
