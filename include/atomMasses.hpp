//
//  atomMasses.hpp
//  utils
// -----------------------------------------------------------------------------
// Copyright 2018 Aaron Maurais
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

#ifndef UTILS_ATOMMASSES_HPP
#define UTILS_ATOMMASSES_HPP

#include <map>

#include <species.hpp>

namespace utils{

    typedef std::map<std::string, Species> AtomMassMapType;

    const AtomMassMapType ATOM_MASSES {
        {"C", {12.011, 12} },
        {"H", {1.008, 1.00783}},
        {"O", {15.999, 15.99491}},
        {"N", {14.007, 14.00307}},
        {"S", {32.06, 31.97207}},
        {"P", {30.97376, 30.97376}},
        {"(15)N", {15.00011, 15.00011}},
        {"D", {2.0141, 2.0141}},
        {"(13)C", {13.00335, 13.00335}},
        {"Se", {78.96, 79.91652}},
        {"Cl", {35.45, 34.96885}},
        {"Br", {79.904, 78.91834}}
    };

} //end of namespace utils

#endif //UTILS_ATOMMASSES_HPP
