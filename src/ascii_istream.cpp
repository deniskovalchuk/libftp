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

#include <ftp/detail/ascii_istream.hpp>

namespace ftp::detail
{

ascii_istream::ascii_istream(input_stream & src, std::size_t buf_size)
    : src_(src),
      need_linefeed_(false),
      skip_linefeed_(false),
      internal_size_(0),
      internal_pos_(0)
{
    internal_.resize(buf_size);
}

std::size_t ascii_istream::read(char *buf, std::size_t size)
{
    std::size_t pos = 0;

    if (pos < size)
    {
        // Handle unfinished business.
        if (need_linefeed_)
        {
            append_char(buf, pos, '\n');
            need_linefeed_ = false;
        }
    }

    while (pos < size)
    {
        if (internal_pos_ >= internal_size_)
        {
            internal_size_ = src_.read(internal_.data(), internal_.size());
            internal_pos_ = 0;

            if (!internal_size_)
                break;
        }

        while (internal_pos_ < internal_size_)
        {
            char ch = internal_[internal_pos_];
            internal_pos_++;

            if (ch == '\r')
            {
                append_crlf(buf, pos, size);
                skip_linefeed_ = true;
            }
            else if (ch == '\n')
            {
                if (!skip_linefeed_)
                    append_crlf(buf, pos, size);
                skip_linefeed_ = false;
            }
            else
            {
                append_char(buf, pos, ch);
                skip_linefeed_ = false;
            }

            if (pos >= size)
                break;
        }
    }

    return pos;
}

void ascii_istream::append_crlf(char *buf, std::size_t & pos, std::size_t size)
{
    buf[pos] = '\r';
    pos++;

    if (pos >= size)
    {
        need_linefeed_ = true;
    }
    else
    {
        buf[pos] = '\n';
        pos++;
    }
}

void ascii_istream::append_char(char *buf, std::size_t & pos, char ch)
{
    buf[pos] = ch;
    pos++;
}

} // namespace ftp::detail
