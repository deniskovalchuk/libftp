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

#include <ftp/detail/utils.hpp>

namespace ftp::detail::utils
{

std::vector<std::string> split_string(std::string_view str, char del)
{
    std::string::size_type size = str.size();
    std::vector<std::string> result;
    std::string substr;

    for (std::string::size_type i = 0; i < size; i++)
    {
        char ch = str[i];

        if (ch == del)
        {
            result.push_back(substr);
            substr.clear();
        }
        else if (i == size - 1)
        {
            substr.push_back(ch);
            result.push_back(substr);
            substr.clear();
        }
        else
        {
            substr.push_back(ch);
        }
    }

    return result;
}

bool try_parse_uint8(std::string_view str, std::uint8_t & result)
{
    std::uint64_t value;

    if (!try_parse_uint64(str, value))
        return false;

    if (value > std::numeric_limits<std::uint8_t>::max())
        return false;

    result = static_cast<std::uint8_t>(value);
    return true;
}

bool try_parse_uint16(std::string_view str, std::uint16_t & result)
{
    std::uint64_t value;

    if (!try_parse_uint64(str, value))
        return false;

    if (value > std::numeric_limits<std::uint16_t>::max())
        return false;

    result = static_cast<std::uint16_t>(value);
    return true;
}

bool try_parse_uint32(std::string_view str, std::uint32_t & result)
{
    std::uint64_t value;

    if (!try_parse_uint64(str, value))
        return false;

    if (value > std::numeric_limits<std::uint32_t>::max())
        return false;

    result = static_cast<std::uint32_t>(value);
    return true;
}

bool try_parse_uint64(std::string_view str, std::uint64_t & result)
{
    if (str.empty())
        return false;

    const std::uint64_t max = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t value = 0;

    for (char ch : str)
    {
        if (ch < '0' || ch > '9')
            return false;

        std::uint64_t digit = ch - '0';

        if (value > max / 10)
            return false;

        value *= 10;

        if (value > max - digit)
            return false;

        value += digit;
    }

    result = value;
    return true;
}

} // namespace ftp::detail::utils
