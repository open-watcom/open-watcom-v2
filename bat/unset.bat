command /c set | sed -e 's/=.*$/=/' -e 's/^/set /' > __unset.bat
echo set dosbox=%dosbox%>>__unset.bat
call __unset
erase __unset.bat
