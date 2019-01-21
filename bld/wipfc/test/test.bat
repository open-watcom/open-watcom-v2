set cmdwipfc=wipfc
if not "%1" == "" set cmdwipfc=%1
call env.bat
echo Test pos1
%cmdwipfc% -q -i .\pos\pos1.ipf
echo Test pos2
%cmdwipfc% -q -i .\pos\pos2.ipf
echo Test pos3
%cmdwipfc% -q -i .\pos\pos3.ipf
echo Test pos4
%cmdwipfc% -q -i .\pos\pos4.ipf
echo Test pos5
%cmdwipfc% -q -i .\pos\pos5.ipf
echo Test pos6
%cmdwipfc% -q -i .\pos\pos6.ipf
echo Test pos7
%cmdwipfc% -q -i .\pos\pos7.ipf
echo Test pos8
%cmdwipfc% -q -i .\pos\pos8.ipf
echo Test pos9
%cmdwipfc% -q -i .\pos\pos9.ipf
echo Test pos10
%cmdwipfc% -q -i .\pos\pos10.ipf
echo Test neg1
%cmdwipfc% -q -i .\neg\neg1.ipf
echo Test neg2
%cmdwipfc% -q -i .\neg\neg2.ipf
echo Test neg3
%cmdwipfc% -q -i .\neg\neg3.ipf
echo Test neg4
%cmdwipfc% -q -i .\neg\neg4.ipf
echo Test neg5
%cmdwipfc% -q -i .\neg\neg5.ipf
echo Test neg6
%cmdwipfc% -q -i .\neg\neg6.ipf
echo Test neg7
%cmdwipfc% -q -i .\neg\neg7.ipf
echo Test neg8
%cmdwipfc% -q -i .\neg\neg8.ipf
echo Test neg9
%cmdwipfc% -q -i .\neg\neg9.ipf
echo Test neg10
%cmdwipfc% -q -i .\neg\neg10.ipf
