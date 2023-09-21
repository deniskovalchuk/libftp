/*
 * MIT License
 *
 * Copyright (c) 2020 Denis Kovalchuk
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

#ifndef LIBFTP_UTILS_HPP
#define LIBFTP_UTILS_HPP

#include <string>
#include <vector>
#include <string_view>
#include <boost/format.hpp>

namespace ftp::detail::utils
{

template<typename ...Args>
std::string format(const std::string & fmt, Args && ...args)
{
    boost::format f(fmt);
    f = (f % ... % std::forward<Args>(args));
    return f.str();
}

std::vector<std::string> split_string(std::string_view str, char del);

bool try_parse_uint16(std::string_view str, std::uint16_t & result);

bool try_parse_uint64(std::string_view str, std::uint64_t & result);

} // namespace ftp::detail::utils
#endif //LIBFTP_UTILS_HPP
