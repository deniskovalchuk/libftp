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
#include <boost/asio/ip/tcp.hpp>

namespace ftp::detail
{

class control_connection
{
public:
    control_connection();

    control_connection(const control_connection &) = delete;

    control_connection & operator=(const control_connection &) = delete;

    void open(std::string_view hostname, std::uint16_t port);

    [[nodiscard]] bool is_open() const;

    void close();

    [[nodiscard]] std::string get_local_ip() const;

    [[nodiscard]] std::string get_remote_ip() const;

    void send(std::string_view command);

    reply recv();

private:
    std::string read_line();

    static bool is_last_line(std::string_view line, std::uint16_t status_code);

    std::string buffer_;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};

} // namespace ftp::detail
#endif //LIBFTP_CONTROL_CONNECTION_HPP
