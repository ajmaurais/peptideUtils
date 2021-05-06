// 
// bufferFile.cpp
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

#include <bufferFile.hpp>

/**
 \brief constructor
 \param fname path of file to be read
 */
utils::BufferFile::BufferFile(std::string fname)
{
    _fname = fname;
    _size = 0;
    _buffer = new char[_size];
}

/**
 \brief copy constructor
 \param rhs object to copy
 */
utils::BufferFile::BufferFile(const utils::BufferFile& rhs)
{
    //copy buffer
    _buffer = new char[rhs._size];
    std::copy(rhs._buffer, rhs._buffer + rhs._size, _buffer);
    
    //other vars
    _fname = rhs._fname;
    _size = rhs._size;
}

/**
 \brief copy assignment
 \param rhs object to copy
 */
utils::BufferFile& utils::BufferFile::operator = (utils::BufferFile rhs)
{
    std::swap(_buffer, rhs._buffer);
    
    //other vars
    _fname = rhs._fname;
    _size = rhs._size;
    return *this;
}

/**
 \brief Read contents of \p fname into FileBuffer::_buffer
 \param fname path of file to read
 \return true if successful
 */
bool utils::BufferFile::read(std::string fname)
{
    _fname = fname;
    return read();
}

/**
 \brief Read contents of FileBuffer::_fname into FileBuffer::_buffer
 \pre FileBuffer::_fname is not empty
 \return true if successful
 */
bool utils::BufferFile::read()
{
    std::ifstream inF(_fname);
    if(!inF) return false;
    
    utils::readBuffer(_fname, &_buffer, _size);
    return true;
}

/**
\brief Determine if BufferFile::_buffer is empty. <br>

Function simply returns true if BufferFile::_size == 0.
At some point maybe I will implement a more rigorous test.
\return true if BufferFile::_buffer is empty.
*/
bool utils::BufferFile::buffer_empty() const{
    return _size == 0;
}

//! Test if file exists.
bool utils::BufferFile::exists() const {
    return utils::fileExists(_fname);
}
