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

#ifndef LIBFTP_TEST_SERVER_HPP
#define LIBFTP_TEST_SERVER_HPP

#include <string>
#include <cstdint>
#include <stdexcept>
#include <boost/process.hpp>

namespace ftp::test
{

class server
{
public:
    void start(const std::string & root_directory, std::uint16_t port)
    {
        const char *server_path = std::getenv("LIBFTP_TEST_SERVER_PATH");
        if (!server_path)
        {
            throw std::runtime_error("LIBFTP_TEST_SERVER_PATH is not set.");
        }

        boost::filesystem::path python_path = boost::process::search_path("python3");
        if (python_path.empty())
        {
            python_path = boost::process::search_path("python");
            if (python_path.empty())
            {
                throw std::runtime_error("Python is not found.");
            }
        }

        std::filesystem::create_directory(root_directory);

        /* Usage: python server.py root_directory port */
        boost::process::ipstream output;
        process_ = boost::process::child(python_path, server_path, root_directory, std::to_string(port),
                                         boost::process::std_out > boost::process::null,
                                         boost::process::std_err > output);

        while (process_.running())
        {
            std::string line;
            std::getline(output, line);

            if (line.find("starting FTP server") != std::string::npos)
                break;
        }
    }

    [[nodiscard]] bool running()
    {
        return process_.running();
    }

    void stop()
    {
        process_.terminate();
    }

private:
    boost::process::child process_;
};

} // namespace ftp::test
#endif //LIBFTP_TEST_SERVER_HPP
