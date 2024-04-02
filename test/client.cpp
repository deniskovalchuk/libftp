/*
 * MIT License
 *
 * Copyright (c) 2020 Denis Kovalchuk
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <memory>
#include <ftp/client.hpp>
#include <ftp/ftp_exception.hpp>
#include <ftp/observer.hpp>
#include <ftp/stream/istream_adapter.hpp>
#include <ftp/stream/ostream_adapter.hpp>
#include "test_server.hpp"
#include "test_utils.hpp"

namespace
{

using namespace ftp::test;
using testing::ElementsAre;

class test_observer : public ftp::observer
{
public:
    explicit test_observer(std::string_view mark)
        : mark_(mark)
    {}

    void on_connected(std::string_view hostname, std::uint16_t port) override
    {
        data_.append(mark_);
        data_.append(": -- ");
        data_.append("Connected to ");
        data_.append(hostname);
        data_.append(" on port ");
        data_.append(std::to_string(port));
        data_.append(".");
    }

    void on_request(std::string_view command) override
    {
        data_.append(mark_);
        data_.append(": -> ");
        data_.append(command);
    }

    void on_reply(const ftp::reply & reply) override
    {
        data_.append(mark_);
        data_.append(": <- ");
        data_.append(reply.get_status_string());
    }

    [[nodiscard]] const std::string & get_data() const
    {
        return data_;
    }

private:
    std::string mark_;
    std::string data_;
};

class test_cancel_callback : public ftp::transfer_callback
{
public:
    bool is_cancelled() override
    {
        return true;
    }
};

template<std::uint16_t server_port, bool server_use_ssl>
class client_base : public testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        try
        {
            server_.start(server_root_dir_, server_port, server_use_ssl);
        }
        catch (const std::exception & ex)
        {
            GTEST_SKIP() << "Skip. Test FTP server is not running: " << ex.what();
        }
    }

    static void TearDownTestSuite()
    {
        if (server_.running())
        {
            server_.stop();
        }

        std::filesystem::remove_all(server_root_dir_);
    }

    void SetUp() override
    {
        if (!server_.running())
        {
            GTEST_SKIP() << "Skip. Test FTP server is not running.";
        }
    }

    void TearDown() override
    {
        /* Clean up the server root directory after each test. */
        if (std::filesystem::exists(server_root_dir_))
        {
            for (const std::filesystem::directory_entry & entry : std::filesystem::directory_iterator(server_root_dir_))
            {
                std::filesystem::remove_all(entry);
            }
        }
    }

private:
    inline static std::string server_root_dir_ = "server_root";
    inline static ftp::test::server server_;
};

class client : public client_base<2121, false>
{
};

TEST_F(client, open_connection)
{
    ftp::client client;

    ASSERT_FALSE(client.is_connected());

    for (int i = 0; i < 5; i++)
    {
        check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");
        ASSERT_TRUE(client.is_connected());

        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_FALSE(client.is_connected());
    }

    for (int i = 0; i < 5; i++)
    {
        check_reply(client.connect("127.0.0.1", 2121, "alice", "password"), CRLF("220 FTP server is ready.",
                                                                                 "331 Username ok, send password.",
                                                                                 "230 Login successful.",
                                                                                 "200 Type set to: Binary."));
        ASSERT_TRUE(client.is_connected());

        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_FALSE(client.is_connected());
    }
}

TEST_F(client, connection_is_not_open)
{
    ftp::client client;

    ASSERT_THROW({
        try
        {
            client.get_file_list();
        }
        catch (const ftp::ftp_exception & ex)
        {
            if (is_windows_platform())
            {
                ASSERT_STREQ("Cannot send data over control connection: The file handle supplied is not valid", ex.what());
            }
            else
            {
                ASSERT_STREQ("Cannot send data over control connection: Bad file descriptor", ex.what());
            }

            throw;
        }
    }, ftp::ftp_exception);
}

TEST_F(client, login)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, login_nonexistent_user)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("nonexistent", "password"), CRLF("331 Username ok, send password.",
                                                              "530 Authentication failed."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, login_wrong_password)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "wrong_password"), CRLF("331 Username ok, send password.",
                                                             "530 Authentication failed."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, logout)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.logout(), "230 Ready for new user.");

    check_reply(client.login("alice", "password"), CRLF("331 Username ok, send password.",
                                                        "230 Login successful.",
                                                        "200 Type set to: Binary."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, send_noop)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.send_noop(), "200 I successfully did nothing'.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_site_commands)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.get_site_commands(), CRLF("214-The following SITE commands are recognized:",
                                                 " CHMOD",
                                                 " HELP",
                                                 "214 Help SITE command successful."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, send_site_command)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.send_site_command("HELP"), CRLF("214-The following SITE commands are recognized:",
                                                       " CHMOD",
                                                       " HELP",
                                                       "214 Help SITE command successful."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.send_site_command("CHMOD 777 dir"), "200 SITE CHMOD successful.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_current_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.get_current_directory(), R"(257 "/" is the current directory.)");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, create_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, create_directory_already_exists)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.create_directory("dir"), "550 File exists.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, remove_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.remove_directory("dir"), "250 Directory removed.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, remove_nonexistent_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.remove_directory("nonexistent"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, remove_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::istringstream iss("content");
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

    check_reply(client.remove_file("file"), "250 File removed.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, remove_nonexistent_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.remove_file("nonexistent"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, change_current_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.change_current_directory("dir"), R"(250 "/dir" is the current directory.)");

    check_reply(client.change_current_directory(".."), R"(250 "/" is the current directory.)");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, change_current_directory_nonexistent_dir)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.change_current_directory("nonexistent"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, change_current_directory_up)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.change_current_directory("dir"), R"(250 "/dir" is the current directory.)");

    check_reply(client.change_current_directory_up(), R"(250 "/" is the current directory.)");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, set_transfer_type)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    ASSERT_EQ(client.get_transfer_type(), ftp::transfer_type::binary);

    check_reply(client.set_transfer_type(ftp::transfer_type::ascii), "200 Type set to: ASCII.");

    ASSERT_EQ(client.get_transfer_type(), ftp::transfer_type::ascii);

    check_reply(client.set_transfer_type(ftp::transfer_type::binary), "200 Type set to: Binary.");

    ASSERT_EQ(client.get_transfer_type(), ftp::transfer_type::binary);

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_status)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.get_status(), CRLF("211-FTP server status:",
                                          " Connected to: 127.0.0.1:2121",
                                          " Waiting for username.",
                                          " TYPE: ASCII; STRUcture: File; MODE: Stream",
                                          " Data connection closed.",
                                          "211 End of status."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_status_directory)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.get_status("."), CRLF(R"(213-Status of "/":)",
                                               "213 End of status."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_status_nonexistent_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.get_status("nonexistent"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_system_type)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    /* pyftpdlib responds '215 UNIX Type: L8' regardless of the system type. */
    check_reply(client.get_system_type(), "215 UNIX Type: L8");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_help)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.get_help(),
                CRLF("214-The following commands are recognized:",
                     " ABOR   ALLO   APPE   CDUP   CWD    DELE   EPRT   EPSV  ",
                     " FEAT   HELP   LIST   MDTM   MFMT   MKD    MLSD   MLST  ",
                     " MODE   NLST   NOOP   OPTS   PASS   PASV   PORT   PWD   ",
                     " QUIT   REIN   REST   RETR   RMD    RNFR   RNTO   SITE  ",
                     " SIZE   STAT   STOR   STOU   STRU   SYST   TYPE   USER  ",
                     " XCUP   XCWD   XMKD   XPWD   XRMD  ",
                     "214 Help command successful."));

    check_reply(client.get_help("ABOR"), "214 Syntax: ABOR (abort transfer).");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, rename)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_reply(client.rename("dir", "new_dir"), CRLF("350 Ready for destination name.",
                                                      "250 Renaming ok."));

    std::istringstream iss("content");
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "new_dir/file"), "226 Transfer complete.");

    check_reply(client.rename("new_dir/file", "new_dir/new_file"), CRLF("350 Ready for destination name.",
                                                                        "250 Renaming ok."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, rename_nonexistent_path)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.rename("nonexistent", "file"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_file_size_nonexistent_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    ftp::file_size_reply reply = client.get_file_size("nonexistent");
    check_reply(reply, "550 /nonexistent is not retrievable.");
    EXPECT_FALSE(reply.get_size().has_value());

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, event_observer)
{
    {
        ftp::client client;
        std::shared_ptr<test_observer> obs1 = std::make_shared<test_observer>("observer1");

        client.add_observer(obs1);

        check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");
        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_EQ("observer1: -- Connected to 127.0.0.1 on port 2121."
                  "observer1: <- 220 FTP server is ready."
                  "observer1: -> QUIT"
                  "observer1: <- 221 Goodbye.", obs1->get_data());

        client.remove_observer(obs1);

        check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");
        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_EQ("observer1: -- Connected to 127.0.0.1 on port 2121."
                  "observer1: <- 220 FTP server is ready."
                  "observer1: -> QUIT"
                  "observer1: <- 221 Goodbye.", obs1->get_data());
    }

    {
        ftp::client client;
        std::shared_ptr<test_observer> obs1 = std::make_shared<test_observer>("observer1");
        std::shared_ptr<test_observer> obs2 = std::make_shared<test_observer>("observer2");

        client.add_observer(obs1);
        client.add_observer(obs2);

        check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");
        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_EQ("observer1: -- Connected to 127.0.0.1 on port 2121."
                  "observer1: <- 220 FTP server is ready."
                  "observer1: -> QUIT"
                  "observer1: <- 221 Goodbye.", obs1->get_data());
        ASSERT_EQ("observer2: -- Connected to 127.0.0.1 on port 2121."
                  "observer2: <- 220 FTP server is ready."
                  "observer2: -> QUIT"
                  "observer2: <- 221 Goodbye.", obs2->get_data());

        client.remove_observer(obs1);

        check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");
        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_EQ("observer1: -- Connected to 127.0.0.1 on port 2121."
                  "observer1: <- 220 FTP server is ready."
                  "observer1: -> QUIT"
                  "observer1: <- 221 Goodbye.", obs1->get_data());
        ASSERT_EQ("observer2: -- Connected to 127.0.0.1 on port 2121."
                  "observer2: <- 220 FTP server is ready."
                  "observer2: -> QUIT"
                  "observer2: <- 221 Goodbye."
                  "observer2: -- Connected to 127.0.0.1 on port 2121."
                  "observer2: <- 220 FTP server is ready."
                  "observer2: -> QUIT"
                  "observer2: <- 221 Goodbye.", obs2->get_data());
    }
}

TEST_F(client, set_transfer_mode)
{
    ftp::client client;

    ASSERT_EQ(ftp::transfer_mode::passive, client.get_transfer_mode());

    client.set_transfer_mode(ftp::transfer_mode::active);

    ASSERT_EQ(ftp::transfer_mode::active, client.get_transfer_mode());
}

TEST_F(client, append_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    {
        std::istringstream iss("content1");
        check_last_reply(client.append_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

        std::ostringstream oss;
        check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");

        ASSERT_EQ("content1", oss.str());
    }

    {
        std::istringstream iss("content2");
        check_last_reply(client.append_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

        std::ostringstream oss;
        check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");

        ASSERT_EQ("content1content2", oss.str());
    }

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, get_file_list_only_names)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    ftp::file_list_reply reply = client.get_file_list(".", true);
    check_last_reply(reply, "226 Transfer complete.");
    ASSERT_THAT(reply.get_file_list(), ElementsAre());

    check_reply(client.create_directory("dir1"), R"(257 "/dir1" directory created.)");
    check_reply(client.create_directory("dir2"), R"(257 "/dir2" directory created.)");

    reply = client.get_file_list(".", true);
    check_last_reply(reply, "226 Transfer complete.");
    ASSERT_THAT(reply.get_file_list(), ElementsAre("dir1", "dir2"));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, upload_unique_file)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    {
        std::istringstream iss("content");
        check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file", true), "226 Transfer complete.");
    }

    ftp::file_list_reply reply = client.get_file_list(".", true);
    check_last_reply(reply, "226 Transfer complete.");
    ASSERT_EQ(1, reply.get_file_list().size());

    {
        std::istringstream iss("content");
        check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file", true), "226 Transfer complete.");
    }

    reply = client.get_file_list(".", true);
    check_last_reply(reply, "226 Transfer complete.");
    ASSERT_EQ(2, reply.get_file_list().size());

    check_reply(client.disconnect(), "221 Goodbye.");
}

class client_with_transfer_mode : public client,
                                  public testing::WithParamInterface<ftp::transfer_mode>
{
};

INSTANTIATE_TEST_SUITE_P(all_modes, client_with_transfer_mode, testing::Values(ftp::transfer_mode::active,
                                                                               ftp::transfer_mode::passive));

TEST_P(client_with_transfer_mode, get_file_list)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_reply(client.create_directory("dir"), R"(257 "/dir" directory created.)");

    check_last_reply(client.get_file_list(), "226 Transfer complete.");

    check_last_reply(client.get_file_list("dir"), "226 Transfer complete.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_P(client_with_transfer_mode, get_file_list_nonexistent_dir)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    check_last_reply(client.get_file_list("nonexistent"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_P(client_with_transfer_mode, upload_file_nonexistent_path)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::istringstream iss("content");
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "nonexistent/file"), "550 No such file or directory.");

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_P(client_with_transfer_mode, download_nonexistent_file)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::ostringstream oss;
    check_last_reply(client.download_file(ftp::ostream_adapter(oss), "nonexistent"), "550 No such file or directory.");
    ASSERT_TRUE(oss.str().empty());

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_P(client_with_transfer_mode, cancel_upload_file)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::istringstream iss("content");
    test_cancel_callback cancel_cb;
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file", false, &cancel_cb),
                     "225 ABOR command successful; data channel closed.");
}

TEST_P(client_with_transfer_mode, cancel_append_file)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::istringstream iss("content");
    test_cancel_callback cancel_cb;
    check_last_reply(client.append_file(ftp::istream_adapter(iss), "file", &cancel_cb),
                     "225 ABOR command successful; data channel closed.");
}

class client_binary_transfer : public client,
                               public testing::WithParamInterface<std::tuple<ftp::transfer_mode, std::string>>
{
};

INSTANTIATE_TEST_SUITE_P(main_dataset, client_binary_transfer,
                         testing::Combine(testing::Values(ftp::transfer_mode::active,
                                                          ftp::transfer_mode::passive),
                                          testing::Values("",
                                                          "\r",
                                                          "\n",
                                                          "\r\n",
                                                          "content",
                                                          "\rcon\ntent\r\n",
                                                          std::string(512, 'a'),
                                                          std::string(4096, 'a'),
                                                          std::string(8192, 'a'),
                                                          std::string(10000, 'a'),
                                                          std::string(25000, 'a'))));

TEST_P(client_binary_transfer, upload_download_file)
{
    auto [mode, data] = GetParam();
    ftp::client client(mode, ftp::transfer_type::binary);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::istringstream iss(data);
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

    std::uint64_t expected_size = data.size();
    ftp::file_size_reply reply = client.get_file_size("file");
    check_reply(reply, "213 " + std::to_string(expected_size));
    EXPECT_TRUE(reply.get_size().has_value());
    EXPECT_EQ(expected_size, reply.get_size().value());

    std::ostringstream oss;
    check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");
    ASSERT_EQ(data, oss.str());

    check_reply(client.disconnect(), "221 Goodbye.");
}

class client_ascii_transfer : public client,
                              public testing::WithParamInterface<std::tuple<ftp::transfer_mode,
                                                                            std::pair<std::string, std::string>>>
{
};

INSTANTIATE_TEST_SUITE_P(main_dataset, client_ascii_transfer,
                         testing::Combine(testing::Values(ftp::transfer_mode::active,
                                                          ftp::transfer_mode::passive),
                                          testing::Values(std::make_pair("", ""),
                                                          std::make_pair("\r", "\n"),
                                                          std::make_pair("\n", "\n"),
                                                          std::make_pair("\r\n", "\n"),
                                                          std::make_pair("content", "content"),
                                                          std::make_pair("\rcon\ntent\r\n", "\ncon\ntent\n"),
                                                          std::make_pair(std::string(512, 'a'), std::string(512, 'a')),
                                                          std::make_pair(std::string(4096, 'a'), std::string(4096, 'a')),
                                                          std::make_pair(std::string(8192, 'a'), std::string(8192, 'a')),
                                                          std::make_pair(std::string(10000, 'a'), std::string(10000, 'a')),
                                                          std::make_pair(std::string(25000, 'a'), std::string(25000, 'a')))));

TEST_P(client_ascii_transfer, upload_download_file)
{
    auto [mode, data] = GetParam();
    auto [content, expected] = data;
    ftp::client client(mode, ftp::transfer_type::ascii);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: ASCII."));

    std::istringstream iss(content);
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

    std::ostringstream oss;
    check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");

    if (is_windows_platform())
    {
        ASSERT_EQ(content, oss.str());
    }
    else
    {
        ASSERT_EQ(expected, oss.str());
    }

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, IPv6_open_connection)
{
    ftp::client client;

    check_reply(client.connect("::1", 2121), "220 FTP server is ready.");

    check_reply(client.get_status(), CRLF("211-FTP server status:",
                                          " Connected to: ::1:2121",
                                          " Waiting for username.",
                                          " TYPE: ASCII; STRUcture: File; MODE: Stream",
                                          " Data connection closed.",
                                          "211 End of status."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_P(client_with_transfer_mode, IPv6_upload_download_file)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    check_reply(client.connect("::1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::string data = "content";
    std::istringstream iss(data);
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

    std::ostringstream oss;
    check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");
    ASSERT_EQ(data, oss.str());

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(client, configure_rfc2428_support)
{
    ftp::client client;

    EXPECT_TRUE(client.get_rfc2428_support());

    client.set_rfc2428_support(false);
    EXPECT_FALSE(client.get_rfc2428_support());

    client.set_rfc2428_support(true);
    EXPECT_TRUE(client.get_rfc2428_support());
}

TEST_P(client_with_transfer_mode, disable_rfc2428_support)
{
    ftp::transfer_mode mode = GetParam();
    ftp::client client(mode);

    client.set_rfc2428_support(false);

    check_reply(client.connect("127.0.0.1", 2121), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("331 Username ok, send password.",
                                                       "230 Login successful.",
                                                       "200 Type set to: Binary."));

    std::string data = "content";
    std::istringstream iss(data);
    check_last_reply(client.upload_file(ftp::istream_adapter(iss), "file"), "226 Transfer complete.");

    std::ostringstream oss;
    check_last_reply(client.download_file(ftp::ostream_adapter(oss), "file"), "226 Transfer complete.");
    ASSERT_EQ(data, oss.str());

    check_reply(client.disconnect(), "221 Goodbye.");
}

class ssl_client : public client_base<2142, true>
{
};

TEST_F(ssl_client, open_connection)
{
    ftp::client client;

    ASSERT_FALSE(client.is_connected());

    for (int i = 0; i < 5; i++)
    {
        check_reply(client.connect("127.0.0.1", 2142), CRLF("220 FTP server is ready.",
                                                            "234 AUTH TLS successful."));
        ASSERT_TRUE(client.is_connected());

        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_FALSE(client.is_connected());
    }

    for (int i = 0; i < 5; i++)
    {
        check_reply(client.connect("127.0.0.1", 2142, "alice", "password"), CRLF("220 FTP server is ready.",
                                                                                 "234 AUTH TLS successful.",
                                                                                 "331 Username ok, send password.",
                                                                                 "230 Login successful.",
                                                                                 "200 Type set to: Binary."));
        ASSERT_TRUE(client.is_connected());

        check_reply(client.disconnect(), "221 Goodbye.");
        ASSERT_FALSE(client.is_connected());
    }
}

TEST_F(ssl_client, login_without_ssl)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2142), "220 FTP server is ready.");

    check_reply(client.login("user", "password"), CRLF("550 SSL/TLS required on the control channel."));

    check_reply(client.disconnect(), "221 Goodbye.");
}

TEST_F(ssl_client, get_help)
{
    ftp::client client;

    check_reply(client.connect("127.0.0.1", 2142), "220 FTP server is ready.");

    check_reply(client.get_help(),
        CRLF("214-The following commands are recognized:",
             " ABOR   ALLO   APPE   AUTH   CDUP   CWD    DELE   EPRT  ",
             " EPSV   FEAT   HELP   LIST   MDTM   MFMT   MKD    MLSD  ",
             " MLST   MODE   NLST   NOOP   OPTS   PASS   PASV   PBSZ  ",
             " PORT   PROT   PWD    QUIT   REIN   REST   RETR   RMD   ",
             " RNFR   RNTO   SITE   SIZE   STAT   STOR   STOU   STRU  ",
             " SYST   TYPE   USER   XCUP   XCWD   XMKD   XPWD   XRMD  ",
             "214 Help command successful."));

    check_reply(client.get_help("AUTH"), "214 Syntax: AUTH <SP> TLS|SSL (set up secure control channel).");

    check_reply(client.disconnect(), "221 Goodbye.");
}

} // namespace
