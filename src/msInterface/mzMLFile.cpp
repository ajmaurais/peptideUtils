//
// mzMLFile.cpp
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

#include <msInterface/mzMLFile.hpp>

void utils::MzMLFile::_buildIndex()
{
    //Check the file type
    if(fileType != FileType::MZML)
        throw utils::FileIOError("Incorrect file type for file: " + _fname);

    // Get indices of beginning and end of each scan
    std::vector<size_t> beginScans, endScans;
    utils::getIdxOfSubstr(_buffer, "<spectrum ", beginScans);
    utils::getIdxOfSubstr(_buffer, "</spectrum>", endScans);

    //validate spectrum indices
    if(beginScans.size() != endScans.size())
        throw utils::InvalidXmlFile("Unbounded <spectrum>");
    size_t len = beginScans.size();
    for(size_t i = 0; i < len; i++)
        if(beginScans[i] >= endScans[i])
            throw utils::InvalidXmlFile("Unbounded <spectrum>");

    _scanCount = 0;
    std::string newID;
    std::string idLine;
    for(size_t i = 0; i < len; i++)
    {
        // Get the attributes in the <scan> node
        char* c = _buffer + beginScans[i];
        char* num = strstr(c, "id=\"");
        char* endNode = strchr(c, '>');
        if(num >= endNode)
            throw utils::InvalidXmlFile("Not able to find required attribute \'num\' in <spectrum>");

        // Find the "num" attribute value
        newID = "";
        idLine = "";
        for(char* it = num + 4; it < endNode; it++) {
            if(*it == '\"')
                break;
           idLine += *it;
        }
        size_t scanStart = idLine.find("scan");
        idLine = utils::trim(idLine.substr(scanStart + 4));
        if(scanStart == std::string::npos || idLine.empty() || idLine[0] != '=')
            throw utils::InvalidXmlFile("Invalid spectrum header: " + idLine);
        for(size_t j = 1; j < idLine.size(); j++){
            if(isspace(idLine[j]))
                break;
            newID += idLine[j];
        }
        try {
            _scanMap[std::stoi(newID)] = _scanCount;
        } catch(std::invalid_argument& e){
            throw utils::InvalidXmlFile("Invalid spectrum ID: " + newID);
        }
        _offsetIndex.push_back(IntPair(beginScans[i], endScans[i]));
        _scanCount ++;
    }
    firstScan = _scanMap.begin()->first;
    lastScan = _scanMap.rbegin()->first;
    assert(firstScan <= lastScan);
}

/**
 \brief Get parsed utils::Spectrum from mzML file.

 \param queryScan scan number to search for
 \param scan empty utils::Spectrum to load scan into
 \return false if \p queryScan not found, true if successful
 */
bool utils::MzMLFile::getScan(size_t queryScan, utils::Scan& scan) const
{
    scan.clear();
    scan.getPrecursor().setSample(_parentFileBase);
    scan.getPrecursor().setFile(_fname);
    if(!((queryScan >= firstScan) && (queryScan <= lastScan))){
        std::cerr << "queryScan: " << queryScan << " not in file scan range!" << NEW_LINE;
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

    // Copy <scan> ... </scan> and initialize xml parser

    scan.updateRanges();
    return true;
}
