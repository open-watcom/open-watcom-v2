/* testdll.cmd */

say "Registering the EZRXFUNC functions.  If the program halts when trying"
say "to call EZLoadFuncs, make sure the EZRXFUNC.DLL file is in your LIBPATH."
say ""

call RXFuncAdd 'EZLoadFuncs', 'EZRXFUNC', 'EZLoadFuncs'
call EZLoadFuncs

do 10
    say EZFunc1()
end

say EZFunc2( 'an argument string' )
say EZFunc3( 'another argument string' )

call EZDropFuncs
