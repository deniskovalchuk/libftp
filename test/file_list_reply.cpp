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
#include <gmock/gmock.h>
#include <ftp/file_list_reply.hpp>
#include "test_utils.hpp"

namespace
{

using namespace ftp::test;
using testing::ElementsAre;

TEST(file_list_reply, construct)
{
    ftp::replies replies;
    replies.append(ftp::reply(229, "229 Entering extended passive mode (|||1234|)."));
    replies.append(ftp::reply(125, "125 Data connection already open. Transfer starting."));
    replies.append(ftp::reply(226, "226 Transfer complete."));

    {
        std::string file_list_str;
        ftp::file_list_reply reply(replies, file_list_str);
        EXPECT_EQ(file_list_str, reply.get_file_list_str());
        EXPECT_THAT(reply.get_file_list(), ElementsAre());
    }

    {
        std::string file_list_str = "file";
        ftp::file_list_reply reply(replies, file_list_str);
        EXPECT_EQ(file_list_str, reply.get_file_list_str());
        EXPECT_THAT(reply.get_file_list(), ElementsAre("file"));
    }

    // Test CRLF newlines.
    {
        std::string file_list_str = CRLF("directory",
                                         "file1",
                                         "file2");

        ftp::file_list_reply reply(replies, file_list_str);
        EXPECT_EQ(file_list_str, reply.get_file_list_str());
        EXPECT_THAT(reply.get_file_list(), ElementsAre("directory", "file1", "file2"));
    }

    // Test LF newlines.
    {
        std::string file_list_str = LF("directory",
                                       "file1",
                                       "file2");

        ftp::file_list_reply reply(replies, file_list_str);
        EXPECT_EQ(file_list_str, reply.get_file_list_str());
        EXPECT_THAT(reply.get_file_list(), ElementsAre("directory", "file1", "file2"));
    }
}

} // namespace
