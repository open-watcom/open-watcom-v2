@rem TESTDARC.BAT -- de-archive a test stream
@rem
@rem 93/02/17 -- J.W.Welch      -- defined from TESTSTRM.BAT
@rem
@fastdel testin
@fastdel testout
md testin
md testout
wpack -ptestin\ %1
