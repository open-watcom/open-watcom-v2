[ BLOCK <BLDRULE> test ]
#=======================
 [ INCLUDE "<OWROOT>/build/deftesth.ctl" ]
 [ IFDEF <BLD_HOST> NT UNIX ]
  set TESTARG=<TESTARG> host=386
  [ INCLUDE "<OWROOT>/build/deftesth.ctl" ]
 [ ENDIF ]
