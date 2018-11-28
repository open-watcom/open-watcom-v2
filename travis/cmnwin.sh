echo "set" >tmp.bat
echo "ENV1"
cmd "/c call tmp.bat"
echo "call \"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat\" amd64" >tmp.bat
echo "set" >>tmp.bat
echo "ENV2"
cmd "/c call tmp.bat"
