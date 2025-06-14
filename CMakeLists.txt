cmake_minimum_required(VERSION 3.14)
project(libftp
        VERSION 1.5.0
        DESCRIPTION "FTP/FTPS client library"
        LANGUAGES CXX)

include(GNUInstallDirs)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

option(LIBFTP_BUILD_TEST "Build tests" ${is_top_level})
option(LIBFTP_BUILD_EXAMPLE "Build examples" ${is_top_level})
option(LIBFTP_BUILD_CMDLINE_CLIENT "Build the command-line FTP client application" ${is_top_level})

set(sources
    include/ftp/detail/ascii_istream.hpp
    include/ftp/detail/ascii_ostream.hpp
    include/ftp/detail/binary_istream.hpp
    include/ftp/detail/binary_ostream.hpp
    include/ftp/detail/control_connection.hpp
    include/ftp/detail/data_connection.hpp
    include/ftp/detail/export_internal.hpp
    include/ftp/detail/net_context.hpp
    include/ftp/detail/net_utils.hpp
    include/ftp/detail/socket.hpp
    include/ftp/detail/socket_base.hpp
    include/ftp/detail/ssl_socket.hpp
    include/ftp/detail/utils.hpp
    include/ftp/stream/input_stream.hpp
    include/ftp/stream/istream_adapter.hpp
    include/ftp/stream/ostream_adapter.hpp
    include/ftp/stream/output_stream.hpp
    include/ftp/client.hpp
    include/ftp/datetime.hpp
    include/ftp/file_list_reply.hpp
    include/ftp/file_modified_time_reply.hpp
    include/ftp/file_size_reply.hpp
    include/ftp/ftp.hpp
    include/ftp/ftp_exception.hpp
    include/ftp/observer.hpp
    include/ftp/replies.hpp
    include/ftp/reply.hpp
    include/ftp/ssl.hpp
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
    src/file_modified_time_reply.cpp
    src/file_size_reply.cpp
    src/istream_adapter.cpp
    src/net_context.cpp
    src/net_utils.cpp
    src/ostream_adapter.cpp
    src/replies.cpp
    src/reply.cpp
    src/socket.cpp
    src/ssl.cpp
    src/ssl_socket.cpp
    src/utils.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Use 'MATCHES' to match all Clang-based compilers.
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(
        /W4
        # C4100: 'identifier' : unreferenced formal parameter
        /wd4100
        # C4251: 'identifier' : class 'type' needs to have dll-interface to be
        #        used by clients of class 'type2'
        /wd4251
        # C4275: non DLL-interface classkey 'identifier' used as base for
        #        DLL-interface classkey 'identifier'
        /wd4275)
endif()

add_library(ftp ${sources})
add_library(ftp::ftp ALIAS ftp)

find_package(Boost 1.88.0 REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)

target_link_libraries(ftp PUBLIC Boost::boost)
target_link_libraries(ftp PUBLIC OpenSSL::SSL)

if (WIN32)
    target_link_libraries(ftp PUBLIC ws2_32)
endif()

target_include_directories(ftp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(ftp PUBLIC cxx_std_17)

include(GenerateExportHeader)
generate_export_header(ftp EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/ftp/export.hpp)

if (LIBFTP_BUILD_TEST)
    enable_testing()
    target_compile_definitions(ftp PRIVATE LIBFTP_FTP_EXPORT_INTERNAL)
    add_subdirectory(test)
endif()

if (LIBFTP_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if (LIBFTP_BUILD_CMDLINE_CLIENT)
    target_compile_definitions(ftp PRIVATE LIBFTP_FTP_EXPORT_INTERNAL)
    add_subdirectory(app/cmdline)
endif()

install(TARGETS ftp
        EXPORT ftp-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ftp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ftp-targets
        FILE ftp-targets.cmake
        NAMESPACE ftp::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ftp)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "ftp-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/ftp-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ftp-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ftp)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ftp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ftp-config-version.cmake"
    DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ftp)