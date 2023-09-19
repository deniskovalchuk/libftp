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
#include <utility>
#include <sstream>
#include <ftp/detail/ascii_ostream.hpp>
#include <ftp/stream/ostream_adapter.hpp>

namespace
{

/*
 * tuple
 *   pair<string, string> - tested value and expected value.
 *   size_t - write block size.
 */
class ascii_ostream : public testing::TestWithParam<std::tuple<std::pair<std::string, std::string>,
                                                               std::size_t>>
{
};

INSTANTIATE_TEST_SUITE_P(main_dataset, ascii_ostream,
                         testing::Combine(
                             testing::Values(std::make_pair("", ""),
                                             std::make_pair("content", "content"),
                                             // LFCR.
                                             std::make_pair("\n\r", "\n\r"),
                                             // CR.
                                             std::make_pair("\r", "\r"),
                                             std::make_pair("\r\r", "\r\r"),
                                             std::make_pair("\r\r\r", "\r\r\r"),
                                             std::make_pair("\rcontent", "\rcontent"),
                                             std::make_pair("con\rtent", "con\rtent"),
                                             std::make_pair("content\r", "content\r"),
                                             std::make_pair("\rcon\r\rtent\r", "\rcon\r\rtent\r"),
                                             // LF.
                                             std::make_pair("\n", "\n"),
                                             std::make_pair("\n\n", "\n\n"),
                                             std::make_pair("\n\n\n", "\n\n\n"),
                                             std::make_pair("\ncontent", "\ncontent"),
                                             std::make_pair("con\ntent", "con\ntent"),
                                             std::make_pair("content\n", "content\n"),
                                             std::make_pair("\ncon\n\ntent\n", "\ncon\n\ntent\n"),
                                             // CRLF.
                                             std::make_pair("\r\n", "\n"),
                                             std::make_pair("\r\n\r\n", "\n\n"),
                                             std::make_pair("\r\n\r\n\r\n", "\n\n\n"),
                                             std::make_pair("\r\ncontent", "\ncontent"),
                                             std::make_pair("con\r\ntent", "con\ntent"),
                                             std::make_pair("content\r\n", "content\n"),
                                             std::make_pair("\r\ncon\r\n\r\ntent\r\n", "\ncon\n\ntent\n"),
                                             // Rotate CR.
                                             std::make_pair("\r\n\n", "\n\n"),
                                             std::make_pair("\n\r\n", "\n\n"),
                                             std::make_pair("\n\n\r", "\n\n\r"),
                                             // Rotate LF.
                                             std::make_pair("\n\r\r", "\n\r\r"),
                                             std::make_pair("\r\n\r", "\n\r"),
                                             std::make_pair("\r\r\n", "\r\n"),
                                             // Rotate CRLF.
                                             std::make_pair("\r\n\r\r", "\n\r\r"),
                                             std::make_pair("\r\r\n\r", "\r\n\r"),
                                             std::make_pair("\r\r\r\n", "\r\r\n"),
                                             std::make_pair("\r\n\n\n", "\n\n\n"),
                                             std::make_pair("\n\r\n\n", "\n\n\n"),
                                             std::make_pair("\n\n\r\n", "\n\n\n"),
                                             // Mixed EOLs.
                                             std::make_pair("\rcon\ntent\r\n", "\rcon\ntent\n"),
                                             std::make_pair("\r\n\rc\r\n\r\n\no\r\r\n\r\nn\r\nte\n\rnt\r\n",
                                                            "\n\rc\n\n\no\r\n\nn\nte\n\rnt\n")),
                             testing::Values(4)));

INSTANTIATE_TEST_SUITE_P(sizes_dataset, ascii_ostream,
                         testing::Combine(
                             testing::Values(std::make_pair("\r\n\rc\r\n\r\n\no\r\r\n\r\nn\r\nte\n\rnt\r\n",
                                                            "\n\rc\n\n\no\r\n\nn\nte\n\rnt\n")),
                             testing::Values(1, 4, 8, 64)));

TEST_P(ascii_ostream, write)
{
    auto [data, block_size] = GetParam();
    auto [content, expected] = data;

    std::ostringstream oss;
    ftp::ostream_adapter adapter(oss);
    ftp::detail::ascii_ostream stream(adapter);

    while (!content.empty())
    {
        std::size_t size = std::min(block_size, content.size());
        std::string buf = content.substr(0, size);
        content.erase(0, size);

        stream.write(buf.data(), buf.size());
    }

    stream.flush();

    ASSERT_EQ(expected, oss.str());
}

} // namespace
