cmake_minimum_required (VERSION 3.1)
project (AsyncQueue)

set (CMAKE_CXX_STANDARD 11)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(aqtest src/Test.cpp)

if(UNIX)
    add_compile_options("-pthread")
    target_link_libraries(aqtest pthread)
endif(UNIX)
    
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

add_test(test_seq_Refuse        aqtest -r 1 -n 20 -l 2000 -b 3)
add_test(test_one_by_one_Refuse aqtest -r 2 -n 20 -l 2000 -b 3)
add_test(test_parallel_Refuse   aqtest -r 4 -n 20 -l 2000 -b 3)
add_test(test_parallel2_Refuse  aqtest -r 8 -n 20 -l 2000 -b 3)
	
add_test(test_parallel_None_F   aqtest -r 4 -n 20 -l 2000 -b 0 -f)
add_test(test_parallel2_None_F  aqtest -r 8 -n 20 -l 2000 -b 0 -f)

add_test(test_parallel_Drop_F   aqtest -r 4 -n 20 -l 2000 -b 1 -f)
add_test(test_parallel2_Drop_F  aqtest -r 8 -n 20 -l 2000 -b 1 -f)

add_test(test_parallel_Wait_F   aqtest -r 4 -n 20 -l 2000 -b 2 -f)
add_test(test_parallel2_Wait_F  aqtest -r 8 -n 20 -l 2000 -b 2 -f)

add_test(test_parallel_Refuse_F   aqtest -r 4 -n 20 -l 2000 -b 3 -f)
add_test(test_parallel2_Refuse_F  aqtest -r 8 -n 20 -l 2000 -b 3 -f)
