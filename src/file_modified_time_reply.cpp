/*
 * MIT License
 *
 * Copyright (c) 2024 Denis Kovalchuk
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

#include <ftp/file_modified_time_reply.hpp>
#include <ftp/detail/utils.hpp>

namespace ftp
{

using namespace ftp::detail;

file_modified_time_reply::file_modified_time_reply()
    : ftp::reply()
{}

file_modified_time_reply::file_modified_time_reply(const reply & reply)
    : ftp::reply(reply)
{
    datetime_ = parse_datetime(reply);
}

const std::optional<datetime> & file_modified_time_reply::get_datetime() const
{
    return datetime_;
}

std::optional<datetime> file_modified_time_reply::parse_datetime(const reply & reply)
{
    /* 213 time-val */
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

    /* time-val = 14DIGIT [ "." 1*DIGIT ]
     *
     * The leading, mandatory, fourteen digits are to be interpreted as, in order from the leftmost,
     * four digits giving the year, with a range of 1000--9999,
     * two digits giving the month of the year, with a range of 01--12,
     * two digits giving the day of the month, with a range of 01--31,
     * two digits giving the hour of the day, with a range of 00--23,
     * two digits giving minutes past the hour, with a range of 00--59, and finally,
     * two digits giving seconds past the minute, with a range of 00--60.
     * ...
     * The optional digits, which are preceded by a period, give decimal fractions of a second.
     *
     * RFC 3659: https://datatracker.ietf.org/doc/html/rfc3659#section-2.3
     */

    static const std::size_t min_time_val_size = 14;
    static const std::size_t fractions_pos = min_time_val_size + 1;

    std::string_view time_val = status_string.substr(4);

    if (time_val.size() < min_time_val_size)
    {
        return std::nullopt;
    }

    datetime result;

    if (!utils::try_parse_uint16(time_val.substr(0, 4), result.year))
    {
        return std::nullopt;
    }

    if (!utils::try_parse_uint8(time_val.substr(4, 2), result.month))
    {
        return std::nullopt;
    }

    if (!utils::try_parse_uint8(time_val.substr(6, 2), result.day))
    {
        return std::nullopt;
    }

    if (!utils::try_parse_uint8(time_val.substr(8, 2), result.hour))
    {
        return std::nullopt;
    }

    if (!utils::try_parse_uint8(time_val.substr(10, 2), result.minute))
    {
        return std::nullopt;
    }

    if (!utils::try_parse_uint8(time_val.substr(12, 2), result.second))
    {
        return std::nullopt;
    }

    /* Are there any chars after the '.'? */
    if (time_val.size() > fractions_pos)
    {
        if (!utils::try_parse_uint32(time_val.substr(fractions_pos), result.fractions))
        {
            return std::nullopt;
        }
    }

    return result;
}

} // namespace ftp
