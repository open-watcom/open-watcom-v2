@rem TESTSTRM.BAT -- run a test stream
@rem
@rem 91/07/06 -- J.W.Welch      -- defined
@rem
@fastdel testin
@fastdel testout
md testin
md testout
wpack -ptestin\ %1
testin\testrun
