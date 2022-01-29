add_executable(testArithmetic_exe arithmetic_main.cpp bitheap.cpp tiling.cpp)
target_link_libraries(testArithmetic_exe hint Boost::unit_test_framework)
add_test(testArithmetic testArithmetic_exe)
