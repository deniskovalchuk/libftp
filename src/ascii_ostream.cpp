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

#include <ftp/detail/ascii_ostream.hpp>

namespace ftp::detail
{

ascii_ostream::ascii_ostream(output_stream & dst, std::size_t hint_size)
    : dst_(dst),
      prev_cr_(false)
{
    internal_.reserve(hint_size);
}

void ascii_ostream::write(char *buf, std::size_t size)
{
    internal_.clear();

    for (std::size_t i = 0; i < size; i++)
    {
        char ch = buf[i];

        if (ch == '\r')
        {
            if (prev_cr_)
                internal_.push_back(ch);
            else
                prev_cr_ = true;
        }
        else if (ch == '\n')
        {
            internal_.push_back('\n');
            prev_cr_ = false;
        }
        else
        {
            if (prev_cr_)
                internal_.push_back('\r');
            internal_.push_back(ch);
            prev_cr_ = false;
        }
    }

    dst_.write(internal_.data(), internal_.size());
}

void ascii_ostream::flush()
{
    if (prev_cr_)
    {
        char buf = '\r';
        dst_.write(&buf, 1);
        prev_cr_ = false;
    }

    dst_.flush();
}

} // namespace ftp::detail
