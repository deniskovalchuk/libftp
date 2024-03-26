/*
 * MIT License
 *
 * Copyright (c) 2022 Denis Kovalchuk
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

#include <gtest/gtest.h>
#include <ftp/replies.hpp>
#include "test_utils.hpp"

namespace
{

using namespace ftp::test;

TEST(replies, append)
{
    {
        ftp::replies replies;
        replies.append(ftp::reply(120, "120 Service ready in 2 minutes."));
        check_reply(replies, "120 Service ready in 2 minutes.", true);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(220, "220 FTP server is ready."));
        check_reply(replies, "220 FTP server is ready.", true);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(331, "331 Username ok, send password."));
        check_reply(replies, "331 Username ok, send password.", true);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(425, "425 Can't open data connection."));
        check_reply(replies, "425 Can't open data connection.", false);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(532, "532 Need account for storing files."));
        check_reply(replies, "532 Need account for storing files.", false);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(331, "331 Username ok, send password."));
        replies.append(ftp::reply(530, "530 Authentication failed."));

        check_reply(replies, CRLF("331 Username ok, send password.",
                                  "530 Authentication failed."), false);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(229, "229 Entering extended passive mode (|||1234|)."));
        replies.append(ftp::reply(125, "125 Data connection already open. Transfer starting."));
        replies.append(ftp::reply(226, "226 Transfer complete."));

        check_reply(replies, CRLF("229 Entering extended passive mode (|||1234|).",
                                  "125 Data connection already open. Transfer starting.",
                                  "226 Transfer complete."), true);
    }

    {
        ftp::replies replies;
        replies.append(ftp::reply(220, "220 FTP server is ready."));
        check_reply(replies, "220 FTP server is ready.", true);

        replies.append(ftp::reply(532, "532 Need account for storing files."));
        check_reply(replies, CRLF("220 FTP server is ready.",
                                  "532 Need account for storing files."), false);
    }
}

} // namespace
