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

#include <ftp/file_size_reply.hpp>
#include <ftp/detail/utils.hpp>

namespace ftp
{

using namespace ftp::detail;

file_size_reply::file_size_reply()
    : ftp::reply()
{}

file_size_reply::file_size_reply(const reply & reply)
    : ftp::reply(reply)
{
    size_ = parse_size(reply);
}

const std::optional<std::uint64_t> & file_size_reply::get_size() const
{
    return size_;
}

std::optional<std::uint64_t> file_size_reply::parse_size(const reply & reply)
{
    /* 213 SIZE_IN_BYTES */
    if (reply.get_code() != 213)
    {
        return std::nullopt;
    }

    std::string_view status_string = reply.get_status_string();

    /* Code, space, and at least one character. */
    if (status_string.size() < 5)
    {
        return std::nullopt;
    }

    std::uint64_t size;
    if (utils::try_parse_uint64(status_string.substr(4), size))
    {
        return size;
    }
    else
    {
        return std::nullopt;
    }
}

} // namespace ftp
