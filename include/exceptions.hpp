//
// Created by Aaron Maurais on 8/10/20.
//

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
        explicit BaseException(std::string  message) : _message(std::move(message)){}
        const char* what() const noexcept override {
            return _message.c_str();
        }
    };

    class NotImplemented : public BaseException {
        explicit NotImplemented(const std::string &message) : BaseException(message) {}
    };
    class FileIOError : public BaseException {
    public:
        explicit FileIOError(const std::string &message) : BaseException(message) {}
    };
    class InvalidXmlFile : public BaseException {
    public:
        explicit InvalidXmlFile(const std::string &message) : BaseException(message) {}
    };

    //class IOException
}

#endif //UTILS_EXCEPTIONS_HPP
