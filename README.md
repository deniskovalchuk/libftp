# libftp

[![C++](https://img.shields.io/badge/C++-17-blue)](https://en.cppreference.com/w/cpp/17)
[![License](https://img.shields.io/badge/License-MIT-blue)](LICENSE)

[![Actions Workflow Windows](https://github.com/deniskovalchuk/ftp-client/actions/workflows/windows.yml/badge.svg)](https://github.com/deniskovalchuk/ftp-client/actions/workflows/windows.yml)
[![Actions Workflow Linux](https://github.com/deniskovalchuk/ftp-client/actions/workflows/linux.yml/badge.svg)](https://github.com/deniskovalchuk/ftp-client/actions/workflows/linux.yml)
[![Actions Workflow macOS](https://github.com/deniskovalchuk/ftp-client/actions/workflows/macos.yml/badge.svg)](https://github.com/deniskovalchuk/ftp-client/actions/workflows/macos.yml)

[![Conan Center](https://img.shields.io/conan/v/libftp?label=ConanCenter&color=blue)](https://conan.io/center/recipes/libftp)

A cross-platform FTP client library based on [Boost.Asio](https://www.boost.org/doc/libs/1_84_0/doc/html/boost_asio.html).

## Table of contents

1. [Overview](#overview)
1. [Features](#features)
1. [Examples](#examples)
1. [Integration](#integration)
1. [Building](#building)
1. [References](#references)

## Overview

FTP (File Transfer Protocol) is a standard network protocol used to transfer files between a client and a server. FTP is
built on a client-server model and uses separate connections for transferring commands and files.

### Connections

- **Control connection** is a persistent connection used to transfer commands and replies between the client and the
server.
- **Data connection** is a temporary connection used to transfer files. The connection is only open for the duration of
the data transfer.

### Transfer modes

The transfer mode determines how the data connection is established.

- In **active mode**, the client uses the control connection to send the server the IP address and port number on which
the client accepts incoming data connections. The server then uses this information to open a data connection.
- In **passive mode**, the client uses the control connection to request from the server the IP address and port number
on which the server accepts incoming data connections. The client then uses this information to open a data connection.
This mode can be used in situations where the client cannot accept incoming connections (firewall, NAT).

### Transfer types

The transfer type determines how data is transferred. 

- **ASCII type** can be used to transfer ASCII files between systems with different newline representations. In this
case, the sending side converts the newlines from system style to `CRLF` style, and the receiving side performs the
reverse conversion.
- **Binary type** is used to transfer files byte by byte. This transfer type is used by default.

## Features

- Windows, Linux and macOS are supported.
- The main command set from [RFC 959](doc/RFC959.txt) is implemented.
- Supports active and passive transfer modes.
- Supports ASCII and binary transfer types.
- IPv4 and IPv6 support.
- Supports FTP and FTP over SSL/TLS (FTPS).

## Examples

Download the `README.TXT` file from [ftp.freebsd.org](https://download.freebsd.org/) and output its contents to `stdout`:

```c++
#include <iostream>
#include <sstream>

#include <ftp/ftp.hpp>

int main(int argc, char *argv[])
{
    ftp::client client;

    client.connect("ftp.freebsd.org", 21, "anonymous");

    std::ostringstream oss;

    client.download_file(ftp::ostream_adapter(oss), "pub/FreeBSD/README.TXT");

    std::cout << oss.str();

    client.disconnect();

    return 0;
}
```

See more examples in the [example](example) folder.

## Integration

This library can be integrated into a project via CMake's `FetchContent`, for example:

```cmake
cmake_minimum_required(VERSION 3.14)
project(application)

include(FetchContent)
FetchContent_Declare(
        libftp
        GIT_REPOSITORY https://github.com/deniskovalchuk/libftp.git
        GIT_TAG        v0.4.1)
FetchContent_MakeAvailable(libftp)

add_executable(application main.cpp)
target_link_libraries(application ftp::ftp)
```

## Building

### Prerequisites

- A C++17-compliant compiler
- CMake 3.14 or newer
- Boost 1.67.0 or newer
- OpenSSL
- Python3 (only for tests)

### Windows

Build and run tests:

```
tool/windows/build.ps1 [-BuildType Debug|Release] [-RunTest]
```

Clean the builds:

```
tool/windows/clean.ps1
```

### Linux/macOS

Build and run tests:

```
tool/unix/build.sh [--debug | --release] [--test]
```

Clean the builds:

```
tool/unix/clean.sh
```

### Custom environment

Build:

```bash
$ mkdir -p build
$ cd build
$ cmake ..
$ cmake --build .
```

To run tests, set the `LIBFTP_TEST_SERVER_PATH` environment variable to the path to the
[server.py](test/server/server.py) file:

```bash
$ export LIBFTP_TEST_SERVER_PATH="/path/to/server.py"
$ cd test
$ ctest -V
```

## References

- [RFC 959](doc/RFC959.txt) File Transfer Protocol (FTP). J. Postel, J. Reynolds. October 1985.
- [RFC 2228](doc/RFC2228.txt) FTP Security Extensions. October 1997.
- [RFC 2428](doc/RFC2428.txt) Extensions for IPv6, NAT, and Extended passive mode. September 1998.
- [RFC 3659](doc/RFC3659.txt) Extensions to FTP. P. Hethmon. March 2007.
- [RFC 4217](doc/RFC4217.txt) Securing FTP with TLS. October 2005.