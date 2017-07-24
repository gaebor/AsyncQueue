CPPFLAGS+=-O2 -std=c++11 -pthread -Wall
CPP=g++

default: test

dir:
	mkdir -p bin
    
asyncqueue: dir bin/libasyncqueue.a

%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) -Iinc $< -o $@

bin/libasyncqueue.a: src/Clock.o src/Event.o src/Exception.o
	$(AR) rcs $@ $^

test: bin/test
bin/test: asyncqueue src/Test.o
	$(CPP) $(CPPFLAGS) -Lbin -o $@ src/Test.o -lasyncqueue 
    
clean:
	rm -f src/*.o bin/libasyncqueue.a bin/test

run: test
	./bin/test -h
	./bin/test -r 15 -s 100000
	./bin/test -r 12 -s 100000 -f
