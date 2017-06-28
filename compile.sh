#/bin/bash
CFLAGS="-O3 $CFLAGS"
rm -Rf bin
mkdir bin
g++ -c $CFLAGS -Iinc src/Event.cpp -o bin/Event.o
ar rcs bin/libAsyncQueue.a bin/Event.o
