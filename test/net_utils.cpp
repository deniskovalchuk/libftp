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
#include <ftp/detail/net_utils.hpp>
/* TODO: Remove. */
#include <boost/asio/ssl/context.hpp>

namespace
{

using boost::asio::ip::make_address;

TEST(net_utils, address_to_string)
{
    EXPECT_EQ("127.0.0.7", ftp::detail::net_utils::address_to_string(make_address("127.0.0.7")));
    EXPECT_EQ("192.168.5.255", ftp::detail::net_utils::address_to_string(make_address("192.168.5.255")));
    EXPECT_EQ("255.255.255.255", ftp::detail::net_utils::address_to_string(make_address("255.255.255.255")));

    EXPECT_EQ("::1", ftp::detail::net_utils::address_to_string(make_address("::1")));
    EXPECT_EQ("2001:db8:85a3::8a2e:370:7334",
              ftp::detail::net_utils::address_to_string(make_address("2001:0db8:85a3:0000:0000:8a2e:0370:7334")));
    EXPECT_EQ("2345:425:2ca1::567:5673:23b5",
              ftp::detail::net_utils::address_to_string(make_address("2345:425:2CA1::567:5673:23B5")));
}

/* TODO: This is a temporary test to check compilation with the OpenSSL library. */
TEST(net_utils, compile_with_OpenSSL)
{
    boost::asio::ssl::context context(boost::asio::ssl::context::method::tls_client);
}

} // namespace
