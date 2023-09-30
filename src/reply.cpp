/*
 * MIT License
 *
 * Copyright (c) 2022 Denis Kovalchuk
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

#include <ftp/reply.hpp>

namespace ftp
{

reply::reply()
    : code_(unspecified)
{
}

reply::reply(std::uint16_t code, std::string_view status_string)
    : code_(code),
      status_string_(status_string)
{
}

bool reply::is_positive() const
{
    return code_ != unspecified && code_ < 400;
}

bool reply::is_negative() const
{
    return code_ != unspecified && code_ >= 400;
}

bool reply::is_intermediate() const
{
    return code_ != unspecified && code_ >= 300 && code_ < 400;
}

std::uint16_t reply::get_code() const
{
    return code_;
}

const std::string & reply::get_status_string() const
{
    return status_string_;
}

} // namespace ftp
