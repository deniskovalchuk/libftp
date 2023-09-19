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

#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "command.hpp"
#include "utils.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <ftp/client.hpp>
#include <ftp/observer.hpp>

class command_handler
{
public:
    command_handler();

    void handle(command command, const std::vector<std::string> & args);

private:
    void open(const std::vector<std::string> & args);

    void mode();

    void active();

    void passive();

    void user(const std::vector<std::string> & args);

    void cd(const std::vector<std::string> & args);

    void cdup();

    void ls(const std::vector<std::string> & args);

    void put(const std::vector<std::string> & args);

    void get(const std::vector<std::string> & args);

    void rename(const std::vector<std::string> & args);

    void pwd();

    void mkdir(const std::vector<std::string> & args);

    void rmdir(const std::vector<std::string> & args);

    void del(const std::vector<std::string> & args);

    void type();

    void binary();

    void ascii();

    void size(const std::vector<std::string> & args);

    void stat(const std::vector<std::string> & args);

    void syst();

    void noop();

    void rhelp(const std::vector<std::string> & args);

    void logout();

    void close();

    void exit();

    static void help();

    class stdout_writer : public ftp::observer
    {
    public:
        void on_reply(const ftp::reply & reply) override
        {
            std::cout << reply.get_status_string() << std::endl;
        }

        void on_file_list(std::string_view file_list) override
        {
            std::cout << file_list;
            std::cout.flush();
        }
    };

    ftp::client ftp_client_;
};

#endif //COMMAND_HANDLER_HPP
