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

#include "command_parser.hpp"
#include "cmdline_exception.hpp"
#include <sstream>
#include <iomanip>
#include <boost/algorithm/string/predicate.hpp>

static command get_command_from_string(std::string_view str)
{
    if (boost::iequals(str, "open"))
    {
        return command::open;
    }
    else if (boost::iequals(str, "mode"))
    {
        return command::mode;
    }
    else if (boost::iequals(str, "active"))
    {
        return command::active;
    }
    else if (boost::iequals(str, "passive"))
    {
        return command::passive;
    }
    else if (boost::iequals(str, "user"))
    {
        return command::user;
    }
    else if (boost::iequals(str, "logout"))
    {
        return command::logout;
    }
    else if (boost::iequals(str, "close"))
    {
        return command::close;
    }
    else if (boost::iequals(str, "cd"))
    {
        return command::cd;
    }
    else if (boost::iequals(str, "cdup"))
    {
        return command::cdup;
    }
    else if (boost::iequals(str, "ls"))
    {
        return command::ls;
    }
    else if (boost::iequals(str, "put"))
    {
        return command::put;
    }
    else if (boost::iequals(str, "get"))
    {
        return command::get;
    }
    else if (boost::iequals(str, "rename"))
    {
        return command::rename;
    }
    else if (boost::iequals(str, "pwd"))
    {
        return command::pwd;
    }
    else if (boost::iequals(str, "mkdir"))
    {
        return command::mkdir;
    }
    else if (boost::iequals(str, "rmdir"))
    {
        return command::rmdir;
    }
    else if (boost::iequals(str, "del"))
    {
        return command::del;
    }
    else if (boost::iequals(str, "stat"))
    {
        return command::stat;
    }
    else if (boost::iequals(str, "syst"))
    {
        return command::syst;
    }
    else if (boost::iequals(str, "type"))
    {
        return command::type;
    }
    else if (boost::iequals(str, "binary"))
    {
        return command::binary;
    }
    else if (boost::iequals(str, "ascii"))
    {
        return command::ascii;
    }
    else if (boost::iequals(str, "size"))
    {
        return command::size;
    }
    else if (boost::iequals(str, "noop"))
    {
        return command::noop;
    }
    else if (boost::iequals(str, "rhelp"))
    {
        return command::rhelp;
    }
    else if (boost::iequals(str, "help"))
    {
        return command::help;
    }
    else if (boost::iequals(str, "exit"))
    {
        return command::exit;
    }
    else
    {
        throw cmdline_exception("Invalid command.");
    }
}

std::pair<command, std::vector<std::string>> parse_command(const std::string & line)
{
    std::istringstream iss(line);
    std::string command_str;

    iss >> command_str;

    command command = get_command_from_string(command_str);

    std::vector<std::string> args;
    std::string arg;

    while (iss >> std::quoted(arg))
    {
        args.push_back(arg);
    }

    return std::make_pair(command, args);
}
