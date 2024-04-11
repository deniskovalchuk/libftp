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

#ifndef LIBFTP_CLIENT_HPP
#define LIBFTP_CLIENT_HPP

#include <ftp/observer.hpp>
#include <ftp/file_list_reply.hpp>
#include <ftp/file_size_reply.hpp>
#include <ftp/replies.hpp>
#include <ftp/reply.hpp>
#include <ftp/ssl.hpp>
#include <ftp/transfer_callback.hpp>
#include <ftp/transfer_mode.hpp>
#include <ftp/transfer_type.hpp>
#include <ftp/stream/input_stream.hpp>
#include <ftp/stream/output_stream.hpp>
#include <ftp/detail/control_connection.hpp>
#include <ftp/detail/data_connection.hpp>
#include <ftp/detail/net_context.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <memory>
#include <list>

namespace ftp
{

class client
{
public:
    explicit client(transfer_mode mode = transfer_mode::passive,
                    transfer_type type = transfer_type::binary,
                    ssl::context_ptr && ssl_context = nullptr,
                    bool rfc2428_support = true);

    explicit client(ssl::context_ptr && ssl_context);

    client(const client &) = delete;

    client & operator=(const client &) = delete;

    client(client &&) noexcept = delete;

    client & operator=(client &&) noexcept = delete;

    replies connect(std::string_view hostname,
                    std::uint16_t port = 21,
                    const std::optional<std::string_view> & username = std::nullopt,
                    std::string_view password = "");

    bool is_connected();

    replies login(std::string_view username, std::string_view password = "");

    reply logout();

    reply change_current_directory(std::string_view path);

    reply change_current_directory_up();

    reply get_current_directory();

    replies download_file(output_stream & dst, std::string_view path, transfer_callback * transfer_cb = nullptr);

    replies download_file(output_stream && dst, std::string_view path, transfer_callback * transfer_cb = nullptr);

    replies upload_file(input_stream & src, std::string_view path, bool upload_unique = false, transfer_callback * transfer_cb = nullptr);

    replies upload_file(input_stream && src, std::string_view path, bool upload_unique = false, transfer_callback * transfer_cb = nullptr);

    replies append_file(input_stream & src, std::string_view path, transfer_callback * transfer_cb = nullptr);

    replies append_file(input_stream && src, std::string_view path, transfer_callback * transfer_cb = nullptr);

    file_list_reply get_file_list(const std::optional<std::string_view> & path = std::nullopt, bool only_names = false);

    replies rename(std::string_view from_path, std::string_view to_path);

    reply remove_file(std::string_view path);

    reply create_directory(std::string_view path);

    reply remove_directory(std::string_view path);

    file_size_reply get_file_size(std::string_view path);

    reply get_status(const std::optional<std::string_view> & path = std::nullopt);

    reply get_system_type();

    reply get_help(const std::optional<std::string_view> & command = std::nullopt);

    reply get_site_commands();

    reply send_site_command(std::string_view command);

    reply send_noop();

    std::optional<reply> disconnect(bool graceful = true);

    void set_transfer_mode(transfer_mode mode);

    [[nodiscard]] transfer_mode get_transfer_mode() const;

    reply set_transfer_type(transfer_type type);

    [[nodiscard]] transfer_type get_transfer_type() const;

    void add_observer(std::shared_ptr<observer> observer);

    void remove_observer(std::shared_ptr<observer> observer);

    void set_rfc2428_support(bool support);

    [[nodiscard]] bool get_rfc2428_support() const;

private:
    void send(std::string_view command);

    reply recv();

    reply recv(replies & replies);

    reply process_command(std::string_view command);

    reply process_command(std::string_view command, replies & replies);

    reply process_login(std::string_view username, std::string_view password, replies & replies);

    replies process_download(output_stream & dst, std::string_view path, transfer_callback * transfer_cb);

    replies process_upload(std::string_view command, input_stream & src, std::string_view path, transfer_callback * transfer_cb);

    reply process_abort(replies & replies);

    input_stream_ptr create_input_stream(input_stream & src);

    output_stream_ptr create_output_stream(output_stream & dst);

    detail::data_connection_ptr create_data_connection(std::string_view command, replies & replies);

    detail::data_connection_ptr process_epsv_command(std::string_view command, replies & replies);

    detail::data_connection_ptr process_eprt_command(std::string_view command, replies & replies);

    detail::data_connection_ptr process_pasv_command(std::string_view command, replies & replies);

    detail::data_connection_ptr process_port_command(std::string_view command, replies & replies);

    static bool try_parse_epsv_reply(const reply & reply, std::uint16_t & port);

    static bool try_parse_pasv_reply(const reply & reply, std::string & ip, std::uint16_t & port);

    static std::string make_command(std::string_view command, const std::optional<std::string_view> & argument = std::nullopt);

    static std::string make_eprt_command(const boost::asio::ip::tcp::endpoint & endpoint);

    static std::string make_port_command(const boost::asio::ip::tcp::endpoint & endpoint);

    static std::string make_type_command(transfer_type type);

    void notify_connected(std::string_view hostname, std::uint16_t port);

    void notify_request(std::string_view command);

    void notify_reply(const reply & reply);

    void notify_file_list(std::string_view file_list);

    transfer_mode transfer_mode_;
    transfer_type transfer_type_;
    ssl::context_ptr ssl_context_;
    bool rfc2428_support_;
    detail::net_context net_context_;
    detail::control_connection control_connection_;
    std::list<std::shared_ptr<observer>> observers_;
};

} // namespace ftp
#endif //LIBFTP_CLIENT_HPP
