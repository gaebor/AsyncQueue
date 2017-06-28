RD /Q /S bin
mkdir bin

set "CFLAGS=%CFLAGS% /O2 /EHsc"

cl %CFLAGS% /MT /c /Iinc src\Event.cpp /Fo:bin\EventMT.obj
lib /OUT:bin\AsyncQueue.lib bin\EventMT.obj

cl %CFLAGS% /MD /c /Iinc src\Event.cpp /Fo:bin\EventMD.obj
link /dll /OUT:bin\AsyncQueue.dll bin\EventMD.obj
