CPPFLAGS+=-O2 -std=c++11 -pthread -Wall
CPP=g++

OUT_DIR=bin
dummy_build_folder := $(shell mkdir -p $(OUT_DIR))

default: test

asyncqueue: $(OUT_DIR)/libasyncqueue.a

%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) -Iinc $< -o $@

$(OUT_DIR)/libasyncqueue.a: src/Clock.o src/Event.o src/Exception.o inc/aq/AsyncQueue.h
	$(AR) rcs $@ $^

test: $(OUT_DIR)/test
$(OUT_DIR)/test: asyncqueue src/Test.o
	$(CPP) $(CPPFLAGS) -Lbin -o $@ src/Test.o -lasyncqueue 
    
clean:
	rm -f src/*.o $(OUT_DIR)/libasyncqueue.a $(OUT_DIR)/test

run: test
	./$(OUT_DIR)/test -h
	./$(OUT_DIR)/test -r 15
	./$(OUT_DIR)/test -r 12 -f
