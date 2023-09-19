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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "command_parser.hpp"
#include "command.hpp"
#include "cmdline_exception.hpp"

namespace
{

using testing::ElementsAre;

TEST(parse_command, open)
{
    auto result = parse_command("open ftp.cisco.com 21");
    ASSERT_EQ(result.first, command::open);
    ASSERT_THAT(result.second, ElementsAre("ftp.cisco.com", "21"));
}

TEST(parse_command, mode)
{
    auto result = parse_command("mode");
    ASSERT_EQ(result.first, command::mode);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, active)
{
    auto result = parse_command("active");
    ASSERT_EQ(result.first, command::active);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, passive)
{
    auto result = parse_command("passive");
    ASSERT_EQ(result.first, command::passive);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, user)
{
    auto result = parse_command("user anonymous");
    ASSERT_EQ(result.first, command::user);
    ASSERT_THAT(result.second, ElementsAre("anonymous"));
}

TEST(parse_command, cd)
{
    auto result = parse_command("cd ../home/dir/");
    ASSERT_EQ(result.first, command::cd);
    ASSERT_THAT(result.second, ElementsAre("../home/dir/"));
}

TEST(parse_command, cdup)
{
    auto result = parse_command("cdup");
    ASSERT_EQ(result.first, command::cdup);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, ls)
{
    auto result = parse_command("ls .");
    ASSERT_EQ(result.first, command::ls);
    ASSERT_THAT(result.second, ElementsAre("."));
}

TEST(parse_command, put)
{
    auto result = parse_command("put local_file remote_file");
    ASSERT_EQ(result.first, command::put);
    ASSERT_THAT(result.second, ElementsAre("local_file", "remote_file"));
}

TEST(parse_command, get)
{
    auto result = parse_command("get remote_file local_file");
    ASSERT_EQ(result.first, command::get);
    ASSERT_THAT(result.second, ElementsAre("remote_file", "local_file"));
}

TEST(parse_command, rename)
{
    auto result = parse_command("rename /public/file /public/file2");
    ASSERT_EQ(result.first, command::rename);
    ASSERT_THAT(result.second, ElementsAre("/public/file", "/public/file2"));
}

TEST(parse_command, pwd)
{
    auto result = parse_command("pwd");
    ASSERT_EQ(result.first, command::pwd);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, mkdir)
{
    auto result = parse_command("mkdir dir");
    ASSERT_EQ(result.first, command::mkdir);
    ASSERT_THAT(result.second, ElementsAre("dir"));
}

TEST(parse_command, rmdir)
{
    auto result = parse_command("rmdir dir");
    ASSERT_EQ(result.first, command::rmdir);
    ASSERT_THAT(result.second, ElementsAre("dir"));
}

TEST(parse_command, del)
{
    auto result = parse_command("del file");
    ASSERT_EQ(result.first, command::del);
    ASSERT_THAT(result.second, ElementsAre("file"));
}

TEST(parse_command, stat)
{
    auto result = parse_command("stat");
    ASSERT_EQ(result.first, command::stat);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, syst)
{
    auto result = parse_command("syst");
    ASSERT_EQ(result.first, command::syst);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, type)
{
    auto result = parse_command("type");
    ASSERT_EQ(result.first, command::type);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, binary)
{
    auto result = parse_command("binary");
    ASSERT_EQ(result.first, command::binary);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, ascii)
{
    auto result = parse_command("ascii");
    ASSERT_EQ(result.first, command::ascii);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, size)
{
    auto result = parse_command("size filename");
    ASSERT_EQ(result.first, command::size);
    ASSERT_THAT(result.second, ElementsAre("filename"));
}

TEST(parse_command, noop)
{
    auto result = parse_command("noop");
    ASSERT_EQ(result.first, command::noop);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, rhelp)
{
    auto result = parse_command("rhelp");
    ASSERT_EQ(result.first, command::rhelp);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, logout)
{
    auto result = parse_command("logout");
    ASSERT_EQ(result.first, command::logout);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, close)
{
    auto result = parse_command("close");
    ASSERT_EQ(result.first, command::close);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, help)
{
    auto result = parse_command("help");
    ASSERT_EQ(result.first, command::help);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, exit)
{
    auto result = parse_command("exit");
    ASSERT_EQ(result.first, command::exit);
    ASSERT_THAT(result.second, ElementsAre());
}

TEST(parse_command, invalid_command)
{
    ASSERT_THROW({
        try
        {
            parse_command("invalid");
        }
        catch (const cmdline_exception & ex)
        {
            ASSERT_STREQ("Invalid command.", ex.what());
            throw;
        }
    }, cmdline_exception);
}

TEST(parse_command, case_insensitive)
{
    auto result = parse_command("STAT");
    EXPECT_EQ(result.first, command::stat);
    EXPECT_THAT(result.second, ElementsAre());

    result = parse_command("SysT");
    EXPECT_EQ(result.first, command::syst);
    EXPECT_THAT(result.second, ElementsAre());

    result = parse_command("BiNaRy");
    EXPECT_EQ(result.first, command::binary);
    EXPECT_THAT(result.second, ElementsAre());
}

TEST(parse_command, quoted_parameters)
{
    auto result = parse_command("get file space");
    EXPECT_EQ(result.first, command::get);
    EXPECT_THAT(result.second, ElementsAre("file", "space"));

    result = parse_command(R"(get "file space")");
    EXPECT_EQ(result.first, command::get);
    EXPECT_THAT(result.second, ElementsAre("file space"));

    result = parse_command(R"(get "/home/space /file" "/downloads/space /file")");
    EXPECT_EQ(result.first, command::get);
    EXPECT_THAT(result.second, ElementsAre("/home/space /file", "/downloads/space /file"));
}

} // namespace
