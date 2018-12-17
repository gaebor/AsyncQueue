# AsyncQueue
C++ portable implementation of a multi-threaded, multi-producer, multi-consumer FIFO queue.

## build & test
[cmake](https://cmake.org/) and [ctest](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest)

## usage
Link against the `asyncqueue` static library and enable C++11 in your code.
Include the `AsyncQueue.h` header. The queue is actually a template class, header library only, 
but the exceptions and event handling are in the precompiled library.
