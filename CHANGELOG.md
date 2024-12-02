# Changelog

## v1.4.0

### Breaking changes

- **Dropped support for Boost version < 1.70.0.**

### New

- **Added support for creating the shared library on Windows.**
- Improvements in the build system.

## v1.3.0

### New
- Supported library installation via `CMake`.
- Minor improvements.

## v1.2.0

### New
- Added `ftp::client::get_file_modified_time()` method. See `MDTM` command in [RFC3659](doc/RFC3659.txt).
- Minor improvements.

## v1.1.0

### New

- Added `ftp::ssl::create_context()` function. It provides a new convenient way to create the `ftp::ssl::context_ptr` object.

## v1.0.0

This is the first stable major release. There are no differences from v0.5.1.

## v0.5.1

### Fixed

- Fixed compilation errors on g++ 13.2.1 and libstdc++ 6.0.32.

## v0.5.0

### New

- **Added support for FTP over TLS/SSL (FTPS).** See [RFC 2228](doc/RFC2228.txt) and [RFC 4217](doc/RFC4217.txt).
- Minor improvements.

## v0.4.1

### Fixed

- Fixed compilation errors on gcc 13+.

## v0.4.0

### New

- Added support for CMake install target.

## v0.3.3

- Updated to [pyftpdlib](https://github.com/giampaolo/pyftpdlib) 1.5.9.

## v0.3.2

### New

- `ftp::client`:
  - Deleted move constructors.
  - Internal improvements.

## v0.3.1

### Fixed

- Added missing includes in `<ftp/ftp.hpp>`:
  - `#include <ftp/file_list_reply.hpp>`
  - `#include <ftp/file_size_reply.hpp>`

## v0.3.0

### New

- **Added support for IPv6 protocol.** See [RFC 2428](doc/RFC2428.txt).
- `ftp::reply`:
  - Added a default constructor.
  - Added `ftp::reply::is_negative()` method.
  - Added `ftp::reply::is_intermediate()` method.
- `ftp::file_size_reply`:
    - Added a default constructor.

## v0.2.0

### Breaking changes

- `ftp::client::get_file_list()` now returns `ftp::file_list_reply`, which provides the parsed list of files.
- `ftp::client::get_file_size()` now returns `ftp::file_size_reply`, which provides the parsed size.
- `ftp::replies::get_list()` now returns `std::vector` instead of `std::list`.
- `ftp::replies::get_list()` renamed to `ftp::replies::get_replies()`.

### New

- Added `ftp::observer::on_connected()`.

## v0.1.0

Initial release.