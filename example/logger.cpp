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
#include <exception>
#include <memory>
#include <ftp/client.hpp>
#include "reply_handlers.hpp"

namespace
{

/*
 * Save logs in the following format:
 *   -- Connected to localhost on port 2121.
 *   <- 220 FTP server is ready.
 *   -> USER user
 *   <- 331 Username ok, send password.
 *   -> PASS *****
 *   <- 230 Login successful.
 *   ...
 *   -> QUIT
 *   <- 221 Goodbye.
 */
class logger : public ftp::observer
{
public:
    explicit logger(const std::string & filename)
    {
        log_file_.open(filename, std::ofstream::out | std::ofstream::trunc);
        if (!log_file_)
        {
            throw std::runtime_error("Cannot create a log file.");
        }
    }

private:
    void on_connected(std::string_view hostname, std::uint16_t port) override
    {
        log_file_ << "-- Connected to " << hostname << " on port " << port << "." << std::endl;
    }

    void on_request(std::string_view command) override
    {
        log_file_ << "-> ";

        /* Do not log user's password. */
        if (command.compare(0, 4, "PASS") == 0)
        {
            log_file_ << "PASS *****";
        }
        else
        {
            log_file_ << command;
        }

        log_file_ << std::endl;
    }

    void on_reply(const ftp::reply & reply) override
    {
        log_file_ << "<- " << reply.get_status_string() << std::endl;
    }

    std::ofstream log_file_;
};

} // namespace

int main(int argc, char *argv[])
{
    try
    {
        ftp::client client;

        client.add_observer(std::make_shared<logger>("ftp.log"));

        handle_reply(client.connect("localhost", 2121, "user", "password"));

        handle_reply(client.get_current_directory());

        handle_reply(client.disconnect());

        return EXIT_SUCCESS;
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
