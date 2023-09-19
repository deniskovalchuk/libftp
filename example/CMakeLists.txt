cmake_minimum_required(VERSION 3.14)
project(libftp-examples LANGUAGES CXX)

function(create_example name)
    add_executable(${name}
            ${name}.cpp
            reply_handlers.cpp
            reply_handlers.hpp)

    target_link_libraries(${name} PRIVATE ftp::ftp)
endfunction()

create_example(download_file)
create_example(upload_file)
create_example(logger)
create_example(file_transfer_progress)