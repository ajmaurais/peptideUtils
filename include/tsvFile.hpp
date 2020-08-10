// 
// tsvFile.hpp
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

#ifndef tsvFile_hpp
#define tsvFile_hpp

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <utils.hpp>

namespace utils{
	
	class TsvFile;
	
	class TsvFile{
	private:
		typedef std::vector<std::string> rowType;
		typedef std::vector<rowType> datType;
		datType _dat;
		
		typedef std::map<std::string, size_t> colMapType;
		colMapType _colNames;
		
		std::string _fname;
		bool _hasHeaders;
		bool _caseSensitive;
		bool _quote;
		char _delim;
		size_t _nCol;
		size_t _nRow;
		std::string _blank;
		
		//private cell access functions
		std::string _getVal(size_t row, std::string colName) const{
			return _getVal(row, getColIndex(colName));
		}
		std::string _getVal(size_t row, size_t col) const{
			return _dat[col][row];
		}
		
	public:
		TsvFile(){
			_fname = "";
			_hasHeaders = true;
			_quote = false;
			_delim = '\t';
			_nCol = 0; _nRow = 0;
			_blank = "";
		}
		/**
		 Constructor with options.
		 
		 \param fname File name to read.
		 \param caseSensitive Should column headers be case sensitive?
		 \param hasHeaders Does the first row contain column headers?
		 \param delim Column delimiter.
		 \param quote Are columns surrounded by quotes?
		 \param blank What value should be stored for blank columns?
		 */
		TsvFile(std::string fname, bool caseSensitive = false, bool hasHeaders = true,
				char delim = IN_DELIM, bool quote = false, std::string blank = ""){
			_fname = fname;
			_hasHeaders = hasHeaders;
			_caseSensitive = caseSensitive;
			_delim = delim;
			_quote = false;
			_nCol = 0; _nRow = 0;
			_blank = blank;
		}
		
		//modifiers
		bool read(std::string fname){
			_fname = fname;
			return read();
		}
		bool read();
		bool write(std::ostream& out, char delim = '\t');
		bool write(std::string ofname, char delim = '\t');
		
		//access to multiple cells
		void getRow(size_t index, std::vector<std::string>& elems) const;
		std::string getRow(size_t index) const;
		std::vector<std::string> getCol(std::string rowName) const;
		datType& getDat(){
			return _dat;
		}
		
		//individual cell access
		std::string getValStr(size_t row, size_t col) const{
			return _getVal(row, col);
		}
		std::string getValStr(size_t row, std::string colName) const{
			return _getVal(row, colName);
		}
		double getValDouble(size_t row, size_t col) const{
			return atof(_getVal(row, col).c_str());
		}
		double getValDouble(size_t row, std::string colName) const{
			return atof(_getVal(row, colName).c_str());
		}
		int getValInt(size_t row, size_t col) const{
			return atoi(_getVal(row, col).c_str());
		}
		int getValInt(size_t row, std::string colName) const{
			return atoi(_getVal(row, colName).c_str());
		}
		size_t getValSize(size_t row, size_t col) const{
			return std::stoi(_getVal(row, col));
		}
		size_t getValSize(size_t row, std::string colName) const{
			return std::stoi(_getVal(row, colName));
		}
		bool getValBool(size_t row, size_t col) const{
			return atoi(_getVal(row, col).c_str());
		}
		bool getValBool(size_t row, std::string colName) const{
			return atoi(_getVal(row, colName).c_str());
		}
		
		//metadata access
		long getColIndex(std::string colName) const{
			if(!_caseSensitive) colName = utils::toLower(colName);
			colMapType::const_iterator it = _colNames.find(colName);
			if(it == _colNames.end())
				return -1;
			return it->second;
		}
		bool colExists(std::string search) const{
			if(!_caseSensitive) search = utils::toLower(search);
			colMapType::const_iterator it = _colNames.find(search);
			return it != _colNames.end();
		}
		size_t getNrow() const{
			return _nRow;
		}
		size_t getNcol() const{
			return _nCol;
		}
	};
}

#endif /* tsvFile_hpp */
