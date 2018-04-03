# AsyncQueue
C++ portable implementation of a multi-threaded, multi-producer, multi-consumer FIFO queue.

## build
GNU `make`, or `nmake` on Visual C++.

## test
The (n)make target `run` tests for every scenario.
See `aqtest -h` for details.

## usage
Link against the `asyncqueue` static library and enable C++11 in your code.
Include the `AsyncQueue.h` header. The queue is actually a template class, header library only, 
but the exceptions and event handling are in the precompiled library.
