cmake_minimum_required(VERSION 3.14)
project(cmdline-tests)

add_executable(cmdline_tests
        parser.cpp
        utils.cpp
        ../src/command_parser.cpp
        ../src/utils.cpp)

target_link_libraries(cmdline_tests
        PRIVATE
            Boost::boost
            GTest::gtest_main
            GTest::gmock_main)

target_include_directories(cmdline_tests PRIVATE ../src)
target_compile_features(cmdline_tests PRIVATE cxx_std_17)

add_test(cmdline_tests cmdline_tests)