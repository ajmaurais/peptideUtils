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

bool utils::internal::_isAttr(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

bool utils::internal::_isAttr(const char *s1, const rapidxml::xml_attribute<>* attr){
    return strcmp(s1, attr->name()) == 0;
}

bool utils::internal::_isVal(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

bool utils::internal::_isVal(const char* s1, const rapidxml::xml_attribute<>* attr) {
    return strcmp(s1, attr->value()) == 0;
}

int utils::internal::_getAttrValInt(const char* name, const rapidxml::xml_node<>* node){
    auto* attr = node->first_attribute(name);
    try{
        if(attr) return std::stoi(attr->value());
    } catch(std::invalid_argument& e){
        throw utils::InvalidXmlFile("Attribute value has incorrect type: " + std::string(name)
                                    + "=\"" + std::string(attr->value(), attr->value_size()) + "\"");
    }
    throw utils::InvalidXmlFile("Required attribute: \'" + std::string(name) + "\' not found.");
}

size_t utils::internal::_getAttrValUL(const char* name, const rapidxml::xml_node<>* node){
    auto* attr = node->first_attribute(name);
    try {
        if (attr) return std::stoi(attr->value());
    } catch(std::invalid_argument& e){
        throw utils::InvalidXmlFile("Attribute value has incorrect type: " + std::string(name)
                                    + "=\"" + std::string(attr->value(), attr->value_size()) + "\"");
    }
    throw utils::InvalidXmlFile("Required attribute: \'" + std::string(name) + "\' not found.");
}

std::string utils::internal::_getAttrValStr(const char* name, const rapidxml::xml_node<>* node) {
    auto* attr = node->first_attribute(name);
    if(attr) return std::string(attr->value(), attr->value_size());
    else throw utils::InvalidXmlFile("Required attribute: \'" + std::string(name) + "\' not found.");
}

double utils::internal::_getAttrValdouble(const char* name, const rapidxml::xml_node<>* node) {
    auto* attr = node->first_attribute(name);
    if(attr) return std::stod(attr->value());
    else throw utils::InvalidXmlFile("Required attribute: \'" + std::string(name) + "\' not found.");
}

rapidxml::xml_node<>* utils::internal::_getFirstChildNode(const char* name, rapidxml::xml_node<>* node) {
    auto* ret = node->first_node(name);
    if(ret) return ret;
    else throw utils::InvalidXmlFile("Required node: \'" + std::string(name) + "\' not found.");
}

bool utils::internal::_checkAttrVal(const char* name,
                          const char* expected,
                          const rapidxml::xml_attribute<>* attr,
                          size_t scanNum)
{
    if(utils::internal::_isAttr(name, attr)){
        if(!utils::internal::_isVal(expected, attr)) {
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
double utils::internal::_xs_duration_to_seconds(char* xs, size_t len)
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

/**
 * Convert time value accession to seconds.
 * @param value Time value in units of \p accession.
 * @param accession Valid time value accession.
 * @return Time in seconds.
 * @throws std::invalid_argument if unknown time value accession.
 */
double utils::internal::_obo_to_seconds(double value, const std::string& accession)
{
    if(accession == "UO:0000031") return value * 60; //minutes
    else if(accession == "UO:0000010") return value; //seconds
    else if(accession == "UO:0000028") return value / 1e3; //millisecond
    else if(accession == "UO:0000029") return value / 1e6; //microsecond
    else if(accession == "UO:0000150") return value / 1e9; //nanosecond
    else if(accession == "UO:0000030") return value / 1e12; //nanosecond
    else if(accession == "UO:0000032") return value * 3600; //hours
    else throw std::invalid_argument("Unknown time unit accession: " + accession);
}