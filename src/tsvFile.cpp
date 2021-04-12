// 
// tsvFile.cpp
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

#include <tsvFile.hpp>

bool utils::TsvFile::read()
{
	if(_fname.empty()){
		std::cerr << "File name must be specified!" << std::endl;
		return false;
	}
	
	std::ifstream inF(_fname.c_str());
	if(!inF)
		return false;
	
	std::string line;
	std::vector<std::string> elems;
	if(_hasHeaders)
	{
		utils::safeGetline(inF, line);
		utils::split(line, _delim, elems);
		_nCol = elems.size();
		colMapType::iterator foundIt;
		for(size_t i = 0; i < _nCol; i++){
			foundIt = _colNames.find(elems[i]);
			if(foundIt == _colNames.end()){
				std::string temp = elems[i];
				if(!_caseSensitive)
					temp = utils::toLower(temp);
				_colNames[temp] = i;
			}
			else{
				throw std::runtime_error("Duplicate colnames found!");
			}
		}
	}
	else{
		std::streampos old = inF.tellg();
		utils::safeGetline(inF, line, old);
		inF.seekg(old);
		utils::split(line, _delim, elems);
		_nCol = elems.size();
		for(size_t i = 0; i < _nCol; i++){
			_colNames[("c" + std::to_string(i + 1))] = i;
		}
	}
	_dat.resize(_nCol);
	
	while(utils::safeGetline(inF, line))
	{
		line = utils::trim(line);
		if(line == "")
			continue;
		utils::split(line, _delim, elems);
		if(elems.size() > _nCol){
			std::cerr << "Not enough colnames!" << std::endl;
			return false;
		}
		
		for(size_t i = 0; i < _nCol; i++){
			if(i < elems.size())
				_dat[i].push_back(elems[i]);
			else _dat[i].push_back(_blank);
		}
		_nRow++;
	}
	
	return true;
}

bool utils::TsvFile::write(std::ostream& out, char delim)
{
	if(!out)
		return false;
	
	for(datType::iterator it1 = _dat.begin(); it1 != _dat.end(); ++it1)
	{
		for(rowType::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			if(it2 == it1->begin())
				out << *it2;
			else out << delim << *it2;
		}
		out << std::endl;
	}
	return true;
}

bool utils::TsvFile::write(std::string ofname, char delim){
	std::ofstream outF(ofname.c_str());
	return write(outF, delim);
}

