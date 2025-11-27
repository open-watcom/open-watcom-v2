[ BLOCK <BLDRULE> test ]
#=======================
 [ INCLUDE "<OWROOT>/build/deftesth.ctl" ]
 [ IFDEF <BLD64BIT> x64 ]
  set TESTARG=<TESTARG> host=386
  [ INCLUDE "<OWROOT>/build/deftesth.ctl" ]
 [ ENDIF ]
