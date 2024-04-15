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

#include <ftp/ftp_exception.hpp>
#include <ftp/detail/control_connection.hpp>
#include <ftp/detail/socket.hpp>
#include <ftp/detail/ssl_socket.hpp>
#include <ftp/detail/utils.hpp>

namespace ftp::detail
{

static bool try_parse_status_code(std::string_view line, std::uint16_t & status_code)
{
    if (line.size() < 3)
    {
        return false;
    }

    return utils::try_parse_uint16(line.substr(0, 3), status_code);
}

control_connection::control_connection(net_context & net_context)
{
    socket_ = std::make_unique<socket>(net_context.get_io_context());
}

void control_connection::connect(std::string_view hostname, std::uint16_t port)
{
    boost::asio::ip::tcp::resolver resolver(socket_->get_executor());
    boost::system::error_code ec;

    boost::asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(hostname, std::to_string(port), ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot open control connection");
    }

    socket_->connect(endpoints, ec);

    if (ec)
    {
        boost::system::error_code ignored;

        /* If the connect fails, and the socket was automatically opened,
         * the socket is not returned to the closed state.
         *
         * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
         */
        socket_->close(ignored);

        throw ftp_exception(ec, "Cannot open control connection");
    }
}

bool control_connection::is_connected() const
{
    return socket_->is_connected();
}

void control_connection::set_ssl(boost::asio::ssl::context *ssl_context)
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

bool control_connection::is_ssl() const
{
    return socket_->has_ssl_support();
}

void control_connection::ssl_handshake()
{
    boost::system::error_code ec;

    socket_->ssl_handshake(boost::asio::ssl::stream_base::client, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot perform SSL/TLS handshake");
    }
}

void control_connection::ssl_shutdown()
{
    boost::system::error_code ec;

    socket_->ssl_shutdown(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot perform SSL/TLS shutdown");
    }
}

reply control_connection::recv()
{
    std::string status_string;
    std::string line;
    std::uint16_t code;

    line = read_line();

    if (!try_parse_status_code(line, code))
    {
        throw ftp_exception("Cannot parse a status code from the server reply: '%1%'.", line);
    }

    /* Thus the format for multi-line replies is that the first line
     * will begin with the exact required reply code, followed
     * immediately by a Hyphen, "-" (also known as Minus), followed by
     * text.
     *
     * RFC 959: https://tools.ietf.org/html/rfc959
     */
    if (line.size() > 3 && line[3] == '-')
    {
        status_string = line;

        for (;;)
        {
            line = read_line();

            status_string += line;

            if (is_last_line(line, code))
            {
                break;
            }
        }
    }
    else
    {
        status_string = line;
    }

    if (!status_string.empty() && status_string.back() == '\n')
    {
        status_string.pop_back();
    }

    if (!status_string.empty() && status_string.back() == '\r')
    {
        status_string.pop_back();
    }

    /* Handle 421 (service not available, closing control connection) code as
     * a generic error. This may be a reply to any command if the service knows
     * it must shut down.
     */
    if (code == 421)
    {
        boost::system::error_code ec;

        if (socket_->has_ssl_support())
        {
            socket_->ssl_shutdown(ec);
        }
        else
        {
            socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        }

        if (ec == boost::asio::error::not_connected)
        {
            /* Ignore 'not_connected' error. We could get ENOTCONN if a server side
             * has already closed the control connection. This suits us, just close
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
            throw ftp_exception(ec, "Cannot close control connection");
        }

        socket_->close(ec);

        if (ec)
        {
            throw ftp_exception(ec, "Cannot close control connection");
        }
    }

    return reply(code, status_string);
}

/* The last line will begin with the same code, followed
 * immediately by Space <SP>, optionally some text, and the Telnet
 * end-of-line code.
 *
 * RFC 959: https://tools.ietf.org/html/rfc959
 */
bool control_connection::is_last_line(std::string_view line, std::uint16_t status_code)
{
    if (line.size() < 4)
    {
        return false;
    }

    if (line[3] != ' ')
    {
        return false;
    }

    std::uint16_t code;
    if (!try_parse_status_code(line, code))
    {
        return false;
    }

    return code == status_code;
}

void control_connection::send(std::string_view command)
{
    boost::system::error_code ec;

    std::string data(command);
    data.append("\r\n");

    socket_->write(data, ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot send data over control connection");
    }
}

std::string control_connection::read_line()
{
    boost::system::error_code ec;

    std::size_t len = socket_->read_line(buffer_, 8192, ec);

    if (ec == boost::asio::error::eof)
    {
        /* Ignore eof. */
    }
    else if (ec)
    {
        throw ftp_exception(ec, "Cannot receive data over control connection");
    }

    std::string line = buffer_.substr(0, len);
    buffer_.erase(0, len);

    return line;
}

void control_connection::disconnect()
{
    boost::system::error_code ec;

    if (socket_->has_ssl_support())
    {
        socket_->ssl_shutdown(ec);
    }
    else
    {
        socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    }

    if (ec == boost::asio::error::not_connected)
    {
        /* Ignore 'not_connected' error. We could get ENOTCONN if a server side
         * has already closed the control connection. This suits us, just close
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
        throw ftp_exception(ec, "Cannot close control connection");
    }

    socket_->close(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot close control connection");
    }
}

boost::asio::ip::tcp::endpoint control_connection::get_local_endpoint() const
{
    boost::system::error_code ec;

    boost::asio::ip::tcp::endpoint local_endpoint = socket_->local_endpoint(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot get local endpoint");
    }

    return local_endpoint;
}

boost::asio::ip::tcp::endpoint control_connection::get_remote_endpoint() const
{
    boost::system::error_code ec;

    boost::asio::ip::tcp::endpoint remote_endpoint = socket_->remote_endpoint(ec);

    if (ec)
    {
        throw ftp_exception(ec, "Cannot get remote endpoint");
    }

    return remote_endpoint;
}

} // namespace ftp::detail
