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
#include <ftp/detail/utils.hpp>

namespace
{

using testing::ElementsAre;

TEST(utils, split_string)
{
    EXPECT_THAT(ftp::detail::utils::split_string("", ','),
                ElementsAre());

    EXPECT_THAT(ftp::detail::utils::split_string(",", ','),
                ElementsAre(""));

    EXPECT_THAT(ftp::detail::utils::split_string(",,,", ','),
                ElementsAre("", "", ""));

    EXPECT_THAT(ftp::detail::utils::split_string("abc", ','),
                ElementsAre("abc"));

    EXPECT_THAT(ftp::detail::utils::split_string("a,b", ','),
                ElementsAre("a", "b"));

    EXPECT_THAT(ftp::detail::utils::split_string("aa,bb,cc,", ','),
                ElementsAre("aa", "bb", "cc"));

    EXPECT_THAT(ftp::detail::utils::split_string("127,0,0,1,198,65", ','),
                ElementsAre("127", "0", "0", "1", "198", "65"));
}

TEST(utils, try_parse_uint8)
{
    std::uint8_t result;

    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("123abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("-1", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("256", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("1000", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint8("99999", result));

    EXPECT_TRUE(ftp::detail::utils::try_parse_uint8("0", result));
    EXPECT_EQ(0, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint8("21", result));
    EXPECT_EQ(21, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint8("101", result));
    EXPECT_EQ(101, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint8("255", result));
    EXPECT_EQ(255, result);
}

TEST(utils, try_parse_uint16)
{
    std::uint16_t result;

    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("123abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("-1", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("65536", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("65540", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint16("99999", result));

    EXPECT_TRUE(ftp::detail::utils::try_parse_uint16("0", result));
    EXPECT_EQ(0, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint16("21", result));
    EXPECT_EQ(21, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint16("2121", result));
    EXPECT_EQ(2121, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint16("65535", result));
    EXPECT_EQ(65535, result);
}

TEST(utils, try_parse_uint32)
{
    std::uint32_t result;

    EXPECT_FALSE(ftp::detail::utils::try_parse_uint32("", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint32("abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint32("123abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint32("-1", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint32("4294967296", result));

    EXPECT_TRUE(ftp::detail::utils::try_parse_uint32("0", result));
    EXPECT_EQ(0, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint32("47", result));
    EXPECT_EQ(47, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint32("123456789", result));
    EXPECT_EQ(123456789, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint32("4294967295", result));
    EXPECT_EQ(4294967295, result);
}

TEST(utils, try_parse_uint64)
{
    std::uint64_t result;

    EXPECT_FALSE(ftp::detail::utils::try_parse_uint64("", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint64("abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint64("123abc", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint64("-1", result));
    EXPECT_FALSE(ftp::detail::utils::try_parse_uint64("18446744073709551616", result));

    EXPECT_TRUE(ftp::detail::utils::try_parse_uint64("0", result));
    EXPECT_EQ(0, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint64("1", result));
    EXPECT_EQ(1, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint64("47", result));
    EXPECT_EQ(47, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint64("4294967295", result));
    EXPECT_EQ(4294967295, result);
    EXPECT_TRUE(ftp::detail::utils::try_parse_uint64("18446744073709551615", result));
    EXPECT_EQ(18446744073709551615u, result);
}

} // namespace
