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

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>
#include <memory>

namespace ftp::detail
{

class socket_base
{
public:
    virtual void connect(const boost::asio::ip::tcp::resolver::results_type & eps, boost::system::error_code & ec) = 0;

    virtual void connect(const boost::asio::ip::tcp::endpoint & ep, boost::system::error_code & ec) = 0;

    [[nodiscard]] virtual bool is_connected() const = 0;

    virtual std::size_t write(const char *buf, std::size_t size, boost::system::error_code & ec) = 0;

    virtual std::size_t write(std::string_view buf, boost::system::error_code & ec) = 0;

    virtual std::size_t read_some(char *buf, std::size_t max_size, boost::system::error_code & ec) = 0;

    virtual std::size_t read_line(std::string & buf, std::size_t max_size, boost::system::error_code & ec) = 0;

    virtual void shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec) = 0;

    virtual void close(boost::system::error_code & ec) = 0;

    [[nodiscard]] virtual boost::asio::ip::tcp::endpoint local_endpoint(boost::system::error_code & ec) const = 0;

    [[nodiscard]] virtual boost::asio::ip::tcp::endpoint remote_endpoint(boost::system::error_code & ec) const = 0;

    [[nodiscard]] virtual boost::asio::ip::tcp::socket::executor_type get_executor() = 0;

    // TODO: Remove.
    [[nodiscard]] virtual boost::asio::ip::tcp::socket & get_socket() = 0;

    [[nodiscard]] virtual boost::asio::ip::tcp::socket detach() = 0;

    virtual ~socket_base() = default;

protected:
    template<typename SocketType>
    void connect(SocketType & socket, const boost::asio::ip::tcp::resolver::results_type & eps, boost::system::error_code & ec)
    {
        boost::asio::connect(socket, eps, ec);
    }

    template<typename SocketType>
    void connect(SocketType & socket, const boost::asio::ip::tcp::endpoint & ep, boost::system::error_code & ec)
    {
        socket.connect(ep, ec);
    }

    template<typename SocketType>
    [[nodiscard]]
    bool is_connected(const SocketType & socket) const
    {
        return socket.is_open();
    }

    template<typename SocketType>
    std::size_t write(SocketType & socket, const char *buf, std::size_t size, boost::system::error_code & ec)
    {
        return boost::asio::write(socket, boost::asio::buffer(buf, size), ec);
    }

    template<typename SocketType>
    std::size_t write(SocketType & socket, std::string_view buf, boost::system::error_code & ec)
    {
        return boost::asio::write(socket, boost::asio::buffer(buf), ec);
    }

    template<typename SocketType>
    std::size_t read_some(SocketType & socket, char *buf, std::size_t max_size, boost::system::error_code & ec)
    {
        return socket.read_some(boost::asio::buffer(buf, max_size), ec);
    }

    template<typename SocketType>
    std::size_t read_line(SocketType & socket, std::string & buf, std::size_t max_size, boost::system::error_code & ec)
    {
        return boost::asio::read_until(socket,
                                       boost::asio::dynamic_buffer(buf, max_size),
                                       match_eol, ec);
    }

    template<typename SocketType>
    void shutdown(SocketType & socket, boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec)
    {
        socket.shutdown(type, ec);
    }

    template<typename SocketType>
    void close(SocketType & socket, boost::system::error_code & ec)
    {
        socket.close(ec);
    }

    template<typename SocketType>
    [[nodiscard]]
    boost::asio::ip::tcp::endpoint local_endpoint(const SocketType & socket, boost::system::error_code & ec) const
    {
        return socket.local_endpoint(ec);
    }

    template<typename SocketType>
    [[nodiscard]]
    boost::asio::ip::tcp::endpoint remote_endpoint(const SocketType & socket, boost::system::error_code & ec) const
    {
        return socket.remote_endpoint(ec);
    }

    template<typename SocketType>
    [[nodiscard]]
    boost::asio::ip::tcp::socket::executor_type get_executor(SocketType & socket)
    {
        return socket.get_executor();
    }

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

using socket_base_ptr = std::unique_ptr<socket_base>;

} // namespace ftp::detail
#endif //LIBFTP_SOCKET_BASE_HPP
