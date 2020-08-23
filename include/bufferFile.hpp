// 
// fileBuffer.hpp
// utils
// -----------------------------------------------------------------------------
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

#ifndef fileBuffer_hpp
#define fileBuffer_hpp

#include <string>
#include <utils.hpp>

namespace utils{
	class BufferFile;
	
	//!Base class for reading and manipulating large file buffers.
	class BufferFile{
	protected:
		//!file path
		std::string _fname;
		
		//!file buffer
		char* _buffer;
		
		//!_buffer length in chars
		size_t _size;
	public:
		BufferFile(std::string fname = "");
		BufferFile(const BufferFile& rhs);
		
		~BufferFile(){
			delete [] _buffer;
		}
		
		//modifiers
		BufferFile& operator = (BufferFile rhs);
		bool read();
		bool read(std::string);
		bool exists() const;

		//properties
		bool buffer_empty() const;
	};
}

#endif /* fileBuffer_hpp */
