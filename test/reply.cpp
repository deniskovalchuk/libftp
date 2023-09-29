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
#include <ftp/reply.hpp>

namespace
{

TEST(reply, construct)
{
    {
        ftp::reply reply;
        EXPECT_EQ(ftp::reply::unspecified, reply.get_code());
        EXPECT_EQ("", reply.get_status_string());
        EXPECT_FALSE(reply.is_positive());
    }

    {
        ftp::reply reply(120, "120 Service ready in 2 minutes.");
        EXPECT_EQ(120, reply.get_code());
        EXPECT_EQ("120 Service ready in 2 minutes.", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
    }

    {
        ftp::reply reply(200, "220 FTP server is ready.");
        EXPECT_EQ(200, reply.get_code());
        EXPECT_EQ("220 FTP server is ready.", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
    }

    {
        ftp::reply reply(331, "331 Username ok, send password.");
        EXPECT_EQ(331, reply.get_code());
        EXPECT_EQ("331 Username ok, send password.", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
    }

    {
        ftp::reply reply(425, "425 Can't open data connection.");
        EXPECT_EQ(425, reply.get_code());
        EXPECT_EQ("425 Can't open data connection.", reply.get_status_string());
        EXPECT_FALSE(reply.is_positive());
    }

    {
        ftp::reply reply(532, "532 Need account for storing files.");
        EXPECT_EQ(532, reply.get_code());
        EXPECT_EQ("532 Need account for storing files.", reply.get_status_string());
        EXPECT_FALSE(reply.is_positive());
    }
}

} // namespace
