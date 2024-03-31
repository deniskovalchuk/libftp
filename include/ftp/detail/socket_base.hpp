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

#ifndef LIBFTP_SOCKET_BASE_HPP
#define LIBFTP_SOCKET_BASE_HPP

#include <ftp/detail/socket_interface.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

namespace ftp::detail
{

template<typename SocketType>
class socket_base : public socket_interface
{
public:
    void connect(std::string_view hostname, std::uint16_t port, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();

        boost::asio::ip::tcp::resolver resolver(socket.get_executor());
        boost::asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(hostname, std::to_string(port), ec);

        if (ec)
            return;

        boost::asio::connect(socket, endpoints, ec);

        if (ec)
        {
            boost::system::error_code ignored;

            /* If the connect fails, and the socket was automatically opened,
             * the socket is not returned to the closed state.
             *
             * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
             */
            socket.close(ignored);
        }
    }

    [[nodiscard]]
    bool is_connected() const override
    {
        const SocketType & socket = get_sock();
        return socket.is_open();
    }

    std::size_t write(const char *buf, std::size_t size, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        return boost::asio::write(socket, boost::asio::buffer(buf, size), ec);
    }

    std::size_t write(std::string_view buf, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        return boost::asio::write(socket, boost::asio::buffer(buf), ec);
    }

    std::size_t read_some(char *buf, std::size_t max_size, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        return socket.read_some(boost::asio::buffer(buf, max_size), ec);
    }

    std::size_t read_line(std::string & buf, std::size_t max_size, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        return boost::asio::read_until(socket,
                                       boost::asio::dynamic_buffer(buf, max_size),
                                       match_eol, ec);
    }

    void shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        socket.shutdown(type, ec);
    }

    void close(boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        socket.close(ec);
    }

    boost::asio::ip::tcp::endpoint local_endpoint(boost::system::error_code & ec) const override
    {
        const SocketType & socket = get_sock();
        return socket.local_endpoint(ec);
    }

    boost::asio::ip::tcp::endpoint remote_endpoint(boost::system::error_code & ec) const override
    {
        const SocketType & socket = get_sock();
        return socket.remote_endpoint(ec);
    }

    SocketType & get_socket() override
    {
        return get_sock();
    }

protected:
    // TODO: Rename to get_socket();
    virtual const SocketType & get_sock() const = 0;
    virtual SocketType & get_sock() = 0;

private:
    static std::pair<boost::asio::buffers_iterator<boost::asio::const_buffers_1>, bool>
    match_eol(boost::asio::buffers_iterator<boost::asio::const_buffers_1> begin,
              boost::asio::buffers_iterator<boost::asio::const_buffers_1> end)
    {
        boost::asio::buffers_iterator<boost::asio::const_buffers_1> it = begin;

        while (it != end)
        {
            if (*it == '\n')
            {
                it++;
                return std::make_pair(it, true);
            }
            else if (*it == '\r')
            {
                it++;

                // Handle CRLF case.
                if (it != end && *it == '\n')
                    it++;

                return std::make_pair(it, true);
            }

            it++;
        }

        return std::make_pair(it, false);
    }
};

} // namespace ftp::detail
#endif //LIBFTP_SOCKET_BASE_HPP
