//
// utils.hpp
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

#include <msInterface/ms2File.hpp>

/**
 \brief Get index for scan in Ms2File::_offsetIndex. <br>
 
 If \p scan is not found, utils::SCAN_INDEX_NOT_FOUND is returned.
 
 \param scan Scan number to search for.
 \return Index for \p scan.
 */
size_t utils::Ms2File::_getScanIndex(size_t scan) const{
	auto it = _scanMap.find(scan);
	if(it == _scanMap.end())
		return SCAN_INDEX_NOT_FOUND;
	return it->second;
}

void utils::Ms2File::_buildIndex()
{
	std::vector<size_t> scanIndecies;
	utils::getIdxOfSubstr(_buffer, "S\t", scanIndecies);
	scanIndecies.push_back(_size);
	
	_scanCount = 0;
	int const scanLen = 20;
	std::string newID;
	size_t len = scanIndecies.size();
	for(size_t i = 0; i < len - 1; i++)
	{
		char* c = &_buffer[scanIndecies[i] + 2];
		newID = "";
		for(int j = 0; j < scanLen; j++)
		{
			newID += *c;
			c += 1;
			if(*c == '\t')
				break;
		}
		_scanMap[std::stoi(newID)] = _scanCount;
		_offsetIndex.push_back(IntPair(scanIndecies[i], scanIndecies.at(i + 1)));
		_scanCount ++;
	}
}

bool utils::Ms2File::getMetaData()
{
	//find header in buffer and put it into ss
	std::stringstream ss;
	std::string line;
	size_t end = utils::offset(_buffer, _size, "LastScan") + 100;
	for(size_t i = 0; i < end; i++)
		ss.put(*(_buffer + i));
	std::streampos sLen = std::streampos(ss.str().length());
	
	std::vector<std::string> elems;
	int mdCount = 0;
	
	//iterate through ss to find metadata
	while(ss.tellg() < sLen){
		utils::safeGetline(ss, line);
		if(line[0] == 'H')
		{
			utils::split(line, IN_DELIM, elems);
			if(elems[1] == "FirstScan")
				firstScan = std::stoi(elems[2]);
			else if(elems[1] == "LastScan")
				lastScan = std::stoi(elems[2]);
			else continue;
			mdCount++;
		}
	}
	
	//check that md is good
    return firstScan <= lastScan &&
           mdCount == MD_NUM;
}

/**
 \brief Overloaded function with \p queryScan as string
 */
bool utils::Ms2File::getScan(std::string queryScan, Scan& scan) const{
	return getScan(std::stoi(queryScan), scan);
}

/**
 \brief Get parsed utils::Spectrum from utils file.
 
 \param queryScan scan number to search for
 \param scan empty utils::Spectrum to load scan into
 \return false if \p queryScan not found, true if successful
 \throws utils::FileIOError if the format of the .ms2 file is invalid.
 */
bool utils::Ms2File::getScan(size_t queryScan, Scan& scan) const
{
	scan.clear();
	scan.getPrecursor().setSample(_parentFileBase);
	scan.getPrecursor().setFile(_fname);
	if(!((queryScan >= firstScan) && (queryScan <= lastScan))){
		std::cerr << "queryScan not in file scan range!" << NEW_LINE;
		return false;
	}
	
	size_t scanOffset, endOfScan;
	size_t scanIndex = _getScanIndex(queryScan);
	if(scanIndex == SCAN_INDEX_NOT_FOUND){
		std::cerr << "queryScan: " << queryScan << ", could not be found in: " << _fname << NEW_LINE;
		return false;
	}
	scanOffset = _offsetIndex[scanIndex].first;
	endOfScan = _offsetIndex[scanIndex].second;
	
	std::vector<std::string> elems;
	std::string line;
	size_t numIons = 0;
	
	std::string temp(_buffer + scanOffset,
					 _buffer + scanOffset + (endOfScan - scanOffset));
	std::stringstream ss(temp);
	std::streampos oldPos = ss.tellg();
	bool z_found = false;
	
	while(utils::safeGetline(ss, line, oldPos))
	{
		if(line.empty()) continue;
		utils::split(line, IN_DELIM, elems);
		
		if(elems[0] == "S")
		{
			if(elems.size() != 4)
			    throw utils::FileIOError();
			scan.setScanNum(std::stoi(elems[2]));
			scan.getPrecursor().setMZ(elems[3]);
		}
		else if(elems[0] == "I")
		{
			if(elems.size() != 3) throw utils::FileIOError();
			if(elems[1] == "RetTime")
				scan.getPrecursor().setRT(std::stod(elems[2]));
			else if(elems[1] == "PrecursorInt")
				scan.getPrecursor().setIntensity(std::stod(elems[2]));
			else if(elems[1] == "PrecursorFile")
                scan.getPrecursor().setFile(utils::removeExtension(elems[2]));
			else if(elems[1] == "PrecursorScan")
                scan.getPrecursor().setScan(elems[2]);
		}
		else if(elems[0] == "Z"){
			if(!z_found){
				if(elems.size() != 3) throw utils::FileIOError();
				scan.getPrecursor().setCharge(std::stoi(elems[1]));
				z_found = true;
			}
		}
		else if(utils::isInteger(std::string(1, elems[0][0])))
		{
			ss.seekg(oldPos);
			while(utils::safeGetline(ss, line))
			{
				if(line.empty()) continue;
				
				utils::split(line, ' ', elems);
				if(elems.size() < 2) throw utils::FileIOError();
				scan.getIons().emplace_back(std::stod(elems[0]), std::stod(elems[1]));
				numIons++;
			}//end of while
		}//end of else
	}//end of while
	scan.updateRanges();
	
	return true;
}

bool utils::Ms2File::read(std::string fname) {
    bool sucess = MsInterface::read(fname);
    return sucess && getMetaData();
}

bool utils::Ms2File::read() {
    bool sucess = MsInterface::read();
    return sucess && getMetaData();
}

