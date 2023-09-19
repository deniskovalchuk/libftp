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

#include "transfer_callback.hpp"
#include <iostream>

transfer_callback::transfer_callback()
    : line_len_(0)
{}

void transfer_callback::begin()
{
    time_ = std::chrono::steady_clock::now();

    std::string str = "Transmitting data...";
    line_len_ = str.size();

    std::cout << str;
    std::cout.flush();
}

void transfer_callback::notify(std::size_t bytes_transferred)
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::seconds elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - time_);

    if (elapsed.count() > 1)
    {
        time_ = now;

        if (line_len_ >= 80)
        {
            line_len_ = 0;
            std::cout << std::endl;
        }

        line_len_++;
        std::cout << ".";
        std::cout.flush();
    }
}

void transfer_callback::end()
{
    std::cout << std::endl;
}
