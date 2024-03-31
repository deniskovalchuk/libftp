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

    void shutdown(boost::asio::ip::tcp::socket::shutdown_type type, boost::system::error_code & ec) override
    {
        SocketType & socket = get_sock();
        socket.shutdown(type, ec);
    }

    SocketType & get_socket() override
    {
        return get_sock();
    }

protected:
    // TODO: Rename to get_socket();
    virtual const SocketType & get_sock() const = 0;
    virtual SocketType & get_sock() = 0;
};

} // namespace ftp::detail
#endif //LIBFTP_SOCKET_BASE_HPP
