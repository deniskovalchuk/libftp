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

#ifndef LIBFTP_SSL_HPP
#define LIBFTP_SSL_HPP

#include <ftp/export.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>

namespace ftp::ssl
{

using namespace boost::asio::ssl;
using context_ptr = std::unique_ptr<context>;

/* Creates a new SSL context.
 * method - Like in Boost.Asio.
 * ssl_session_resumption - Configures the SSL session resumption. The SSL session of
 *                          control connection will be reused for data connections.
 */
FTP_EXPORT
context_ptr create_context(context::method method, bool ssl_session_resumption = false);

} // namespace ftp::ssl
#endif //LIBFTP_SSL_HPP
