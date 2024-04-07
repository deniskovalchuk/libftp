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

#include <ftp/detail/ssl_socket.hpp>

namespace ftp::detail
{

ssl_socket::ssl_socket(boost::asio::ip::tcp::socket && socket, boost::asio::ssl::context & ssl_context)
    : socket_(std::move(socket), ssl_context)
{
}

void ssl_socket::connect(const boost::asio::ip::tcp::resolver::results_type & eps, boost::system::error_code & ec)
{
    socket_base::connect(socket_.lowest_layer(), eps, ec);
}

void ssl_socket::connect(const boost::asio::ip::tcp::endpoint & ep, boost::system::error_code & ec)
{
    socket_base::connect(socket_.lowest_layer(), ep, ec);
}

void ssl_socket::handshake(boost::asio::ssl::stream_base::handshake_type type, boost::system::error_code & ec)
{
    socket_.handshake(type, ec);
}

bool ssl_socket::is_connected() const
{
    return socket_base::is_connected(socket_.lowest_layer());
}

std::size_t ssl_socket::write(const char *buf, std::size_t size, boost::system::error_code & ec)
{
    return socket_base::write(socket_, buf, size, ec);
}

std::size_t ssl_socket::write(std::string_view buf, boost::system::error_code & ec)
{
    return socket_base::write(socket_, buf, ec);
}

std::size_t ssl_socket::read_some(char *buf, std::size_t max_size, boost::system::error_code & ec)
{
    return socket_base::read_some(socket_, buf, max_size, ec);
}

std::size_t ssl_socket::read_line(std::string & buf, std::size_t max_size, boost::system::error_code & ec)
{
    return socket_base::read_line(socket_, buf, max_size, ec);
}

void ssl_socket::shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec)
{
    /* Careful: the non-SSL layer shutdown. */
    socket_base::shutdown(socket_.lowest_layer(), type, ec);
}

void ssl_socket::close(boost::system::error_code & ec)
{
    socket_base::close(socket_.lowest_layer(), ec);
}

boost::asio::ip::tcp::endpoint ssl_socket::local_endpoint(boost::system::error_code & ec) const
{
    return socket_base::local_endpoint(socket_.lowest_layer(), ec);
}

boost::asio::ip::tcp::endpoint ssl_socket::remote_endpoint(boost::system::error_code & ec) const
{
    return socket_base::remote_endpoint(socket_.lowest_layer(), ec);
}

boost::asio::ip::tcp::socket::executor_type ssl_socket::get_executor()
{
    return socket_base::get_executor(socket_);
}

boost::asio::ip::tcp::socket & ssl_socket::get_socket()
{
    return socket_.next_layer();
}

boost::asio::ip::tcp::socket ssl_socket::detach()
{
    return std::move(socket_.next_layer());
}

} // namespace ftp::detail
