/*
 * MIT License
 *
 * Copyright (c) 2023 Denis Kovalchuk
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
#include <fstream>
#include <ftp/client.hpp>
#include <ftp/stream/ostream_adapter.hpp>
#include "reply_handlers.hpp"

int main(int argc, char *argv[])
{
    try
    {
        const char *filename = "favicon.ico";

        std::ofstream ofs(filename, std::ofstream::binary | std::ofstream::trunc);

        if (!ofs)
        {
            std::cerr << "Cannot open a file." << std::endl;
            return EXIT_FAILURE;
        }

        ftp::client client;

        handle_reply(client.connect("ftp.freebsd.org", 21, "anonymous"));

        handle_reply(client.download_file(ftp::ostream_adapter(ofs), filename));

        handle_reply(client.disconnect());

        return EXIT_SUCCESS;
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
