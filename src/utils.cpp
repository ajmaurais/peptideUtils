//
//  utils.cpp
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

#include <utils.hpp>

/*******************/
/*  file utilities */
/*******************/

/**
 \brief Read contents of \p fname into \p buffer
 \param fname path of file to read
 \param buffer location to store file contents
 \param size length of \p buffer after reading
 */
void utils::readBuffer(std::string fname, char** buffer, size_t& size)
{
	std::ifstream inF(fname.c_str());
	
	if(!inF)
		throw std::runtime_error("Could not open " + fname);
	
	inF.seekg(0, inF.end);
	size = inF.tellg();
	inF.seekg(0, inF.beg);
	*buffer = new char [size];
	
	if(!inF.read(*buffer, size))
		throw std::runtime_error("Could not read " + fname);
}

/**
 returns true if folder at end of path exists and false if it does not
 \param path path of file to test
 */
bool utils::dirExists (const char* path)
{
	struct stat buffer;
	return stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode);
}

//!returns true if file at end of \p path exists and false if it does not
bool utils::fileExists(const char* path)
{
	struct stat buffer;
	return stat(path, &buffer) == 0;
}

bool utils::fileExists(std::string path)
{
	return fileExists(path.c_str());
}

bool utils::dirExists(std::string path)
{
	return dirExists(path.c_str());
}

bool utils::isDir(std::string path)
{
	return utils::isDir(path.c_str());
}

//!checks whether file is directory
bool utils::isDir(const char* path)
{
	struct stat buffer;
	if(stat(path, &buffer) != 0)
		std::cerr << path << " does not exist!";
	return S_ISDIR(buffer.st_mode);
}

//!returns dirrectory from which program is run
std::string utils::pwd()
{
	char temp[PATH_MAX + 1];
	return (getcwd(temp, PATH_MAX) ? std::string(temp) : std::string(""));
}

//!resolves relative and symbolic file references
std::string utils::absPath(const char* _fname)
{
	char fbuff [PATH_MAX + 1];
	realpath(_fname, fbuff);
	return std::string(fbuff);
}

//!resolves relative and symbolic file references
std::string utils::absPath(std::string _fname)
{
	return(absPath(_fname.c_str()));
}

bool utils::ls(const char* path, std::vector<std::string>& files)
{
	files.clear();
	DIR* dirFile = opendir(path);
	if(!dirFile)
	return false;
	else{
		struct dirent* hFile;
		while((hFile = readdir(dirFile)))
		{
			//skip . and ..
			if(!strcmp(hFile->d_name, ".") || !strcmp(hFile->d_name, ".."))
			continue;
			
			//skip hidden files
			if(IGNORE_HIDDEN_FILES && (hFile->d_name[0] == '.'))
			continue;
			
			//add to files
			files.push_back(hFile->d_name);
		}
		closedir(dirFile);
	}
	return true;
}

bool utils::ls(const char* path, std::vector<std::string>& files, std::string extension)
{
	files.clear();
	std::vector<std::string> allFiles;
	if(!ls(path, allFiles))
		return false;
	
	for(std::vector<std::string>::iterator it = allFiles.begin(); it != allFiles.end(); ++it)
		if(endsWith(*it, extension))
			files.push_back(*it);
	return true;
}

//!executes \p command as system command
void utils::systemCommand(std::string command)
{
	system(command.c_str());
}

/**
 \brief make new dir.
 \param path path of new dir to create
 \return true if successful.
 */
bool utils::mkdir(const char* path)
{
	//get abs path and make sure that it doesn't already exist
	//return false if it does
	std::string rpath = absPath(path);
	if(dirExists(rpath))
	return false;
	
	//make sure that parent dir exists
	//return false if not
	size_t pos = rpath.find_last_of("/");
	assert(pos != std::string::npos);
	std::string parentDir = rpath.substr(0, pos);
	if(!dirExists(parentDir))
	return false;
	
	//make dir
	systemCommand("mkdir " + rpath);
	
	//test that new dir exists
	return dirExists(rpath);
}

std::string utils::baseName(std::string path, const std::string& delims)
{
	if(path[path.length() - 1] == '/')
		path = path.substr(0, path.length() - 1);
	return path.substr(path.find_last_of(delims) + 1);
}

std::string utils::dirName(std::string path, const std::string& delims)
{
	std::string ret = path.substr(0, path.find_last_of(delims));
	if(ret == path) return "./";
	return ret;
}

std::string utils::parentDir(std::string path, char delim)
{
	//split by delim
	std::vector<std::string> elems;
	utils::split(path, delim, elems);
	utils::removeEmptyStrings(elems);
	size_t len = elems.size();
	
	if(len == 0){
		throw std::runtime_error("Delim not found!");
	}
	else if(len == 1){
		return "/";
	}
	else{
		std::string ret = "";
		for(size_t i = 0; i < len - 1; i++)
			ret += "/" + elems[i];
		return ret;
	}
}

std::string utils::removeExtension(const std::string& filename)
{
	typename std::string::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
}

std::string utils::getExtension(const std::string& filename)
{
	typename std::string::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != std::string::npos ? filename.substr(p) : filename;
}

/**
 \brief Get next line from \p is and store it in \p t.
 
 std::getline only handels \\n. safeGetLine handels \\n \\r \\r\\n.
 \param is stream to read from
 \param s string to store next line in \p t is cleared prior to adding new string.
 \param oldPos stores state of stream before reading new line
 \return ref to \p is after reaing next line.
 */
std::istream& utils::safeGetline(std::istream& is, std::string& s, std::streampos& oldPos){
	oldPos = is.tellg();
	return utils::safeGetline(is, s);
}

/**
 \brief Get next line from \p is and store it in \p t.
 
 std::getline only handels \\n. safeGetLine handels \\n \\r \\r\\n.
 \param is stream to read from
 \param s string to store next line in \p t is cleared prior to adding new string.
 \return ref to \p is after reaing next line.
 */
std::istream& utils::safeGetline(std::istream& is, std::string& s)
{
	s.clear();
	
	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.
	
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();
	
	while(true){
		int c = sb->sbumpc();
		switch (c) {
			case '\n':
				return is;
			case '\r':
				if(sb->sgetc() == '\n')
					sb->sbumpc();
				return is;
				//case std::streambuf::traits_type::eof():
			case -1:
				// Also handle the case when the last line has no line ending
				if(s.empty())
					is.setstate(std::ios::eofbit);
				return is;
			default:
				s += (char)c;
		}
	}
}

/*****************/
/* std::string utils */
/*****************/

//!returns true if \p findTxt is found in \p whithinTxt and false if it it not
bool utils::strContains(std::string findTxt, std::string whithinTxt)
{
	return whithinTxt.find(findTxt) != std::string::npos;
}

//!overloaded version of strContains, handels \p findTxt as char
bool utils::strContains(char findTxt, std::string whithinTxt)
{
	return strContains(std::string(1, findTxt), whithinTxt);
}

bool utils::startsWith(std::string whithinStr, std::string findStr)
{
	return (whithinStr.find(findStr) == 0);
}

bool utils::endsWith(std::string whithinStr, std::string findStr)
{
	size_t pos = whithinStr.rfind(findStr);
	if(pos == std::string::npos)
	return false;
	
	return (whithinStr.substr(pos) == findStr);
}

//!split \p str by \p delim and populate each split into \p elems
void utils::split (const std::string& str, const char delim, std::vector<std::string>& elems)
{
	elems.clear();
	elems.shrink_to_fit();
	std::stringstream ss (str);
	std::string item;
	
	while(getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

/**
 remove trailing WHITESPACE
 \param str string to trim
 \return trimmed string
*/
std::string utils::trimTraling(const std::string& str)
{
	if(str.empty())
	return "";
	return str.substr(0, str.find_last_not_of(WHITESPACE) + 1);
}

/**
 remove leading utils::WHITESPACE
 \param str string to trim
 \return trimmed string
 */
std::string utils::trimLeading(const std::string& str)
{
	if(str.empty())
	return "";
	return str.substr(str.find_first_not_of(WHITESPACE));
}

/**
 Remove trailing and leading utils::WHITESPACE
 \param str string to trim
 \return trimmed string
 */
std::string utils::trim(const std::string& str)
{
	if(str.empty())
	return "";
	return trimLeading(trimTraling(str));
}

void utils::trimAll(std::vector<std::string>& elems)
{
	for(std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); ++it)
		(*it) = trim((*it));
}

//!returns true if line begins with utils::COMMENT_SYMBOL, ignoring leading whitespace
bool utils::isCommentLine(std::string line)
{
	line = trimLeading(line);
	return line.substr(0, COMMENT_SYMBOL.length()) == COMMENT_SYMBOL;
}

//!removes \p findStr from \p whithinStr and returns \p whithinStr
std::string utils::removeSubstr(std::string findStr, std::string whithinStr)
{
	std::string::size_type i = whithinStr.find(findStr);
	
	if(i !=std::string::npos)
	whithinStr.erase(i, findStr.length());
	
	return whithinStr;
}

std::string utils::removeChars(char findChar, std::string wStr)
{
	wStr.erase(remove(wStr.begin(), wStr.end(), findChar), wStr.end());
	return wStr;
}

std::string utils::toLower(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string utils::repeat(std::string str, size_t numTimes)
{
	std::string ret = "";
	assert(!str.empty());
	for(int i = 0; i < numTimes; i++)
	ret += str;
	return ret;
}

std::string utils::toSubscript(int _num)
{
	std::string strNum = std::to_string(_num);
	std::string ret = "";
	
	size_t len = strNum.length();
	for(size_t i = 0; i < len; i++)
	{
		int tempInt = (int)strNum[i] - 48; //convert char to int
		assert(tempInt >= 0 && tempInt <= 9); //check that tempInt will not overrun buffer
		ret += SUBSCRIPT_MAP[tempInt];
	}
	return ret;
}

//!Find \p str in buffer \p buf of length \p len
size_t utils::offset(const char* buf, size_t len, std::string s)
{
	const char* str = s.c_str();
	return std::search(buf, buf + len, str, str + strlen(str)) - buf;
}

//!Find \p str in buffer \p buf of length \p len
size_t utils::offset(const char* buf, size_t len, const char* str)
{
	return std::search(buf, buf + len, str, str + strlen(str)) - buf;
}

void utils::removeEmptyStrings(std::vector<std::string>& elems)
{
	for(auto it = elems.begin(); it != elems.end();)
	{
		if(*it == "")
			elems.erase(it);
		else ++it;
	}
}

/*********/
/* other */
/*********/

bool utils::isFlag(const char* tok)
{
	if(tok == nullptr)
	return false;
	else return tok[0] == '-';
}

bool utils::isArg(const char* tok)
{
	if(tok == nullptr)
	return false;
	else return !isFlag(tok);
}

std::string utils::ascTime()
{
	//get current time
	const char* curTime;
	time_t rawTime;
	struct tm * timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	curTime = asctime(timeInfo);
	return(std::string(curTime));
}

bool utils::isInteger(const std::string & s)
{
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
	
	char * p ;
	strtol(s.c_str(), &p, 10) ;
	
	return (*p == 0) ;
}

/**
 \brief Read in from std::cin between min and max.
 
 Continue asking for input until user suplies valid value.
 \param min minimum valid value
 \param max maximum valid value
 \return int between min and max
 */
int utils::getInt(int min, int max)
{
	std::string choice;
	int ret = min - 1;
	bool good;
	do{
		choice.clear();
		std::cin.sync();
		std::getline(std::cin, choice);
		choice = utils::trim(choice);
		if(utils::isInteger(choice))
		{
			ret = std::stoi(choice);
			if(ret >= min && ret <= max){
				good = true;
			}
			else{
				good = false;
				std::cout << "Invalid choice!" << NEW_LINE << "Enter choice: ";
			}
		}
		else{
			good = false;
			std::cout << "Invalid choice!" << NEW_LINE << "Enter choice: ";
		}
	} while(!good);
	return ret;
}//end of fxn

/**
 Prints progress bar to std::out
 \pre \p progress <= 1
 \param progress as a fraction of 1
 */
void utils::printProgress(float progress, int barWidth){
	utils::printProgress(progress, std::cout, barWidth);
}

/**
 Prints progress bar to \p out
 \pre \p progress <= 1
 \param progress as a fraction of 1
 \param out stream to print to
 */
void utils::printProgress(float progress, std::ostream& out, int barWidth)
{
	assert(progress <= 1);
	std::cout << "[";
	int pos = barWidth * progress;
	for(int i = 0; i < barWidth; ++i) {
		if (i < pos) out << "=";
		else if (i == pos) out << ">";
		else out << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	out.flush();
}

/**
 \brief Find indecies of all instances of substrings in string.
 
 \param searchStr Buffer to search.
 \param findStr String to find.
 \param indecies set contain all indecies
 */
void utils::getIdxOfSubstr(char* searchStr, const char* findStr,
						   std::vector<size_t>& indecies)
{
	char* tmp = searchStr;
	size_t incrementAmt = strlen(findStr);
	while((tmp = strstr(tmp, findStr)) != nullptr){
		indecies.push_back((size_t)(tmp - searchStr));
		tmp += incrementAmt;
	}
}

