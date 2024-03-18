cmake_minimum_required(VERSION 3.14)
project(libftp
    VERSION 0.3.3
    DESCRIPTION "FTP client library"
    LANGUAGES CXX)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

option(LIBFTP_BUILD_TEST "Build tests" ${is_top_level})
option(LIBFTP_BUILD_EXAMPLE "Build examples" ${is_top_level})
option(LIBFTP_BUILD_CMDLINE_CLIENT "Build the command-line FTP client application" ${is_top_level})

find_package(Boost 1.67.0 REQUIRED)

set(sources
        include/ftp/detail/ascii_istream.hpp
        include/ftp/detail/ascii_ostream.hpp
        include/ftp/detail/binary_istream.hpp
        include/ftp/detail/binary_ostream.hpp
        include/ftp/detail/control_connection.hpp
        include/ftp/detail/data_connection.hpp
        include/ftp/detail/net_context.hpp
        include/ftp/detail/net_utils.hpp
        include/ftp/detail/utils.hpp
        include/ftp/stream/input_stream.hpp
        include/ftp/stream/istream_adapter.hpp
        include/ftp/stream/ostream_adapter.hpp
        include/ftp/stream/output_stream.hpp
        include/ftp/client.hpp
        include/ftp/file_list_reply.hpp
        include/ftp/file_size_reply.hpp
        include/ftp/ftp.hpp
        include/ftp/ftp_exception.hpp
        include/ftp/observer.hpp
        include/ftp/replies.hpp
        include/ftp/reply.hpp
        include/ftp/transfer_callback.hpp
        include/ftp/transfer_mode.hpp
        include/ftp/transfer_type.hpp
        src/ascii_istream.cpp
        src/ascii_ostream.cpp
        src/binary_istream.cpp
        src/binary_ostream.cpp
        src/client.cpp
        src/control_connection.cpp
        src/data_connection.cpp
        src/file_list_reply.cpp
        src/file_size_reply.cpp
        src/istream_adapter.cpp
        src/net_context.cpp
        src/net_utils.cpp
        src/ostream_adapter.cpp
        src/replies.cpp
        src/reply.cpp
        src/utils.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_library(ftp ${sources})
add_library(ftp::ftp ALIAS ftp)

target_link_libraries(ftp PUBLIC Boost::boost)
target_include_directories(ftp PUBLIC include)
target_compile_features(ftp PUBLIC cxx_std_17)

if (LIBFTP_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (LIBFTP_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if (LIBFTP_BUILD_CMDLINE_CLIENT)
    add_subdirectory(app/cmdline)
endif()

include(GNUInstallDirs)
install(
    TARGETS ftp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    DIRECTORY include/ftp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
