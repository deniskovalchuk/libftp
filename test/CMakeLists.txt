cmake_minimum_required(VERSION 3.14)
project(libftp-tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(sources
    ascii_istream.cpp
    ascii_ostream.cpp
    client.cpp
    file_list_reply.cpp
    file_modified_time_reply.cpp
    file_size_reply.cpp
    net_utils.cpp
    replies.cpp
    reply.cpp
    test_server.hpp
    test_utils.cpp
    test_utils.hpp
    utils.cpp)

add_executable(ftp_tests ${sources})

find_package(Boost 1.88.0 COMPONENTS filesystem process REQUIRED CONFIG)

target_link_libraries(ftp_tests
    PRIVATE
        ftp::ftp
        Boost::boost
        Boost::filesystem
        Boost::process
        GTest::gtest_main
        GTest::gmock_main)

add_test(ftp_tests ftp_tests)