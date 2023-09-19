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
#include <sstream>
#include <ftp/client.hpp>
#include <ftp/stream/istream_adapter.hpp>
#include "reply_handlers.hpp"

namespace
{

class transfer_callback : public ftp::transfer_callback
{
public:
    explicit transfer_callback(std::size_t total_bytes)
        : total_bytes_(total_bytes)
        , transferred_bytes_(0)
    {}

private:
    void begin() override
    {
        std::cout << "The file transfer has started:" << std::endl;

        print_progress();
    }

    void notify(std::size_t bytes_transferred) override
    {
        transferred_bytes_ += bytes_transferred;

        print_progress();
    }

    void end() override
    {
        std::cout << "The file transfer is complete." << std::endl;
    }

    void print_progress() const
    {
        std::cout << transferred_bytes_ << " bytes out of " << total_bytes_ << " bytes have been transferred." << std::endl;
    }

    std::size_t total_bytes_;
    std::size_t transferred_bytes_;
};

} // namespace

int main(int argc, char *argv[])
{
    try
    {
        ftp::client client;

        handle_reply(client.connect("localhost", 2121, "user", "password"));

        /* Prepare 32 KB for uploading. */
        std::string data(32 * 1024, 'a');
        std::istringstream iss(data);

        transfer_callback transfer_cb(data.size());
        handle_reply(client.upload_file(ftp::istream_adapter(iss), "file", false, &transfer_cb));

        handle_reply(client.disconnect());

        return EXIT_SUCCESS;
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
