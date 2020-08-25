//
// mzXMLFile.hpp
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

#ifndef mzXMLFile_hpp
#define mzXMLFile_hpp

#include <regex>

#include <utils.hpp>
#include <msInterface/msInterface.hpp>
#include <msInterface/internal/base64_utils.hpp>
#include <msInterface/internal/xml_utils.hpp>
#include <thirdparty/rapidxml/rapidxml.hpp>
#include <exceptions.hpp>

namespace utils{
    class MzXMLFile;

    class MzXMLFile: public utils::MsInterface{
    private:
        void _buildIndex() override;

        static const std::string _scanBegin;
        static const std::string _scanEnd;
    public:

        //properties
        bool getScan(size_t, Scan&) const override;
    };
}

#endif
