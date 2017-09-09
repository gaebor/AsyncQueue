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
	$(OUT_DIR)\test.exe -r 15
    $(OUT_DIR)\test.exe -r 12 -f
