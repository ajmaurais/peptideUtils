//
// xml_utils.cpp
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

#include <msInterface/internal/xml_utils.hpp>

bool utils::_isAttr(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

bool utils::_isVal(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

bool utils::_checkAttrVal(const char* name,
                          const char* expected,
                          const rapidxml::xml_attribute<>* attr,
                          size_t scanNum)
{
    if(utils::_isAttr(name, attr->name())){
        if(!utils::_isVal(expected, attr->value())) {
            std::cerr << "Invalid " << std::string(name) << ": " <<
                      std::string(attr->value(), attr->value_size()) << ", in scan" <<
                      std::to_string(scanNum) << NEW_LINE;
            return false;
        }
    }
    return true;
}

/**
 * Convert xs:duration to seconds.
 * @param xs xs:duration string.
 * @param len Length of data.
 * @return Seconds.
 */
double utils::_xs_duration_to_seconds(char* xs, size_t len)
{
    bool positive = true;
    if(xs[0] != 'P') {
        if (xs[0] == '-') {
            positive = false;
        }
        else throw std::runtime_error("Invalid xs:duration: " + std::string(xs, len));
    }
    double ret = 0;
    std::string tmp = "";
    bool inTime = false;
    for(size_t i = 1; i < len; i++)
    {
        switch(xs[i]) {
            case 'Y':
                ret += std::stod(tmp) * 3.154e+7;
                tmp = "";
                break;
            case 'M':
                ret += std::stod(tmp) * (inTime ? 2.628e+6: 60);
                tmp = "";
                break;
            case 'D':
                ret += std::stod(tmp) * 86400;
                tmp = "";
                break;
            case 'T':
                inTime = true;
                break;
            case 'H':
                ret += std::stod(tmp) * 3600;
                tmp = "";
                break;
            case 'S':
                ret += std::stod(tmp);
                tmp = "";
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                tmp += xs[i];
                break;
        }
    }
    return positive ? ret: -1 * ret;
}
