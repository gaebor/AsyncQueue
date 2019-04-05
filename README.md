# AsyncQueue
C++ portable implementation of a multi-threaded, multi-producer, multi-consumer FIFO queue.

## build & test
[cmake](https://cmake.org/) and [ctest](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest)

Requires C++11 compliant compiler.

## usage
All classes are __header only__, just include what you need.
* `Event`: a semaphore for signaling threads
  * see `event` test
* `Clock`: a nice clock for measuring runtime
* `AsyncQueue`: the queue
  * see `aqtest`
