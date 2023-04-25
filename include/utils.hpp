// 
// utils.hpp
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
#include <set>
#include <tuple>
#include <cfloat>

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
    //!ignore hidden files in utils::ls
    bool const IGNORE_HIDDEN_FILES = true;
    int const PROGRESS_BAR_WIDTH = 60;
    std::string const SUBSCRIPT_MAP [] = {"\u2080", "\u2081", "\u2082", "\u2083",
        "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089"};
    
    /*************/
    /* functions */
    /*************/
    
    //file utils
    void readBuffer(const std::string& fname, char** buffer, std::streamsize& size);
    bool dirExists(const char*);
    bool dirExists(const std::string&);
    bool fileExists(const char*);
    bool fileExists(const std::string&);
    bool isDir(const char*);
    bool isDir(const std::string&);
    bool isFile(const std::string&);
    bool isFile(const char*);
    std::string pwd();
    std::string absPath(const std::string&);
    std::string absPath(const char*);
    bool ls(const char*, std::vector<std::string>&);
    bool ls(const char*, std::vector<std::string>&, const std::string&);
    bool mkdir(const std::string& path, const std::string& options = "");
    bool mkdir(const char* path, const std::string& options = "");
    void systemCommand(const std::string& command);
    std::string baseName(std::string path, const std::string& delims = "/\\");
    std::string dirName(const std::string& path, const std::string& delims = "/\\");
    std::string parentDir(const std::string& path, char delim = '/');
    std::string removeExtension(const std::string&);
    std::string getExtension(const std::string&);
    std::istream& safeGetline(std::istream& is, std::string& t);
    std::istream& safeGetline(std::istream& is, std::string& t, std::streampos& oldPos);
        
    //string utils
    bool strContains(const std::string&, const std::string&);
    bool strContains(char, const std::string&);
    bool startsWith(const std::string& whithinStr, const std::string& findStr);
    bool endsWith(const std::string& whithinStr, const std::string& findStr);
    void split (const std::string&, const char, std::vector<std::string>&);
    std::string trimTraling(const std::string&);
    std::string trimLeading(const std::string&);
    std::string trim(const std::string&);
    std::string removeWhitespace(const std::string&);
    void trimAll(std::vector<std::string>&);
    bool isCommentLine(std::string);
    std::string removeSubstr(const std::string& findStr, std::string whithinStr);
    std::string removeSubstrs(const std::string& findStr, std::string whithinStr, bool checkEmpty = true);
    std::string removeChars(char findChar, std::string whithinStr);
    std::string toLower(std::string);
    std::string repeat(const std::string&, size_t);
    size_t offset(const char* buf, size_t len, const char* str);
    size_t offset(const char* buf, size_t len, std::string s);
    void removeEmptyStrings(std::vector<std::string>&);
    void getIdxOfSubstr(char*, const char*, std::vector<size_t>&);
    std::string toSubscript(int);
    void addChar(const std::string& toAdd, std::string& s, const std::string& delim = "|");
    void addChar(char toAdd, std::string& s, const std::string& delim = "|");
    
    //other
    bool isInteger(const std::string & s);
    bool isFlag(const char*);
    bool isArg(const char*);
    void printProgress(float progress, std::ostream& out, int barWidth = PROGRESS_BAR_WIDTH);
    void printProgress(float progress, int barWidth = PROGRESS_BAR_WIDTH);
    std::string ascTime();
    int readInt(int min, int max);
    bool almostEqual(float a, float b, float epsilon = FLT_EPSILON);
    bool almostEqual(double a, double b, double epsilon = DBL_EPSILON);
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
     \param delim how to separate elements
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

    /**
    \brief Remove duplicate values from vector.

    \param v Vector to remove duplicates from. 
    \param sort Should the values in \p v be sorted?
    
    Sorting is performed using the default comparator for \p _Tp. 
    If sort is set to \p false, elements are returned in a random order.
    */
    template<typename _Tp> void unique(std::vector<_Tp>& v, bool sort = true){
        std::set<_Tp> s(v.begin(), v.end());
        v.assign(s.begin(), s.end());
        if(sort) std::sort(v.begin(), v.end());
    }

    /**
    Compare std::string by length.
    If lengths are the same, then compare alphabetically.
    */
    struct strLenCompare {
        bool operator() (const std::string& lhs, const std::string& rhs) const{
            if(lhs.length() < rhs.length())
                return true;
            else if(lhs.length() == rhs.length())
                return lhs < rhs;
            else return false;
        }
    };
}

#endif /* utils_hpp */
