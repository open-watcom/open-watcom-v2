@rem TESTSTRM.BAT -- run a test stream
@rem
@rem 91/07/06 -- J.W.Welch      -- defined
@rem
@rm -rf testin
@rm -rf testout
md testin
md testout
wpack -ptestin\ %1
testin\testrun
