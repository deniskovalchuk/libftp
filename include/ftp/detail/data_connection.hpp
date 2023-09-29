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

#ifndef LIBFTP_DATA_CONNECTION_HPP
#define LIBFTP_DATA_CONNECTION_HPP

#include <ftp/stream/input_stream.hpp>
#include <ftp/stream/output_stream.hpp>
#include <ftp/transfer_callback.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <string_view>

namespace ftp::detail
{

class data_connection
{
public:
    data_connection();

    data_connection(const data_connection &) = delete;

    data_connection & operator=(const data_connection &) = delete;

    void connect(std::string_view ip, std::uint16_t port);

    void connect(const boost::asio::ip::tcp::endpoint & endpoint);

    void listen(const boost::asio::ip::tcp::endpoint & endpoint);

    void accept();

    [[nodiscard]] boost::asio::ip::tcp::endpoint get_listen_endpoint() const;

    void disconnect(bool graceful = true);

    void send(input_stream & stream, transfer_callback * transfer_cb);

    void recv(output_stream & stream, transfer_callback * transfer_cb);

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

using data_connection_ptr = std::unique_ptr<data_connection>;

} // namespace ftp::detail
#endif //LIBFTP_DATA_CONNECTION_HPP
