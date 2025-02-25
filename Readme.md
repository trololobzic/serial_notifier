# Serial Notifier
A small C++98 project used Microsoft Foundation Classes (MFC) library for Windows of a tray utility that notifies the user about plugged...

![serial_notifier0](https://github.com/user-attachments/assets/bfb65c8b-4b7c-4f84-abe6-82a77dd7afe1) 

...and unplugged serial ports.

![serial_notifier1](https://github.com/user-attachments/assets/20290a35-a277-441c-a8b3-313a493691a3)

## About
The application can automatically start when the system starts and monitor connections and disconnections of serial ports. If you use interface converters such as USB - RS232, USB - RS485 or other devices that create new COM ports in your system, the application will notify you of the name of the newly connected device. Now you do not need to run the "Device Manager" to find out which serial port was created. All you need to do is place the executable file in the right place and run it.

## Backward compatibility
The application does not use modern C++ standards such as C++11 and above, because the developer's goal was to maintain backward compatibility with legacy operating systems such as Windows XP and Windows 7. These systems are currently not supported, but are still used in industrial computers to this day.
Unfortunately, the developer has not found a way to correctly compile C++11 and above code for Windows XP at this time, so the C++98 standard was chosen to write the application.

## Building
Building the project requires the following software to be installed:
- Windows (at least Windows XP)
- CMake (at least version 3.13)
- Microsoft Visual Studio (at least "Visual Studio 9 2008")

After cloning the project you should execute next commands:
```
cd serial_notifier\build
cmake.exe ..
cmake.exe --build . --config Release
```
Possible you should use full path to your installed CMake, for example ```"c:\Program Files\CMake\bin\cmake.exe"```.
You may use "-G" key for set Visual Studio version directly.
Possible variants for cmake "-G" key your can use:
```
Visual Studio 16 2019
Visual Studio 15 2017
Visual Studio 14 2015
Visual Studio 12 2013
Visual Studio 11 2012
Visual Studio 10 2010
Visual Studio 9 2008
```


