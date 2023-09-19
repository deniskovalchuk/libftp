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

#include <ftp/replies.hpp>

namespace ftp
{

replies::replies()
    : is_positive_(false)
{
}

void replies::append(const reply & reply)
{
    if (replies_.empty())
    {
        is_positive_ = reply.is_positive();
        status_string_.append(reply.get_status_string());
    }
    else if (reply.is_positive())
    {
        /* Keep previous 'is_positive' state. */
        status_string_.append("\r\n");
        status_string_.append(reply.get_status_string());
    }
    else
    {
        is_positive_ = false;
        status_string_.append("\r\n");
        status_string_.append(reply.get_status_string());
    }

    replies_.push_back(reply);
}

bool replies::is_positive() const
{
    return is_positive_;
}

const std::string & replies::get_status_string() const
{
    return status_string_;
}

std::list<reply>::const_iterator replies::begin() const
{
    return replies_.cbegin();
}

std::list<reply>::const_iterator replies::end() const
{
    return replies_.cend();
}

const std::list<reply> & replies::get_list() const
{
    return replies_;
}

} // namespace ftp
