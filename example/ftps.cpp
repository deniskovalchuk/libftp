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

#include <iostream>
#include <ftp/client.hpp>
#include "reply_handlers.hpp"

int main(int argc, char *argv[])
{
    try
    {
        /* Learn more about ssl_context in the Boost.Asio documentation.
         * https://live.boost.org/doc/libs/1_85_0/doc/html/boost_asio/overview/ssl.html
         */
        ftp::ssl::context_ptr ssl_context = std::make_unique<ftp::ssl::context>(ftp::ssl::context::tlsv13_client);
        ssl_context->set_default_verify_paths();
        ssl_context->set_verify_mode(ftp::ssl::verify_peer);

        /* Set SSL_SESS_CACHE_CLIENT to support SSL session resumption for data connections. */
        SSL_CTX_set_session_cache_mode(ssl_context->native_handle(), SSL_SESS_CACHE_CLIENT);

        ftp::client client(std::move(ssl_context));

        handle_reply(client.connect("test.rebex.net", 21, "demo", "password"));

        ftp::file_list_reply reply = client.get_file_list();

        handle_reply(reply);

        std::cout << reply.get_file_list_str();

        handle_reply(client.disconnect());

        return EXIT_SUCCESS;
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
