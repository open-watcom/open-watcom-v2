@echo off
rem Note: This script needs WGET tool in order to fetch updates from Internet

rem Get most recent OpenGL Extension headers from SGI (only win32 specific headers)
wget http://oss.sgi.com/projects/ogl-sample/ABI/glext.h -O include\GL\glext.h
wget http://oss.sgi.com/projects/ogl-sample/ABI/wglext.h -O include\GL\wglext.h
