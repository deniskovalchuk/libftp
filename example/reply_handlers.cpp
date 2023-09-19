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

#include <string_view>
#include <iostream>
#include <ftp/ftp_exception.hpp>
#include "reply_handlers.hpp"

void handle_reply(const ftp::replies & replies)
{
    const std::string & status_string = replies.get_status_string();

    if (replies.is_positive())
    {
        std::cout << status_string << std::endl;
    }
    else
    {
        throw ftp::ftp_exception(status_string);
    }
}

void handle_reply(const ftp::reply & reply)
{
    const std::string & status_string = reply.get_status_string();

    if (reply.is_positive())
    {
        std::cout << status_string << std::endl;
    }
    else
    {
        throw ftp::ftp_exception(status_string);
    }
}

void handle_reply(const std::optional<ftp::reply> & reply)
{
    if (reply)
    {
        const std::string & status_string = reply->get_status_string();

        if (reply->is_positive())
        {
            std::cout << status_string << std::endl;
        }
        else
        {
            throw ftp::ftp_exception(status_string);
        }
    }
}
