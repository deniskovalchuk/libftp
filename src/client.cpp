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

#include <ftp/client.hpp>
#include <ftp/ftp_exception.hpp>
#include <ftp/detail/ascii_istream.hpp>
#include <ftp/detail/ascii_ostream.hpp>
#include <ftp/detail/binary_istream.hpp>
#include <ftp/detail/binary_ostream.hpp>
#include <ftp/detail/boost_utils.hpp>
#include <ftp/stream/ostream_adapter.hpp>
#include <sstream>

namespace ftp
{

using namespace ftp::detail;

client::client(transfer_mode mode, transfer_type type, bool rfc2428_support)
    : transfer_mode_(mode),
      transfer_type_(type),
      rfc2428_support_(rfc2428_support)
{
}

replies client::connect(std::string_view hostname,
                        std::uint16_t port,
                        const std::optional<std::string_view> & username,
                        std::string_view password)
{
    control_connection_.open(hostname, port);

    notify_connected(hostname, port);

    replies replies;
    reply reply = recv(replies);

    if (username && reply.is_positive())
    {
        process_login(username.value(), password, replies);
    }

    return replies;
}

bool client::is_connected()
{
    return control_connection_.is_open();
}

replies client::login(std::string_view username, std::string_view password)
{
    replies replies;

    process_login(username, password, replies);

    return replies;
}

reply client::logout()
{
    std::string command = make_command("REIN");

    return process_command(command);
}

reply client::change_current_directory(std::string_view path)
{
    std::string command = make_command("CWD", path);

    return process_command(command);
}

reply client::change_current_directory_up()
{
    std::string command = make_command("CDUP");

    return process_command(command);
}

reply client::get_current_directory()
{
    std::string command = make_command("PWD");

    return process_command(command);
}

replies client::download_file(output_stream & dst, std::string_view path, transfer_callback * transfer_cb)
{
    return process_download(dst, path, transfer_cb);
}

replies client::download_file(output_stream && dst, std::string_view path, transfer_callback * transfer_cb)
{
    return process_download(dst, path, transfer_cb);
}

replies client::upload_file(input_stream & src, std::string_view path, bool upload_unique, transfer_callback * transfer_cb)
{
    std::string_view command;

    if (upload_unique)
    {
        command = "STOU";
    }
    else
    {
        command = "STOR";
    }

    return process_upload(command, src, path, transfer_cb);
}

replies client::upload_file(input_stream && src, std::string_view path, bool upload_unique, transfer_callback * transfer_cb)
{
    std::string_view command;

    if (upload_unique)
    {
        command = "STOU";
    }
    else
    {
        command = "STOR";
    }

    return process_upload(command, src, path, transfer_cb);
}

replies client::append_file(input_stream & src, std::string_view path, transfer_callback * transfer_cb)
{
    return process_upload("APPE", src, path, transfer_cb);
}

replies client::append_file(input_stream && src, std::string_view path, transfer_callback * transfer_cb)
{
    return process_upload("APPE", src, path, transfer_cb);
}

file_list_reply client::get_file_list(const std::optional<std::string_view> & path, bool only_names)
{
    std::string command;

    if (only_names)
    {
        command = make_command("NLST", path);
    }
    else
    {
        command = make_command("LIST", path);
    }

    replies replies;
    std::string file_list;

    data_connection_ptr connection = create_data_connection(command, replies);

    if (connection)
    {
        std::ostringstream oss;
        ostream_adapter adapter(oss);
        output_stream_ptr stream = create_output_stream(adapter);
        connection->recv(*stream, nullptr);

        file_list = oss.str();
        notify_file_list(file_list);

        connection->close();
        recv(replies);
    }

    return { replies, file_list };
}

replies client::rename(std::string_view from_path, std::string_view to_path)
{
    std::string command = make_command("RNFR", from_path);
    replies replies;
    reply reply = process_command(command, replies);

    /* 350 Ready for destination name. */
    if (reply.get_code() == 350)
    {
        command = make_command("RNTO", to_path);
        process_command(command, replies);
    }

    return replies;
}

reply client::remove_file(std::string_view path)
{
    std::string command = make_command("DELE", path);

    return process_command(command);
}

reply client::create_directory(std::string_view path)
{
    std::string command = make_command("MKD", path);

    return process_command(command);
}

reply client::remove_directory(std::string_view path)
{
    std::string command = make_command("RMD", path);

    return process_command(command);
}

file_size_reply client::get_file_size(std::string_view path)
{
    std::string command = make_command("SIZE", path);
    reply reply = process_command(command);

    std::uint64_t size;
    if (try_parse_size_reply(reply, size))
    {
        return { reply, size };
    }
    else
    {
        return { reply, std::nullopt };
    }
}

bool client::try_parse_size_reply(const reply & reply, std::uint64_t & size)
{
    /* 213 SIZE_IN_BYTES */
    if (reply.get_code() != 213)
    {
        return false;
    }

    std::string_view status_string = reply.get_status_string();

    if (status_string.size() < 5)
    {
        return false;
    }

    return utils::try_parse_uint64(status_string.substr(4), size);
}

reply client::get_status(const std::optional<std::string_view> & path)
{
    std::string command = make_command("STAT", path);

    return process_command(command);
}

reply client::get_system_type()
{
    std::string command = make_command("SYST");

    return process_command(command);
}

reply client::get_help(const std::optional<std::string_view> & remote_command)
{
    std::string command = make_command("HELP", remote_command);

    return process_command(command);
}

reply client::get_site_commands()
{
    std::string command = make_command("SITE", "HELP");

    return process_command(command);
}

reply client::send_site_command(std::string_view remote_command)
{
    std::string command = make_command("SITE", remote_command);

    return process_command(command);
}

reply client::send_noop()
{
    std::string command = make_command("NOOP");

    return process_command(command);
}

std::optional<reply> client::disconnect(bool graceful)
{
    std::optional<reply> reply;

    if (graceful)
    {
        std::string command = make_command("QUIT");
        reply = process_command(command);
    }
    else
    {
        reply = std::nullopt;
    }

    /* The control connection may have been closed while processing the QUIT
     * command.
     */
    if (control_connection_.is_open())
    {
        control_connection_.close();
    }

    return reply;
}

void client::set_transfer_mode(transfer_mode mode)
{
    transfer_mode_ = mode;
}

transfer_mode client::get_transfer_mode() const
{
    return transfer_mode_;
}

reply client::set_transfer_type(transfer_type type)
{
    std::string command = make_type_command(type);
    reply reply = process_command(command);

    if (reply.is_positive())
    {
        transfer_type_ = type;
    }

    return reply;
}

transfer_type client::get_transfer_type() const
{
    return transfer_type_;
}

void client::add_observer(std::shared_ptr<observer> observer)
{
    observers_.emplace_back(observer);
}

void client::remove_observer(std::shared_ptr<observer> observer)
{
    observers_.remove(observer);
}

void client::set_rfc2428_support(bool support)
{
    rfc2428_support_ = support;
}

bool client::get_rfc2428_support() const
{
    return rfc2428_support_;
}

void client::send(std::string_view command)
{
    notify_request(command);

    control_connection_.send(command);
}

reply client::recv()
{
    reply reply = control_connection_.recv();

    notify_reply(reply);

    return reply;
}

reply client::recv(replies & replies)
{
    reply reply = control_connection_.recv();

    notify_reply(reply);

    replies.append(reply);

    return reply;
}

reply client::process_command(std::string_view command)
{
    send(command);

    return recv();
}

reply client::process_command(std::string_view command, replies & replies)
{
    send(command);

    return recv(replies);
}

reply client::process_login(std::string_view username, std::string_view password, replies & replies)
{
    std::string command = make_command("USER", username);
    reply reply = process_command(command, replies);

    /* 331 Username okay, need password. */
    if (reply.get_code() == 331)
    {
        command = make_command("PASS", password);
        reply = process_command(command, replies);
    }

    if (reply.is_positive())
    {
        command = make_type_command(transfer_type_);
        process_command(command, replies);
    }

    return reply;
}

replies client::process_download(output_stream & dst, std::string_view path, transfer_callback * transfer_cb)
{
    std::string command = make_command("RETR", path);
    replies replies;
    data_connection_ptr connection = create_data_connection(command, replies);

    if (connection)
    {
        output_stream_ptr stream = create_output_stream(dst);
        connection->recv(*stream, transfer_cb);

        if (transfer_cb && transfer_cb->is_cancelled())
        {
            process_abort(replies);

            /* Close the connection not gracefully in the case of abort. */
            connection->close(false);
        }
        else
        {
            connection->close();
            recv(replies);
        }
    }

    return replies;
}

replies client::process_upload(std::string_view remote_command, input_stream & src, std::string_view path, transfer_callback * transfer_cb)
{
    std::string command = make_command(remote_command, path);
    replies replies;
    data_connection_ptr connection = create_data_connection(command, replies);

    if (connection)
    {
        input_stream_ptr stream = create_input_stream(src);
        connection->send(*stream, transfer_cb);

        if (transfer_cb && transfer_cb->is_cancelled())
        {
            process_abort(replies);

            /* Close the connection not gracefully in the case of abort. */
            connection->close(false);
        }
        else
        {
            connection->close();
            recv(replies);
        }
    }

    return replies;
}

reply client::process_abort(replies & replies)
{
    /* RFC 959 requires sending Telnet IP/Synch sequence as OOB data before
     * aborting, but since many ftp servers do not handle it correctly, we
     * ignore this requirement.
     */

    std::string command = make_command("ABOR");
    reply reply = process_command(command, replies);

    /* 426 Connection closed; transfer aborted. */
    if (reply.get_code() == 426)
    {
        reply = recv(replies);
    }

    return reply;
}

input_stream_ptr client::create_input_stream(input_stream & src)
{
    if (transfer_type_ == transfer_type::binary)
    {
        return std::make_unique<binary_istream>(src);
    }
    else if (transfer_type_ == transfer_type::ascii)
    {
#ifdef _WIN32
        /* There is no difference between ascii and binary data transfer types
           on the Windows platform. */
        return std::make_unique<binary_istream>(src);
#else
        return std::make_unique<ascii_istream>(src);
#endif
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

output_stream_ptr client::create_output_stream(output_stream & dst)
{
    if (transfer_type_ == transfer_type::binary)
    {
        return std::make_unique<binary_ostream>(dst);
    }
    else if (transfer_type_ == transfer_type::ascii)
    {
#ifdef _WIN32
        /* There is no difference between ascii and binary data transfer types
           on the Windows platform. */
        return std::make_unique<binary_ostream>(dst);
#else
        return std::make_unique<ascii_ostream>(dst);
#endif
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

data_connection_ptr client::create_data_connection(std::string_view command, replies & replies)
{
    if (transfer_mode_ == transfer_mode::passive)
    {
        if (rfc2428_support_)
        {
            return process_epsv_command(command, replies);
        }
        else
        {
            return process_pasv_command(command, replies);
        }
    }
    else if (transfer_mode_ == transfer_mode::active)
    {
        return process_port_command(command, replies);
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

data_connection_ptr client::process_epsv_command(std::string_view command, replies & replies)
{
    std::string epsv_command = make_command("EPSV");
    reply reply = process_command(epsv_command, replies);

    if (!reply.is_positive())
    {
        return nullptr;
    }

    std::uint16_t remote_port;

    if (!try_parse_epsv_reply(reply, remote_port))
    {
        throw ftp_exception("Cannot parse a port number from the server reply: '%1%'.",
                            reply.get_status_string());
    }

    boost::asio::ip::tcp::endpoint remote_endpoint = control_connection_.get_remote_endpoint();
    boost::asio::ip::tcp::endpoint endpoint(remote_endpoint.address(), remote_port);

    data_connection_ptr connection = std::make_unique<data_connection>();
    connection->open(endpoint);

    reply = process_command(command, replies);

    if (!reply.is_positive())
    {
        connection->close();
        return nullptr;
    }

    return connection;
}

/* The text returned in response to the EPSV command MUST be:
 *   <text indicating server is entering extended passive mode>
 *   (<d><d><d><tcp-port><d>)
 *
 *  229 Entering Extended Passive Mode (|||6446|)
 */
bool client::try_parse_epsv_reply(const reply & reply, std::uint16_t & port)
{
    std::string_view status_string = reply.get_status_string();

    std::string_view::size_type begin = status_string.find('(');
    if (begin == std::string_view::npos)
    {
        return false;
    }

    std::string_view::size_type end = status_string.rfind(')');
    if (end == std::string_view::npos)
    {
        return false;
    }

    if (begin >= end)
    {
        return false;
    }

    /* Skip the "(|||" and ")" parts. */
    begin += 4;
    --end;

    if (begin >= end)
    {
        return false;
    }

    std::string_view port_str = status_string.substr(begin, end - begin);
    return utils::try_parse_uint16(port_str, port);
}

data_connection_ptr client::process_pasv_command(std::string_view command, replies & replies)
{
    std::string pasv_command = make_command("PASV");
    reply reply = process_command(pasv_command, replies);

    if (!reply.is_positive())
    {
        return nullptr;
    }

    std::string remote_ip;
    std::uint16_t remote_port;

    if (!try_parse_pasv_reply(reply, remote_ip, remote_port))
    {
        throw ftp_exception("Cannot parse IP address and port number from the server reply: '%1%'.",
                            reply.get_status_string());
    }

    data_connection_ptr connection = std::make_unique<data_connection>();
    connection->open(remote_ip, remote_port);

    reply = process_command(command, replies);

    if (!reply.is_positive())
    {
        connection->close();
        return nullptr;
    }

    return connection;
}

data_connection_ptr client::process_port_command(std::string_view command, replies & replies)
{
    boost::asio::ip::tcp::endpoint local_endpoint = control_connection_.get_local_endpoint();
    boost::asio::ip::tcp::endpoint listen_endpoint(local_endpoint.address(), 0);

    data_connection_ptr connection = std::make_unique<data_connection>();
    connection->listen(listen_endpoint);
    listen_endpoint = connection->get_listen_endpoint();

    std::string port_command = make_port_command(listen_endpoint);
    reply reply = process_command(port_command, replies);

    if (!reply.is_positive())
    {
        return nullptr;
    }

    reply = process_command(command, replies);

    if (!reply.is_positive())
    {
        return nullptr;
    }

    connection->accept();
    return connection;
}

/* This address information is broken into 8-bit fields and the
 * value of each field is transmitted as a decimal number (in
 * character string representation). The fields are separated
 * by commas.
 *
 * 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
 */
bool client::try_parse_pasv_reply(const reply & reply, std::string & ip, uint16_t & port)
{
    std::string_view status_string = reply.get_status_string();
    std::string::size_type begin = status_string.find('(');
    std::string::size_type end = status_string.rfind(')');

    if (begin == status_string.npos || end == status_string.npos || end <= begin)
    {
        return false;
    }

    std::string_view data = status_string.substr(begin + 1, end - begin - 1);
    std::vector<std::string> tokens = utils::split_string(data, ',');

    if (tokens.size() != 6)
    {
        return false;
    }

    ip.clear();
    ip.append(tokens[0]);
    ip.append(".");
    ip.append(tokens[1]);
    ip.append(".");
    ip.append(tokens[2]);
    ip.append(".");
    ip.append(tokens[3]);

    std::uint16_t port_high;
    if (!utils::try_parse_uint16(tokens[4], port_high))
    {
        return false;
    }

    std::uint16_t port_low;
    if (!utils::try_parse_uint16(tokens[5], port_low))
    {
        return false;
    }

    port = port_high * 256 + port_low;
    return true;
}

std::string client::make_command(std::string_view command, const std::optional<std::string_view> & argument)
{
    std::string result(command);

    if (argument)
    {
        result.append(" ");
        result.append(argument.value());
    }

    return result;
}

std::string client::make_port_command(const boost::asio::ip::tcp::endpoint & endpoint)
{
    std::string command = "PORT";
    command.append(" ");

    std::string address_string = boost_utils::address_to_string(endpoint.address());

    for (char ch : address_string)
    {
        if (ch == '.')
            command.push_back(',');
        else
            command.push_back(ch);
    }

    boost::asio::ip::port_type port = endpoint.port();

    command.append(",");
    command.append(std::to_string(port / 256));
    command.append(",");
    command.append(std::to_string(port % 256));
    return command;
}

std::string client::make_type_command(transfer_type type)
{
    if (type == transfer_type::binary)
    {
        return make_command("TYPE", "I");
    }
    else if (type == transfer_type::ascii)
    {
        return make_command("TYPE", "A");
    }
    else
    {
        assert(false);
        return "";
    }
}

void client::notify_connected(std::string_view hostname, std::uint16_t port)
{
    for (const std::shared_ptr<observer> & observer : observers_)
    {
        observer->on_connected(hostname, port);
    }
}

void client::notify_request(std::string_view command)
{
    for (const std::shared_ptr<observer> & observer : observers_)
    {
        observer->on_request(command);
    }
}

void client::notify_reply(const reply & reply)
{
    for (const std::shared_ptr<observer> & observer : observers_)
    {
        observer->on_reply(reply);
    }
}

void client::notify_file_list(std::string_view file_list)
{
    for (const std::shared_ptr<observer> & observer : observers_)
    {
        observer->on_file_list(file_list);
    }
}

} // namespace ftp
