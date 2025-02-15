# Serial Notifier
A small C++98/03 project used Microsoft Foundation Classes (MFC) library for Windows of a tray utility that notifies the user about plugged and unplugged serial ports.

## Building
Building the project requires the following software to be installed:
- Windows (at least Windows XP)
- CMake (at least version 3.5)
- Microsoft Visual Studio (at least "Visual Studio 9 2008")

After cloning the project you should execute next commands:
```
cd serial_notifier\build
cmake.exe .. -G "Visual Studio 9 2008"
cmake.exe --build . --config Release
```
Possible you should use full path to your installed CMake, for example ```"c:\Program Files\CMake\bin\cmake.exe"```.
Possible variants for cmake key -G your can use:
```
Visual Studio 16 2019
Visual Studio 15 2017
Visual Studio 14 2015
Visual Studio 12 2013
Visual Studio 11 2012
Visual Studio 10 2010
Visual Studio 9 2008
```
