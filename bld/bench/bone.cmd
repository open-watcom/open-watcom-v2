@echo %verbose% off
if     .%1 == .                  for %%C in (echo false goto:done) do %%C %0: usage %0 result_file [extra_opts]
if not .%3 == .                  for %%C in (echo false goto:done) do %%C %0: usage %0 result_file [extra_opts]
if not .%result_file% == .       for %%C in (echo false goto:done) do %%C %0: result_file environment variable already set
if not .%extra_opts% == .        for %%C in (echo false goto:done) do %%C %0: extra_opts environment variable already set
if not .%BENCH_REPORT_FILE% == . for %%C in (echo false goto:done) do %%C %0: BENCH_REPORT_FILE environment variable already set
set result_file=%1
set extra_opts=%2
set BENCH_REPORT_FILE=%result_file%
pmake -d watcom  -h clean
pmake -d watcom support .and -h extra_w_opts=%extra_opts%
pmake -d watcom  -h extra_w_opts=%extra_opts%
set result_file=
set extra_opts=
set BENCH_REPORT_FILE=
:done
