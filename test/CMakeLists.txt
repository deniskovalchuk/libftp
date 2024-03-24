cmake_minimum_required(VERSION 3.14)
project(libftp-tests LANGUAGES CXX)

find_package(Boost 1.67.0 COMPONENTS filesystem REQUIRED)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(sources
        ascii_istream.cpp
        ascii_ostream.cpp
        client.cpp
        file_list_reply.cpp
        file_size_reply.cpp
        net_utils.cpp
        replies.cpp
        reply.cpp
        test_helpers.cpp
        test_helpers.hpp
        utils.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(ftp_tests ${sources})

target_link_libraries(ftp_tests
        PRIVATE
            ftp::ftp
            Boost::boost
            Boost::filesystem
            GTest::gtest_main
            GTest::gmock_main)

add_test(ftp_tests ftp_tests)