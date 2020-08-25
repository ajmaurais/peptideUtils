//
// utils.hpp
// ionFinder
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

#ifndef ms2_hpp
#define ms2_hpp

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <vector>
#include <string>
#include <map>

#include <utils.hpp>
#include <msInterface/msInterface.hpp>
#include <msInterface/msScan.hpp>

namespace utils {
	
	int const MD_NUM = 2;
	size_t const SCAN_INDEX_NOT_FOUND = std::string::npos;

    class Ms2File;
	class Ms2File : public utils::MsInterface{
	private:

		bool getMetaData();

		void _buildIndex() override;
		size_t _getScanIndex(size_t) const;
		
	public:

		Ms2File(std::string fname = "") : MsInterface(fname){
			initMetadata();
		}
		~Ms2File(){}
		
		bool read() override;
		bool read(std::string fname) override;
		
		//properties
		bool getScan(std::string, Scan&) const;
		bool getScan(size_t, Scan&) const override;
	};
		
}//end of namespace

#endif /* ms2_hpp */
