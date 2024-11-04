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

#include <gtest/gtest.h>
#include <ftp/file_modified_time_reply.hpp>

namespace
{

TEST(file_modified_time_reply, construct)
{
    {
        ftp::file_modified_time_reply reply({213, "213 20241104170749"});
        EXPECT_EQ(213, reply.get_code());
        EXPECT_EQ("213 20241104170749", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());

        const std::optional<ftp::datetime> & datetime = reply.get_datetime();
        ASSERT_TRUE(datetime.has_value());
        EXPECT_EQ(2024, datetime.value().year);
        EXPECT_EQ(11, datetime.value().month);
        EXPECT_EQ(4, datetime.value().day);
        EXPECT_EQ(17, datetime.value().hour);
        EXPECT_EQ(7, datetime.value().minute);
        EXPECT_EQ(49, datetime.value().second);
        EXPECT_EQ(0, datetime.value().fractions);
    }

    {
        ftp::file_modified_time_reply reply({213, "213 19980615100045.014"});
        EXPECT_EQ(213, reply.get_code());
        EXPECT_EQ("213 19980615100045.014", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());

        const std::optional<ftp::datetime> & datetime = reply.get_datetime();
        ASSERT_TRUE(datetime.has_value());
        EXPECT_EQ(1998, datetime.value().year);
        EXPECT_EQ(6, datetime.value().month);
        EXPECT_EQ(15, datetime.value().day);
        EXPECT_EQ(10, datetime.value().hour);
        EXPECT_EQ(0, datetime.value().minute);
        EXPECT_EQ(45, datetime.value().second);
        EXPECT_EQ(14, datetime.value().fractions);
    }

    {
        ftp::file_modified_time_reply reply({213, "213 123456789"});
        EXPECT_EQ(213, reply.get_code());
        EXPECT_EQ("213 123456789", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
        EXPECT_FALSE(reply.get_datetime().has_value());
    }

    {
        ftp::file_modified_time_reply reply({213, "213 *9980705132316"});
        EXPECT_EQ(213, reply.get_code());
        EXPECT_EQ("213 *9980705132316", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
        EXPECT_FALSE(reply.get_datetime().has_value());
    }

    {
        ftp::file_modified_time_reply reply({213, "213 19980705132316.*"});
        EXPECT_EQ(213, reply.get_code());
        EXPECT_EQ("213 19980705132316.*", reply.get_status_string());
        EXPECT_TRUE(reply.is_positive());
        EXPECT_FALSE(reply.get_datetime().has_value());
    }

    {
        ftp::file_modified_time_reply reply({550, "550 /file is not retrievable."});
        EXPECT_EQ(550, reply.get_code());
        EXPECT_EQ("550 /file is not retrievable.", reply.get_status_string());
        EXPECT_FALSE(reply.is_positive());
        EXPECT_FALSE(reply.get_datetime().has_value());
    }
}

} // namespace
