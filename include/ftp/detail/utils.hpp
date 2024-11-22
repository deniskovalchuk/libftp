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

#include <ftp/detail/export_internal.hpp>
#include <boost/format.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

namespace ftp::detail::utils
{

template<typename ...Args>
std::string format(const std::string & fmt, Args && ...args)
{
    boost::format f(fmt);
    f = (f % ... % std::forward<Args>(args));
    return f.str();
}

FTP_EXPORT_INTERNAL
std::vector<std::string> split_string(std::string_view str, char del);

FTP_EXPORT_INTERNAL
bool try_parse_uint8(std::string_view str, std::uint8_t & result);

FTP_EXPORT_INTERNAL
bool try_parse_uint16(std::string_view str, std::uint16_t & result);

FTP_EXPORT_INTERNAL
bool try_parse_uint32(std::string_view str, std::uint32_t & result);

FTP_EXPORT_INTERNAL
bool try_parse_uint64(std::string_view str, std::uint64_t & result);

} // namespace ftp::detail::utils
#endif //LIBFTP_UTILS_HPP
