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

#ifndef LIBFTP_SSL_SOCKET_HPP
#define LIBFTP_SSL_SOCKET_HPP

#include <ftp/detail/socket_base.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace ftp::detail
{

class ssl_socket : public socket_base
{
public:
    ssl_socket(boost::asio::ip::tcp::socket && socket,
               boost::asio::ssl::context & ssl_context,
               SSL_SESSION *ssl_session = nullptr);

    void connect(const boost::asio::ip::tcp::resolver::results_type & eps, boost::system::error_code & ec) override;

    void connect(const boost::asio::ip::tcp::endpoint & ep, boost::system::error_code & ec) override;

    [[nodiscard]] bool is_connected() const override;

    [[nodiscard]] bool has_ssl_support() const override;

    void ssl_handshake(boost::asio::ssl::stream_base::handshake_type type, boost::system::error_code & ec) override;

    void ssl_shutdown(boost::system::error_code & ec) override;

    SSL_SESSION * get_ssl_session() override;

    std::size_t write(const char *buf, std::size_t size, boost::system::error_code & ec) override;

    std::size_t write(std::string_view buf, boost::system::error_code & ec) override;

    std::size_t read_some(char *buf, std::size_t max_size, boost::system::error_code & ec) override;

    std::size_t read_line(std::string & buf, std::size_t max_size, boost::system::error_code & ec) override;

    void shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec) override;

    void close(boost::system::error_code & ec) override;

    [[nodiscard]] boost::asio::ip::tcp::endpoint local_endpoint(boost::system::error_code & ec) const override;

    [[nodiscard]] boost::asio::ip::tcp::endpoint remote_endpoint(boost::system::error_code & ec) const override;

    [[nodiscard]] boost::asio::ip::tcp::socket::executor_type get_executor() override;

    [[nodiscard]] boost::asio::ip::tcp::socket & get_socket() override;

    [[nodiscard]] boost::asio::ip::tcp::socket detach() override;

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
};

} // namespace ftp::detail
#endif //LIBFTP_SSL_SOCKET_HPP
