//
//  utils.hpp
//  utils
// -----------------------------------------------------------------------------
// Copyright 2018 Aaron maurais
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

#ifndef utils_hpp
#define utils_hpp

#include <iostream>
#include <cassert>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <cctype>
#include <type_traits>

#ifndef PATH_MAX
	#define PATH_MAX 1024
#endif

#ifndef IN_DELIM
	#define IN_DELIM '\t'
#endif
#ifndef OUT_DELIM
	#define OUT_DELIM '\t'
#endif
#ifndef NEW_LINE
	#define NEW_LINE '\n'
#endif

namespace utils{
	
	/******************************/
	/* namespace scoped constants */
	/*****************************/
	
	std::string const WHITESPACE = " \f\n\r\t\v";
	std::string const COMMENT_SYMBOL = "#";
	bool const IGNORE_HIDDEN_FILES = true; //ignore hidden files in utils::ls
	int const PROGRESS_BAR_WIDTH = 60;
	std::string const SUBSCRIPT_MAP [] = {"\u2080", "\u2081", "\u2082", "\u2083",
		"\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089"};
	
	/*************/
	/* functions */
	/*************/
	
	//file utils
	void readBuffer(std::string fname, char** buffer, size_t& size);
	bool dirExists(const char*);
	bool dirExists(std::string);
	bool fileExists(const char*);
	bool fileExists(std::string);
	bool isDir(const char*);
	bool isDir(std::string);
	std::string pwd();
	std::string absPath(std::string);
	std::string absPath(const char*);
	bool ls(const char*, std::vector<std::string>&);
	bool ls(const char*, std::vector<std::string>&, std::string);
	bool mkdir(std::string);
	bool mkdir(const char*);
	void systemCommand(std::string command);
	std::string baseName(std::string path, const std::string& delims = "/\\");
	std::string dirName(std::string path, const std::string& delims = "/\\");
	std::string parentDir(std::string path, char delim = '/');
	std::string removeExtension(const std::string&);
	std::string getExtension(const std::string&);
	std::istream& safeGetline(std::istream& is, std::string& t);
	std::istream& safeGetline(std::istream& is, std::string& t, std::streampos& oldPos);
		
	//string utils
	bool strContains(std::string, std::string);
	bool strContains(char, std::string);
	bool startsWith(std::string whithinStr, std::string findStr);
	bool endsWith(std::string whithinStr, std::string findStr);
	void split (const std::string&, const char, std::vector<std::string>&);
	std::string trimTraling(const std::string&);
	std::string trimLeading(const std::string&);
	std::string trim(const std::string&);
	void trimAll(std::vector<std::string>&);
	bool isCommentLine(std::string);
	std::string removeSubstr(std::string,std::string);
	std::string removeChars(char,std::string);
	std::string toLower(std::string);
	std::string repeat(std::string, size_t);
	size_t offset(const char* buf, size_t len, const char* str);
	size_t offset(const char* buf, size_t len, std::string s);
	void removeEmptyStrings(std::vector<std::string>&);
	void getIdxOfSubstr(char*, const char*, std::vector<size_t>&);
	std::string toSubscript(int);
	void addChar(std::string toAdd, std::string& s, std::string delim = "|");
	
	//other
	bool isInteger(const std::string & s);
	bool isFlag(const char*);
	bool isArg(const char*);
	void printProgress(float progress, std::ostream& out, int barWidth = PROGRESS_BAR_WIDTH);
	void printProgress(float progress, int barWidth = PROGRESS_BAR_WIDTH);
	std::string ascTime();
	template <typename _Tp> int round(_Tp num){
		return floor(num + 0.5);
	}

	/**
	 \brief Concat elements between \p begin and \p end into a single string. <br>
	 
	 This function calls utils::addChar to combine the elements deferenced by the
	 pointers. Using iterators which do not point to strings causes undefined
	 behavior.
	 
	 \param begin where to begin
	 \param end where to end
	 \param delim how to seperate elements
	 \return elements concated to a single string
	 */
	template <typename Iterator>
	std::string concat(Iterator begin, Iterator end, std::string delim = "|")
	{
		std::string ret;
		for(auto it = begin; it != end; ++it)
			addChar(*it, ret, "|");
		return ret;
	}
	
	/**
	 Get number of digits in an integer.
	 \param num number to count digits of
	 \pre \p num must be an integer type
	 \return number of digits in \p num
	 */
	template <typename _Tp> int numDigits(_Tp num){
		static_assert(std::is_integral<_Tp>::value, "num must be integer");
		int count = 0;
		while (num != 0) {
			count++;
			num /= 10;
		}
		return count;
	}
	
	/**
	 Determine whether compare is between compare - range and compare + range.
	 \param value reference value
	 \param compare value to search for in range
	 \param range the range
	 \return True if compare is in range
	 */
	template<typename _Tp> bool inRange(_Tp value, _Tp compare, _Tp range){
		return abs(value - compare) <= range;
	}
	
	/**
	 Determine whether comp is beg <= comp and comp <= end
	 \param beg beginning of range
	 \param end end of range
	 \param comp the value to search for in range
	 \return True if comp is in range
	 */
	template<typename _Tp> bool inSpan(_Tp beg, _Tp end, _Tp comp){
		return beg <= comp && comp <= end;
	}
	int getInt(int min, int max);
	
	/**
	 Get underlying integer value for enum class
	 \param value enum class value
	 \return integer value of \p value
	 */
	template <typename Enumeration>
	auto as_integer(Enumeration const value)-> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}
}

#endif /* utils_hpp */

