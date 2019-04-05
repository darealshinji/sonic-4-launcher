This is a C++ rewrite of the Sonic 4 (Episode 1) game launcher for Windows.
The original launcher was written in Java but the game itself uses no Java.
I made this to avoid a pointless Java runtime dependency and to make it easier
to run Sonic 4 on Linux using Proton.

The original Java launcher can be extracted and examined with the
following Unix shell command: `tail -c+178689 SonicLauncher.exe > SonicLauncher.jar`

How to build
------------
Checkout the submodule with `git submodule init && git submodule update` and open `SonicLauncher.sln`.

License
-------
Most of the stuff I've written is MIT licensed, check the source file headers for details.
This project is using the FLTK library which is LGPLv2 licensed but with certain exceptions,
see fltk/COPYING for details. The image files are copyrighted and not under a free license.

Problems
--------
There are some key binding inconsistencies between US and non-US layouts.
