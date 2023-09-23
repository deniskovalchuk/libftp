# Changelog

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