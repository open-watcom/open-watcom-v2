setenv BENCH_REPORT_FILE=d:\dev\bench\msvc.results
pmake -d msvc -h clean
cd support\msvc
wmake -h
cd ..\..
pmake -d msvc -h extra_m_opts=/G5
