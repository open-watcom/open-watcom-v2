@rem TESTARCH.BAT -- archive contents of .\testin directory
@rem
@rem 91\07\06 -- J.W.Welch      -- defined
@rem
@rm -f %1
@wpack -a %1 testin\*.*
