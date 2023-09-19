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

#ifndef LIBFTP_TEST_HELPERS_HPP
#define LIBFTP_TEST_HELPERS_HPP

#include <string>
#include <string_view>
#include <optional>
#include <ftp/replies.hpp>
#include <ftp/reply.hpp>

namespace ftp::test
{

void check_reply(const ftp::replies & replies, std::string_view expected, std::optional<bool> positive = std::nullopt);

void check_reply(const ftp::reply & reply, std::string_view expected);

void check_reply(const std::optional<ftp::reply> & reply, std::string_view expected);

void check_last_reply(const ftp::replies & replies, std::string_view expected);

bool is_windows_platform();

template<typename ...Strings>
std::string CRLF(const std::string & str, Strings && ...strs)
{
    return (str + ... + (std::string("\r\n") + std::forward<Strings>(strs)));
}

} // namespace ftp::test
#endif //LIBFTP_TEST_HELPERS_HPP
