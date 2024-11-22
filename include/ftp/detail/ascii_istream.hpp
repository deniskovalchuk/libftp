/*
 * MIT License
 *
 * Copyright (c) 2023 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LIBFTP_ASCII_ISTREAM_HPP
#define LIBFTP_ASCII_ISTREAM_HPP

#include <ftp/detail/export_internal.hpp>
#include <ftp/stream/input_stream.hpp>
#include <vector>

namespace ftp::detail
{

class FTP_EXPORT_INTERNAL ascii_istream : public input_stream
{
public:
    explicit ascii_istream(input_stream & src, std::size_t buf_size = 8192);

    std::size_t read(char *buf, std::size_t size) override;

private:
    void append_crlf(char *buf, std::size_t & pos, std::size_t size);

    void append_char(char *buf, std::size_t & pos, char ch);

    input_stream & src_;
    bool need_linefeed_;
    bool skip_linefeed_;
    std::size_t internal_size_;
    std::size_t internal_pos_;
    std::vector<char> internal_;
};

} // namespace ftp::detail
#endif //LIBFTP_ASCII_ISTREAM_HPP
