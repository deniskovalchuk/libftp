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

#include <ftp/detail/data_connection.hpp>
#include <ftp/detail/socket.hpp>
#include <ftp/detail/ssl_socket.hpp>
#include <ftp/ftp_exception.hpp>
#include <array>

namespace ftp::detail
{

data_connection::data_connection(net_context & net_context)
    : acceptor_(net_context.get_io_context())
{
    socket_ = std::make_unique<socket>(net_context.get_io_context());
}

void data_connection::connect(std::string_view ip, std::uint16_t port)
{
    boost::system::error_code ec;

    boost::asio::ip::address address = boost::asio::ip::make_address(ip, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot get IP address");
    }

    boost::asio::ip::tcp::endpoint remote_endpoint(address, port);
    socket_->connect(remote_endpoint, ec);

    if (ec)
    {
        boost::system::error_code ignored;

        /* If the connect fails, and the socket was automatically opened,
         * the socket is not returned to the closed state.
         *
         * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
         */
        socket_->close(ignored);

        throw ftp_exception(ec, "Cannot open data connection");
    }
}

void data_connection::connect(const boost::asio::ip::tcp::endpoint & endpoint)
{
    boost::system::error_code ec;

    socket_->connect(endpoint, ec);

    if (ec)
    {
        boost::system::error_code ignored;

        /* If the connect fails, and the socket was automatically opened,
         * the socket is not returned to the closed state.
         *
         * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
         */
        socket_->close(ignored);

        throw ftp_exception(ec, "Cannot open data connection");
    }
}

void data_connection::listen(const boost::asio::ip::tcp::endpoint & endpoint)
{
    boost::system::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot open socket acceptor");
    }

    acceptor_.bind(endpoint, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot bind socket acceptor");
    }

    acceptor_.listen(1, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot listen socket acceptor");
    }
}

void data_connection::accept()
{
    boost::asio::ip::tcp::socket & socket = socket_->get_socket();
    boost::system::error_code ec;

    acceptor_.accept(socket, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot accept data connection");
    }
}

void data_connection::set_ssl(boost::asio::ssl::context *ssl_context)
{
    boost::asio::ip::tcp::socket raw = socket_->detach();

    if (ssl_context)
    {
        socket_ = std::make_unique<ssl_socket>(std::move(raw), *ssl_context);
    }
    else
    {
        socket_ = std::make_unique<socket>(std::move(raw));
    }
}

void data_connection::ssl_handshake()
{
    boost::system::error_code ec;

    socket_->ssl_handshake(boost::asio::ssl::stream_base::client, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot perform SSL/TLS handshake");
    }
}

void data_connection::send(input_stream & stream, transfer_callback * transfer_cb)
{
    if (transfer_cb)
    {
        if (transfer_cb->is_cancelled())
        {
            return;
        }

        transfer_cb->begin();
    }

    std::array<char, 8192> buf;
    std::size_t size;

    while ((size = stream.read(buf.data(), buf.size())) > 0)
    {
        boost::system::error_code ec;

        socket_->write(buf.data(), size, ec);

        if (ec)
        {
            throw ftp_exception(ec, "Cannot send data over data connection");
        }

        if (transfer_cb)
        {
            transfer_cb->notify(size);

            if (transfer_cb->is_cancelled())
            {
                break;
            }
        }
    }

    if (transfer_cb)
    {
        transfer_cb->end();
    }
}

void data_connection::recv(output_stream & stream, transfer_callback * transfer_cb)
{
    if (transfer_cb)
    {
        if (transfer_cb->is_cancelled())
        {
            return;
        }

        transfer_cb->begin();
    }

    boost::system::error_code ec;
    std::array<char, 8192> buf;
    std::size_t size;

    while ((size = socket_->read_some(buf.data(), buf.size(), ec)) > 0)
    {
        stream.write(buf.data(), size);

        if (transfer_cb)
        {
            transfer_cb->notify(size);

            if (transfer_cb->is_cancelled())
            {
                break;
            }
        }
    }

    if (ec == boost::asio::error::eof)
    {
        /* Ignore eof. */
    }
    else if (ec)
    {
        throw ftp_exception(ec, "Cannot receive data over data connection");
    }

    stream.flush();

    if (transfer_cb)
    {
        transfer_cb->end();
    }
}

void data_connection::disconnect(bool graceful)
{
    boost::system::error_code ec;

    if (socket_->has_ssl_support())
    {
        socket_->ssl_shutdown(ec);
    }
    else if (graceful)
    {
        socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    }

    if (ec == boost::asio::error::not_connected)
    {
        /* Ignore 'not_connected' error. We could get ENOTCONN if a server side
         * has already closed the data connection. This suits us, just close
         * the socket.
         */
    }
    else if (ec == boost::asio::error::eof)
    {
        /* Rationale:
         * http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
         */
    }
    else if (ec)
    {
        throw ftp_exception(ec, "Cannot close data connection");
    }

    socket_->close(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot close data connection");
    }

    if (acceptor_.is_open())
    {
        acceptor_.close(ec);

        if (ec)
        {
            throw ftp_exception(ec, "Cannot close data connection");
        }
    }
}

boost::asio::ip::tcp::endpoint data_connection::get_listen_endpoint() const
{
    boost::system::error_code ec;

    boost::asio::ip::tcp::endpoint endpoint = acceptor_.local_endpoint(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot get listen endpoint");
    }

    return endpoint;
}

} // namespace ftp::detail
