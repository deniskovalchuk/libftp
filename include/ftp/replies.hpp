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

#ifndef LIBFTP_REPLIES_HPP
#define LIBFTP_REPLIES_HPP

#include <ftp/reply.hpp>
#include <vector>

namespace ftp
{

class replies
{
public:
    replies();

    void append(const reply & reply);

    [[nodiscard]] bool is_positive() const;

    [[nodiscard]] const std::string & get_status_string() const;

    [[nodiscard]] std::vector<reply>::const_iterator begin() const;

    [[nodiscard]] std::vector<reply>::const_iterator end() const;

    [[nodiscard]] const std::vector<reply> & get_list() const;

private:
    bool is_positive_;
    std::string status_string_;
    std::vector<reply> replies_;
};

} // namespace ftp
#endif //LIBFTP_REPLIES_HPP
