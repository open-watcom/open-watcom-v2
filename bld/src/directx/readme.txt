Open Watcom DirectX Sample Code
-------------------------------

These folders contain DirectX sample code.
The examples require DirectX 9.0 SDK or newer to compile.

The latest DirectX SDK can be downloaded from
http://www.microsoft.com/directx

At the time of this writing DirectX9.0b is the latest version.

You should add the path to where you installed the SDK
to the front of the LIB and INCLUDE environment variables,
before the Open Watcom paths, eg.

set LIB=c:\dxsdk9\lib;c:\ow\lib386\nt;c:\ow\lib386
set INCLUDE=c:\dxsdk9\include;c:\ow\h\nt;c:\ow\h

This is to override the older DirectX headers and libraries
supplied with Open Watcom.

It is recommended to install the SDK into an 8.3 conforming
folder name.

Miscellaneous notes on the samples follow:

In most cases, pressing the Esc key will exit the demos.

Direct3D
--------
You can use the keys
A                 - toggle animation.
R                 - reset cube to start position.
UP/DOWN
LEFT/RIGHT
PGUP/PGDN         - control the object's XYZ position.
NUMPAD7/4/8/5/9/6 - control the object's pitch/yaw/roll.
NUMPAD+/-         - control the object's size

Inside the code there are toggles for running fullscreen or
windowed, with or without a Z-buffer.

DirectSound
-----------
The sample will likely not work properly with "exotic"
files (like mp3) encapsulated inside WAV files.

DirectShow
----------
The sample will likely not work properly with "exotic"
files (like mpeg2) encapsulated inside AVI files.

Inside the code there is a toggle to allow DirectShow to
manage its own window instead of using the application
provided one.

DirectInput
-----------
The sample uses the most conservative locking of resources
to be maximally Windows friendly.

DirectPlay
----------
There's currently no DirectPlay example. Using BSD sockets
instead is highly recommended.

DirectMusic
-----------
Looking for contributors!


Jim Shaw - March 16, 2004
