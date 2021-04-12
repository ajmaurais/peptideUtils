//
// exceptions.hpp
// utils
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

//! \file This file contains implementations of custom exceptions raised by the library.
#ifndef UTILS_EXCEPTIONS_HPP
#define UTILS_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <utility>

namespace utils {

    class BaseException: public std::exception {
    private:
        std::string _message;
    public:
        explicit BaseException(std::string message) : _message(std::move(message)){}
        const char* what() const noexcept override {
            return _message.c_str();
        }
    };

    //! Feature/function not implemented
    class NotImplemented : public BaseException {
        explicit NotImplemented(const std::string &message) : BaseException(message) {}
    };
    //! Error reading/writing file.
    class FileIOError : public BaseException {
    public:
        explicit FileIOError(const std::string &message) : BaseException(message) {}
    };
    //! Error parsing xml file
    class InvalidXmlFile : public BaseException {
    public:
        explicit InvalidXmlFile(const std::string &message) : BaseException(message) {}
    };

    //class IOException
}

#endif //UTILS_EXCEPTIONS_HPP
