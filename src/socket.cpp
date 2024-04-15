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

#include <ftp/detail/socket.hpp>
#include <boost/asio/connect.hpp>

namespace ftp::detail
{

socket::socket(boost::asio::io_context & io_context)
    : socket_(io_context)
{}

socket::socket(boost::asio::ip::tcp::socket && socket)
    : socket_(std::move(socket))
{}

void socket::connect(const boost::asio::ip::tcp::resolver::results_type & eps, boost::system::error_code & ec)
{
    boost::asio::connect(socket_, eps, ec);
}

void socket::connect(const boost::asio::ip::tcp::endpoint & ep, boost::system::error_code & ec)
{
    socket_.connect(ep, ec);
}

bool socket::is_connected() const
{
    return socket_.is_open();
}

bool socket::has_ssl_support() const
{
    return false;
}

void socket::ssl_handshake(boost::asio::ssl::stream_base::handshake_type type, boost::system::error_code & ec)
{
    /* Handshake makes sense only for SSL-sockets. */
}

std::size_t socket::write(const char *buf, std::size_t size, boost::system::error_code & ec)
{
    return socket_base::write(socket_, buf, size, ec);
}

std::size_t socket::write(std::string_view buf, boost::system::error_code & ec)
{
    return socket_base::write(socket_, buf, ec);
}

std::size_t socket::read_some(char *buf, std::size_t max_size, boost::system::error_code & ec)
{
    return socket_base::read_some(socket_, buf, max_size, ec);
}

std::size_t socket::read_line(std::string & buf, std::size_t max_size, boost::system::error_code & ec)
{
    return socket_base::read_line(socket_, buf, max_size, ec);
}

void socket::shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec)
{
    socket_.shutdown(type, ec);
}

void socket::close(boost::system::error_code & ec)
{
    socket_.close(ec);
}

boost::asio::ip::tcp::endpoint socket::local_endpoint(boost::system::error_code & ec) const
{
    return socket_.local_endpoint(ec);
}

boost::asio::ip::tcp::endpoint socket::remote_endpoint(boost::system::error_code & ec) const
{
    return socket_.remote_endpoint(ec);
}

boost::asio::ip::tcp::socket::executor_type socket::get_executor()
{
    return socket_.get_executor();
}

boost::asio::ip::tcp::socket & socket::get_socket()
{
    return socket_;
}

boost::asio::ip::tcp::socket socket::detach()
{
    return std::move(socket_);
}

} // namespace ftp::detail
