//
// xml_utils.hpp
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

#ifndef xml_utils_hpp
#define xml_utils_hpp

#include <exception>
#include <string>

#include <utils.hpp>
#include <exceptions.hpp>
#include <thirdparty/rapidxml/rapidxml.hpp>

namespace utils {
    namespace internal {
        bool _isVal(const char* s1, const char* s2);
        bool _isVal(const char* s1, const rapidxml::xml_attribute<>* attr);
        bool _isAttr(const char* s1, const char* s2);
        bool _isAttr(const char* s1, const rapidxml::xml_attribute<>* attr);
        bool _checkAttrVal(const char* name, const char* expected, const rapidxml::xml_attribute<>* attr, size_t scanNum);
        double _xs_duration_to_seconds(char* xs, size_t len);
        double _obo_to_seconds(double value, const std::string& accession);
        int _getAttrValInt(const char* name, const rapidxml::xml_node<>* node);
        size_t _getAttrValUL(const char* name, const rapidxml::xml_node<>* node);
        std::string _getAttrValStr(const char* name, const rapidxml::xml_node<>* node);
        double _getAttrValdouble(const char* name, const rapidxml::xml_node<>* node);
        rapidxml::xml_node<>* _getFirstChildNode(const char* name, rapidxml::xml_node<>* node);
    }
}

#endif
