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

#ifndef LIBFTP_FILE_MODIFIED_TIME_REPLY_HPP
#define LIBFTP_FILE_MODIFIED_TIME_REPLY_HPP

#include <ftp/reply.hpp>
#include <ftp/datetime.hpp>
#include <optional>

namespace ftp
{

class file_modified_time_reply : public reply
{
public:
    file_modified_time_reply();

    explicit file_modified_time_reply(const reply & reply);

    [[nodiscard]] const std::optional<datetime> & get_datetime() const;

private:
    static std::optional<datetime> parse_datetime(const reply & reply);

    std::optional<datetime> datetime_;
};

} // namespace ftp
#endif //LIBFTP_FILE_MODIFIED_TIME_REPLY_HPP
