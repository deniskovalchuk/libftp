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
    void start(const std::string & root_directory, std::uint16_t port, bool use_ssl)
    {
        std::filesystem::path server_script_path = get_server_script_path();

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

        const char *use_ssl_arg;
        if (use_ssl)
        {
            use_ssl_arg = "--use-ssl=yes";
        }
        else
        {
            use_ssl_arg = "--use-ssl=no";
        }

        /* Usage: python server.py root_directory port [--use-ssl {yes,no}] */
        boost::process::ipstream output;
        process_ = boost::process::child(python_path, server_script_path.string(),
                                         root_directory, std::to_string(port), use_ssl_arg,
                                         boost::process::std_out > boost::process::null,
                                         boost::process::std_err > output);

        while (process_.running())
        {
            std::string line;
            std::getline(output, line);

            if (line.find("starting FTP server") != std::string::npos ||
                line.find("starting FTP+SSL server") != std::string::npos)
            {
                break;
            }
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

    static std::filesystem::path get_root_ca_cert_path()
    {
        std::filesystem::path server_script_path = get_server_script_path();
        std::filesystem::path server_dir = server_script_path.parent_path();
        std::filesystem::path certs_dir = server_dir / "certs";
        std::filesystem::path root_ca_cert = certs_dir / "root_ca_cert.pem";
        return root_ca_cert;
    }

    static std::filesystem::path get_ca_cert_path()
    {
        std::filesystem::path server_script_path = get_server_script_path();
        std::filesystem::path server_dir = server_script_path.parent_path();
        std::filesystem::path certs_dir = server_dir / "certs";
        std::filesystem::path ca_cert = certs_dir / "ca_cert.pem";
        return ca_cert;
    }

private:
    static std::filesystem::path get_server_script_path()
    {
        const char *path = std::getenv("LIBFTP_TEST_SERVER_PATH");

        if (!path)
        {
            throw std::runtime_error("LIBFTP_TEST_SERVER_PATH is not set.");
        }

        return { path };
    }

    boost::process::child process_;
};

} // namespace ftp::test
#endif //LIBFTP_TEST_SERVER_HPP
