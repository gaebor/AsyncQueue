cmake_minimum_required (VERSION 2.6)
project (AsyncQueue)

include_directories(inc)

if(UNIX)
add_compile_options("-std=c++11" "-pthread")
SET( CMAKE_EXE_LINKER_FLAGS  "-pthread" )
endif(UNIX)

add_executable(aqtest src/Test.cpp )

add_library(asyncqueue src/Clock.cpp src/Event.cpp src/Exception.cpp)

target_link_libraries (aqtest asyncqueue)

include(CTest)

add_test (testhelp aqtest -h)

add_test(test_seq_None        aqtest -r 1 -n 20 -l 2000 -b 0)
add_test(test_one_by_one_None aqtest -r 2 -n 20 -l 2000 -b 0)
add_test(test_parallel_None   aqtest -r 4 -n 20 -l 2000 -b 0)
add_test(test_parallel2_None  aqtest -r 8 -n 20 -l 2000 -b 0)

add_test(test_seq_Drop        aqtest -r 1 -n 20 -l 2000 -b 1)
add_test(test_one_by_one_Drop aqtest -r 2 -n 20 -l 2000 -b 1)
add_test(test_parallel_Drop   aqtest -r 4 -n 20 -l 2000 -b 1)
add_test(test_parallel2_Drop  aqtest -r 8 -n 20 -l 2000 -b 1)

add_test(test_one_by_one_Wait aqtest -r 2 -n 20 -l 2000 -b 2)
add_test(test_parallel_Wait   aqtest -r 4 -n 20 -l 2000 -b 2)
add_test(test_parallel2_Wait  aqtest -r 8 -n 20 -l 2000 -b 2)

add_test(test_one_by_one_Refuse aqtest -r 1 -n 20 -l 2000 -b 3)
add_test(test_one_by_one_Refuse aqtest -r 2 -n 20 -l 2000 -b 3)
add_test(test_parallel_Refuse   aqtest -r 4 -n 20 -l 2000 -b 3)
add_test(test_parallel2_Refuse  aqtest -r 8 -n 20 -l 2000 -b 3)
	
add_test(test_parallel_None_F   aqtest -r 4 -n 20 -l 2000 -b 0)
add_test(test_parallel2_None_F  aqtest -r 8 -n 20 -l 2000 -b 0)

add_test(test_parallel_Drop_F   aqtest -r 4 -n 20 -l 2000 -b 1)
add_test(test_parallel2_Drop_F  aqtest -r 8 -n 20 -l 2000 -b 1)

add_test(test_parallel_Wait_F   aqtest -r 4 -n 20 -l 2000 -b 2)
add_test(test_parallel2_Wait_F  aqtest -r 8 -n 20 -l 2000 -b 2)

add_test(test_parallel_Refuse_F   aqtest -r 4 -n 20 -l 2000 -b 3)
add_test(test_parallel2_Refuse_F  aqtest -r 8 -n 20 -l 2000 -b 3)
