CPPFLAGS=/O2 /Ot /DNDEBUG /EHsc /Iinc /W4 /nologo /MD

OUT_DIR=bin

!if [if not exist $(OUT_DIR) mkdir $(OUT_DIR)]
!endif

default: test

asyncqueue: $(OUT_DIR)\asyncqueue.lib

{src\}.cpp{src\}.obj:
	cl /c $(CPPFLAGS) /Fo:$@ $< 

$(OUT_DIR)\asyncqueue.lib: src\Clock.obj src\Event.obj src\Exception.obj inc\aq\AsyncQueue.h
	lib /OUT:$@ src\Clock.obj src\Event.obj src\Exception.obj

test: $(OUT_DIR)/test.exe

$(OUT_DIR)\test.exe: asyncqueue src\Test.obj
	cl $(CPPFLAGS) src\Test.obj /link $(OUT_DIR)\asyncqueue.lib /OUT:$@

clean:
	del /Q /S $(OUT_DIR)\test.exe $(OUT_DIR)\asyncqueue.lib src\*.obj

run: test
	$(OUT_DIR)\test.exe -h
	$(OUT_DIR)\test.exe -r 15 -n 20 -l 2000 -b 0
	$(OUT_DIR)\test.exe -r 15 -n 20 -l 2000 -b 1
	$(OUT_DIR)\test.exe -r 14 -n 20 -l 2000 -b 2
	$(OUT_DIR)\test.exe -r 15 -n 20 -l 2000 -b 3
	
	$(OUT_DIR)\test.exe -r 12 -n 20 -l 20000 -b 0 -f
	$(OUT_DIR)\test.exe -r 12 -n 20 -l 20000 -b 1 -f
	$(OUT_DIR)\test.exe -r 12 -n 20 -l 20000 -b 2 -f
	$(OUT_DIR)\test.exe -r 12 -n 20 -l 20000 -b 3 -f
