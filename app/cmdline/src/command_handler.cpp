/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
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

#include "command_handler.hpp"
#include "cmdline_exception.hpp"
#include "transfer_callback.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ftp/ftp_exception.hpp>
#include <ftp/stream/istream_adapter.hpp>
#include <ftp/stream/ostream_adapter.hpp>

command_handler::command_handler()
{
    ftp_client_.add_observer(std::make_shared<stdout_writer>());
}

void command_handler::handle(command command, const std::vector<std::string> & args)
{
    try
    {
        if (command == command::open)
        {
            open(args);
        }
        else if (command == command::mode)
        {
            mode();
        }
        else if (command == command::active)
        {
            active();
        }
        else if (command == command::passive)
        {
            passive();
        }
        else if (command == command::user)
        {
            user(args);
        }
        else if (command == command::logout)
        {
            logout();
        }
        else if (command == command::close)
        {
            close();
        }
        else if (command == command::cd)
        {
            cd(args);
        }
        else if (command == command::cdup)
        {
            cdup();
        }
        else if (command == command::ls)
        {
            ls(args);
        }
        else if (command == command::put)
        {
            put(args);
        }
        else if (command == command::get)
        {
            get(args);
        }
        else if (command == command::rename)
        {
            rename(args);
        }
        else if (command == command::pwd)
        {
            pwd();
        }
        else if (command == command::mkdir)
        {
            mkdir(args);
        }
        else if (command == command::rmdir)
        {
            rmdir(args);
        }
        else if (command == command::del)
        {
            del(args);
        }
        else if (command == command::stat)
        {
            stat(args);
        }
        else if (command == command::syst)
        {
            syst();
        }
        else if (command == command::type)
        {
            type();
        }
        else if (command == command::binary)
        {
            binary();
        }
        else if (command == command::ascii)
        {
            ascii();
        }
        else if (command == command::size)
        {
            size(args);
        }
        else if (command == command::noop)
        {
            noop();
        }
        else if (command == command::rhelp)
        {
            rhelp(args);
        }
        else if (command == command::help)
        {
            help();
        }
        else if (command == command::exit)
        {
            exit();
        }
        else
        {
            throw cmdline_exception("Invalid command.");
        }
    }
    catch (ftp::ftp_exception &)
    {
        try
        {
            ftp_client_.disconnect(false);
        }
        catch (...)
        {
        }

        throw;
    }
}

void command_handler::open(const std::vector<std::string> & args)
{
    if (ftp_client_.is_connected())
    {
        throw cmdline_exception("Already connected, use close first.");
    }

    std::string hostname;
    std::uint16_t port = 21;

    if (args.empty())
    {
        hostname = utils::read_line("hostname: ");
    }
    else if (args.size() == 1)
    {
        hostname = args[0];
    }
    else if (args.size() == 2)
    {
        hostname = args[0];

        if (!ftp::detail::utils::try_parse_uint16(args[1], port))
        {
            throw cmdline_exception("Invalid port number.");
        }
    }
    else
    {
        throw cmdline_exception("usage: open hostname [ port ]");
    }

    ftp::replies replies = ftp_client_.connect(hostname, port);

    if (replies.is_positive())
    {
        std::string username = utils::read_line("username: ");
        std::string password = utils::read_password("password: ");

        ftp_client_.login(username, password);
    }
}

void command_handler::mode()
{
    ftp::transfer_mode mode = ftp_client_.get_transfer_mode();

    if (mode == ftp::transfer_mode::passive)
    {
        std::cout << "Using passive mode for data connection." << std::endl;
    }
    else if (mode == ftp::transfer_mode::active)
    {
        std::cout << "Using active mode for data connection." << std::endl;
    }
    else
    {
        assert(false);
    }
}

void command_handler::active()
{
    ftp_client_.set_transfer_mode(ftp::transfer_mode::active);
    std::cout << "Active mode on." << std::endl;
}

void command_handler::passive()
{
    ftp_client_.set_transfer_mode(ftp::transfer_mode::passive);
    std::cout << "Passive mode on." << std::endl;
}

void command_handler::user(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string username;
    std::string password;

    if (args.empty())
    {
        username = utils::read_line("username: ");
        password = utils::read_password("password: ");
    }
    else if (args.size() == 1)
    {
        username = args[0];
        password = utils::read_password("password: ");
    }
    else
    {
        throw cmdline_exception("usage: user username");
    }

    ftp_client_.login(username, password);
}

void command_handler::cd(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string remote_directory;

    if (args.empty())
    {
        remote_directory = utils::read_line("remote directory: ");
    }
    else if (args.size() == 1)
    {
        remote_directory = args[0];
    }
    else
    {
        throw cmdline_exception("usage: cd remote-directory");
    }

    ftp_client_.change_current_directory(remote_directory);
}

void command_handler::cdup()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.change_current_directory_up();
}

void command_handler::ls(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::optional<std::string> remote_directory;

    if (args.empty())
    {
        remote_directory = std::nullopt;
    }
    else if (args.size() == 1)
    {
        remote_directory = args[0];
    }
    else
    {
        throw cmdline_exception("usage: ls [ remote-directory ]");
    }

    ftp_client_.get_file_list(remote_directory);
}

void command_handler::put(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string local_file, remote_file;

    if (args.empty())
    {
        local_file = utils::read_line("local-file: ");
        remote_file = utils::get_filename(local_file);
    }
    else if (args.size() == 1)
    {
        local_file = args[0];
        remote_file = utils::get_filename(local_file);
    }
    else if (args.size() == 2)
    {
        local_file = args[0];
        remote_file = args[1];
    }
    else
    {
        throw cmdline_exception("usage: put local-file [ remote-file ]");
    }

    std::ifstream ifs(local_file, std::ios_base::binary);

    if (!ifs)
    {
        throw cmdline_exception("Cannot open file '%1%'.", local_file);
    }

    transfer_callback transfer_cb;
    ftp_client_.upload_file(ftp::istream_adapter(ifs), remote_file, false, &transfer_cb);
}

void command_handler::get(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string remote_file, local_file;

    if (args.empty())
    {
        remote_file = utils::read_line("remote-file: ");
        local_file = utils::get_filename(remote_file);
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
        local_file = utils::get_filename(remote_file);
    }
    else if (args.size() == 2)
    {
        remote_file = args[0];
        local_file = args[1];
    }
    else
    {
        throw cmdline_exception("usage: get remote-file [ local-file ]");
    }

    if (std::filesystem::exists(local_file))
    {
        throw cmdline_exception("File '%1%' already exists.", local_file);
    }

    std::ofstream ofs(local_file, std::ios_base::binary);

    if (!ofs)
    {
        throw cmdline_exception("Cannot create file '%1%'.", local_file);
    }

    transfer_callback transfer_cb;
    ftp::replies replies = ftp_client_.download_file(ftp::ostream_adapter(ofs), remote_file, &transfer_cb);

    /* Delete the created file in case of errors. */
    if (!replies.is_positive())
    {
        std::filesystem::remove(local_file);
    }
}

void command_handler::rename(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string from_remote_path, to_remote_path;

    if (args.size() == 2)
    {
        from_remote_path = args[0];
        to_remote_path = args[1];
    }
    else
    {
        throw cmdline_exception("usage: rename from-remote-path to-remote-path");
    }

    ftp_client_.rename(from_remote_path, to_remote_path);
}

void command_handler::pwd()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.get_current_directory();
}

void command_handler::mkdir(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string directory_name;

    if (args.empty())
    {
        directory_name = utils::read_line("directory-name: ");
    }
    else if (args.size() == 1)
    {
        directory_name = args[0];
    }
    else
    {
        throw cmdline_exception("usage: mkdir directory-name");
    }

    ftp_client_.create_directory(directory_name);
}

void command_handler::rmdir(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string directory_name;

    if (args.empty())
    {
        directory_name = utils::read_line("directory-name: ");
    }
    else if (args.size() == 1)
    {
        directory_name = args[0];
    }
    else
    {
        throw cmdline_exception("usage: rmdir directory-name");
    }

    ftp_client_.remove_directory(directory_name);
}

void command_handler::del(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string remote_file;

    if (args.empty())
    {
        remote_file = utils::read_line("remote-file: ");
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
    }
    else
    {
        throw cmdline_exception("usage: del remote-file");
    }

    ftp_client_.remove_file(remote_file);
}

void command_handler::type()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp::transfer_type type = ftp_client_.get_transfer_type();

    if (type == ftp::transfer_type::binary)
    {
        std::cout << "Using binary transfer type." << std::endl;
    }
    else if (type == ftp::transfer_type::ascii)
    {
        std::cout << "Using ascii transfer type." << std::endl;
    }
    else
    {
        assert(false);
    }
}

void command_handler::binary()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.set_transfer_type(ftp::transfer_type::binary);
}

void command_handler::ascii()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.set_transfer_type(ftp::transfer_type::ascii);
}

void command_handler::size(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::string remote_file;

    if (args.empty())
    {
        remote_file = utils::read_line("remote-file: ");
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
    }
    else
    {
        throw cmdline_exception("usage: size remote-file");
    }

    ftp_client_.get_file_size(remote_file);
}

void command_handler::stat(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::optional<std::string> remote_file;

    if (args.empty())
    {
        remote_file = std::nullopt;
    }
    else if (args.size() == 1)
    {
        remote_file = args[0];
    }
    else
    {
        throw cmdline_exception("usage: stat [ remote-file ]");
    }

    ftp_client_.get_status(remote_file);
}

void command_handler::syst()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.get_system_type();
}

void command_handler::noop()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.send_noop();
}

void command_handler::rhelp(const std::vector<std::string> & args)
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    std::optional<std::string> remote_command;

    if (args.empty())
    {
        remote_command = std::nullopt;
    }
    else if (args.size() == 1)
    {
        remote_command = args[0];
    }
    else
    {
        throw cmdline_exception("usage: rhelp [ remote-command ]");
    }

    ftp_client_.get_help(remote_command);
}

void command_handler::logout()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.logout();
}

void command_handler::close()
{
    if (!ftp_client_.is_connected())
    {
        throw cmdline_exception("Connection is not open.");
    }

    ftp_client_.disconnect();
}

void command_handler::help()
{
    std::cout <<
        "Commands:\n"
        "  open hostname [ port ] - connect to ftp server\n"
        "  user username - send new user information\n"
        "  cd remote-directory - change working directory on the server\n"
        "  cdup - change working directory on the server to parent directory\n"
        "  ls [ remote-directory ] - list contents of remote directory\n"
        "  pwd - print working directory on the server\n"
        "  mkdir directory-name - make directory on the server\n"
        "  rmdir directory-name - remove directory on the server\n"
        "  put local-file [ remote-file ] - upload file to the server\n"
        "  get remote-file [ local-file ] - download file from the server\n"
        "  rename from-remote-path to-remote-path - rename file/directory on the server\n"
        "  size remote-file - show size of remote file\n"
        "  del remote-file - delete file on the server\n"
        "  stat [ remote-file ] - show current status\n"
        "  syst - show remote system type\n"
        "  type - show current transfer type\n"
        "  binary - set binary transfer type\n"
        "  ascii - set ascii transfer type\n"
        "  mode - show current mode for data connection\n"
        "  active - set active mode for data connection\n"
        "  passive - set passive mode for data connection\n"
        "  noop - no operation\n"
        "  rhelp [ remote-command ] - get help from the server\n"
        "  logout - log out current user\n"
        "  close - close ftp connection\n"
        "  help - print local help information\n"
        "  exit - close ftp connection and exit\n";
}

void command_handler::exit()
{
    try
    {
        if (ftp_client_.is_connected())
        {
            ftp_client_.disconnect();
        }
    }
    catch (...)
    {
        /* Ignore all errors if a user exits. */
    }
}
