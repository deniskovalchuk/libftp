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

#include <ftp/file_list_reply.hpp>
#include <sstream>

namespace ftp
{

file_list_reply::file_list_reply()
    : ftp::replies()
{}

file_list_reply::file_list_reply(const replies & replies, const std::string & file_list_str)
    : ftp::replies(replies),
      file_list_str_(file_list_str)
{
    file_list_ = parse_file_list(file_list_str);
}

const std::string & file_list_reply::get_file_list_str() const
{
    return file_list_str_;
}

const std::vector<std::string> & file_list_reply::get_file_list() const
{
    return file_list_;
}

std::vector<std::string> file_list_reply::parse_file_list(const std::string & file_list_str)
{
    std::vector<std::string> file_list;

    std::istringstream iss(file_list_str);
    std::string line;
    while (std::getline(iss, line))
    {
        /* Handle CRLF. */
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        file_list.push_back(line);
    }

    return file_list;
}

} // namespace ftp
