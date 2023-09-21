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

#include <gtest/gtest.h>
#include <ftp/file_size_reply.hpp>

namespace
{

TEST(file_size_reply, construct)
{
    {
        ftp::file_size_reply reply({230, "230 0"}, 0);
        EXPECT_EQ(230, reply.get_code());
        EXPECT_EQ("230 0", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
        EXPECT_TRUE(reply.get_size().has_value());
        EXPECT_EQ(0, reply.get_size().value());
    }

    {
        ftp::file_size_reply reply({230, "230 4267539"}, 4267539);
        EXPECT_EQ(230, reply.get_code());
        EXPECT_EQ("230 4267539", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
        EXPECT_TRUE(reply.get_size().has_value());
        EXPECT_EQ(4267539, reply.get_size().value());
    }

    {
        ftp::file_size_reply reply({550, "550 /file is not retrievable."}, std::nullopt);
        EXPECT_EQ(550, reply.get_code());
        EXPECT_EQ("550 /file is not retrievable.", reply.get_status_string());
        EXPECT_FALSE(reply.is_positive());
        EXPECT_FALSE(reply.get_size().has_value());
    }
}

} // namespace
