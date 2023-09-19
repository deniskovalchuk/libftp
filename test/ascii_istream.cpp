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
#include <string>
#include <tuple>
#include <array>
#include <utility>
#include <ftp/detail/ascii_istream.hpp>
#include <ftp/stream/istream_adapter.hpp>

namespace
{

/*
 * tuple
 *   pair<string, string> - tested value and expected value.
 *   size_t - internal stream buffer size.
 *   size_t - read block size.
 */
class ascii_istream : public testing::TestWithParam<std::tuple<std::pair<std::string, std::string>,
                                                               std::size_t,
                                                               std::size_t>>
{
};

INSTANTIATE_TEST_SUITE_P(main_dataset, ascii_istream,
                         testing::Combine(
                             testing::Values(std::make_pair("", ""),
                                             std::make_pair("content", "content"),
                                             // LFCR.
                                             std::make_pair("\n\r", "\r\n\r\n"),
                                             // CR.
                                             std::make_pair("\r", "\r\n"),
                                             std::make_pair("\r\r", "\r\n\r\n"),
                                             std::make_pair("\r\r\r", "\r\n\r\n\r\n"),
                                             std::make_pair("\rcontent", "\r\ncontent"),
                                             std::make_pair("con\rtent", "con\r\ntent"),
                                             std::make_pair("content\r", "content\r\n"),
                                             std::make_pair("\rcon\r\rtent\r", "\r\ncon\r\n\r\ntent\r\n"),
                                             // LF.
                                             std::make_pair("\n", "\r\n"),
                                             std::make_pair("\n\n", "\r\n\r\n"),
                                             std::make_pair("\n\n\n", "\r\n\r\n\r\n"),
                                             std::make_pair("\ncontent", "\r\ncontent"),
                                             std::make_pair("con\ntent", "con\r\ntent"),
                                             std::make_pair("content\n", "content\r\n"),
                                             std::make_pair("\ncon\n\ntent\n", "\r\ncon\r\n\r\ntent\r\n"),
                                             // CRLF.
                                             std::make_pair("\r\n", "\r\n"),
                                             std::make_pair("\r\n\r\n", "\r\n\r\n"),
                                             std::make_pair("\r\n\r\n\r\n", "\r\n\r\n\r\n"),
                                             std::make_pair("\r\ncontent", "\r\ncontent"),
                                             std::make_pair("con\r\ntent", "con\r\ntent"),
                                             std::make_pair("content\r\n", "content\r\n"),
                                             std::make_pair("\r\ncon\r\n\r\ntent\r\n", "\r\ncon\r\n\r\ntent\r\n"),
                                             // Rotate CR.
                                             std::make_pair("\r\n\n", "\r\n\r\n"),
                                             std::make_pair("\n\r\n", "\r\n\r\n"),
                                             std::make_pair("\n\n\r", "\r\n\r\n\r\n"),
                                             // Rotate LF.
                                             std::make_pair("\n\r\r", "\r\n\r\n\r\n"),
                                             std::make_pair("\r\n\r", "\r\n\r\n"),
                                             std::make_pair("\r\r\n", "\r\n\r\n"),
                                             // Rotate CRLF.
                                             std::make_pair("\r\n\r\r", "\r\n\r\n\r\n"),
                                             std::make_pair("\r\r\n\r", "\r\n\r\n\r\n"),
                                             std::make_pair("\r\r\r\n", "\r\n\r\n\r\n"),
                                             std::make_pair("\r\n\n\n", "\r\n\r\n\r\n"),
                                             std::make_pair("\n\r\n\n", "\r\n\r\n\r\n"),
                                             std::make_pair("\n\n\r\n", "\r\n\r\n\r\n"),
                                             // Mixed EOLs.
                                             std::make_pair("\rcon\ntent\r\n", "\r\ncon\r\ntent\r\n"),
                                             std::make_pair("\r\rc\n\r\r\n\ro\r\n\r\n\n\rn\nte\rnt\n",
                                                            "\r\n\r\nc\r\n\r\n\r\n\r\no\r\n\r\n\r\n\r\nn\r\nte\r\nnt\r\n")),
                             testing::Values(4),
                             testing::Values(4)));

INSTANTIATE_TEST_SUITE_P(sizes_dataset, ascii_istream,
                         testing::Combine(
                             testing::Values(std::make_pair("\r\rc\n\r\r\n\ro\r\n\r\n\n\rn\nte\rnt\n",
                                                            "\r\n\r\nc\r\n\r\n\r\n\r\no\r\n\r\n\r\n\r\nn\r\nte\r\nnt\r\n")),
                             testing::Values(1, 4, 8, 64),
                             testing::Values(1, 4, 8, 64)));

TEST_P(ascii_istream, read)
{
    auto [data, buf_size, block_size] = GetParam();
    auto [content, expected] = data;

    std::istringstream iss(content);
    ftp::istream_adapter adapter(iss);
    ftp::detail::ascii_istream stream(adapter, buf_size);

    std::array<char, 512> buf = {};
    std::size_t total_read = 0;
    std::size_t size;

    while ((size = stream.read(buf.data() + total_read, block_size)) > 0)
    {
        total_read += size;
    }

    buf[total_read] = '\0';

    ASSERT_EQ(expected.size(), total_read);
    ASSERT_EQ(expected, std::string(buf.data()));
}

} // namespace
