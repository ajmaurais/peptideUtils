//
// Created by Aaron Maurais on 8/10/20.
//

#ifndef UTILS_EXCEPTIONS_HPP
#define UTILS_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace utils {

    class BaseException : public std::exception
    {
        std::string what_message;
    public:
        const char* what() {
            return what_message.c_str();
        }
    };

    class NotImplemented : public BaseException {};
    class FileIOError : public BaseException {};

    //class IOException
}

#endif //UTILS_EXCEPTIONS_HPP
