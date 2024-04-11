/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
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

#ifndef LIBFTP_CONTROL_CONNECTION_HPP
#define LIBFTP_CONTROL_CONNECTION_HPP

#include <ftp/reply.hpp>
#include <ftp/detail/net_context.hpp>
#include <ftp/detail/socket_base.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

namespace ftp::detail
{

class control_connection
{
public:
    explicit control_connection(net_context & net_context);

    control_connection(const control_connection &) = delete;

    control_connection & operator=(const control_connection &) = delete;

    void connect(std::string_view hostname, std::uint16_t port);

    [[nodiscard]] bool is_connected() const;

    void set_ssl(boost::asio::ssl::context *ssl_context);

    [[nodiscard]] bool is_ssl() const;

    void handshake();

    void send(std::string_view command);

    reply recv();

    void disconnect();

    [[nodiscard]] boost::asio::ip::tcp::endpoint get_local_endpoint() const;

    [[nodiscard]] boost::asio::ip::tcp::endpoint get_remote_endpoint() const;

private:
    std::string read_line();

    static bool is_last_line(std::string_view line, std::uint16_t status_code);

    std::string buffer_;
    socket_base_ptr socket_;
};

} // namespace ftp::detail
#endif //LIBFTP_CONTROL_CONNECTION_HPP
