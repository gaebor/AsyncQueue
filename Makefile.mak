CPPFLAGS=/O2 /Ot /DNDEBUG /EHsc /Iinc /W4 /nologo /MD

default: test

dir:
	IF not exist bin ( MD bin)
    
asyncqueue: dir bin\asyncqueue.lib

{src\}.cpp{src\}.obj:
	cl /c $(CPPFLAGS) /Fo:$@ $< 

bin\asyncqueue.lib: src\Clock.obj src\Event.obj src\Exception.obj
	lib /OUT:$@ $**

test: bin/test.exe

bin\test.exe: asyncqueue src\Test.obj
	cl $(CPPFLAGS) src\Test.obj /link bin\asyncqueue.lib /OUT:$@

clean:
	del /Q /S bin\test.exe bin\asyncqueue.lib src\*.obj

run: test
	bin\test.exe -h
	bin\test.exe -r 15
    bin\test.exe -r 12 -f
