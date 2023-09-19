cmake_minimum_required(VERSION 3.14)
project(cmdline
        VERSION 0.0.1
        DESCRIPTION "Command-line FTP client"
        LANGUAGES CXX)

add_executable(cmdline
        src/cmdline_exception.hpp
        src/cmdline_interface.cpp
        src/cmdline_interface.hpp
        src/command.hpp
        src/command_handler.cpp
        src/command_handler.hpp
        src/command_parser.cpp
        src/command_parser.hpp
        src/main.cpp
        src/transfer_callback.cpp
        src/transfer_callback.hpp
        src/utils.cpp
        src/utils.hpp)

target_link_libraries(cmdline PRIVATE ftp::ftp Boost::boost)
target_compile_features(cmdline PRIVATE cxx_std_17)

if (LIBFTP_BUILD_TEST)
    add_subdirectory(test)
endif()